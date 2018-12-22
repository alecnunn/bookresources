vs.1.1

;Move the angle value to r1.x. Multiply by the angle manipulator
mul r1.x, v0.x, c6.x

;Compute first 4 values in sine Taylor series
;First compute the square, then use that to compute
; the odd exponents
mul r2.x, r1.x, r1.x
mul r1.y, r2.x, r1.x
mul r1.z, r1.y, r2.x
mul r1.w, r1.y, r2.x

;Compute the 4 values for Cosine. Use the 1.0 value from c5.x
;for the first term
mov r3.x, c5.x
mov r3.y, r2.x
mul r3.z, r2.x, r2.x
mul r3.w, r2.x, r3.z

;Compute the value for sine and output
;that value directly into r0.x. Again, c5.x is used for 1.0
mul r1, r1, c4
dp4 r0.x, r1, c5.x

;Compute the cosine and output to r0.y
mul r3, r3, c5
dp4 r0.y, r3, c5.x

;Set r3.x to the radius * the radius scaler
mul r3.x, v0.y, c6.y

;Now...  the real X value equal Radius * cos(angle), the 
;real Z value equal Radius * sin(Angle), and the Y value
;is the height * the height scaler.
;Set r4.w to v0.w. r4 now contains the cartesian model.
mul r4.x, r3.x, r0.y
mul r4.y, v0.z, c6.z
mul r4.z, r3.x, r0.x
mov r4.w, v0.w

;Do the final transformation
dp4 oPos.x, r4, c0
dp4 oPos.y, r4, c1
dp4 oPos.z, r4, c2
dp4 oPos.w, r4, c3

;output the color
mov oD0, v5

