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

#ifndef	_VIRTUAL_DISK_H_INCLUDED
#define	_VIRTUAL_DISK_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Structures and constants, required for work in Windows XP, but not defined in DDK 2000
#include "XPStuff.h"

class	VirtualDisk
{
public:
	BOOL				Initialized;
	
	// Saved time
	FILE_BASIC_INFORMATION	fileBasicInfo;

	// Device parameters
	DISK_BASIC_INFO		DiskInfo;
	BOOL				bRemovable;
	BOOL				bReadOnly;
	BOOL				bMountMgr;	// used by DisksManager
	BOOL				bSaveTime;
	int					nDiskNumber;

	// Image parameters
	HANDLE				hImageFile;
	PFILE_OBJECT		pFileObject;

	// Cryptographic parameters
	DiskCipher			Cipher;

	// Worker thread and IRP queue
	PVOID				pWorkerThread;
	KSEMAPHORE			Semaphore;
	LIST_ENTRY			IrpQueueHead;
	KSPIN_LOCK			QueueLock;
	BOOL				bTerminateThread;

	// Generic functions
	NTSTATUS			Init(DISK_ADD_INFO *Info,PDEVICE_OBJECT pDevice);
	NTSTATUS			Close(BOOL bForce);

	// Initialization and cleanup from worker thread
	void				Cleanup();
	NTSTATUS			InternalInit(DISK_ADD_INFO *Info,PDEVICE_OBJECT pDevice);

	// Wrappers
	static	NTSTATUS	IoControl(PDEVICE_OBJECT DeviceObject,PIRP Irp);
	static	NTSTATUS	IoReadWrite(PDEVICE_OBJECT DeviceObject,PIRP Irp);
	static	NTSTATUS	IoCreateClose(PDEVICE_OBJECT DeviceObject,PIRP Irp);

	// Worker thread
	static	void __stdcall VirtualDiskThread(PVOID param);
	// IRP_MJ_DEVICE_CONTROL handler, in context of worker thread
	NTSTATUS	VirtualDiskIoControl(PDEVICE_OBJECT DeviceObject,PIRP Irp);
};

#endif	//_VIRTUAL_DISK_H_INCLUDED
