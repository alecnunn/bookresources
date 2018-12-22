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
	ld	hl,ptr_var
	ld	a,(hli)
	ld	l,(hl)
	ld	h,a
	ld	a,(hl)
	push	af 	; Save A register
	;
	; do something with A
	;
	pop	af		; Restore A register
   	ret
