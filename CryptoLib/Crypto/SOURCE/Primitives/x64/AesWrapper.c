
#include <memory.h>
#include <intrin.h>
#include <emmintrin.h>
#include "aes.h"
#include "AesWrapper.h"

void __forceinline XorAesBlock(const void* xorBlock, const void* inBlock, void* outBlock)
{
	const __m128i* pXor = (const __m128i*)xorBlock;
	const __m128i* pIn = (const __m128i*)inBlock;
	__m128i* pOut = (__m128i*)outBlock;

	int i;

	for (i = 0; i < AES_BLOCK_SIZE/sizeof(__m128i); i++)
	{
		pOut[i] = _mm_xor_si128(pXor[i], pIn[i]);
	}
}

void __stdcall SetupKey(RIJNDAEL_KEY_CTX *pCTX, void *pUserKey)
{
	aes_encrypt_key256(pUserKey, &pCTX->encCtx);
	aes_decrypt_key256(pUserKey, &pCTX->decCtx);
}

void __stdcall EncipherBlock1(const RIJNDAEL_KEY_CTX *pCTX, const void *PlainText, void *CipherText)
{
	aes_encrypt(PlainText, CipherText, &pCTX->encCtx);
}

void __stdcall EncipherBlock2(const RIJNDAEL_KEY_CTX *pCTX, void *Buff)
{
	unsigned char tmp[AES_BLOCK_SIZE];

	aes_encrypt(Buff, tmp, &pCTX->encCtx);
	memcpy(Buff, tmp, sizeof(tmp));
}

void __stdcall XorAndEncipher1(const RIJNDAEL_KEY_CTX *pCTX, const void *XorData, const void *PlainText, void *CipherText)
{
	unsigned char tmp[AES_BLOCK_SIZE];

	XorAesBlock(XorData, PlainText, tmp);

	aes_encrypt(tmp, CipherText, &pCTX->encCtx);
}

void __stdcall XorAndEncipher2(const RIJNDAEL_KEY_CTX *pCTX, const void *XorData, void *Buff)
{
	unsigned char tmp[AES_BLOCK_SIZE];

	XorAesBlock(XorData, Buff, tmp);

	aes_encrypt(tmp, Buff, &pCTX->encCtx);
}

void __stdcall DecipherBlock1(const RIJNDAEL_KEY_CTX *pCTX, const void *CipherText, void *PlainText)
{
	aes_decrypt(CipherText, PlainText, &pCTX->decCtx);
}

void __stdcall DecipherBlock2(const RIJNDAEL_KEY_CTX *pCTX, void *Buff)
{
	unsigned char tmp[AES_BLOCK_SIZE];

	aes_decrypt(Buff, tmp, &pCTX->decCtx);
	memcpy(Buff, tmp, sizeof(tmp));
}

void __stdcall DecipherAndXor1(const RIJNDAEL_KEY_CTX *pCTX, const void *XorData, const void *CipherText, void *PlainText)
{
	unsigned char tmp[AES_BLOCK_SIZE];

	aes_decrypt(CipherText, tmp, &pCTX->decCtx);

	XorAesBlock(XorData, tmp, PlainText);
}

void __stdcall DecipherAndXor2(const RIJNDAEL_KEY_CTX *pCTX, const void *XorData, void *Buff)
{
	unsigned char tmp[AES_BLOCK_SIZE];

	aes_decrypt(Buff, tmp, &pCTX->decCtx);

	XorAesBlock(XorData, tmp, Buff);
}
