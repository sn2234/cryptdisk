#include "StdAfx.h"
#include "ManagerNew.h"
#include "DialogFormatProgress.h"
#include "afxdialogex.h"
#include "CryptDiskHelpers.h"
#include "AppRandom.h"
#include "CreateWizardModel.h"
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

void CreateWizardModel::DoCreateImage()
{
	PasswordBuilder pb(m_keyFiles, reinterpret_cast<const unsigned char*>(m_password.c_str()), m_password.size());

	DialogFormatProgress dlg(ImageFilePath().c_str(), ImageSize(), CipherAlgorithm(),
		pb.Password(), pb.PasswordLength(), QuickFormat());

	dlg.DoModal();
}

void CreateWizardModel::RefreshVolumes()
{
	m_volumes = m_volumeTools.enumVolumes();

	UpdateViews();
}
