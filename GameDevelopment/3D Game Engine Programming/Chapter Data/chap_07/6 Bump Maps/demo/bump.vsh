vs.1.1                 
dcl_position  v0
dcl_normal    v3
dcl_texcoord  v7
dcl_tangent   v8
m4x4 oPos, v0, c0  ; transform position

m3x3 r5.xyz, v8, c31   ; rotate tangent (U)
mov  r5.w, c30.w
m3x3 r7.xyz, v3, c31   ; rotate normal (N and UxV)
mov  r7.w, c30.w

; calculate binormal V as
; crossproduct of U and N
mul r0,r5.zxyw,-r7.yzxw;
mad r6,r5.yzxw,-r7.zxyw,-r0;

; build vector to the light position
sub r2, c25, v0
dp3 r2.w, r2, r2
rsq r2.w, r2.w
mul r2.xyz, r2, r2.w

; transform light vector
dp3 r8.x, r5.xyz, r2
dp3 r8.y, r6.xyz, r2
dp3 r8.z, r7.xyz, r2

; *.5 + .5
mad oD0.xyz, r8.xyz, c30.x, c30.x   

mov oT0.xy, v7.xy
mov oT1.xy, v7.xy
