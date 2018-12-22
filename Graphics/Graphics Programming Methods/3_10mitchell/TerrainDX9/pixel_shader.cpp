// GPM - Pixel Shader Optimizations for Terrain Rendering Demo
// Kenny Mitchell
// Copyright Electronic Arts 2002, 2003

#include "pixel_shader.h"

Pixel_Shader_Class::Pixel_Shader_Class()
: Pixel_Shader(0)
{
}

Pixel_Shader_Class::~Pixel_Shader_Class()
{
	Safe_Release(Pixel_Shader);
}

void Pixel_Shader_Class::Create_Pixel_Shader(char* file_name)
{
	// Create pixel shader
	LPD3DXBUFFER shader_code=NULL;

	Preprocess_And_Assemble_Shader_From_File
	(
		file_name,
		NULL,
		&shader_code
	);

	HRESULT result=DEV->CreatePixelShader
	(
		(DWORD*)shader_code->GetBufferPointer(), 
		&Pixel_Shader
	);
	FAILMSG("Failed to create pixel shader");
	Safe_Release(shader_code);
}

