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
	long GetPickObject(long X, long Y);
	void CleanUpTarget();
	HRESULT CreateShaders();
	HRESULT ExtractBuffers();
	HRESULT SetupDevice();
	HRESULT LoadMesh();
	CTechniqueApplication();
	virtual ~CTechniqueApplication();

	virtual BOOL HandleMessage(MSG *pMessage);

	virtual BOOL PostInitialize();
	virtual BOOL PreTerminate();
	virtual BOOL PreReset();
	virtual BOOL PostReset();
	virtual void Render();

	LPD3DXMESH              m_pMesh;
	LPDIRECT3DVERTEXBUFFER8 m_pMeshVertexBuffer;
	LPDIRECT3DINDEXBUFFER8  m_pMeshIndexBuffer;

	D3DMATERIAL8 *m_pMeshMaterials;
	DWORD         m_NumMaterials;

	LPDIRECT3DSURFACE8 m_pPickSurface;
	LPDIRECT3DSURFACE8 m_pPickZSurface;
	LPDIRECT3DSURFACE8 m_pBackBuffer;
	LPDIRECT3DSURFACE8 m_pZBuffer;

	DWORD m_BasicShader;
	DWORD m_PickShader;

	long m_CurrentObject;
};
