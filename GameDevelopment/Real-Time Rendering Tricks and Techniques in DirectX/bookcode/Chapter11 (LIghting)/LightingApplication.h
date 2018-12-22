/***************************************************************
* LightingApplication.h                                        *
*                                                              *
* This file contains the header information for the class      *
* derived from CHostApplication.  This new class will          *
* demonstrate the basics of Lighting.  For this file to        *
* compile correctly, the path for the DX include files must be *
* added to your path.                                          *
*                                                              *
* Changes:                                                     *
*	*  None                                                    *
***************************************************************/
#include "Application.h"

class CLightingApplication : public CHostApplication  
{
public:
	void RenderLightVisuals();
	void DestroyLightVisuals();
	BOOL InitializeLightVisuals();
	void SetupDevice();
	BOOL LoadMesh();
	void InitializeLights();

	CLightingApplication();
	virtual ~CLightingApplication();

	virtual BOOL PostInitialize();
	virtual BOOL PreTerminate();
	virtual BOOL PreReset();
	virtual BOOL PostReset();
	virtual void Render();
	virtual BOOL HandleMessage(MSG *pMessage);

	LPD3DXMESH    m_pMesh;
	D3DMATERIAL8 *m_pMeshMaterials;
	DWORD         m_NumMaterials;
	DWORD         m_CurrentSubset;

	D3DLIGHT8 m_Light[3];

	DWORD m_CurrentLight;

	LPDIRECT3DVERTEXBUFFER8 m_pLightsBuffer;

	//These matrices are reusable transformation matrices
	D3DXMATRIX m_WorldMatrix;
	D3DXMATRIX m_ViewMatrix;
	D3DXMATRIX m_ProjectionMatrix;
};
