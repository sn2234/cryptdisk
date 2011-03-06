
#include "stdafx.h"

using namespace CryptoLib;
//using namespace boost;
using namespace std;
using namespace stdext;

struct AES_ECB_TEST{
	unsigned char key[RijndaelEngine::KeySize];
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
	0xf3, 0xee, 0xd1, 0xbd, 0xb5, 0xd2, 0xa0, 0x3c, 0x06, 0x4b, 0x5a, 0x7e, 0x3d, 0xb1, 0x81, 0xf8,
	0x59, 0x1c, 0xcb, 0x10, 0xd4, 0x10, 0xed, 0x26, 0xdc, 0x5b, 0xa7, 0x4a, 0x31, 0x36, 0x28, 0x70,
	0xb6, 0xed, 0x21, 0xb9, 0x9c, 0xa6, 0xf4, 0xf9, 0xf1, 0x53, 0xe7, 0xb1, 0xbe, 0xaf, 0xed, 0x1d,
	0x23, 0x30, 0x4b, 0x7a, 0x39, 0xf9, 0xf3, 0xff, 0x06, 0x7d, 0x8d, 0x8f, 0x9e, 0x24, 0xec, 0xc7
};

static const AES_ECB_TEST aesEcbTestVectors[] = 
{
	{
		{
			0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
			0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
		},
		sizeof(plainTextAes1)/RijndaelEngine::BlockSize,
		plainTextAes1,
		cipherTextAes1
	}
};

void DoAesEcbDecTest(const AES_ECB_TEST& test)
{
	vector<unsigned char> plainText(test.blocksCount*RijndaelEngine::BlockSize, 0);

	RijndaelEngine engine;

	engine.SetupKey(test.key);
	DecipherDataEcb(engine, test.blocksCount, test.ciphertext, &plainText[0]);

	BOOST_CHECK(std::equal(begin(plainText), end(plainText),
		checked_array_iterator<const unsigned char*>(test.plaintext, test.blocksCount*RijndaelEngine::BlockSize)));
}

void DoAesEcbDecTestInPlace(const AES_ECB_TEST& test)
{
	vector<unsigned char> plainText(test.ciphertext, test.ciphertext + test.blocksCount*RijndaelEngine::BlockSize);

	RijndaelEngine engine;

	engine.SetupKey(test.key);
	DecipherDataEcb(engine, test.blocksCount, &plainText[0]);

	BOOST_CHECK(std::equal(begin(plainText), end(plainText),
		checked_array_iterator<const unsigned char*>(test.plaintext, test.blocksCount*RijndaelEngine::BlockSize)));
}

BOOST_AUTO_TEST_CASE( aesEcbDecTest )
{
	BOOST_MESSAGE("AES ECB decryption test");

	for_each(begin(aesEcbTestVectors), end(aesEcbTestVectors), DoAesEcbDecTest);
	for_each(begin(aesEcbTestVectors), end(aesEcbTestVectors), DoAesEcbDecTestInPlace);
}

void DoAesEcbEncTest(const AES_ECB_TEST& test)
{
	vector<unsigned char> cipherText(test.blocksCount*RijndaelEngine::BlockSize, 0);

	RijndaelEngine engine;

	engine.SetupKey(test.key);
	EncipherDataEcb(engine, test.blocksCount, test.plaintext, &cipherText[0]);

	BOOST_CHECK(std::equal(begin(cipherText), end(cipherText),
		checked_array_iterator<const unsigned char*>(test.ciphertext, test.blocksCount*RijndaelEngine::BlockSize)));
}

void DoAesEcbEncTestInPlace(const AES_ECB_TEST& test)
{
	vector<unsigned char> cipherText(test.plaintext, test.plaintext + test.blocksCount*RijndaelEngine::BlockSize);

	RijndaelEngine engine;

	engine.SetupKey(test.key);
	EncipherDataEcb(engine, test.blocksCount, &cipherText[0]);

	BOOST_CHECK(std::equal(begin(cipherText), end(cipherText),
		checked_array_iterator<const unsigned char*>(test.ciphertext, test.blocksCount*RijndaelEngine::BlockSize)));
}


BOOST_AUTO_TEST_CASE( aesEcbEncTest )
{
	BOOST_MESSAGE("AES ECB encryption test");

	for_each(begin(aesEcbTestVectors), end(aesEcbTestVectors), DoAesEcbEncTest);
	for_each(begin(aesEcbTestVectors), end(aesEcbTestVectors), DoAesEcbEncTestInPlace);
}
