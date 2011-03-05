
#pragma once

#ifdef __cplusplus
extern "C"{
#endif 


#define		Nk		8
#define		Nb		4
#define		Nr		14

#define	RJ_KEY_LEN_BITS			(256)
#define	RJ_KEY_LEN_BYTES		(RJ_KEY_LEN_BITS/8)

#define	RJ_BLOCK_LEN_BITS		(128)
#define	RJ_BLOCK_LEN_BYTES		(RJ_BLOCK_LEN_BITS/8)

#pragma pack(push,16)
typedef	__declspec(align(16)) struct	RIJNDAEL_KEY_CTX
{

	unsigned char	EncKey[Nb*(Nr+1)*4];
	unsigned char	DecKey[Nb*(Nr+1)*4];

}RIJNDAEL_KEY_CTX;
#pragma pack(pop)

#ifdef __cplusplus
};
#endif 
