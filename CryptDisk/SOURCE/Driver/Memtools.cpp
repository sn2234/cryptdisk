
#include "stdafx.h"

void* __cdecl operator new (size_t size, unsigned char* pBuff)
{
	return pBuff;
}

void __cdecl operator delete(void *ptr, unsigned char* pBuff)
{
}

void __cdecl operator delete(void *ptr)
{
}
