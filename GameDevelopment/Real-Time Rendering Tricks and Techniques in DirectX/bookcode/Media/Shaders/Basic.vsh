vs.1.1

;Do the final transformation and emit it in oPos. This
;is completely independent of the lighting operations
dp4 oPos.x, v0, c0
dp4 oPos.y, v0, c1
dp4 oPos.z, v0, c2
dp4 oPos.w, v0, c3

;Dot product of light vector with normal vector
;and store it in r0.x.
dp3 r0.x, v3, -c4

;Multiply by the diffuse color and add the ambient
mad oD0, r0.x, v5, c5

