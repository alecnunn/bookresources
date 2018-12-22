/***************************************************************
* TextureStateApplication.h                                    *
*                                                              *
* This file contains the header information for the class      *
* derived from CHostApplication.  This new class will          *
* demonstrate the basics of texture filtering and texture      *
* coordinate texture states..  For this file to                *
* compile correctly, the path for the DX include files must be *
* added to your path.                                          *
*                                                              *
* Changes:                                                     *
*	*  None                                                    *
***************************************************************/
#include "Application.h"

class CTextureStateApplication : public CHostApplication  
{
public:
	void VerifyModes();
	void SetupDevice();
	BOOL CreateGeometry();
	void DestroyGeometry();
	
	CTextureStateApplication();
	virtual ~CTextureStateApplication();

	BOOL HandleMessage(MSG *pMessage);

	virtual BOOL PostInitialize();
	virtual BOOL PreTerminate();
	virtual BOOL PreReset();
	virtual BOOL PostReset();
	virtual void Render();

	//Override PreRender so that we can change the clear color
	virtual void PreRender();

	LPDIRECT3DVERTEXBUFFER8 m_pVertexBuffer;

	long m_CurrentFilterMode;
	long m_CurrentAddressMode;

	//Our texture object
	LPDIRECT3DTEXTURE8 m_pCheckerTexture;

	//These matrices are reusable transformation matrices
	D3DXMATRIX m_WorldMatrix;
	D3DXMATRIX m_ViewMatrix;
	D3DXMATRIX m_ProjectionMatrix;
};
