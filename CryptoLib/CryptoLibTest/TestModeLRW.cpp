
#include "stdafx.h"

using namespace CryptoLib;
using namespace boost;
using namespace std;
using namespace stdext;


struct LrwTest{
	unsigned char key1[32];
	unsigned char key2[16];
	unsigned char index[16];
	unsigned char plaintext[16];
	unsigned char ciphertext[16];
};


static const LrwTest aesLrwTestVectors[] = {
	{
		{
			0xf8, 0xd4, 0x76, 0xff, 0xd6, 0x46, 0xee, 0x6c, 0x23, 0x84, 0xcb, 0x1c, 0x77, 0xd6, 0x19, 0x5d,
			0xfe, 0xf1, 0xa9, 0xf3, 0x7b, 0xbc, 0x8d, 0x21, 0xa7, 0x9c, 0x21, 0xf8, 0xcb, 0x90, 0x02, 0x89
		},
		{ 0xa8, 0x45, 0x34, 0x8e, 0xc8, 0xc5, 0xb5, 0xf1, 0x26, 0xf5, 0x0e, 0x76, 0xfe, 0xfd, 0x1b, 0x1e },
		{ 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46 },
		{ 0xbd, 0x06, 0xb8, 0xe1, 0xdb, 0x98, 0x89, 0x9e, 0xc4, 0x98, 0xe4, 0x91, 0xcf, 0x1c, 0x70, 0x2b }
	}
// 	,
// 	{
// 		{
// 			0xfb, 0x76, 0x15, 0xb2, 0x3d, 0x80, 0x89, 0x1d, 0xd4, 0x70, 0x98, 0x0b, 0xc7, 0x95, 0x84, 0xc8,
// 			0xb2, 0xfb, 0x64, 0xce, 0x60, 0x97, 0x87, 0x8d, 0x17, 0xfc, 0xe4, 0x5a, 0x49, 0xe8, 0x30, 0xb7
// 		},
// 		{ 0x0c ,0x9e ,0x2f ,0xf1 ,0x7a ,0x04 ,0x64 ,0x60 ,0xd4 ,0x12 ,0x5e ,0x2d ,0xe7 ,0x17 ,0x78 ,0x6e },
// 		{ 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
// 		{ 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46 },
// 		{ 0x5b, 0x90, 0x8e, 0xc1, 0xab, 0xdd, 0x67, 0x5f, 0x3d, 0x69, 0x8a, 0x95, 0x53, 0xc8, 0x9c, 0xe5 }
// 	}
};

void DoAesLrwEncTest(const LrwTest& testData)
{
	LRWAES cipher;

	cipher.SetupKey(testData.key1, testData.key2);

	unsigned char tmp[sizeof(testData.ciphertext)];
	std::fill_n(tmp, _countof(tmp), 0);

	cipher.EncipherFirstBlock(testData.index, testData.plaintext, tmp);

	BOOST_CHECK(equal(begin(testData.ciphertext), end(testData.ciphertext), tmp));
}

void DoAesLrwEncTestInPlace(const LrwTest& testData)
{
	LRWAES cipher;

	cipher.SetupKey(testData.key1, testData.key2);

	unsigned char tmp[sizeof(testData.ciphertext)];
	std::copy(testData.plaintext, testData.plaintext+_countof(testData.plaintext), tmp);

	cipher.EncipherFirstBlock(testData.index, tmp);

	BOOST_CHECK(equal(begin(testData.ciphertext), end(testData.ciphertext), tmp));
}

BOOST_AUTO_TEST_CASE( aesLrwEncTest )
{
	BOOST_TEST_MESSAGE("LRW-AES-256 encryption test");

	for_each(begin(aesLrwTestVectors), end(aesLrwTestVectors), DoAesLrwEncTest);
	for_each(begin(aesLrwTestVectors), end(aesLrwTestVectors), DoAesLrwEncTestInPlace);
}
