// shader
// (c) Kenny Mitchell - Westwood Studios EA 2002
#include "stdafx.h"


#include "Shader.h"
#include "MyDebug.h"

void Shader::PreprocessAndAssembleShaderFromBuffer(
	LPCVOID pSrcData,
  UINT SrcDataLen,
  LPD3DXBUFFER* ppConstants,
  LPD3DXBUFFER* ppCode)
{
	LPD3DXBUFFER* ppErrors=NULL;
    HRESULT result=D3DXAssembleShader(pSrcData, SrcDataLen, 0,  ppConstants, ppCode, ppErrors);
	FAILMSG("Failed to assemble VertexShader from file");
}


void Shader::PreprocessAndAssembleShaderFromFile2(
        LPCSTR          szFile, 
        LPD3DXBUFFER*   ppConstants, 
        LPD3DXBUFFER*   ppCode)
{
	LPD3DXBUFFER* ppErrors=NULL;

#ifdef _PREPROCESS
    char szPath[_MAX_PATH];
    char szTemp[_MAX_PATH];
    char szCmd [_MAX_PATH];

    GetTempPath(sizeof(szPath), szPath);
    GetTempFileName(szPath, "acl", 0, szTemp);

     _snprintf(szCmd, sizeof(szCmd), "cl /nologo /E %s > %s", szFile, szTemp);

    if(0 != system(szCmd))
       	FAILMSG("Failed to execute preprocessor on VertexShader (check Visual C++'s cl.exe is in path)");

    HRESULT result=D3DXAssembleShaderFromFile(szTemp, 0, ppConstants, ppCode, ppErrors);

#else
    HRESULT result=D3DXAssembleShaderFromFile(szFile, 0, ppConstants, ppCode, ppErrors);
#endif

	FAILMSG("Failed to assemble VertexShader from file");

}


