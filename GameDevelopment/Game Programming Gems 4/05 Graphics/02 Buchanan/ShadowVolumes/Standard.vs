;	c[0]	0.0, 0.5, 1.0, 2.0
;	c[1-4]	world*view*projection matrix
;	c[5]	light position
;	c[6]	light colour
;	c[7]	ambient light

vs.1.1


dcl_position	v0
dcl_normal		v1


;	Transform position to clip space
dp4 oPos.x, v0, c[1]
dp4 oPos.y, v0, c[2]
dp4 oPos.z, v0, c[3]
dp4 oPos.w, v0, c[4]


;	Get vector from vertex to light and normalise
sub r0, c[5].xyz, v0
dp3 r0.w, r0, r0
rsq r0.w, r0.w
mul r0, r0, r0.w


;	(Normal dot light vector)*light colour
dp3 r0, v1, r0
mul r0, r0, c[6]


;	Ambient + diffuse
add oD0, r0, c[7]