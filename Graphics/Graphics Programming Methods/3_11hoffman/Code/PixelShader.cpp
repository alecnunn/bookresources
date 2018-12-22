#include "stdafx.h"
#include "Shader.h"
#include "PixelShader.h"
#include "MyDebug.h"

extern LPDIRECT3DDEVICE8 g_Device;

void PixelShader::CreatePixelShaderFromFile(char* filename)
{
	// Create pixel shader
	LPD3DXBUFFER shadercode=NULL;
	Shader::PreprocessAndAssembleShaderFromFile2(filename, NULL,	&shadercode);

	HRESULT result=g_Device->CreatePixelShader(	(DWORD*)shadercode->GetBufferPointer(), 
		&m_PixelShader);

	FAILMSG("Failed to create pixel shader");
	SAFE_RELEASE(shadercode);
}

void PixelShader::CreatePixelShaderFromBuffer(char *string)
{
	// Create pixel shader
	LPD3DXBUFFER shadercode=NULL;
	Shader::PreprocessAndAssembleShaderFromBuffer(string, strlen(string), NULL,	&shadercode);

	HRESULT result=g_Device->CreatePixelShader(	(DWORD*)shadercode->GetBufferPointer(), 
		&m_PixelShader);

	FAILMSG("Failed to create pixel shader");
	SAFE_RELEASE(shadercode);
}
