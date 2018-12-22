vs.1.1

;Do the final transformation and emit it in oPos. This
;is completely independent of the textured reflect/refract
;operations below.
dp4 oPos.x, v0, c0
dp4 oPos.y, v0, c1
dp4 oPos.z, v0, c2
dp4 oPos.w, v0, c3

;Convert the position and normal to world space to make
;it consistent with the cube map. When we're doing lighting,
;we can convert to object space, but now we are interacting
;with something that we can't easily sync up to our object space
dp4 r0.x, v0, c4
dp4 r0.y, v0, c5
dp4 r0.z, v0, c6

dp3 r1.x, v3, c4
dp3 r1.y, v3, c5
dp3 r1.z, v3, c6

;Now we need to renormalize the transformed normal. You'll see
;one of the reasons why this is important when we get to the
;refraction calculations
dp3 r1.w, r1, r1
rsq r1.w, r1.w
mul r1, r1, r1.w

;Compute the vector from the eye to the world space vertex
;and normalize.
sub r2, r0.xyz, c8.xyz
dp3 r2.w, r2, r2
rsq r2.w, r2.w		
mul r2, r2, r2.w

;Compute the reflection vector. First compute eye dot (world space) normal
;and double it.
dp3 r3, r2, r1
add r3, r3, r3

;Now, multiply it by the normal and subtract it from the eye 
;vector
mad oT0.xyz, r1, -r3, r2

;For refraction, we must shorten the normal (the chapter text shows why).
;The amount of shortening determines the "index of refraction" value.
mul r1, r1, c10.xxxx

;Repeat the reflection calculation with the shortened normal
;and use it for the other texture coordinate.
dp3 r3, r2, r1
add r3, r3, r3
mad oT1.xyz, r1, -r3, r2


