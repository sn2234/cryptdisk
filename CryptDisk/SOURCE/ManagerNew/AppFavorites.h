
#pragma once

#include "Singleton.h"
#include "Favorites.h"

class InitAppFavorites
{
public:
	static std::vector<FavoriteImage>* Alloc();
};

typedef Singleton<std::vector<FavoriteImage>, InitAppFavorites> AppFavorites;
