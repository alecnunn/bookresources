vs.1.1

;Do the final transformation and emit it in oPos.
dp4 oPos.x, v0, c0
dp4 oPos.y, v0, c1
dp4 oPos.z, v0, c2
dp4 oPos.w, v0, c3

;Transform the position by the texture matrix
;to create the projected texture coordinates
dp4 oT0.x, v0, c4
dp4 oT0.y, v0, c5
dp4 oT0.z, v0, c6
dp4 oT0.w, v0, c7

;Get the distance from the light to the vertex.
;Subtract the two vectors, get d^2, get the reciprocal
;square root and then flip it to get d.
sub r0, v0, c8
dp3 r0.w, r0, r0
rsq r1.w, r0.w
rcp r1.w, r1.w

;multiply d by 1/range to get the texture coordinate
;for the attenuation factor. The alpha channel contains
;the attenuation ramp. The y component of c9 contains 0,
;which effectively sets the y value of the texture
;coordinate to 0 just to give it some value.
mul oT1, r1.wwww, c9.xyyy

;Move the diffuse color out
mov oD0, v5


;This next line isthe alternate method
;Uncomment them to enable the pixel shader less
;technique and comment out the pixel shader lines
;in the application.
;mad oD0, r1.w, -c9.x, c9.w





