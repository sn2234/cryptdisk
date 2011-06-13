
#include "stdafx.h"

#include "DriverTools.h"

static TCHAR DriverControlDeviceName[] = _T("\\\\.\\DNDiskControl40");

DriverHelper* InitDriverHelper::Alloc()
{
	try
	{
		return new DriverHelper(std::shared_ptr<DNDriverControl>(DNDriverControl::Create(DriverControlDeviceName)));
	}
	catch (std::exception&)
	{
		return new DriverHelper(std::shared_ptr<DNDriverControl>(nullptr));
	}
}
