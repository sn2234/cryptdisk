#include "StdAfx.h"
#include "CreateWizardModel.h"


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
