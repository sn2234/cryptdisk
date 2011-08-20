
#include "StdAfx.h"

#include "FavoritesModel.h"
#include "AppFavorites.h"


FavoritesModel::FavoritesModel(void)
{
	m_connection = AppFavorites::instance().connect(std::bind(&FavoritesModel::OnFavoritesUpdate, this));
}

FavoritesModel::~FavoritesModel(void)
{
	AppFavorites::instance().disconnect(m_connection);
}

void FavoritesModel::OnFavoritesUpdate()
{
	UpdateViews();
}
