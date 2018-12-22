vs.1.1

;Dot product of light vector with normal vector
;and store it in r0.x.
dp3 r0.x, v3, -c4

;Get the vector from the light to the vertex
sub r1, v0, c7

;If r0.x is less than zero, then this vertex is facing away
;from the light. If that's the case, push it farther.
slt r2.x, r0.x, c8.y
mul r2.x, r2.x, c8.x
mad r3, r2.x, r1, v0

;Do the final transformation and emit it in oPos. This
;is completely independent of the lighting operations
dp4 oPos.x, r3, c0
dp4 oPos.y, r3, c1
dp4 oPos.z, r3, c2
dp4 oPos.w, r3, c3


