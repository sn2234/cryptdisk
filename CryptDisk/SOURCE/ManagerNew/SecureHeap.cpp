/*
* Copyright (c) 2006, nobody
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "stdafx.h"
#include "SecureHeap.h"
#include "winapi_exception.h"

SecureHeap::SecureHeap(DWORD dwPoolSize)
{
	m_pHeap=(HEAP_BLOCK*)VirtualAlloc(NULL, dwPoolSize, MEM_COMMIT, PAGE_READWRITE);
	if(!m_pHeap)
	{
		throw winapi_exception("SecureHeap::SecureHeap: Unable to allocate memory");
	}

	if(!VirtualLock(m_pHeap, dwPoolSize))
	{
		VirtualFree(m_pHeap, 0, MEM_RELEASE);

		throw winapi_exception("SecureHeap::SecureHeap: Unable to lock memory");
	}
	m_heapSize=dwPoolSize;

	m_pHeap->bFree=TRUE;
	m_pHeap->size=dwPoolSize;
}

SecureHeap::~SecureHeap(void)
{
	VirtualUnlock(m_pHeap, m_heapSize);
	VirtualFree(m_pHeap, 0, MEM_RELEASE);
}

BOOL SecureHeap::Init(DWORD dwPoolSize)
{
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

void *SecureHeap::Alloc(DWORD size)
{
	HEAP_BLOCK	*ptr;

	ASSERT(m_pHeap);
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

void SecureHeap::Free(void *ptr)
{
	// Check if given ptr is in heap
	if((ptr < m_pHeap) || ((BYTE*)ptr >= (BYTE*)m_pHeap+m_heapSize))
		return;

	// Fond block header for given ptr
	HEAP_BLOCK	*pHeader;

	pHeader=(HEAP_BLOCK*)((BYTE*)ptr-sizeof(HEAP_BLOCK));

	pHeader->bFree=TRUE;

	// Zero data in block
	RtlSecureZeroMemory(ptr, pHeader->size-sizeof(HEAP_BLOCK));

	CheckFreeBlocks(pHeader);
}

void SecureHeap::CheckFreeBlocks(HEAP_BLOCK *ptr)
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
