#include "StdAfx.h"
#include "ManagerNew.h"
#include "DialogFormatProgress.h"
#include "afxdialogex.h"
#include "CryptDiskHelpers.h"
#include "AppRandom.h"
#include "CreateWizardModel.h"
#include "PasswordBuilder.h"


CreateWizardModel::CreateWizardModel(const VolumeDesk* descriptor)
	: m_bUseRecentDocuments(false)
	, m_cipherAlgorithm(DISK_CIPHER_AES)
	, m_bQuickFormat(false)
	, m_imageSize(0)
	, m_volumeDescriptor(descriptor != nullptr ? std::make_unique<VolumeDesk>(*descriptor) : nullptr)
	, m_isVolume(descriptor != nullptr)
{
}

void CreateWizardModel::DoCreate()
{
	PasswordBuilder pb(m_keyFiles, reinterpret_cast<const unsigned char*>(m_password.c_str()), m_password.size());

	std::function<void(std::function<bool(double)>)> processFunc;

	if (m_isVolume)
	{
		processFunc = [this, &pb](std::function<bool(double)> f){
			std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
			CryptDiskHelpers::EncryptVolume(&AppRandom::instance(), 
				conv.from_bytes(m_volumeDescriptor->deviceId).c_str(), CipherAlgorithm(),
				pb.Password(), pb.PasswordLength(), !QuickFormat(), f);
		};
	}
	else
	{
		processFunc = [this, &pb](std::function<bool(double)> f){
			CryptDiskHelpers::CreateImage(&AppRandom::instance(), ImageFilePath().c_str(), ImageSize(), CipherAlgorithm(),
				pb.Password(), pb.PasswordLength(), !QuickFormat(), f);
		};
	}

	DialogFormatProgress dlg(processFunc, NULL);

	dlg.DoModal();
}
