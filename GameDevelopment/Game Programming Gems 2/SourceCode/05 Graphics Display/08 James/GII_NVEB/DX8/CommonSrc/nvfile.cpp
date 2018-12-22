/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/
#include "nvinc.h"
#include "rmxfguid.h"
#include "rmxftmpl.h"
#include "nvfile.h"

using namespace std;
namespace nv_objects
{

HRESULT NVFile::LoadFrame( NVDevice* pDevice,
                             LPDIRECTXFILEDATA pFileData,
                             NVFrame* pParentFrame )
{
    LPDIRECTXFILEDATA   pChildData = NULL;
    LPDIRECTXFILEOBJECT pChildObj = NULL;
    const GUID* pGUID;
    DWORD       cbSize;
    NVFrame*  pCurrentFrame;
    HRESULT     hr;

    // Get the type of the object
    if( FAILED( hr = pFileData->GetType( &pGUID ) ) )
        return hr;

    if( *pGUID == TID_D3DRMMesh )
    {
        hr = LoadMesh( pDevice, pFileData, pParentFrame );
        if( FAILED(hr) )
            return hr;
    }
    if( *pGUID == TID_D3DRMFrameTransformMatrix )
    {
        D3DXMATRIX* pmatMatrix;
        hr = pFileData->GetData( NULL, &cbSize, (VOID**)&pmatMatrix );
        if( FAILED(hr) )
            return hr;

        // Update the parents matrix with the new one
        pParentFrame->SetMatrix( pmatMatrix );
    }
    if( *pGUID == TID_D3DRMFrame )
    {
        // Get the frame name
        CHAR strAnsiName[512];
        DWORD dwNameLength;
        pFileData->GetName( NULL, &dwNameLength );
        if( dwNameLength > 0 )
            pFileData->GetName( strAnsiName, &dwNameLength );

        // Create the frame
        pCurrentFrame = new NVFrame( strAnsiName );

        pCurrentFrame->m_pNext = pParentFrame->m_pChild;
        pParentFrame->m_pChild = pCurrentFrame;

        // Enumerate child objects
        while( SUCCEEDED( pFileData->GetNextObject( &pChildObj ) ) )
        {
            // Query the child for it's FileData
            hr = pChildObj->QueryInterface( IID_IDirectXFileData,
                                            (VOID**)&pChildData );
            if( SUCCEEDED(hr) )
            {
                hr = LoadFrame( pDevice, pChildData, pCurrentFrame );
                pChildData->Release();
            }

            pChildObj->Release();

            if( FAILED(hr) )
                return hr;
        }
    }

    return S_OK;
}

HRESULT NVFile::LoadMesh( NVDevice* pDevice,
                            LPDIRECTXFILEDATA pFileData,
                            NVFrame* pParentFrame )
{
    // Currently only allowing one mesh per frame
    if( pParentFrame->m_pMesh )
        return E_FAIL;

    // Get the mesh name
    CHAR  strAnsiName[512];
    DWORD dwNameLength;
    pFileData->GetName( NULL, &dwNameLength );
    if( dwNameLength > 0 )
        pFileData->GetName( strAnsiName, &dwNameLength );

    // Create the mesh
    pParentFrame->m_pMesh = new NVMesh( strAnsiName );
    pParentFrame->m_pMesh->Create( pDevice, pFileData );

    return S_OK;
}

HRESULT NVFile::Create( NVDevice* pDevice, const string& strPathname )
{
    LPDIRECTXFILE           pDXFile   = NULL;
    LPDIRECTXFILEENUMOBJECT pEnumObj  = NULL;
    LPDIRECTXFILEDATA       pFileData = NULL;
    HRESULT hr;

	std::string::size_type Pos = strPathname.find_last_of("\\", strPathname.size());
	if (Pos != strPathname.npos)
	{
		// Make sure we are on the right path for loading resources associated with this file
		m_strFilePath = strPathname.substr(0, Pos);
		SetCurrentDirectory(m_strFilePath.c_str());
	}

    // Create a x file object
    if( FAILED( hr = DirectXFileCreate( &pDXFile ) ) )
        return E_FAIL;

    // Register templates for d3drm and patch extensions.
    if( FAILED( hr = pDXFile->RegisterTemplates( (VOID*)D3DRM_XTEMPLATES,
                                                 D3DRM_XTEMPLATE_BYTES ) ) )
    {
        pDXFile->Release();
        return E_FAIL;
    }

    // Create enum object
    hr = pDXFile->CreateEnumObject( (VOID*)strPathname.c_str(),
                                    DXFILELOAD_FROMFILE, &pEnumObj );
	if (FAILED(hr))
    {
		pDXFile->Release();
        return hr;
    }

    // Enumerate top level objects (which are always frames)
    while( SUCCEEDED( pEnumObj->GetNextDataObject( &pFileData ) ) )
    {
        hr = LoadFrame( pDevice, pFileData, this );
        pFileData->Release();
        if( FAILED(hr) )
        {
            pEnumObj->Release();
            pDXFile->Release();
            return E_FAIL;
        }
    }

    SAFE_RELEASE( pFileData );
    SAFE_RELEASE( pEnumObj );
    SAFE_RELEASE( pDXFile );

    return S_OK;
}

HRESULT NVFile::Render( NVDevice* pDevice )
{
    // Setup the world transformation
    D3DXMATRIX matSavedWorld, matWorld;
    matSavedWorld = pDevice->GetWorldTransform();
    D3DXMatrixMultiply( &matWorld, &matSavedWorld, &m_mat );
    pDevice->SetWorldTransform(&matWorld );

    // Render opaque subsets in the meshes
    if( m_pChild )
        m_pChild->Render( pDevice, TRUE, FALSE );

    // Enable alpha blending
    pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    pDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

    // Render alpha subsets in the meshes
    if( m_pChild )
        m_pChild->Render( pDevice, FALSE, TRUE );

    // Restore state
    pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    pDevice->SetWorldTransform(&matSavedWorld );

    return S_OK;
}


bool CalcFileObjectSizeCB( NVFrame* pFrame, D3DXMATRIX* pMat, VOID* pfSize )
{
	NVBounds* pBounds = (NVBounds*)pfSize;
	NVMesh* pMesh = pFrame->m_pMesh;
	DWORD dwStride = 0;

	if (!pMesh)
	{
		return true;
	}
	
	// Get the bounds for the mesh and transform them by the local
	// world transform.
	NVBounds MeshBounds = *pMesh->GetBounds();
	MeshBounds.Transform(pMat);
	
	if (pBounds->m_vecMaxExtents.x < MeshBounds.m_vecMaxExtents.x)
		pBounds->m_vecMaxExtents.x = MeshBounds.m_vecMaxExtents.x;

	if (pBounds->m_vecMaxExtents.y < MeshBounds.m_vecMaxExtents.y)
		pBounds->m_vecMaxExtents.y = MeshBounds.m_vecMaxExtents.y;

	if (pBounds->m_vecMaxExtents.z < MeshBounds.m_vecMaxExtents.z)
		pBounds->m_vecMaxExtents.z = MeshBounds.m_vecMaxExtents.z;


	if (pBounds->m_vecMinExtents.x > MeshBounds.m_vecMinExtents.x)
		pBounds->m_vecMinExtents.x = MeshBounds.m_vecMinExtents.x;

	if (pBounds->m_vecMinExtents.y > MeshBounds.m_vecMinExtents.y)
		pBounds->m_vecMinExtents.y = MeshBounds.m_vecMinExtents.y;

	if (pBounds->m_vecMinExtents.z > MeshBounds.m_vecMinExtents.z)
		pBounds->m_vecMinExtents.z = MeshBounds.m_vecMinExtents.z;

	if (pBounds->m_fRadius < MeshBounds.m_fRadius)
		pBounds->m_fRadius = MeshBounds.m_fRadius;

    return true;
}

//-----------------------------------------------------------------------------
// Name: GetBoundingInfo()
// Desc:
//-----------------------------------------------------------------------------
void NVFile::GetBoundingInfo(NVBounds* pBounds)
{
	pBounds->m_vecCenter = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pBounds->m_fRadius = 0.0f;
	pBounds->m_vecMinExtents = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
	pBounds->m_vecMaxExtents = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	D3DXMATRIX Matrix;
	D3DXMatrixIdentity(&Matrix);
    WalkFrames(CalcFileObjectSizeCB, &Matrix, (VOID*)pBounds);

	pBounds->m_vecCenter = (pBounds->m_vecMaxExtents + pBounds->m_vecMinExtents) / 2.0f;
}

}; // namespace nv_objects