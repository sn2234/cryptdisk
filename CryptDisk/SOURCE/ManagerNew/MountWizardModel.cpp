
#include "StdAfx.h"
#include "ManagerNew.h"

#include "MountWizardModel.h"

#include "CryptDiskHelpers.h"
#include "PasswordBuilder.h"

#include "DriverProtocol.h"
#include "DriverTools.h"
#include "DialogChangePassword.h"
#include "AppFavorites.h"

MountWizardModel::MountWizardModel(const VolumeDesk* descriptor)
	: m_useMountManager(true)
	, m_preserveImageTimestamp(true)
	, m_driveLetter(L'A')
	, m_bUseRecentDocuments(false)
	, m_mountAsReadOnly(false)
	, m_mountAsRemovable(false)
	, m_addToFavorites(false)
	, m_isVolume(descriptor != nullptr)
	, m_volumeDescriptor(descriptor != nullptr ? std::make_unique<VolumeDesk>(*descriptor) : nullptr)
{
}


MountWizardModel::~MountWizardModel(void)
{
	if(!m_password.empty())
	{
		RtlSecureZeroMemory(&m_password[0], m_password.size());
	}
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

	if(m_addToFavorites)
	{
		AppFavorites::instance().Favorites().push_back(
			FavoriteImage(m_imageFilePath, m_driveLetter, m_mountAsReadOnly, m_mountAsRemovable, m_useMountManager, m_preserveImageTimestamp));
		AppFavorites::instance().UpdateViews();
	}
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
