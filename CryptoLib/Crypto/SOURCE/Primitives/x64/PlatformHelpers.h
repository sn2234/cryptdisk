
#pragma once

#include <intrin.h>
#include <emmintrin.h>

void __forceinline XorBlock128(const void* xorBlock, const void* inBlock, void* outBlock)
{
	/*
	const __m128i* pXor = (const __m128i*)xorBlock;
	const __m128i* pIn = (const __m128i*)inBlock;
	__m128i* pOut = (__m128i*)outBlock;

	int i;

	for (i = 0; i < 16/sizeof(__m128i); i++)
	{
		pOut[i] = _mm_xor_si128(pXor[i], pIn[i]);
	}
	*/
	const unsigned __int64* pXor = (const unsigned __int64*)xorBlock;
	const unsigned __int64* pIn = (const unsigned __int64*)inBlock;
	unsigned __int64* pOut = (unsigned __int64*)outBlock;

	int i;

	for (i = 0; i < 16/sizeof(unsigned __int64); i++)
	{
		pOut[i] =pXor[i] ^ pIn[i];
	}
}
