
#include "StdAfx.h"
#include "ManagerNew.h"

#include "MountWizardModel.h"

#include "CryptDiskHelpers.h"
#include "PasswordBuilder.h"

#include "DriverProtocol.h"
#include "DriverTools.h"
#include "DialogChangePassword.h"

MountWizardModel::MountWizardModel(void)
	: m_useMountManager(true)
	, m_preserveImageTimestamp(true)
	, m_driveLetter(L'A')
	, m_bUseRecentDocuments(false)
	, m_mountAsReadOnly(false)
	, m_mountAsRemovable(false)
	, m_addToFavorites(true)
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
	PasswordBuilder pb(m_keyFiles, reinterpret_cast<const unsigned char*>(m_password.c_str()), m_password.size());
	
	ULONG mountOptions = 0;

	if(m_useMountManager) mountOptions |= MOUNT_VIA_MOUNTMGR;
	if(m_preserveImageTimestamp) mountOptions |= MOUNT_SAVE_TIME;
	if(m_mountAsReadOnly) mountOptions |= MOUNT_READ_ONLY;
	if(m_mountAsRemovable) mountOptions |= MOUNT_AS_REMOVABLE;

	CryptDiskHelpers::MountImage(*AppDriver::instance().getDriverControl(), m_imageFilePath.c_str(), m_driveLetter, pb.Password(), pb.PasswordLength(), mountOptions);
}

void MountWizardModel::ChangePassword()
{
	PasswordBuilder pb(m_keyFiles, reinterpret_cast<const unsigned char*>(m_password.c_str()), m_password.size());

	if(!CryptDiskHelpers::CheckImage(m_imageFilePath.c_str(), pb.Password(), pb.PasswordLength()))
	{
		throw std::invalid_argument("Unable to open image");
	}
	
	DialogChangePassword dlg(ImageFilePath(), pb.Password(), pb.PasswordLength());

	dlg.DoModal();
}
