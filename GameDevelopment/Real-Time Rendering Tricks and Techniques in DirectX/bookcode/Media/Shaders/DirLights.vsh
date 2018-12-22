vs.1.1

;Do the final transformation and emit it in oPos. This
;is completely independent of the lighting operations
dp4 oPos.x, v0, c0
dp4 oPos.y, v0, c1
dp4 oPos.z, v0, c2
dp4 oPos.w, v0, c3

;Dot product of light vector with normal vector
;and store it in r0.x for use with the lit instruction
dp3 r0.x, v3, -c5

;Compute the Vector from eye to vertex in object space
;and store the vector in r1
sub r1, c6, v0

;Normalize the eye vector
dp3 r1.w, r1, r1
rsq r1.w, r1.w
mul r1, r1, r1.w

;Compute half angle between the eye vector and the light
;vector. The result in stored in r2
add r2, r1, -c5
dp3 r2.w, r2, r2
rsq r2.w, r2.w
mul r2, r2, r2.w

;Find N dot H and store the result in r0.y
dp3 r0.y, r2, v3

;Place the power in r0.w
mov r0.w, c7.w

;Find the final diffuse and specular intensities
lit r4, r0

;Compute the specular light value
mul r5, r4.z, c7

;Compute the diffuse light value and add the ambient light
mul r6, r4.y, c8
mad r6, r6, v5, c9

;Add the specular component and output the final color
add oD0, r5, r6


