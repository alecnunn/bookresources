;	c[0]	0.0, 0.5, 1.0, 2.0
;	c[1-4]	world*view*projection matrix
;	c[5]	light position

vs.1.1


dcl_position	v0
dcl_normal		v1


;	Get vector from vertex to light and normalise
sub r0, c[5], v0
dp3 r0.w, r0, r0
rsq r0.w, r0.w
mul r0, r0, r0.w


;	Dot light and normal vector
dp3 r1, v1, r0


;	Output shading
mov oD0, r1


;	Make r1 (0, 0, 0, 0) if normal is facing light else (1, 1, 1, 1)
slt r1, r1, c[0].xxxx


;	Make r2 (1, 1, 1, 1) if normal is not unit length else (0, 0, 0, 0)
mov r2, v1
dp3 r2, r2, r2
sge r2, r2, c[0].w

add r1, r1, r2
min r1, c[0].zzzz, r1


;	Extrude vertex if facing away from light
mul r0, r0, r1
mad r0, c[5].wwww, -r0, v0


;	Transform position to clip space
dp4 oPos.x, r0, c[1]
dp4 oPos.y, r0, c[2]
dp4 oPos.z, r0, c[3]
dp4 oPos.w, r0, c[4]