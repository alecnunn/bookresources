// GPM - Pixel Shader Optimizations for Terrain Rendering Demo
// Kenny Mitchell
// Copyright Electronic Arts 2002, 2003

#ifndef SHADER_H
#define SHADER_H

#ifndef DEBUG_H
#include "debug.h"
#endif

class Shader_Class
{
public:
	Shader_Class();
	virtual ~Shader_Class();

	virtual void Apply()=0;
	virtual void Update_Instance()=0;
	virtual int Get_Vertex_Stride() const =0;
	virtual DWORD Get_Vertex_Format() const =0;

protected:
	
	void Create_Vertex_Shader
	(
		char* file_name
	);

	void Preprocess_And_Assemble_Shader_From_File
	(
		char*				file_name,
		LPD3DXBUFFER*	constants,
		LPD3DXBUFFER*	shader_code
	);

	LPDIRECT3DVERTEXSHADER9			Vertex_Shader;
	LPDIRECT3DVERTEXDECLARATION9	Vertex_Declaration;
};

#endif