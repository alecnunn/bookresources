/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/
#include "nvinc.h"
#include "nvframe.h"
namespace nv_objects
{

NVFrame::NVFrame( TCHAR* strName )
: m_pMesh(NULL),
	m_pChild(NULL),
	m_pNext(NULL)

{
    _tcscpy( m_strName, strName );
    D3DXMatrixIdentity( &m_mat );
}

NVFrame::~NVFrame()
{
	Destroy();
}

bool NVFrame::EnumMeshes( bool (*EnumMeshCB)(NVMesh*, D3DXMATRIX*, VOID*), VOID* pContext)
{
	if (m_pMesh)
		EnumMeshCB( m_pMesh, &m_mat, pContext );
    if( m_pChild )
        m_pChild->EnumMeshes( EnumMeshCB, pContext );
    if( m_pNext )
        m_pNext->EnumMeshes( EnumMeshCB, pContext );

    return TRUE;
}

NVMesh* NVFrame::FindMesh( TCHAR* strMeshName )
{
    NVMesh* pMesh;

    if( m_pMesh )
        if( !lstrcmpi( m_pMesh->m_strName, strMeshName ) )
            return m_pMesh;

    if( m_pChild )
        if( NULL != ( pMesh = m_pChild->FindMesh( strMeshName ) ) )
            return pMesh;

    if( m_pNext )
        if( NULL != ( pMesh = m_pNext->FindMesh( strMeshName ) ) )
            return pMesh;

    return NULL;
}

NVFrame* NVFrame::FindFrame( TCHAR* strFrameName )
{
    NVFrame* pFrame;

    if( !lstrcmpi( m_strName, strFrameName ) )
        return this;

    if( m_pChild )
        if( NULL != ( pFrame = m_pChild->FindFrame( strFrameName ) ) )
            return pFrame;

    if( m_pNext )
        if( NULL != ( pFrame = m_pNext->FindFrame( strFrameName ) ) )
            return pFrame;

    return NULL;
}

HRESULT NVFrame::Destroy()
{
	InvalidateDeviceObjects();

    if( m_pMesh )  m_pMesh->Destroy();
    if( m_pChild ) m_pChild->Destroy();
    if( m_pNext )  m_pNext->Destroy();

    SAFE_DELETE( m_pMesh );
    SAFE_DELETE( m_pNext );
    SAFE_DELETE( m_pChild );

    return S_OK;
}

// Walks the whole hierarchy setting up this FVF for all objects
HRESULT NVFrame::SetFVF(NVDevice* pDevice, DWORD dwFVF)
{
	if (m_pMesh) m_pMesh->SetFVF(pDevice, dwFVF);
	if (m_pChild) m_pChild->SetFVF(pDevice, dwFVF);
	if (m_pNext) m_pNext->SetFVF(pDevice, dwFVF);
	return S_OK;
}

HRESULT NVFrame::SetTexture(LPDIRECT3DBASETEXTURE8 pTexture)
{
	if (m_pMesh) m_pMesh->SetTexture(pTexture);
	if (m_pChild) m_pChild->SetTexture(pTexture);
	if (m_pNext) m_pNext->SetTexture(pTexture);
	return S_OK;
}

HRESULT NVFrame::RestoreDeviceObjects( NVDevice* pDevice )
{
    if( m_pMesh )  m_pMesh->RestoreDeviceObjects( pDevice );
    if( m_pChild ) m_pChild->RestoreDeviceObjects( pDevice );
    if( m_pNext )  m_pNext->RestoreDeviceObjects( pDevice );
    return S_OK;
}

HRESULT NVFrame::InvalidateDeviceObjects()
{
    if( m_pMesh )  m_pMesh->InvalidateDeviceObjects();
    if( m_pChild ) m_pChild->InvalidateDeviceObjects();
    if( m_pNext )  m_pNext->InvalidateDeviceObjects();
    return S_OK;
}

HRESULT NVFrame::Render( NVDevice* pDevice, BOOL bDrawOpaqueSubsets,
                           BOOL bDrawAlphaSubsets )
{
    D3DXMATRIX matSavedWorld, matWorld;
    matSavedWorld = pDevice->GetWorldTransform();
    D3DXMatrixMultiply( &matWorld, &m_mat, &matSavedWorld );
    pDevice->SetWorldTransform(&matWorld );

    if( m_pMesh )
        m_pMesh->Render( pDevice, bDrawOpaqueSubsets, bDrawAlphaSubsets );

    if( m_pChild )
        m_pChild->Render( pDevice, bDrawOpaqueSubsets, bDrawAlphaSubsets );

    pDevice->SetWorldTransform(&matSavedWorld );

    if( m_pNext )
        m_pNext->Render( pDevice, bDrawOpaqueSubsets, bDrawAlphaSubsets );

    return S_OK;
}

// Walks the frames, applying the transforms down the hierarchy.  Calls a callback
// with the current frame and transform matrix.
bool NVFrame::WalkFrames(bool (*WalkFramesCB)(NVFrame*, D3DXMATRIX* pMatrix, VOID*), D3DXMATRIX* pMatrix, void* pContext)
{
	D3DXMATRIX matSavedWorld, matWorld;
    matSavedWorld = *pMatrix;
    D3DXMatrixMultiply( &matWorld, &m_mat, &matSavedWorld );
    
	WalkFramesCB(this, &matWorld, pContext);

    if( m_pChild )
	{
		m_pChild->WalkFrames(WalkFramesCB, &matWorld, pContext);
	}
	
    if( m_pNext )
	{
		m_pNext->WalkFrames(WalkFramesCB, pMatrix, pContext);
	}

	return true;
}


}; // nv_objects namespace
