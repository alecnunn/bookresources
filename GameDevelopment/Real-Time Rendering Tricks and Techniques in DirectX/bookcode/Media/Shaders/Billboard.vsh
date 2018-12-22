vs.1.1

;v3 doesn't contain a normal vector, it contains the
;vectors that determine how the billboard is scaled.
;Multiply this vector by the scaling factor defined by c7.
mul r2, v3, c7

;Use the scaled expansion vector in r2 to expand the vectex
;in the correct up and side directions. The result is 
;stored in r1
mad r1, r2.xxx, c4, v0  
mad r1, r2.yyy, c5, r1

;This instruction is specific to this example. It moves the
;vertex slightly closer to the eye by sliding it aLong the eye
;vector. This ensures that the light flare is always in front
;of the light fixture, but not in front of the lamp post itself.
;This is not really needed for general billboarding, but a similar
;technique might be useful for other specific applications.
add r1, -c6, r1

;These final lines emit the transformed vertex position, color, 
;and texture coordinates.
dp4 oPos.x, r1, c0       
dp4 oPos.y, r1, c1       
dp4 oPos.z, r1, c2       
dp4 oPos.w, r1, c3
mov oT0,  v7         
mov oD0,  v5
