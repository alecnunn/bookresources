vs.1.1

;Do the final transformation and emit it in oPos.
dp4 oPos.x, v0, c0
dp4 oPos.y, v0, c1
dp4 oPos.z, v0, c2
dp4 oPos.w, v0, c3

;Get the distance from the light to the vertex.
sub r0, v0, c8

;Divide each component by the range value
mul r0, r0, c9.x

;Divide by 2 and add 0.5
mad r0, r0, c9.yyyy, c9.yyyy

;Map the x and y components into the first texture
mov oT0.xy, r0.xy

;Map the z component into the second texture
mov oT1.x, r0.z

;Move the diffuse color out
mov oD0, v5




