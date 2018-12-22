!!FP1.0
# cgc version 1.3.0001, build date Aug  4 2004 10:01:10
# command line args: -profile fp30
# source file: water_fragment.cg
#vendor NVIDIA Corporation
#version 1.0.02
#profile fp30
#program main
#semantic main.environmentMap
#var float reflectionFactor : $vin.COL0 : COL0 : 0 : 1
#var float3 incident : $vin.TEX0 : TEX0 : 1 : 1
#var float3 N : $vin.TEX1 : TEX1 : 2 : 1
#var float4 color : $vout.COL : COL : 3 : 1
#var samplerCUBE environmentMap :  : texunit 0 : 4 : 1
DP3R  R0.x, f[TEX1], f[TEX1];
RSQR  R0.w, R0.x;
DP3R  R0.x, f[TEX0], f[TEX0];
RSQR  R0.x, R0.x;
MULR  R0.xyz, R0.x, f[TEX0];
MULR  R2.xyz, R0.w, f[TEX1];
DP3R  R1.x, -R0, R2;
MADR  R0.w, -R1.x, R1.x, {1}.x;
MULR  R0.w, R0, {1.69}.x;
ADDR  R1.y, -R0.w, {1}.x;
RSQR  R1.y, |R1.y|;
RCPR  R1.y, R1.y;
MADR  R1.x, R1, {1.3}, -R1.y;
MULR  R1.xyz, R1.x, R2;
SGTR  H0.x, -R0.w, {-1};
MADR  R1.xyz, R0, {1.3}.x, R1;
MULR  R1.xyz, R1, H0.x;
DP3R  R0.w, R2, R0;
MULR  R2.xyz, R2, R0.w;
MULR  R2.xyz, R2, {2}.x;
ADDR  R0.xyz, R0, -R2;
TEX   R1, R1, TEX0, CUBE;
TEX   R0, R0, TEX0, CUBE;
ADDR  R0, R0, -R1;
MADR  o[COLR], f[COL0].x, R0, R1;
END
# 25 instructions, 3 R-regs, 1 H-regs
