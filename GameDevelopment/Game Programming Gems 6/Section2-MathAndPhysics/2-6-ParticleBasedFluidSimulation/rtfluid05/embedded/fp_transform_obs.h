const char* fp_str_transform_obs=
"!!FP1.0\n"\
"# cgc version 1.3.0001, build date Aug  4 2004 10:01:10\n"\
"# command line args: -profile fp30\n"\
"# source file: transform_obs.cg\n"\
"#vendor NVIDIA Corporation\n"\
"#version 1.0.02\n"\
"#profile fp30\n"\
"#program main\n"\
"#semantic main.tri_rect : TEXUNIT0\n"\
"#semantic main.mat\n"\
"#var float2 tri_coord : $vin.TEX0 : TEX0 : 0 : 1\n"\
"#var samplerRECT tri_rect : TEXUNIT0 : texunit 0 : 1 : 1\n"\
"#var float4x4 mat :  : c[0], 4 : 2 : 1\n"\
"#var float result : $vout.COL : COL : 3 : 1\n"\
"DECLARE mat$0;\n"\
"DECLARE mat$1;\n"\
"DECLARE mat$2;\n"\
"DECLARE mat$3;\n"\
"MOVR  R0.w, {1}.x;\n"\
"TEX   R0.xyz, f[TEX0], TEX0, RECT;\n"\
"DP4R  o[COLR].x, R0, mat$0;\n"\
"END\n"\
"# 3 instructions, 1 R-regs, 0 H-regs\n"\
"";
