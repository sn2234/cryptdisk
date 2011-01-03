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

#include "stdafx.h"

#include "DNDriverControl.h"
#include "winapi_exception.h"

int DNDriverControl::Close()
{
	CloseHandle(hDriver);
	return 1;
}

DWORD DNDriverControl::Control(DWORD Code)
{
	DWORD dwResult;

	return DeviceIoControl(hDriver,Code,0,0,0,0,&dwResult,0);
}

DWORD DNDriverControl::Control(DWORD Code, unsigned char* Input, DWORD InputLen)
{
	DWORD dwResult;

	return DeviceIoControl(hDriver,Code,Input,InputLen,0,0,&dwResult,0);
}

DWORD DNDriverControl::Control(DWORD Code, unsigned char* Output, DWORD OutputLen, DWORD *Result)
{
	return DeviceIoControl(hDriver,Code,0,0,Output,OutputLen,Result,0);
}

DWORD DNDriverControl::Control(DWORD Code, unsigned char* Input, DWORD InputLen, unsigned char* Output, DWORD OutputLen, DWORD *Result)
{
	return DeviceIoControl(hDriver,Code,Input,InputLen,Output,OutputLen,Result,0);
}

DNDriverControl* DNDriverControl::Create( TCHAR *szDriverDeviceName, DWORD access /*= GENERIC_READ|GENERIC_WRITE*/ )
{
	HANDLE hDriver;

	hDriver=CreateFile(szDriverDeviceName, access, FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE, 0, OPEN_EXISTING, 0, 0);

	if(hDriver == INVALID_HANDLE_VALUE)
	{
		throw winapi_exception("Error opening driver");
	}

	return new DNDriverControl(hDriver);
}
