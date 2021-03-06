
#include "stdafx.h"

#include "DiskHeaderTools.h"
#include <Random/Random.h>

using namespace std;
//using namespace boost;
using namespace CryptoLib;

ostream& operator<<(ostream& out, const DISK_CIPHER cipher)
{
	switch (cipher)
	{
	case DISK_CIPHER::DISK_CIPHER_AES:
		out << "DISK_CIPHER_AES";
		break;
	case DISK_CIPHER::DISK_CIPHER_TWOFISH:
		out << "DISK_CIPHER_TWOFISH";
		break;
	case DISK_CIPHER::MAX_DISK_CIPHER:
		out << "MAX_DISK_CIPHER";
		break;
	default:
		out << "Invalid cipher (" << static_cast<unsigned int>(cipher) << ")";
		throw std::invalid_argument("Invalid disk cipher");
	}

	return out;
}

ostream& operator<<(ostream& out, const DISK_VERSION version)
{
	switch (version)
	{
	case DISK_VERSION::DISK_VERSION_3:
		out << "DISK_VERSION_3";
		break;
	case DISK_VERSION::DISK_VERSION_4:
		out << "DISK_VERSION_4";
		break;
	default:
		out << "Invalid version (" << static_cast<unsigned int>(version) << ")";
		throw std::invalid_argument("Invalid disk version");
	}

	return out;
}

bool operator==(const UCHAR ch, const DISK_VERSION v)
{
	return ch == static_cast<UCHAR>(v);
}

shared_ptr<IRandomGenerator> CreateRandomGen()
{
	shared_ptr<RandomGeneratorBase> result(new RandomGeneratorBase());

	vector<char> sample(512, 0);

	result->AddSample(&sample[0], static_cast<ULONG>(sample.size()), static_cast<ULONG>(sample.size()*8));

	return result;
}

BOOST_AUTO_TEST_CASE( testDiskHeaderV3_AES )
{
	BOOST_TEST_MESSAGE("Disk header V3 AES test");

	shared_ptr<IRandomGenerator> rndGen = CreateRandomGen();

	vector<unsigned char> headerBuff(sizeof(DISK_HEADER_V3), 0);

	BOOST_CHECK(DiskHeaderTools::Initialize(reinterpret_cast<DISK_HEADER_V3*>(&headerBuff[0]), rndGen.get()));

	vector<unsigned char> headerBuffEnc(headerBuff);

	DiskHeaderTools::Encipher(reinterpret_cast<DISK_HEADER_V3*>(&headerBuffEnc[0]), (const UCHAR*)"test", 4, DISK_CIPHER::DISK_CIPHER_AES);

	BOOST_CHECK(!std::equal(begin(headerBuff), end(headerBuff), begin(headerBuffEnc)));

	vector<unsigned char> headerBuffKrnl(headerBuffEnc);
	DiskHeaderTools::CIPHER_INFO cipherInfo;

	BOOST_CHECK(DiskHeaderTools::Decipher(&headerBuffEnc[0], (const UCHAR*)"test", 4, &cipherInfo));

	BOOST_CHECK_EQUAL(cipherInfo.diskCipher, DISK_CIPHER::DISK_CIPHER_AES);
	BOOST_CHECK_EQUAL(cipherInfo.versionInfo.formatVersion, DISK_VERSION::DISK_VERSION_3);

	BOOST_CHECK(std::equal(begin(headerBuff), end(headerBuff), begin(headerBuffEnc)));

	BOOST_CHECK(DiskHeaderTools::Decipher(reinterpret_cast<DISK_HEADER_V3*>(&headerBuffKrnl[0]), cipherInfo.userKey, cipherInfo.diskCipher));
}

BOOST_AUTO_TEST_CASE( testDiskHeaderV3_Twofish )
{
	BOOST_TEST_MESSAGE("Disk header V3 AES test");

	shared_ptr<IRandomGenerator> rndGen = CreateRandomGen();

	vector<unsigned char> headerBuff(sizeof(DISK_HEADER_V3), 0);

	BOOST_CHECK(DiskHeaderTools::Initialize(reinterpret_cast<DISK_HEADER_V3*>(&headerBuff[0]), rndGen.get()));

	vector<unsigned char> headerBuffEnc(headerBuff);

	DiskHeaderTools::Encipher(reinterpret_cast<DISK_HEADER_V3*>(&headerBuffEnc[0]), (const UCHAR*)"test", 4, DISK_CIPHER::DISK_CIPHER_TWOFISH);

	BOOST_CHECK(!std::equal(begin(headerBuff), end(headerBuff), begin(headerBuffEnc)));

	vector<unsigned char> headerBuffKrnl(headerBuffEnc);
	DiskHeaderTools::CIPHER_INFO cipherInfo;

	BOOST_CHECK(DiskHeaderTools::Decipher(&headerBuffEnc[0], (const UCHAR*)"test", 4, &cipherInfo));

	BOOST_CHECK_EQUAL(cipherInfo.diskCipher, DISK_CIPHER::DISK_CIPHER_TWOFISH);
	BOOST_CHECK_EQUAL(cipherInfo.versionInfo.formatVersion, DISK_VERSION::DISK_VERSION_3);

	BOOST_CHECK(std::equal(begin(headerBuff), end(headerBuff), begin(headerBuffEnc)));

	BOOST_CHECK(DiskHeaderTools::Decipher(reinterpret_cast<DISK_HEADER_V3*>(&headerBuffKrnl[0]), cipherInfo.userKey, cipherInfo.diskCipher));
}

BOOST_AUTO_TEST_CASE( testDiskHeaderV4_AES )
{
	BOOST_TEST_MESSAGE("Disk header V4 AES test");

	shared_ptr<IRandomGenerator> rndGen = CreateRandomGen();
	vector<unsigned char> headerBuff(sizeof(DISK_HEADER_V4), 0);

	// Initialize header
	BOOST_CHECK(DiskHeaderTools::Initialize(reinterpret_cast<DISK_HEADER_V4*>(&headerBuff[0]), rndGen.get()));

	// Encipher header
	vector<unsigned char> headerBuffEnc(headerBuff);

	DiskHeaderTools::Encipher(reinterpret_cast<DISK_HEADER_V4*>(&headerBuffEnc[0]), (const UCHAR*)"test", 4, DISK_CIPHER::DISK_CIPHER_AES);

	BOOST_CHECK(!std::equal(begin(headerBuff), end(headerBuff), begin(headerBuffEnc)));

	// Decipher header
	vector<unsigned char> headerBuffKrnl(headerBuffEnc);
	DiskHeaderTools::CIPHER_INFO cipherInfo;

	BOOST_CHECK(DiskHeaderTools::Decipher(&headerBuffEnc[0], (const UCHAR*)"test", 4, &cipherInfo));

	BOOST_CHECK_EQUAL(cipherInfo.diskCipher, DISK_CIPHER::DISK_CIPHER_AES);
	BOOST_CHECK_EQUAL(cipherInfo.versionInfo.formatVersion, DISK_VERSION::DISK_VERSION_4);

	BOOST_CHECK(std::equal(begin(headerBuff), end(headerBuff), begin(headerBuffEnc)));

	// Decipher header using kernel-mode functions
	BOOST_CHECK(DiskHeaderTools::Decipher(reinterpret_cast<DISK_HEADER_V4*>(&headerBuffKrnl[0]), cipherInfo.userKey, cipherInfo.initVector, cipherInfo.diskCipher));
}
