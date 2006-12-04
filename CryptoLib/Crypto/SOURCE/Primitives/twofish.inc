; 
; Copyright (c) 2006, nobody
; All rights reserved.
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
; 
;     * Redistributions of source code must retain the above copyright
;       notice, this list of conditions and the following disclaimer.
;     * Redistributions in binary form must reproduce the above copyright
;       notice, this list of conditions and the following disclaimer in the
;       documentation and/or other materials provided with the distribution.
; 
; THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
; EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
; WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
; DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
; (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
; ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
; (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
; SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
; 

TWOFISH_CTX	struct	8
	
	KeySched	DWORD	40 dup(?)
	
	SBox0		DWORD	256 dup(?)
	SBox1		DWORD	256 dup(?)
	SBox2		DWORD	256 dup(?)
	SBox3		DWORD	256 dup(?)
	
TWOFISH_CTX	ends

TwofishSetKey		PROTO pCTX:DWORD,pUserKey:DWORD

TwofishEncBlock1	PROTO pCTX:DWORD,PlainText:DWORD,CipherText:DWORD
TwofishEncBlock2	PROTO pCTX:DWORD,Buff:DWORD

TwofishXorEnc1		PROTO pCTX:DWORD,XorData:DWORD,PlainText:DWORD,CipherText:DWORD
TwofishXorEnc2		PROTO pCTX:DWORD,XorData:DWORD,Buff:DWORD

TwofishDecBlock1	PROTO pCTX:DWORD,CipherText:DWORD,PlainText:DWORD
TwofishDecBlock2	PROTO pCTX:DWORD,Buff:DWORD

TwofishDecXor1		PROTO pCTX:DWORD,XorData:DWORD,CipherText:DWORD,PlainText:DWORD
TwofishDecXor2		PROTO pCTX:DWORD,XorData:DWORD,Buff:DWORD
