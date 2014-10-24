
#pragma once

class CommonTools
{
public:
	static std::wstring MakeAppDataPath(const std::wstring& fileName);
	static std::wstring FormatSize(UINT64 size);
};
