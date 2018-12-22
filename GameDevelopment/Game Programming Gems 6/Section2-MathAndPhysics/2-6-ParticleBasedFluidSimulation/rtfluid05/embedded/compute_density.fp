!!FP1.0
# cgc version 1.3.0001, build date Aug  4 2004 10:01:10
# command line args: -profile fp30
# source file: compute_density.cg
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
#var float result : $vout.COL : COL : 3 : 1
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
TEX   R0.xyz, R0, TEX0, RECT;
TEX   R1.x, f[TEX0], TEX1, RECT;
MOVR  R1.y, {0}.x;
TEX   R1.xyz, R1, TEX0, RECT;
ADDR  R0.xyz, R0, -R1;
DP3R  R0.x, R0, R0;
ADDR  R0.y, -R0.x, {0.0001}.x;
MULR  R0.z, R0.y, R0.y;
MULR  R0.y, R0, R0.z;
MOVR  o[COLR].x, {0};
SGTRC HC.x, -R0, {-0.0001};
MULR  o[COLR].x(NE), R0.y, {3.21843e+014};
END
# 14 instructions, 2 R-regs, 0 H-regs
