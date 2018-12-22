vs.1.1

;scale the normal by the scalefactor in c5
mul r0, c5, v3

;add the vector to the original vertex position
add r0, r0, v0

;transform and output
dp4 oPos.x, r0, c0
dp4 oPos.y, r0, c1
dp4 oPos.z, r0, c2
dp4 oPos.w, r0, c3

mov oD0,  v5
