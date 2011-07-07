
#include "stdafx.h"

#include "DriverTools.h"
#include "winapi_exception.h"

static TCHAR DriverServiceName[]=_TEXT("CryptDisk40");
static TCHAR DriverControlDeviceName[] = _T("\\\\.\\DNDiskControl40");

DriverHelper* InitDriverHelper::Alloc()
{
	try
	{
		return new DriverHelper(std::shared_ptr<DNDriverControl>(DNDriverControl::Create(DriverControlDeviceName)));
	}
	catch (winapi_exception& ex)
	{
		if(ex.error() == ERROR_FILE_NOT_FOUND)
		{
			// Start driver
			SC_HANDLE		hScm;
			SC_HANDLE		hDriverService;
			DWORD			dwResult;
			bool			Result=false;

			if(hScm=OpenSCManager(NULL,NULL,GENERIC_EXECUTE))
			{
				if(hDriverService=OpenService(hScm,DriverServiceName,GENERIC_EXECUTE))
				{
					if(StartService(hDriverService,0,NULL)||
						(GetLastError()==ERROR_SERVICE_ALREADY_RUNNING))
					{
						Result = true;
					}
					CloseServiceHandle(hDriverService);
				}
				CloseServiceHandle(hScm);
			}

			return Result ?  new DriverHelper(std::shared_ptr<DNDriverControl>(DNDriverControl::Create(DriverControlDeviceName))) :
				new DriverHelper(std::shared_ptr<DNDriverControl>(nullptr));
		}
	}
	catch (std::exception& ex)
	{
		return new DriverHelper(std::shared_ptr<DNDriverControl>(nullptr));
	}
}
