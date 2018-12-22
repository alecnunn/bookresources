vs.1.1

;Compute the final output position in screen space
;It is not output directly to oPos because it is
;still needed to compute the depth and oPos is
;write only.
dp4 r1.x, v0, c0
dp4 r1.y, v0, c1
dp4 r1.z, v0, c2
dp4 r1.w, v0, c3

;emit the position
mov oPos, r1

;subtract the near plane distance from r1.z (the distance)
sub r1.z, r1.z, c4.x

;multiply the resulting distance by the scaling factor
mul oD0, r1.z, c4.y



