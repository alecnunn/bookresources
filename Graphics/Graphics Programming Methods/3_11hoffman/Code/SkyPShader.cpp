#include "stdafx.h"

#include "SkyPShader.h"
#include "MyDebug.h"
#include "App.h"

extern LPDIRECT3DDEVICE8 g_Device;

char StrSkyPS10[] = {
#line 1 "SkyPS10.psh"
"ps.1.0 \n\
mov r0, v1 \n\
"
};


SkyPShader::SkyPShader()
{

	// Create pixel shader
#ifdef _PREPROCESS
	CreatePixelShaderFromFile("SkyPS10.psh");
#else
	CreatePixelShaderFromFile("SkyPS10a.psh");
#endif

//	CreatePixelShaderFromBuffer(StrSkyPS10);

	OutputDebugString("Compiled pixel shader\n");
}


SkyPShader::~SkyPShader()
{
}


void SkyPShader::Apply()
{

	// set pixel shader
	g_Device->SetPixelShader(m_PixelShader);
	SetupPixelShaderConstants();
}

void SkyPShader::SetupPixelShaderConstants()
{
}
