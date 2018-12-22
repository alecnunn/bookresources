!!FP1.0
# cgc version 1.3.0001, build date Aug  4 2004 10:01:10
# command line args: -profile fp30
# source file: update_position.cg
# source file: ./sph.cg
#vendor NVIDIA Corporation
#version 1.0.02
#profile fp30
#program main
#semantic main.attr_rect : TEXUNIT0
#semantic main.timestep
#semantic MASS
#semantic SMOOTHING_LENGTH
#semantic VISCOSITY
#var fixed index : $vin.TEX0 : TEX0 : 0 : 1
#var samplerRECT attr_rect : TEXUNIT0 : texunit 0 : 1 : 1
#var float timestep :  :  : 2 : 1
#var float3 result : $vout.COL : COL : 3 : 1
#var float MASS :  :  : -1 : 0
#var float SMOOTHING_LENGTH :  :  : -1 : 0
#var float VISCOSITY :  :  : -1 : 0
#default MASS = 0.00020543
#default SMOOTHING_LENGTH = 0.01
#default VISCOSITY = 0.1
DECLARE timestep;
MOVH  H0.x, f[TEX0];
MOVH  H0.y, {1}.x;
TEX   R0.xyz, H0, TEX0, RECT;
MOVH  H0.x, f[TEX0];
MOVH  H0.y, {2}.x;
TEX   R1.xyz, H0, TEX0, RECT;
MADR  R1.xyz, -R0, {1.1}.x, R1;
ADDR  R1.xyz, R1, {0, -9.8}.xxyw;
MADR  R1.xyz, R1, timestep.x, R0;
ADDR  R0.xyz, R0, R1;
MULR  R1.xyz, timestep.x, R0;
MOVH  H0.x, f[TEX0];
MOVH  H0.y, {5}.x;
TEX   R0.xyz, H0, TEX0, RECT;
MADR  o[COLR].xyz, R1, {0.5}.x, R0;
END
# 15 instructions, 2 R-regs, 1 H-regs
