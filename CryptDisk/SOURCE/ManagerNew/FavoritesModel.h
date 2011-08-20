
#pragma once

#include "Document.h"
#include "Favorites.h"

class FavoritesModel : public Document
{
public:
	FavoritesModel(void);
	virtual ~FavoritesModel(void);
	
private:
	void OnFavoritesUpdate();

private:
	Document::connection_t		m_connection;
};
