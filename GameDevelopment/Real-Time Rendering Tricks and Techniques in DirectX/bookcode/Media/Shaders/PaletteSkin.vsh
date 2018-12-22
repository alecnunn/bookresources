vs.1.1

;Retrieve the address for the first bone matrix
mov a0.x, v7.x

;Transform the position by the first bone matrix
dp4 r1.x, v0, c[a0.x]
dp4 r1.y, v0, c[a0.x + 1]
dp4 r1.z, v0, c[a0.x + 2]

;Transform the normal
dp3 r3.x, v3, c[a0.x]
dp3 r3.y, v3, c[a0.x + 1]
dp3 r3.z, v3, c[a0.x + 2]

;Retrieve the address for the second bone matrix
mov a0.x, v7.z

;Repeat the position and normal calculations for the 
;second bone matrix.
dp4 r2.x, v0, c[a0.x]
dp4 r2.y, v0, c[a0.x + 1]
dp4 r2.z, v0, c[a0.x + 2]
dp3 r4.x, v3, c[a0.x]
dp3 r4.y, v3, c[a0.x + 1]
dp3 r4.z, v3, c[a0.x + 2]

;r1 and r2 now contain the two transformed positions.
;Scale r1 by the first bone weight and r2 by the 
;second bone weight. The two weights should sum up to
;1.0, so adding the two positions together should give
;the final interpolated transformed position(stored in
;r2). Finally, set r2.w to 1.0.
mul r1.xyz, r1.xyz, v7.y
mad r2.xyz, r2.xyz, v7.w, r1.xyz
mov r2.w, c6.x

;Perform the same interpolation calculation for the
;normal vector and store the result in r4. Don't bother
;set r4.w because it's never used.
mul r3.xyz, r3.xyz, v7.y
mad r4.xyz, r4.xyz, v7.w, r3.xyz

;Normalize the normal
dp3 r4.w, r4.xyz, r4.xyz
rsq r4.w, r4.w
mul r4, r4, r4.w

;Find the dot product of the normal and the object space
;light direction (negate the light direction to get the
;direction from the vertex to the light). This is for very simple
;diffuse lighting - assume the vertex color is white and the
;light is white. The "color" of the lit vertex is then equal to
;the dot product.
dp3 r0.x, r4, -c4

;Transform the interpolated position to screen space.
dp4 oPos.x, r2, c0
dp4 oPos.y, r2, c1
dp4 oPos.z, r2, c2
dp4 oPos.w, r2, c3

;Add some ambient lighting to the mix. For the skeleton
;vertices, the ambient lighting is set to a full color, 
;effectively coloring the vertices through the ambient 
;constant.
add oD0, r0.x, c5


