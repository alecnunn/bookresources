// (c) Kenny Mitchell - Westwood Studios EA 2002

#ifndef _SHADER_H
#define _SHADER_H

#include <d3dx8.h>

class Shader
{
public:
	Shader() {};
	virtual ~Shader() {};

	virtual void Apply()=0;
	
	static void Shader::PreprocessAndAssembleShaderFromBuffer(
			LPCVOID pSrcData,
		  UINT SrcDataLen,
		  LPD3DXBUFFER* ppConstants,
		  LPD3DXBUFFER* ppCode);
	static void PreprocessAndAssembleShaderFromFile2(
        LPCSTR          szFile, 
        LPD3DXBUFFER*   ppConstants, 
        LPD3DXBUFFER*   ppCode);
protected:
};

#endif