
#pragma once

#include "aes.h"

#define	RJ_KEY_LEN_BITS			(256)
#define	RJ_KEY_LEN_BYTES		(RJ_KEY_LEN_BITS/8)

#define	RJ_BLOCK_LEN_BITS		(128)
#define	RJ_BLOCK_LEN_BYTES		(RJ_BLOCK_LEN_BITS/8)

#pragma pack(push,16)
typedef	__declspec(align(16)) struct	RIJNDAEL_KEY_CTX
{
	aes_encrypt_ctx		encCtx;
	aes_decrypt_ctx		decCtx;
}RIJNDAEL_KEY_CTX;



#pragma pack(pop)
