// GPM - Pixel Shader Optimizations for Terrain Rendering Demo
// Kenny Mitchell
// Copyright Electronic Arts 2002, 2003

#include "shader.h"

Shader_Class::Shader_Class()
: Vertex_Shader(0)
{
}

Shader_Class::~Shader_Class()
{
	if (Vertex_Shader) DEV->DeleteVertexShader(Vertex_Shader);
}

void Shader_Class::Create_Vertex_Shader
(
	char* file_name, 
	DWORD* vertex_shader_declaration
)
{
	// Create vertex shader
	LPD3DXBUFFER shader_code=NULL;

	Preprocess_And_Assemble_Shader_From_File
	(
		file_name,
		NULL,
		&shader_code
	);

	HRESULT result=DEV->CreateVertexShader
	(
		vertex_shader_declaration,
		(DWORD*)shader_code->GetBufferPointer(),
		&Vertex_Shader,
		0
	);
	FAILMSG("Failed to create vertex shader");
	Safe_Release(shader_code);
}

void Shader_Class::Preprocess_And_Assemble_Shader_From_File
(
	char*				file_name,
	LPD3DXBUFFER*	constants,
	LPD3DXBUFFER*	shader_code
)
{
	char shell_command[_MAX_PATH];
	char temp_path[_MAX_PATH];
	char temp_file[_MAX_PATH];


   GetTempPath(_MAX_PATH, temp_path);
	GetTempFileName(temp_path,"cld",1,temp_file);

	_snprintf
	(
		shell_command, 
		sizeof(shell_command), 
		"rspp %s %s", 
		file_name, 
		temp_file
	);

	int result=system(shell_command);
	FAILMSG("Failed to execute preprocessor on shader (check Visual C++'s cl.exe is in path)");

	LPD3DXBUFFER* errors=NULL;

	result=D3DXAssembleShaderFromFile
	(
		temp_file, 
		0, 
		constants, 
		shader_code, 
		errors
	);
	FAILMSG("Failed to assemble shader from file");
}
