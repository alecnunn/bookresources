vs.1.0;
;----------------------------------------------------------------------------------------
;Shader produced by prototype version 0.01 of the Game Programming Gems 3 Programmable Shader Compiler
;Graphics and 3D Technologies Group, Intel Labs
;-----------------------------------------------------------------------------------------


;These 3 vectors are used for approximating sin and cos via Taylor Series
;this is our temporary hack for vertex pos mods
mov r0, v0; move the vertex pos for xform

mov	r1, c22;

mul	r11, r1, c22;
mov	r1, r11;

mov	r2, c25;

mul	r11, r2, c21;
mov	r2, r11;

;normalize function
dp3	r11, c20, c20;
rsq	r10, r11;
mul	r11, c20, r10;
mov	r3, r11;

dp3	r11, r3, v3;
mov	r4, r11;

mul	r11, r4, r2;
mov	r2, r11;

max	r11, r2, c0.xxxx;
mov	r2, r11;

sub	r11, c29, v0;
mov	r5, r11;

;normalize function
dp3	r11, r5, r5;
rsq	r10, r11;
mul	r11, r5, r10;
mov	r5, r11;

add	r11, r3, r5;
mov	r6, r11;

;normalize function
dp3	r11, r6, r6;
rsq	r10, r11;
mul	r11, r6, r10;
mov	r6, r11;

dp3	r11, r6, v3;
mov	r7, r11;

max	r11, r7, c0.xxxx;
mov	r7, r11;

; raise to a power
mov	r10.x, c1.x;
mov	r10.y, r7.x;
mov	r10.z, c30.y;
lit	r11.z, r10.xyzz;
mov	r11, r11.z
mov	r7, r11;

mul	r11, r7, c27;
mul	r11, r11, c31;
mov	r8, r11;

max	r11, r8, c0.xxxx;
mov	r8, r11;

add	r11, r1, r2;
add	r11, r11, r8;
mov	oD0, r11;

; xform to view space
 m4x4 r9,r0,c8
; xform to proj space
 m4x4 r10, r9, c12
; store output position
 mov oPos, r10

