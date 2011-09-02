
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
