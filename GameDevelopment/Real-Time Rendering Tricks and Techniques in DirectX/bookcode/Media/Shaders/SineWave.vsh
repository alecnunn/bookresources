vs.1.1

;Move the position to the temporary value r0.
;Later, we'll set r0.y to the sine value...
mov r0, v0

;Compute where on the sine wave this point is
;by applying the wave function.
mad r1.x, v0.x, c4.y, c4.x

;Clamp the new value to the range of -pi to pi
;This section and the following section borrows
;heavily from sine samples from nVidia and Microsoft
add r1.x, r1.x, c6.x
mul r1.x, r1.x, c6.y
frc r1.xy, r1.x
mad r1.x, r1.x, c6.z, -c6.x

;Compute first 4 values in sine Taylor series
;First compute the square, then use that to compute
; the odd exponents
mul r2.x, r1.x, r1.x
mul r1.y, r2.x, r1.x
mul r1.z, r1.y, r2.x
mul r1.w, r1.y, r2.x

;Compute the value for sine and output
;that value directly into r0.y
mul r1, r1, c7
dp4 r0.y, r1, c7.x

;Set color. We add a little to make sure it's not
;black, but this causes some oversaturation effects
mad oD0, v5, r0.y, c4.w

;Scale for the amplitude and add that to the original value
mad r0.y, r0.y, c4.z, v0.y

;Do the final transformation
dp4 oPos.x, r0, c0
dp4 oPos.y, r0, c1
dp4 oPos.z, r0, c2
dp4 oPos.w, r0, c3

