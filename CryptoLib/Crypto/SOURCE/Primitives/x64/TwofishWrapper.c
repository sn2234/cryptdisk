
#include <memory.h>
#include "PlatformHelpers.h"
#include "twofish_ltc.h"

#define	TWOFISH_KEY_BITS		(256)
#define	TWOFISH_KEY_BYTES		(TWOFISH_KEY_BITS/8)

#define	TWOFISH_BLOCK_BITS		(128)
#define	TWOFISH_BLOCK_BYTES		(TWOFISH_BLOCK_BITS/8)

void twofish_setup(const unsigned char *key, int keylen, TWOFISH_KEY_CTX *skey);
void twofish_ecb_encrypt(const unsigned char *pt, unsigned char *ct, const TWOFISH_KEY_CTX *skey);
void twofish_ecb_decrypt(const unsigned char *ct, unsigned char *pt, const TWOFISH_KEY_CTX *skey);

void __stdcall TwofishSetKey(TWOFISH_KEY_CTX *pCtx, const void *pUserKey)
{
	twofish_setup(pUserKey, TWOFISH_KEY_BYTES, pCtx);
}

void __stdcall TwofishEncBlock1(const TWOFISH_KEY_CTX *pCtx, const void *PlainText, void *CipherText)
{
	twofish_ecb_encrypt(PlainText, CipherText, pCtx);
}

void __stdcall TwofishEncBlock2(const TWOFISH_KEY_CTX *pCtx, void *Buff)
{
	unsigned char tmp[TWOFISH_BLOCK_BYTES];

	twofish_ecb_encrypt(Buff, tmp, pCtx);
	memcpy(Buff, tmp, sizeof(tmp));
}

void __stdcall TwofishXorEnc1(const TWOFISH_KEY_CTX *pCtx, void *XorData, const void *PlainText, void *CipherText)
{
	unsigned char tmp[TWOFISH_BLOCK_BYTES];

	XorBlock128(XorData, PlainText, tmp);

	twofish_ecb_encrypt(tmp, CipherText, pCtx);
}

void __stdcall TwofishXorEnc2(const TWOFISH_KEY_CTX *pCtx, void *XorData, void *Buff)
{
	unsigned char tmp[TWOFISH_BLOCK_BYTES];

	XorBlock128(XorData, Buff, tmp);

	twofish_ecb_encrypt(tmp, Buff, pCtx);
}

void __stdcall TwofishDecBlock1(const TWOFISH_KEY_CTX *pCtx, const void *CipherText, void *PlainText)
{
	twofish_ecb_decrypt(CipherText, PlainText, pCtx);
}

void __stdcall TwofishDecBlock2(const TWOFISH_KEY_CTX *pCtx, void *Buff)
{
	unsigned char tmp[TWOFISH_BLOCK_BYTES];

	twofish_ecb_decrypt(Buff, tmp, pCtx);
	memcpy(Buff, tmp, sizeof(tmp));
}

void __stdcall TwofishDecXor1(const TWOFISH_KEY_CTX *pCtx, void *XorData, const void *CipherText, void *PlainText)
{
	unsigned char tmp[TWOFISH_BLOCK_BYTES];

	twofish_ecb_decrypt(CipherText, tmp, pCtx);

	XorBlock128(XorData, tmp, PlainText);
}

void __stdcall TwofishDecXor2(const TWOFISH_KEY_CTX *pCtx, void *XorData, void *Buff)
{
	unsigned char tmp[TWOFISH_BLOCK_BYTES];

	twofish_ecb_decrypt(Buff, tmp, pCtx);

	XorBlock128(XorData, tmp, Buff);
}
