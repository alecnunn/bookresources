const char* fp_str_passthru=
"!!FP1.0\n"\
"# cgc version 1.3.0001, build date Aug  4 2004 10:01:10\n"\
"# command line args: -profile fp30\n"\
"# source file: passthru.cg\n"\
"#vendor NVIDIA Corporation\n"\
"#version 1.0.02\n"\
"#profile fp30\n"\
"#program main\n"\
"#semantic main.tex\n"\
"#semantic main.scale\n"\
"#semantic main.bias\n"\
"#var float2 uv : $vin.TEX0 : TEX0 : 0 : 1\n"\
"#var samplerRECT tex :  : texunit 0 : 1 : 1\n"\
"#var float4 scale :  :  : 2 : 1\n"\
"#var float4 bias :  :  : 3 : 1\n"\
"#var float4 main : $vout.COL : COL : -1 : 1\n"\
"DECLARE scale;\n"\
"DECLARE bias;\n"\
"TEX   R0, f[TEX0], TEX0, RECT;\n"\
"MULR  R0, R0, scale;\n"\
"ADDR  o[COLR], R0, bias;\n"\
"END\n"\
"# 3 instructions, 1 R-regs, 0 H-regs\n"\
"";
