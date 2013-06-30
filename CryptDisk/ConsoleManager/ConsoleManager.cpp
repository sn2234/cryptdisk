// ConsoleManager.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "CryptDiskHelpers.h"
#include "DNDriverControl.h"
#include "DriverProtocol.h"

#include <Random/Random.h>

namespace po = boost::program_options;

using namespace std;
using namespace std::tr1;

static TCHAR DriverControlDeviceName[]=_TEXT("\\\\.\\DNDiskControl40");

shared_ptr<DNDriverControl> CreateDriverControl()
{
	return shared_ptr<DNDriverControl>(DNDriverControl::Create(DriverControlDeviceName));
}

shared_ptr<CryptoLib::RandomGeneratorBase> CreateRandomGen()
{
	shared_ptr<CryptoLib::RandomGeneratorBase> result(new CryptoLib::RandomGeneratorBase());

	vector<char> sample(512, 0);

	result->AddSample(&sample[0], static_cast<ULONG>(sample.size()), static_cast<ULONG>(sample.size()*8));

	return result;
}


void MountImage(const wstring& imagePath, const wstring& driveLetter, const string& password)
{
	shared_ptr<DNDriverControl> driverControl(CreateDriverControl());

	ULONG mountOptions = MOUNT_VIA_MOUNTMGR | MOUNT_SAVE_TIME;

	CryptDiskHelpers::MountImage(*driverControl, imagePath.c_str(), driveLetter[0], reinterpret_cast<const unsigned char*>(password.c_str()), password.size(), 0);
}

void CheckImage(const wstring& imagePath, const string& password)
{
	if(CryptDiskHelpers::CheckImage(imagePath.c_str(), reinterpret_cast<const unsigned char*>(password.c_str()), password.size()))
	{
		wcout << "Success" << endl;
	}
	else
	{
		wcout << "Failed" << endl;
	}
}

void ChangePassword(const wstring& imagePath, const string& password, const string& passwordNew)
{
	shared_ptr<CryptoLib::IRandomGenerator> randomGen = CreateRandomGen();
	CryptDiskHelpers::ChangePassword(randomGen.get(), imagePath.c_str(), reinterpret_cast<const unsigned char*>(password.c_str()), password.size(),
		reinterpret_cast<const unsigned char*>(passwordNew.c_str()), passwordNew.size());
}

void UnmountImage(ULONG diskId, bool forceUnmount)
{
	shared_ptr<DNDriverControl> driverControl(CreateDriverControl());

	CryptDiskHelpers::UnmountImage(*driverControl, diskId, forceUnmount);
}

void ListImages()
{
	shared_ptr<DNDriverControl> driverControl(CreateDriverControl());

	vector<MountedImageInfo> images(CryptDiskHelpers::ListMountedImages(*driverControl));

	for (vector<MountedImageInfo>::iterator it = images.begin(); it != images.end(); ++it)
	{
		wcout	<< "DiskId:\t" << (*it).diskId << endl
				<< "Letter:\t" << (*it).driveLetter << endl
				<< "Size:\t" << (*it).size << endl
				<< "Image:\t" << (*it).imageFilePath << endl;
	}
}

void CreateImage(const wstring& imagePath, const string& algoName, const string& password, INT64 imageSize)
{
	DISK_CIPHER algo;

	if(_stricmp(algoName.c_str(), "aes") == 0)
	{
		algo = DISK_CIPHER_AES;
	}
	else if(_stricmp(algoName.c_str(), "twofish") == 0)
	{
		algo = DISK_CIPHER_TWOFISH;
	}
	else
	{
		throw logic_error("Unknown algorithm");
	}

	shared_ptr<CryptoLib::IRandomGenerator> randomGen = CreateRandomGen();

	CryptDiskHelpers::CreateImage(randomGen.get(), imagePath.c_str(), imageSize, algo, reinterpret_cast<const unsigned char*>(password.c_str()), password.size(), true,
		[](double progress){ return true; });
}

int wmain(int argc, WCHAR* argv[])
{
	locale::global(locale(""));

	try
	{
		wstring imagePath;
		wstring driveLetter;
		bool forceUnmount;
		string password;
		string passwordNew;
		wstring operation;
		ULONG diskId;
		INT64 imageSize;
		string algoName;

		po::options_description desc("Program options");
		desc.add_options()
			("help", "produce help message")
			("op", po::wvalue<wstring>(&operation),
				"Operation to be performed: "
				"\n\t m - mount an image"
				"\n\t u - unmount image"
				"\n\t l - list all mounted images"
				"\n\t c - create image"
				"\n\t t - test image"
				"\n\t chp - change password")
			("image", po::wvalue<wstring>(&imagePath), "Path to image file to be mounted or created")
			("letter", po::wvalue<wstring>(&driveLetter), "Drive letter")
			("id", po::value<ULONG>(&diskId), "Disk id to unmount")
			("size", po::value<INT64>(&imageSize), "Image size")
			("password", po::value<string>(&password), "Password")
			("newpassword", po::value<string>(&passwordNew), "New password (for change password)")
			("algo", po::value<string>(&algoName), "Encryption algorithm (AES, Twofish)")
			("force", po::wvalue<bool>(&forceUnmount)->default_value(false, "false"), "Force unmount")
			;

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if(argc == 1 || vm.count("help") || (vm.count("op") != 1))
		{
			// Print help message

			cout << desc;
			return 0;
		}
		else if (operation == _T("m"))
		{
			// Mount image

			// Check for required parameters
			if (!vm.count("image") || !vm.count("letter") || !vm.count("password"))
			{
				cout << desc;
				return 0;
			}

			MountImage(imagePath, driveLetter, password);
		}
		else if (operation == _T("u"))
		{
			// Unmount image

			// Check for required parameters
			if (!vm.count("id"))
			{
				cout << desc;
				return 0;
			}

			UnmountImage(diskId, forceUnmount);
		}
		else if (operation == _T("l"))
		{
			// List images
			ListImages();
		}
		else if (operation == _T("c"))
		{
			// Create image
			// Check for required parameters
			if (!vm.count("image") || !vm.count("size")  || !vm.count("algo") || !vm.count("password"))
			{
				cout << desc;
				return 0;
			}

			CreateImage(imagePath, algoName, password, imageSize);
		}
		else if (operation == _T("t"))
		{
			// Test image
			// Check for required parameters
			if (!vm.count("image") || !vm.count("password"))
			{
				cout << desc;
				return 0;
			}

			CheckImage(imagePath, password);
		}
		else if (operation == _T("chp"))
		{
			// Change password
			// Check for required parameters
			if (!vm.count("image") || !vm.count("password") || !vm.count("newpassword"))
			{
				cout << desc;
				return 0;
			}

			ChangePassword(imagePath, password, passwordNew);
		}
		else
		{
			// Print help message

			cout << desc;
			return 0;
		}
	}
	catch (exception& e)
	{
		cout << "Exception occurs: " << e.what();
		cout.flush();
	}

	cout << endl;
	return 0;
}
