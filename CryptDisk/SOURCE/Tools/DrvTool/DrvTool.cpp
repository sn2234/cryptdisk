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

// DrvTool.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "SCMHelper.h"

const TCHAR *szTitle   = _T("[DrvTool 1.0] by nobody\n")
						_T("--------------------\n");
const TCHAR *szArgs       = _T("Command line:\n")
_T(" DrvTool <command> <driver name> [driver path]\n")
_T("  Commands:\n")
_T("   /REG           - register driver, driver path must be specified\n")
_T("   /UNREG         - unregister driver\n")
_T("   /RUN           - start driver\n")
_T("   /STOP          - stop driver\n")
_T(" Quotes are allowed and options must be interrupted by spaces.\n");

void PrintUsage();
void RegDriver(LPCTSTR driverName, LPCTSTR driverPath);
void UnregDriver(LPCTSTR driverName);
void RunDriver(LPCTSTR driverName);
void StopDriver(LPCTSTR driverName);

int _tmain(int argc, _TCHAR* argv[])
{
	if(argc < 3)
	{
		PrintUsage();
		return 1;
	}

	if(!_tcsicmp(_T("/REG"), argv[1]))
	{
		// Register driver
		if(argc < 4)
		{
			PrintUsage();
		}
		else
		{
			RegDriver(argv[2], argv[3]);
		}
	}
	else if(!_tcsicmp(_T("/UNREG"), argv[1]))
	{
		// Unregister driver
		UnregDriver(argv[2]);
	}
	else if(!_tcsicmp(_T("/RUN"), argv[1]))
	{
		// Start driver
		RunDriver(argv[2]);
	}
	else if(!_tcsicmp(_T("/STOP"), argv[1]))
	{
		// Stop driver
		StopDriver(argv[2]);
	}
	else
	{
		PrintUsage();
	}

	return 0;
}

void PrintUsage()
{
	_tprintf(szTitle);
	_tprintf(szArgs);
}

void RegDriver(LPCTSTR driverName, LPCTSTR driverPath)
{
	//_tprintf(_T("\nReg, Name: %s Path: %s"), driverName, driverPath);

	_tprintf(_T("Registering driver... "));
	if(CSCMHelper::RegService(driverName, driverPath))
	{
		_tprintf(_T("OK\n"));
	}
	else
	{
		_tprintf(_T("Failed\n"));
	}
}

void UnregDriver(LPCTSTR driverName)
{
	//_tprintf(_T("\nUnreg, Name: %s"), driverName);

	_tprintf(_T("Unregistering driver... "));
	if(CSCMHelper::UnregService(driverName))
	{
		_tprintf(_T("OK\n"));
	}
	else
	{
		_tprintf(_T("Failed\n"));
	}
}

void RunDriver(LPCTSTR driverName)
{
	//_tprintf(_T("\nRun, Name: %s"), driverName);

	_tprintf(_T("Starting driver... "));
	if(CSCMHelper::StartService(driverName))
	{
		_tprintf(_T("OK\n"));
	}
	else
	{
		_tprintf(_T("Failed\n"));
	}
}

void StopDriver(LPCTSTR driverName)
{
	//_tprintf(_T("\nStop, Name: %s"), driverName);

	_tprintf(_T("Stopping driver... "));
	if(CSCMHelper::StopService(driverName))
	{
		_tprintf(_T("OK\n"));
	}
	else
	{
		_tprintf(_T("Failed\n"));
	}
}
