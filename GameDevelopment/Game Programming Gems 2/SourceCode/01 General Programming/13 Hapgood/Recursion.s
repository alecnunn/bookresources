; Copyright (C) Bryon Hapgood, 2001. 
; All rights reserved worldwide.
;
; This software is provided "as is" without express or implied
; warranties. You may freely copy and compile this source into
; applications you distribute provided that the copyright text
; below is included in the resulting source code, for example:
; "Portions Copyright (C) Bryon Hapgood, 2001"
;
.586
.model flat
.code

public SafeEnter,SafeExit

; struct SAFE{
;     void*__reg[8];
;     void*__ret;
; }

; assembly for SafeEnter routine

_SafeEnter:

	pop	edx	; return address
	mov	eax,[esp] ; safe
	;
	;
	;
	mov	[eax].safe.__ret,edx
	mov	[eax].safe.__ebx,ebx
	mov	[eax].safe.__ebp,ebp
	mov	[eax].safe.__esp,esp
	mov	[eax].safe.__esi,esi
	mov	[eax].safe.__edi,edi
	;
	;
	;
	pop	eax ; safe pointer
	pop	edx ; call function
	push	eax ; safe pointer
	mov	ebp,eax
	call	edx
	mov	eax,ebp
	jmp	sex

_SafeExit:

	pop	edx	;	return
	pop		eax	;	regs context
	;
	;
	;
sex:	mov	edi,[eax].safe.__edi
	mov	esi,[eax].safe.__esi
	mov	esp,[eax].safe.__esp
	mov	ebp,[eax].safe.__ebp
	mov	ebx,[eax].safe.__ebx
	mov	edx,[eax].safe.__ret
	mov	eax,[eax].safe.__eax
	;
	;
	;
	jmp	edx

	end

