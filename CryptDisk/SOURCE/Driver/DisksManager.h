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

#include "DriverProtocol.h"

#define		MOUNTMGR_DEV_NAME		L"\\Device\\DNVirtualDisks\\DiskZ"
#define		MOUNTMGR_DOS_NAME		L"\\DosDevices\\Z:"
#define		MOUNTMGR_UNIQUE_ID		L"CryptDiskZ"

typedef	struct	DISKS_MANAGER_INIT_INFO
{
	ULONG				MaxDisks;
}DISKS_MANAGER_INIT_INFO;

class	DisksManager
{
public:
	BOOL				Initialized;
	
	ULONG				DisksCount;
	ULONG				DisksCountMax;
	PDEVICE_OBJECT		*VirtualDisks;


	NTSTATUS			Init(DISKS_MANAGER_INIT_INFO *Info);
	NTSTATUS			Close(bool bForce);

	BOOLEAN				GetDisksCount(DISKS_COUNT_INFO *Info);
	
	NTSTATUS			InstallDisk(DISK_ADD_INFO *Info);
	NTSTATUS			UninstallDisk(DISK_DELETE_INFO *Info);
	
	NTSTATUS			MountDisk(PUNICODE_STRING pusSymLinkName,PUNICODE_STRING pusDeviceName);
	NTSTATUS			UnmountDisk(DISK_DELETE_INFO *Info);

	NTSTATUS			IoControl(PDEVICE_OBJECT DeviceObject,PIRP Irp);
	NTSTATUS			IoCreateClose(PDEVICE_OBJECT DeviceObject,PIRP Irp);

	//  Returns length in bytes
	static int GetDosNameLength(WCHAR wcDriveLetter)
	{
		return sizeof(MOUNTMGR_DOS_NAME);
	}

	static int GetDosName(WCHAR wcDriveLetter,WCHAR * pBuff)
	{
		memcpy(pBuff,MOUNTMGR_DOS_NAME,sizeof(MOUNTMGR_DOS_NAME));
		pBuff[sizeof(MOUNTMGR_DOS_NAME)/2-3]=wcDriveLetter;
		return sizeof(MOUNTMGR_DOS_NAME);
	}

	//  Returns length in bytes
	static int GetNtNameLength(WCHAR wcDriveLetter)
	{
		return sizeof(MOUNTMGR_DEV_NAME);
	}

	static int GetNtName(WCHAR wcDriveLetter,WCHAR *pBuff)
	{
		memcpy(pBuff,MOUNTMGR_DEV_NAME,sizeof(MOUNTMGR_DEV_NAME));
		pBuff[sizeof(MOUNTMGR_DEV_NAME)/2-2]=wcDriveLetter;
		return sizeof(MOUNTMGR_DOS_NAME);
	}

	//  Returns length in bytes
	static int GetUniqueIdLength(WCHAR wcDriveLetter)
	{
		return sizeof(MOUNTMGR_UNIQUE_ID);
	}

	static int GetUniqueId(WCHAR wcDriveLetter,WCHAR *Buff)
	{
		memcpy(Buff,MOUNTMGR_UNIQUE_ID,sizeof(MOUNTMGR_UNIQUE_ID));
		Buff[sizeof(MOUNTMGR_UNIQUE_ID)/2-2]=wcDriveLetter;
		return sizeof(MOUNTMGR_UNIQUE_ID);
	}
};
