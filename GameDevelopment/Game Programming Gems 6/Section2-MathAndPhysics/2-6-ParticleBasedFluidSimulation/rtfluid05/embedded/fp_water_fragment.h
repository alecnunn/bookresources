const char* fp_str_water_fragment=
"!!ARBfp1.0\n"\
"# cgc version 1.3.0001, build date Aug  4 2004 10:01:10\n"\
"# command line args: -profile arbfp1\n"\
"# source file: water_fragment.cg\n"\
"#vendor NVIDIA Corporation\n"\
"#version 1.0.02\n"\
"#profile arbfp1\n"\
"#program main\n"\
"#semantic main.environmentMap\n"\
"#var float reflectionFactor : $vin.COL0 : COL0 : 0 : 1\n"\
"#var float3 incident : $vin.TEX0 : TEX0 : 1 : 1\n"\
"#var float3 N : $vin.TEX1 : TEX1 : 2 : 1\n"\
"#var float4 color : $vout.COL : COL : 3 : 1\n"\
"#var samplerCUBE environmentMap :  : texunit 0 : 4 : 1\n"\
"#const c[0] = 2 1.3 1 1.69\n"\
"#const c[1] = 0\n"\
"PARAM c[2] = { { 2, 1.3, 1, 1.6899998 },\n"\
"		{ 0 } };\n"\
"TEMP R0;\n"\
"TEMP R1;\n"\
"TEMP R2;\n"\
"DP3 R0.y, fragment.texcoord[1], fragment.texcoord[1];\n"\
"RSQ R0.y, R0.y;\n"\
"DP3 R0.x, fragment.texcoord[0], fragment.texcoord[0];\n"\
"MUL R2.xyz, R0.y, fragment.texcoord[1];\n"\
"RSQ R0.x, R0.x;\n"\
"MUL R0.xyz, R0.x, fragment.texcoord[0];\n"\
"DP3 R1.x, -R0, R2;\n"\
"MAD R0.w, -R1.x, R1.x, c[0].z;\n"\
"MAD R0.w, -R0, c[0], c[0].z;\n"\
"ABS R1.y, R0.w;\n"\
"RSQ R1.y, R1.y;\n"\
"RCP R1.y, R1.y;\n"\
"MAD R1.x, R1, c[0].y, -R1.y;\n"\
"MUL R1.xyz, R1.x, R2;\n"\
"DP3 R1.w, R2, R0;\n"\
"MUL R2.xyz, R2, R1.w;\n"\
"MAD R1.xyz, R0, c[0].y, R1;\n"\
"SLT R0.w, c[1].x, R0;\n"\
"MUL R1.xyz, R1, R0.w;\n"\
"MUL R2.xyz, R2, c[0].x;\n"\
"ADD R0.xyz, R0, -R2;\n"\
"TEX R1, R1, texture[0], CUBE;\n"\
"TEX R0, R0, texture[0], CUBE;\n"\
"ADD R0, R0, -R1;\n"\
"MAD result.color, fragment.color.primary.x, R0, R1;\n"\
"END\n"\
"# 25 instructions, 3 R-regs\n"\
"";
