
#pragma once

#pragma pack(push,16)
typedef	__declspec(align(16)) struct TWOFISH_KEY_CTX
{
	unsigned long	KeySched[40];

	unsigned long	SBox0[256];
	unsigned long	SBox1[256];
	unsigned long	SBox2[256];
	unsigned long	SBox3[256];

}TWOFISH_KEY_CTX;
#pragma pack(pop)

