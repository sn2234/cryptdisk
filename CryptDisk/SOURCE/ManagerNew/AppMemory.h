
#pragma once

#include "Singleton.h"
#include "SecureHeap.h"

namespace
{
class InitSecHeap
{
public:
	static SecureHeap* Alloc()
	{
		return new SecureHeap(10*1024);
	}
};
}

typedef Singleton<SecureHeap, InitSecHeap> AppMemory;

