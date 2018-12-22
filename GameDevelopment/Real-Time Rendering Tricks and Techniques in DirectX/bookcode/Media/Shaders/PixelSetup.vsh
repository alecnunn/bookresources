vs.1.1

;Do the final transformation and emit it in oPos. This
;is completely independent of the lighting operations
dp4 oPos.x, v0, c0
dp4 oPos.y, v0, c1
dp4 oPos.z, v0, c2
dp4 oPos.w, v0, c3

;Dot product of light vector with normal vector
;and store it in oD1.
dp3 oD1.xyzw, v3, -c4

;Place the ambient value in oD0
mov oD0, c5

;Output the texture coordinates
mov oT0, v7




