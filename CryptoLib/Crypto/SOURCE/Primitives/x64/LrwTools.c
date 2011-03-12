
#include "LrwTools.h"

void LShift128(unsigned char *pBuff) // Tested
{
	int		i,carry1,carry2;
	unsigned long	*pNumber=(unsigned long*)pBuff;

	carry1=carry2=0;
	for(i=0;i<4;i++)
	{
		carry2=(pNumber[i]&0x80000000)>>31;
		pNumber[i]<<=1;
		pNumber[i]|=carry1;
		carry1=carry2;
	}
}

int GetBit(unsigned char *pSrc, int bitNumber) // Tested
{
	return pSrc[bitNumber/8]&(1<<(bitNumber%8));
}

void SetBit(unsigned char *pSrc, int bitNumber) // Tested
{
	pSrc[bitNumber/8]|=(1<<(bitNumber%8));
}

void SetBit1(unsigned char *pSrc, int bitNumber) // Tested
{
	pSrc[(127-bitNumber)/8]|=(0x80>>((127-bitNumber)%8));
}

void MirrorBits(unsigned char *pDest, unsigned char *pSrc)
{
	int	i;

	for(i=0;i<128;i++)
	{
		SetBit1(pDest, GetBit(pSrc, i));
	}
}

void Swap128(unsigned char *pNumber) // Tested
{
	int	i;

	for(i=0;i<16/2;i++)
	{
		pNumber[i]^=pNumber[15-i];
		pNumber[15-i]^=pNumber[i];
		pNumber[i]^=pNumber[15-i];
	}
}

void Inc128(unsigned char *pNumber)
{
	int	i, carry, carry1;

	carry=0;
	carry1=0;
	if(((int)pNumber[0]+1) > 255)
	{
		carry=1;
	}
	pNumber[0]++;
	for(i=1;i<16;i++)
	{
		carry1=0;
		if(((int)pNumber[i]+carry) > 255)
		{
			carry1=1;
		}
		pNumber[i]+=carry;
		carry=carry1;
	}
}
