
#pragma once

typedef struct HEAP_BLOCK
{
	DWORD		bFree:1;	// Is this block free
	DWORD		size:31;	// Size of block with header
}HEAP_BLOCK;

class CSecureHeap
{
public:
	CSecureHeap(void);
	~CSecureHeap(void);

	BOOL Init(DWORD dwPoolSize);
	void Close();

	void *Alloc(DWORD size);
	void Free(void *ptr);
protected:
	void CheckFreeBlocks(HEAP_BLOCK *ptr);
protected:
	DWORD		m_heapSize;
	HEAP_BLOCK	*m_pHeap;
};
