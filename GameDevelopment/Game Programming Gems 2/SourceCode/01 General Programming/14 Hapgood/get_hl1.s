; Copyright (C) Bryon Hapgood, 2001. 
; All rights reserved worldwide.
;
; This software is provided "as is" without express or implied
; warranties. You may freely copy and compile this source into
; applications you distribute provided that the copyright text
; below is included in the resulting source code, for example:
; "Portions Copyright (C) Bryon Hapgood, 2001"
;
get_hl:
	ld	hl,ptr_var	; Load HL register with the address ptr_var
	ld	a,(hli)		; Load A register with low byte
					; and increment HL
	ld	h,(hl)		; Load L register with high byte of ptr_var
	ld	l,a			; Save low byte into L
	ret				; Return
