#include "stdafx.h"

#include "Shader.h"
#include "VertexShader.h"
#include "MyDebug.h" 

extern LPDIRECT3DDEVICE8 g_Device;


void VertexShader::CreateVertexShader(char* filename, DWORD* vertexShaderDeclaration)
{
	// Create vertex VertexShader
	LPD3DXBUFFER VertexShadercode=NULL;

	Shader::PreprocessAndAssembleShaderFromFile2(filename,	NULL, &VertexShadercode);

	HRESULT result=g_Device->CreateVertexShader(vertexShaderDeclaration,
		(DWORD*)VertexShadercode->GetBufferPointer(),	&m_VertexShader, 0);

	FAILMSG("Failed to create vertex VertexShader");
	SAFE_RELEASE(VertexShadercode);
}

