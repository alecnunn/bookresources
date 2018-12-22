const char* cg_str_sum_forces=
"void main(float2 coord : TEXCOORD0,\n"\
"		  uniform samplerRECT rect : TEXUNIT0,\n"\
"		  uniform samplerRECT attr_rect : TEXUNIT1,\n"\
"		  uniform int len,\n"\
"		  out float3 result : COLOR)\n"\
"{\n"\
"	result = float3(0, 0, 0);\n"\
"	float2 dp = f2texRECT(attr_rect, float2(coord.y, 4));\n"\
"	for (int i = 1; i < len; i++)\n"\
"		result += f3texRECT(rect, coord + float2(i, 0));\n"\
"	result *= dp.x;///= dp.x;\n"\
"	result += f3texRECT(rect, coord);\n"\
"}\n"\
"";
