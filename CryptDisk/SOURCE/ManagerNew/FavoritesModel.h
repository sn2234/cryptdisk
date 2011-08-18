
#pragma once

#include "Document.h"
#include "Favorites.h"

class FavoritesModel : public Document
{
public:
	FavoritesModel(void);
	virtual ~FavoritesModel(void);

	
	std::vector<FavoriteImage> FavoriteImages() const { return m_favoriteImages; }
private:
	std::vector<FavoriteImage>	m_favoriteImages;
};
