/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/
#ifndef __NVMESH_H
#define __NVMESH_H

#include "nvinc.h"
#include <nvdevice.h>

namespace nv_objects
{

class NVBounds
{
public:
	Transform(const D3DXMATRIX* pMatrix);
	D3DXVECTOR3 m_vecCenter;
	D3DXVECTOR3 m_vecMinExtents;
	D3DXVECTOR3 m_vecMaxExtents;
	float m_fRadius;
};

// Base frame
class NVMesh
{
public:
	NVMesh(TCHAR* strName = _T("NVMesh") );
	~NVMesh();

	virtual HRESULT InitMaterials( NVDevice* pd3dDevice, LPD3DXBUFFER pMtrlBuffer );

	// Rendering
    virtual HRESULT Render( NVDevice* pd3dDevice, 
		            BOOL bDrawOpaqueSubsets = TRUE,
		            BOOL bDrawAlphaSubsets = TRUE );

	// Mesh access
    virtual LPD3DXMESH GetSysMemMesh() { return m_pSysMemMesh; }
    virtual LPD3DXMESH GetLocalMesh()  { return m_pLocalMesh; }
	virtual const NVBounds* GetBounds() const { return &m_Bounds; }

	// Rendering options
	virtual HRESULT SetFVF( NVDevice* pd3dDevice, DWORD dwFVF );
	virtual HRESULT RenderPart(NVDevice* pDevice, DWORD dwPart);
	virtual HRESULT SetVertexShader(DWORD dwVertexShader) { m_dwVertexShader = dwVertexShader; return S_OK;}
	virtual HRESULT SetTexture(LPDIRECT3DBASETEXTURE8 pTexture) { m_pTexture = pTexture; return S_OK; }
	virtual HRESULT GetRenderInfo();
	virtual HRESULT ComputeBounds();
	
	// Initializing
    virtual HRESULT RestoreDeviceObjects( NVDevice* pd3dDevice );
    virtual HRESULT InvalidateDeviceObjects();

	// Creation/destruction
	virtual HRESULT Create( NVDevice* pd3dDevice, const std::string& strFilename );
	virtual HRESULT Create( NVDevice* pd3dDevice, LPDIRECTXFILEDATA pFileData );
	virtual HRESULT Destroy();

	TCHAR               m_strName[512];
    LPD3DXMESH          m_pSysMemMesh;    // SysSem mesh, lives through resize
    LPD3DXMESH          m_pLocalMesh;     // Local mesh, rebuilt on resize
    
    DWORD               m_dwNumMaterials; // Materials for the mesh
    D3DMATERIAL8*       m_pMaterials;
    LPDIRECT3DTEXTURE8* m_pTextures;

	DWORD					m_dwVertexShader;
	LPDIRECT3DBASETEXTURE8	m_pTexture;

	// Attributes for rendering parts of the model
	LPD3DXBUFFER		m_pMeshAdjacency;

	LPDIRECT3DVERTEXBUFFER8 m_pVB;
	LPDIRECT3DINDEXBUFFER8 m_pIB;

	DWORD				m_dwAttributes;
	DWORD				m_dwStride;
	D3DXATTRIBUTERANGE*	m_pAttributeTable;	// Attributes

	std::string			m_strMeshPath;

	NVBounds m_Bounds;

};


}; // nv_objects

#endif // __NVDEVICE_H
	