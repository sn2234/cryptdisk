
#include "stdafx.h"

#include "AppRandom.h"

namespace fs = boost::filesystem;

const wchar_t seedFileName[] = L"random.seed";

RandomGenerator* InitAppRandom::Alloc()
{
	RandomGenerator* pGen = nullptr;

	wchar_t* pExecutablePath;

	if(_get_wpgmptr(&pExecutablePath) == 0)
	{
		fs::wpath seedFilePath(pExecutablePath);
		seedFilePath = seedFilePath.parent_path();
		seedFilePath /= seedFileName;

		pGen = new RandomGenerator(fs::absolute(seedFilePath).generic_wstring());
	}

	return pGen;
}
