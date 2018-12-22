!!FP1.0
# cgc version 1.3.0001, build date Aug  4 2004 10:01:10
# command line args: -profile fp30
# source file: collision_tri_point.cg
#vendor NVIDIA Corporation
#version 1.0.02
#profile fp30
#program main
#semantic main.attr_rect : TEXUNIT0
#semantic main.tri_rect : TEXUNIT1
#var float2 par_tri_coord : $vin.TEX0 : TEX0 : 0 : 1
#var samplerRECT attr_rect : TEXUNIT0 : texunit 0 : 1 : 1
#var samplerRECT tri_rect : TEXUNIT1 : texunit 1 : 2 : 1
#var float3 result : $vout.COL : COL : 3 : 1
FLRR  R0.x, f[TEX0].y;
MULR  R3.x, R0, {8};
MOVR  R3.y, {0}.x;
ADDR  R0.xy, R3, {3, 0};
TEX   R0.xyz, R0, TEX1, RECT;
MOVR  R1.x, f[TEX0];
MOVR  R1.y, {0}.x;
TEX   R1.xyz, R1, TEX0, RECT;
ADDR  R2.xyz, R1, -R0;
ADDR  R0.xy, R3, {1, 0};
TEX   R0.xyz, R0, TEX1, RECT;
DP3R  R0.w, R0, R2;
TEX   R0.xyz, R3, TEX1, RECT;
DP3R  R0.x, R0, R2;
ADDR  R0.y, R0.x, R0.w;
SGTR  H0.y, {0}.x, R0.w;
SGTR  H0.x, {0}, R0;
ADDX_SAT H0.x, H0, H0.y;
SGTR  H0.z, R0.y, {1}.x;
ADDXC_SAT HC.x, H0, H0.z;
ADDR  R0.xy, R3, {2, 0};
TEX   R0.xyz, R0, TEX1, RECT;
MOVR  R0.w, {0.016}.x;
DP3R  R0.w(LE.x), R0, R2;
ADDR  R2.xy, R3, {7, 0};
TEX   R2.xyz, R2, TEX1, RECT;
ADDR  R2.xyz, R1, -R2;
ADDR  R1.xy, R3, {4, 0};
TEX   R1.xyz, R1, TEX1, RECT;
DP3R  R1.w, R1, R2;
ADDR  R1.xy, R3, {5, 0};
TEX   R1.xyz, R1, TEX1, RECT;
DP3R  R1.x, R1, R2;
ADDR  R1.y, R1.w, R1.x;
SGTR  H0.x, {0}, R1.w;
SGTR  H0.y, {0}.x, R1.x;
ADDX_SAT H0.x, H0, H0.y;
SGTR  H0.y, R1, {1}.x;
ADDXC_SAT HC.x, H0, H0.y;
ADDR  R1.xy, R3, {6, 0};
TEX   R1.xyz, R1, TEX1, RECT;
MOVR  R1.w, {0.016}.x;
DP3R  R1.w(LE.x), R1, R2;
SLTR  H0.x, R0.w, R1.w;
MOVRC RC.x, H0;
ADDR  R0.w, -R0, {0.008}.x;
ADDR  R0.w(EQ.x), -R1, {0.008}.x;
MOVRC RC.x, H0;
MOVR  R0.xyz(EQ.x), R1;
MOVR  R1.x, f[TEX0];
MOVR  R1.y, {1}.x;
TEX   R1.xyz, R1, TEX0, RECT;
DP3R  R1.x, R1, R0;
MULR  R1.x, -R1, {64};
MADR  R1.x, R0.w, {30000}, R1;
MOVR  o[COLR].xyz, {0}.x;
SGTRC HC.x, R0.w, {1e-006};
MULR  o[COLR].xyz(NE.x), R1.x, R0;
END
# 58 instructions, 4 R-regs, 1 H-regs
