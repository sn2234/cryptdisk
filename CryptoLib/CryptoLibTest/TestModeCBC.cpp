
#include "stdafx.h"

using namespace CryptoLib;
//using namespace boost;
using namespace std;
using namespace stdext;

struct AES_CBC_TEST{
	unsigned char key[RijndaelEngine::KeySize];
	unsigned char iv[RijndaelEngine::BlockSize];
	size_t blocksCount;
	const unsigned char* plaintext;
	const unsigned char* ciphertext;
};

static const unsigned char plainTextAes1[] =
{
	0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
		0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
		0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
		0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10
};

static const unsigned char cipherTextAes1[] =
{
	0xf5, 0x8c, 0x4c, 0x04, 0xd6, 0xe5, 0xf1, 0xba, 0x77, 0x9e, 0xab, 0xfb, 0x5f, 0x7b, 0xfb, 0xd6,
	0x9c, 0xfc, 0x4e, 0x96, 0x7e, 0xdb, 0x80, 0x8d, 0x67, 0x9f, 0x77, 0x7b, 0xc6, 0x70, 0x2c, 0x7d,
	0x39, 0xf2, 0x33, 0x69, 0xa9, 0xd9, 0xba, 0xcf, 0xa5, 0x30, 0xe2, 0x63, 0x04, 0x23, 0x14, 0x61,
	0xb2, 0xeb, 0x05, 0xe2, 0xc3, 0x9b, 0xe9, 0xfc, 0xda, 0x6c, 0x19, 0x07, 0x8c, 0x6a, 0x9d, 0x1b
};

static const AES_CBC_TEST aesCbcTestVectors[] =
{
	{
		{
			0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
			0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
		},
		{
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
		},
		sizeof(plainTextAes1)/RijndaelEngine::BlockSize,
		plainTextAes1,
		cipherTextAes1
	}
};

void DoAesCbcDecTest(const AES_CBC_TEST& test)
{
	vector<unsigned char> plainText(test.blocksCount*RijndaelEngine::BlockSize, 0);

	RijndaelEngine engine;

	engine.SetupKey(test.key);
	DecipherDataCbc(engine, test.blocksCount, test.iv, test.ciphertext, &plainText[0]);

	BOOST_CHECK(std::equal(begin(plainText), end(plainText),
		checked_array_iterator<const unsigned char*>(test.plaintext, test.blocksCount*RijndaelEngine::BlockSize)));
}

void DoAesCbcDecTestInPlace(const AES_CBC_TEST& test)
{
	vector<unsigned char> plainText(test.ciphertext, test.ciphertext + test.blocksCount*RijndaelEngine::BlockSize);

	RijndaelEngine engine;

	engine.SetupKey(test.key);
	DecipherDataCbc(engine, test.blocksCount, test.iv, &plainText[0]);

	BOOST_CHECK(std::equal(begin(plainText), end(plainText),
		checked_array_iterator<const unsigned char*>(test.plaintext, test.blocksCount*RijndaelEngine::BlockSize)));
}

BOOST_AUTO_TEST_CASE( aesCbcDecTest )
{
	BOOST_TEST_MESSAGE("AES CBC Decryption test");

	for_each(begin(aesCbcTestVectors), end(aesCbcTestVectors), DoAesCbcDecTest);
	for_each(begin(aesCbcTestVectors), end(aesCbcTestVectors), DoAesCbcDecTestInPlace);
}

void DoAesCbcEncTest(const AES_CBC_TEST& test)
{
	vector<unsigned char> cipherText(test.blocksCount*RijndaelEngine::BlockSize, 0);

	RijndaelEngine engine;

	engine.SetupKey(test.key);
	EncipherDataCbc(engine, test.blocksCount, test.iv, test.plaintext, &cipherText[0]);

	BOOST_CHECK(std::equal(begin(cipherText), end(cipherText),
		checked_array_iterator<const unsigned char*>(test.ciphertext, test.blocksCount*RijndaelEngine::BlockSize)));
}

void DoAesCbcEncTestInPlace(const AES_CBC_TEST& test)
{
	vector<unsigned char> cipherText(test.plaintext, test.plaintext + test.blocksCount*RijndaelEngine::BlockSize);

	RijndaelEngine engine;

	engine.SetupKey(test.key);
	EncipherDataCbc(engine, test.blocksCount, test.iv, &cipherText[0]);

	BOOST_CHECK(std::equal(begin(cipherText), end(cipherText),
		checked_array_iterator<const unsigned char*>(test.ciphertext, test.blocksCount*RijndaelEngine::BlockSize)));
}

BOOST_AUTO_TEST_CASE( aesCbcEncTest )
{
	BOOST_TEST_MESSAGE("AES CBC Encryption test");

	for_each(begin(aesCbcTestVectors), end(aesCbcTestVectors), DoAesCbcEncTest);
	for_each(begin(aesCbcTestVectors), end(aesCbcTestVectors), DoAesCbcEncTestInPlace);
}
