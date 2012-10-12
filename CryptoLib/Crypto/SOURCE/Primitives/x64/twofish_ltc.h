
#pragma once

#define ENDIAN_NEUTRAL

#include "tomcrypt_macros.h"

typedef struct TWOFISH_KEY_CTX
{
	ulong32 S[4][256], K[40];
}TWOFISH_KEY_CTX;

