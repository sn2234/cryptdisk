
#pragma once

typedef		long	BOOL;

#pragma intrinsic(memset,memcpy,memcmp)

// Defines
#if DBG
#define		DnPrint(X)	DbgPrint("\nCryptDisk:"X)
#else
#define		DnPrint(X)
#endif

#define	MEM_TAG		('sDrC')
