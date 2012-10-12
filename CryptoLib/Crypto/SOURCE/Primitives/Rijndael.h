
#pragma once

#ifdef __cplusplus
extern "C"{
#endif 

#ifdef _WIN64
// 64-bit stuff
#include "x64/AesWrapper.h"
#else
// 32-bit stuff
#include "x86/rj_opt.h"
#endif

extern void __stdcall SetupKey(RIJNDAEL_KEY_CTX *pCTX, void *pUserKey);

extern void __stdcall EncipherBlock1(const RIJNDAEL_KEY_CTX *pCTX, const void *PlainText, void *CipherText);
extern void __stdcall EncipherBlock2(const RIJNDAEL_KEY_CTX *pCTX, void *Buff);

extern void __stdcall XorAndEncipher1(const RIJNDAEL_KEY_CTX *pCTX, const void *XorData, const void *PlainText, void *CipherText);
extern void __stdcall XorAndEncipher2(const RIJNDAEL_KEY_CTX *pCTX, const void *XorData, void *Buff);

extern void __stdcall DecipherBlock1(const RIJNDAEL_KEY_CTX *pCTX, const void *CipherText, void *PlainText);
extern void __stdcall DecipherBlock2(const RIJNDAEL_KEY_CTX *pCTX, void *Buff);

extern void __stdcall DecipherAndXor1(const RIJNDAEL_KEY_CTX *pCTX, const void *XorData, const void *CipherText, void *PlainText);
extern void __stdcall DecipherAndXor2(const RIJNDAEL_KEY_CTX *pCTX, const void *XorData, void *Buff);


#ifdef __cplusplus
};
#endif 
