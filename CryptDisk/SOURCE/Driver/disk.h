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

#ifndef	_DISK_H_INCLUDED
#define _DISK_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma pack(push)
#pragma pack(1)

#define		IOCTL_VDISK_GET_VERSION		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		IOCTL_VDISK_GET_DISKS_COUNT	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		IOCTL_VDISK_GET_DISKS_INFO	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		IOCTL_VDISK_ADD_DISK		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x804, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		IOCTL_VDISK_DELETE_DISK		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x805, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef	struct	VERSION_INFO
{
	UCHAR		formatVersion;
}VERSION_INFO;

typedef	struct	VERSION_DRIVER
{
	UCHAR		versionMinor;
	UCHAR		versionMajor;
}VERSION_DRIVER;

typedef	struct DISKS_COUNT_INFO
{
	ULONG	DiskCount;
	ULONG	DiskCountMax;
}DISKS_COUNT_INFO;

typedef struct DISK_BASIC_INFO
{
	WCHAR			DriveLetter;
	ULONG			OpenCount;
	LARGE_INTEGER	FileSize;
	WCHAR			FilePath[MAXIMUM_FILENAME_LENGTH];
}DISK_BASIC_INFO;

// Flags for MountOptions
#define		MOUNT_AS_REMOVABLE		0x01	// Mount drive as removable media
#define		MOUNT_READ_ONLY			0x02	// Mount drive read-only (troubles with NTFS!)
#define		MOUNT_VIA_MOUNTMGR		0x04	// Use mount manager to create symbolic links,
											// otherwise create links itself. This may surprise some software
#define		MOUNT_SAVE_TIME			0x08	// Save current file modify/access time and restore it on dismount
#define		MOUNT_DEVICE			0x10	// FilePath is partition device name

typedef struct DISK_ADD_INFO
{
	ULONG	MountOptions;
	WCHAR	DriveLetter;
	USHORT	wAlgoId;
	UCHAR	UserKey[AES_KEY_SIZE];
	WCHAR	FilePath[MAXIMUM_FILENAME_LENGTH];
}DISK_ADD_INFO;

typedef struct DISK_DELETE_INFO
{
	WCHAR	DriveLetter;
	BOOL	bForce;
}DISK_DELETE_INFO;

typedef union DISK_INIT_VECTOR
{
	UCHAR			InitVector[AES_BLOCK_SIZE];
	ULONG			SectorNumber;
}DISK_INIT_VECTOR;

#define DISK_SALT_SIZE				32
#define	DISK_HEADER_REAL_SIZE		(sizeof(VERSION_INFO)+DISK_SALT_SIZE+AES_KEY_SIZE+AES_BLOCK_SIZE+SHA256_DIDGEST_SIZE+SHA256_BLOCK_SIZE)
#define DISK_HEADER_ENC_SIZE		(sizeof(VERSION_INFO)+AES_BLOCK_SIZE+SHA256_DIDGEST_SIZE+SHA256_BLOCK_SIZE+(BYTES_PER_SECTOR-DISK_HEADER_REAL_SIZE))
#define DISK_HEADER_ENC_PASS_SIZE	(sizeof(VERSION_INFO)+AES_KEY_SIZE+AES_BLOCK_SIZE+SHA256_DIDGEST_SIZE+SHA256_BLOCK_SIZE+(BYTES_PER_SECTOR-DISK_HEADER_REAL_SIZE))

typedef struct DISK_HEADER
{
// Not encrypted data
	UCHAR			DiskSalt[DISK_SALT_SIZE];
// Data encrypted with password
	UCHAR			DiskKey[AES_KEY_SIZE];
// Data encrypted with disk key and password
	UCHAR			TweakKey[AES_BLOCK_SIZE];
	VERSION_INFO	DiskVersion;
	UCHAR			MagicHash[SHA256_DIDGEST_SIZE];
	UCHAR			MagicValue[SHA256_BLOCK_SIZE];
	
	UCHAR			Padding[BYTES_PER_SECTOR-DISK_HEADER_REAL_SIZE];
}DISK_HEADER;

#define		MAGIC_BLOCKS	((SHA256_DIDGEST_SIZE+SHA256_BLOCK_SIZE)/AES_BLOCK_SIZE)

#pragma pack(pop)
#endif	//_DISK_H_INCLUDED
