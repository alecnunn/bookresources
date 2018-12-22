/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/
#include "nvmesh.h"

using namespace std;

namespace nv_objects
{

NVMesh::NVMesh(TCHAR* strName)
{
    _tcscpy( m_strName, strName );
    m_pSysMemMesh        = NULL;
    m_pLocalMesh         = NULL;
    m_dwNumMaterials     = 0L;
    m_pMaterials         = NULL;
    m_pTextures          = NULL;
	m_pMeshAdjacency	 = NULL;
	m_pVB				 = NULL;
	m_pIB				 = NULL;
	m_pAttributeTable = NULL;
	m_dwAttributes = 0;
	m_dwVertexShader = 0;
	m_pTexture = NULL;
	m_pTextures = NULL;
}

NVMesh::~NVMesh()
{
	Destroy();
}

HRESULT NVMesh::Create( NVDevice* pd3dDevice, const std::string& strPathName)
{
    LPD3DXBUFFER pMtrlBuffer = NULL;
    HRESULT      hr;


	std::string::size_type Pos = strPathName.find_last_of("\\", strPathName.size());
	if (Pos != strPathName.npos)
	{
		// Check the root directory of the running process
		m_strMeshPath = strPathName.substr(0, Pos);
	}
	else
	{
		m_strMeshPath = ".";
	}

	// Load the mesh from the specified file
    hr = D3DXLoadMeshFromX( (char*)strPathName.c_str(), D3DXMESH_MANAGED, pd3dDevice->GetDevice(),
                                           &m_pMeshAdjacency, &pMtrlBuffer, &m_dwNumMaterials,
                                           &m_pSysMemMesh);

	if (FAILED(hr))
		return hr;

	LPD3DXMESH pMeshTemp = NULL;
    hr = m_pSysMemMesh->Optimize(/*D3DXMESHOPT_COMPACT |*/ D3DXMESHOPT_ATTRSORT /*| D3DXMESHOPT_VERTEXCACHE*/,
                                (DWORD*)m_pMeshAdjacency->GetBufferPointer(), NULL, NULL, NULL, &pMeshTemp);
	if (FAILED(hr))
		return hr;

	SAFE_RELEASE(m_pSysMemMesh);
	m_pSysMemMesh = pMeshTemp;

    // Get material info for the mesh
    InitMaterials( pd3dDevice, pMtrlBuffer );
    SAFE_RELEASE( pMtrlBuffer );

	// Calculate the stride of the current FVF buffer that will be created
	m_dwStride = D3DXGetFVFVertexSize(m_pSysMemMesh->GetFVF());
	ComputeBounds();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT NVMesh::Create( NVDevice* pd3dDevice,
                          LPDIRECTXFILEDATA pFileData )
{
    LPD3DXBUFFER pMtrlBuffer = NULL;
    HRESULT      hr;

	m_strMeshPath = ".";

    // Load the mesh from the DXFILEDATA object
    hr = D3DXLoadMeshFromXof( pFileData, D3DXMESH_MANAGED, pd3dDevice->GetDevice(),
                              &m_pMeshAdjacency, &pMtrlBuffer, &m_dwNumMaterials,
                              &m_pSysMemMesh);
    if( FAILED(hr) )
        return hr;

	LPD3DXMESH pMeshTemp = NULL;
    hr = m_pSysMemMesh->Optimize(/*D3DXMESHOPT_COMPACT |*/ D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
                                (DWORD*)m_pMeshAdjacency->GetBufferPointer(), NULL, NULL, NULL, &pMeshTemp);
	if (FAILED(hr))
		return hr;

	SAFE_RELEASE(m_pSysMemMesh);
	m_pSysMemMesh = pMeshTemp;


    // Get material info for the mesh
    InitMaterials( pd3dDevice, pMtrlBuffer );
    SAFE_RELEASE( pMtrlBuffer );

	// Calculate the stride of the current FVF buffer that will be created
	m_dwStride = D3DXGetFVFVertexSize(m_pSysMemMesh->GetFVF());
	ComputeBounds();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT NVMesh::InitMaterials( NVDevice* pd3dDevice,
                                 LPD3DXBUFFER pMtrlBuffer )
{
    HRESULT hr;

    // Get the array of materials out of the buffer
    if( pMtrlBuffer && m_dwNumMaterials > 0 )
    {
        // Allocate memory for the materials and textures
        D3DXMATERIAL* d3dxMtrls = (D3DXMATERIAL*)pMtrlBuffer->GetBufferPointer();
        m_pMaterials = new D3DMATERIAL8[m_dwNumMaterials];
        m_pTextures  = new LPDIRECT3DTEXTURE8[m_dwNumMaterials];

        // Copy each material and create it's texture
        for( DWORD i=0; i<m_dwNumMaterials; i++ )
        {
            m_pMaterials[i] = d3dxMtrls[i].MatD3D;
            m_pMaterials[i].Ambient = m_pMaterials[i].Diffuse;

            // Create a texture, if specified
            if( d3dxMtrls[i].pTextureFilename )
            {

				std::string strTexturePath = m_strMeshPath + "\\" + d3dxMtrls[i].pTextureFilename;
				hr = D3DXCreateTextureFromFileEx(pd3dDevice->GetDevice(), 
					strTexturePath.c_str(),
					D3DX_DEFAULT,
					D3DX_DEFAULT,
					0,
					0,
					D3DFMT_UNKNOWN,
					D3DPOOL_MANAGED,
					D3DX_FILTER_LINEAR,
					D3DX_FILTER_LINEAR,
					0,
					NULL,
					NULL,
					&m_pTextures[i]);
				if (FAILED(hr))
				{
					m_pTextures[i] = NULL;
					continue;
				}
            }
			else
			{
				m_pTextures[i] = NULL;
			}
        }
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT NVMesh::SetFVF( NVDevice* pd3dDevice, DWORD dwFVF )
{
    LPD3DXMESH pTempSysMemMesh = NULL;
    LPD3DXMESH pTempLocalMesh  = NULL;

	DWORD Declaration[MAX_FVF_DECL_SIZE];
    D3DXDeclaratorFromFVF(dwFVF, Declaration);

    if( m_pSysMemMesh )
    {
		if ( FAILED( m_pSysMemMesh->CloneMeshFVF( D3DXMESH_MANAGED, 
			                                      dwFVF, 
												  pd3dDevice->GetDevice(), 
												  &pTempSysMemMesh ) ) )
		{
	        return E_FAIL;
		}

	    SAFE_RELEASE( m_pSysMemMesh );
		if( pTempSysMemMesh ) 
			m_pSysMemMesh = pTempSysMemMesh;

	    // Compute normals in case the meshes have them
	    D3DXComputeNormals( m_pSysMemMesh );
    }

    if( m_pLocalMesh )
    {

		if ( FAILED( m_pLocalMesh->CloneMeshFVF( D3DXMESH_MANAGED, 
			                                      dwFVF, 
												  pd3dDevice->GetDevice(), 
												  &pTempLocalMesh ) ) )
		{
            SAFE_RELEASE( pTempSysMemMesh );
	        return E_FAIL;
		}

		SAFE_RELEASE( m_pLocalMesh );
		if( pTempLocalMesh ) 
			m_pLocalMesh  = pTempLocalMesh;
		
		D3DXComputeNormals( m_pLocalMesh );

		GetRenderInfo();
    }

	// Calculate the stride of the current FVF buffer that will be created
	m_dwStride = D3DXGetFVFVertexSize(m_pSysMemMesh->GetFVF());

    return S_OK;
}

HRESULT NVMesh::ComputeBounds()
{
	HRESULT hr;
	m_Bounds.m_vecCenter = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Bounds.m_fRadius = 0.0f;
	m_Bounds.m_vecMinExtents = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
	m_Bounds.m_vecMaxExtents = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    // Walk the geometry
    LPDIRECT3DVERTEXBUFFER8 pVB;
    hr = m_pSysMemMesh->GetVertexBuffer(&pVB);
	if (FAILED(hr))
		return false;

	D3DXVECTOR3* pVertices;
	DWORD   dwNumVertices = m_pSysMemMesh->GetNumVertices();

	pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 );

	// We assume that the position is the first thing in the vertex data, and 
	// that it is 3 floats long... This is OK, because the mesh classes only
	// handle valid FVF meshes.
    for( DWORD i=0; i<dwNumVertices; i++ )
	{
		pVertices = (D3DXVECTOR3*) (((BYTE*)pVertices) + m_dwStride);


		if (m_Bounds.m_vecMaxExtents.x < pVertices->x)
			m_Bounds.m_vecMaxExtents.x = pVertices->x;

		if (m_Bounds.m_vecMaxExtents.y < pVertices->y)
			m_Bounds.m_vecMaxExtents.y = pVertices->y;

		if (m_Bounds.m_vecMaxExtents.z < pVertices->z)
			m_Bounds.m_vecMaxExtents.z = pVertices->z;

		if (m_Bounds.m_vecMinExtents.x > pVertices->x)
			m_Bounds.m_vecMinExtents.x = pVertices->x;

		if (m_Bounds.m_vecMinExtents.y > pVertices->y)
			m_Bounds.m_vecMinExtents.y = pVertices->y;

		if (m_Bounds.m_vecMinExtents.z > pVertices->z)
			m_Bounds.m_vecMinExtents.z = pVertices->z;

	}

	pVB->Unlock();
	SAFE_RELEASE(pVB);


	m_Bounds.m_vecCenter = ( m_Bounds.m_vecMaxExtents + m_Bounds.m_vecMinExtents ) / 2.0f;
	
	D3DXVECTOR3 aVector( m_Bounds.m_vecMaxExtents.x - m_Bounds.m_vecCenter.x,
						 m_Bounds.m_vecMaxExtents.y - m_Bounds.m_vecCenter.y,
						 m_Bounds.m_vecMaxExtents.z - m_Bounds.m_vecCenter.z );

	m_Bounds.m_fRadius = sqrtf( aVector.x * aVector.x + 
								aVector.y * aVector.y + 
								aVector.z * aVector.z );

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT NVMesh::RestoreDeviceObjects( NVDevice* pd3dDevice )
{
    if( NULL == m_pSysMemMesh )
        return E_FAIL;

	SAFE_RELEASE(m_pLocalMesh);

    // Make a local memory version of the mesh. Note: because we are passing in
    // no flags, the default behavior is to clone into local memory.
    if( FAILED( m_pSysMemMesh->CloneMeshFVF( D3DXMESH_MANAGED, m_pSysMemMesh->GetFVF(),
                                             pd3dDevice->GetDevice(), &m_pLocalMesh ) ) )
        return E_FAIL;

	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);

	m_pLocalMesh->GetVertexBuffer(&m_pVB);
	m_pLocalMesh->GetIndexBuffer(&m_pIB);

	GetRenderInfo();

    return S_OK;

}

HRESULT NVMesh::GetRenderInfo()
{
	assert(m_pLocalMesh);

	// Get the file attributes
	SAFE_DELETE_ARRAY(m_pAttributeTable);
	m_pAttributeTable = NULL;
	m_dwAttributes = 0;

	m_pAttributeTable = new D3DXATTRIBUTERANGE[m_dwNumMaterials];	
	ZeroMemory(m_pAttributeTable, sizeof(D3DXATTRIBUTERANGE) * m_dwNumMaterials);
	m_pLocalMesh->GetAttributeTable(m_pAttributeTable, &m_dwAttributes);

	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);

	m_pLocalMesh->GetVertexBuffer(&m_pVB);
	m_pLocalMesh->GetIndexBuffer(&m_pIB);

	return S_OK;
}



//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT NVMesh::InvalidateDeviceObjects()
{
	SAFE_DELETE_ARRAY(m_pAttributeTable);
    SAFE_RELEASE( m_pLocalMesh );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT NVMesh::Destroy()
{
	InvalidateDeviceObjects();
	if (m_pTextures)
	{
		for(int i=0; i< m_dwNumMaterials; i++ )
		{
			SAFE_RELEASE( m_pTextures[i] );
		}
	}
    SAFE_DELETE_ARRAY( m_pTextures );
    SAFE_DELETE_ARRAY( m_pMaterials );

    SAFE_RELEASE(m_pSysMemMesh);
	SAFE_RELEASE(m_pMeshAdjacency);

	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);

	SAFE_DELETE_ARRAY(m_pAttributeTable );
    m_dwNumMaterials = 0L;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT NVMesh::Render( NVDevice* pDevice, BOOL bDrawOpaqueSubsets,
                          BOOL bDrawAlphaSubsets )
{
    if( NULL == m_pLocalMesh )
        return E_FAIL;

    // Frist, draw the subsets without alpha
    if( bDrawOpaqueSubsets )
    {
        for( DWORD i=0; i<m_dwNumMaterials; i++ )
        {
            if( m_pMaterials[i].Diffuse.a < 1.0f )
                    continue;

            RenderPart(pDevice, i);
        }
    }

    // Then, draw the subsets with alpha
    if( bDrawAlphaSubsets)
    {
        for( DWORD i=0; i<m_dwNumMaterials; i++ )
        {
            if( m_pMaterials[i].Diffuse.a == 1.0f )
                continue;

            RenderPart(pDevice, i);
        }
    }

    return S_OK;
}

HRESULT NVMesh::RenderPart(NVDevice* pDevice, DWORD dwPart)
{
	D3DXATTRIBUTERANGE* pAttrib = &m_pAttributeTable[dwPart];
	if (!pAttrib)
		return E_FAIL;

	// Tell the device the part we are rendering
	pDevice->SetRenderPart(dwPart);

	// Set the material for this part
    pDevice->SetMaterial(&m_pMaterials[dwPart] );

	// Set the texture for this part
	if (m_pTexture != NULL)
	{
		pDevice->SetTexture(0, m_pTexture);
	}
	else
	{
		if(m_pTextures[dwPart] != NULL)
			pDevice->SetTexture(0, m_pTextures[dwPart] );
		else
			pDevice->SetTexture(0, NULL);
	}
         
	// Set the vertex shader for this mesh
	if (m_dwVertexShader == 0)
		pDevice->SetVertexShader(m_pLocalMesh->GetFVF());
	else
		pDevice->SetVertexShader(m_dwVertexShader);

	// Setup the stream and index buffers for the mesh
	pDevice->SetStreamSource(0, m_pVB, m_dwStride);
	pDevice->SetIndices(m_pIB, 0);

	// Draw it.
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, pAttrib->VertexStart, pAttrib->VertexCount, pAttrib->FaceStart * 3, pAttrib->FaceCount);

	return S_OK;
}

// Transforms bounding info by a matrix.
NVBounds::Transform(const D3DXMATRIX* pMatrix)
{
	NVBounds Transformed;

	D3DXVECTOR4 theCenter, theMin, theMax;

	D3DXVec3Transform(&theCenter, &m_vecCenter,  pMatrix);
	D3DXVec3Transform(&theMin, &m_vecMinExtents, pMatrix);
	D3DXVec3Transform(&theMax, &m_vecMaxExtents, pMatrix);

	// Min & Max components may have switched places during xform

	Transformed.m_vecMinExtents.x = min( theMin.x, theMax.x );
	Transformed.m_vecMinExtents.y = min( theMin.y, theMax.y );
	Transformed.m_vecMinExtents.z = min( theMin.z, theMax.z );

	Transformed.m_vecMaxExtents.x = max( theMin.x, theMax.x );
	Transformed.m_vecMaxExtents.y = max( theMin.y, theMax.y );
	Transformed.m_vecMaxExtents.z = max( theMin.z, theMax.z );

	Transformed.m_vecCenter = (Transformed.m_vecMaxExtents + Transformed.m_vecMinExtents) / 2.0f;

	// The bounding radius will either be from the center to the min or the center to the max

	// The max & min distance should be about the same, but I have seen differences, so we will select
	//  the max of the two anyway to ensure we get it right

	D3DXVECTOR3 maxDirection( Transformed.m_vecMaxExtents - Transformed.m_vecCenter );
	D3DXVECTOR3 minDirection( Transformed.m_vecMinExtents - Transformed.m_vecCenter );

	float radius1 = maxDirection.x * maxDirection.x +
					maxDirection.y * maxDirection.y +
					maxDirection.z * maxDirection.z;

	float radius2 = minDirection.x * minDirection.x +
					minDirection.y * minDirection.y +
					minDirection.z * minDirection.z;

	Transformed.m_fRadius = max( sqrtf( radius1 ), sqrtf( radius2 ) );

	*this = Transformed;
}

}; // nv_objects

