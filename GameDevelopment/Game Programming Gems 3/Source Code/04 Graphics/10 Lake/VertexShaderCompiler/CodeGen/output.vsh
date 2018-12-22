vs.1.0;
;----------------------------------------------------------------------------------------
;Shader produced by prototype version 0.01 of the Intel Programmable Shader Compiler
;Graphics and 3D Technologies Group, Intel Architecture Labs
;-----------------------------------------------------------------------------------------


;These 3 vectors are used for approximating sin and cos via Taylor Series
; xform to view space
 m4x4 r9,r0,c8
; xform to proj space
 m4x4 r10, r9, c12
; store output position
 mov oPos, r10

