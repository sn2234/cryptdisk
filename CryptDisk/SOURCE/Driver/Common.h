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

#ifndef	_COMMON_H_INCLUDED
#define	_COMMON_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Typedefs
typedef		long	BOOL;

// Crypto includes
#include "..\Crypto\DNAES.h"
#include "..\Crypto\DNTwofish.h"
#include "..\Crypto\LRWMode.h"
#include "..\Crypto\LRWAES.h"
#include "..\Crypto\LRWTwofish.h"
#include "..\Crypto\SHA256_HASH.h"

// Driver includes
#include "..\Version.h"
#include "format.h"
#include "XPStuff.h"
#include "disk.h"
#include "DiskCipher.h"
#include "DiskHeader.h"
#include "Tools.h"
#include "VirtualDisk.h"
#include "DisksManager.h"
#include "main.h"

//
#pragma intrinsic(memset,memcpy,memcmp)

// Defines
#if DBG
#define		DnPrint(X)	DbgPrint("\nCryptDisk:"X)
#else
#define		DnPrint(X)
#endif

#endif	//_COMMON_H_INCLUDED