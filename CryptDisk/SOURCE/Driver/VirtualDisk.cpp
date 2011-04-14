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

#include "DriverProtocol.h"
#include "DiskHeaderTools.h"

#include "VirtualDisk.h"
#include "DisksManager.h"
#include "DiskCipherAesV3.h"
#include "DiskCipherTwofishV3.h"
#include "DiskCipherV4.h"
#include "Tools.h"

#include <BaseCrypt\RijndaelEngine.h>
#include <BaseCrypt\TwofishEngine.h>

using namespace CryptoLib;

extern "C"
NTSYSAPI
NTSTATUS
NTAPI
ZwWaitForSingleObject (
					   IN HANDLE           Handle,
					   IN BOOLEAN          Alertable,
					   IN PLARGE_INTEGER   Timeout OPTIONAL
					   );

#define	CACHE_BUFF_SIZE		0x10000

typedef	struct VDISK_THREAD_INIT_PARAMS
{
	DISK_ADD_INFO*	Info;
	PDEVICE_OBJECT	pDevice;
	NTSTATUS		statusReturned;
	KEVENT			syncEvent;
}VDISK_THREAD_INIT_PARAMS;

NTSTATUS	VirtualDisk::Init(DISK_ADD_INFO *Info,PDEVICE_OBJECT pDevice)
{
	VDISK_THREAD_INIT_PARAMS	threadParams;
	HANDLE						hThread;
	NTSTATUS					status=STATUS_UNSUCCESSFUL;

	threadParams.Info=Info;
	threadParams.pDevice=pDevice;
	threadParams.statusReturned=STATUS_UNSUCCESSFUL;
	KeInitializeEvent(&threadParams.syncEvent,NotificationEvent,0);

	status=PsCreateSystemThread(&hThread,0,0,0,0,&VirtualDiskThread,&threadParams);
	if(NT_SUCCESS(status))
	{
		status=ObReferenceObjectByHandle(hThread,THREAD_ALL_ACCESS,
			0,KernelMode,&m_pWorkerThread,NULL);
		if(NT_SUCCESS(status))
		{
			KeWaitForSingleObject(&threadParams.syncEvent,Executive,
				KernelMode,FALSE,0);
			status=threadParams.statusReturned;
			if(!NT_SUCCESS(status))
			{
				ObDereferenceObject(m_pWorkerThread);
			}
		}
		ZwClose(hThread);
	}
	return status;
}

NTSTATUS VirtualDisk::InternalInit(DISK_ADD_INFO *pInfo, PDEVICE_OBJECT pDevice)
{
	NTSTATUS					status=STATUS_UNSUCCESSFUL;
	IO_STATUS_BLOCK				io_status;
	OBJECT_ATTRIBUTES			obj_attr;
	UNICODE_STRING				usFileName;
	LARGE_INTEGER				Offset;
	FILE_STANDARD_INFORMATION	file_info;
	PDEVICE_OBJECT				pFSD;

	if(m_bInitialized)
	{
		return STATUS_UNSUCCESSFUL;
	}

	m_pDiskInfo = (DISK_BASIC_INFO*)ExAllocatePoolWithTag(PagedPool, sizeof(DISK_BASIC_INFO) + pInfo->PathSize - sizeof(WCHAR), MEM_TAG);

	m_pDiskInfo->OpenCount=0;
	// Set drive letter
	m_pDiskInfo->DriveLetter=pInfo->DriveLetter;
	m_pDiskInfo->DiskId = m_nDiskNumber;
	m_pDiskInfo->PathSize = pInfo->PathSize;

	m_pDevice=pDevice;
	m_pFileObject=NULL;

	// Copy file name
	RtlStringCbCopyW(m_pDiskInfo->FilePath, pInfo->PathSize, pInfo->FilePath);

	status = SetFlags(pInfo);

	if(!NT_SUCCESS(status))
	{
		ExFreePoolWithTag(m_pDiskInfo, MEM_TAG);
		return status;
	}

	status = OpenFile(pInfo->FilePath);

	if(!NT_SUCCESS(status))
	{
		ExFreePoolWithTag(m_pDiskInfo, MEM_TAG);
		return status;
	}

	status = InitCipher(pInfo);

	if(!NT_SUCCESS(status))
	{
		ExFreePoolWithTag(m_pDiskInfo, MEM_TAG);
		ObDereferenceObject(m_pFileObject);
		ZwClose(m_hImageFile);

		return status;
	}

	// Success initialization
	InitializeListHead(&m_irpQueueHead);
	KeInitializeSemaphore(&m_queueSemaphore,0,10000);
	KeInitializeSpinLock(&m_irpQueueLock);

	m_bInitialized=TRUE;

	return status;
}

NTSTATUS VirtualDisk::Close(bool bForce)
{
	NTSTATUS			status=STATUS_UNSUCCESSFUL;

	if(!m_bInitialized)
	{
		return STATUS_SUCCESS;
	}
	KdPrint(("\nVirtualDisk::Close()"));

	m_bTerminateThread=TRUE;
	KeReleaseSemaphore(&m_queueSemaphore,0,1,TRUE);
	KeWaitForSingleObject(m_pWorkerThread,Executive,KernelMode,FALSE,0);
	ObDereferenceObject(m_pWorkerThread);

	m_pDiskCipher->~IDiskCipher();
	ExFreePoolWithTag(m_pDiskCipher, MEM_TAG);

	m_bInitialized=FALSE;

	status=STATUS_SUCCESS;

	return status;
}

NTSTATUS VirtualDisk::IoReadWrite(PDEVICE_OBJECT DeviceObject,PIRP Irp)
{
	VirtualDisk			*pDisk;
	NTSTATUS			status=STATUS_INVALID_DEVICE_STATE;
	PIO_STACK_LOCATION	ioStack;
	ULONG				RequestLength;
	LARGE_INTEGER		FileOffset;

	pDisk=(VirtualDisk*)(DeviceObject->DeviceExtension);
	if(!pDisk->m_bInitialized)
	{
		Irp->IoStatus.Information=0;
		Irp->IoStatus.Status=status;
		IofCompleteRequest(Irp, IO_NO_INCREMENT);
		return status;
	}

	ioStack=IoGetCurrentIrpStackLocation(Irp);
	status=STATUS_INVALID_DEVICE_REQUEST;

	if(ioStack)
	{
		RequestLength=ioStack->Parameters.Read.Length;
		FileOffset.QuadPart=ioStack->Parameters.Read.ByteOffset.QuadPart;
		// Check for alignment
		if(!(RequestLength&(BYTES_PER_SECTOR-1)))
		{
			if(!(FileOffset.QuadPart&(BYTES_PER_SECTOR-1)))
			{
				IoMarkIrpPending(Irp);
				status=STATUS_PENDING;

				ExInterlockedInsertTailList(
					&pDisk->m_irpQueueHead,
					&Irp->Tail.Overlay.ListEntry,
					&pDisk->m_irpQueueLock);
				KeReleaseSemaphore(&pDisk->m_queueSemaphore,8,1,FALSE);
			}
		}
	}

	return status;
}

NTSTATUS VirtualDisk::IoCreateClose(PDEVICE_OBJECT DeviceObject,PIRP Irp)
{
	VirtualDisk			*pDisk;
	PIO_STACK_LOCATION	ioStack;
	NTSTATUS			status=STATUS_INVALID_DEVICE_REQUEST;

	pDisk=(VirtualDisk*)(DeviceObject->DeviceExtension);
	if(!pDisk->m_bInitialized)
	{
		Irp->IoStatus.Information=0;
		Irp->IoStatus.Status=status;
		IofCompleteRequest(Irp, IO_NO_INCREMENT);
		return status;
	}

	ioStack=IoGetCurrentIrpStackLocation(Irp);
	switch(ioStack->MajorFunction)
	{
	case IRP_MJ_CREATE:
		DnPrint("Create");
		pDisk->m_pDiskInfo->OpenCount++;
		status=STATUS_SUCCESS;
		break;
	case IRP_MJ_CLOSE:
		if(pDisk->m_pDiskInfo->OpenCount)
		{
			DnPrint("Close");
			pDisk->m_pDiskInfo->OpenCount--;
			status=STATUS_SUCCESS;
		}
#if DBG
		else
		{
			DnPrint("Warning. Trying to close while not opened");
		}
#endif
		break;
	}
	
	Irp->IoStatus.Status=status;
	Irp->IoStatus.Information=FILE_OPENED;
	IofCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}


NTSTATUS	VirtualDisk::ThreadIoControl(PDEVICE_OBJECT DeviceObject,PIRP Irp)
{
	NTSTATUS			status=STATUS_INVALID_DEVICE_REQUEST;
	PIO_STACK_LOCATION	ioStack;
	void				*Buffer;
	ULONG				InputSize,OutputSize;
	VirtualDisk			*pDisk;
	ULONG				sizeRequired;

	pDisk=(VirtualDisk*)(DeviceObject->DeviceExtension);
	if(!pDisk->m_bInitialized)
	{
		Irp->IoStatus.Information=0;
		Irp->IoStatus.Status=status;
		IofCompleteRequest(Irp, IO_NO_INCREMENT);
		return status;
	}

	ioStack=IoGetCurrentIrpStackLocation(Irp);
	Buffer=Irp->AssociatedIrp.SystemBuffer;
	InputSize=ioStack->Parameters.DeviceIoControl.InputBufferLength;
	OutputSize=ioStack->Parameters.DeviceIoControl.OutputBufferLength;

	KdPrint(("\nCryptDisk: IoControl, Code=%08X",ioStack->Parameters.DeviceIoControl.IoControlCode));
	switch(ioStack->Parameters.DeviceIoControl.IoControlCode)
	{
	case IOCTL_DISK_IS_WRITABLE:
		status=Irp->IoStatus.Status=
			pDisk->m_bReadOnly ? STATUS_MEDIA_WRITE_PROTECTED : STATUS_SUCCESS;
		Irp->IoStatus.Information=0;
		break;

	case IOCTL_DISK_VERIFY:
	case IOCTL_DISK_CHECK_VERIFY:
	case IOCTL_STORAGE_CHECK_VERIFY:
	case IOCTL_STORAGE_CHECK_VERIFY2:
		status=Irp->IoStatus.Status=STATUS_SUCCESS;
		Irp->IoStatus.Information=0;
		break;

	case IOCTL_DISK_MEDIA_REMOVAL:
	case IOCTL_STORAGE_MEDIA_REMOVAL:
		status = STATUS_SUCCESS;
		Irp->IoStatus.Information = 0;
		break;

	case IOCTL_DISK_FORMAT_TRACKS:
		status=STATUS_SUCCESS;
		break;

	case IOCTL_DISK_GET_MEDIA_TYPES:
	case IOCTL_DISK_GET_DRIVE_GEOMETRY:
		if(OutputSize>=sizeof(DISK_GEOMETRY))
		{
			// Fill information

			((PDISK_GEOMETRY)Buffer)->BytesPerSector = BYTES_PER_SECTOR;
			((PDISK_GEOMETRY)Buffer)->Cylinders.QuadPart =
				pDisk->m_pDiskInfo->FileSize.QuadPart >> 15;
			((PDISK_GEOMETRY)Buffer)->MediaType =
				pDisk->m_bRemovable ? RemovableMedia : FixedMedia;
			((PDISK_GEOMETRY)Buffer)->SectorsPerTrack =
				SECTORS_PER_TRACK;
			((PDISK_GEOMETRY)Buffer)->TracksPerCylinder =
				TRACKS_PER_CYLINDER;

			status=STATUS_SUCCESS;
			Irp->IoStatus.Information = sizeof(DISK_GEOMETRY);
		}
		else
		{
			status=STATUS_BUFFER_OVERFLOW;
			Irp->IoStatus.Information = sizeof(DISK_GEOMETRY);
		}
		break;

	case IOCTL_DISK_GET_PARTITION_INFO:
		if(OutputSize>=sizeof(PARTITION_INFORMATION))
		{
			((PPARTITION_INFORMATION)Buffer)->BootIndicator = FALSE;
			((PPARTITION_INFORMATION)Buffer)->HiddenSectors = 1;
			((PPARTITION_INFORMATION)Buffer)->PartitionLength.QuadPart =
				pDisk->m_pDiskInfo->FileSize.QuadPart-sizeof(DISK_HEADER_V4);
			((PPARTITION_INFORMATION)Buffer)->PartitionNumber = -1;
			((PPARTITION_INFORMATION)Buffer)->PartitionType = 0;
			((PPARTITION_INFORMATION)Buffer)->RecognizedPartition = TRUE;
			((PPARTITION_INFORMATION)Buffer)->RewritePartition = 0;
			((PPARTITION_INFORMATION)Buffer)->StartingOffset.QuadPart = 0;

			status=STATUS_SUCCESS;
			Irp->IoStatus.Information=sizeof(PARTITION_INFORMATION);
		}
		else
		{
			status=STATUS_BUFFER_OVERFLOW;
			Irp->IoStatus.Information=sizeof(PARTITION_INFORMATION);
		}
		break;
	case IOCTL_MOUNTDEV_QUERY_DEVICE_NAME:
		{
			if(OutputSize<sizeof(MOUNTDEV_NAME))
			{
				Irp->IoStatus.Information=sizeof(MOUNTDEV_NAME);
				status=STATUS_BUFFER_TOO_SMALL;

				break;
			}

			WCHAR			buff[64];
			UNICODE_STRING	usDeviceName;

			memset(buff,0,sizeof(buff));
			DisksManager::GetNtName(pDisk->m_pDiskInfo->DriveLetter,buff);
			RtlInitUnicodeString(&usDeviceName,buff);

			sizeRequired=usDeviceName.Length+sizeof(((PMOUNTDEV_NAME)Buffer)->NameLength);

			((PMOUNTDEV_NAME)Buffer)->NameLength=usDeviceName.Length;
			KdPrint(("\nCryptDisk: IOCTL_MOUNTDEV_QUERY_DEVICE_NAME DeviceName=%S Length=%d sizeRequired=%d",
				buff,usDeviceName.Length,sizeRequired));
			if(OutputSize>=sizeRequired)
			{
				memcpy((((PMOUNTDEV_NAME)Buffer)->Name),
					usDeviceName.Buffer,usDeviceName.Length);

				((PMOUNTDEV_NAME)Buffer)->NameLength=usDeviceName.Length;

				Irp->IoStatus.Information=sizeRequired;
				status=STATUS_SUCCESS;
			}
			else
			{
				Irp->IoStatus.Information=sizeof(MOUNTDEV_NAME);
				status=STATUS_BUFFER_OVERFLOW;
			}
		}
		break;

	case IOCTL_MOUNTDEV_QUERY_SUGGESTED_LINK_NAME:
		{
			if(OutputSize<sizeof(MOUNTDEV_SUGGESTED_LINK_NAME))
			{
				Irp->IoStatus.Information=sizeof(MOUNTDEV_SUGGESTED_LINK_NAME);
				status=STATUS_BUFFER_TOO_SMALL;

				break;
			}

			WCHAR			buff[64];
			UNICODE_STRING	usSymLink;

			memset(buff,0,sizeof(buff));
			DisksManager::GetDosName(pDisk->m_pDiskInfo->DriveLetter, buff);
			RtlInitUnicodeString(&usSymLink,buff);

			sizeRequired=usSymLink.Length
				+sizeof(((PMOUNTDEV_SUGGESTED_LINK_NAME)Buffer)->UseOnlyIfThereAreNoOtherLinks)
				+sizeof(((PMOUNTDEV_SUGGESTED_LINK_NAME)Buffer)->NameLength);

			((PMOUNTDEV_SUGGESTED_LINK_NAME)Buffer)->NameLength=usSymLink.Length;

			KdPrint(("\nCryptDisk: IOCTL_MOUNTDEV_QUERY_SUGGESTED_LINK_NAME DeviceName=%S Length=%d sizeRequired=%d",
				buff,usSymLink.Length,sizeRequired));

			if(OutputSize>=sizeRequired)
			{
//				memset(Buffer,0,OutputSize);

				memcpy((((PMOUNTDEV_SUGGESTED_LINK_NAME)Buffer)->Name),
					usSymLink.Buffer,usSymLink.Length);

				((PMOUNTDEV_SUGGESTED_LINK_NAME)Buffer)->UseOnlyIfThereAreNoOtherLinks=FALSE;
				((PMOUNTDEV_SUGGESTED_LINK_NAME)Buffer)->NameLength=usSymLink.Length;

				Irp->IoStatus.Information=sizeRequired;
				status=STATUS_SUCCESS;
			}
			else
			{
				Irp->IoStatus.Information=sizeof(MOUNTDEV_SUGGESTED_LINK_NAME);
				status=STATUS_BUFFER_OVERFLOW;
			}
		}
		break;

	case IOCTL_MOUNTDEV_QUERY_UNIQUE_ID:
		if(OutputSize<sizeof(MOUNTDEV_UNIQUE_ID))
		{
			Irp->IoStatus.Information=sizeof(MOUNTDEV_UNIQUE_ID);
			status=STATUS_BUFFER_TOO_SMALL;

			break;
		}

		WCHAR			buff[64];
		UNICODE_STRING	usUniqueId;

		memset(buff,0,sizeof(buff));
		DisksManager::GetUniqueId(pDisk->m_pDiskInfo->DriveLetter, buff);
		RtlInitUnicodeString(&usUniqueId,buff);

		sizeRequired=usUniqueId.Length
			+sizeof(((PMOUNTDEV_UNIQUE_ID)Buffer)->UniqueIdLength);

		((PMOUNTDEV_UNIQUE_ID)Buffer)->UniqueIdLength=usUniqueId.Length;
		KdPrint(("\nCryptDisk: IOCTL_MOUNTDEV_QUERY_UNIQUE_ID DeviceName=%S Length=%d sizeRequired=%d",
			buff,usUniqueId.Length,sizeRequired));
		if(OutputSize>=sizeRequired)
		{
			((PMOUNTDEV_UNIQUE_ID)Buffer)->UniqueIdLength=usUniqueId.Length;
			memcpy(((PMOUNTDEV_UNIQUE_ID)Buffer)->UniqueId,
				usUniqueId.Buffer,usUniqueId.Length);

			Irp->IoStatus.Information=sizeRequired;
			status=STATUS_SUCCESS;
		}
		else
		{
			Irp->IoStatus.Information=sizeof(MOUNTDEV_UNIQUE_ID);
			status=STATUS_BUFFER_OVERFLOW;
		}
		break;

	case IOCTL_DISK_GET_LENGTH_INFO:
		if(OutputSize>=sizeof(GET_LENGTH_INFORMATION))
		{
			((PGET_LENGTH_INFORMATION)Buffer)->Length.QuadPart=
				pDisk->m_pDiskInfo->FileSize.QuadPart-sizeof(DISK_HEADER_V4);
			Irp->IoStatus.Information=sizeof(GET_LENGTH_INFORMATION);
			status=STATUS_SUCCESS;
		}
		else
		{
			status=STATUS_BUFFER_OVERFLOW;
			Irp->IoStatus.Information=sizeof(GET_LENGTH_INFORMATION);
		}
		break;
	case IOCTL_DISK_GET_PARTITION_INFO_EX:
		if(OutputSize>=sizeof(PARTITION_INFORMATION_EX))
		{
			((PPARTITION_INFORMATION_EX)Buffer)->PartitionStyle=PARTITION_STYLE_MBR;
			((PPARTITION_INFORMATION_EX)Buffer)->StartingOffset.QuadPart=0;
			((PPARTITION_INFORMATION_EX)Buffer)->PartitionLength.QuadPart=
				pDisk->m_pDiskInfo->FileSize.QuadPart-sizeof(DISK_HEADER_V4);
			((PPARTITION_INFORMATION_EX)Buffer)->PartitionNumber=-1;
			((PPARTITION_INFORMATION_EX)Buffer)->RewritePartition=FALSE;
			((PPARTITION_INFORMATION_EX)Buffer)->Mbr.PartitionType=PARTITION_FAT_16;
			((PPARTITION_INFORMATION_EX)Buffer)->Mbr.BootIndicator=FALSE;
			((PPARTITION_INFORMATION_EX)Buffer)->Mbr.RecognizedPartition=TRUE;
			((PPARTITION_INFORMATION_EX)Buffer)->Mbr.HiddenSectors=1;

			Irp->IoStatus.Information=sizeof(PARTITION_INFORMATION_EX);
			status=STATUS_SUCCESS;
		}
		else
		{
			status=STATUS_BUFFER_OVERFLOW;
			Irp->IoStatus.Information=sizeof(PARTITION_INFORMATION_EX);
		}
		break;
	case IOCTL_DISK_GET_DRIVE_LAYOUT_EX:
		if(OutputSize>=sizeof(DRIVE_LAYOUT_INFORMATION_EX))
		{
			((PDRIVE_LAYOUT_INFORMATION_EX)Buffer)->PartitionStyle=PARTITION_STYLE_MBR;
			((PDRIVE_LAYOUT_INFORMATION_EX)Buffer)->PartitionCount=1;
			((PDRIVE_LAYOUT_INFORMATION_EX)Buffer)->Mbr.Signature=12345678;

			((PDRIVE_LAYOUT_INFORMATION_EX)Buffer)->PartitionEntry[0].PartitionStyle=PARTITION_STYLE_MBR;
			((PDRIVE_LAYOUT_INFORMATION_EX)Buffer)->PartitionEntry[0].StartingOffset.QuadPart=0;
			((PDRIVE_LAYOUT_INFORMATION_EX)Buffer)->PartitionEntry[0].PartitionLength.QuadPart=
				pDisk->m_pDiskInfo->FileSize.QuadPart-sizeof(DISK_HEADER_V4);
			((PDRIVE_LAYOUT_INFORMATION_EX)Buffer)->PartitionEntry[0].PartitionNumber=-1;
			((PDRIVE_LAYOUT_INFORMATION_EX)Buffer)->PartitionEntry[0].RewritePartition=FALSE;
			((PDRIVE_LAYOUT_INFORMATION_EX)Buffer)->PartitionEntry[0].Mbr.BootIndicator=FALSE;
			((PDRIVE_LAYOUT_INFORMATION_EX)Buffer)->PartitionEntry[0].Mbr.PartitionType=PARTITION_FAT_16;
			((PDRIVE_LAYOUT_INFORMATION_EX)Buffer)->PartitionEntry[0].Mbr.RecognizedPartition=TRUE;
			((PDRIVE_LAYOUT_INFORMATION_EX)Buffer)->PartitionEntry[0].Mbr.HiddenSectors=1;

			Irp->IoStatus.Information=sizeof(DRIVE_LAYOUT_INFORMATION_EX);
			status=STATUS_SUCCESS;
		}
		else
		{
			status=STATUS_BUFFER_OVERFLOW;
			Irp->IoStatus.Information=sizeof(DRIVE_LAYOUT_INFORMATION_EX);
		}
		break;
	case IOCTL_DISK_GET_DRIVE_LAYOUT:
		if(OutputSize>=sizeof(DRIVE_LAYOUT_INFORMATION))
		{
			((PDRIVE_LAYOUT_INFORMATION)Buffer)->PartitionCount=1;
			((PDRIVE_LAYOUT_INFORMATION)Buffer)->Signature=12345678;
			((PDRIVE_LAYOUT_INFORMATION)Buffer)->PartitionEntry[0].PartitionLength.QuadPart=
				pDisk->m_pDiskInfo->FileSize.QuadPart-sizeof(DISK_HEADER_V4);
			((PDRIVE_LAYOUT_INFORMATION)Buffer)->PartitionEntry[0].PartitionNumber=-1;
			((PDRIVE_LAYOUT_INFORMATION)Buffer)->PartitionEntry[0].RewritePartition=FALSE;
			((PDRIVE_LAYOUT_INFORMATION)Buffer)->PartitionEntry[0].StartingOffset.QuadPart=0;
			((PDRIVE_LAYOUT_INFORMATION)Buffer)->PartitionEntry[0].BootIndicator=FALSE;
			((PDRIVE_LAYOUT_INFORMATION)Buffer)->PartitionEntry[0].HiddenSectors=1;
			((PDRIVE_LAYOUT_INFORMATION)Buffer)->PartitionEntry[0].PartitionType=PARTITION_FAT_16;
			((PDRIVE_LAYOUT_INFORMATION)Buffer)->PartitionEntry[0].RecognizedPartition=TRUE;
			((PDRIVE_LAYOUT_INFORMATION)Buffer)->PartitionEntry[0].RewritePartition=FALSE;

			Irp->IoStatus.Information=sizeof(DRIVE_LAYOUT_INFORMATION);
			status=STATUS_SUCCESS;
		}
		else
		{
			status=STATUS_BUFFER_OVERFLOW;
			Irp->IoStatus.Information=sizeof(DRIVE_LAYOUT_INFORMATION);
		}
		break;

	default:
		KdPrint(("\nCryptDisk:IoControl fail.Reason: function is not supported (%08X)",
			ioStack->Parameters.DeviceIoControl.IoControlCode));
		break;
	}
	return status;
}
NTSTATUS VirtualDisk::IoControl(PDEVICE_OBJECT DeviceObject,PIRP Irp)
{
	VirtualDisk			*pDisk;
	NTSTATUS			status=STATUS_INVALID_DEVICE_STATE;

	pDisk=(VirtualDisk*)(DeviceObject->DeviceExtension);
	if(!pDisk->m_bInitialized)
	{
		Irp->IoStatus.Information=0;
		Irp->IoStatus.Status=status;
		IofCompleteRequest(Irp, IO_NO_INCREMENT);
		return status;
	}

	status=STATUS_INVALID_DEVICE_REQUEST;

	IoMarkIrpPending(Irp);
	status=STATUS_PENDING;

	ExInterlockedInsertTailList(
		&pDisk->m_irpQueueHead,
		&Irp->Tail.Overlay.ListEntry,
		&pDisk->m_irpQueueLock);
	KeReleaseSemaphore(&pDisk->m_queueSemaphore,0,1,FALSE);

	return status;
}

void VirtualDisk::Cleanup()
{
	NTSTATUS			status;

	if(!m_bInitialized)
	{
		return;
	}
	KdPrint(("\nVirtualDisk::Cleanup()"));

	ExFreePoolWithTag(m_pDiskInfo, MEM_TAG);

	ObDereferenceObject(m_pFileObject);
	if(m_bPreserveTimestamp)
	{
		IO_STATUS_BLOCK	io_status;

		status=ZwSetInformationFile(m_hImageFile, &io_status, &m_fileBasicInfo,
			sizeof(FILE_BASIC_INFORMATION), FileBasicInformation);
		if(!NT_SUCCESS(status))
		{
			KdPrint(("\nCryptDisk: VirtualDisk::Cleanup ZwSetInformationFile status: %08X", status));
		}
	}
	ZwClose(m_hImageFile);
}

void __stdcall VirtualDisk::VirtualDiskThread(PVOID param)
{
	PIRP						Irp;
	PLIST_ENTRY					Element;

	PDEVICE_OBJECT				pDeviceObject;
	VirtualDisk					*pDisk;
	PIO_STACK_LOCATION			ioStack;
	NTSTATUS					status=STATUS_UNSUCCESSFUL;
	ULONG						RequestLength;
	LARGE_INTEGER				FileOffset;
	void						*pDataBuff;
	IO_STATUS_BLOCK				IoStatus;
	ULONG						Sectors;
	ULONG						beginSector;
	VDISK_THREAD_INIT_PARAMS	*threadParams;
	UCHAR						*cacheBuff;

	// Initialization
	if(!(threadParams=(VDISK_THREAD_INIT_PARAMS*)param))
	{
		PsTerminateSystemThread(status);
	}

	pDeviceObject=threadParams->pDevice ;
	pDisk=(VirtualDisk*)(pDeviceObject->DeviceExtension);
	
	// Allocate cache buffer
	cacheBuff=(UCHAR*)ExAllocatePoolWithTag(PagedPool, CACHE_BUFF_SIZE, MEM_TAG);
	if(!cacheBuff)
	{
		threadParams->statusReturned=STATUS_INSUFFICIENT_RESOURCES;
		KeSetEvent(&(threadParams->syncEvent),0,FALSE);
		PsTerminateSystemThread(status);
	}

	// Perform VirtualDisk initialization
	status=pDisk->InternalInit(threadParams->Info,
		threadParams->pDevice);

	threadParams->statusReturned=status;
	KeSetEvent(&(threadParams->syncEvent),0,FALSE);

	if(!NT_SUCCESS(status))
	{
		ExFreePoolWithTag(cacheBuff, MEM_TAG);
		PsTerminateSystemThread(status);
	}

	KeSetPriorityThread(KeGetCurrentThread(),LOW_REALTIME_PRIORITY);
	
	// Main loop
	for(;;)
	{
		KeWaitForSingleObject(&pDisk->m_queueSemaphore,Executive,KernelMode,
			FALSE,0);
		if(pDisk->m_bTerminateThread)
		{
			pDisk->Cleanup();
			ExFreePoolWithTag(cacheBuff, MEM_TAG);
			PsTerminateSystemThread(STATUS_SUCCESS);
		}

		// Get IRP from queue
		while(Element=ExInterlockedRemoveHeadList(
			&pDisk->m_irpQueueHead,&pDisk->m_irpQueueLock))
		{
			status=STATUS_INVALID_DEVICE_REQUEST;

			Irp=CONTAINING_RECORD(Element,IRP,Tail.Overlay.ListEntry);

			ioStack=IoGetCurrentIrpStackLocation(Irp);

			Irp->IoStatus.Information=0;

			if(ioStack)
			{
				switch(ioStack->MajorFunction)
				{
				case IRP_MJ_READ:
				case IRP_MJ_WRITE:
					RequestLength=ioStack->Parameters.Read.Length;
					FileOffset.QuadPart=ioStack->Parameters.Read.ByteOffset.QuadPart;
					// Check for alignment
					if(!(RequestLength&(BYTES_PER_SECTOR-1)))
					{
						if(!(FileOffset.QuadPart&(BYTES_PER_SECTOR-1)))
						{
							FileOffset.QuadPart+=sizeof(DISK_HEADER_V4);
							// Get buffer address
							if(Irp->MdlAddress)
							{
								if(pDataBuff=MmGetSystemAddressForMdlSafe(Irp->MdlAddress,NormalPagePriority))
								{
									if((FileOffset.QuadPart+RequestLength)<=
										pDisk->m_pDiskInfo->FileSize.QuadPart)
									{
										beginSector=(ULONG)(FileOffset.QuadPart/BYTES_PER_SECTOR-1);
										Sectors=RequestLength/BYTES_PER_SECTOR;

										switch(ioStack->MajorFunction)
										{
										case IRP_MJ_READ:
											{
												// Read from file to buffer
												KdPrint(("\nCryptDisk:Read Offset=(%I64d) Length=(%d)",
													FileOffset.QuadPart-sizeof(DISK_HEADER_V4),
													RequestLength));
												{
													if(NT_SUCCESS(status=ZwReadFile(pDisk->m_hImageFile,NULL,NULL,NULL,
														&IoStatus,pDataBuff,RequestLength,
														&FileOffset,NULL)))
													{
														if(status==STATUS_PENDING)
														{
															ZwWaitForSingleObject(pDisk->m_hImageFile,
																FALSE,NULL);
														}
													}
													else
													{
														KdPrint(("\nCryptDisk:ReadWrite error while read. Status= %08X",status));
														IoStatus.Status=status;
													}
													if(NT_SUCCESS(IoStatus.Status))
													{
														// Decipher buffer
														pDisk->m_pDiskCipher->DecipherDataBlocks(beginSector, Sectors, pDataBuff);
													}
													status=IoStatus.Status;
													Irp->IoStatus.Information=IoStatus.Information;
												}
											}
											break;

										case IRP_MJ_WRITE:
											{
												if(pDisk->m_bReadOnly)
												{
													status=STATUS_MEDIA_WRITE_PROTECTED;
													Irp->IoStatus.Information=0;
													break;
												}

												UCHAR	*Buff;

												KdPrint(("\nCryptDisk:Write Offset=(%I64d) Length=(%d)",
													FileOffset.QuadPart-sizeof(DISK_HEADER_V4),
													RequestLength));

												// If request length less then cache buffer we use it
												if(RequestLength<=CACHE_BUFF_SIZE)
												{
													pDisk->m_pDiskCipher->EncipherDataBlocks(beginSector, Sectors, pDataBuff, cacheBuff);

													if(NT_SUCCESS(status=ZwWriteFile(pDisk->m_hImageFile,NULL,NULL,NULL,
														&IoStatus,cacheBuff,RequestLength,
														&FileOffset,NULL)))
													{
														if(status==STATUS_PENDING)
														{
															ZwWaitForSingleObject(pDisk->m_hImageFile,
																FALSE,NULL);
														}
													}
													else
													{
														KdPrint(("\nCryptDisk:ReadWrite error while write. Status= %08X",status));
														IoStatus.Status=status;
													}

													Irp->IoStatus.Information=IoStatus.Information;
													status=IoStatus.Status;
												}
												else
												{
													// Allocate buffer
													if(Buff=(UCHAR*)ExAllocatePoolWithTag(PagedPool, RequestLength, MEM_TAG))
													{
														pDisk->m_pDiskCipher->EncipherDataBlocks(beginSector, Sectors, pDataBuff, Buff);

														if(NT_SUCCESS(status=ZwWriteFile(pDisk->m_hImageFile, NULL, NULL, NULL,
															&IoStatus, Buff, RequestLength,
															&FileOffset, NULL)))
														{
															if(status==STATUS_PENDING)
															{
																ZwWaitForSingleObject(pDisk->m_hImageFile,
																	FALSE, NULL);
															}
														}
														else
														{
															KdPrint(("\nCryptDisk:ReadWrite error while write. Status= %08X",status));
															IoStatus.Status=status;
														}

														Irp->IoStatus.Information=IoStatus.Information;
														status=IoStatus.Status;

														memset(Buff,0,RequestLength);
														ExFreePoolWithTag(Buff, MEM_TAG);
													}
													else
													{
														status=STATUS_INSUFFICIENT_RESOURCES;
														Irp->IoStatus.Information=0;
													}
												}
											}
											break;
										}
									}
								}
								else
								{
									DnPrint("ReadWrite: unable to map pages");
									status=STATUS_INSUFFICIENT_RESOURCES;
									Irp->IoStatus.Information=0;
								}
							}
						}
						else
						{
							DnPrint("ReadWrite fail: invalid request offset");
							status=STATUS_INVALID_DEVICE_REQUEST;
						}
					}
					else
					{
						DnPrint("ReadWrite fail: invalid request length");
						status=STATUS_INVALID_DEVICE_REQUEST;
					}
					break;
				case IRP_MJ_DEVICE_CONTROL:
					status=pDisk->ThreadIoControl(pDeviceObject,Irp);
					break;
				}
			}

			Irp->IoStatus.Status=status;
			IofCompleteRequest(Irp, 
				NT_SUCCESS(status)?IO_DISK_INCREMENT:IO_NO_INCREMENT);
		}
	}
}

NTSTATUS VirtualDisk::SetFlags(DISK_ADD_INFO *pInfo)
{
	m_bRemovable = (pInfo->MountOptions&MOUNT_AS_REMOVABLE) != 0;
	m_bReadOnly = (pInfo->MountOptions&MOUNT_READ_ONLY) != 0;
	m_bMountDevice = (pInfo->MountOptions&MOUNT_DEVICE) != 0;

	// Do not save time for read-only images and devices
	m_bPreserveTimestamp = (pInfo->MountOptions&MOUNT_SAVE_TIME) &&
		((pInfo->MountOptions&(MOUNT_READ_ONLY|MOUNT_DEVICE)) == 0);

	return STATUS_SUCCESS;
}

NTSTATUS VirtualDisk::OpenFile(PCWSTR fileName)
{
	NTSTATUS					status;
	IO_STATUS_BLOCK				io_status;
	OBJECT_ATTRIBUTES			obj_attr;
	UNICODE_STRING				usFileName;

	bool						bFileOpened = false;

	// Prepare object attributes
	RtlInitUnicodeString(&usFileName, fileName);

	InitializeObjectAttributes(&obj_attr,&usFileName,
		OBJ_CASE_INSENSITIVE|OBJ_KERNEL_HANDLE,NULL,NULL);

	ULONG	fileAcess;
	ULONG	fileAttributes;

	fileAcess = m_bReadOnly ? GENERIC_READ : GENERIC_READ|GENERIC_WRITE;
	fileAttributes = FILE_NON_DIRECTORY_FILE|
		FILE_RANDOM_ACCESS|FILE_NO_INTERMEDIATE_BUFFERING|FILE_SYNCHRONOUS_IO_NONALERT;

	// Open file
	status = ZwCreateFile(&m_hImageFile, fileAcess,
		&obj_attr,&io_status, 0, FILE_ATTRIBUTE_NORMAL, 0, FILE_OPEN,
		fileAttributes,
		0, 0);
	if(NT_SUCCESS(status))
	{
		bFileOpened = true;

		if( m_bPreserveTimestamp && !m_bMountDevice )
		{
			status = ZwQueryInformationFile(m_hImageFile, &io_status, &m_fileBasicInfo,
				sizeof(FILE_BASIC_INFORMATION), FileBasicInformation);
			if(!NT_SUCCESS(status))
			{
				ZwClose(m_hImageFile);
				return status;
			}
		}

		status = ObReferenceObjectByHandle(m_hImageFile, fileAcess,
			*IoFileObjectType, KernelMode, (PVOID*)&m_pFileObject, NULL);
		if(NT_SUCCESS(status))
		{
			PDEVICE_OBJECT				pFSD;

			// Get related FS Device
			pFSD = IoGetRelatedDeviceObject(m_pFileObject);
			if(pFSD)
			{
				m_pDevice->StackSize = pFSD->StackSize+1;
			}

			// Get file size
			if(m_bMountDevice)
			{
				GET_LENGTH_INFORMATION	lengthInfo;
				ULONG_PTR				retLength;

				status = DNDeviceIoControl(pFSD, IOCTL_DISK_GET_LENGTH_INFO,
					NULL, 0, &lengthInfo, sizeof(lengthInfo), &retLength);
				if(NT_SUCCESS(status))
				{
					m_pDiskInfo->FileSize.QuadPart=lengthInfo.Length.QuadPart;
				}
				else
				{
					// If IOCTL_DISK_GET_LENGTH_INFO not supported,
					// try to use IOCTL_DISK_GET_DRIVE_GEOMETRY
					DISK_GEOMETRY	geometryInfo;

					status = DNDeviceIoControl(pFSD, IOCTL_DISK_GET_DRIVE_GEOMETRY,
						NULL, 0, &geometryInfo, sizeof(geometryInfo), &retLength);
					if(NT_SUCCESS(status))
					{
						m_pDiskInfo->FileSize.QuadPart =
							geometryInfo.Cylinders.QuadPart *
							geometryInfo.TracksPerCylinder *
							geometryInfo.SectorsPerTrack *
							geometryInfo.BytesPerSector;
					}
				}
			}
			else
			{
				FILE_STANDARD_INFORMATION	file_info;

				status = ZwQueryInformationFile(m_hImageFile, &io_status,
					&file_info,	sizeof(FILE_STANDARD_INFORMATION),
					FileStandardInformation);
				if(NT_SUCCESS(status) && NT_SUCCESS(io_status.Status))
				{
					m_pDiskInfo->FileSize.QuadPart = file_info.EndOfFile.QuadPart;
				}
				else
				{
					if(NT_SUCCESS(status))
					{
						status=io_status.Status;
					}
				}
			}
		}
	}

	if(!NT_SUCCESS(status))
	{
		// Cleanup on error
		if(m_pFileObject)
		{
			ObDereferenceObject(m_pFileObject);
			m_pFileObject=NULL;
		}
		if(bFileOpened)
		{
			ZwClose(m_hImageFile);
		}
	}
	return status;
}

NTSTATUS VirtualDisk::InitCipher(DISK_ADD_INFO *pInfo)
{
	NTSTATUS			status;
	const size_t		headerMaxSize = max(sizeof(DISK_HEADER_V4), sizeof(DISK_HEADER_V3));

	PVOID diskHeaderBuff = ExAllocatePoolWithTag(PagedPool, headerMaxSize, MEM_TAG);
	if(diskHeaderBuff)
	{
		LARGE_INTEGER		fileOffset;
		IO_STATUS_BLOCK		io_status;

		fileOffset.QuadPart=0;
		status = ZwReadFile(m_hImageFile, 0, 0, 0, &io_status,
			diskHeaderBuff, headerMaxSize, &fileOffset, 0);

		if(NT_SUCCESS(status)&&NT_SUCCESS(io_status.Status)&&
			(io_status.Information == headerMaxSize))
		{
			switch(pInfo->DiskFormatVersion)
			{
			case DISK_VERSION_3:
				{
					DISK_HEADER_V3* pHeader = reinterpret_cast<DISK_HEADER_V3*>(diskHeaderBuff);

					// Decipher header
					bool decipherResult = DiskHeaderTools::Decipher(pHeader, pInfo->UserKey, static_cast<DISK_CIPHER>(pInfo->wAlgoId));

					if(decipherResult)
					{
						switch(static_cast<DISK_CIPHER>(pInfo->wAlgoId))
						{
						case DISK_CIPHER_AES:
							{
								PVOID pCipherbuff = ExAllocatePoolWithTag(PagedPool, sizeof(DiscCipherAesV3), MEM_TAG);

								if (pCipherbuff)
								{
									DiscCipherAesV3* pCipher = new (pCipherbuff) DiscCipherAesV3(DiskParamatersV3(pHeader->DiskKey, pHeader->TweakKey));
									m_pDiskCipher = pCipher;
									status = STATUS_SUCCESS;
								}
								else
								{
									status = STATUS_INSUFFICIENT_RESOURCES;
								}
							}
							break;

						case DISK_CIPHER_TWOFISH:
							{
								PVOID pCipherbuff = ExAllocatePoolWithTag(PagedPool, sizeof(DiskCipherTwofishV3), MEM_TAG);

								if (pCipherbuff)
								{
									DiskCipherTwofishV3* pCipher = new (pCipherbuff) DiskCipherTwofishV3(DiskParamatersV3(pHeader->DiskKey, pHeader->TweakKey));
									m_pDiskCipher = pCipher;
									status = STATUS_SUCCESS;
								}
								else
								{
									status = STATUS_INSUFFICIENT_RESOURCES;
								}
							}
							break;
						}
					}
					else
					{
						status = STATUS_WRONG_PASSWORD;
					}
				}
				break;
			case DISK_VERSION_4:
				{
					DISK_HEADER_V4* pHeader = reinterpret_cast<DISK_HEADER_V4*>(diskHeaderBuff);

					// Decipher header
					bool decipherResult = DiskHeaderTools::Decipher(pHeader, pInfo->UserKey, pInfo->InitVector, static_cast<DISK_CIPHER>(pInfo->wAlgoId));
					if(decipherResult)
					{
						switch(static_cast<DISK_CIPHER>(pInfo->wAlgoId))
						{
						case DISK_CIPHER_AES:
							{
								PVOID pCipherbuff = ExAllocatePoolWithTag(PagedPool, sizeof(DiskCipherV4<RijndaelEngine>), MEM_TAG);

								if (pCipherbuff)
								{
									DiskCipherV4<RijndaelEngine>* pCipher =
										new (pCipherbuff) DiskCipherV4<RijndaelEngine>(
											DiskParametersV4(pHeader->DiskKey, pHeader->TweakKey, pHeader->TweakNumber, pHeader->DiskSectorSize));
									m_pDiskCipher = pCipher;
									status = STATUS_SUCCESS;
								}
								else
								{
									status = STATUS_INSUFFICIENT_RESOURCES;
								}
							}
							break;

						case DISK_CIPHER_TWOFISH:
							{
								PVOID pCipherbuff = ExAllocatePoolWithTag(PagedPool, sizeof(DiskCipherV4<TwofishEngine>), MEM_TAG);

								if (pCipherbuff)
								{
									DiskCipherV4<TwofishEngine>* pCipher =
										new (pCipherbuff) DiskCipherV4<TwofishEngine>(
										DiskParametersV4(pHeader->DiskKey, pHeader->TweakKey, pHeader->TweakNumber, pHeader->DiskSectorSize));
									m_pDiskCipher = pCipher;
									status = STATUS_SUCCESS;
								}
								else
								{
									status = STATUS_INSUFFICIENT_RESOURCES;
								}
							}
							break;
						}
					}
					else
					{
						status = STATUS_WRONG_PASSWORD;
					}
				}
				break;
			}
		}

		RtlSecureZeroMemory(diskHeaderBuff, headerMaxSize);
		ExFreePoolWithTag(diskHeaderBuff, MEM_TAG);
	}
	else
	{
		status = STATUS_INSUFFICIENT_RESOURCES;
	}

	return status;
}
