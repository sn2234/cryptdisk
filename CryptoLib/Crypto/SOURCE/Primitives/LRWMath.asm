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

.model flat, stdcall
option casemap:none

include LrwMath.inc
.code
GF_MULT_STEP	macro
		shr		ebp, 1
		jnc		@F
		
		xor		[edi+0*4], eax
		xor		[edi+1*4], ebx
		xor		[edi+2*4], ecx
		xor		[edi+3*4], edx
@@:
		shl		eax, 1
		rcl		ebx, 1
		rcl		ecx, 1
		rcl		edx, 1
		
		jnc		@F
		
		xor		eax, 087h
@@:
		endm

GFMult		PROC pDest:DWORD, pC:DWORD, pY:DWORD
		
		push		ebx
		push		ebp
		push		edi
		push		esi
		
	; Zero pDest
		xor		eax, eax
		mov		edi, pDest
		
		mov		[edi+0*4], eax
		mov		[edi+1*4], eax
		mov		[edi+2*4], eax
		mov		[edi+3*4], eax
		
		mov		esi, pC
		mov		eax, [esi+0*4]
		mov		ebx, [esi+1*4]
		mov		ecx, [esi+2*4]
		mov		edx, [esi+3*4]
		
		mov		esi, pY		; !!! using ebp
		mov		ebp, 128/32
@@loop_begin:
		push		ebp
		
		mov		ebp, dword ptr [esi]
		
	rept 32
		GF_MULT_STEP
	endm
		
		pop		ebp
		add		esi, 4
		
		sub		ebp, 1
		jnz		@@loop_begin
		
		pop		esi
		pop		edi
		pop		ebp
		pop		ebx
		
		ret
GFMult		endp

; NOTE: Table must be zeroed before call this function
GFPrepareTable	PROC pTable:DWORD, pConstant:DWORD
		
		push		ebx
		push		ebp
		push		edi
		push		esi
		
		mov		edi, pTable
		mov		esi, pConstant
		
		mov		eax, 1
		xor		ebx, ebx
		xor		ecx, ecx
		xor		edx, edx
		
		mov		ebp, 128
@@loop0_begin:
		movd		mm0, eax
		movd		mm1, ebx
		movd		mm2, ecx
		movd		mm3, edx
		
		push		ebp
		
		mov		ebp, 128/32
@@loop1_begin:
		push		ebp
		
		mov		ebp, dword ptr [esi]
		
	rept 32
		GF_MULT_STEP
	endm
		
		pop		ebp
		add		esi, 4
		
		sub		ebp, 1
		jnz		@@loop1_begin
		
		movd		eax, mm0
		movd		ebx, mm1
		movd		ecx, mm2
		movd		edx, mm3
		
		shl		eax, 1
		rcl		ebx, 1
		rcl		ecx, 1
		rcl		edx, 1
		
		pop		ebp
		
		sub		esi, 16
		add		edi, 16
		
		sub		ebp, 1
		jnz		@@loop0_begin
		
		pop		esi
		pop		edi
		pop		ebp
		pop		ebx
		
		emms
		ret
		
GFPrepareTable	ENDP

GFMultTable	PROC pTable:DWORD, pDest:DWORD, pSrc:DWORD
		
		push		edi
		push		esi
		
		mov		esi, pSrc
		mov		edi, pTable
		
		; Zero destiation buffer
		pxor		mm0, mm0
		pxor		mm1, mm1
		
		xor		ecx,ecx
@@loop_begin:
		mov		edx, dword ptr [esi]
	rept 32
		shr		edx, 1
		jnc		@F
		
		pxor		mm0, qword ptr [edi+ecx+0*8]
		pxor		mm1, qword ptr [edi+ecx+1*8]
@@:
		add		ecx, 16
	endm
		add		esi, 4
		cmp		ecx, 128*16
		jb		@@loop_begin
		
		mov		edi, pDest
		
		movq		qword ptr [edi+0*8], mm0
		movq		qword ptr [edi+1*8], mm1
		
		pop		esi
		pop		edi
		
		ret
		
GFMultTable	ENDP

LRWPrepareTable	PROC pTable:DWORD, pKeyTable:DWORD
LOCAL	tmp[16]:BYTE
		
		push		ebx
		push		edi
		push		esi
		
		lea		edi, tmp
		mov		esi, pTable
		mov		ebx, pKeyTable
		
		xor		eax,eax
		mov		dword ptr [edi+0*4], eax
		mov		dword ptr [edi+1*4], eax
		mov		dword ptr [edi+2*4], eax
		mov		dword ptr [edi+3*4], eax
		
		xor		ecx, ecx
@@loop_begin:
		bts		[edi], ecx
		
		push		ecx
		
		push		edi
		push		esi
		push		ebx
		call		GFMultTable
		
		pop		ecx
		
		add		ecx, 1
		add		esi, 16
		cmp		ecx, 128
		jb		@@loop_begin
		
		pop		esi
		pop		edi
		pop		ebx
		
		emms
		ret
		
LRWPrepareTable	ENDP

LRWMult		PROC pTable:DWORD, pNumber:DWORD, pBuff:DWORD
		
		push		ebx
		push		edi
		
		mov		edi, pNumber
		
		mov		ecx, dword ptr [edi+0*4]
		not		ecx
		bsf		edx, ecx
		jz		@F
		jmp		@@zero_found
@@:
		mov		ecx, dword ptr [edi+1*4]
		not		ecx
		bsf		edx, ecx
		jz		@F
		add		edx, 32*1
		jmp		@@zero_found
@@:
		mov		ecx, dword ptr [edi+2*4]
		not		ecx
		bsf		edx, ecx
		jz		@F
		add		edx, 32*2
		jmp		@@zero_found
@@:
		mov		ecx, dword ptr [edi+3*4]
		not		ecx
		bsf		edx, ecx
		jz		@F
		add		edx, 32*3
		jmp		@@zero_found
@@:
		mov		edx, 127
@@zero_found:
		shl		edx, 4
		add		edx, pTable
		mov		ecx, pBuff
		
		movq		mm0, qword ptr [edx+0*8]
		movq		mm1, qword ptr [edx+1*8]
		
		pxor		mm0, qword ptr [ecx+0*8]
		pxor		mm1, qword ptr [ecx+1*8]
		
		movq		qword ptr [ecx+0*8], mm0
		movq		qword ptr [ecx+1*8], mm1
		
		mov		eax, dword ptr [edi+0*4]
		mov		ebx, dword ptr [edi+1*4]
		mov		ecx, dword ptr [edi+2*4]
		mov		edx, dword ptr [edi+3*4]
		
		add		eax, 1
		adc		ebx, 0
		adc		ecx, 0
		adc		edx, 0
		
		mov		dword ptr [edi+0*4], eax
		mov		dword ptr [edi+1*4], ebx
		mov		dword ptr [edi+2*4], ecx
		mov		dword ptr [edi+3*4], edx
		
		pop		edi
		pop		ebx
		
		emms
		ret
		
LRWMult		ENDP

LRWInitContext	PROC pCtx: PTR LRW_CONTEXT, tweakKey:DWORD
		
		push		ebx
		push		edi
		
		mov		ebx, pCtx
		
		; Zero context
		xor		eax, eax
		mov		ecx, (sizeof LRW_CONTEXT) /4
		mov		edi, ebx
		rep stosd
		
		mov		ecx, tweakKey
	assume	ebx:PTR LRW_CONTEXT
		
		movq		mm0, qword ptr [ecx+0*8]
		movq		mm1, qword ptr [ecx+1*8]
		pxor		mm2, mm2
		
		lea		edx, [ebx].m_key
		
		movq		qword ptr [edx+0*8], mm0	; Copy m_key
		movq		qword ptr [edx+1*8], mm1
		movq		qword ptr [edx+2*8], mm2	; Zero m_keyIndex
		movq		qword ptr [edx+3*8], mm2
		movq		qword ptr [edx+4*8], mm2	; Zero m_currentIndex
		movq		qword ptr [edx+5*8], mm2
		
		push		ecx
		push		ebx
		call		GFPrepareTable
		
		lea		eax, [ebx].m_lrwTable
		push		ebx
		push		eax
		call		LRWPrepareTable
		
	assume	ebx:nothing
		
		pop		edi
		pop		ebx
		emms
		ret
		
LRWInitContext	ENDP

LRWStartSequence	PROC pCtx: PTR LRW_CONTEXT, indexBegin:DWORD
		
		mov		ecx, pCtx
		mov		edx, indexBegin
	assume	ecx:PTR LRW_CONTEXT
		
		movq		mm0, qword ptr [edx+0*8]
		movq		mm1, qword ptr [edx+1*8]
		
		lea		eax, [ecx].m_currentIndex
		
		movq		qword ptr [eax+0*8], mm0
		movq		qword ptr [eax+1*8], mm1
		
		push		edx
		lea		eax, [ecx].m_currentTweak
		push		eax
		push		ecx
		call		GFMultTable
		
	assume	ecx:nothing
		
		emms
		ret
		
LRWStartSequence	ENDP

LRWXorTweak	PROC pCtx: PTR LRW_CONTEXT, pBuff:DWORD
		
		mov		ecx, pCtx
		mov		edx, pBuff
	assume	ecx:PTR LRW_CONTEXT
		
		lea		eax, [ecx].m_currentTweak
		
		movq		mm0, qword ptr [edx+0*8]
		movq		mm1, qword ptr [edx+1*8]
		
		pxor		mm0, qword ptr [eax+0*8]
		pxor		mm1, qword ptr [eax+1*8]
		
		movq		qword ptr [edx+0*8], mm0
		movq		qword ptr [edx+1*8], mm1
		
	assume	ecx:nothing
		emms
		ret
		
LRWXorTweak	ENDP

LRWNextTweak	PROC pCtx: PTR LRW_CONTEXT
		
		mov		ecx, pCtx
	assume	ecx:PTR LRW_CONTEXT
		
		lea		eax, [ecx].m_currentIndex
		lea		edx, [ecx].m_currentTweak
		push		edx
		push		eax
		lea		eax, [ecx].m_lrwTable
		push		eax
		call		LRWMult
		
	assume	ecx:nothing
		
		emms
		ret
LRWNextTweak	endp
end
