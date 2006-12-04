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

#ifdef POOL_TAGGING
#pragma message("************************************************************")
#pragma message("*** NT 4.0 incompatibility because of using pool tagging ***")
#pragma message("************************************************************")
#endif

#include "Common.h"

#define	MAX_DISKS		26

// private functions
static void DriverUnload(PDRIVER_OBJECT DriverObject);
static NTSTATUS DispatchControl(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
static NTSTATUS DispatchCreateClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
static NTSTATUS DispatchReadWrite(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);

// public vars
PDRIVER_OBJECT		pDriverObj;
PDEVICE_OBJECT		pControlDevice;
DisksManager		Manager;

// private vars
static	WCHAR			wsControlDeviceName[]=L"\\Device\\DNDiskControl30";
static	WCHAR			wsControlDeviceLink[]=L"\\??\\DNDiskControl30";
static	WCHAR			wsDirName[]=L"\\Device\\DNVirtualDisks";
static	UNICODE_STRING	usSymLink;
static	HANDLE			hDirectory;
static	BOOL			Initialized;


extern "C"
NTSTATUS __stdcall DriverEntry(PDRIVER_OBJECT DriverObj, PUNICODE_STRING RegistryPath)
{
	NTSTATUS				status;
	UNICODE_STRING			usDeviceName;
	UNICODE_STRING			usDirectoryName;
	int						init_step=0;
	OBJECT_ATTRIBUTES		object_attributes;
	DISKS_MANAGER_INIT_INFO	dm_info;

// #if	DBG
// 	__asm int 3;
// #endif

	DbgPrint("\nCryptDisk: Initializing...");
	DbgPrint("\nCryptDisk: Driver version: %d.%d",DRIVER_VERSION_MAJOR,
		DRIVER_VERSION_MINOR);

	pDriverObj=DriverObj;

	// Create control device
	
	RtlInitUnicodeString(&usSymLink,wsControlDeviceLink);
	RtlInitUnicodeString(&usDeviceName,wsControlDeviceName);

	status=IoCreateDevice(DriverObj,0,&usDeviceName,FILE_DEVICE_UNKNOWN,0,TRUE,&pControlDevice);
	if(NT_SUCCESS(status))
	{
		init_step++;
		status=IoCreateSymbolicLink(&usSymLink,&usDeviceName);
		if(NT_SUCCESS(status))
		{
			init_step++;

			// Create directory for virtual disks devices
			RtlInitUnicodeString(&usDirectoryName,wsDirName);
			InitializeObjectAttributes(&object_attributes,&usDirectoryName,
				OBJ_PERMANENT,
				NULL,NULL);
			status=ZwCreateDirectoryObject(&hDirectory,DIRECTORY_ALL_ACCESS,
				&object_attributes);
			if(NT_SUCCESS(status))
			{
				ZwMakeTemporaryObject(hDirectory);
				init_step++;
				// Init disks manager
				dm_info.MaxDisks=MAX_DISKS;
				status=Manager.Init(&dm_info);
				if(NT_SUCCESS(status))
				{
					init_step++;
					// Init driver controls
					pDriverObj->DriverUnload=DriverUnload;
					pDriverObj->MajorFunction[IRP_MJ_CLOSE]=
					pDriverObj->MajorFunction[IRP_MJ_CREATE]=DispatchCreateClose;
					pDriverObj->MajorFunction[IRP_MJ_DEVICE_CONTROL]=DispatchControl;
					pDriverObj->MajorFunction[IRP_MJ_READ]=
					pDriverObj->MajorFunction[IRP_MJ_WRITE]=DispatchReadWrite;

					Initialized=TRUE;
				}
			}
		}
	}
	if(!NT_SUCCESS(status))
	{
		// perform cleanup on error
		switch(init_step)
		{
		case 4:
			Manager.Close(TRUE);
		case 3:
			ZwClose(hDirectory);
		case 2:
			IoDeleteSymbolicLink(&usSymLink);
		case 1:
			IoDeleteDevice(pControlDevice);
		case 0:
			break;
		}
	}

	return status;
}

void DriverUnload(PDRIVER_OBJECT DriverObject)
{
	if(Initialized)
	{
		Manager.Close(TRUE);
		ZwMakeTemporaryObject(hDirectory);
		ZwClose(hDirectory);
		IoDeleteDevice(pControlDevice);
		IoDeleteSymbolicLink(&usSymLink);

		Initialized=FALSE;
	}
}

NTSTATUS DispatchControl(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	if(DeviceObject==pControlDevice)
	{
		return Manager.IoControl(DeviceObject,Irp);
	}

	return VirtualDisk::IoControl(DeviceObject,Irp);
}

NTSTATUS DispatchCreateClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	if(DeviceObject==pControlDevice)
	{
		return Manager.IoCreateClose(DeviceObject,Irp);
	}
	
	return VirtualDisk::IoCreateClose(DeviceObject,Irp);
}

NTSTATUS DispatchReadWrite(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	if(DeviceObject==pControlDevice)
	{
		Irp->IoStatus.Status=STATUS_INVALID_DEVICE_REQUEST;
		return STATUS_INVALID_DEVICE_REQUEST;
	}

	return VirtualDisk::IoReadWrite(DeviceObject,Irp);
}
