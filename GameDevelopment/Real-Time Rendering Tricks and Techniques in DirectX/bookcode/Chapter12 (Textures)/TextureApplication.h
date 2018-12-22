/***************************************************************
* TextureApplication.h                                        *
*                                                              *
* This file contains the header information for the class      *
* derived from CHostApplication.  This new class will          *
* demonstrate the basics of texturing.  For this file to       *
* compile correctly, the path for the DX include files must be *
* added to your path.                                          *
*                                                              *
* Changes:                                                     *
*	*  None                                                    *
***************************************************************/
#include "Application.h"

class CTextureApplication : public CHostApplication  
{
public:
	BOOL CreateCustomTextures();
	void SetupDevice();
	BOOL CreateGeometry();
	void DestroyGeometry();
	
	CTextureApplication();
	virtual ~CTextureApplication();

	BOOL FillVertexBuffer();

	virtual BOOL PostInitialize();
	virtual BOOL PreTerminate();
	virtual BOOL PreReset();
	virtual BOOL PostReset();
	virtual void Render();

	//Override PreRender so that we can change the clear color
	virtual void PreRender();

	LPDIRECT3DVERTEXBUFFER8 m_pVertexBuffer;

	//Our three texture objects
	LPDIRECT3DTEXTURE8 m_pImageTexture;
	LPDIRECT3DTEXTURE8 m_pMipMapTexture;
	LPDIRECT3DTEXTURE8 m_pCheckerTexture;

	//These matrices are reusable transformation matrices
	D3DXMATRIX m_WorldMatrix;
	D3DXMATRIX m_ViewMatrix;
	D3DXMATRIX m_ProjectionMatrix;
};
