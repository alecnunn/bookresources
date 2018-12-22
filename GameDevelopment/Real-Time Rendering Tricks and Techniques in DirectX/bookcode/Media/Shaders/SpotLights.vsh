vs.1.1

;Do the final transformation and emit it in oPos. This
;is completely independent of the lighting operations
dp4 oPos.x, v0, c0
dp4 oPos.y, v0, c1
dp4 oPos.z, v0, c2
dp4 oPos.w, v0, c3

;Compute the vector from the vertex to the light and
;store the value in r0.
sub r0, c4, v0

;To get the attenuation factor, compute d^2 and 1/d and
;send those factors to the dst instruction. combine the results
;of the dst instruction with the attenuation constants and
;get the reciprocal for the final attenuation factor.

;First, get d^2
dp3 r1.w, r0, r0

;Then, 1/d. R1 and r2 now hold the values to be sent to
;the dst instruction.
rsq r2.w, r1.w

;Before moving to the dst instruction, use r2.w to
;normalize the vertex to light vector
mul r0, r0, r2.w

;Get the distance values, multiply those by the attenuation
;constants, and get the reciprocal
dst r1, r1.wwww, r2.wwww
dp3 r1.w, r1, c10
rcp r1.w, r1.w

;r0 now contains the normalized vertex to light vector and
;r1.w contains the attenuation factor. Onto the Umbra/Penumbra.
;For simplicity, this example has a linear falloff through the
;penumbra. You could implement a nonlinear falloff factor using
;a few more instructions and the concepts from Chapter 11.

;Compute the dot product of the light direction with the 
;vertex-to-light vector (both should be normalized).
dp3 r5.x, -r0, c5

;Subtract from the umbra value
sub r5.y, c11.x, r5.x

;Scale it with the 1/(umbra-penumbra) constant and find
;the complement to actually compute the falloff. 
mad r5.y, -c11.z, r5.y, c11.w

;Its possible that points inside of the umbra will have values greater than 1. 
;Use max to clamp it to 1.
min r5.y, r5.y, c11.w

;Use sge to create a multiplier that will be used to clamp values outside
;of the penumbra. Use the original value (r5.x) to do the comparison. If the 
;value is outside the penumbra, r6.w will be set to zero, otherwise it will be
;1.0. multiplying that value by the scaled value will either set it to zero
;or retain the scaled value
sge r6.w, r5.x, c11.y 
mul r5.y, r5.y, r6.w

;r5.y now contains the falloff term
;The rest in very similar to the directional lights

;Dot product of light vector with normal vector
;and store it in r0.x for use with the lit instruction
dp3 r0.x, v3, r0

;Compute the Vector from eye to vertex in object space
;and store the vector in r2
sub r2, c6, v0

;Normalize the eye vector
dp3 r2.w, r2, r2
rsq r2.w, r2.w
mul r2, r2, r2.w

;Compute half angle between the eye vector and the light
;vector. The result in stored in r3
add r3, r2, r0
dp3 r4.x, r3, r3
rsq r4, r4.x
mul r3, r3, r4

;Find N dot H and store the result in r0.y
dp3 r0.y, r3, v3

;Place the power in r0.w
mov r0.w, c7.w

;Find the final diffuse and specular intensities
lit r4, r0

;Scale the result by the attenuation factor and falloff
mul r4, r4, r1.w
mul r4, r4, r5.y

;Compute the specular light value
mul r5, r4.z, c7

;Compute the diffuse light value and add the ambient light
mul r6, r4.y, c8
mad r6, r6, v5, c9

;Add the specular component and output the final color
add oD0, r5, r6



