
#include <memory.h>
#include "aes.h"
#include "AesWrapper.h"
#include "PlatformHelpers.h"

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
	__declspec(align(16)) unsigned char tmp[AES_BLOCK_SIZE];

	aes_encrypt(Buff, tmp, &pCTX->encCtx);
	memcpy(Buff, tmp, sizeof(tmp));
}

void __stdcall XorAndEncipher1(const RIJNDAEL_KEY_CTX *pCTX, const void *XorData, const void *PlainText, void *CipherText)
{
	__declspec(align(16)) unsigned char tmp[AES_BLOCK_SIZE];

	XorBlock128(XorData, PlainText, tmp);

	aes_encrypt(tmp, CipherText, &pCTX->encCtx);
}

void __stdcall XorAndEncipher2(const RIJNDAEL_KEY_CTX *pCTX, const void *XorData, void *Buff)
{
	__declspec(align(16)) unsigned char tmp[AES_BLOCK_SIZE];

	XorBlock128(XorData, Buff, tmp);

	aes_encrypt(tmp, Buff, &pCTX->encCtx);
}

void __stdcall DecipherBlock1(const RIJNDAEL_KEY_CTX *pCTX, const void *CipherText, void *PlainText)
{
	aes_decrypt(CipherText, PlainText, &pCTX->decCtx);
}

void __stdcall DecipherBlock2(const RIJNDAEL_KEY_CTX *pCTX, void *Buff)
{
	__declspec(align(16)) unsigned char tmp[AES_BLOCK_SIZE];

	aes_decrypt(Buff, tmp, &pCTX->decCtx);
	memcpy(Buff, tmp, sizeof(tmp));
}

void __stdcall DecipherAndXor1(const RIJNDAEL_KEY_CTX *pCTX, const void *XorData, const void *CipherText, void *PlainText)
{
	__declspec(align(16)) unsigned char tmp[AES_BLOCK_SIZE];

	aes_decrypt(CipherText, tmp, &pCTX->decCtx);

	XorBlock128(XorData, tmp, PlainText);
}

void __stdcall DecipherAndXor2(const RIJNDAEL_KEY_CTX *pCTX, const void *XorData, void *Buff)
{
	__declspec(align(16)) unsigned char tmp[AES_BLOCK_SIZE];

	aes_decrypt(Buff, tmp, &pCTX->decCtx);

	XorBlock128(XorData, tmp, Buff);
}
