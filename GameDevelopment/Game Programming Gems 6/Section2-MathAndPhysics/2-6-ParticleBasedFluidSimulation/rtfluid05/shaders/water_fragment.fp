!!ARBfp1.0
# cgc version 1.3.0001, build date Aug  4 2004 10:01:10
# command line args: -profile arbfp1
# source file: water_fragment.cg
#vendor NVIDIA Corporation
#version 1.0.02
#profile arbfp1
#program main
#semantic main.environmentMap
#var float reflectionFactor : $vin.COL0 : COL0 : 0 : 1
#var float3 incident : $vin.TEX0 : TEX0 : 1 : 1
#var float3 N : $vin.TEX1 : TEX1 : 2 : 1
#var float4 color : $vout.COL : COL : 3 : 1
#var samplerCUBE environmentMap :  : texunit 0 : 4 : 1
#const c[0] = 2 1.3 1 1.69
#const c[1] = 0
PARAM c[2] = { { 2, 1.3, 1, 1.6899998 },
		{ 0 } };
TEMP R0;
TEMP R1;
TEMP R2;
DP3 R0.y, fragment.texcoord[1], fragment.texcoord[1];
RSQ R0.y, R0.y;
DP3 R0.x, fragment.texcoord[0], fragment.texcoord[0];
MUL R2.xyz, R0.y, fragment.texcoord[1];
RSQ R0.x, R0.x;
MUL R0.xyz, R0.x, fragment.texcoord[0];
DP3 R1.x, -R0, R2;
MAD R0.w, -R1.x, R1.x, c[0].z;
MAD R0.w, -R0, c[0], c[0].z;
ABS R1.y, R0.w;
RSQ R1.y, R1.y;
RCP R1.y, R1.y;
MAD R1.x, R1, c[0].y, -R1.y;
MUL R1.xyz, R1.x, R2;
DP3 R1.w, R2, R0;
MUL R2.xyz, R2, R1.w;
MAD R1.xyz, R0, c[0].y, R1;
SLT R0.w, c[1].x, R0;
MUL R1.xyz, R1, R0.w;
MUL R2.xyz, R2, c[0].x;
ADD R0.xyz, R0, -R2;
TEX R1, R1, texture[0], CUBE;
TEX R0, R0, texture[0], CUBE;
ADD R0, R0, -R1;
MAD result.color, fragment.color.primary.x, R0, R1;
END
# 25 instructions, 3 R-regs
