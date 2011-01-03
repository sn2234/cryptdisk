// ConsoleManager.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "CryptDiskHelpers.h"
#include "DNDriverControl.h"

namespace po = boost::program_options;

using namespace std;

static TCHAR DriverControlDeviceName[]=_TEXT("\\\\.\\DNDiskControl40");

shared_ptr<DNDriverControl> CreateDriverControl()
{
	return shared_ptr<DNDriverControl>(DNDriverControl::Create(DriverControlDeviceName));
}

void MountImage(const wstring& imagePath, const wstring& driveLetter, const string& password)
{
	shared_ptr<DNDriverControl> driverControl(CreateDriverControl());

	CryptDiskHelpers::MountImage(*driverControl, imagePath.c_str(), driveLetter[0], password.c_str());
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
		wstring operation;

		po::options_description desc("Program options");
		desc.add_options()
			("help", "produce help message")
			("op", po::wvalue<wstring>(&operation), "Operation to be performed: \n\t m - mount an image\n\t u - unmount image\n\t l - list all mounted images")
			("image", po::wvalue<wstring>(&imagePath), "Path to image file to be mounted")
			("letter", po::wvalue<wstring>(&driveLetter), "Drive letter")
			("password", po::value<string>(&password), "password")
			("force", po::wvalue<bool>(&forceUnmount)->default_value(false, "false"), "Force unmount")
			;

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if(argc == 1 || vm.count("help"))
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
			if (!vm.count("letter"))
			{
				cout << desc;
				return 0;
			}
		}
		else if (operation == _T("l"))
		{
			// List images
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
		cout << e.what();
	}

	cout << endl;
	return 0;
}
