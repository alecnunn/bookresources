/***************************************************************
* TechniqueApplication.h                                       *
*                                                              *
* This file contains the header information for the class      *
* derived from CHostApplication.  This new class will          *
* serve as a base for all the techniques.  For this file to    *
* compile correctly, the path for the DX include files must be *
* added to your path.                                          *
*                                                              *
* Changes:                                                     *
*	*  None                                                    *
***************************************************************/
#include "Application.h"

class CTechniqueApplication : public CHostApplication  
{
public:
	void CleanUpTarget();
	void RenderPlane();
	HRESULT CreatePlaneBuffers();
	HRESULT CreateShaders();
	HRESULT ExtractBuffers();
	HRESULT SetupDevice();
	HRESULT LoadMesh();
	CTechniqueApplication();
	virtual ~CTechniqueApplication();

	virtual BOOL PostInitialize();
	virtual BOOL PreTerminate();
	virtual BOOL PreReset();
	virtual BOOL PostReset();
	virtual void Render();
	virtual void PreRender();

	LPD3DXMESH              m_pMesh;
	LPDIRECT3DVERTEXBUFFER8 m_pMeshVertexBuffer;
	LPDIRECT3DINDEXBUFFER8  m_pMeshIndexBuffer;

	LPDIRECT3DVERTEXBUFFER8 m_pPlaneVertexBuffer;
	LPDIRECT3DVERTEXBUFFER8 m_pTestVertexBuffer;

	D3DMATERIAL8 *m_pMeshMaterials;
	DWORD         m_NumMaterials;

	LPDIRECT3DTEXTURE8 m_pShadowTexture;
	LPDIRECT3DSURFACE8 m_pShadowTextureSurface;
	LPDIRECT3DSURFACE8 m_pShadowZSurface;
	LPDIRECT3DSURFACE8 m_pBackBuffer;
	LPDIRECT3DSURFACE8 m_pZBuffer;

	DWORD m_CompareShader;
	DWORD m_DistanceShader;
	DWORD m_PixelShader;
};
