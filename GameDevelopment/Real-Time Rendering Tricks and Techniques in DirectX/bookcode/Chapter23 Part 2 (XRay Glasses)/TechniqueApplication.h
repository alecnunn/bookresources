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
	HRESULT CreateShaders();
	HRESULT ExtractBuffers();
	void SetupDevice();
	HRESULT LoadMesh();
	CTechniqueApplication();
	virtual ~CTechniqueApplication();

	virtual BOOL PostInitialize();
	virtual BOOL PreTerminate();
	virtual BOOL PreReset();
	virtual BOOL PostReset();
	virtual void Render();

	LPD3DXMESH              m_pBox;
	LPDIRECT3DVERTEXBUFFER8 m_pBoxVertexBuffer;
	LPDIRECT3DINDEXBUFFER8  m_pBoxIndexBuffer;

	LPD3DXMESH              m_pRing;
	LPDIRECT3DVERTEXBUFFER8 m_pRingVertexBuffer;
	LPDIRECT3DINDEXBUFFER8  m_pRingIndexBuffer;

	D3DMATERIAL8 *m_pRingMaterials;
	DWORD         m_NumRingMaterials;

	D3DMATERIAL8 *m_pBoxMaterials;
	DWORD         m_NumBoxMaterials;

	DWORD m_XRayShader;
	DWORD m_LightingShader;
};
