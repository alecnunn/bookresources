/***************************************************************
* BasicVertexShaderApplication.h                               *
*                                                              *
* This file contains the header information for the class      *
* derived from CHostApplication.  This new class will          *
* demonstrate the basics of vertex shaders.  For this file to  *
* compile correctly, the path for the DX include files must be *
* added to your path.                                          *
*                                                              *
* Changes:                                                     *
*	*  None                                                    *
***************************************************************/
#include "Application.h"

class CBasicVertexShaderApplication : public CHostApplication  
{
public:
	HRESULT CreateShader();
	CBasicVertexShaderApplication();
	virtual ~CBasicVertexShaderApplication();

	BOOL FillVertexBuffer();
	BOOL CreateVertexBuffer();
	void DestroyVertexBuffer();

	virtual BOOL PostInitialize();
	virtual BOOL PreTerminate();
	virtual BOOL PreReset();
	virtual BOOL PostReset();
	virtual void Render();

	LPDIRECT3DVERTEXBUFFER8 m_pVertexBuffer;

	//These matrices are reusable transformation matrices
	D3DXMATRIX m_WorldMatrix;
	D3DXMATRIX m_ViewMatrix;
	D3DXMATRIX m_ProjectionMatrix;

	//Our new shader handle!!
	DWORD m_ShaderHandle;
};
