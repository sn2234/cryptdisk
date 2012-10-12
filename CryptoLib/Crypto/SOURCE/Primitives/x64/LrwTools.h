
#ifndef _LrwTools_H_INCLUDED_
#define _LrwTools_H_INCLUDED_

#pragma once

int GetBit(unsigned char *pSrc, int bitNumber);
void SetBit1(unsigned char *pSrc, int bitNumber);
void LShift128(unsigned char *pNumber);
void MirrorBits(unsigned char *pDest, unsigned char *pSrc);
void Swap128(unsigned char *pNumber);
void Inc128(unsigned char *pNumber);

#endif // _LrwTools_H_INCLUDED_
