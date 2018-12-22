vs.1.1

; vertex layout
dcl_position0    v0
dcl_normal0      v1
dcl_texcoord0    v2
dcl_texcoord1    v3

; Do some basic lighting
dp3  r0.x, v1, -c21
add  oD0, r0.x, c4

; transform
m4x4 oPos, v0, c0
mov  oT0, v2
mov  oT1, v3
