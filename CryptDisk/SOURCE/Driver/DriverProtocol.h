#pragma once

#pragma pack(push, 1)

#define		IOCTL_VDISK_GET_VERSION		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		IOCTL_VDISK_GET_DISKS_COUNT	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		IOCTL_VDISK_GET_DISKS_INFO	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		IOCTL_VDISK_ADD_DISK		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x804, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define		IOCTL_VDISK_DELETE_DISK		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x805, METHOD_BUFFERED, FILE_ANY_ACCESS)

// All ciphers uses 256 bit key and 128 bit blocks
#define CIPHER_KEY_SIZE			32
#define CIPHER_BLOCK_SIZE		16

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
	ULONG			DiskId;
	WCHAR			DriveLetter;
	ULONG			OpenCount;
	LARGE_INTEGER	FileSize;
	ULONG			PathSize;		// Size of FilePath in bytes, including FilePath member own size
	WCHAR			FilePath[1];
}DISK_BASIC_INFO;

typedef struct DISK_ADD_INFO
{
	ULONG			MountOptions;
	WCHAR			DriveLetter;
	int				DiskFormatVersion;
	USHORT			wAlgoId;
	UCHAR			UserKey[CIPHER_KEY_SIZE];
	UCHAR			InitVector[CIPHER_BLOCK_SIZE];
	ULONG			PathSize;
	WCHAR			FilePath[1];
}DISK_ADD_INFO;

typedef struct DISK_DELETE_INFO
{
	ULONG	DiskId;
	BOOL	bForce;
}DISK_DELETE_INFO;


// Flags for MountOptions
#define		MOUNT_AS_REMOVABLE		0x01	// Mount drive as removable media
#define		MOUNT_READ_ONLY			0x02	// Mount drive read-only (troubles with NTFS!)
#define		MOUNT_VIA_MOUNTMGR		0x04	// Use mount manager to create symbolic links,
											// otherwise create links itself. This may surprise some software
#define		MOUNT_SAVE_TIME			0x08	// Save current file modify/access time and restore it on dismount
#define		MOUNT_DEVICE			0x10	// FilePath is partition device name

#pragma pack(pop)
