const char* vp_str_water_vertex=
"!!ARBvp1.0\n"\
"# cgc version 1.3.0001, build date Aug  4 2004 10:01:10\n"\
"# command line args: -profile arbvp1\n"\
"# source file: water_vertex.cg\n"\
"#vendor NVIDIA Corporation\n"\
"#version 1.0.02\n"\
"#profile arbvp1\n"\
"#program main\n"\
"#semantic main.fresnelValues\n"\
"#semantic main.texMatrix : STATE.MATRIX.TEXTURE\n"\
"#semantic main.modelViewProj : STATE.MATRIX.MVP\n"\
"#semantic main.modelToWorld : STATE.MATRIX.MODELVIEW\n"\
"#var float4 position : $vin.POSITION : POSITION : 0 : 1\n"\
"#var float3 normal : $vin.NORMAL : NORMAL : 1 : 1\n"\
"#var float4 oPosition : $vout.HPOS : HPOS : 2 : 1\n"\
"#var float3 incident : $vout.TEX0 : TEX0 : 3 : 1\n"\
"#var float3 n_out : $vout.TEX1 : TEX1 : 4 : 1\n"\
"#var float reflectionFactor : $vout.COL0 : COL0 : 5 : 1\n"\
"#var float3 fresnelValues :  : c[13] : 6 : 1\n"\
"#var float4x4 texMatrix : STATE.MATRIX.TEXTURE : c[0], 4 : 7 : 1\n"\
"#var float4x4 modelViewProj : STATE.MATRIX.MVP : c[4], 4 : 8 : 1\n"\
"#var float4x4 modelToWorld : STATE.MATRIX.MODELVIEW : c[8], 4 : 9 : 1\n"\
"#const c[12] = 1\n"\
"PARAM c[14] = { state.matrix.texture,\n"\
"		state.matrix.mvp,\n"\
"		state.matrix.modelview,\n"\
"		{ 1 },\n"\
"		program.local[13] };\n"\
"TEMP R0;\n"\
"TEMP R1;\n"\
"TEMP R2;\n"\
"DP4 result.position.w, vertex.position, c[7];\n"\
"DP4 result.position.z, vertex.position, c[6];\n"\
"DP4 result.position.y, vertex.position, c[5];\n"\
"DP4 result.position.x, vertex.position, c[4];\n"\
"MOV R1.w, c[13].x;\n"\
"DP3 R0.z, vertex.normal, c[10];\n"\
"DP3 R0.y, vertex.normal, c[9];\n"\
"DP4 R1.z, vertex.position, c[10];\n"\
"DP4 R1.y, vertex.position, c[9];\n"\
"DP4 R1.x, vertex.position, c[8];\n"\
"DP3 R0.x, vertex.normal, c[8];\n"\
"DP3 result.texcoord[0].z, R1, c[2];\n"\
"DP3 result.texcoord[0].y, R1, c[1];\n"\
"DP3 R2.x, R1, R1;\n"\
"DP3 R0.w, R0, R0;\n"\
"RSQ R2.x, R2.x;\n"\
"DP3 result.texcoord[0].x, R1, c[0];\n"\
"RSQ R0.w, R0.w;\n"\
"MUL R1.xyz, R2.x, R1;\n"\
"MUL R2.xyz, R0.w, R0;\n"\
"DP3 R0.w, R1, R2;\n"\
"DP3 result.texcoord[1].z, R0, c[2];\n"\
"ADD R1.xy, R0.w, c[12].x;\n"\
"DP3 result.texcoord[1].y, R0, c[1];\n"\
"LIT R1.z, R1;\n"\
"DP3 result.texcoord[1].x, R0, c[0];\n"\
"MAD result.color.x, R1.z, c[13].y, c[13].z;\n"\
"END\n"\
"# 27 instructions, 3 R-regs\n"\
"";
