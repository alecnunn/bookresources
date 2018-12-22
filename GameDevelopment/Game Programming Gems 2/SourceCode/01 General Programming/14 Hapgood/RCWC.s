; Copyright (C) Bryon Hapgood, 2001. 
; All rights reserved worldwide.
;
; This software is provided "as is" without express or implied
; warranties. You may freely copy and compile this source into
; applications you distribute provided that the copyright text
; below is included in the resulting source code, for example:
; "Portions Copyright (C) Bryon Hapgood, 2001"
;
RC_BGR_1x8		dd	18					;	(size)
				db	033h,0C0h			;	xor	eax,eax
				db	0ACh				;	lodsb
				db	08Bh,0D8h			;	mov	ebx,eax
				db	003h,0C0h			;	add	eax,eax
				db	003h,0C3h			;	add	eax,ebx
				db	003h,0C2h			;	add	eax,edx
				db	08Bh,000h			;	mov	eax,[eax]
				db	025h,0FFh,0FFh,0FFh,000h	;	and	eax,-1

RC_BGR_3x8		dd	7					;	(size)
				db	0ADh				;	lodsd
				db	025h,0FFh,0FFh,0FFh,000h	;	and	eax,-1
				db	04Eh				;	dec	esi

RC_BGR_4x8		dd	1					;	(size)
				db	0ADh				;	lodsd

RC_BGR_565:		dd	1					;	(size)
				lodsw

RC_BGR_555:		dd	1					;	(size)
				lodsw	

WC_BGR_3x8		dd	6					;	(size)
				db	0AAh				;	stosb
				db	0C1h,0E8h,008h		;	shr	eax,8
				db	066h,0ABh			;	stosw

WC_BGR_555		dd	28					;	(size)
				db	033h,0DBh			;	xor	ebx,ebx
				db	0C0h,0E8h,003h		;	shr	al,3
				db	0C0h,0ECh,003h		;	shr	ah,3
				db	08Ah,0DCh			;	mov	bl,ah
				db	066h,0C1h,0E3h,005h		;	shl	bx,5
				db	00Ah,0D8h			;	or	bl,al
				db	0C1h,0E8h,013h		;	shr	eax,13h
				db	066h,0C1h,0E0h,00Ah		;	shl	ax,0Ah
				db	066h,00Bh,0C3h		;	or	ax,bx
				db	066h,0ABh			;	stosw

WC_BGR_565		dd	28					;	(size)
				db	033h,0DBh			;	xor	ebx,ebx
				db	0C0h,0E8h,003h		;	shr	al,3
				db	0C0h,0ECh,002h		;	shr	ah,2
				db	08Ah,0DCh			;	mov	bl,ah
				db	066h,0C1h,0E3h,005h		;	shl	bx,5
				db	00Ah,0D8h			;	or	bl,al
				db	0C1h,0E8h,013h		;	shr	eax,13h
				db	066h,0C1h,0E0h,00Bh		;	shl	ax,0Bh
				db	066h,00Bh,0C3h		;	or	ax,bx
				db	066h,0ABh			;	stosw

WC_BGR_4x8		dd	1					;	(size)
				db	0ABh				;	stosd

rtbl_conv		dd	RC_BGR_3x8
				dd	RC_BGR_565
				dd	RC_BGR_555
				dd	RC_BGR_4x8
				dd	RC_BGR_1x8

wtbl_conv		dd	WC_BGR_3x8
				dd	WC_BGR_565
				dd	WC_BGR_555
				dd	WC_BGR_4x8
				dd	?

