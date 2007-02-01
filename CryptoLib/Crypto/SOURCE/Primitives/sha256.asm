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

include sha256.inc

;--------------------------------------------------------------------------------
Const0=0428A2F98h
Const1=071374491h
Const2=0B5C0FBCFh
Const3=0E9B5DBA5h
Const4=03956C25Bh
Const5=059F111F1h
Const6=0923F82A4h
Const7=0AB1C5ED5h
Const8=0D807AA98h
Const9=012835B01h
Const10=0243185BEh
Const11=0550C7DC3h
Const12=072BE5D74h
Const13=080DEB1FEh
Const14=09BDC06A7h
Const15=0C19BF174h
Const16=0E49B69C1h
Const17=0EFBE4786h
Const18=00FC19DC6h
Const19=0240CA1CCh
Const20=02DE92C6Fh
Const21=04A7484AAh
Const22=05CB0A9DCh
Const23=076F988DAh
Const24=0983E5152h
Const25=0A831C66Dh
Const26=0B00327C8h
Const27=0BF597FC7h
Const28=0C6E00BF3h
Const29=0D5A79147h
Const30=006CA6351h
Const31=014292967h
Const32=027B70A85h
Const33=02E1B2138h
Const34=04D2C6DFCh
Const35=053380D13h
Const36=0650A7354h
Const37=0766A0ABBh
Const38=081C2C92Eh
Const39=092722C85h
Const40=0A2BFE8A1h
Const41=0A81A664Bh
Const42=0C24B8B70h
Const43=0C76C51A3h
Const44=0D192E819h
Const45=0D6990624h
Const46=0F40E3585h
Const47=0106AA070h
Const48=019A4C116h
Const49=01E376C08h
Const50=02748774Ch
Const51=034B0BCB5h
Const52=0391C0CB3h
Const53=04ED8AA4Ah
Const54=05B9CCA4Fh
Const55=0682E6FF3h
Const56=0748F82EEh
Const57=078A5636Fh
Const58=084C87814h
Const59=08CC70208h
Const60=090BEFFFAh
Const61=0A4506CEBh
Const62=0BEF9A3F7h
Const63=0C67178F2h

;--------------------------------------------------------------------------------

.code

align 16
SHA256Init	proc	ctx:dword
		
		mov		edx,ctx
		
		; zero length
		xor		eax,eax
		mov		(SHA256_CTX ptr [edx]).MessageLength,eax
		mov		(SHA256_CTX ptr [edx]).MessageLength+4,eax
		
		; init state
		mov		(SHA256_CTX ptr [edx]).State[0*4],06A09E667h
		mov		(SHA256_CTX ptr [edx]).State[1*4],0BB67AE85h
		mov		(SHA256_CTX ptr [edx]).State[2*4],03C6EF372h
		mov		(SHA256_CTX ptr [edx]).State[3*4],0A54FF53Ah
		mov		(SHA256_CTX ptr [edx]).State[4*4],0510E527Fh
		mov		(SHA256_CTX ptr [edx]).State[5*4],09B05688Ch
		mov		(SHA256_CTX ptr [edx]).State[6*4],01F83D9ABh
		mov		(SHA256_CTX ptr [edx]).State[7*4],05BE0CD19h
		
		ret
		
SHA256Init	endp

align 16
SHA256Update	proc	uses ebx edi esi ,ctx:dword,Input:dword,InputLen:dword

;setup
;	len=InputLen
		mov		ecx,InputLen
		test		ecx, ecx
		jz		@@return
		
		assume		ebx:PTR SHA256_CTX
		mov		ebx,ctx
;	ptr=Input
		mov		esi,Input		;esi - ptr
;calculate number of bytes in buffer
;	buff_len=MessageLength & 3Fh
		mov		eax,[ebx].MessageLength	;low dword only
		and		eax,3Fh
		mov		edx,eax			;edx - buff_len
		test		eax,eax
		jz		@@no_buffer
;copy data from input to buffer
;	bytes_copy=64-buff_len
		neg		eax
		add		eax,64			;eax - bytes_copy
;	if(bytes_copy>len) bytes_copy=len
		cmp		eax,ecx
		jbe		@F
		
		mov		eax,ecx
align 16
@@:
;	copy(buffer,ptr,bytes_copy)
		push		ecx
		mov		ecx,eax
		
		lea		edi,[ebx].Buffer
		add		edi,edx
		rep	movsb
		
		pop		ecx
;	len-=bytes_copy
		sub		ecx,eax
;	buff_len+=bytes_copy
		add		edx,eax
;	ctx->MessageLength+=bytes_copy
		add		[ebx].MessageLength,eax
		adc		[ebx].MessageLength+4,0
;	ptr+=bytes_copy
;if buffer if full ,hash it
;	if(buff_len==64)
		cmp		edx,64
		jb		@F
;		SHA256HashBlock(ctx,buffer)
		lea		eax,[ebx].Buffer
		push		eax
		push		ebx
		call		SHA256HashBlock
align 16
@@no_buffer:
@@:
;while input len >= 64 ,hash data from ptr
;	ctx->MessageLength+=len
		add		[ebx].MessageLength,ecx
		adc		[ebx].MessageLength+4,0
;	while(len>=64)
;	{
align 16
@@:
		cmp		ecx,64
		jb		@F
;		SHA256HashBlock(ctx,ptr);
		push		esi
		push		ebx
		call		SHA256HashBlock
;		ptr+=64
		add		esi,64
;		len-=64
		sub		ecx,64
		jmp	short	@B
;	}
align 16
@@:
;copy remainder bytes to the buffer
;	if(len)
		jecxz		@@return
;	copy(buffer,ptr,len)
		lea		edi,[ebx].Buffer
		rep	movsb
		
@@return:
align 16
		ret
		
		assume	ebx:nothing
SHA256Update	endp

align 16
SHA256Final	proc	uses ebx esi edi ,ctx:dword,Output:dword
		
LOCAL	bitlength[2]:dword
		
;calculate padding length
;	padding_len=64-BufferLength
		mov		ebx,ctx
		assume		ebx:PTR SHA256_CTX
		
		mov		eax,[ebx].MessageLength
		mov		edx,[ebx].MessageLength+4
		shld		edx,eax,3
		shl		eax,3
		bswap		eax
		bswap		edx
		mov		bitlength,edx
		mov		bitlength+4,eax
		
		mov		ecx,[ebx].MessageLength
		and		ecx,3Fh
		cmp		ecx,56
		mov		eax,56
		jb		@F
		
		mov		eax,120
align 16
@@:
		sub		eax,ecx
@@hash_pad:
;hash padding
		push		eax
		call		@F
		
		db		80h
		db		63 dup(0)
align 16
@@:
		push		ctx
		call		SHA256Update
		
;hash length
		lea		eax,bitlength
		push		8
		push		eax
		push		ctx
		call		SHA256Update
		
;		mov		ecx,Output
		
;		movq		mm0,qword ptr [ebx].State[0*8]
;		movq		mm1,qword ptr [ebx].State[1*8]
;		movq		mm2,qword ptr [ebx].State[2*8]
;		movq		mm3,qword ptr [ebx].State[3*8]
		
;		movq		[ecx+0*8],mm0
;		movq		[ecx+1*8],mm1
;		movq		[ecx+2*8],mm2
;		movq		[ecx+3*8],mm3

		mov			edi,Output
		lea			esi,[ebx].State
		mov			ecx,8
align 16
@@:
		lodsd
		bswap		eax
		stosd
		
		loop		@B
				
		; zero state
		mov		edi,ebx
		mov		ecx,(sizeof SHA256_CTX)/4
		xor		eax,eax
		rep	stosd
		
		assume	ebx:nothing
		ret
		
SHA256Final	endp

; T1 - esi
; T2 - edi
;

SHA256Round	macro	a,b,c,d,e,f,g,h,Wt,Kt
		
; T1
		movd		ecx,e
		mov		eax,ecx
		mov		edx,ecx
		ror		ecx,6
		ror		eax,11
		rol		edx,7		;ror		edx,25
		xor		ecx,eax
		xor		ecx,edx		;ecx=S1(e)
		
		movd		esi,f
		movd		eax,e
		and		esi,eax
		movd		edx,g
		not		eax
		and		eax,edx
		xor		esi,eax		;esi=Ch(e,f,g)
		
		add		esi,ecx
		movd		eax,h
		lea		esi,[esi+eax+Kt]
		add		esi,Wt		;esi=T1
; T2
		movd		edi,a
		mov		eax,edi
		mov		edx,edi
		ror		edi,2
		ror		eax,13
		rol		edx,10		;ror		edx,22
		xor		edi,eax
		xor		edi,edx		;edi=S0(a)
		
		movd		eax,a
		movd		ebx,b
		movd		ecx,c
		and		eax,ebx
		and		ebx,ecx
		xor		eax,ebx
		movd		ebx,a
		and		ebx,ecx
		xor		eax,ebx		;eax=Maj(a,b,c)
		
		add		edi,eax		;esi=T2
		
		movd		eax,d
		add		eax,esi
		movd		d,eax
		
		add		esi,edi
		movd		h,esi
ifdef	DEBUG
		nop
		nop
endif
		
endm

Rr	macro	Wt,j
		
		;s1(W[(i-2)*4])
		mov		eax,dword ptr [Wt+(j-2)*4]
		mov		ecx,eax
		mov		edx,eax
		rol		eax,15		;ror		eax,17
		rol		ecx,13		;ror		ecx,19
		shr		edx,10
		xor		eax,ecx
		xor		eax,edx
		
		;s0(W[(i-15)*4])
		mov		ebx,dword ptr [Wt+(j-15)*4]
		mov		ecx,ebx
		mov		edx,ebx
		ror		ebx,7
		rol		ecx,14		;ror		ecx,18
		shr		edx,3
		xor		ebx,ecx
		xor		ebx,edx
		
		mov		ecx,dword ptr [Wt+(j-7)*4]
		mov		edx,dword ptr [Wt+(j-16)*4]
		
		add		ebx,eax
		add		ebx,ecx
		add		ebx,edx
		
		mov		dword ptr [Wt+(j*4)],ebx
endm

align 16
SHA256HashBlock	proc	ctx:dword,Input:dword
LOCAL	W[64]:DWORD
		
i = 0

_a	textequ		<@CatStr(<mm>,%((0-@CatStr(%i))and 7))>
_b	textequ		<@CatStr(<mm>,%((1-@CatStr(%i))and 7))>
_c	textequ		<@CatStr(<mm>,%((2-@CatStr(%i))and 7))>
_d	textequ		<@CatStr(<mm>,%((3-@CatStr(%i))and 7))>
_e	textequ		<@CatStr(<mm>,%((4-@CatStr(%i))and 7))>
_f	textequ		<@CatStr(<mm>,%((5-@CatStr(%i))and 7))>
_g	textequ		<@CatStr(<mm>,%((6-@CatStr(%i))and 7))>
_h	textequ		<@CatStr(<mm>,%((7-@CatStr(%i))and 7))>

		pushad

		mov		eax,ctx
		assume	eax:PTR SHA256_CTX

		movd		mm0,[eax].State[0*4]
		movd		mm1,[eax].State[1*4]
		movd		mm2,[eax].State[2*4]
		movd		mm3,[eax].State[3*4]
		movd		mm4,[eax].State[4*4]
		movd		mm5,[eax].State[5*4]
		movd		mm6,[eax].State[6*4]
		movd		mm7,[eax].State[7*4]
		
		assume	eax:nothing
		
		push		ebp
		
		mov		esi,Input
		lea		edi,W
		mov		ecx,16
align 16
@@:
		lodsd
		bswap		eax
		stosd
		
		loop		@B
		
		lea		ebp,W
		
	i=0
	while	i lt 16
		SHA256Round	_a,_b,_c,_d,_e,_f,_g,_h,[ebp+i*4],@CatStr(Const,%i)
	i=i+1
	endm
		
	while	i lt 64
		Rr		ebp,i
		SHA256Round	_a,_b,_c,_d,_e,_f,_g,_h,[ebp+i*4],@CatStr(Const,%i)
	i=i+1
	endm
		pop		ebp
		
		mov		eax,ctx
		assume	eax:PTR SHA256_CTX
		
		movd		ebx,mm0
		movd		ecx,mm1
		movd		edx,mm2
		movd		edi,mm3
		movd		esi,mm4
		
		add		[eax].State[0*4],ebx
		add		[eax].State[1*4],ecx
		add		[eax].State[2*4],edx
		add		[eax].State[3*4],edi
		add		[eax].State[4*4],esi
		
		movd		ebx,mm5
		movd		ecx,mm6
		movd		edx,mm7
		
		add		[eax].State[5*4],ebx
		add		[eax].State[6*4],ecx
		add		[eax].State[7*4],edx
		
		assume	eax:nothing
		
		emms
		popad
		ret
		
SHA256HashBlock	endp

end
