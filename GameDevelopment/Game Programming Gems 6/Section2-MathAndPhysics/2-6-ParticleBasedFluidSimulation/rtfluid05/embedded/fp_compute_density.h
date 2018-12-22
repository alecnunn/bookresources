const char* fp_str_compute_density=
"!!FP1.0\n"\
"# cgc version 1.3.0001, build date Aug  4 2004 10:01:10\n"\
"# command line args: -profile fp30\n"\
"# source file: compute_density.cg\n"\
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
"#var float result : $vout.COL : COL : 3 : 1\n"\
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
"TEX   R0.xyz, R0, TEX0, RECT;\n"\
"TEX   R1.x, f[TEX0], TEX1, RECT;\n"\
"MOVR  R1.y, {0}.x;\n"\
"TEX   R1.xyz, R1, TEX0, RECT;\n"\
"ADDR  R0.xyz, R0, -R1;\n"\
"DP3R  R0.x, R0, R0;\n"\
"ADDR  R0.y, -R0.x, {0.0001}.x;\n"\
"MULR  R0.z, R0.y, R0.y;\n"\
"MULR  R0.y, R0, R0.z;\n"\
"MOVR  o[COLR].x, {0};\n"\
"SGTRC HC.x, -R0, {-0.0001};\n"\
"MULR  o[COLR].x(NE), R0.y, {3.21843e+014};\n"\
"END\n"\
"# 14 instructions, 2 R-regs, 0 H-regs\n"\
"";
