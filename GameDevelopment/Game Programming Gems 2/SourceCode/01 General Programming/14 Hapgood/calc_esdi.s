; Copyright (C) Bryon Hapgood, 2001. 
; All rights reserved worldwide.
;
; This software is provided "as is" without express or implied
; warranties. You may freely copy and compile this source into
; applications you distribute provided that the copyright text
; below is included in the resulting source code, for example:
; "Portions Copyright (C) Bryon Hapgood, 2001"
;
calc_esdi: 	;
			;	Destination
			;
			mov	eax,[ebp-12].point.x	;	get dx
			mul	[edi].image.depth		;	multiply by d
			mov	ecx,eax					;	store result
			mov	eax,[ebp-12].point.y	;	get dy
			mul	[edi].image.stride		;	multiple by stride
			mov	edi,[edi].image.lfb		;	get target pixels
			add	edi,ecx					;	add x
			add	edi,eax					;	add y
			;
			;	Source
			;
			mov	eax,[ebp].rectangle.x	;	get sx
			mul	[esi].image.depth		;	multiply by d
			mov	ecx,eax					;	store result
			mov	eax,[ebp].rectangle.y	;	get sy
			mul	[esi].image.stride		;	multiple by stride
			mov	edx,[esi].image.pal		;	palette info
			mov	esi,[esi].image.lfb		;	get target pixels
			add	esi,ecx					;	add x
			add	esi,eax					;	add y
			ret
