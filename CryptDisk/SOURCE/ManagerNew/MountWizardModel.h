
#pragma once

#include "Document.h"
#include "VolumeTools.h"

class MountWizardModel : public Document
{
public:
	MountWizardModel(const VolumeDesk* descriptor);
	virtual ~MountWizardModel(void);

	bool TryOpenImage() const;
	
	void PerformMount();

	void ChangePassword();

	const std::wstring& ImageFilePath() const { return m_imageFilePath; }
	void ImageFilePath(const std::wstring& val) { m_imageFilePath = val; }

	const std::vector<std::wstring>& KeyFiles() const { return m_keyFiles; }
	void KeyFiles(const std::vector<std::wstring>& val) { m_keyFiles = val; }

	const std::string& Password() const { return m_password; }
	void Password(const std::string& val) { m_password = val; }

	bool UseRecentDocuments() const { return m_bUseRecentDocuments; }
	void UseRecentDocuments(bool val) { m_bUseRecentDocuments = val; }

	WCHAR DriveLetter() const { return m_driveLetter; }
	void DriveLetter(WCHAR val) { m_driveLetter = val; }

	bool MountAsReadOnly() const { return m_mountAsReadOnly; }
	void MountAsReadOnly(bool val) { m_mountAsReadOnly = val; }

	bool MountAsRemovable() const { return m_mountAsRemovable; }
	void MountAsRemovable(bool val) { m_mountAsRemovable = val; }

	bool UseMountManager() const { return m_useMountManager; }
	void UseMountManager(bool val) { m_useMountManager = val; }

	bool PreserveImageTimestamp() const { return m_preserveImageTimestamp; }
	void PreserveImageTimestamp(bool val) { m_preserveImageTimestamp = val; }

	bool AddToFavorites() const { return m_addToFavorites; }
	void AddToFavorites(bool val) { m_addToFavorites = val; }

	bool IsVolume() const { return m_isVolume; }
	VolumeDesk VolumeDescriptor() const { return *m_volumeDescriptor; }
private:

	bool						m_isVolume;

	// For images
	std::wstring				m_imageFilePath;

	// For volumes
	std::unique_ptr<VolumeDesk>	m_volumeDescriptor;

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
