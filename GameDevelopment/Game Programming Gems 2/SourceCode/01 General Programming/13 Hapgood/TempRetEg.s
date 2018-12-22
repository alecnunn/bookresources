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
.data
buffer dd ?
file_handle dd ?
filesize dd ?
.code

_TempRetEg:

	call	fn0
	call	fn1
	;
	;   before
	;
	pop	edx
	call	edx
	;
	;   after
	;
	call	fn2
	call	fn3
	ret

A:	call	_TempRetEg
	ret

end
