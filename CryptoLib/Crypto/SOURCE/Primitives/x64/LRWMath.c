
#include <memory.h>

#include "LrwTools.h"
#include "../LRWMath.h"

void Xor128(unsigned char *pBuff, unsigned char *pSrc)
{
	((unsigned long*)pBuff)[0]^=((unsigned long*)pSrc)[0];
	((unsigned long*)pBuff)[1]^=((unsigned long*)pSrc)[1];
	((unsigned long*)pBuff)[2]^=((unsigned long*)pSrc)[2];
	((unsigned long*)pBuff)[3]^=((unsigned long*)pSrc)[3];
}

void GFMult(unsigned char *pDest, unsigned char *pC, unsigned char *pY)
{
	unsigned char	v[16];
	int		i;

	memset(pDest, 0, 16);
	memcpy(v, pC, 16);

	for(i=0;i<128;i++)
	{
		if(GetBit(pY, i))
		{
			Xor128(pDest, v);
		}

		if(GetBit(v, 127) == 0)
		{
			LShift128(v);
		}
		else
		{
			LShift128(v);
			v[0]^=0x87;
		}
	}
}

// This function prepares table for multiplication
void GFPrepareTable(unsigned char *pTable, unsigned char *pConstant)
{
	unsigned char	tmp[16];
	int		i;

	memset(tmp, 0, 16);
	memset(pTable, 0, 2048);

	tmp[0]=1;
	for(i=0;i<128;i++)
	{
		GFMult(&pTable[i*16], pConstant, tmp);
		LShift128(tmp);
	}
}

// This function performs multiplication in GF(2^128) using multiplication table
void GFMultTable(unsigned char *pTable, unsigned char *pDest, unsigned char *pSrc)
{
	int		i;

	memset(pDest, 0, 16);

	for(i=0;i<128;i++)
	{
		if(GetBit(pSrc, i))
		{
			Xor128(pDest, &pTable[i*16]);
		}
	}
}

void LRWPrepareTable(unsigned char *pTable, unsigned char *pKeyTable)
{
	unsigned char	tmp[16];
	int		i;

	memset(tmp, 0, 16);
	memset(pTable, 0, 2048);

	for(i=0;i<128;i++)
	{
		tmp[0]|=1;
		GFMultTable(pKeyTable, &pTable[i*16], tmp);
		LShift128(tmp);
	}
}

void LRWMult(unsigned char *pTable, unsigned char *pNumber, unsigned char *pBuff)
{
	int		zeroBitNumber;

	// Find zero bit in number
	for(zeroBitNumber=0;zeroBitNumber<128;zeroBitNumber++)
	{
		if(GetBit(pNumber, zeroBitNumber) == 0)
		{
			break;
		}
	}
	if(zeroBitNumber == 128)
	{
		zeroBitNumber=127;
	}

	Xor128(pBuff, &pTable[zeroBitNumber*16]);
	Inc128(pNumber);
}

void LRWInitContext(LRW_CONTEXT *pCtx, unsigned char *tweakKey)
{
	memcpy(pCtx->m_key, tweakKey, sizeof(pCtx->m_key));
	memset(pCtx->m_currentTweak, 0, sizeof(pCtx->m_currentTweak));
	memset(pCtx->m_currentIndex, 0, sizeof(pCtx->m_currentIndex));
	GFPrepareTable(pCtx->m_gfTable, tweakKey);
	LRWPrepareTable(pCtx->m_lrwTable, pCtx->m_gfTable);
}

void LRWStartSequence(LRW_CONTEXT *pCtx, const unsigned char *indexBegin)
{
	memcpy(pCtx->m_currentIndex, indexBegin, sizeof(pCtx->m_currentIndex));
	GFMultTable(pCtx->m_gfTable,
		pCtx->m_currentTweak, pCtx->m_currentIndex);
}

void LRWXorTweak(LRW_CONTEXT *pCtx,void *pBuff)
{
	Xor128((unsigned char *)pBuff, pCtx->m_currentTweak);
}

void LRWNextTweak(LRW_CONTEXT *pCtx)
{
	LRWMult(pCtx->m_lrwTable, pCtx->m_currentIndex, pCtx->m_currentTweak);
}
