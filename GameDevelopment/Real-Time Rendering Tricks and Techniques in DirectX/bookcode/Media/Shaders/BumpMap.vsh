vs.1.1

;Do the final transformation and emit it in oPos. This
;is completely independent of the lighting operations
dp4 oPos.x, v0, c0
dp4 oPos.y, v0, c1
dp4 oPos.z, v0, c2
dp4 oPos.w, v0, c3

;transform light vector by the transformed basis vectors
dp3 r0.x, v8.xyz,  -c4
dp3 r0.y, v9.xyz,  -c4
dp3 r0.z, v10.xyz, -c4

;Normalize the newly transformed light vector
dp3 r0.w, r0, r0
rsq r0.w, r0.w
mul r0, r0, r0.w

;place it in the range from zero to one and place it in the 
;diffuse color.
mad oD0, r0, c6.x, c6.x

;Output the texture coordinates
mov oT0, v7





