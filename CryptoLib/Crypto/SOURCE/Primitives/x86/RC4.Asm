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

.model flat,stdcall
option casemap:none

include RC4.inc

.code

align 16
rc4KeySetup	proc	pCtx:DWORD,pUserKey:DWORD,KeyLength:DWORD
		
		pushad
		
		mov		eax,pCtx
		lea		edi,(RC4_KEY_CTX PTR [eax]).State
		mov		esi,pUserKey
		
		xor		ecx,ecx
align 16
@@:
		mov		byte ptr [edi+ecx],cl
		
		inc		ecx
		cmp		ecx,256
		jb		@B
		
		mov		ecx,pCtx
		xor		eax,eax
		mov		(RC4_KEY_CTX PTR [ecx]).x,eax
		mov		(RC4_KEY_CTX PTR [ecx]).y,eax
		
		xor		ecx,ecx
		xor		ebx,ebx
		xor		edx,edx
align 16
swap_loop:
		add		dl,byte ptr [edi+ecx]
		add		dl,byte ptr [esi+ebx]
		
		mov		al,byte ptr [edi+ecx]
		mov		ah,byte ptr [edi+edx]
		mov		byte ptr [edi+edx],al
		mov		byte ptr [edi+ecx],ah
		
		inc		bl
		cmp		ebx,KeyLength
		jb		@F
		
		xor		ebx,ebx
align 16
@@:
		inc		ecx
		cmp		ecx,256
		jb		swap_loop
		
		popad
		
		ret
		
rc4KeySetup	endp

align 16
rc4Gen		proc		pCtx:DWORD,Buff:DWORD,BuffLength:DWORD
		
		pushad
		
		mov		eax,pCtx
		lea		esi,(RC4_KEY_CTX PTR [eax]).State
		
		mov		ebx,(RC4_KEY_CTX PTR [eax]).x
		mov		edx,(RC4_KEY_CTX PTR [eax]).y
		
		mov		edi,Buff
		mov		ecx,BuffLength
		xor		eax,eax
align 16
@@:
		jecxz		@F
		
		inc		bl
		add		dl,byte ptr [esi+ebx]
		
		mov		al,byte ptr [esi+ebx]
		mov		ah,byte ptr [esi+edx]
		mov		byte ptr [esi+ebx],ah
		mov		byte ptr [esi+edx],al
		
		add		al,ah
		xor		ah,ah
		
		mov		al,byte ptr [esi+eax]
		
		mov		byte ptr [edi],al
		inc		edi
		
		dec		ecx
		jmp	short	@B
align 16
@@:
		popad
		ret
		
rc4Gen		endp

align 16
rc4Xor		proc		pCtx:DWORD,Buff:DWORD,BuffLength:DWORD
		
		pushad
		
		mov		eax,pCtx
		lea		esi,(RC4_KEY_CTX PTR [eax]).State
		
		mov		ebx,(RC4_KEY_CTX PTR [eax]).x
		mov		edx,(RC4_KEY_CTX PTR [eax]).y
		
		mov		edi,Buff
		mov		ecx,BuffLength
		xor		eax,eax
align 16
@@:
		jecxz		@F
		
		inc		bl
		add		dl,byte ptr [esi+ebx]
		
		mov		al,byte ptr [esi+ebx]
		mov		ah,byte ptr [esi+edx]
		mov		byte ptr [esi+ebx],ah
		mov		byte ptr [esi+edx],al
		
		add		al,ah
		xor		ah,ah
		
		mov		al,byte ptr [esi+eax]
		
		xor		byte ptr [edi],al
		inc		edi
		
		dec		ecx
		jmp	short	@B
align 16
@@:
		popad
		ret
		
rc4Xor		endp

align 16
rc4Skip		proc		pCtx:DWORD,SkipLength:DWORD
		
		pushad
		
		mov		eax,pCtx
		lea		esi,(RC4_KEY_CTX PTR [eax]).State
		
		mov		ebx,(RC4_KEY_CTX PTR [eax]).x
		mov		edx,(RC4_KEY_CTX PTR [eax]).y
		
		mov		ecx,SkipLength
		xor		eax,eax
align 16
@@:
		jecxz		@F
		
		inc		bl
		add		dl,byte ptr [esi+ebx]
		
		mov		al,byte ptr [esi+ebx]
		mov		ah,byte ptr [esi+edx]
		mov		byte ptr [esi+ebx],ah
		mov		byte ptr [esi+edx],al
		
		add		al,ah
		xor		ah,ah
		
		mov		al,byte ptr [esi+eax]
		
		dec		ecx
		jmp	short	@B
align 16
@@:
		popad
		ret
		
rc4Skip		endp

end 
