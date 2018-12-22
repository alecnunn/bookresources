!!FP1.0
# cgc version 1.3.0001, build date Aug  4 2004 10:01:10
# command line args: -profile fp30
# source file: transform_obs.cg
#vendor NVIDIA Corporation
#version 1.0.02
#profile fp30
#program main
#semantic main.tri_rect : TEXUNIT0
#semantic main.mat
#var float2 tri_coord : $vin.TEX0 : TEX0 : 0 : 1
#var samplerRECT tri_rect : TEXUNIT0 : texunit 0 : 1 : 1
#var float4x4 mat :  : c[0], 4 : 2 : 1
#var float result : $vout.COL : COL : 3 : 1
DECLARE mat$0;
DECLARE mat$1;
DECLARE mat$2;
DECLARE mat$3;
MOVR  R0.w, {1}.x;
TEX   R0.xyz, f[TEX0], TEX0, RECT;
DP4R  o[COLR].x, R0, mat$0;
END
# 3 instructions, 1 R-regs, 0 H-regs
