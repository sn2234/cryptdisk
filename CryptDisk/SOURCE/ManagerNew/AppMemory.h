
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
		return new CSecureHeap(100000);
	}
};
}

typedef Singleton<CSecureHeap, InitSecHeap> AppMemory;

