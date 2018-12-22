; Copyright (C) Bryon Hapgood, 2001. 
; All rights reserved worldwide.
;
; This software is provided "as is" without express or implied
; warranties. You may freely copy and compile this source into
; applications you distribute provided that the copyright text
; below is included in the resulting source code, for example:
; "Portions Copyright (C) Bryon Hapgood, 2001"
;
BITMAP struct
pixels	dd	?
w	dd	?
h	dd	?
depth	dd	?
pal	dd	?
pxf	dd	?
stride	dd	?
_size	dd	?
BITMAP ends

PF_BGR_3x8	=	00h
PF_BGR_565	=	04h
PF_BGR_555	=	08h
PF_BGR_4x8	=	0Ch
PF_BGR_1x8	=	10h

execute_buffer db 128 dup(?)

?draw@BITMAP@@QAEXHHAAU1@HHHH@Z:

	push	ebp
	lea	ebp,[esp+8]			;	get arguments address
	push	ebx
	push	edi
	push	esi

	mov	edi,ecx			;	dst bitmap
	mov	esi,[ebp+8]			;	src bitmap
	mov	eax,[esi].bitmap.pxf


	mov	edx,[edi].bitmap.pxf
	cmp	eax,edx
	jne	dislike
	;
		;	like copy
		;
		mov	ecx,[esi].bitmap._size
		cmp	ecx,[edi].bitmap._size
		je	k3
			mov	ecx,[edi].image.stride
			mov	edx,[esi].image.stride
			cmp	edx,ecx
			jne	@f
				;
				;	same w different h
				;
				mov	edx,[edi].image.h
				mov	eax,[esi].image.h
				cmp	eax,edx
				jl	k2
					mov	eax,edx
k2:	mul	ecx
				mov	ecx,eax
k3:	mov	esi,[esi].image.lfb
			mov	edi,[edi].image.lfb
			shr	ecx,2
			rep	movsd
			jmp	ou
@@:	;
		;	find smallest h --> ebx
		;
		mov	eax,[edi].image.h
		mov	ebx,[esi].image.h
		cmp	ebx,eax
		jl	@f
			mov	ebx,eax
@@:	;
		;	calc strides
		;
		add	ebp,12
		mov	eax,[ebp].rectangle.w
		mul	[esi].image.depth; edx corrupts
		mov	edx,[esi].image.stride
		sub	ecx,eax
		sub	edx,eax
		;
		;	calc offsets with intentional reg swap
		;
		push	eax
		push	ecx
		push	edx
		call	calc_esdi
		pop	edx
		pop	eax
		pop	ecx
		;
		;	edx=dest pad
		;
		shr	ecx,2
		mov	ebp,ecx
@@:	rep	movsd
		lea	edi,[edi+eax]
		lea	esi,[esi+edx]
		mov	ecx,ebp
		dec	ebx
		jne	@b
ou:		pop	esi
		pop	edi
		pop	ebx
		pop	ebp
		ret	1ch


dislike:	lea	eax,execute_buffer
		add	ebp,12
		push	ou
		push	eax
		push	edi
		push	esi
		push	ebp
		mov	ebx,edi	;	destination image
		mov	edi,eax
		;
		;	write "mov ebx,h"
		;
		mov	al,0BDh
		stosb
		mov	eax,[ebp].rectangle.h
		stosd
		;
		;	write "mov ecx,w"
		;
		mov	al,0B9h
		stosb
		mov	eax,[ebp].rectangle.w
		stosd
		;
		;	get read
		;
		mov	edx,22
		mov	ebp,esi; source
		mov	eax,[ebp].image.pf
		mov	esi,rtbl_conv[eax]
		lodsd
		mov	ecx,eax
		add	edx,ecx
		rep	movsb
		;
		;	put write
		;
		mov	eax,[ebx].image.pf
		mov	esi,wtbl_conv[eax]
		lodsd
		mov	ecx,eax
		add	edx,ecx
		rep	movsb
		;
		;	write tail
		;
		mov	ecx,[esp]
		push	edx
		sub	dl,19
		neg	dl
		shl	edx,16
		or	edx,08D007549h
		mov	eax,edx
		stosd	;	start of exec_tail
		mov	eax,[ecx].rectangle.w; args
		push	eax
		mov	ecx,[ebp].image.stride;	source
		mul	[ebp].image.depth
		sub	ecx,eax
		jz	@f
			mov	al,0B6h
			stosb
			mov	eax,ecx
			stosd
			jmp	pq
@@:	;
		;	modify outer branch
		;
		dec	edi
		mov	eax,[esp+4]
		sub	eax,6
		mov	[esp+4],eax
pq:	pop	eax
		mul	[ebx].image.depth
		mov	ecx,[ebx].image.stride;	dest
		sub	ecx,eax
		jz	@f
			mov	ax,0BF8Dh
			stosw
			mov	eax,ecx
			stosd
			pop	eax
			jmp	pr
@@:	pop	eax
		sub	eax,6
pr:	inc	al
		neg	al
		shl	eax,16
		or	eax,0C300754Dh
			stosd
			pop	ebp
			pop	esi
			pop	edi
