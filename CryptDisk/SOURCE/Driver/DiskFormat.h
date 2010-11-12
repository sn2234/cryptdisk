
#pragma once

#include "format.h"

#include <BaseCrypt\DNAES.h>
#include <BaseCrypt\DNTwofish.h>
#include <BaseCrypt\LRWMode.h>
#include <BaseCrypt\LRWAES.h>
#include <BaseCrypt\LRWTwofish.h>
#include <BaseCrypt\SHA256_HASH.h>

#pragma pack(push)
#pragma pack(1)

enum DISK_CIPHER
{
	DISK_CIPHER_AES=1,
	DISK_CIPHER_TWOFISH=2,

	MAX_DISK_CIPHER
};

enum DISK_VERSION
{
	DISK_VERSION_3 = 1,
	DISK_VERSION_4 = 2,
};

typedef	struct	VERSION_INFO
{
	UCHAR		formatVersion;
}VERSION_INFO;

//////////////////////////////////////////////////////////////////////////
// V3

#define DISK_SALT_SIZE				32
#define	DISK_HEADER_REAL_SIZE_V3		(sizeof(VERSION_INFO)+DISK_SALT_SIZE+AES_KEY_SIZE+AES_BLOCK_SIZE+SHA256_DIDGEST_SIZE+SHA256_BLOCK_SIZE)
#define DISK_HEADER_ENC_SIZE_V3		(sizeof(VERSION_INFO)+AES_BLOCK_SIZE+SHA256_DIDGEST_SIZE+SHA256_BLOCK_SIZE+(BYTES_PER_SECTOR-DISK_HEADER_REAL_SIZE_V3))
#define DISK_HEADER_ENC_PASS_SIZE_V3	(sizeof(VERSION_INFO)+AES_KEY_SIZE+AES_BLOCK_SIZE+SHA256_DIDGEST_SIZE+SHA256_BLOCK_SIZE+(BYTES_PER_SECTOR-DISK_HEADER_REAL_SIZE_V3))

typedef struct DISK_HEADER_V3
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

	UCHAR			Padding[BYTES_PER_SECTOR-DISK_HEADER_REAL_SIZE_V3];
}DISK_HEADER_V3;

#define		MAGIC_BLOCKS	((SHA256_DIDGEST_SIZE+SHA256_BLOCK_SIZE)/AES_BLOCK_SIZE)

//////////////////////////////////////////////////////////////////////////
// V4

#define	DISK_HEADER_REAL_SIZE_V4		\
	( \
		sizeof(VERSION_INFO)+ \
		DISK_SALT_SIZE+ \
		AES_KEY_SIZE+ \
		AES_BLOCK_SIZE+ \
		AES_KEY_SIZE+ \
		sizeof(UINT64)+ \
		sizeof(UINT32)+ \
		SHA256_DIDGEST_SIZE+ \
		SHA256_BLOCK_SIZE \
	)

#define DISK_HEADER_ENC_SIZE_V4		\
	( \
		sizeof(VERSION_INFO)+ \
		AES_KEY_SIZE+ \
		sizeof(UINT64)+ \
		sizeof(UINT32)+ \
		SHA256_DIDGEST_SIZE+ \
		SHA256_BLOCK_SIZE+ \
		( \
			BYTES_PER_SECTOR- \
			DISK_HEADER_REAL_SIZE_V4 \
		) \
		)

#define DISK_HEADER_ENC_PASS_SIZE_V4	\
	( \
		sizeof(VERSION_INFO)+ \
		AES_KEY_SIZE+ \
		AES_BLOCK_SIZE+ \
		AES_KEY_SIZE+ \
		sizeof(UINT64)+ \
		sizeof(UINT32)+ \
		SHA256_DIDGEST_SIZE+ \
		SHA256_BLOCK_SIZE+ \
		( \
			BYTES_PER_SECTOR- \
			DISK_HEADER_REAL_SIZE_V4 \
		) \
	)

typedef struct DISK_HEADER_V4
{
	// Not encrypted data
	UCHAR			DiskSalt[DISK_SALT_SIZE];
	// Data encrypted with password
	UCHAR			DiskKey[AES_KEY_SIZE];
	UCHAR			TweakNumber[AES_BLOCK_SIZE];
	// Data encrypted with disk key and password
	VERSION_INFO	DiskVersion;
	UCHAR			TweakKey[AES_KEY_SIZE];
	UINT64			ImageOffset;
	UINT32			DiskSectorSize;
	UCHAR			MagicHash[SHA256_DIDGEST_SIZE];
	UCHAR			MagicValue[SHA256_BLOCK_SIZE];

	UCHAR			Padding[BYTES_PER_SECTOR-DISK_HEADER_REAL_SIZE_V4];
}DISK_HEADER_V4;


#pragma pack(pop)
