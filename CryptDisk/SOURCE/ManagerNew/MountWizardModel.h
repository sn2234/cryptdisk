
#pragma once

#include "Document.h"

class MountWizardModel : public Document
{
public:
	MountWizardModel(void);
	virtual ~MountWizardModel(void);

	bool TryOpenImage() const;
	
	void PerformMount();

	const std::wstring& ImageFilePath() const { return m_imageFilePath; }
	void ImageFilePath(const std::wstring& val) { m_imageFilePath = val; UpdateViews(); }

	const std::vector<std::wstring>& KeyFiles() const { return m_keyFiles; }
	void KeyFiles(const std::vector<std::wstring>& val) { m_keyFiles = val; UpdateViews(); }

	const std::string& Password() const { return m_password; }
	void Password(const std::string& val) { m_password = val; UpdateViews(); }

	bool UseRecentDocuments() const { return m_bUseRecentDocuments; }
	void UseRecentDocuments(bool val) { m_bUseRecentDocuments = val; UpdateViews(); }

	WCHAR DriveLetter() const { return m_driveLetter; }
	void DriveLetter(WCHAR val) { m_driveLetter = val; UpdateViews(); }

	bool MountAsReadOnly() const { return m_mountAsReadOnly; }
	void MountAsReadOnly(bool val) { m_mountAsReadOnly = val; UpdateViews(); }

	bool MountAsRemovable() const { return m_mountAsRemovable; }
	void MountAsRemovable(bool val) { m_mountAsRemovable = val; UpdateViews(); }

	bool UseMountManager() const { return m_useMountManager; }
	void UseMountManager(bool val) { m_useMountManager = val; UpdateViews(); }

	bool PreserveImageTimestamp() const { return m_preserveImageTimestamp; }
	void PreserveImageTimestamp(bool val) { m_preserveImageTimestamp = val; UpdateViews(); }

	bool AddToFavorites() const { return m_addToFavorites; }
	void AddToFavorites(bool val) { m_addToFavorites = val; UpdateViews(); }
private:
	std::wstring				m_imageFilePath;
	std::vector<std::wstring>	m_keyFiles;
	std::string					m_password;
	bool						m_bUseRecentDocuments;
	WCHAR						m_driveLetter;
	// Mount options
	bool						m_mountAsReadOnly;
	bool						m_mountAsRemovable;
	bool						m_useMountManager;
	bool						m_preserveImageTimestamp;

	bool						m_addToFavorites;
};
