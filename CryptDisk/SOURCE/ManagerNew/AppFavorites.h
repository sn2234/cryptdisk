
#pragma once

#include "Singleton.h"
#include "Favorites.h"

class InitAppFavorites
{
public:
	static FavoritesManager* Alloc()
	{
		return new FavoritesManager();
	}
};

typedef Singleton<FavoritesManager, InitAppFavorites> AppFavorites;
