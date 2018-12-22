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
	void GetBetterTransformation(D3DXVECTOR3 *pPosition, D3DXVECTOR3 *pDirection,
		                         D3DXMATRIX *pMatrix);
	float GetHeightAt(float X, float Z, D3DXMATRIX* pRotation);
	HRESULT CreateShaders();
	HRESULT ExtractBuffers();
	void SetupDevice();
	HRESULT LoadMeshes();
	CTechniqueApplication();
	virtual ~CTechniqueApplication();

	virtual BOOL PostInitialize();
	virtual BOOL PreTerminate();
	virtual BOOL PreReset();
	virtual BOOL PostReset();
	virtual void Render();
	virtual void PreRender();

	LPD3DXMESH              m_pTerrainMesh;
	LPDIRECT3DVERTEXBUFFER8 m_pTerrainVertexBuffer;
	LPDIRECT3DINDEXBUFFER8  m_pTerrainIndexBuffer;

	LPD3DXMESH              m_pCubeMesh;
	LPDIRECT3DVERTEXBUFFER8 m_pCubeVertexBuffer;
	LPDIRECT3DINDEXBUFFER8  m_pCubeIndexBuffer;

	DWORD m_BasicShader;
};
