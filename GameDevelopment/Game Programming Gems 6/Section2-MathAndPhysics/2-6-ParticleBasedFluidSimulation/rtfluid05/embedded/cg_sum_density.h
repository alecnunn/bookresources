const char* cg_str_sum_density=
"void main(fixed2 coord : TEXCOORD0,\n"\
"		  uniform samplerRECT rect : TEXUNIT0,\n"\
"		  uniform int len,\n"\
"		  out float3 result : COLOR)\n"\
"{\n"\
"	float density = 0.0;\n"\
"	for (int i = 0; i < len; i++)\n"\
"		density += f1texRECT(rect, coord + float2(i, 0));\n"\
"	result = float3(1.0/density, 150000.0f*max(density - 1000.0f, 0.0), 0);\n"\
"}\n"\
"";
