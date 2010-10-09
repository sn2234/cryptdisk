
#include "stdafx.h"

using namespace CryptoLib;
using namespace boost;
using namespace std;
using namespace stdext;

// Test cases are generated using Crypto++ as reference implementation
struct PBKDF2Test
{
	size_t					passwordLength;
	const unsigned char*	password;
	size_t					saltLength;
	const unsigned char*	salt;
	unsigned long			iterations;
	size_t					keyLength;
	const unsigned char*	key;
};


const unsigned char test1Password[] = {0x70, 0x61, 0x73, 0x73, 0x77, 0x6F, 0x72, 0x64};
const unsigned char test1Salt[] = {0x12, 0x34, 0x56, 0x78, 0x78, 0x56, 0x34, 0x12};
const unsigned char test1Key[] = {0x74, 0xE9, 0x8B, 0x2E, 0x9E, 0xED, 0xDA, 0xAB};
const unsigned char test2Password[] = {0x41, 0x6C, 0x6C, 0x20, 0x6E, 0x2D, 0x65, 0x6E, 0x74, 0x69, 0x74, 0x69, 0x65, 0x73, 0x20, 0x6D, 0x75, 0x73, 0x74, 0x20, 0x63, 0x6F, 0x6D, 0x6D, 0x75, 0x6E, 0x69, 0x63, 0x61, 0x74, 0x65, 0x20, 0x77, 0x69, 0x74, 0x68, 0x20, 0x6F, 0x74, 0x68, 0x65, 0x72, 0x20, 0x6E, 0x2D, 0x65, 0x6E, 0x74, 0x69, 0x74, 0x69, 0x65, 0x73, 0x20, 0x76, 0x69, 0x61, 0x20, 0x6E, 0x2D, 0x31, 0x20, 0x65, 0x6E, 0x74, 0x69, 0x74, 0x65, 0x65, 0x68, 0x65, 0x65, 0x68, 0x65, 0x65, 0x73};
const unsigned char test2Salt[] = {0x12, 0x34, 0x56, 0x78, 0x78, 0x56, 0x34, 0x12};
const unsigned char test2Key[] = {0x80, 0xB, 0x1C, 0x9D, 0x6D, 0x0, 0x75, 0xA8, 0xF3, 0xDF, 0x7A, 0x17, 0xCA, 0x32, 0x72, 0x2E, 0x93, 0x1, 0xA1, 0x9F, 0x6C, 0xB0, 0x52, 0x65};
const PBKDF2Test testCases[] =
{
	{
		sizeof(test1Password), test1Password, sizeof(test1Salt), test1Salt, 5, sizeof(test1Key), test1Key
	},
	{
		sizeof(test2Password), test2Password, sizeof(test2Salt), test2Salt, 500, sizeof(test2Key), test2Key
	}
};

void DoPkcs5HmacSha256Test(const PBKDF2Test& testData)
{
	vector<unsigned char>	tmp(testData.keyLength, 0);

	Pkcs5DeriveKey<SHA256_HASH>(testData.passwordLength, testData.password,
		testData.saltLength, testData.salt, testData.iterations,
		testData.keyLength, &tmp[0]);

	BOOST_CHECK(equal(boost::begin(tmp), boost::end(tmp),
		checked_array_iterator<const unsigned char*>(testData.key, testData.keyLength)));
}

BOOST_AUTO_TEST_CASE( pkcs5HmacSha256Test )
{
	BOOST_MESSAGE("PKCS 5.2.1 with HMAC-SHA-256 test");

	for_each(begin(testCases), end(testCases), DoPkcs5HmacSha256Test);
}
