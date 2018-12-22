; Copyright (C) Greg James, 2001. 
; All rights reserved worldwide.
;
; This software is provided "as is" without express or implied
; warranties. You may freely copy and compile this source into
; applications you distribute provided that the copyright text
; below is included in the resulting source code, for example:
; "Portions Copyright (C) Greg James, 2001"
;
; v0 = vertex position
; v1 = vertex texture coordinate

; Transform vertex position to clip space.  4-vec * 4x4-matrix
dp4 oPos.x, v0, c[ WORLDVIEWPROJ_0 ]
dp4 oPos.y, v0, c[ WORLDVIEWPROJ_1 ]
dp4 oPos.z, v0, c[ WORLDVIEWPROJ_2 ]
dp4 oPos.w, v0, c[ WORLDVIEWPROJ_3 ]

; Read which set of offsets to use - set A or B
mov a0.x, c[OFFSET_TO_USE ].x

; Write S,T,R,Q coordinates to all four texture stages, offsetting
;  each by either offset_a(1-4) or offset_b(1-4)

add oT0, v1, c[ a0.x + T0_BASE ]
add oT1, v1, c[ a0.x + T1_BASE ] 
add oT2, v1, c[ a0.x + T2_BASE ] 
add oT3, v1, c[ a0.x + T3_BASE ]
