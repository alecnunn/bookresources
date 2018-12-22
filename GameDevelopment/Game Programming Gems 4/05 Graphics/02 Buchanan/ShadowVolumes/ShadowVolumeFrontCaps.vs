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


dp3 r0, r0, v1
mov oD0, r0


;	Make r0 (1, 1, 1, 1) if normal is facing light else (0, 0, 0, 0)
sge r0, r0, c[0].xxxx


;	Transform position to clip space
dp4 r1.x, v0, c[1]
dp4 r1.y, v0, c[2]
dp4 r1.z, v0, c[3]
dp4 r1.w, v0, c[4]


mul oPos, r0, r1