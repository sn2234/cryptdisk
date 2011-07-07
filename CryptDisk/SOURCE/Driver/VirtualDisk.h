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

#pragma once

#include "IDiskCipher.h"
#include "DriverProtocol.h"

class	VirtualDisk
{
	friend class DisksManager;
public:
	// Generic functions
	NTSTATUS			Init(DISK_ADD_INFO *Info, PDEVICE_OBJECT pDevice);
	NTSTATUS			Close(bool bForce);

	// Initialization and cleanup from worker thread
	void				Cleanup();
	NTSTATUS			InternalInit(DISK_ADD_INFO *Info, PDEVICE_OBJECT pDevice);

	// Wrappers
	static	NTSTATUS	IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);
	static	NTSTATUS	IoReadWrite(PDEVICE_OBJECT DeviceObject, PIRP Irp);
	static	NTSTATUS	IoCreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp);

	// Worker thread
	static KSTART_ROUTINE VirtualDiskThread;
	// IRP_MJ_DEVICE_CONTROL handler, in context of worker thread
	NTSTATUS	ThreadIoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);

	// Acessors
	const DISK_BASIC_INFO& GetDiskInfo() const {return *m_pDiskInfo;}
	size_t GetDiskInfoSize() const { return sizeof(DISK_BASIC_INFO) + m_pDiskInfo->PathSize - sizeof(WCHAR); }
	UINT64 ImageDataOffset() const { return m_imageDataOffset; }

protected:
	// Internal functions
	NTSTATUS SetFlags(DISK_ADD_INFO *pInfo);
	NTSTATUS OpenFile(PCWSTR fileName);
	NTSTATUS InitCipher(DISK_ADD_INFO *pInfo);

protected:
	BOOL				m_bInitialized;

	PDEVICE_OBJECT		m_pDevice;

	// Saved time
	FILE_BASIC_INFORMATION	m_fileBasicInfo;

	// Device parameters
	DISK_BASIC_INFO*	m_pDiskInfo;

	// Flags
	BOOL				m_bRemovable;
	BOOL				m_bReadOnly;
	BOOL				m_bMountMgr;	// used by DisksManager
	BOOL				m_bPreserveTimestamp;
	BOOL				m_bMountDevice;

	int					m_nDiskNumber;

	// Image parameters
	HANDLE				m_hImageFile;
	PFILE_OBJECT		m_pFileObject;
	UINT64				m_imageDataOffset;

	// Cryptographic parameters
	IDiskCipher*		m_pDiskCipher;

	// Worker thread and IRP queue
	PVOID				m_pWorkerThread;
	KSEMAPHORE			m_queueSemaphore;
	LIST_ENTRY			m_irpQueueHead;
	KSPIN_LOCK			m_irpQueueLock;
	BOOL				m_bTerminateThread;
};
