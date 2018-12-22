vs.1.1

;Compute the position
dp4 oPos.x, v0, c0
dp4 oPos.y, v0, c1
dp4 oPos.z, v0, c2
dp4 oPos.w, v0, c3

;Compute the Vector from eye to vertex in object space
;and store the vector in r1
sub r1, c6, v0

;Normalize the eye vector
dp3 r1.w, r1, r1
rsq r1.w, r1.w
mul r1, r1, r1.w

;Find Eye-vertex vector dot Eye direction
;and use the lit instruction to generate a higher
;power of the value to help contrain the effect.
;the .xy swizzle is used to load r4.x with some value
;even though it's not really used.
dp3 r4.xy, r1, -c10
mov r4.w, c11.y
lit r4, r4

;Load oD0 with the vertex color (There are no lighting
;effects in this sample)
mov oD0, v5

;Compute the transparency by subtracting the "XRay" 
;value from 1 and setting the alpha channel of the color.
sub oD0.w, c11.x, r4.z


