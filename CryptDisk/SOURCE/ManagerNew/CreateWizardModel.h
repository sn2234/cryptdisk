#pragma once
#include "document.h"
#include "DiskFormat.h"
#include "VolumeTools.h"

class CreateWizardModel : public Document
{
public:
	CreateWizardModel(const VolumeDesk* descriptor);
	~CreateWizardModel() = default;

	void DoCreate();

	const std::wstring& ImageFilePath() const { return m_imageFilePath; }
	void ImageFilePath(const std::wstring& val) { m_imageFilePath = val; }

	const std::vector<std::wstring>& KeyFiles() const { return m_keyFiles; }
	void KeyFiles(const std::vector<std::wstring>& val) { m_keyFiles = val; }

	const std::string& Password() const { return m_password; }
	void Password(const std::string& val) { m_password = val; }

	bool UseRecentDocuments() const { return m_bUseRecentDocuments; }
	void UseRecentDocuments(bool val) { m_bUseRecentDocuments = val; }

	unsigned __int64 ImageSize() const { return m_imageSize; }
	void ImageSize(unsigned __int64 val) { m_imageSize = val; }

	bool QuickFormat() const { return m_bQuickFormat; }
	void QuickFormat(bool val) { m_bQuickFormat = val; }

	DISK_CIPHER CipherAlgorithm() const { return m_cipherAlgorithm; }
	void CipherAlgorithm(DISK_CIPHER val) { m_cipherAlgorithm = val; }

	bool IsVolume() const { return m_isVolume; }
private:
	// Image file specific
	std::wstring				m_imageFilePath;

	std::vector<std::wstring>	m_keyFiles;
	std::string					m_password;
	bool						m_bUseRecentDocuments;
	unsigned __int64			m_imageSize;
	bool						m_bQuickFormat;
	DISK_CIPHER					m_cipherAlgorithm;

	bool						m_isVolume;
	std::unique_ptr<VolumeDesk>	m_volumeDescriptor;
};
