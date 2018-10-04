
#include "stdafx.h"

using namespace CryptoLib;
//using namespace boost;
using namespace std;
using namespace stdext;

struct AES_TEST{
	unsigned char key[RijndaelEngine::KeySize];
	unsigned char plaintext[RijndaelEngine::BlockSize];
	unsigned char ciphertext[RijndaelEngine::BlockSize];
};

static const AES_TEST AesTests[]=
{
	{
		{
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
			0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 
			0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
		},
		{
			0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 
			0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
		},
		{
			0x8E, 0xA2, 0xB7, 0xCA, 0x51, 0x67, 0x45, 0xBF, 
			0xEA, 0xFC, 0x49, 0x90, 0x4B, 0x49, 0x60, 0x89
		}
	},
	{
		{0xc4, 0x7b, 0x02, 0x94, 0xdb, 0xbb, 0xee, 0x0f, 0xec, 0x47, 0x57, 0xf2, 0x2f, 0xfe, 0xee, 0x35, 0x87, 0xca, 0x47, 0x30, 0xc3, 0xd3, 0x3b, 0x69, 0x1d, 0xf3, 0x8b, 0xab, 0x07, 0x6b, 0xc5, 0x58},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x46, 0xf2, 0xfb, 0x34, 0x2d, 0x6f, 0x0a, 0xb4, 0x77, 0x47, 0x6f, 0xc5, 0x01, 0x24, 0x2c, 0x5f}
	},
	{
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x07, 0xe2, 0x0b, 0x82, 0x98, 0xec, 0x35, 0x4f, 0x0f, 0x5f, 0xe7, 0x47, 0x0f, 0x36, 0xbd}
	}
};

void AesEncTest(const AES_TEST& testData)
{
	// Copy version of encipher
	{
		RijndaelEngine	cipher;
		cipher.SetupKey(testData.key);

		unsigned char tmp[RijndaelEngine::BlockSize];

		std::fill_n(tmp, _countof(tmp), 0);

		cipher.EncipherBlock(testData.plaintext, tmp);

		BOOST_CHECK_EQUAL_COLLECTIONS(begin(testData.ciphertext),
			end(testData.ciphertext),
			begin(tmp),
			end(tmp));
	}

	// In-place version of encipher
	{
		RijndaelEngine	cipher;
		cipher.SetupKey(testData.key);

		unsigned char tmp[RijndaelEngine::BlockSize];

		copy(begin(testData.plaintext), end(testData.plaintext), tmp);


		cipher.EncipherBlock(tmp);

		BOOST_CHECK_EQUAL_COLLECTIONS(begin(testData.ciphertext),
			end(testData.ciphertext),
			begin(tmp),
			end(tmp));
	}
}

void AesDecTest(const AES_TEST& testData)
{
	// Copy version of decipher
	{
		RijndaelEngine	cipher;
		cipher.SetupKey(testData.key);

		unsigned char tmp[RijndaelEngine::BlockSize];

		std::fill_n(tmp, _countof(tmp), 0);

		cipher.DecipherBlock(testData.ciphertext, tmp);

		BOOST_CHECK_EQUAL_COLLECTIONS(begin(testData.plaintext),
			end(testData.plaintext),
			begin(tmp),
			end(tmp));
	}

	// In-place version of decipher
	{
		RijndaelEngine	cipher;
		cipher.SetupKey(testData.key);

		unsigned char tmp[RijndaelEngine::BlockSize];

		copy(begin(testData.ciphertext), end(testData.ciphertext), tmp);


		cipher.DecipherBlock(tmp);

		BOOST_CHECK_EQUAL_COLLECTIONS(begin(testData.plaintext),
			end(testData.plaintext),
			begin(tmp),
			end(tmp));
	}
}

BOOST_AUTO_TEST_CASE( aesEncTestMy )
{
	BOOST_TEST_MESSAGE("Rijndael-256 encryption test");
	for_each(begin(AesTests), end(AesTests), AesEncTest);
}

BOOST_AUTO_TEST_CASE( aesDecTestMy )
{
	BOOST_TEST_MESSAGE("Rijndael-256 decryption test");
	for_each(begin(AesTests), end(AesTests), AesDecTest);
}

typedef struct TWOFISH_TEST{
	BYTE key[TWOFISH_KEY_SIZE];
	BYTE plaintext[TWOFISH_BLOCK_SIZE];
	BYTE ciphertext[TWOFISH_BLOCK_SIZE];
} TWOFISH_TEST;

static const TWOFISH_TEST TwofishTests[]=
{
	{
		{00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00},		// Key
		{00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00},		// Plaintext
		{0x57,0xFF,0x73,0x9D,0x4D,0xC9,0x2C,0x1B,0xD7,0xFC,0x01,0x70,0x0C,0xC8,0x21,0x6F}		// Ciphertext
	},
	{
		{0x24,0x8A,0x7F,0x35,0x28,0xB1,0x68,0xAC,0xFD,0xD1,0x38,0x6E,0x3F,0x51,0xE3,0x0C,0x2E,0x21,0x58,0xBC,0x3E,0x5F,0xC7,0x14,0xC1,0xEE,0xEC,0xA0,0xEA,0x69,0x6D,0x48},		// Key
		{0x43,0x10,0x58,0xF4,0xDB,0xC7,0xF7,0x34,0xDA,0x4F,0x02,0xF0,0x4C,0xC4,0xF4,0x59},		// Plaintext
		{0x37,0xFE,0x26,0xFF,0x1C,0xF6,0x61,0x75,0xF5,0xDD,0xF4,0xC3,0x3B,0x97,0xA2,0x05}		// Ciphertext
	}
};

void TwofishDecTest(const TWOFISH_TEST& testData)
{
	// Test copy version of cipher
	{
		TwofishEngine cipher;

		cipher.SetupKey(testData.key);

		unsigned char tmp[TwofishEngine::BlockSize];

		std::fill_n(tmp, _countof(tmp), 0);

		cipher.DecipherBlock(testData.ciphertext, tmp);

		BOOST_CHECK_EQUAL_COLLECTIONS(begin(testData.plaintext),
			end(testData.plaintext),
			begin(tmp),
			end(tmp));
	}

	// Test in-place version of cipher
	{
		TwofishEngine cipher;

		cipher.SetupKey(testData.key);

		unsigned char tmp[TwofishEngine::BlockSize];

		copy(begin(testData.ciphertext), end(testData.ciphertext), tmp);

		cipher.DecipherBlock(tmp);

		BOOST_CHECK_EQUAL_COLLECTIONS(begin(testData.plaintext),
			end(testData.plaintext),
			begin(tmp),
			end(tmp));
	}
}

void TwofishEncTest(const TWOFISH_TEST& testData)
{
	// Test copy version of cipher
	{
		TwofishEngine cipher;

		cipher.SetupKey(testData.key);

		unsigned char tmp[TwofishEngine::BlockSize];

		std::fill_n(tmp, _countof(tmp), 0);

		cipher.EncipherBlock(testData.plaintext, tmp);

		BOOST_CHECK_EQUAL_COLLECTIONS(begin(testData.ciphertext),
			end(testData.ciphertext),
			begin(tmp),
			end(tmp));
	}

	// Test in-place version of cipher
	{
		TwofishEngine cipher;

		cipher.SetupKey(testData.key);

		unsigned char tmp[TwofishEngine::BlockSize];

		copy(begin(testData.plaintext), end(testData.plaintext), tmp);

		cipher.EncipherBlock(tmp);

		BOOST_CHECK_EQUAL_COLLECTIONS(begin(testData.ciphertext),
			end(testData.ciphertext),
			begin(tmp),
			end(tmp));
	}
}


BOOST_AUTO_TEST_CASE( twofishEncTestMy )
{
	BOOST_TEST_MESSAGE("Twofish encryption test");
	for_each(begin(TwofishTests), end(TwofishTests), TwofishEncTest);
}

BOOST_AUTO_TEST_CASE( twofishDecTestMy )
{
	BOOST_TEST_MESSAGE("Twofish decrytption test");
	for_each(begin(TwofishTests), end(TwofishTests), TwofishDecTest);
}

static const  BYTE testRC4key1[] =
{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF };
static const  BYTE testRC4plaintext1[] =
{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF };
static const  BYTE testRC4ciphertext1[] =
{ 0x75, 0xB7, 0x87, 0x80, 0x99, 0xE0, 0xC5, 0x96 };

static const  BYTE testRC4key2[] =
{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF };
static const  BYTE testRC4plaintext2[] =
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const  BYTE testRC4ciphertext2[] =
{ 0x74, 0x94, 0xC2, 0xE7, 0x10, 0x4B, 0x08, 0x79 };

static const  BYTE testRC4key3[] =
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const  BYTE testRC4plaintext3[] =
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const  BYTE testRC4ciphertext3[] =
{ 0xDE, 0x18, 0x89, 0x41, 0xA3, 0x37, 0x5D, 0x3A };

static const  BYTE testRC4key4[] =
{ 0xEF, 0x01, 0x23, 0x45 };
static const  BYTE testRC4plaintext4[] =
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const  BYTE testRC4ciphertext4[] =
{ 0xD6, 0xA1, 0x41, 0xA7, 0xEC, 0x3C, 0x38, 0xDF, 0xBD, 0x61 };

static const  BYTE testRC4key5[] =
{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF };
static const  BYTE testRC4plaintext5[] =
{ 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 };
static const  BYTE testRC4ciphertext5[] =
{ 0x75, 0x95, 0xC3, 0xE6, 0x11, 0x4A, 0x09, 0x78,
0x0C, 0x4A, 0xD4, 0x52, 0x33, 0x8E, 0x1F, 0xFD,
0x9A, 0x1B, 0xE9, 0x49, 0x8F, 0x81, 0x3D, 0x76,
0x53, 0x34, 0x49, 0xB6, 0x77, 0x8D, 0xCA, 0xD8,
0xC7, 0x8A, 0x8D, 0x2B, 0xA9, 0xAC, 0x66, 0x08,
0x5D, 0x0E, 0x53, 0xD5, 0x9C, 0x26, 0xC2, 0xD1,
0xC4, 0x90, 0xC1, 0xEB, 0xBE, 0x0C, 0xE6, 0x6D,
0x1B, 0x6B, 0x1B, 0x13, 0xB6, 0xB9, 0x19, 0xB8,
0x47, 0xC2, 0x5A, 0x91, 0x44, 0x7A, 0x95, 0xE7,
0x5E, 0x4E, 0xF1, 0x67, 0x79, 0xCD, 0xE8, 0xBF,
0x0A, 0x95, 0x85, 0x0E, 0x32, 0xAF, 0x96, 0x89,
0x44, 0x4F, 0xD3, 0x77, 0x10, 0x8F, 0x98, 0xFD,
0xCB, 0xD4, 0xE7, 0x26, 0x56, 0x75, 0x00, 0x99,
0x0B, 0xCC, 0x7E, 0x0C, 0xA3, 0xC4, 0xAA, 0xA3,
0x04, 0xA3, 0x87, 0xD2, 0x0F, 0x3B, 0x8F, 0xBB,
0xCD, 0x42, 0xA1, 0xBD, 0x31, 0x1D, 0x7A, 0x43,
0x03, 0xDD, 0xA5, 0xAB, 0x07, 0x88, 0x96, 0xAE,
0x80, 0xC1, 0x8B, 0x0A, 0xF6, 0x6D, 0xFF, 0x31,
0x96, 0x16, 0xEB, 0x78, 0x4E, 0x49, 0x5A, 0xD2,
0xCE, 0x90, 0xD7, 0xF7, 0x72, 0xA8, 0x17, 0x47,
0xB6, 0x5F, 0x62, 0x09, 0x3B, 0x1E, 0x0D, 0xB9,
0xE5, 0xBA, 0x53, 0x2F, 0xAF, 0xEC, 0x47, 0x50,
0x83, 0x23, 0xE6, 0x71, 0x32, 0x7D, 0xF9, 0x44,
0x44, 0x32, 0xCB, 0x73, 0x67, 0xCE, 0xC8, 0x2F,
0x5D, 0x44, 0xC0, 0xD0, 0x0B, 0x67, 0xD6, 0x50,
0xA0, 0x75, 0xCD, 0x4B, 0x70, 0xDE, 0xDD, 0x77,
0xEB, 0x9B, 0x10, 0x23, 0x1B, 0x6B, 0x5B, 0x74,
0x13, 0x47, 0x39, 0x6D, 0x62, 0x89, 0x74, 0x21,
0xD4, 0x3D, 0xF9, 0xB4, 0x2E, 0x44, 0x6E, 0x35,
0x8E, 0x9C, 0x11, 0xA9, 0xB2, 0x18, 0x4E, 0xCB,
0xEF, 0x0C, 0xD8, 0xE7, 0xA8, 0x77, 0xEF, 0x96,
0x8F, 0x13, 0x90, 0xEC, 0x9B, 0x3D, 0x35, 0xA5,
0x58, 0x5C, 0xB0, 0x09, 0x29, 0x0E, 0x2F, 0xCD,
0xE7, 0xB5, 0xEC, 0x66, 0xD9, 0x08, 0x4B, 0xE4,
0x40, 0x55, 0xA6, 0x19, 0xD9, 0xDD, 0x7F, 0xC3,
0x16, 0x6F, 0x94, 0x87, 0xF7, 0xCB, 0x27, 0x29,
0x12, 0x42, 0x64, 0x45, 0x99, 0x85, 0x14, 0xC1,
0x5D, 0x53, 0xA1, 0x8C, 0x86, 0x4C, 0xE3, 0xA2,
0xB7, 0x55, 0x57, 0x93, 0x98, 0x81, 0x26, 0x52,
0x0E, 0xAC, 0xF2, 0xE3, 0x06, 0x6E, 0x23, 0x0C,
0x91, 0xBE, 0xE4, 0xDD, 0x53, 0x04, 0xF5, 0xFD,
0x04, 0x05, 0xB3, 0x5B, 0xD9, 0x9C, 0x73, 0x13,
0x5D, 0x3D, 0x9B, 0xC3, 0x35, 0xEE, 0x04, 0x9E,
0xF6, 0x9B, 0x38, 0x67, 0xBF, 0x2D, 0x7B, 0xD1,
0xEA, 0xA5, 0x95, 0xD8, 0xBF, 0xC0, 0x06, 0x6F,
0xF8, 0xD3, 0x15, 0x09, 0xEB, 0x0C, 0x6C, 0xAA,
0x00, 0x6C, 0x80, 0x7A, 0x62, 0x3E, 0xF8, 0x4C,
0x3D, 0x33, 0xC1, 0x95, 0xD2, 0x3E, 0xE3, 0x20,
0xC4, 0x0D, 0xE0, 0x55, 0x81, 0x57, 0xC8, 0x22,
0xD4, 0xB8, 0xC5, 0x69, 0xD8, 0x49, 0xAE, 0xD5,
0x9D, 0x4E, 0x0F, 0xD7, 0xF3, 0x79, 0x58, 0x6B,
0x4B, 0x7F, 0xF6, 0x84, 0xED, 0x6A, 0x18, 0x9F,
0x74, 0x86, 0xD4, 0x9B, 0x9C, 0x4B, 0xAD, 0x9B,
0xA2, 0x4B, 0x96, 0xAB, 0xF9, 0x24, 0x37, 0x2C,
0x8A, 0x8F, 0xFF, 0xB1, 0x0D, 0x55, 0x35, 0x49,
0x00, 0xA7, 0x7A, 0x3D, 0xB5, 0xF2, 0x05, 0xE1,
0xB9, 0x9F, 0xCD, 0x86, 0x60, 0x86, 0x3A, 0x15,
0x9A, 0xD4, 0xAB, 0xE4, 0x0F, 0xA4, 0x89, 0x34,
0x16, 0x3D, 0xDD, 0xE5, 0x42, 0xA6, 0x58, 0x55,
0x40, 0xFD, 0x68, 0x3C, 0xBF, 0xD8, 0xC0, 0x0F,
0x12, 0x12, 0x9A, 0x28, 0x4D, 0xEA, 0xCC, 0x4C,
0xDE, 0xFE, 0x58, 0xBE, 0x71, 0x37, 0x54, 0x1C,
0x04, 0x71, 0x26, 0xC8, 0xD4, 0x9E, 0x27, 0x55,
0xAB, 0x18, 0x1A, 0xB7, 0xE9, 0x40, 0xB0, 0xC0 };

struct RC4_TEST
{
	size_t					keyLength;
	const unsigned char*	keyData;
	size_t					textLength;
	const unsigned char*	plainText;
	const unsigned char*	cipherText;
};


static const RC4_TEST rc4TestSamples[] =
{
	{
		sizeof(testRC4key1), testRC4key1,
		sizeof(testRC4plaintext1),
		testRC4plaintext1, testRC4ciphertext1
	},
	{
		sizeof(testRC4key2), testRC4key2,
		sizeof(testRC4plaintext2),
		testRC4plaintext2, testRC4ciphertext2
	},
	{
		sizeof(testRC4key3), testRC4key3,
		sizeof(testRC4plaintext3),
		testRC4plaintext3, testRC4ciphertext3
	},
	{
		sizeof(testRC4key4), testRC4key4,
		sizeof(testRC4plaintext4),
		testRC4plaintext4, testRC4ciphertext4
	},
	{
		sizeof(testRC4key5), testRC4key5,
		sizeof(testRC4plaintext5),
		testRC4plaintext5, testRC4ciphertext5
	}
};


void Rc4Test(const RC4_TEST& testData)
{
	unsigned char *tmp = new unsigned char[testData.textLength];

	RC4_CIPHER_ENGINE cipher;
	cipher.SetupKey(testData.keyData, static_cast<int>(testData.keyLength));

	std::copy(testData.plainText, testData.plainText + testData.textLength,
		checked_array_iterator<unsigned char*>(tmp, testData.textLength));

	cipher.Xor(tmp, static_cast<int>(testData.textLength));

	BOOST_CHECK(std::equal(testData.cipherText, testData.cipherText + testData.textLength,
		checked_array_iterator<unsigned char*>(tmp, testData.textLength)));

	delete[] tmp;
}

BOOST_AUTO_TEST_CASE( rc4Test )
{
	BOOST_TEST_MESSAGE("RC4 test");

	BOOST_REQUIRE_EQUAL(sizeof(testRC4plaintext1), sizeof(testRC4ciphertext1));
	BOOST_REQUIRE_EQUAL(sizeof(testRC4plaintext2), sizeof(testRC4ciphertext2));
	BOOST_REQUIRE_EQUAL(sizeof(testRC4plaintext3), sizeof(testRC4ciphertext3));
	BOOST_REQUIRE_EQUAL(sizeof(testRC4plaintext4), sizeof(testRC4ciphertext4));
	BOOST_REQUIRE_EQUAL(sizeof(testRC4plaintext5), sizeof(testRC4ciphertext5));

	for_each(begin(rc4TestSamples), end(rc4TestSamples), Rc4Test);
}

struct SHA_TEST
{
	char *data;							/* Data to hash */
	int length;							/* Length of data */
	BYTE dig256[ SHA256_HASH::didgestSize ];	/* Digest of data */
};

const SHA_TEST ShaTests[]=
{
	{
		"abc",3,
		{
			0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea,
			0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
			0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c,
			0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad
		},
	},
	{
		"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 56,
		{
			0x24, 0x8d, 0x6a, 0x61, 0xd2, 0x06, 0x38, 0xb8,
			0xe5, 0xc0, 0x26, 0x93, 0x0c, 0x3e, 0x60, 0x39,
			0xa3, 0x3c, 0xe4, 0x59, 0x64, 0xff, 0x21, 0x67,
			0xf6, 0xec, 0xed, 0xd4, 0x19, 0xdb, 0x06, 0xc1
		},
	}
};

SHA_TEST ShaTestLong =
{
	"aaaaa...", 1000000L,
	{
		0xcd, 0xc7, 0x6e, 0x5c, 0x99, 0x14, 0xfb, 0x92,
			0x81, 0xa1, 0xc7, 0xe2, 0x84, 0xd7, 0x3e, 0x67,
			0xf1, 0x80, 0x9a, 0x48, 0xa4, 0x97, 0x20, 0x0e,
			0x04, 0x6d, 0x39, 0xcc, 0xc7, 0x11, 0x2c, 0xd0
	},
};

void TestSha256(const SHA_TEST& testData)
{
	SHA256_HASH	hash;

	unsigned char tmp[SHA256_HASH::didgestSize];

	hash.Init();
	hash.Update(testData.data, testData.length);
	hash.Final(tmp);

	BOOST_CHECK(std::equal(begin(testData.dig256), end(testData.dig256), tmp));
}

BOOST_AUTO_TEST_CASE( sha256Test )
{
	BOOST_TEST_MESSAGE("SHA-256 test");

	for_each(begin(ShaTests), end(ShaTests), TestSha256);

	ShaTestLong.data = new char [ShaTestLong.length];

	std::fill_n(
		checked_array_iterator<char*>(ShaTestLong.data, ShaTestLong.length),
		ShaTestLong.length,
		'a');

	TestSha256(ShaTestLong);

	delete[] ShaTestLong.data;
}
