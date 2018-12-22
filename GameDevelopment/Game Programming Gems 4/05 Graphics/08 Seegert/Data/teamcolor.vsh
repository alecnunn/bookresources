vs.1.1

dcl_position0	v0
dcl_normal0		v1
dcl_color0		v2
dcl_texcoord0	v3


;*************
;* Constants *
;*************
;  c0 == zero
;  c1 == one
;  c2 == eye position
;  c3 == world transform
;  c7 == projection transform
; c11 == view transform
; c15 == light 1 direction
; c16 == light 1 color
; c17 == light 2 direction
; c18 == light 2 color
; c19 == ambient color
; c20 == major team color
; c21 == minor team color


;************
;* Position *
;************
; transform to world space
m4x4	r0,		v0,		c3

; transform to view space
m4x4	r1,		r0,		c11

; transform to proj space
m4x4	r2,		r1,		c7

; set position
mov		oPos,	r2


;************
;* Lighting *
;************
; transform normal to world space and dp3
m4x4	r0,			v1,			c3
dp3		r1.x,		c15,		r0
max		r1.x,		r1.x,		c0.x
mul		oD0.rgb,	v2.rgb,		r1.x
mov		oD0.a,		c1.x


;*******
;* Fog *
;*******
mov		oFog,	c0.x


;******
;* UV *
;******
; just copy the uv's for stage 0
mov		oT0,	v3

; compute and normalize the vector from the eye to the
; point on the vertex (r2; from above)
add		r1,			c2,			-r2
dp3		r1.w,		r1,			 r1
rsq		r1.w,		r1.w		
mul		r1,			r1,			 r1.w

; average the eye vector with the light normal
; and normalize the result
add		r1,			r1,			c15
dp3		r1.w,		r1,			r1
rsq		r1.w,		r1.w
mul		r1,			r1,			r1.w

; compute our uv pair
dp3		oT1.x,		c15,		r0
dp3		oT1.y,		r1,			r0
