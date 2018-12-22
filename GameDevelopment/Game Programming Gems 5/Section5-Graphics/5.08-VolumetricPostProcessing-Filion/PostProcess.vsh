vs_1_1
dcl_position v0
mul r0, v0.x, c0
mad r2, v0.y, c1, r0
mad r4, v0.z, c2, r2
mad oPos, v0.w, c3, r4
