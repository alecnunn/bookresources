!!FP1.0
# cgc version 1.3.0001, build date Aug  4 2004 10:01:10
# command line args: -profile fp30
# source file: passthru.cg
#vendor NVIDIA Corporation
#version 1.0.02
#profile fp30
#program main
#semantic main.tex
#semantic main.scale
#semantic main.bias
#var float2 uv : $vin.TEX0 : TEX0 : 0 : 1
#var samplerRECT tex :  : texunit 0 : 1 : 1
#var float4 scale :  :  : 2 : 1
#var float4 bias :  :  : 3 : 1
#var float4 main : $vout.COL : COL : -1 : 1
DECLARE scale;
DECLARE bias;
TEX   R0, f[TEX0], TEX0, RECT;
MULR  R0, R0, scale;
ADDR  o[COLR], R0, bias;
END
# 3 instructions, 1 R-regs, 0 H-regs
