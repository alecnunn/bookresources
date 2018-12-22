/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/
#ifndef __NVFRAME_H
#define __NVFRAME_H

#include "nvinc.h"
#include "nvmesh.h"

namespace nv_objects
{

class NVFrame
{
public:
	NVFrame( TCHAR* strName = _T("NVFile_Frame") );	
	~NVFrame();

	TCHAR      m_strName[512];
	D3DXMATRIX m_mat;
	NVMesh*    m_pMesh;

	NVFrame* m_pNext;
	NVFrame* m_pChild;

public:
	// Matrix access
    VOID        SetMatrix( D3DXMATRIX* pmat ) { m_mat = *pmat; }
    D3DXMATRIX* GetMatrix()                   { return &m_mat; }

	NVMesh*   FindMesh( TCHAR* strMeshName );
	NVFrame*  FindFrame( TCHAR* strFrameName );
    bool EnumMeshes( bool (*EnumMeshCB)(NVMesh*,D3DXMATRIX*,VOID*), VOID* pContext );
	bool WalkFrames(bool (*WalkFramesCB)(NVFrame*, D3DXMATRIX* pMatrix, VOID*), D3DXMATRIX* pMatrix, void* pContext);


	HRESULT Destroy();
    HRESULT RestoreDeviceObjects( NVDevice* pDevice );
	HRESULT SetFVF(NVDevice* pDevice, DWORD dwFVF);
	HRESULT SetTexture(LPDIRECT3DBASETEXTURE8 pTexture);
    HRESULT InvalidateDeviceObjects();
    HRESULT Render( NVDevice* pDevice, 
		            BOOL bDrawOpaqueSubsets = TRUE,
		            BOOL bDrawAlphaSubsets = TRUE );

};

}; // nv_objects
#endif



