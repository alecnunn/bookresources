// mModelViewProj: registers c0-c3 (4 registers total)
// mModelView: registers c4-c7 (4 registers total)
// mDepthBufferMatrix: registers c8-c11 (4 registers total)
// vNearFar: register c12 (1 register total)
vs_1_1
dcl_position v0
mul r0, v0.x, c0
mad r2, v0.y, c1, r0
mad r4, v0.z, c2, r2
mad oPos, v0.w, c3, r4
mul r6.xy, v0.x, c8
mad r8.xy, v0.y, c9, r6
mad r10.xy, v0.z, c10, r8
mad oT1.xy, v0.w, c11, r10
mul r0.w, v0.x, c4.z
mad r2.w, v0.y, c5.z, r0.w
mad r4.w, v0.z, c6.z, r2.w
mad r6.w, v0.w, c7.z, r4.w
max r1.w, r6.w, c12.x
min r8.w, r1.w, c12.y
add r10.w, r8.w, -c12.x
add r5.w, -c12.x, c12.y
rcp r0.w, r5.w
mul oT0.x, r10.w, r0.w
