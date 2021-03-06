
#pragma once

#include "Document.h"

class FavoriteImage
{
public:
	FavoriteImage(const std::wstring& imagePath, wchar_t driveLetter, bool readOnly, bool removable, bool mountManager, bool preserveTimestamp);

	const std::wstring& ImagePath() const { return m_imagePath; }
	wchar_t DriveLetter() const { return m_driveLetter; }
	bool ReadOnly() const { return m_readOnly; }
	bool Removable() const { return m_removable; }
	bool MountManager() const { return m_mountManager; }
	bool PreserveTimestamp() const { return m_preserveTimestamp; }
private:
	std::wstring	m_imagePath;
	wchar_t			m_driveLetter;
	bool			m_readOnly;
	bool			m_removable;
	bool			m_mountManager;
	bool			m_preserveTimestamp;
};

class Favorites
{
public:
	static std::vector<FavoriteImage> Load(const std::wstring& filePath);
	static void Save(const std::wstring& filePath, const std::vector<FavoriteImage>& favorites);
	static std::wstring PreparePath();
};

class FavoritesManager : private boost::noncopyable, public Document
{
public:
	FavoritesManager();
	~FavoritesManager();

	const std::vector<FavoriteImage>& Favorites() const { return m_favorites; }
	std::vector<FavoriteImage>& Favorites() { return m_favorites; }

private:
	std::vector<FavoriteImage>	m_favorites;
};
