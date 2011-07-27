
#pragma once

#include "Singleton.h"
#include "SecureHeap.h"

namespace
{
class InitSecHeap
{
public:
	static CSecureHeap* Alloc()
	{
		return new CSecureHeap(10*1024);
	}
};
}

typedef Singleton<CSecureHeap, InitSecHeap> AppMemory;

