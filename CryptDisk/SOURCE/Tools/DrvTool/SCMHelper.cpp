/*
* Copyright (c) 2006, nobody
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "StdAfx.h"
#include "SCMHelper.h"

BOOL CSCMHelper::RegService(LPCTSTR serviceName, LPCTSTR imagePath)
{
	SC_HANDLE	hSCM;
	SC_HANDLE	hService;
	BOOL		bRez=FALSE;

	TCHAR		fullPath[MAX_PATH];

	GetFullPathName(imagePath, _countof(fullPath), fullPath, NULL);

	hSCM=OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ALL_ACCESS);
	
	if(!hSCM)
	{
		return FALSE;
	}

	hService=CreateService(hSCM, serviceName, serviceName, GENERIC_ALL,
		SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
		fullPath, NULL, NULL, NULL, NULL, NULL);
	if(hService)
		bRez=TRUE;

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCM);
	return bRez;
}

BOOL CSCMHelper::UnregService(LPCTSTR serviceName)
{
	SC_HANDLE	hSCM;
	SC_HANDLE	hService;
	BOOL		bRez=FALSE;

	hSCM=OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE,
		SC_MANAGER_ALL_ACCESS);

	if(!hSCM)
	{
		return FALSE;
	}

	hService=OpenService(hSCM, serviceName,
		SERVICE_ALL_ACCESS);
	if(hService)
	{
		SERVICE_STATUS	stastus;

		ControlService(hService, SERVICE_CONTROL_STOP, &stastus);
		bRez=DeleteService(hService);
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCM);
	return bRez;
}

BOOL CSCMHelper::StartService(LPCTSTR serviceName)
{
	SC_HANDLE	hSCM;
	SC_HANDLE	hService;
	BOOL		bRez=FALSE;

	hSCM=OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE,
		GENERIC_EXECUTE);

	if(!hSCM)
	{
		return FALSE;
	}

	hService=OpenService(hSCM, serviceName,
		SERVICE_START);
	if(hService)
	{
		bRez=::StartService(hService, 0, NULL);
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCM);
	return bRez;
}

BOOL CSCMHelper::StopService(LPCTSTR serviceName)
{
	SC_HANDLE	hSCM;
	SC_HANDLE	hService;
	BOOL		bRez=FALSE;

	hSCM=OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE,
		GENERIC_EXECUTE);

	if(!hSCM)
	{
		return FALSE;
	}

	hService=OpenService(hSCM, serviceName,
		SERVICE_STOP);
	if(hService)
	{
		SERVICE_STATUS	stastus;

		bRez=ControlService(hService, SERVICE_CONTROL_STOP, &stastus);
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCM);
	return bRez;
}
