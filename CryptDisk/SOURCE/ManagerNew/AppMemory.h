
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
		return new CSecureHeap(100*1024);
	}
};
}

typedef Singleton<CSecureHeap, InitSecHeap> AppMemory;

