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

#ifndef	_LRW_AES_H_INCLUDED_
#define	_LRW_AES_H_INCLUDED_

#pragma once

#include "RijndaelEngine.h"

#define		ENGINE_BLOCK_SIZE	RJ_BLOCK_LEN_BYTES
#define		ENGINE_KEY_SIZE		RJ_KEY_LEN_BYTES

#define		AES_KEY_SIZE		RJ_KEY_LEN_BYTES
#ifndef AES_BLOCK_SIZE
#define		AES_BLOCK_SIZE		RJ_BLOCK_LEN_BYTES
#endif

#include "SymmetricCipherLRW.h"

namespace CryptoLib
{
class LRWAES: public SymmetricCipherLRW<RijndaelEngine>
{
};
};

#undef		ENGINE_BLOCK_SIZE
#undef		ENGINE_KEY_SIZE

#endif	//_LRW_AES_H_INCLUDED_