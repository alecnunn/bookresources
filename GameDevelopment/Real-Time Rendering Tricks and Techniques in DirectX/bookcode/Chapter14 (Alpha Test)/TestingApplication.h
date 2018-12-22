/***************************************************************
* TestingApplication.h                                         *
*                                                              *
* This file contains the header information for the class      *
* derived from CHostApplication.  This new class will          *
* demonstrate the basics of alpha and depth testing and alpha  *
* blending.  For this file to                                  *
* compile correctly, the path for the DX include files must be *
* added to your path.                                          *
*                                                              *
* Changes:                                                     *
*	*  None                                                    *
***************************************************************/
#include "Application.h"

class CTestingApplication : public CHostApplication  
{
public:
	void SetupDevice();
	BOOL CreateGeometry();
	void DestroyGeometry();
	
	CTestingApplication();
	virtual ~CTestingApplication();

	virtual BOOL PostInitialize();
	virtual BOOL PreTerminate();
	virtual BOOL PreReset();
	virtual BOOL PostReset();
	virtual void Render();

	//Override PreRender so that we can change the clear color
	virtual void PreRender();

	LPDIRECT3DVERTEXBUFFER8 m_pVertexBuffer;

	//Our two texture objects
	LPDIRECT3DTEXTURE8 m_pFrontTexture;
	LPDIRECT3DTEXTURE8 m_pBackTexture;

	//These matrices are reusable transformation matrices
	D3DXMATRIX m_WorldMatrix;
	D3DXMATRIX m_ViewMatrix;
	D3DXMATRIX m_ProjectionMatrix;
};
