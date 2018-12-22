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
	HRESULT CreateShader();
	HRESULT CreateBuffer();
	void SetupDevice();
	HRESULT LoadMesh();
	CTechniqueApplication();
	virtual ~CTechniqueApplication();

	virtual BOOL PostInitialize();
	virtual BOOL PreTerminate();
	virtual BOOL PreReset();
	virtual BOOL PostReset();
	virtual void Render();

	LPD3DXMESH    m_pMesh;
	D3DMATERIAL8 *m_pMeshMaterials;
	DWORD         m_NumMaterials;

	LPDIRECT3DVERTEXBUFFER8 m_pVertexBuffer;
	LPDIRECT3DTEXTURE8      m_pLightTexture;

	DWORD m_BillboardShaderHandle;
};
