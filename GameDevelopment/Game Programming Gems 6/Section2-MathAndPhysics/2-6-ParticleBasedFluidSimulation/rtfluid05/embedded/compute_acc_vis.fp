!!FP1.0
# cgc version 1.3.0001, build date Aug  4 2004 10:01:10
# command line args: -profile fp30
# source file: compute_acc_vis.cg
# source file: ./kernel.cg
# source file: ./sph.cg
#vendor NVIDIA Corporation
#version 1.0.02
#profile fp30
#program main
#semantic main.attr_rect : TEXUNIT0
#semantic main.neighbour_rect : TEXUNIT1
#semantic PI
#semantic MASS
#semantic SMOOTHING_LENGTH
#semantic VISCOSITY
#var fixed2 sn_coord : $vin.TEX0 : TEX0 : 0 : 1
#var samplerRECT attr_rect : TEXUNIT0 : texunit 0 : 1 : 1
#var samplerRECT neighbour_rect : TEXUNIT1 : texunit 1 : 2 : 1
#var float3 result : $vout.COL : COL : 3 : 1
#var float PI :  :  : -1 : 0
#var float MASS :  :  : -1 : 0
#var float SMOOTHING_LENGTH :  :  : -1 : 0
#var float VISCOSITY :  :  : -1 : 0
#default PI = 3.141593
#default MASS = 0.00020543
#default SMOOTHING_LENGTH = 0.01
#default VISCOSITY = 0.1
MOVR  R0.x, f[TEX0].y;
MOVR  R0.y, {0}.x;
TEX   R1.xyz, R0, TEX0, RECT;
TEX   R2.x, f[TEX0], TEX1, RECT;
MOVR  R0.y, {0}.x;
MOVR  R0.x, R2;
TEX   R3.xyz, R0, TEX0, RECT;
ADDR  R0.xyz, R1, -R3;
ADDR  R1.xyz, R3, -R1;
DP3R  R0.w, R1, R1;
RSQR  R0.w, R0.w;
RCPR  R0.w, R0.w;
ADDR  R1.z, -R0.w, {0.01}.x;
MOVR  R1.x, f[TEX0].y;
MOVR  R1.y, {4}.x;
TEX   R2.y, R1, TEX0, RECT;
MOVR  R1.x, R2;
MOVR  R1.y, {4}.x;
TEX   R1.xy, R1, TEX0, RECT;
ADDR  R1.y, R2, R1;
MULR  R1.y, R1, R1.z;
MULR  R1.y, R1, R1.z;
MOVR  R2.y, {1}.x;
MULR  R0.xyz, R1.y, R0;
TEX   R2.xyz, R2, TEX0, RECT;
MOVR  R3.x, f[TEX0].y;
MOVR  R3.y, {1}.x;
TEX   R3.xyz, R3, TEX0, RECT;
ADDR  R2.xyz, R2, -R3;
MULR  R2.xyz, R2, {0.3}.x;
MADR  R0.xyz, R0, {0.5}.x, R2;
MULR  R0.xyz, R1.z, R0;
MULR  R0.xyz, R0, R1.x;
MOVR  o[COLR].xyz, {0}.x;
SGTRC HC.x, -R0.w, {-0.01};
MULR  o[COLR].xyz(NE.x), R0, {9.80856e+008}.x;
END
# 36 instructions, 5 R-regs, 0 H-regs
