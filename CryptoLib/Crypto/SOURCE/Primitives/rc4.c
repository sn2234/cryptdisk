
#include "rc4.h"

void __stdcall rc4KeySetup(RC4_KEY_CTX *pCtx, void *pUserKey, int KeyLengthBytes)
{
	int i;
	int j;
	char tmp;
	unsigned char* pKey = (unsigned char*)pUserKey;

	for (i = 0; i < 256; i++)
	{
		pCtx->State[i] = i;
	}

	for (i =0, j = 0; i < 256; i++)
	{
		j = (j + pKey[i % KeyLengthBytes] + pCtx->State[i]) & 255;

		tmp = pCtx->State[i];
		pCtx->State[i] = pCtx->State[j];
		pCtx->State[j] = tmp;
	}

	pCtx->x = 0;
	pCtx->y = 0;
}

void __stdcall rc4Gen(RC4_KEY_CTX *pCtx, void *Buff, int BuffLen)
{
	int i;
	unsigned long x;
	unsigned long y;
	char tmp;
	unsigned char* pOut = (unsigned char*)Buff;

	x = pCtx->x;
	y = pCtx->y;

	for (i = 0; i < BuffLen; i++)
	{
		x = (x + 1) % 256;
		y = (pCtx->State[x] + y) % 256;

		tmp = pCtx->State[x];
		pCtx->State[x] = pCtx->State[y];
		pCtx->State[y] = tmp;

		pOut[i] = pCtx->State[(pCtx->State[x] + pCtx->State[y]) % 256];
	}

	pCtx->x = x;
	pCtx->y = y;
}

void __stdcall rc4Xor(RC4_KEY_CTX *pCtx, void *Buff, int BuffLen)
{
	int i;
	unsigned long x;
	unsigned long y;
	char tmp;
	unsigned char* pOut = (unsigned char*)Buff;

	x = pCtx->x;
	y = pCtx->y;

	for (i = 0; i < BuffLen; i++)
	{
		x = (x + 1) % 256;
		y = (pCtx->State[x] + y) % 256;

		tmp = pCtx->State[x];
		pCtx->State[x] = pCtx->State[y];
		pCtx->State[y] = tmp;

		pOut[i] ^= pCtx->State[(pCtx->State[x] + pCtx->State[y]) % 256];
	}

	pCtx->x = x;
	pCtx->y = y;
}

void __stdcall rc4Skip(RC4_KEY_CTX *pCtx, int SkipLen)
{
	int i;
	unsigned long x;
	unsigned long y;
	char tmp;

	x = pCtx->x;
	y = pCtx->y;

	for (i = 0; i < SkipLen; i++)
	{
		x = (x + 1) % 256;
		y = (pCtx->State[x] + y) % 256;

		tmp = pCtx->State[x];
		pCtx->State[x] = pCtx->State[y];
		pCtx->State[y] = tmp;
	}

	pCtx->x = x;
	pCtx->y = y;
}
