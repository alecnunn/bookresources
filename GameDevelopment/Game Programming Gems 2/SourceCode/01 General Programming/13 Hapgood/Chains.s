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

TempRet macro
pop 	edx
call 	edx
TempRet endm

createWindow:
	; open the window
	TempRet
	; close it
	ret
setCooperativeLevel:
	; set to exclusive
	TempRet
	; restore
	ret
changeDisplayMode:
	; set 640x480 16 bpp
	TempRet
	; restore
	ret
createSurfaces:
	; create primary surface
	; get attached back
	TempRet
	; release primary
	ret
_SetupDX7:
	call	createWindow
	call	setCooperativeLevel
	call	changeDisplayMode
	call	createSurfaces
	jmp	_SomeUserRunFunc

end
