
#include "stdafx.h"

#include "DiskHeaderTools.h"
#include <Random/Random.h>

using namespace std;
using namespace boost;
using namespace CryptoLib;

shared_ptr<CRandom> CreateRandomGen()
{
	shared_ptr<CRandom> result(new CRandom());

	vector<char> sample(512, 0);

	result->AddSample(&sample[0], sample.size(), sample.size()*8);

	return result;
}

BOOST_AUTO_TEST_CASE( testDiskHeaderV3_AES )
{
	BOOST_MESSAGE("Disk header V3 AES test");

	shared_ptr<CRandom> rndGen = CreateRandomGen();

	vector<unsigned char> headerBuff(sizeof(DISK_HEADER_V3), 0);

	BOOST_CHECK(DiskHeaderTools::Initialize(reinterpret_cast<DISK_HEADER_V3*>(&headerBuff[0]), rndGen.get()));

	vector<unsigned char> headerBuffEnc(headerBuff);

	DiskHeaderTools::Encipher(reinterpret_cast<DISK_HEADER_V3*>(&headerBuffEnc[0]), (const UCHAR*)"test", 4, DISK_CIPHER_AES);

	BOOST_CHECK(!std::equal(begin(headerBuff), end(headerBuff), begin(headerBuffEnc)));

	DiskHeaderTools::CIPHER_INFO cipherInfo;

	BOOST_CHECK(DiskHeaderTools::Decipher(&headerBuffEnc[0], (const UCHAR*)"test", 4, &cipherInfo));

	BOOST_CHECK_EQUAL(cipherInfo.diskCipher, DISK_CIPHER_AES);
	BOOST_CHECK_EQUAL(cipherInfo.versionInfo.formatVersion, DISK_VERSION_3);

	BOOST_CHECK(std::equal(begin(headerBuff), end(headerBuff), begin(headerBuffEnc)));
}

BOOST_AUTO_TEST_CASE( testDiskHeaderV3_Twofish )
{
	BOOST_MESSAGE("Disk header V3 AES test");

	shared_ptr<CRandom> rndGen = CreateRandomGen();

	vector<unsigned char> headerBuff(sizeof(DISK_HEADER_V3), 0);

	BOOST_CHECK(DiskHeaderTools::Initialize(reinterpret_cast<DISK_HEADER_V3*>(&headerBuff[0]), rndGen.get()));

	vector<unsigned char> headerBuffEnc(headerBuff);

	DiskHeaderTools::Encipher(reinterpret_cast<DISK_HEADER_V3*>(&headerBuffEnc[0]), (const UCHAR*)"test", 4, DISK_CIPHER_TWOFISH);

	BOOST_CHECK(!std::equal(begin(headerBuff), end(headerBuff), begin(headerBuffEnc)));

	DiskHeaderTools::CIPHER_INFO cipherInfo;

	BOOST_CHECK(DiskHeaderTools::Decipher(&headerBuffEnc[0], (const UCHAR*)"test", 4, &cipherInfo));

	BOOST_CHECK_EQUAL(cipherInfo.diskCipher, DISK_CIPHER_TWOFISH);
	BOOST_CHECK_EQUAL(cipherInfo.versionInfo.formatVersion, DISK_VERSION_3);

	BOOST_CHECK(std::equal(begin(headerBuff), end(headerBuff), begin(headerBuffEnc)));
}

BOOST_AUTO_TEST_CASE( testDiskHeaderV4_AES )
{
	BOOST_MESSAGE("Disk header V4 AES test");

	shared_ptr<CRandom> rndGen = CreateRandomGen();

	vector<unsigned char> headerBuff(sizeof(DISK_HEADER_V4), 0);

	BOOST_CHECK(DiskHeaderTools::Initialize(reinterpret_cast<DISK_HEADER_V4*>(&headerBuff[0]), rndGen.get()));

	vector<unsigned char> headerBuffEnc(headerBuff);

	DiskHeaderTools::Encipher(reinterpret_cast<DISK_HEADER_V4*>(&headerBuffEnc[0]), (const UCHAR*)"test", 4, DISK_CIPHER_AES);

	BOOST_CHECK(!std::equal(begin(headerBuff), end(headerBuff), begin(headerBuffEnc)));

	DiskHeaderTools::CIPHER_INFO cipherInfo;

	BOOST_CHECK(DiskHeaderTools::Decipher(&headerBuffEnc[0], (const UCHAR*)"test", 4, &cipherInfo));

	BOOST_CHECK_EQUAL(cipherInfo.diskCipher, DISK_CIPHER_AES);
	BOOST_CHECK_EQUAL(cipherInfo.versionInfo.formatVersion, DISK_VERSION_4);

	BOOST_CHECK(std::equal(begin(headerBuff), end(headerBuff), begin(headerBuffEnc)));
}
