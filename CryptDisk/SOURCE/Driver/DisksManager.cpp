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

#include "..\Version.h"
#include "format.h"

#include "main.h"
#include "VirtualDisk.h"
#include "Tools.h"

#ifndef FSCTL_LOCK_VOLUME
#define FSCTL_LOCK_VOLUME               CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  6, METHOD_BUFFERED, FILE_ANY_ACCESS)
#endif	//FSCTL_LOCK_VOLUME

#ifndef	FSCTL_DISMOUNT_VOLUME
#define FSCTL_DISMOUNT_VOLUME           CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  8, METHOD_BUFFERED, FILE_ANY_ACCESS)
#endif	// FSCTL_DISMOUNT_VOLUME

NTSTATUS DisksManager::Init(DISKS_MANAGER_INIT_INFO *Info)
{
	if(Initialized)
	{
		return STATUS_UNSUCCESSFUL;
	}

	DisksCountMax=Info->MaxDisks;
	DisksCount=0;

	if(VirtualDisks=(PDEVICE_OBJECT*)ExAllocatePoolWithTag(NonPagedPool,
		DisksCountMax*sizeof(PDEVICE_OBJECT), MEM_TAG))
	{
		memset(VirtualDisks,0,DisksCountMax*sizeof(PDEVICE_OBJECT));
		Initialized=1;
		return STATUS_SUCCESS;
	}

	return STATUS_NO_MEMORY;
}

BOOLEAN DisksManager::GetDisksCount(DISKS_COUNT_INFO *Info)
{
	if(!Initialized)
	{
		return FALSE;
	}

	Info->DiskCount=DisksCount;
	Info->DiskCountMax=DisksCountMax;
	
	return TRUE;
}

NTSTATUS DisksManager::MountDisk(PUNICODE_STRING pusSymLinkName,PUNICODE_STRING pusDeviceName)
{
	NTSTATUS						status;

	UNICODE_STRING					usMountMgrName;
	PFILE_OBJECT					pFileObject;
	PDEVICE_OBJECT					pDeviceObject;

	PMOUNTMGR_TARGET_NAME			pTargetName;
	PMOUNTMGR_CREATE_POINT_INPUT	pCreatePoint;

	int								structSize;

	status=STATUS_UNSUCCESSFUL;

	if(!Initialized)
	{
		return STATUS_DEVICE_NOT_READY;
	}

	RtlInitUnicodeString(&usMountMgrName,MOUNTMGR_DEVICE_NAME);
	status=IoGetDeviceObjectPointer(&usMountMgrName,FILE_READ_ATTRIBUTES,
		&pFileObject,&pDeviceObject);

	if(NT_SUCCESS(status))
	{
		structSize=sizeof(pTargetName->DeviceNameLength)+pusDeviceName->Length;

		if(pTargetName=(PMOUNTMGR_TARGET_NAME)
			ExAllocatePoolWithTag(PagedPool, structSize+4*sizeof(WCHAR), MEM_TAG))
		{
			memset(pTargetName,0,structSize);
			pTargetName->DeviceNameLength=pusDeviceName->Length;
			memcpy(pTargetName->DeviceName,pusDeviceName->Buffer,pusDeviceName->Length);

			status=DNDeviceIoControl(pDeviceObject,IOCTL_MOUNTMGR_VOLUME_ARRIVAL_NOTIFICATION,
				pTargetName,structSize,NULL,0,NULL);

			ExFreePoolWithTag(pTargetName, MEM_TAG);
		}
		else
		{
			status=STATUS_INSUFFICIENT_RESOURCES;
		}

		if(NT_SUCCESS(status))
		{
			structSize=pusDeviceName->Length+
				pusSymLinkName->Length+
				sizeof(MOUNTMGR_CREATE_POINT_INPUT);

			if(pCreatePoint=(PMOUNTMGR_CREATE_POINT_INPUT)
				ExAllocatePoolWithTag(PagedPool, structSize+4*sizeof(WCHAR), MEM_TAG))
			{
				memset(pCreatePoint,0,structSize);

				pCreatePoint->SymbolicLinkNameOffset=sizeof(MOUNTMGR_CREATE_POINT_INPUT);
				pCreatePoint->SymbolicLinkNameLength=pusSymLinkName->Length;
				pCreatePoint->DeviceNameOffset=pCreatePoint->SymbolicLinkNameOffset+
					pCreatePoint->SymbolicLinkNameLength;
				pCreatePoint->DeviceNameLength=pusDeviceName->Length;

				memcpy(((char*)pCreatePoint)+pCreatePoint->SymbolicLinkNameOffset,
					pusSymLinkName->Buffer,pusSymLinkName->Length);
				memcpy(((char*)pCreatePoint)+pCreatePoint->DeviceNameOffset,
					pusDeviceName->Buffer,pusDeviceName->Length);

				status=DNDeviceIoControl(pDeviceObject,IOCTL_MOUNTMGR_CREATE_POINT,
					pCreatePoint,structSize,
					NULL,0,NULL);

				KdPrint(("\nCryptDisk: DisksManager::MountDisk info: IOCTL_MOUNTMGR_CREATE_POINT status %08X", status));

				ExFreePoolWithTag(pCreatePoint, MEM_TAG);
			}
			else
			{
				status=STATUS_INSUFFICIENT_RESOURCES;
			}

		}

		ObDereferenceObject(pFileObject);
	}

	return status;
}

NTSTATUS DisksManager::UnmountDisk(DISK_DELETE_INFO *Info)
{
	NTSTATUS				status=STATUS_UNSUCCESSFUL;
	WCHAR					wsSymLinkName[64];
	UNICODE_STRING			usSymLinkName;

	OBJECT_ATTRIBUTES		ob_attr;
	HANDLE					hVolume;
	PFILE_OBJECT			pVolumeFSObject;
	PDEVICE_OBJECT			pVolumeObject;
	IO_STATUS_BLOCK			iosb;

	UNICODE_STRING			usMountMgrName;
	PFILE_OBJECT			pMountMgrFileObject;
	PDEVICE_OBJECT			pMountMgrDeviceObject;
	int						structSize;
	PMOUNTMGR_MOUNT_POINT	pMountPoint;
	PVOID					pBuff;
	BOOL					bVolumeInUse=FALSE;

	memset(wsSymLinkName,0,sizeof(wsSymLinkName));

	DisksManager::GetDosName((static_cast<const VirtualDisk*>(VirtualDisks[Info->DiskId]->DeviceExtension))->GetDiskInfo().DriveLetter,
		wsSymLinkName);
	RtlInitUnicodeString(&usSymLinkName, wsSymLinkName);

	bVolumeInUse=FALSE;

	// Open FS volume
	InitializeObjectAttributes(&ob_attr, &usSymLinkName, OBJ_CASE_INSENSITIVE, NULL, NULL);
	memset(&iosb,0,sizeof(IO_STATUS_BLOCK));
	status=ZwCreateFile(&hVolume,SYNCHRONIZE|GENERIC_READ,&ob_attr,&iosb,0,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ|FILE_SHARE_WRITE,
		FILE_OPEN,
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,0);
	if(NT_SUCCESS(status))
	{
		status=ObReferenceObjectByHandle(hVolume,
			FILE_READ_DATA,
			NULL,
			KernelMode,
			(PVOID*)&pVolumeFSObject,NULL);
		if(NT_SUCCESS(status))
		{

			pVolumeObject=IoGetRelatedDeviceObject(pVolumeFSObject);

			// Lock volume
			status=DNDeviceFSControl(pVolumeObject,pVolumeFSObject,FSCTL_LOCK_VOLUME,
				0,0,0,0,NULL);
			KdPrint(("\nCryptDisk: DisksManager::UnmountDisk FSCTL_LOCK_VOLUME status=%08X",status));
			
			if(!NT_SUCCESS(status))
			{
				bVolumeInUse=TRUE;
			}

			// If lock volume succeeds there are no open files on this volume.
			// Otherwise we check if force unmount required.
			if(NT_SUCCESS(status) || Info->bForce)
			{
				// Dismont FS from this volume
				status=DNDeviceFSControl(pVolumeObject,pVolumeFSObject,FSCTL_DISMOUNT_VOLUME,
					0,0,0,0,NULL);
				KdPrint(("\nCryptDisk: DisksManager::UnmountDisk FSCTL_DISMOUNT_VOLUME status=%08X",status));

				// Perform MountMgr unmount
				RtlInitUnicodeString(&usMountMgrName,MOUNTMGR_DEVICE_NAME);
				status=IoGetDeviceObjectPointer(&usMountMgrName,FILE_READ_ATTRIBUTES,
					&pMountMgrFileObject,&pMountMgrDeviceObject);
				if(NT_SUCCESS(status))
				{
					structSize=sizeof(MOUNTMGR_MOUNT_POINT)+usSymLinkName.Length;
					pMountPoint=(PMOUNTMGR_MOUNT_POINT)ExAllocatePoolWithTag(PagedPool,
						structSize+2*sizeof(WCHAR), MEM_TAG);
					if(pMountPoint)
					{
						// Allocate buffer for output
						if(pBuff=ExAllocatePoolWithTag(PagedPool, PAGE_SIZE, MEM_TAG))
						{
							memset(pMountPoint,0,structSize+2*sizeof(WCHAR));

							pMountPoint->SymbolicLinkNameLength=usSymLinkName.Length;
							pMountPoint->SymbolicLinkNameOffset=sizeof(MOUNTMGR_MOUNT_POINT);
							memcpy(((PUCHAR)pMountPoint)+sizeof(MOUNTMGR_MOUNT_POINT),
								usSymLinkName.Buffer,usSymLinkName.Length);
							status=DNDeviceIoControl(pMountMgrDeviceObject,IOCTL_MOUNTMGR_DELETE_POINTS,
								pMountPoint,structSize,pBuff,PAGE_SIZE,NULL);
							KdPrint(("\nCryptDisk: DisksManager::UnmountDisk IOCTL_MOUNTMGR_DELETE_POINTS status=%08X",status));

							ExFreePoolWithTag(pBuff, MEM_TAG);
						}
						ExFreePoolWithTag(pMountPoint, MEM_TAG);
					}
					else
					{
						status=STATUS_INSUFFICIENT_RESOURCES;
					}

					ObDereferenceObject(pMountMgrFileObject);
				}
			}
			ObDereferenceObject(pVolumeFSObject);
		}
		ZwClose(hVolume);
	}

	// If MountMgr can't unmount this volume we try to delete symbolic link
	if(!bVolumeInUse || Info->bForce)
	{
		if(!NT_SUCCESS(status))
		{
			// If MountManager returns error, check status form IoDeleteSymbolicLink()
			status=IoDeleteSymbolicLink(&usSymLinkName);
		}
		else
		{
			// Call IoDeleteSymbolicLink() to delete local session link
			// Symbolic link in local session created if we mount
			// volume on new letter first time
			IoDeleteSymbolicLink(&usSymLinkName);
		}
	}
	
	return status;
}

NTSTATUS DisksManager::InstallDisk(DISK_ADD_INFO *Info)
{
	NTSTATUS			status;
	WCHAR				wsSymLinkName[64];
	UNICODE_STRING		usSymLinkName;

	WCHAR				wsDeviceName[64];
	UNICODE_STRING		usDeviceName;

	ULONG				i;

	OBJECT_ATTRIBUTES	attrib;
	HANDLE				hLink;
	
	ULONG				devChar;

	status=STATUS_UNSUCCESSFUL;

	KdPrint(("\nCryptDisk: DisksManager::InstallDisk enter"));

	if(!Initialized)
	{
		return STATUS_DEVICE_NOT_READY;
	}

	if( (Info->DriveLetter < L'A') || (Info->DriveLetter > L'Z'))
	{
		return STATUS_INVALID_PARAMETER;
	}

	memset(wsSymLinkName,0,sizeof(wsSymLinkName));
	memset(wsDeviceName,0,sizeof(wsSymLinkName));
	DisksManager::GetDosName(Info->DriveLetter,wsSymLinkName);
	DisksManager::GetNtName(Info->DriveLetter,wsDeviceName);
	RtlInitUnicodeString(&usSymLinkName,wsSymLinkName);
	RtlInitUnicodeString(&usDeviceName,wsDeviceName);

	// Check if disk already exists
	InitializeObjectAttributes(&attrib,&usSymLinkName,OBJ_KERNEL_HANDLE|OBJ_CASE_INSENSITIVE,NULL,NULL);
	status=ZwOpenSymbolicLinkObject(&hLink,GENERIC_READ,&attrib);
	if(NT_SUCCESS(status)) // must be STATUS_OBJECT_NAME_NOT_FOUND
	{
		ZwClose(hLink);
		KdPrint(("\nCryptDisk: DisksManager::InstallDisk error:STATUS_OBJECT_NAME_COLLISION"));
		return STATUS_OBJECT_NAME_COLLISION;
	}

	// Find free slot
	for(i=0;i<DisksCountMax;i++)
	{
		if(!VirtualDisks[i])
		{
			break;
		}
	}
	if(i>=DisksCountMax)
	{
		KdPrint(("\nCryptDisk: DisksManager::InstallDisk error:no free slots"));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	// Create device
	devChar=0;
	if(Info->MountOptions&MOUNT_AS_REMOVABLE)
	{
		devChar|=FILE_REMOVABLE_MEDIA;
	}
	if(Info->MountOptions&MOUNT_READ_ONLY)
	{
		devChar|=FILE_READ_ONLY_DEVICE;
	}

	status=IoCreateDevice(pDriverObj,sizeof(VirtualDisk),&usDeviceName,
		FILE_DEVICE_DISK,devChar,0,&VirtualDisks[i]);
	if(NT_SUCCESS(status))
	{
		VirtualDisks[i]->Flags|=DO_DIRECT_IO;
		VirtualDisks[i]->AlignmentRequirement=FILE_WORD_ALIGNMENT;

		// Init device

		memset(VirtualDisks[i]->DeviceExtension, 0, sizeof(VirtualDisk));

		((VirtualDisk*)(VirtualDisks[i]->DeviceExtension))->m_bInitialized=FALSE;
		((VirtualDisk*)(VirtualDisks[i]->DeviceExtension))->m_nDiskNumber=i;

		status=((VirtualDisk*)(VirtualDisks[i]->DeviceExtension))->Init(Info,VirtualDisks[i]);
		if(!NT_SUCCESS(status))
		{
			KdPrint(("\nCryptDisk: DisksManager::InstallDisk error: VirtualDisk->Init failed with status %08X", status));
			IoDeleteDevice(VirtualDisks[i]);
			VirtualDisks[i]=NULL;
		}
		else
		{
			// Clear DO_DEVICE_INITIALIZING flag
			VirtualDisks[i]->Flags&=~DO_DEVICE_INITIALIZING;

			if(Info->MountOptions&MOUNT_VIA_MOUNTMGR)
			{
				// Mount disk
				status=MountDisk(&usSymLinkName,&usDeviceName);

				((VirtualDisk*)(VirtualDisks[i]->DeviceExtension))->m_bMountMgr=TRUE;
				// If MountMgr fail to create symbolic link, we do it here
				if(!NT_SUCCESS(status))
				{
					KdPrint(("\nCryptDisk: DisksManager::InstallDisk error: MountDisk failed with status %08X", status));
					status=IoCreateSymbolicLink(&usSymLinkName,&usDeviceName);
					KdPrint(("\nCryptDisk: DisksManager::InstallDisk info: IoCreateSymbolicLink status %08X", status));
					((VirtualDisk*)(VirtualDisks[i]->DeviceExtension))->m_bMountMgr=FALSE;
				}
			}
			else
			{
				KdPrint(("\nCryptDisk: DisksManager::InstallDisk info: mounting without MountMgr"));
				status=IoCreateSymbolicLink(&usSymLinkName,&usDeviceName);
				KdPrint(("\nCryptDisk: DisksManager::InstallDisk info: IoCreateSymbolicLink status %08X", status));
				((VirtualDisk*)(VirtualDisks[i]->DeviceExtension))->m_bMountMgr=FALSE;
			}

			DisksCount++;
		}
	}

	KdPrint(("\nCryptDisk: DisksManager::InstallDisk return with status %08X", status));
	return status;
}

NTSTATUS DisksManager::UninstallDisk(DISK_DELETE_INFO *Info)
{
	NTSTATUS		status=STATUS_UNSUCCESSFUL;
	unsigned int	i;

	if(!Initialized)
	{
		return STATUS_DEVICE_NOT_READY;
	}

	// Is it our disk?
	for(i=0;i<DisksCountMax;i++)
	{
		if(VirtualDisks[i])
		{
			if(((VirtualDisk*)(VirtualDisks[i]->DeviceExtension))->m_nDiskNumber==Info->DiskId)
			{
				break;
			}
		}
	}
	if(i==DisksCountMax)
	{
		return STATUS_UNSUCCESSFUL;
	}

	// Unmount disk
	status=UnmountDisk(Info);
	if(NT_SUCCESS(status))
	{
		// Close VirtualDisk
		status=((VirtualDisk*)(VirtualDisks[i]->DeviceExtension))->Close(Info->bForce);
		if(NT_SUCCESS(status))
		{
			// Delete device
			IoDeleteDevice(VirtualDisks[i]);
			VirtualDisks[i]=NULL;

			DisksCount--;
		}
	}
	return status;
}

NTSTATUS DisksManager::Close(BOOL bForce)
{
	ULONG				i;
	BOOL				bAllDevicesDeleted;
	DISK_DELETE_INFO	info;
	NTSTATUS			status;

	if(!Initialized)
	{
		return STATUS_SUCCESS;
	}

	bAllDevicesDeleted=TRUE;

	for(i=0;i<DisksCountMax;i++)
	{
		if(VirtualDisks[i])
		{
			info.bForce=bForce;
			info.DiskId=((VirtualDisk*)(VirtualDisks[i]->DeviceExtension))->m_nDiskNumber;
			status=UninstallDisk(&info);
			if(!NT_SUCCESS(status))
			{
				bAllDevicesDeleted=FALSE;
			}
		}
	}

	if(bAllDevicesDeleted)
	{
		ExFreePoolWithTag(VirtualDisks, MEM_TAG);
	}

	Initialized=FALSE;
	return STATUS_SUCCESS;
}

NTSTATUS DisksManager::IoControl(PDEVICE_OBJECT DeviceObject,PIRP Irp)
{
	NTSTATUS			status=STATUS_INVALID_DEVICE_REQUEST;
	PIO_STACK_LOCATION	ioStack;
	void				*Buffer;
	ULONG				InputSize,OutputSize;

	ioStack=IoGetCurrentIrpStackLocation(Irp);
	Buffer=Irp->AssociatedIrp.SystemBuffer;
	InputSize=ioStack->Parameters.DeviceIoControl.InputBufferLength;
	OutputSize=ioStack->Parameters.DeviceIoControl.OutputBufferLength;

	if(!Initialized)
	{
		Irp->IoStatus.Information=0;
		Irp->IoStatus.Status=status;
		IofCompleteRequest(Irp, IO_NO_INCREMENT);
		return status;
	}

	switch(ioStack->Parameters.DeviceIoControl.IoControlCode)
	{
	case IOCTL_VDISK_GET_VERSION:
		if(Buffer)
		{
			if(OutputSize>=sizeof(VERSION_DRIVER))
			{
				((VERSION_DRIVER*)Buffer)->versionMinor=DRIVER_VERSION_MINOR;
				((VERSION_DRIVER*)Buffer)->versionMajor=DRIVER_VERSION_MAJOR;
				Irp->IoStatus.Information=sizeof(VERSION_DRIVER);
				status=STATUS_SUCCESS;
			}
			else
			{
				status=STATUS_BUFFER_TOO_SMALL;
				Irp->IoStatus.Information=sizeof(VERSION_DRIVER);
			}
		}
		break;
	case IOCTL_VDISK_GET_DISKS_COUNT:
		if(Buffer)
		{
			if(OutputSize>=sizeof(DISKS_COUNT_INFO))
			{
				GetDisksCount((DISKS_COUNT_INFO*)Buffer);
				Irp->IoStatus.Information=sizeof(DISKS_COUNT_INFO);
				status=STATUS_SUCCESS;
			}
			else
			{
				status=STATUS_BUFFER_TOO_SMALL;
				Irp->IoStatus.Information=sizeof(DISKS_COUNT_INFO);
			}
		}
		break;
	case IOCTL_VDISK_GET_DISKS_INFO:
		if(Buffer)
		{
			size_t infosSize = 0;
			for(ULONG i=0;i<DisksCountMax;i++)
			{
				if(VirtualDisks[i])
				{
					infosSize += ((const VirtualDisk*)(VirtualDisks[i]->DeviceExtension))->GetDiskInfoSize();
				}
			}

			if(OutputSize>=infosSize)
			{
				DISK_BASIC_INFO	*pInfo;

				pInfo = (DISK_BASIC_INFO*)Buffer;

				for(ULONG i=0,j=0;i<DisksCountMax;i++)
				{
					if(j >= DisksCount)
					{
						break;
					}
					if(VirtualDisks[i])
					{
						pInfo[j++] = ((VirtualDisk*)(VirtualDisks[i]->DeviceExtension))->GetDiskInfo();
					}
				}

				Irp->IoStatus.Information = infosSize;
				status=STATUS_SUCCESS;
			}
			else
			{
				status=STATUS_BUFFER_TOO_SMALL;
				Irp->IoStatus.Information = infosSize;
			}
		}
		break;
	case IOCTL_VDISK_ADD_DISK_V3:
	case IOCTL_VDISK_ADD_DISK_V4:
		if(Buffer)
		{
			if(InputSize>=sizeof(DISK_ADD_INFO))
			{
				size_t fullSize = sizeof(DISK_ADD_INFO) + ((DISK_ADD_INFO*)Buffer)->PathSize - sizeof(WCHAR);

				if(InputSize >= fullSize)
				{
					status=InstallDisk((DISK_ADD_INFO*)Buffer);
					Irp->IoStatus.Information=0;
				}
				else
				{
					status=STATUS_BUFFER_TOO_SMALL;
					Irp->IoStatus.Information=fullSize;
				}
			}
			else
			{
				status=STATUS_BUFFER_TOO_SMALL;
				Irp->IoStatus.Information=sizeof(DISK_ADD_INFO);
			}
		}
		break;
	case IOCTL_VDISK_DELETE_DISK:
		if(Buffer)
		{
			if(InputSize>=sizeof(DISK_DELETE_INFO))
			{
				status=UninstallDisk((DISK_DELETE_INFO*)Buffer);
				Irp->IoStatus.Information=0;
			}
			else
			{
				status=STATUS_BUFFER_TOO_SMALL;
				Irp->IoStatus.Information=sizeof(DISK_DELETE_INFO);
			}
		}
		break;
	}

	Irp->IoStatus.Status=status;
	IofCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS DisksManager::IoCreateClose(PDEVICE_OBJECT DeviceObject,PIRP Irp)
{
	Irp->IoStatus.Status=STATUS_SUCCESS;
	Irp->IoStatus.Information=0;
	IofCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}
