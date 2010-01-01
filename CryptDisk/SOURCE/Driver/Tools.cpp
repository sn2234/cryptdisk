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

NTSTATUS DNDeviceIoControl(IN PDEVICE_OBJECT pDevice, IN ULONG dwIoControlCode, IN PVOID lpInBuffer, IN ULONG nInBufferSize, OUT PVOID lpOutBuffer, IN ULONG nOutBufferSize, OUT PULONG lpBytesReturned)
{
	NTSTATUS			status;
	
	IO_STATUS_BLOCK		iosb;
	PIRP				irp;
	KEVENT				event;

	KeInitializeEvent(&event, NotificationEvent, FALSE);
	memset(&iosb,0,sizeof(IO_STATUS_BLOCK));

	irp=IoBuildDeviceIoControlRequest(dwIoControlCode,pDevice,lpInBuffer,nInBufferSize,
		lpOutBuffer,nOutBufferSize,FALSE,&event,&iosb);
	if(!irp)
	{
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	status=IoCallDriver(pDevice,irp);
	if(status==STATUS_PENDING)
	{
		KeWaitForSingleObject (&event, Executive, KernelMode, FALSE, NULL);
		status=iosb.Status;
	}
	if(lpBytesReturned)
	{
		*lpBytesReturned=iosb.Information;
	}

	return status;
}

NTSTATUS DNDeviceFSControl(IN PDEVICE_OBJECT pDevice, IN PFILE_OBJECT pFileObject, IN ULONG dwIoControlCode, IN PVOID lpInBuffer, IN ULONG nInBufferSize, OUT PVOID lpOutBuffer, IN ULONG nOutBufferSize, OUT PULONG lpBytesReturned)
{
	NTSTATUS			status;

	IO_STATUS_BLOCK		iosb;
	PIRP				irp;
	KEVENT				event;
	
	PIO_STACK_LOCATION	pStack;

	KeInitializeEvent(&event, NotificationEvent, FALSE);
	memset(&iosb,0,sizeof(IO_STATUS_BLOCK));

	irp=IoBuildDeviceIoControlRequest(dwIoControlCode,pDevice,lpInBuffer,nInBufferSize,
		lpOutBuffer,nOutBufferSize,FALSE,&event,&iosb);
	if(!irp)
	{
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	
	pStack=IoGetNextIrpStackLocation(irp);
	
	pStack->MajorFunction=IRP_MJ_FILE_SYSTEM_CONTROL;
	pStack->MinorFunction=IRP_MN_USER_FS_REQUEST;
	pStack->FileObject=pFileObject;

	status=IoCallDriver(pDevice,irp);
	if(status==STATUS_PENDING)
	{
		KeWaitForSingleObject (&event, Executive, KernelMode, FALSE, NULL);
		status=iosb.Status;
	}
	if(lpBytesReturned)
	{
		*lpBytesReturned=iosb.Information;
	}

	return status;
}
