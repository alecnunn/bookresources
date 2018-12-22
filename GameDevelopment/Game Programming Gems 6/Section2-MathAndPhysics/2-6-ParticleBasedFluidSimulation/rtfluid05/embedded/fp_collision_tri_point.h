const char* fp_str_collision_tri_point=
"!!FP1.0\n"\
"# cgc version 1.3.0001, build date Aug  4 2004 10:01:10\n"\
"# command line args: -profile fp30\n"\
"# source file: collision_tri_point.cg\n"\
"#vendor NVIDIA Corporation\n"\
"#version 1.0.02\n"\
"#profile fp30\n"\
"#program main\n"\
"#semantic main.attr_rect : TEXUNIT0\n"\
"#semantic main.tri_rect : TEXUNIT1\n"\
"#var float2 par_tri_coord : $vin.TEX0 : TEX0 : 0 : 1\n"\
"#var samplerRECT attr_rect : TEXUNIT0 : texunit 0 : 1 : 1\n"\
"#var samplerRECT tri_rect : TEXUNIT1 : texunit 1 : 2 : 1\n"\
"#var float3 result : $vout.COL : COL : 3 : 1\n"\
"FLRR  R0.x, f[TEX0].y;\n"\
"MULR  R3.x, R0, {8};\n"\
"MOVR  R3.y, {0}.x;\n"\
"ADDR  R0.xy, R3, {3, 0};\n"\
"TEX   R0.xyz, R0, TEX1, RECT;\n"\
"MOVR  R1.x, f[TEX0];\n"\
"MOVR  R1.y, {0}.x;\n"\
"TEX   R1.xyz, R1, TEX0, RECT;\n"\
"ADDR  R2.xyz, R1, -R0;\n"\
"ADDR  R0.xy, R3, {1, 0};\n"\
"TEX   R0.xyz, R0, TEX1, RECT;\n"\
"DP3R  R0.w, R0, R2;\n"\
"TEX   R0.xyz, R3, TEX1, RECT;\n"\
"DP3R  R0.x, R0, R2;\n"\
"ADDR  R0.y, R0.x, R0.w;\n"\
"SGTR  H0.y, {0}.x, R0.w;\n"\
"SGTR  H0.x, {0}, R0;\n"\
"ADDX_SAT H0.x, H0, H0.y;\n"\
"SGTR  H0.z, R0.y, {1}.x;\n"\
"ADDXC_SAT HC.x, H0, H0.z;\n"\
"ADDR  R0.xy, R3, {2, 0};\n"\
"TEX   R0.xyz, R0, TEX1, RECT;\n"\
"MOVR  R0.w, {0.016}.x;\n"\
"DP3R  R0.w(LE.x), R0, R2;\n"\
"ADDR  R2.xy, R3, {7, 0};\n"\
"TEX   R2.xyz, R2, TEX1, RECT;\n"\
"ADDR  R2.xyz, R1, -R2;\n"\
"ADDR  R1.xy, R3, {4, 0};\n"\
"TEX   R1.xyz, R1, TEX1, RECT;\n"\
"DP3R  R1.w, R1, R2;\n"\
"ADDR  R1.xy, R3, {5, 0};\n"\
"TEX   R1.xyz, R1, TEX1, RECT;\n"\
"DP3R  R1.x, R1, R2;\n"\
"ADDR  R1.y, R1.w, R1.x;\n"\
"SGTR  H0.x, {0}, R1.w;\n"\
"SGTR  H0.y, {0}.x, R1.x;\n"\
"ADDX_SAT H0.x, H0, H0.y;\n"\
"SGTR  H0.y, R1, {1}.x;\n"\
"ADDXC_SAT HC.x, H0, H0.y;\n"\
"ADDR  R1.xy, R3, {6, 0};\n"\
"TEX   R1.xyz, R1, TEX1, RECT;\n"\
"MOVR  R1.w, {0.016}.x;\n"\
"DP3R  R1.w(LE.x), R1, R2;\n"\
"SLTR  H0.x, R0.w, R1.w;\n"\
"MOVRC RC.x, H0;\n"\
"ADDR  R0.w, -R0, {0.008}.x;\n"\
"ADDR  R0.w(EQ.x), -R1, {0.008}.x;\n"\
"MOVRC RC.x, H0;\n"\
"MOVR  R0.xyz(EQ.x), R1;\n"\
"MOVR  R1.x, f[TEX0];\n"\
"MOVR  R1.y, {1}.x;\n"\
"TEX   R1.xyz, R1, TEX0, RECT;\n"\
"DP3R  R1.x, R1, R0;\n"\
"MULR  R1.x, -R1, {64};\n"\
"MADR  R1.x, R0.w, {30000}, R1;\n"\
"MOVR  o[COLR].xyz, {0}.x;\n"\
"SGTRC HC.x, R0.w, {1e-006};\n"\
"MULR  o[COLR].xyz(NE.x), R1.x, R0;\n"\
"END\n"\
"# 58 instructions, 4 R-regs, 1 H-regs\n"\
"";
