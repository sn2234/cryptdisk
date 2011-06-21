
#include "StdAfx.h"
#include "MountWizardModel.h"

#include "CryptDiskHelpers.h"
#include "PasswordBuilder.h"

MountWizardModel::MountWizardModel(void)
	: m_useMountManager(true)
	, m_preserveImageTimestamp(true)
{
}


MountWizardModel::~MountWizardModel(void)
{
}

bool MountWizardModel::TryOpenImage() const
{
	PasswordBuilder pb(m_keyFiles, reinterpret_cast<const unsigned char*>(m_password.c_str()), m_password.size());
	return CryptDiskHelpers::CheckImage(m_imageFilePath.c_str(), pb.Password(), pb.PasswordLength());
}

void MountWizardModel::PerformMount()
{

}
