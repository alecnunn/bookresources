/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/

#include "eb_effect.h"
#include "nvdebug.h"
#include "shader_Dot3_Util.h"

#define SMALL_FLOAT 1e-12

// Creates basis vectors, based on a vertex and index list.
// NOTE: Assumes an indexed triangle list, with vertices of type Dot3Vertex
HRESULT CreateBasisVectors(LPDIRECT3DVERTEXBUFFER8 pVertexBuffer, LPDIRECT3DINDEXBUFFER8 pIndexBuffer)
{
	DWORD i;
	Dot3Vertex* pVertices;
	WORD* pIndices;
	HRESULT hr;
	
	assert(pVertexBuffer);
	assert(pIndexBuffer);

	// Calculate number of vertices and indices
	D3DVERTEXBUFFER_DESC VBDesc;
	D3DINDEXBUFFER_DESC IBDesc;
	
	pVertexBuffer->GetDesc(&VBDesc);
	pIndexBuffer->GetDesc(&IBDesc);

	DWORD dwNumIndices;
	DWORD dwNumVertices;
	switch(IBDesc.Format)
	{
		case D3DFMT_INDEX16:
			dwNumIndices = IBDesc.Size / 2;
			break;
		case D3DFMT_INDEX32:
			dwNumIndices = IBDesc.Size / 4;
			break;
		default:
			assert(0);
			return E_FAIL;
	}

	dwNumVertices = VBDesc.Size / sizeof(Dot3Vertex);

	// Get a pointer to the indices and the vertices
	hr = pVertexBuffer->Lock(0, 0, (BYTE**)&pVertices, 0);
	if (FAILED(hr))
		return hr;

	hr = pIndexBuffer->Lock(0, 0, (BYTE**)&pIndices, 0);
	if (FAILED(hr))
		return hr;

	// Clear the basis vectors
	for (i = 0; i < dwNumVertices; i++)
	{
		pVertices[i].S = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVertices[i].T = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}

	// Walk through the triangle list and calculate gradiants for each triangle.
	// Sum the results into the S and T components.
    for( i = 0; i < dwNumIndices; i += 3 )
    {       
		DWORD TriIndex[3];
		D3DXVECTOR3 du, dv;
		D3DXVECTOR3 edge01;
		D3DXVECTOR3 edge02;
		D3DXVECTOR3 cp;
		
		TriIndex[0] = pIndices[i];
		TriIndex[1] = pIndices[i+1];
		TriIndex[2] = pIndices[i+2];

		assert((TriIndex[0] < dwNumVertices) && (TriIndex[1] < dwNumVertices) && (TriIndex[2] < dwNumVertices));

		Dot3Vertex& v0 = pVertices[TriIndex[0]];
		Dot3Vertex& v1 = pVertices[TriIndex[1]];
		Dot3Vertex& v2 = pVertices[TriIndex[2]];

		// x, s, t
		edge01 = D3DXVECTOR3( v1.Position.x - v0.Position.x, v1.Texture.x - v0.Texture.x, v1.Texture.y - v0.Texture.y );
		edge02 = D3DXVECTOR3( v2.Position.x - v0.Position.x, v2.Texture.x - v0.Texture.x, v2.Texture.y - v0.Texture.y );

		D3DXVec3Cross(&cp, &edge01, &edge02);
		if ( fabs(cp.x) > SMALL_FLOAT )
		{
			v0.S.x += -cp.y / cp.x;
			v0.T.x += -cp.z / cp.x;

			v1.S.x += -cp.y / cp.x;
			v1.T.x += -cp.z / cp.x;
			
			v2.S.x += -cp.y / cp.x;
			v2.T.x += -cp.z / cp.x;
		}

		// y, s, t
		edge01 = D3DXVECTOR3( v1.Position.y - v0.Position.y, v1.Texture.x - v0.Texture.x, v1.Texture.y - v0.Texture.y );
		edge02 = D3DXVECTOR3( v2.Position.y - v0.Position.y, v2.Texture.x - v0.Texture.x, v2.Texture.y - v0.Texture.y );

		D3DXVec3Cross(&cp, &edge01, &edge02);
		if ( fabs(cp.x) > SMALL_FLOAT )
		{
			v0.S.y += -cp.y / cp.x;
			v0.T.y += -cp.z / cp.x;

			v1.S.y += -cp.y / cp.x;
			v1.T.y += -cp.z / cp.x;
			
			v2.S.y += -cp.y / cp.x;
			v2.T.y += -cp.z / cp.x;
		}

		// z, s, t
		edge01 = D3DXVECTOR3( v1.Position.z - v0.Position.z, v1.Texture.x - v0.Texture.x, v1.Texture.y - v0.Texture.y );
		edge02 = D3DXVECTOR3( v2.Position.z - v0.Position.z, v2.Texture.x - v0.Texture.x, v2.Texture.y - v0.Texture.y );

		D3DXVec3Cross(&cp, &edge01, &edge02);
		if ( fabs(cp.x) > SMALL_FLOAT )
		{
			v0.S.z += -cp.y / cp.x;
			v0.T.z += -cp.z / cp.x;

			v1.S.z += -cp.y / cp.x;
			v1.T.z += -cp.z / cp.x;
			
			v2.S.z += -cp.y / cp.x;
			v2.T.z += -cp.z / cp.x;
		}
    }

    // Calculate the SxT vector
  	for(i = 0; i < dwNumVertices; i++)
  	{		
  		// Normalize the S, T vectors
  		D3DXVec3Normalize(&pVertices[i].S, &pVertices[i].S);
  		D3DXVec3Normalize(&pVertices[i].T, &pVertices[i].T);
  
  		// Get the cross of the S and T vectors
  		D3DXVec3Cross(&pVertices[i].SxT, &pVertices[i].S, &pVertices[i].T);
  
  		// Need a normalized normal
  		D3DXVec3Normalize(&pVertices[i].Normal, &pVertices[i].Normal);
  
  		// v coordinates go in opposite direction from the texture v increase in xyz
  		pVertices[i].T = -pVertices[i].T;
  		
  		// Get the direction of the SxT vector
  		if (D3DXVec3Dot(&pVertices[i].SxT, &pVertices[i].Normal) < 0.0f)
  		{
  			pVertices[i].SxT = -pVertices[i].SxT;
  		}
  	}

	pVertexBuffer->Unlock();
	pIndexBuffer->Unlock();

	return S_OK;

}

