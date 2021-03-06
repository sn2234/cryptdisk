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

#ifndef	_CRYPT_LIB_H_INCLUDED_
#define	_CRYPT_LIB_H_INCLUDED_

#pragma once

// Include all headers

#include "BaseCrypt/SHA256_HASH.h"
#include "BaseCrypt/RC4_CIPHER_ENGINE.h"

#include "BaseCrypt/DeriveKey.h"
#include "BaseCrypt/DNAES.h"
#include "BaseCrypt/DNTwofish.h"
#include "BaseCrypt/LRWMode.h"
#include "BaseCrypt/LRWTwofish.h"
#include "BaseCrypt/LRWAes.h"

#include "BaseCrypt/EncryptionModes.h"
#include "BaseCrypt/Hmac.h"
#include "BaseCrypt/Pkcs5.h"

#include "Random/RndGenerator.h"
#include "Random/RndSampler.h"
#include "Random/RndPool.h"

#include "Random/IRandomGenerator.h"
#include "Random/RandomGeneratorBase.h"

#include "Random/Random.h"

#endif	//_CRYPT_LIB_H_INCLUDED_
