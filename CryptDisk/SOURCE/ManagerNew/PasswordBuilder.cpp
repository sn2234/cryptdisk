
#include "stdafx.h"

#include "PasswordBuilder.h"
#include "winapi_exception.h"
#include "AppMemory.h"

PasswordBuilder::PasswordBuilder( const std::vector<std::wstring>& keyFilesList, const unsigned char *password, size_t passwordLength, ULONG keyFileDataLength )
	: m_password(NULL)
	, m_passwordLength((keyFilesList.empty() ? 0 : SHA256_DIDGEST_SIZE) + passwordLength)
{
	m_password = reinterpret_cast<unsigned char*>(AppMemory::instance().Alloc(static_cast<DWORD>(m_passwordLength)));
	SCOPE_EXIT
	{
		AppMemory::instance().Free(m_password);
		m_password = NULL;
	};

	if(!keyFilesList.empty())
	{
		// Collect files
		CryptoLib::SHA256_HASH hash;

		hash.Init();

		std::for_each(keyFilesList.cbegin(), keyFilesList.cend(), [&hash, keyFileDataLength](const std::wstring&filePath){
			HANDLE	hFile,hMapping;
			void	*pData;
			ULONG	fileSize;
			bool	bResult = false;
			ULONG	dataLength = keyFileDataLength;
#pragma pack(push, 1)
			struct
			{
				FILETIME			creation;
				FILETIME			lastAccess;
				FILETIME			lastWrite;
			}fileTimes;
#pragma pack(pop)

			hFile=CreateFileW(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL, NULL);
			if(hFile == INVALID_HANDLE_VALUE)
			{
				throw winapi_exception("Unable to open keyfile");
			}

			SCOPE_EXIT
			{
				CloseHandle(hFile);
			};

			if(GetFileTime(hFile, &fileTimes.creation,
				&fileTimes.lastAccess, &fileTimes.lastWrite))
			{

				fileSize=GetFileSize(hFile, NULL);

				if(dataLength > fileSize)
				{
					dataLength=fileSize;
				}

				hMapping=CreateFileMapping(hFile, NULL, PAGE_READONLY, 0,
					dataLength, NULL);
				if(hMapping)
				{
					pData=MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, dataLength);
					if(pData)
					{
						hash.Update(pData, dataLength);
						bResult=true;

						UnmapViewOfFile(pData);
					}
					CloseHandle(hMapping);
				}

				SetFileTime(hFile, &fileTimes.creation,
					&fileTimes.lastAccess, &fileTimes.lastWrite);
			}

			if(!bResult)
			{
				throw std::logic_error("Unable to add keyfile");
			}
		});
			
		hash.Final(m_password);
		hash.Clear();
	}

	std::copy_n(password, passwordLength, m_password + (keyFilesList.empty() ? 0 : SHA256_DIDGEST_SIZE));
}

PasswordBuilder::~PasswordBuilder( void )
{
	if(m_password)
	{
		AppMemory::instance().Free(m_password);
	}
}
