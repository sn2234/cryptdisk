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

#define		_WIN32_WINNT	0x0501
#define		WINVER			0x0501

#include "stdafx.h"

#include "Common.h"

static TCHAR DriverServiceName[]=_TEXT("CryptDisk30");
static TCHAR DriverControlDeviceName[]=_TEXT("\\\\.\\DNDiskControl30");

static HRESULT BroadcastVolumeDeviceChange(WPARAM notification, WCHAR DriveLetter);

//##ModelId=41D53B2701F3
BOOL DriverManager::Open()
{
	SC_HANDLE		hScm;
	SC_HANDLE		hDriverService;
	DWORD			dwResult;
	VERSION_DRIVER	version;
	BOOL			Result=FALSE;

	if(Initialized)
	{
		return FALSE;
	}

	if(hScm=OpenSCManager(NULL,NULL,GENERIC_EXECUTE))
	{
		if(hDriverService=OpenService(hScm,DriverServiceName,GENERIC_EXECUTE))
		{
			if(StartService(hDriverService,0,NULL)||
				(GetLastError()==ERROR_SERVICE_ALREADY_RUNNING))
			{
				if(DriverControl.Open(DriverControlDeviceName))
				{
					// Check version
					if(DriverControl.Control(IOCTL_VDISK_GET_VERSION,
						(unsigned char*)&version,sizeof(VERSION_DRIVER),
						&dwResult))
					{
						if((version.versionMajor==DRIVER_VERSION_MAJOR)&&
							(version.versionMinor==DRIVER_VERSION_MINOR))
						{
							Initialized=1;
							Result=TRUE;
						}
					}
				}
			}
			CloseServiceHandle(hDriverService);
		}
		CloseServiceHandle(hScm);
	}
	return Result;
}

//##ModelId=41D53B350243
BOOL DriverManager::Close()
{
	if(Initialized)
	{
		DriverControl.Close();
		Initialized=FALSE;
	}
	return TRUE;
}

//##ModelId=41D53B3C0301
BOOL DriverManager::GetDisksCount(DISKS_COUNT_INFO *Info)
{
	BOOL	Result=FALSE;
	DWORD	dwResult;

	if(Initialized)
	{
		if(DriverControl.Control(IOCTL_VDISK_GET_DISKS_COUNT,
			(unsigned char*)Info,sizeof(DISKS_COUNT_INFO),&dwResult))
		{
			Result=TRUE;
		}
	}
	
	return Result;
}

//##ModelId=41D53B4B0277
DWORD DriverManager::GetDisksInfo(DISK_BASIC_INFO* Buffer, int BufferSize)
{
	DWORD	dwResult=0;

	if(Initialized)
	{
		DriverControl.Control(IOCTL_VDISK_GET_DISKS_INFO,
			(unsigned char*)Buffer,BufferSize,&dwResult);
	}
	
	return dwResult;
}

//##ModelId=41D53B550375
BOOL DriverManager::InstallDisk(DISK_ADD_INFO* Info)
{
	BOOL	Result=FALSE;

	if(Initialized)
	{
		if(DriverControl.Control(IOCTL_VDISK_ADD_DISK,
			(unsigned char*)Info,sizeof(DISK_ADD_INFO)))
		{
			BroadcastVolumeDeviceChange(DBT_DEVICEARRIVAL, Info->DriveLetter);
			Result=TRUE;
		}
	}
	
	return Result;
}

//##ModelId=41D53B600385
BOOL DriverManager::UninstallDisk(DISK_DELETE_INFO* Info)
{
	BOOL	Result=FALSE;
	
	if(Initialized)
	{
		if(DriverControl.Control(IOCTL_VDISK_DELETE_DISK,
			(unsigned char*)Info,sizeof(DISK_DELETE_INFO)))
		{
			BroadcastVolumeDeviceChange(DBT_DEVICEREMOVECOMPLETE,Info->DriveLetter);
			Result=true;
		}
	}
	
	return Result;
}

//##ModelId=41D53BD80099
BOOL DriverManager::UnloadDriver()
{
	SC_HANDLE		hScm;
	SC_HANDLE		hDriverService;
	BOOL			Result=FALSE;
	SERVICE_STATUS	status;

	if(hScm=OpenSCManager(NULL,NULL,GENERIC_EXECUTE))
	{
		if(hDriverService=OpenService(hScm,DriverServiceName,GENERIC_EXECUTE))
		{
			if(ControlService(hDriverService,SERVICE_CONTROL_STOP,&status)||
				(GetLastError()==ERROR_SERVICE_NOT_ACTIVE))
			{
				Result=TRUE;
			}
			CloseServiceHandle(hDriverService);
		}
		CloseServiceHandle(hScm);
	}

	return Result;
}

HRESULT BroadcastVolumeDeviceChange(WPARAM notification, WCHAR DriveLetter)
{
	static DEV_BROADCAST_VOLUME MyDevHdr;
	DWORD dwFlag = BSM_ALLCOMPONENTS;
	DWORD volumeMask = 1 << (DriveLetter - L'A');

	// Initialize our broadcast header
	MyDevHdr.dbcv_devicetype = DBT_DEVTYP_VOLUME;
	MyDevHdr.dbcv_size = sizeof(DEV_BROADCAST_VOLUME);
	MyDevHdr.dbcv_flags = DBTF_NET;
	MyDevHdr.dbcv_unitmask = volumeMask;

	// Broadcast the device change notification
	BroadcastSystemMessage(BSF_IGNORECURRENTTASK,
		&dwFlag,
		WM_DEVICECHANGE,
		notification,
		(LPARAM)&MyDevHdr);

	return S_OK;
}