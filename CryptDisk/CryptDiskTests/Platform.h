
#pragma once

#define ASSERT(x) assert(x)

typedef enum _POOL_TYPE {
	NonPagedPool,
	PagedPool,
	NonPagedPoolMustSucceed,
	DontUseThisType,
	NonPagedPoolCacheAligned,
	PagedPoolCacheAligned,
	NonPagedPoolCacheAlignedMustS,
	MaxPoolType,

	//
	// Note these per session types are carefully chosen so that the appropriate
	// masking still applies as well as MaxPoolType above.
	//

	NonPagedPoolSession = 32,
	PagedPoolSession = NonPagedPoolSession + 1,
	NonPagedPoolMustSucceedSession = PagedPoolSession + 1,
	DontUseThisTypeSession = NonPagedPoolMustSucceedSession + 1,
	NonPagedPoolCacheAlignedSession = DontUseThisTypeSession + 1,
	PagedPoolCacheAlignedSession = NonPagedPoolCacheAlignedSession + 1,
	NonPagedPoolCacheAlignedMustSSession = PagedPoolCacheAlignedSession + 1,
} POOL_TYPE;

inline
PVOID
ExAllocatePoolWithTag(
POOL_TYPE PoolType,
size_t NumberOfBytes,
ULONG Tag
)
{
	return malloc(NumberOfBytes);
}

inline
void
ExFreePoolWithTag(
PVOID P,
ULONG Tag
)
{
	free(P);
}

#define	MEM_TAG		('sDrC')
