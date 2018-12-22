const char* fp_str_compute_acc_vis=
"!!FP1.0\n"\
"# cgc version 1.3.0001, build date Aug  4 2004 10:01:10\n"\
"# command line args: -profile fp30\n"\
"# source file: compute_acc_vis.cg\n"\
"# source file: ./kernel.cg\n"\
"# source file: ./sph.cg\n"\
"#vendor NVIDIA Corporation\n"\
"#version 1.0.02\n"\
"#profile fp30\n"\
"#program main\n"\
"#semantic main.attr_rect : TEXUNIT0\n"\
"#semantic main.neighbour_rect : TEXUNIT1\n"\
"#semantic PI\n"\
"#semantic MASS\n"\
"#semantic SMOOTHING_LENGTH\n"\
"#semantic VISCOSITY\n"\
"#var fixed2 sn_coord : $vin.TEX0 : TEX0 : 0 : 1\n"\
"#var samplerRECT attr_rect : TEXUNIT0 : texunit 0 : 1 : 1\n"\
"#var samplerRECT neighbour_rect : TEXUNIT1 : texunit 1 : 2 : 1\n"\
"#var float3 result : $vout.COL : COL : 3 : 1\n"\
"#var float PI :  :  : -1 : 0\n"\
"#var float MASS :  :  : -1 : 0\n"\
"#var float SMOOTHING_LENGTH :  :  : -1 : 0\n"\
"#var float VISCOSITY :  :  : -1 : 0\n"\
"#default PI = 3.141593\n"\
"#default MASS = 0.00020543\n"\
"#default SMOOTHING_LENGTH = 0.01\n"\
"#default VISCOSITY = 0.1\n"\
"MOVR  R0.x, f[TEX0].y;\n"\
"MOVR  R0.y, {0}.x;\n"\
"TEX   R1.xyz, R0, TEX0, RECT;\n"\
"TEX   R2.x, f[TEX0], TEX1, RECT;\n"\
"MOVR  R0.y, {0}.x;\n"\
"MOVR  R0.x, R2;\n"\
"TEX   R3.xyz, R0, TEX0, RECT;\n"\
"ADDR  R0.xyz, R1, -R3;\n"\
"ADDR  R1.xyz, R3, -R1;\n"\
"DP3R  R0.w, R1, R1;\n"\
"RSQR  R0.w, R0.w;\n"\
"RCPR  R0.w, R0.w;\n"\
"ADDR  R1.z, -R0.w, {0.01}.x;\n"\
"MOVR  R1.x, f[TEX0].y;\n"\
"MOVR  R1.y, {4}.x;\n"\
"TEX   R2.y, R1, TEX0, RECT;\n"\
"MOVR  R1.x, R2;\n"\
"MOVR  R1.y, {4}.x;\n"\
"TEX   R1.xy, R1, TEX0, RECT;\n"\
"ADDR  R1.y, R2, R1;\n"\
"MULR  R1.y, R1, R1.z;\n"\
"MULR  R1.y, R1, R1.z;\n"\
"MOVR  R2.y, {1}.x;\n"\
"MULR  R0.xyz, R1.y, R0;\n"\
"TEX   R2.xyz, R2, TEX0, RECT;\n"\
"MOVR  R3.x, f[TEX0].y;\n"\
"MOVR  R3.y, {1}.x;\n"\
"TEX   R3.xyz, R3, TEX0, RECT;\n"\
"ADDR  R2.xyz, R2, -R3;\n"\
"MULR  R2.xyz, R2, {0.3}.x;\n"\
"MADR  R0.xyz, R0, {0.5}.x, R2;\n"\
"MULR  R0.xyz, R1.z, R0;\n"\
"MULR  R0.xyz, R0, R1.x;\n"\
"MOVR  o[COLR].xyz, {0}.x;\n"\
"SGTRC HC.x, -R0.w, {-0.01};\n"\
"MULR  o[COLR].xyz(NE.x), R0, {9.80856e+008}.x;\n"\
"END\n"\
"# 36 instructions, 5 R-regs, 0 H-regs\n"\
"";
