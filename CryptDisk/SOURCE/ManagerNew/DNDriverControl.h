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

#ifndef DNDRIVERCONTROL_H_HEADER_INCLUDED_BF160C0F
#define DNDRIVERCONTROL_H_HEADER_INCLUDED_BF160C0F

#include <windows.h>

class DNDriverControl : private boost::noncopyable
{
	DNDriverControl(HANDLE hDriver)
		: hDriver(hDriver)
	{
	}

	int Close();

public:

	virtual ~DNDriverControl()
	{
			Close();
	}

	static DNDriverControl* Create(const TCHAR *szDriverDeviceName, DWORD access = GENERIC_READ|GENERIC_WRITE);

	DWORD Control(DWORD Code);

	DWORD Control(DWORD Code, void* Input, DWORD InputLen);

	DWORD Control(DWORD Code, void* Output, DWORD OutputLen, DWORD *Result);

	DWORD Control(DWORD Code, void* Input, DWORD InputLen, void* Output, DWORD OutputLen, DWORD *Result);

  protected:
	HANDLE hDriver;
};



#endif /* DNDRIVERCONTROL_H_HEADER_INCLUDED_BF160C0F */
