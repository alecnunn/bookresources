!!FP1.0
# cgc version 1.3.0001, build date Aug  4 2004 10:01:10
# command line args: -profile fp30
# source file: collision_glass.cg
#vendor NVIDIA Corporation
#version 1.0.02
#profile fp30
#program main
#semantic main.attr_rect : TEXUNIT0
#semantic main.mat_col
#semantic main.mat_inv_col
#var float pindex : $vin.TEX0 : TEX0 : 0 : 1
#var samplerRECT attr_rect : TEXUNIT0 : texunit 0 : 1 : 1
#var float4x4 mat_col :  : c[0], 4 : 2 : 1
#var float4x4 mat_inv_col :  : c[4], 4 : 3 : 1
#var float3 result : $vout.COL : COL : 4 : 1
DECLARE mat_col$0;
DECLARE mat_col$1;
DECLARE mat_col$2;
DECLARE mat_col$3;
DECLARE mat_inv_col$0;
DECLARE mat_inv_col$1;
DECLARE mat_inv_col$2;
DECLARE mat_inv_col$3;
MOVR  R0.x, f[TEX0];
MOVR  R0.y, {1}.x;
TEX   R0.xyz, R0, TEX0, RECT;
DP3R  R2.y, R0, mat_col$1;
DP3R  R2.x, R0, mat_col$0;
DP3R  R1.xw, R0, mat_col$2;
MOVR  R0.x, f[TEX0];
MOVR  R0.y, {0}.x;
TEX   R0.xyz, R0, TEX0, RECT;
MOVR  R0.w, {1}.x;
DP4R  R1.z, R0, mat_col$1;
DP4R  R1.y, R0, mat_col$0;
MOVR  R3.x, R1.y;
MOVR  R3.y, R1.z;
MOVR  R3.z, {0}.x;
DP3R  R2.z, R3, R3;
RSQR  R2.z, R2.z;
MULR  R3.xyz, R2.z, R3;
MOVR  R2.w, R1.z;
MOVR  R2.z, R1.y;
MULR  R2.zw, R2, R2;
ADDR  R1.y, R2.z, R2.w;
MOVR  R2.z, R1.x;
DP3R  R1.x, R2, -R3;
RSQR  R1.y, R1.y;
RCPR  R1.y, R1.y;
MULR  R1.x, -R1, {64};
ADDR  R1.y, R1, {-0.042}.x;
MADR  R1.x, R1.y, {25000}, R1;
SGTRC HC.x, R1.y, {1e-006};
MAXR  R2.w, R1.x, {0}.x;
MOVR  R1.xyz, {0}.x;
MULR  R1.xyz(GT.x), R2.w, -R3;
DP4R  R0.w, R0, mat_col$2;
MULR  R2.w, -R1, {64}.x;
ADDR  R1.w, -R0, {0.008}.x;
MOVR  R0.xyz, R1;
MADR  R2.w, R1, {25000}.x, R2;
SGTRC HC.x, R1.w, {1e-006};
MAXR  R2.w, R2, {0}.x;
MADR  R0.xyz(GT.x), R2.w, {0, 1}.xxyw, R1;
ADDR  R0.w, R0, {-0.182}.x;
DP3R  R1.w, R2, {0, -64}.xxyw;
MOVR  R1.xyz, R0;
MADR  R1.w, R0, {25000}.x, -R1;
MAXR  R1.w, R1, {0}.x;
SGTRC HC.x, R0.w, {1e-006};
MADR  R1.xyz(GT.x), R1.w, {0, -1}.xxyw, R0;
DP3R  o[COLR].z, R1, mat_inv_col$2;
DP3R  o[COLR].y, R1, mat_inv_col$1;
DP3R  o[COLR].x, R1, mat_inv_col$0;
END
# 51 instructions, 4 R-regs, 0 H-regs
