vs.1.1

;Do the final transformation and emit it in oPos. This
;is completely independent of the lighting operations
dp4 oPos.x, v0, c0
dp4 oPos.y, v0, c1
dp4 oPos.z, v0, c2
dp4 oPos.w, v0, c3

;Dot product of light vector with normal vector
;and output it as the first texture coordinate.
dp3 oT0.x, v3, -c5

;Compute the Vector from eye to vertex in object space
;and store the vector in r1
sub r1, c6, v0

;Normalize the eye vector
dp3 r1.w, r1, r1
rsq r1.w, r1.w
mul r1, r1, r1.w

;Dot product the eye vector with the normal and output 
;as the second texture coordinate.
dp3 oT1.x, v3, r1

;Final color
mov oD0, v5

