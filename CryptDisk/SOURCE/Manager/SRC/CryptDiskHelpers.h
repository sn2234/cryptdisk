#pragma once

#include "DNDriverControl.h"

class CryptDiskHelpers
{
public:
	static void MountImage(DNDriverControl& driverControl, const WCHAR* imagePath, WCHAR driveLetter, const char* password);
};
