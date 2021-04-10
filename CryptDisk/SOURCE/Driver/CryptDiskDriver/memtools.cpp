
#include "stdafx.h"

void* __cdecl operator new (size_t size, unsigned char* pBuff)
{
	return pBuff;
}

void __cdecl operator delete(void *ptr, unsigned char* pBuff)
{
}

void __cdecl operator delete(void *ptr, unsigned __int64)
{
}

void __cdecl operator delete(void *ptr, unsigned int)
{
}

void __cdecl operator delete(void *ptr)
{
}

#if	defined(_USER_MODE_)
#include <new>
typedef align_val_t std::align_val_t;
#else
enum class align_val_t : size_t {};
#endif

void __cdecl operator delete(void*, unsigned int, enum std::align_val_t)
{
}
