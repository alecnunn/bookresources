const char* fp_str_collision_glass=
"!!FP1.0\n"\
"# cgc version 1.3.0001, build date Aug  4 2004 10:01:10\n"\
"# command line args: -profile fp30\n"\
"# source file: collision_glass.cg\n"\
"#vendor NVIDIA Corporation\n"\
"#version 1.0.02\n"\
"#profile fp30\n"\
"#program main\n"\
"#semantic main.attr_rect : TEXUNIT0\n"\
"#semantic main.mat_col\n"\
"#semantic main.mat_inv_col\n"\
"#var float pindex : $vin.TEX0 : TEX0 : 0 : 1\n"\
"#var samplerRECT attr_rect : TEXUNIT0 : texunit 0 : 1 : 1\n"\
"#var float4x4 mat_col :  : c[0], 4 : 2 : 1\n"\
"#var float4x4 mat_inv_col :  : c[4], 4 : 3 : 1\n"\
"#var float3 result : $vout.COL : COL : 4 : 1\n"\
"DECLARE mat_col$0;\n"\
"DECLARE mat_col$1;\n"\
"DECLARE mat_col$2;\n"\
"DECLARE mat_col$3;\n"\
"DECLARE mat_inv_col$0;\n"\
"DECLARE mat_inv_col$1;\n"\
"DECLARE mat_inv_col$2;\n"\
"DECLARE mat_inv_col$3;\n"\
"MOVR  R0.x, f[TEX0];\n"\
"MOVR  R0.y, {1}.x;\n"\
"TEX   R0.xyz, R0, TEX0, RECT;\n"\
"DP3R  R2.y, R0, mat_col$1;\n"\
"DP3R  R2.x, R0, mat_col$0;\n"\
"DP3R  R1.xw, R0, mat_col$2;\n"\
"MOVR  R0.x, f[TEX0];\n"\
"MOVR  R0.y, {0}.x;\n"\
"TEX   R0.xyz, R0, TEX0, RECT;\n"\
"MOVR  R0.w, {1}.x;\n"\
"DP4R  R1.z, R0, mat_col$1;\n"\
"DP4R  R1.y, R0, mat_col$0;\n"\
"MOVR  R3.x, R1.y;\n"\
"MOVR  R3.y, R1.z;\n"\
"MOVR  R3.z, {0}.x;\n"\
"DP3R  R2.z, R3, R3;\n"\
"RSQR  R2.z, R2.z;\n"\
"MULR  R3.xyz, R2.z, R3;\n"\
"MOVR  R2.w, R1.z;\n"\
"MOVR  R2.z, R1.y;\n"\
"MULR  R2.zw, R2, R2;\n"\
"ADDR  R1.y, R2.z, R2.w;\n"\
"MOVR  R2.z, R1.x;\n"\
"DP3R  R1.x, R2, -R3;\n"\
"RSQR  R1.y, R1.y;\n"\
"RCPR  R1.y, R1.y;\n"\
"MULR  R1.x, -R1, {64};\n"\
"ADDR  R1.y, R1, {-0.042}.x;\n"\
"MADR  R1.x, R1.y, {25000}, R1;\n"\
"SGTRC HC.x, R1.y, {1e-006};\n"\
"MAXR  R2.w, R1.x, {0}.x;\n"\
"MOVR  R1.xyz, {0}.x;\n"\
"MULR  R1.xyz(GT.x), R2.w, -R3;\n"\
"DP4R  R0.w, R0, mat_col$2;\n"\
"MULR  R2.w, -R1, {64}.x;\n"\
"ADDR  R1.w, -R0, {0.008}.x;\n"\
"MOVR  R0.xyz, R1;\n"\
"MADR  R2.w, R1, {25000}.x, R2;\n"\
"SGTRC HC.x, R1.w, {1e-006};\n"\
"MAXR  R2.w, R2, {0}.x;\n"\
"MADR  R0.xyz(GT.x), R2.w, {0, 1}.xxyw, R1;\n"\
"ADDR  R0.w, R0, {-0.182}.x;\n"\
"DP3R  R1.w, R2, {0, -64}.xxyw;\n"\
"MOVR  R1.xyz, R0;\n"\
"MADR  R1.w, R0, {25000}.x, -R1;\n"\
"MAXR  R1.w, R1, {0}.x;\n"\
"SGTRC HC.x, R0.w, {1e-006};\n"\
"MADR  R1.xyz(GT.x), R1.w, {0, -1}.xxyw, R0;\n"\
"DP3R  o[COLR].z, R1, mat_inv_col$2;\n"\
"DP3R  o[COLR].y, R1, mat_inv_col$1;\n"\
"DP3R  o[COLR].x, R1, mat_inv_col$0;\n"\
"END\n"\
"# 51 instructions, 4 R-regs, 0 H-regs\n"\
"";
