
#pragma once

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

std::vector<FavoriteImage> LoadFavorites(const std::wstring& filePath);
void SaveFavorites(const std::wstring& filePath, const std::vector<FavoriteImage>& favorites);
