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

include	Twofish.inc

include	Twotables.inc

	SK_STEP		equ	01010101h
	SK_ROUNDS	equ	20
	SK_END		equ	SK_STEP*SK_ROUNDS*2
	
.code

MUL_BYTE	macro	table
		
		test		ebx,ebx
		jz		@F
		
		movzx		eax, &table&_log [ecx]
		movzx		edx, &table&_log [ebx]
		add		eax,edx
		mov		edx,0FFh
		div		dl
		movzx		eax,ah
		movzx		edx, &table&_alog[eax]
		or		edi,edx
@@:
endm

MUL_DWORD	macro	a,b,table
LOCAL	proc_exit

		mov		ecx,a
		and		ecx,0FFh
		
		xor		eax,eax
		test		ecx,ecx
		jz		proc_exit
		
		mov		eax,b
		cmp		ecx,1
		jz		proc_exit
		
		mov		esi,eax
		bswap		esi
		
; Process byte 3 of b
		mov		ebx,esi
		and		ebx,0FFh
		
		xor		eax,eax
		xor		edi,edi
		
		MUL_BYTE	table
; Process byte 2 of b
		shr		esi,8
		shl		edi,8
		
		mov		ebx,esi
		and		ebx,0FFh
		
		MUL_BYTE	table
; Process byte 1 of b
		shr		esi,8
		shl		edi,8
		
		mov		ebx,esi
		and		ebx,0FFh
		
		MUL_BYTE	table
; Process byte 0 of b
		shr		esi,8
		shl		edi,8
		
		mov		ebx,esi
		and		ebx,0FFh
		
		MUL_BYTE	table
		
		mov		eax,edi
proc_exit:
endm
MDSMul	proc	uses ebx edi esi ecx edx, a:DWORD,b:DWORD
;Perform multiplication each byte of b with byte 0 of a

;Parameters
; b - dword constant
; a - only first byte meaning

;Registers:
; eax,ecx,ebx,edx,esi

;Load data into the registers
		
		MUL_DWORD	a,b,mds
		ret
MDSMul	endp

MDSEnc	proc	uses ebx edi esi,	x:dword
;MDS-encode two bytes k0 and k1

;Registers: eax,ecx,edx
		
		mov		ecx,x
		
		push		0EFEF5B01H
		push		ecx
		call		MDSMul
		mov		edx,eax
		
		shr		ecx,8
		push		015BEFEFH
		push		ecx
		call		MDSMul
		xor		edx,eax
		
		shr		ecx,8
		push		0EF01EF5BH
		push		ecx
		call		MDSMul
		xor		edx,eax
		
		shr		ecx,8
		push		5BEF015BH
		push		ecx
		call		MDSMul
		xor		edx,eax
		
		mov		eax,edx
		ret
MDSEnc	endp

RSMul	proc	uses ecx edx, a:DWORD,b:DWORD
;Perform multiplication each byte of b with byte 0 of a

;Parameters
; b - dword constant
; a - only first byte meaning

;Registers:
; eax,ecx,ebx,edx,esi

;Load data into the registers
		
		MUL_DWORD	a,b,rs
		ret
RSMul	endp

RSEnc	proc	uses ebx edi esi,	k0:dword,k1:dword
;RS-encode two bytes k0 and k1

;Registers: eax,ecx,edx
		
		mov		ecx,k0
		
		push		0A402A401h	;01A402A4h
		push		ecx
		call		RSMul
		mov		edx,eax
		
		shr		ecx,8
		push		55A156A4H	;0A456A155h
		push		ecx
		call		RSMul
		xor		edx,eax
		
		shr		ecx,8
		push		87FC8255H	;05582FC87h
		push		ecx
		call		RSMul
		xor		edx,eax
		
		shr		ecx,8
		push		5AC1F387h	;87F3C15Ah
		push		ecx
		call		RSMul
		xor		edx,eax
		
		mov		ecx,k1
		
		push		58471E5AH	;5A1E4758h
		push		ecx
		call		RSMul
		xor		edx,eax
		
		shr		ecx,8
		push		0DBAEC658h	;58C6AEDBh
		push		ecx
		call		RSMul
		xor		edx,eax
		
		shr		ecx,8
		push		9E3D68DBH	;0DB683D9Eh
		push		ecx
		call		RSMul
		xor		edx,eax
		
		shr		ecx,8
		push		0319E59EH	;9EE51903h
		push		ecx
		call		RSMul
		xor		edx,eax
		
		mov		eax,edx
		ret
RSEnc	endp

H_ROUND	macro	p0,p1,p2,p3,n

; Substitute byte #0
		mov		eax,edi
		and		eax,0FFh
		mov		bl,p0[eax]
		shl		ebx,8

; Substitute byte #1
		shr		edi,8
		mov		eax,edi
		and		eax,0FFh
		mov		bl,p1[eax]
		shl		ebx,8
		
; Substitute byte #2
		shr		edi,8
		mov		eax,edi
		and		eax,0FFh
		mov		bl,p2[eax]
		shl		ebx,8
		
; Substitute byte #3
		shr		edi,8
		mov		eax,edi
		and		eax,0FFh
		mov		bl,p3[eax]
ifnb	<n>
; Xor with key
		xor		ebx,[edx+n*4]
endif
		
		mov		edi,ebx
endm

H	proc	uses	edi ebx, x:dword,l:dword
;NOTE: This function must NOT change ecx
		
		mov		edi,x
		bswap		edi
		mov		edx,l
		
		H_ROUND		q1,q0,q0,q1,3
;		bswap		edi
;		H_ROUND		q1,q1,q0,q0,2
;		bswap		edi
;		H_ROUND		q0,q1,q0,q1,1
;		bswap		edi
;		H_ROUND		q0,q0,q1,q1,0
;		bswap		edi
;		H_ROUND		q1,q0,q1,q0

		bswap		edi
		H_ROUND		q0,q0,q1,q1,2
		bswap		edi
		H_ROUND		q1,q0,q1,q0,1
		bswap		edi
		H_ROUND		q1,q1,q0,q0,0
		bswap		edi
		H_ROUND		q0,q1,q0,q1
		
		mov		eax,edi
		ret
		
H	endp

TwofishSetKey	PROC uses ebx edi esi, pCTX:DWORD,pUserKey:DWORD
		
LOCAL	KeyOdd	[4]:DWORD
LOCAL	KeyEven	[4]:DWORD
LOCAL	KeyS	[4]:DWORD
		
LOCAL	s0	[256]:BYTE
LOCAL	s1	[256]:BYTE
LOCAL	s2	[256]:BYTE
LOCAL	s3	[256]:BYTE
		
LOCAL	KeyPtr:DWORD
		
;Split key into KeyEven and KeyOdd and compute KeyS
		mov		esi,pUserKey

I=0
	rept	4
		mov		eax,dword ptr [esi+I*4]
		mov		ebx,dword ptr [esi+(I+1)*4]
		mov		KeyEven[I*2],eax
		mov		KeyOdd[I*2],ebx
		
		push		ebx
		push		eax
		call		RSEnc
		mov		KeyS[4*3-I*2],eax
I=I+2
	endm
;Compute 8-8 SBoxes
		xor		ecx,ecx
		xor		ebx,ebx
		lea		esi,KeyS
@@:
		push		esi
		push		ebx
		call		H
		
		mov		s0[ecx],al
		mov		s1[ecx],ah
		shr		eax,16
		mov		s2[ecx],al
		mov		s3[ecx],ah
		
		add		ebx,SK_STEP
		inc		cl
		jnz		@B
		
; Compute key schedule from KeyEven and KeyOdd
		xor		ebx,ebx
		lea		esi,KeyEven
		lea		edi,KeyOdd
		
		mov		eax,pCTX
		assume	eax:PTR TWOFISH_CTX
		lea		eax,[eax].KeySched
		assume	eax:nothing
		mov		KeyPtr,eax
@@:
		push		esi
		push		ebx
		add		ebx,SK_STEP
		call		H
		
		push		eax
		call		MDSEnc
		
		push		eax
		
		push		edi
		push		ebx
		add		ebx,SK_STEP
		call		H
		
		push		eax
		call		MDSEnc
		
		pop		ecx		; ecx - a,eax - b
		
		rol		eax,8
		
		mov		edx,KeyPtr
		
		add		ecx,eax
		mov		dword ptr [edx],ecx
		add		edx,4
		
		add		eax,ecx
		rol		eax,9
		mov		dword ptr [edx],eax
		add		edx,4
		
		mov		KeyPtr,edx
		
;		add		ebx,SK_STEP
		cmp		ebx,SK_END
		jb		@B
		
; Generate expanded S-boxes
		mov		edi,pCTX
		assume	edi:PTR TWOFISH_CTX
		
		xor		ebx,ebx		;ebx - i
@@:
		mov		al,s0[ebx]
		push		0EFEF5B01h
		push		eax
		call		MDSMul
		lea		esi,[edi].SBox0
		mov		dword ptr [esi+4*ebx],eax
		
		mov		al,s1[ebx]
		push		0015BEFEFh
		push		eax
		call		MDSMul
		lea		esi,[edi].SBox1
		mov		dword ptr [esi+4*ebx],eax
		
		mov		al,s2[ebx]
		push		0EF01EF5Bh
		push		eax
		call		MDSMul
		lea		esi,[edi].SBox2
		mov		dword ptr [esi+4*ebx],eax
		
		mov		al,s3[ebx]
		push		05BEF015Bh
		push		eax
		call		MDSMul
		lea		esi,[edi].SBox3
		mov		dword ptr [esi+4*ebx],eax
		
		inc		bl
		jnz		@B
		assume	edi:nothing
		
		emms
		ret
		
TwofishSetKey	ENDP

S_BOX		MACRO	reg
		
		movzx		eax,bl
		shr		ebx,8
		mov		reg ,dword ptr [esi+4*eax+0*256*4]
		movzx		eax,bl
		shr		ebx,8
		xor		reg ,dword ptr [esi+4*eax+1*256*4]
		movzx		eax,bl
		shr		ebx,8
		xor		reg ,dword ptr [esi+4*eax+2*256*4]
		mov		eax,ebx
		xor		reg ,dword ptr [esi+4*eax+3*256*4]
		
endm

ENC_CYCLE	macro	nCycle
		
; Encipher cycle
; Get key data
		movd		mm4,dword ptr [edx+8*4]
		movd		mm5,dword ptr [edx+9*4]
		movd		mm6,dword ptr [edx+10*4]
		movd		mm7,dword ptr [edx+11*4]
		
; Perform S-Box
		movd		ebx,mm0
		
		S_BOX		ecx
; ecx - x0
		
		movd		ebx,mm1
		rol		ebx,8
		
		S_BOX		edx
; edx - x1
		
		add		ecx,edx
		add		edx,ecx
		
		movd		eax,mm4
		movd		ebx,mm2
		add		ecx,eax
		xor		ecx,ebx
		ror		ecx,1
		
		movd		eax,mm5
		movd		ebx,mm3
		rol		ebx,1
		add		edx,eax
		xor		edx,ebx
		
		movd		mm2,ecx
		movd		mm3,edx
		
; Perform S-Box
		movd		ebx,mm2
		
		S_BOX		ecx
; ecx - x0
		
		movd		ebx,mm3
		rol		ebx,8
		
		S_BOX		edx
; edx - x1
		
		add		ecx,edx
		add		edx,ecx
		
		movd		eax,mm6
		movd		ebx,mm0
		add		ecx,eax
		xor		ecx,ebx
		ror		ecx,1
		
		movd		eax,mm7
		movd		ebx,mm1
		rol		ebx,1
		add		edx,eax
		xor		edx,ebx
		
if nCycle lt 7	; if not last round
		
		movd		mm0,ecx
		movd		mm1,edx
		
		lea		edx,[edi].KeySched+(nCycle+1)*4*4
		
endif
		
endm

TwofishEncBlock1	PROC uses ebx edi esi, pCTX:DWORD,PlainText:DWORD,CipherText:DWORD
		
		mov		ecx,PlainText
		
		mov		edi,pCTX
		assume	edi:PTR TWOFISH_CTX
		
		lea		esi,[edi].SBox0
		lea		edx,[edi].KeySched
		
; Get plaintext and perform input whitening
		mov		eax,dword ptr [ecx+0*4]
		mov		ebx,dword ptr [ecx+1*4]
		xor		eax,dword ptr [edx+0*4]
		xor		ebx,dword ptr [edx+1*4]
		movd		mm0,eax
		movd		mm1,ebx
		
		mov		eax,dword ptr [ecx+2*4]
		mov		ebx,dword ptr [ecx+3*4]
		xor		eax,dword ptr [edx+2*4]
		xor		ebx,dword ptr [edx+3*4]
		movd		mm2,eax
		movd		mm3,ebx
		
		ENC_CYCLE	0
		ENC_CYCLE	1
		ENC_CYCLE	2
		ENC_CYCLE	3
		ENC_CYCLE	4
		ENC_CYCLE	5
		ENC_CYCLE	6
		ENC_CYCLE	7
		
; Perform output whitening
		lea		esi,[edi].KeySched
		
		movd		eax,mm2
		movd		ebx,mm3
		
		xor		eax, dword ptr [esi+4*4]
		xor		ebx, dword ptr [esi+5*4]
		xor		ecx, dword ptr [esi+6*4]
		xor		edx, dword ptr [esi+7*4]
		
		mov		esi,CipherText
		mov		dword ptr [esi+0*4],eax
		mov		dword ptr [esi+1*4],ebx
		mov		dword ptr [esi+2*4],ecx
		mov		dword ptr [esi+3*4],edx
		
		assume		edi:nothing
		
		emms
		ret
		
TwofishEncBlock1	ENDP

TwofishEncBlock2	PROC uses ebx edi esi, pCTX:DWORD,Buff:DWORD
		
		mov		ecx,Buff
		
		mov		edi,pCTX
		assume	edi:PTR TWOFISH_CTX
		
		lea		esi,[edi].SBox0
		lea		edx,[edi].KeySched
		
; Get plaintext and perform input whitening
		mov		eax,dword ptr [ecx+0*4]
		mov		ebx,dword ptr [ecx+1*4]
		xor		eax,dword ptr [edx+0*4]
		xor		ebx,dword ptr [edx+1*4]
		movd		mm0,eax
		movd		mm1,ebx
		
		mov		eax,dword ptr [ecx+2*4]
		mov		ebx,dword ptr [ecx+3*4]
		xor		eax,dword ptr [edx+2*4]
		xor		ebx,dword ptr [edx+3*4]
		movd		mm2,eax
		movd		mm3,ebx
		
		ENC_CYCLE	0
		ENC_CYCLE	1
		ENC_CYCLE	2
		ENC_CYCLE	3
		ENC_CYCLE	4
		ENC_CYCLE	5
		ENC_CYCLE	6
		ENC_CYCLE	7
		
; Perform output whitening
		lea		esi,[edi].KeySched
		
		movd		eax,mm2
		movd		ebx,mm3
		
		xor		eax, dword ptr [esi+4*4]
		xor		ebx, dword ptr [esi+5*4]
		xor		ecx, dword ptr [esi+6*4]
		xor		edx, dword ptr [esi+7*4]
		
		mov		esi,Buff
		mov		dword ptr [esi+0*4],eax
		mov		dword ptr [esi+1*4],ebx
		mov		dword ptr [esi+2*4],ecx
		mov		dword ptr [esi+3*4],edx
		
		assume		edi:nothing
		
		emms
		ret
		
TwofishEncBlock2	ENDP

TwofishXorEnc1		PROC uses ebx edi esi, pCTX:DWORD,XorData:DWORD,PlainText:DWORD,CipherText:DWORD
		
		mov		ecx,PlainText
		
		mov		edi,pCTX
		assume	edi:PTR TWOFISH_CTX
		
		lea		edx,[edi].KeySched
		
; Get plaintext and perform input whitening
		mov		esi,XorData
		
		mov		eax,dword ptr [ecx+0*4]
		mov		ebx,dword ptr [ecx+1*4]
		xor		eax,dword ptr [esi+0*4]
		xor		ebx,dword ptr [esi+1*4]
		xor		eax,dword ptr [edx+0*4]
		xor		ebx,dword ptr [edx+1*4]
		movd		mm0,eax
		movd		mm1,ebx
		
		mov		eax,dword ptr [ecx+2*4]
		mov		ebx,dword ptr [ecx+3*4]
		xor		eax,dword ptr [esi+2*4]
		xor		ebx,dword ptr [esi+3*4]
		xor		eax,dword ptr [edx+2*4]
		xor		ebx,dword ptr [edx+3*4]
		movd		mm2,eax
		movd		mm3,ebx
		
		lea		esi,[edi].SBox0
		
		ENC_CYCLE	0
		ENC_CYCLE	1
		ENC_CYCLE	2
		ENC_CYCLE	3
		ENC_CYCLE	4
		ENC_CYCLE	5
		ENC_CYCLE	6
		ENC_CYCLE	7
		
; Perform output whitening
		lea		esi,[edi].KeySched
		
		movd		eax,mm2
		movd		ebx,mm3
		
		xor		eax, dword ptr [esi+4*4]
		xor		ebx, dword ptr [esi+5*4]
		xor		ecx, dword ptr [esi+6*4]
		xor		edx, dword ptr [esi+7*4]
		
		mov		esi,CipherText
		mov		dword ptr [esi+0*4],eax
		mov		dword ptr [esi+1*4],ebx
		mov		dword ptr [esi+2*4],ecx
		mov		dword ptr [esi+3*4],edx
		
		assume		edi:nothing
		
		emms
		ret
		
TwofishXorEnc1		ENDP

TwofishXorEnc2		PROC uses ebx edi esi, pCTX:DWORD,XorData:DWORD,Buff:DWORD
		
		mov		ecx,Buff
		
		mov		edi,pCTX
		assume	edi:PTR TWOFISH_CTX
		
		lea		edx,[edi].KeySched
		
; Get plaintext and perform input whitening
		mov		esi,XorData
		
		mov		eax,dword ptr [ecx+0*4]
		mov		ebx,dword ptr [ecx+1*4]
		xor		eax,dword ptr [esi+0*4]
		xor		ebx,dword ptr [esi+1*4]
		xor		eax,dword ptr [edx+0*4]
		xor		ebx,dword ptr [edx+1*4]
		movd		mm0,eax
		movd		mm1,ebx
		
		mov		eax,dword ptr [ecx+2*4]
		mov		ebx,dword ptr [ecx+3*4]
		xor		eax,dword ptr [esi+2*4]
		xor		ebx,dword ptr [esi+3*4]
		xor		eax,dword ptr [edx+2*4]
		xor		ebx,dword ptr [edx+3*4]
		movd		mm2,eax
		movd		mm3,ebx
		
		lea		esi,[edi].SBox0
		
		ENC_CYCLE	0
		ENC_CYCLE	1
		ENC_CYCLE	2
		ENC_CYCLE	3
		ENC_CYCLE	4
		ENC_CYCLE	5
		ENC_CYCLE	6
		ENC_CYCLE	7
		
; Perform output whitening
		lea		esi,[edi].KeySched
		
		movd		eax,mm2
		movd		ebx,mm3
		
		xor		eax, dword ptr [esi+4*4]
		xor		ebx, dword ptr [esi+5*4]
		xor		ecx, dword ptr [esi+6*4]
		xor		edx, dword ptr [esi+7*4]
		
		mov		esi,Buff
		mov		dword ptr [esi+0*4],eax
		mov		dword ptr [esi+1*4],ebx
		mov		dword ptr [esi+2*4],ecx
		mov		dword ptr [esi+3*4],edx
		
		assume		edi:nothing
		
		emms
		ret
		
TwofishXorEnc2		ENDP

DEC_CYCLE	macro	nCycle
		
		lea		edx,[edi].KeySched+(nCycle+1)*4*4+4*4
; Encipher cycle
; Get key data
		movd		mm4,dword ptr [edx+0*4]
		movd		mm5,dword ptr [edx+1*4]
		movd		mm6,dword ptr [edx+2*4]
		movd		mm7,dword ptr [edx+3*4]
		
; Perform S-Box
		movd		ebx,mm0
		
		S_BOX		ecx
; ecx - x0
		
		movd		ebx,mm1
		rol		ebx,8
		
		S_BOX		edx
; edx - x1
		add		ecx,edx
		add		edx,ecx
		
		movd		eax,mm6
		movd		ebx,mm2
		rol		ebx,1
		add		ecx,eax
		xor		ecx,ebx
		
		movd		eax,mm7
		movd		ebx,mm3
		add		edx,eax
		xor		edx,ebx
		ror		edx,1
		
		movd		mm2,ecx
		movd		mm3,edx
		
; Perform S-Box
		movd		ebx,mm2
		
		S_BOX		ecx
; ecx - x0
		
		movd		ebx,mm3
		rol		ebx,8
		
		S_BOX		edx
; edx - x1
		
		add		ecx,edx
		add		edx,ecx
		
		movd		eax,mm4
		movd		ebx,mm0
		rol		ebx,1
		add		ecx,eax
		xor		ecx,ebx
		
		movd		eax,mm5
		movd		ebx,mm1
		add		edx,eax
		xor		edx,ebx
		ror		edx,1
		
		movd		mm0,ecx
		movd		mm1,edx
		
endm

TwofishDecBlock1	PROC	uses ebx edi esi, pCTX:DWORD,CipherText:DWORD,PlainText:DWORD
		
		mov		ecx,CipherText
		
		mov		edi,pCTX
		assume	edi:PTR TWOFISH_CTX
		
		lea		esi,[edi].SBox0
		lea		edx,[edi].KeySched
		
; Get plaintext and perform input whitening
		mov		eax,dword ptr [ecx+0*4]
		mov		ebx,dword ptr [ecx+1*4]
		xor		eax,dword ptr [edx+4*4]
		xor		ebx,dword ptr [edx+5*4]
		movd		mm0,eax
		movd		mm1,ebx
		
		mov		eax,dword ptr [ecx+2*4]
		mov		ebx,dword ptr [ecx+3*4]
		xor		eax,dword ptr [edx+6*4]
		xor		ebx,dword ptr [edx+7*4]
		movd		mm2,eax
		movd		mm3,ebx
		
		DEC_CYCLE	7
		DEC_CYCLE	6
		DEC_CYCLE	5
		DEC_CYCLE	4
		DEC_CYCLE	3
		DEC_CYCLE	2
		DEC_CYCLE	1
		DEC_CYCLE	0
		
; Perform output whitening
		lea		esi,[edi].KeySched
		
		movd		eax,mm2
		movd		ebx,mm3
		
		xor		eax, dword ptr [esi+0*4]
		xor		ebx, dword ptr [esi+1*4]
		xor		ecx, dword ptr [esi+2*4]
		xor		edx, dword ptr [esi+3*4]
		
		mov		esi,PlainText
		mov		dword ptr [esi+0*4],eax
		mov		dword ptr [esi+1*4],ebx
		mov		dword ptr [esi+2*4],ecx
		mov		dword ptr [esi+3*4],edx
		
		assume		edi:nothing
		
		emms
		ret
		
TwofishDecBlock1	ENDP

TwofishDecBlock2	PROC	uses ebx edi esi, pCTX:DWORD,Buff:DWORD
		
		mov		ecx,Buff
		
		mov		edi,pCTX
		assume	edi:PTR TWOFISH_CTX
		
		lea		esi,[edi].SBox0
		lea		edx,[edi].KeySched
		
; Get plaintext and perform input whitening
		mov		eax,dword ptr [ecx+0*4]
		mov		ebx,dword ptr [ecx+1*4]
		xor		eax,dword ptr [edx+4*4]
		xor		ebx,dword ptr [edx+5*4]
		movd		mm0,eax
		movd		mm1,ebx
		
		mov		eax,dword ptr [ecx+2*4]
		mov		ebx,dword ptr [ecx+3*4]
		xor		eax,dword ptr [edx+6*4]
		xor		ebx,dword ptr [edx+7*4]
		movd		mm2,eax
		movd		mm3,ebx
		
		DEC_CYCLE	7
		DEC_CYCLE	6
		DEC_CYCLE	5
		DEC_CYCLE	4
		DEC_CYCLE	3
		DEC_CYCLE	2
		DEC_CYCLE	1
		DEC_CYCLE	0
		
; Perform output whitening
		lea		esi,[edi].KeySched
		
		movd		eax,mm2
		movd		ebx,mm3
		
		xor		eax, dword ptr [esi+0*4]
		xor		ebx, dword ptr [esi+1*4]
		xor		ecx, dword ptr [esi+2*4]
		xor		edx, dword ptr [esi+3*4]
		
		mov		esi,Buff
		mov		dword ptr [esi+0*4],eax
		mov		dword ptr [esi+1*4],ebx
		mov		dword ptr [esi+2*4],ecx
		mov		dword ptr [esi+3*4],edx
		
		assume		edi:nothing
		
		emms
		ret
		
TwofishDecBlock2	ENDP

TwofishDecXor1		PROC uses ebx edi esi, pCTX:DWORD,XorData:DWORD,CipherText:DWORD,PlainText:DWORD
		
		mov		ecx,CipherText
		
		mov		edi,pCTX
		assume	edi:PTR TWOFISH_CTX
		
		lea		esi,[edi].SBox0
		lea		edx,[edi].KeySched
		
; Get plaintext and perform input whitening
		mov		eax,dword ptr [ecx+0*4]
		mov		ebx,dword ptr [ecx+1*4]
		xor		eax,dword ptr [edx+4*4]
		xor		ebx,dword ptr [edx+5*4]
		movd		mm0,eax
		movd		mm1,ebx
		
		mov		eax,dword ptr [ecx+2*4]
		mov		ebx,dword ptr [ecx+3*4]
		xor		eax,dword ptr [edx+6*4]
		xor		ebx,dword ptr [edx+7*4]
		movd		mm2,eax
		movd		mm3,ebx
		
		DEC_CYCLE	7
		DEC_CYCLE	6
		DEC_CYCLE	5
		DEC_CYCLE	4
		DEC_CYCLE	3
		DEC_CYCLE	2
		DEC_CYCLE	1
		DEC_CYCLE	0
		
; Perform output whitening
		lea		esi,[edi].KeySched
		
		movd		eax,mm2
		movd		ebx,mm3
		
		xor		eax, dword ptr [esi+0*4]
		xor		ebx, dword ptr [esi+1*4]
		xor		ecx, dword ptr [esi+2*4]
		xor		edx, dword ptr [esi+3*4]
		
		mov		esi,PlainText
		assume		edi:nothing
		mov		edi,XorData
		
		xor		eax,dword ptr [edi+0*4]
		xor		ebx,dword ptr [edi+1*4]
		xor		ecx,dword ptr [edi+2*4]
		xor		edx,dword ptr [edi+3*4]
		
		mov		dword ptr [esi+0*4],eax
		mov		dword ptr [esi+1*4],ebx
		mov		dword ptr [esi+2*4],ecx
		mov		dword ptr [esi+3*4],edx
		
		emms
		ret
		
TwofishDecXor1		ENDP

TwofishDecXor2		PROC uses ebx edi esi, pCTX:DWORD,XorData:DWORD,Buff:DWORD
		
		mov		ecx,Buff
		
		mov		edi,pCTX
		assume	edi:PTR TWOFISH_CTX
		
		lea		esi,[edi].SBox0
		lea		edx,[edi].KeySched
		
; Get plaintext and perform input whitening
		mov		eax,dword ptr [ecx+0*4]
		mov		ebx,dword ptr [ecx+1*4]
		xor		eax,dword ptr [edx+4*4]
		xor		ebx,dword ptr [edx+5*4]
		movd		mm0,eax
		movd		mm1,ebx
		
		mov		eax,dword ptr [ecx+2*4]
		mov		ebx,dword ptr [ecx+3*4]
		xor		eax,dword ptr [edx+6*4]
		xor		ebx,dword ptr [edx+7*4]
		movd		mm2,eax
		movd		mm3,ebx
		
		DEC_CYCLE	7
		DEC_CYCLE	6
		DEC_CYCLE	5
		DEC_CYCLE	4
		DEC_CYCLE	3
		DEC_CYCLE	2
		DEC_CYCLE	1
		DEC_CYCLE	0
		
; Perform output whitening
		lea		esi,[edi].KeySched
		
		movd		eax,mm2
		movd		ebx,mm3
		
		xor		eax, dword ptr [esi+0*4]
		xor		ebx, dword ptr [esi+1*4]
		xor		ecx, dword ptr [esi+2*4]
		xor		edx, dword ptr [esi+3*4]
		
		mov		esi,Buff
		assume		edi:nothing
		mov		edi,XorData
		
		xor		eax,dword ptr [edi+0*4]
		xor		ebx,dword ptr [edi+1*4]
		xor		ecx,dword ptr [edi+2*4]
		xor		edx,dword ptr [edi+3*4]
		
		mov		dword ptr [esi+0*4],eax
		mov		dword ptr [esi+1*4],ebx
		mov		dword ptr [esi+2*4],ecx
		mov		dword ptr [esi+3*4],edx
		
		emms
		ret
		
TwofishDecXor2		ENDP

end
