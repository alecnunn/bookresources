vs.1.1

;Do the final transformation and emit it in oPos. This
;is completely independent of the lighting operations
dp4 oPos.x, v0, c0
dp4 oPos.y, v0, c1
dp4 oPos.z, v0, c2
dp4 oPos.w, v0, c3

;Transform the basis vectors with the world matrix
;v8, v9, and v10 are the basis vectors
;c10-c2 are the first three rows of the world matrix
;Place the results in oT1 - oT3
dp3 oT1.x, v8,  c10
dp3 oT1.y, v9,  c10
dp3 oT1.z, v10, c10

dp3 oT2.x, v8,  c11
dp3 oT2.y, v9,  c11
dp3 oT2.z, v10, c11

dp3 oT3.x, v8,  c12
dp3 oT3.y, v9,  c12
dp3 oT3.z, v10, c12

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

;Get the world space position
dp3 r0.x, v0, c10
dp3 r0.y, v0, c11
dp3 r0.z, v0, c12

;Get the eye vector
sub r1.xyz, c5.xyz, r0.xyz

dp3 r1.w, r1.xyz, r1.xyz
rsq r1.w, r1.w
mul r1.xyz, r1, r1.w

;Place the vector in the w components of the 
;texture coordinates
mov oT1.w, r1.x
mov oT2.w, r1.y
mov oT3.w, r1.z

;Output the texture coordinates
mov oT0.xy, v7.xy

