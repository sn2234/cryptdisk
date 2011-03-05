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

.686
.mmx

.model flat,stdcall
option casemap:none
option scoped
include rj_opt.inc
include rj_tables.inc

SubRot	macro	reg
		
		xlat
		rol		reg,8
		xlat
		rol		reg,8
		xlat
		rol		reg,8
		xlat
		
endm

SubByte	macro	reg
		
		SubRot		reg
		rol		reg,8
endm

GetByte	macro	dest, base, index
	iDword	=	index/4
;	iByte	=	index mod 4
	iByte	=	index AND 3
			
		mov		dest, dword ptr [base+iDword]
if iByte
		shr		dest, iByte*8
endif
		and		dest,0FFh
endm

rjCipherRoundTransform	macro	Dest,i0,i1,i2,i3
		
		mov		eax,[esi]	;get Rk
		add		esi,4
		
		GetByte	edx,ebx,i0*4+0
		GetByte	ecx,ebx,i1*4+1
		xor		eax,Te0[edx*4]
		xor		eax,Te1[ecx*4]
		
		GetByte	edx,ebx,i2*4+2
		GetByte	ecx,ebx,i3*4+3
		xor		eax,Te2[edx*4]
		xor		eax,Te3[ecx*4]
		movd		Dest,eax
		
endm

rjDecipherRoundTransform	macro	Dest,i0,i1,i2,i3
		
		mov		eax,[esi]	;get Rk
		add		esi,4
		
		GetByte	edx,ebx,i0*4+0
		GetByte	ecx,ebx,i1*4+1
		xor		eax,Td0[edx*4]
		xor		eax,Td1[ecx*4]
		
		GetByte	edx,ebx,i2*4+2
		GetByte	ecx,ebx,i3*4+3
		xor		eax,Td2[edx*4]
		xor		eax,Td3[ecx*4]
		movd		Dest,eax
		
endm

rjCipherFinalRound	macro	Dest,i0,i1,i2,i3
		
		GetByte	edx,ebx,i2*4+2
		GetByte	ecx,ebx,i3*4+3
		mov		al,byte ptr Se[edx]
		mov		ah,byte ptr Se[ecx]
		
		shl		eax,16
		
		GetByte	edx,ebx,i0*4+0
		GetByte	ecx,ebx,i1*4+1
		mov		al,byte ptr Se[edx]
		mov		ah,byte ptr Se[ecx]
		
		xor		eax,dword ptr [esi]
		movd		Dest,eax
		add		esi,4
		
endm

rjDecipherFinalRound	macro	Dest,i0,i1,i2,i3
		
		GetByte	edx,ebx,i2*4+2
		GetByte	ecx,ebx,i3*4+3
		mov		al,byte ptr Sd[edx]
		mov		ah,byte ptr Sd[ecx]
		
		shl		eax,16
		
		GetByte	edx,ebx,i0*4+0
		GetByte	ecx,ebx,i1*4+1
		mov		al,byte ptr Sd[edx]
		mov		ah,byte ptr Sd[ecx]
		
		xor		eax,dword ptr [esi]
		movd		Dest,eax
		add		esi,4
		
endm

.code

align 16
SetupKey	proc	pCTX:DWORD,pUserKey:DWORD
		
		pushad
; prepare cipher key shedule
		
; copy Nk dwords from user key to key enc
		mov		eax,pCTX
		lea		edi,(RIJNDAEL_KEY_CTX PTR [eax]).EncKey
		mov		esi,pUserKey
		xor		ecx,ecx
align 16
@@:
		cmp		ecx,Nk
		jae		@F
		
		mov		eax,[esi+ecx*4]
		mov		[edi+ecx*4],eax
		
		inc		ecx
		
		jmp	short	@B
align 16
@@:
;//////////////////
		mov		esi,offset Rcon
		mov		ebx,offset Se
align 16
@@:
;for(j=Nk;j<Nb*(Nr+1);j+=Nk)
		cmp		ecx,Nb*(Nr+1)		;ecx - j
		jae		@F
; W[j]=W[j-Nk]^SubByte(Rotl(W[j-1]))^Rkon[rcon_pointer++];
		mov		edx,[edi+ecx*4-Nk*4]
		lodsd
		xor		edx,eax
		mov		eax,[edi+ecx*4-1*4]
		SubRot		eax
		xor		eax,edx
		mov		[edi+ecx*4],eax
		
; W[j+1]=W[j+1-Nk]^W[j]
		xor		eax,[edi+ecx*4+(1-Nk)*4]
		mov		[edi+ecx*4+1*4],eax
; W[j+2]=W[j+2-Nk]^W[j+1]
		xor		eax,[edi+ecx*4+(2-Nk)*4]
		mov		[edi+ecx*4+2*4],eax
; W[j+3]=W[j+3-Nk]^W[j+2]
		xor		eax,[edi+ecx*4+(3-Nk)*4]
		mov		[edi+ecx*4+3*4],eax
		
		cmp		ecx,Nb*(Nr+1)-4
		jae		@F
		
; W[j+4]=W[j+4-Nk]^SubByte(W[j+3]);
		SubByte		eax
		xor		eax,[edi+ecx*4+(4-Nk)*4]
		mov		[edi+ecx*4+4*4],eax
		
; for(i=5;i<Nk;i++)
; W[i+j]=W[i+j-Nk]^W[j+i-1];
	i=5
	while	i lt Nk
		
		xor		eax,[edi+ecx*4+(i-Nk)*4]
		mov		[edi+ecx*4+i*4],eax
		
	i=i+1
	endm
		
		add		ecx,Nk
		jmp		@B
align 16
@@:

; prepare inverse cipher key shedule
		
		mov		eax,pCTX
		lea		esi,(RIJNDAEL_KEY_CTX PTR [eax]).DecKey
		
; invert the order of round keys
		xor		ebx,ebx
		mov		ecx,(Nb*(Nr+1))*4-Nb*4
align 16
@@:
		cmp		ebx,Nb*(Nr+1)*4
		jae		@F
		
		movq		mm0,[edi+ebx]
		movq		mm1,[edi+ebx+8]
		movq		[esi+ecx],mm0
		movq		[esi+ecx+8],mm1
		
		add		ebx,Nb*4
		sub		ecx,Nb*4
		jmp	short	@B
align 16
@@:
		
		add		esi,Nb*4
		mov		ecx,Nr-1
		mov		ebx,offset Se
		xor		eax,eax
align 16
@@:
	rept 4
		lodsb					; byte 0
		xlat
		mov		edx,Td0[eax*4]
		lodsb					; byte 1
		xlat
		xor		edx,Td1[eax*4]
		lodsb					; byte 2
		xlat
		xor		edx,Td2[eax*4]
		lodsb					; byte 3
		xlat
		xor		edx,Td3[eax*4]
		mov		dword ptr [esi-4],edx
	endm
		dec		ecx
		jnz		@B
		
		emms
		popad
		ret

SetupKey	endp

align 16
EncipherBlock1	proc	pCTX:DWORD,PlainText:DWORD,CipherText:DWORD
		
		pushad
		
		mov		eax,pCTX
		lea		esi,(RIJNDAEL_KEY_CTX PTR [eax]).EncKey
		mov		ebx,CipherText
		mov		edi,PlainText
		
		movq		mm0,[edi+0*8]
		movq		mm1,[edi+1*8]
		movq		mm2,[esi+0*8]
		movq		mm3,[esi+1*8]
		pxor		mm0,mm2
		pxor		mm1,mm3
		movq		[ebx+0*8],mm0
		movq		[ebx+1*8],mm1
		add		esi,Nb*4
		
		xor		edx,edx
		xor		ecx,ecx
		
	rept	Nr-1
		rjCipherRoundTransform	mm0,0,1,2,3
		rjCipherRoundTransform	mm1,1,2,3,0
		rjCipherRoundTransform	mm2,2,3,0,1
		rjCipherRoundTransform	mm3,3,0,1,2

		movd		dword ptr [ebx+0*4],mm0
		movd		dword ptr [ebx+1*4],mm1
		movd		dword ptr [ebx+2*4],mm2
		movd		dword ptr [ebx+3*4],mm3
	endm
		
		rjCipherFinalRound	mm0,0,1,2,3
		rjCipherFinalRound	mm1,1,2,3,0
		rjCipherFinalRound	mm2,2,3,0,1
		rjCipherFinalRound	mm3,3,0,1,2
		
		movd		dword ptr [ebx+0*4],mm0
		movd		dword ptr [ebx+1*4],mm1
		movd		dword ptr [ebx+2*4],mm2
		movd		dword ptr [ebx+3*4],mm3
		
		emms
		popad
		ret
EncipherBlock1	endp

align 16
EncipherBlock2	proc	pCTX:DWORD,Buff:DWORD
		
		pushad
		
		mov		eax,pCTX
		lea		esi,(RIJNDAEL_KEY_CTX PTR [eax]).EncKey
		mov		ebx,Buff
		
		movq		mm0,[ebx+0*8]
		movq		mm1,[ebx+1*8]
		movq		mm2,[esi+0*8]
		movq		mm3,[esi+1*8]
		pxor		mm0,mm2
		pxor		mm1,mm3
		movq		[ebx+0*8],mm0
		movq		[ebx+1*8],mm1
		add		esi,Nb*4
		
		xor		edx,edx
		xor		ecx,ecx
		
	rept	Nr-1
		rjCipherRoundTransform	mm0,0,1,2,3
		rjCipherRoundTransform	mm1,1,2,3,0
		rjCipherRoundTransform	mm2,2,3,0,1
		rjCipherRoundTransform	mm3,3,0,1,2
		
		movd		dword ptr [ebx+0*4],mm0
		movd		dword ptr [ebx+1*4],mm1
		movd		dword ptr [ebx+2*4],mm2
		movd		dword ptr [ebx+3*4],mm3
	endm
		
		rjCipherFinalRound	mm0,0,1,2,3
		rjCipherFinalRound	mm1,1,2,3,0
		rjCipherFinalRound	mm2,2,3,0,1
		rjCipherFinalRound	mm3,3,0,1,2
		
		movd		dword ptr [ebx+0*4],mm0
		movd		dword ptr [ebx+1*4],mm1
		movd		dword ptr [ebx+2*4],mm2
		movd		dword ptr [ebx+3*4],mm3
		
		emms
		popad
		ret
		
EncipherBlock2	endp

align 16
XorAndEncipher1	proc	pCTX:DWORD,XorData:DWORD,PlainText:DWORD,CipherText:DWORD
		
		pushad
		
		mov		eax,pCTX
		lea		esi,(RIJNDAEL_KEY_CTX PTR [eax]).EncKey
		mov		ebx,CipherText
		mov		edi,PlainText
		mov		eax,XorData
		
		movq		mm0,[eax+0*8]
		movq		mm1,[eax+1*8]
		movq		mm2,[edi+0*8]
		movq		mm3,[edi+1*8]
		pxor		mm0,mm2
		pxor		mm1,mm3
		movq		mm2,[esi+0*8]
		movq		mm3,[esi+1*8]
		pxor		mm0,mm2
		pxor		mm1,mm3
		movq		[ebx+0*8],mm0
		movq		[ebx+1*8],mm1
		add		esi,Nb*4
		
		xor		edx,edx
		xor		ecx,ecx
		
	rept	Nr-1
		rjCipherRoundTransform	mm0,0,1,2,3
		rjCipherRoundTransform	mm1,1,2,3,0
		rjCipherRoundTransform	mm2,2,3,0,1
		rjCipherRoundTransform	mm3,3,0,1,2
		
		movd		dword ptr [ebx+0*4],mm0
		movd		dword ptr [ebx+1*4],mm1
		movd		dword ptr [ebx+2*4],mm2
		movd		dword ptr [ebx+3*4],mm3
	endm
		
		rjCipherFinalRound	mm0,0,1,2,3
		rjCipherFinalRound	mm1,1,2,3,0
		rjCipherFinalRound	mm2,2,3,0,1
		rjCipherFinalRound	mm3,3,0,1,2
		
		movd		dword ptr [ebx+0*4],mm0
		movd		dword ptr [ebx+1*4],mm1
		movd		dword ptr [ebx+2*4],mm2
		movd		dword ptr [ebx+3*4],mm3
		
		emms
		popad
		ret
		
XorAndEncipher1	endp

align 16
XorAndEncipher2	proc	pCTX:DWORD,XorData:DWORD,Buff:DWORD
		
		pushad
		
mov		eax,pCTX
		lea		esi,(RIJNDAEL_KEY_CTX PTR [eax]).EncKey
		mov		ebx,Buff
		mov		eax,XorData
		
		movq		mm0,[eax+0*8]
		movq		mm1,[eax+1*8]
		movq		mm2,[ebx+0*8]
		movq		mm3,[ebx+1*8]
		pxor		mm0,mm2
		pxor		mm1,mm3
		movq		mm2,[esi+0*8]
		movq		mm3,[esi+1*8]
		pxor		mm0,mm2
		pxor		mm1,mm3
		movq		[ebx+0*8],mm0
		movq		[ebx+1*8],mm1
		add		esi,Nb*4
		
		xor		edx,edx
		xor		ecx,ecx
		
	rept	Nr-1
		rjCipherRoundTransform	mm0,0,1,2,3
		rjCipherRoundTransform	mm1,1,2,3,0
		rjCipherRoundTransform	mm2,2,3,0,1
		rjCipherRoundTransform	mm3,3,0,1,2
		
		movd		dword ptr [ebx+0*4],mm0
		movd		dword ptr [ebx+1*4],mm1
		movd		dword ptr [ebx+2*4],mm2
		movd		dword ptr [ebx+3*4],mm3
	endm
		
		rjCipherFinalRound	mm0,0,1,2,3
		rjCipherFinalRound	mm1,1,2,3,0
		rjCipherFinalRound	mm2,2,3,0,1
		rjCipherFinalRound	mm3,3,0,1,2
		
		movd		dword ptr [ebx+0*4],mm0
		movd		dword ptr [ebx+1*4],mm1
		movd		dword ptr [ebx+2*4],mm2
		movd		dword ptr [ebx+3*4],mm3
		
		emms
		popad
		ret
		
XorAndEncipher2	endp

align 16
DecipherBlock1	proc	pCTX:DWORD,CipherText:DWORD,PlainText:DWORD
		
		pushad
		
		mov		eax,pCTX
		lea		esi,(RIJNDAEL_KEY_CTX PTR [eax]).DecKey
		mov		ebx,PlainText
		mov		edi,CipherText
		
		movq		mm0,[edi+0*8]
		movq		mm1,[edi+1*8]
		movq		mm2,[esi+0*8]
		movq		mm3,[esi+1*8]
		pxor		mm0,mm2
		pxor		mm1,mm3
		movq		[ebx+0*8],mm0
		movq		[ebx+1*8],mm1
		add		esi,Nb*4
		
		xor		edx,edx
		xor		ecx,ecx
		
	rept	Nr-1
		rjDecipherRoundTransform	mm0,0,3,2,1
		rjDecipherRoundTransform	mm1,1,0,3,2
		rjDecipherRoundTransform	mm2,2,1,0,3
		rjDecipherRoundTransform	mm3,3,2,1,0
		
		movd		dword ptr [ebx+0*4],mm0
		movd 		dword ptr [ebx+1*4],mm1
		movd		dword ptr [ebx+2*4],mm2
		movd		dword ptr [ebx+3*4],mm3
	endm
		
		rjDecipherFinalRound	mm0,0,3,2,1
		rjDecipherFinalRound	mm1,1,0,3,2
		rjDecipherFinalRound	mm2,2,1,0,3
		rjDecipherFinalRound	mm3,3,2,1,0
		
		movd		dword ptr [ebx+0*4],mm0
		movd		dword ptr [ebx+1*4],mm1
		movd		dword ptr [ebx+2*4],mm2
		movd		dword ptr [ebx+3*4],mm3
		
		emms
		popad
		ret
		
DecipherBlock1	endp

align 16
DecipherBlock2	proc	pCTX:DWORD,Buff:DWORD
		
		pushad
		
		mov		eax,pCTX
		lea		esi,(RIJNDAEL_KEY_CTX PTR [eax]).DecKey
		mov		ebx,Buff
		
		movq		mm0,[ebx+0*8]
		movq		mm1,[ebx+1*8]
		movq		mm2,[esi+0*8]
		movq		mm3,[esi+1*8]
		pxor		mm0,mm2
		pxor		mm1,mm3
		movq		[ebx+0*8],mm0
		movq		[ebx+1*8],mm1
		add		esi,Nb*4
		
		xor		edx,edx
		xor		ecx,ecx
		
	rept	Nr-1
		rjDecipherRoundTransform	mm0,0,3,2,1
		rjDecipherRoundTransform	mm1,1,0,3,2
		rjDecipherRoundTransform	mm2,2,1,0,3
		rjDecipherRoundTransform	mm3,3,2,1,0
		
		movd		dword ptr [ebx+0*4],mm0
		movd		dword ptr [ebx+1*4],mm1
		movd		dword ptr [ebx+2*4],mm2
		movd		dword ptr [ebx+3*4],mm3
	endm
		
		rjDecipherFinalRound	mm0,0,3,2,1
		rjDecipherFinalRound	mm1,1,0,3,2
		rjDecipherFinalRound	mm2,2,1,0,3
		rjDecipherFinalRound	mm3,3,2,1,0
		
		movd		dword ptr [ebx+0*4],mm0
		movd		dword ptr [ebx+1*4],mm1
		movd		dword ptr [ebx+2*4],mm2
		movd		dword ptr [ebx+3*4],mm3
		
		emms
		popad
		ret
		
DecipherBlock2	endp

align 16
DecipherAndXor1	proc	pCTX:DWORD,XorData:DWORD,CipherText:DWORD,PlainText:DWORD
		
		pushad
		
		mov		eax,pCTX
		lea		esi,(RIJNDAEL_KEY_CTX PTR [eax]).DecKey
		mov		ebx,PlainText
		mov		edi,CipherText
		
		movq		mm0,[edi+0*8]
		movq		mm1,[edi+1*8]
		movq		mm2,[esi+0*8]
		movq		mm3,[esi+1*8]
		pxor		mm0,mm2
		pxor		mm1,mm3
		movq		[ebx+0*8],mm0
		movq		[ebx+1*8],mm1
		add		esi,Nb*4
		
		xor		edx,edx
		xor		ecx,ecx
		
	rept	Nr-1
		rjDecipherRoundTransform	mm0,0,3,2,1
		rjDecipherRoundTransform	mm1,1,0,3,2
		rjDecipherRoundTransform	mm2,2,1,0,3
		rjDecipherRoundTransform	mm3,3,2,1,0
		
		movd		dword ptr [ebx+0*4],mm0
		movd 		dword ptr [ebx+1*4],mm1
		movd		dword ptr [ebx+2*4],mm2
		movd		dword ptr [ebx+3*4],mm3
	endm
		
		rjDecipherFinalRound	mm0,0,3,2,1
		rjDecipherFinalRound	mm1,1,0,3,2
		rjDecipherFinalRound	mm2,2,1,0,3
		rjDecipherFinalRound	mm3,3,2,1,0
		
		mov		eax,XorData
		
		psllq		mm1,32
		por		mm0,mm1
		psllq		mm3,32
		por		mm2,mm3
		
		movq		mm1,[eax+0*8]
		movq		mm3,[eax+1*8]
		
		pxor		mm0,mm1
		pxor		mm2,mm3
		
		movq		[ebx+0*8],mm0
		movq		[ebx+1*8],mm2
		
		emms
		popad
		ret
		
DecipherAndXor1	endp

align 16
DecipherAndXor2	proc	pCTX:DWORD,XorData:DWORD,Buff:DWORD
		
		pushad
		
		mov		eax,pCTX
		lea		esi,(RIJNDAEL_KEY_CTX PTR [eax]).DecKey
		mov		ebx,Buff
		
		movq		mm0,[ebx+0*8]
		movq		mm1,[ebx+1*8]
		movq		mm2,[esi+0*8]
		movq		mm3,[esi+1*8]
		pxor		mm0,mm2
		pxor		mm1,mm3
		movq		[ebx+0*8],mm0
		movq		[ebx+1*8],mm1
		add		esi,Nb*4
		
		xor		edx,edx
		xor		ecx,ecx
		
	rept	Nr-1
		rjDecipherRoundTransform	mm0,0,3,2,1
		rjDecipherRoundTransform	mm1,1,0,3,2
		rjDecipherRoundTransform	mm2,2,1,0,3
		rjDecipherRoundTransform	mm3,3,2,1,0
		
		movd		dword ptr [ebx+0*4],mm0
		movd 		dword ptr [ebx+1*4],mm1
		movd		dword ptr [ebx+2*4],mm2
		movd		dword ptr [ebx+3*4],mm3
	endm
		
		rjDecipherFinalRound	mm0,0,3,2,1
		rjDecipherFinalRound	mm1,1,0,3,2
		rjDecipherFinalRound	mm2,2,1,0,3
		rjDecipherFinalRound	mm3,3,2,1,0
		
		mov		eax,XorData
		
		psllq		mm1,32
		por		mm0,mm1
		psllq		mm3,32
		por		mm2,mm3
		
		movq		mm1,[eax+0*8]
		movq		mm3,[eax+1*8]
		
		pxor		mm0,mm1
		pxor		mm2,mm3
		
		movq		[ebx+0*8],mm0
		movq		[ebx+1*8],mm2
		
		emms
		popad
		ret
		
DecipherAndXor2	endp

end 
