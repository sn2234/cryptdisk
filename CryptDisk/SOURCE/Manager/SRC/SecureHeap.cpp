
#include "StdAfx.h"
#include "SecureHeap.h"

CSecureHeap::CSecureHeap(void)
{
	m_pHeap=NULL;
	m_heapSize=0;
}

CSecureHeap::~CSecureHeap(void)
{
	Close();
}

BOOL CSecureHeap::Init(DWORD dwPoolSize)
{
	Close();

	m_pHeap=(HEAP_BLOCK*)VirtualAlloc(NULL, dwPoolSize, MEM_COMMIT, PAGE_READWRITE);
	if(!m_pHeap)
		return FALSE;
	if(!VirtualLock(m_pHeap, dwPoolSize))
	{
		VirtualFree(m_pHeap, 0, MEM_RELEASE);
		m_pHeap=NULL;
		return FALSE;
	}
	m_heapSize=dwPoolSize;

	m_pHeap->bFree=TRUE;
	m_pHeap->size=dwPoolSize;
	return TRUE;
}

void CSecureHeap::Close()
{
	if(m_pHeap)
	{
		VirtualUnlock(m_pHeap, m_heapSize);
		VirtualFree(m_pHeap, 0, MEM_RELEASE);
		m_pHeap=NULL;
		m_heapSize=0;
	}
}

void *CSecureHeap::Alloc(DWORD size)
{
	HEAP_BLOCK	*ptr;

	if(!m_pHeap)
		return NULL;

	ptr=m_pHeap;

	for(;;)
	{
		// Find free block
		if(ptr->bFree)
		{
			DWORD	blockSize;

			// Check size
			blockSize=ptr->size-sizeof(HEAP_BLOCK);
			if(blockSize > size)
			{
				// Check if we can split it
				if(blockSize-size > (sizeof(HEAP_BLOCK)+8))
				{
					// Split block
					HEAP_BLOCK	*pNewBlock;

					pNewBlock=(HEAP_BLOCK*)((BYTE*)ptr+sizeof(HEAP_BLOCK)+size);
					pNewBlock->bFree=TRUE;
					pNewBlock->size=ptr->size-sizeof(HEAP_BLOCK)-size;
					ptr->size=sizeof(HEAP_BLOCK)+size;
					ptr->bFree=FALSE;

					return (void*)((BYTE*)ptr+sizeof(HEAP_BLOCK));
				}
				else
				{
					// Allocate whole block
					ptr->bFree=FALSE;

					return (void*)((BYTE*)ptr+sizeof(HEAP_BLOCK));
				}
			}
		}

		if((BYTE*)ptr+ptr->size >= (BYTE*)m_pHeap+m_heapSize)
		{
			break;
		}
		ptr=(HEAP_BLOCK*)((BYTE*)ptr+ptr->size);
	}

	return NULL;
}

void CSecureHeap::Free(void *ptr)
{
	// Check if given ptr is in heap
	if((ptr < m_pHeap) || ((BYTE*)ptr >= (BYTE*)m_pHeap+m_heapSize))
		return;

	// Fond block header for given ptr
	HEAP_BLOCK	*pHeader;

	pHeader=(HEAP_BLOCK*)((BYTE*)ptr-sizeof(HEAP_BLOCK));

	pHeader->bFree=TRUE;

	CheckFreeBlocks(pHeader);
}

void CSecureHeap::CheckFreeBlocks(HEAP_BLOCK *ptr)
{
	HEAP_BLOCK *pNext;

	for(;;)
	{
		pNext=(HEAP_BLOCK*)((BYTE*)ptr+ptr->size);

		if(((BYTE*)pNext >= (BYTE*)m_pHeap+m_heapSize) || (!pNext->bFree))
		{
			break;
		}
		ptr->size+=pNext->size;
	}
}
