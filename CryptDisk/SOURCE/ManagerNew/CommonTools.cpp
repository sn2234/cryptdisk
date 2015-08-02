
#include "stdafx.h"

#include "CommonTools.h"

namespace fs = boost::filesystem;

std::wstring CommonTools::MakeAppDataPath( const std::wstring& fileName )
{
	wchar_t appDataPath[MAX_PATH];

	SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, appDataPath);

	fs::wpath folder(appDataPath);

	folder /= L"CryptDisk";

	if(!fs::exists(folder))
	{
		fs::create_directories(folder);
	}

	fs::wpath file(folder);
	file /= fileName;

	return file.wstring();
}

std::wstring CommonTools::FormatSize(UINT64 size)
{
	const UINT64 szGiga = 1024 * 1024 * 1024;
	const UINT64 szMega = 1024 * 1024;
	const UINT64 szKilo = 1024;

	if (size >= szGiga) // GB
	{
		return boost::lexical_cast<std::wstring>(size / szGiga) + L"GB";
	}
	else if (size >= szMega) // MB
	{
		return boost::lexical_cast<std::wstring>(size / szMega) + L"MB";
	}
	else if (size >= szKilo) // KB
	{
		return boost::lexical_cast<std::wstring>(size / szKilo) + L"KB";
	}
	else
	{
		return boost::lexical_cast<std::wstring>(size) +L" Bytes";
	}
}
