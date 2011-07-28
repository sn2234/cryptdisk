#include "StdAfx.h"
#include "CreateWizardModel.h"
#include "CryptDiskHelpers.h"
#include "AppRandom.h"
#include "PasswordBuilder.h"

CreateWizardModel::CreateWizardModel(void)
	: m_bUseRecentDocuments(false)
	, m_cipherAlgorithm(DISK_CIPHER_AES)
	, m_bQuickFormat(false)
	, m_imageSize(0)
{
}


CreateWizardModel::~CreateWizardModel(void)
{
}

void CreateWizardModel::DoCreateImage(std::function<void (double)> callback)
{
	PasswordBuilder pb(m_keyFiles, reinterpret_cast<const unsigned char*>(m_password.c_str()), m_password.size());

	CryptDiskHelpers::CreateImage(&AppRandom::instance(), ImageFilePath().c_str(), ImageSize(), CipherAlgorithm(),
		pb.Password(), pb.PasswordLength(),
		QuickFormat(),
		callback);
}
