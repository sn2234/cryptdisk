
#pragma once

#include "Singleton.h"
#include "SecureHeap.h"

class InitSecHeap
{
public:
	static SecureHeap* Alloc()
	{
		return new SecureHeap(10*1024);
	}
};

typedef Singleton<SecureHeap, InitSecHeap> AppMemory;

inline boost::shared_array<char> AllocPasswordBuffer(size_t length)
{
	return boost::shared_array<char>(static_cast<char*>(AppMemory::instance().Alloc(static_cast<DWORD>(length))),
		[](void *memoryPointer) {
		AppMemory::instance().Free(memoryPointer);
	});
}
