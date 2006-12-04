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

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_DRIVERMANAGER_41D53A900340_INCLUDED
#define _INC_DRIVERMANAGER_41D53A900340_INCLUDED

//##ModelId=41D53A900340
class DriverManager 
{
public:
	DriverManager()
	{
		Initialized=false;
	}

	~DriverManager()
	{
		Close();
	}

	//Opens a new instance and loads driver if necessary
	//##ModelId=41D53B2701F3
	BOOL Open();

	//Closes instance, but not unloads driver
	//##ModelId=41D53B350243
	BOOL Close();

	//##ModelId=41D53B3C0301
	BOOL GetDisksCount(DISKS_COUNT_INFO *Info);

	//##ModelId=41D53B4B0277
	DWORD GetDisksInfo(DISK_BASIC_INFO* Buffer, int BufferSize);

	//##ModelId=41D53B550375
	BOOL InstallDisk(DISK_ADD_INFO* Info);

	//##ModelId=41D53B600385
	BOOL UninstallDisk(DISK_DELETE_INFO* Info);

	//Unload drier, uninstall all disks if exist any
	//##ModelId=41D53BD80099
	BOOL UnloadDriver();

private:
	//##ModelId=41D53C000028
	BOOL Initialized;

	//##ModelId=41D53C11025D
	DNDriverControl DriverControl;
};

#endif /* _INC_DRIVERMANAGER_41D53A900340_INCLUDED */
