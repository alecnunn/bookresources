vs.1.1

;The two sets of basis functions are 
;v7 and v8. Copy those to r7 and r8 to get
;around access issues.
mov r7, v7
mov r8, v8

;Copy the functions for the tangent vectors
;(v9 and v10) to r9 and r10
mov r9, v9
mov r10, v10

;Each block below takes the form of two lines
;to compute the actual shape deformation  (r1) and
;two lines to compute the tangent vectors (r2 and r3)
;that will be used to find the normal of the deformed point.

;S0T0 control point
mul r0.x, r7.x, r8.x
mul r1, c10, r0.x

mul r2, c10, r9.x
mul r3, c10, r10.x

;S0T1 control point
mul r0.x, r7.x, r8.y
mad r1, c11, r0.x, r1

mad r2, r9.x,  c11, r2
mad r3, r10.y, c11, r3

;S0T2 control point
mul r0.x, r7.x, r8.z
mad r1, c12, r0.x, r1

mad r2, r9.x,  c12, r2
mad r3, r10.z, c12, r3

;S0T3 control point
mul r0.x, r7.x, r8.w
mad r1, c13, r0.x, r1

mad r2, r9.x,  c13, r2
mad r3, r10.w, c13, r3

;S1T0 control point
mul r0.x, r7.y, r8.x
mad r1, c14, r0.x, r1

mad r2, r9.y,  c14, r2
mad r3, r10.x, c14, r3

;S1T1 control point
mul r0.x, r7.y, r8.y
mad r1, c15, r0.x, r1

mad r2, r9.y,  c15, r2
mad r3, r10.y, c15, r3

;S1T2 control point
mul r0.x, r7.y, r8.z
mad r1, c16, r0.x, r1

mad r2, r9.y,  c16, r2
mad r3, r10.z, c16, r3

;S1T3 control point
mul r0.x, r7.y, r8.w
mad r1, c17, r0.x, r1

mad r2, r9.y,  c17, r2
mad r3, r10.w, c17, r3

;S2T0 control point
mul r0.x, r7.z, r8.x
mad r1, c18, r0.x, r1

mad r2, r9.z,  c18, r2
mad r3, r10.x, c18, r3

;S2T1 control point
mul r0.x, r7.z, r8.y
mad r1, c19, r0.x, r1

mad r2, r9.z,  c19, r2
mad r3, r10.y, c19, r3

;S2T2 control point
mul r0.x, r7.z, r8.z
mad r1, c20, r0.x, r1

mad r2, r9.z,  c20, r2
mad r3, r10.z, c20, r3

;S2T3 control point
mul r0.x, r7.z, r8.w
mad r1, c21, r0.x, r1

mad r2, r9.z,  c21, r2
mad r3, r10.w, c21, r3

;S3T0 control point
mul r0.x, r7.w, r8.x
mad r1, c22, r0.x, r1

mad r2, r9.w,  c22, r2
mad r3, r10.x, c22, r3

;S3T1 control point
mul r0.x, r7.w, r8.y
mad r1, c23, r0.x, r1

mad r2, r9.w,  c23, r2
mad r3, r10.y, c23, r3

;S3T2 control point
mul r0.x, r7.w, r8.z
mad r1, c24, r0.x, r1

mad r2, r9.w,  c24, r2
mad r3, r10.z, c24, r3

;S3T3 control point
mul r0.x, r7.w, r8.w
mad r1, c25, r0.x, r1

mad r2, r9.w,  c25, r2
mad r3, r10.w, c25, r3

;r1 now contains the new point. Undo the (+0.5) correction
;that was done when the vertex buffer was initialized
add r1, r1, c6

;Compute the cross product of r2 and r3 to get 
;the normal vector
mul r6, r3.yzxw, r2.zxyw
mad r6, -r2.yzxw, r3.zxyw, r6

;Normalize the normal
dp3 r5.w, r6, r6
rsq r5.w, r5.w
mul r6, r6, r5.w

;Compute the simple diffuse lighting. The direction is negated to
;account for the direction relative to the mesh.
dp3 oD0, r6, -c5


;Do the final transformation
dp4 oPos.x, r1, c0
dp4 oPos.y, r1, c1
dp4 oPos.z, r1, c2
dp4 oPos.w, r1, c3


