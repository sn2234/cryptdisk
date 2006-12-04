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

//##ModelId=40E9962601E4
class DNDriverControl
{
  public:
	DNDriverControl()
	{
		initialized=0;
	}

	~DNDriverControl()
	{
		if(initialized)
		{
			Close();
		}
	}
	//##ModelId=40E9979802EE
	int Open(TCHAR *szDriverDeviceName);

	int Open(TCHAR *szDriverDeviceName,DWORD access);

	//##ModelId=40E997AD02DE
	int Close();

	//##ModelId=40E997F503A9
	DWORD Control(DWORD Code);

	//##ModelId=40E998AE0138
	DWORD Control(DWORD Code, unsigned char* Input, DWORD InputLen);

	//##ModelId=40E998770138
	DWORD Control(DWORD Code, unsigned char* Output, DWORD OutputLen, DWORD *Result);

	//##ModelId=40E998E90000
	DWORD Control(DWORD Code, unsigned char* Input, DWORD InputLen, unsigned char* Output, DWORD OutputLen, DWORD *Result);

  protected:
	//##ModelId=40E999D3001F
	HANDLE hDriver;

  private:
	//##ModelId=40E99DA60177
	int initialized;

};



#endif /* DNDRIVERCONTROL_H_HEADER_INCLUDED_BF160C0F */
