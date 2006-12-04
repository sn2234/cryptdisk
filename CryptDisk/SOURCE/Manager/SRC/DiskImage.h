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

#ifndef DISKIMAGE_H_HEADER_INCLUDED_BCD3CC16
#define DISKIMAGE_H_HEADER_INCLUDED_BCD3CC16

//##ModelId=432710020148
class DiskImage
{
  public:
	// Contains information for image creation
	//##ModelId=432C6B51000F
	struct IMAGE_CREATE_INFO
	{
		//##ModelId=432C6E2402AF
		LARGE_INTEGER diskSize;
	};

	DiskImage()
	{
		Initialized=FALSE;
		hFile=INVALID_HANDLE_VALUE;
	}
	
	virtual ~DiskImage()
	{
		Close();
	}
	
	// Create new disk image
	//##ModelId=43271066001F
	BOOL Create(const TCHAR *filePath, IMAGE_CREATE_INFO *pCreateInfo, CRandom *pRandom, UCHAR *password, ULONG passwordLength, DISK_CIPHER cipherAlgo);

	// Open existing image
	//##ModelId=432710E101D4
	BOOL Open(const TCHAR *filePath, UCHAR *password, ULONG passwordLength);

	BOOL ChangePassword(UCHAR *password, ULONG passwordLength);

	//##ModelId=4327149D001F
	void Close();

	//##ModelId=432714660157
	ULONG GetSectorsCount()
	{
		return sectorsCount;
	}

	UINT64 GetSize()
	{
		return imageParams.diskSize.QuadPart;
	}

	//##ModelId=432714A90177
	BOOL ReadSectors(ULONG sectorBegin, ULONG sectorCount, void *pBuff);

	BOOL WriteSectors(ULONG sectorBegin, ULONG sectorCount, void *pBuff);

	// This function formate file system header
	// return number of sectors occupied by file system
	// all other sectors must be filled externally
	//##ModelId=432714C300BB
	virtual ULONG Format(PVOID param) = 0;

	CDiskHeader* GetHeader()
	{
		return &diskHeader;
	}

	DISK_HEADER* GetHeaderBuff()
	{
		return &headerBuff;
	}

	DiskCipher* GetCipher()
	{
		return &diskCipher;
	}
  protected:
	//##ModelId=432C70270290
	IMAGE_CREATE_INFO		imageParams;

#pragma pack(push, 1)
	struct
	{
		FILETIME			creation;
		FILETIME			lastAccess;
		FILETIME			lastWrite;
	}fileTimes;
#pragma pack(pop)

	//##ModelId=432C713E00AB
	HANDLE					hFile;

	//##ModelId=432C718201A5
	BOOL					Initialized;
	
	CDiskHeader				diskHeader;
	DISK_HEADER				headerBuff; // Buffer contains disk header

	DiskCipher				diskCipher;

	ULONG					sectorsCount;
};


#endif /* DISKIMAGE_H_HEADER_INCLUDED_BCD3CC16 */
