
#include "stdafx.h"

#include "AppFavorites.h"

namespace fs = boost::filesystem;

std::vector<FavoriteImage>* InitAppFavorites::Alloc()
{
	fs::wpath file(Favorites::PreparePath());

	if(fs::exists(file))
	{
		return new std::vector<FavoriteImage>(Favorites::Load(file.wstring()));
	}

	return new std::vector<FavoriteImage>();
}
