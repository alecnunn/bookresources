/*********************************************************************NVMH2****
Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/

#include "nvcomplexmesh.h"
#include "nvresourcemanager.h"
#include "nvtextureresource.h"
#include "nvrenderstyleresource.h"
#include <algorithm>
using namespace std;
namespace nv_objects
{

NVComplexMesh::NVComplexMesh()
: m_dwNumVertices(0),
	m_dwStride(0),
	m_pVB(NULL),
	m_pBoneNames(NULL),
	m_pBoneTransforms(NULL),
	m_pBoneCombinations(NULL),
	m_pBoneMatrices(NULL),
	m_dwNumBones(0),
	m_dwNumBoneCombinations(0),
	m_dwDesiredPaletteSize(10),
	m_dwFVF(0)
{
}

NVComplexMesh::~NVComplexMesh()
{
	Release();
}

void NVComplexMesh::Release()
{
	// Remove the vertex data associated with this mesh
	tVertexDataMap::iterator itrVertex = m_VertexData.begin();
	while(itrVertex != m_VertexData.end())
	{
		NVVertexData* pVertexData = itrVertex->second;
		SAFE_DELETE(pVertexData);
		itrVertex++;
	}
	m_dwNumVertices = 0;

	tIndexBufferArray::iterator itrIndices = m_IndexBuffers.begin();
	while (itrIndices != m_IndexBuffers.end())
	{
		SAFE_RELEASE(*itrIndices);
		itrIndices++;
	}
	m_IndexBuffers.clear();

	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pBoneNames);
	SAFE_RELEASE(m_pBoneTransforms);
	SAFE_RELEASE(m_pBoneCombinations);
	m_dwNumBoneCombinations = 0;
	m_dwNumBones = 0;
	
	SAFE_DELETE_ARRAY(m_pBoneMatrices);
}

bool NVComplexMesh::FlipNormals()
{
	NVVertexData* pNormalData = FindVertexData(NVVERT_NORMAL);
	if (!pNormalData)
		return false;

	D3DXVECTOR3* pData = GetDataPointer<D3DXVECTOR3, D3DDATATYPE_FLOAT3>(pNormalData);

	for (DWORD i = 0; i < pNormalData->size(); i++)
	{
		pData[i] = -pData[i];
	}

	return true;
}


bool NVComplexMesh::GenerateNormals()
{
	DWORD i;
	NVVertexData* pPositionData = FindVertexData(NVVERT_POSITION);
	NVVertexData* pNormalData = FindVertexData(NVVERT_NORMAL);

	if (!pPositionData || !pNormalData ||
		(pPositionData->GetDataType() != D3DDATATYPE_FLOAT3) ||
		(pNormalData->GetDataType() != D3DDATATYPE_FLOAT3))
	{
		return false;
	}

	D3DXVECTOR3* pPosition = GetDataPointer<D3DXVECTOR3, D3DDATATYPE_FLOAT3>(pPositionData);
	D3DXVECTOR3* pNormal = GetDataPointer<D3DXVECTOR3, D3DDATATYPE_FLOAT3>(pNormalData);

	tIndexArray::iterator itrIndices;
	D3DXVECTOR3 norm;

	// Clear the normals
	for (i = 0; i < pNormalData->size(); i++)
	{
		pNormal[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}

	// Generate normals
	itrIndices = m_Indices.begin();
	while (itrIndices != m_Indices.end())
	{
		D3DXVECTOR3 vec1 = pPosition[*(itrIndices + 0)] - pPosition[*(itrIndices + 1)];
		D3DXVECTOR3 vec2 = pPosition[*(itrIndices + 0)] - pPosition[*(itrIndices + 2)];
				
		D3DXVec3Cross(&norm, &vec1, &vec2);
		pNormal[*(itrIndices + 0)] += norm;
		pNormal[*(itrIndices + 1)] += norm;
		pNormal[*(itrIndices + 2)] += norm;

		itrIndices += 3;
	}

	// Normalize
	for (i = 0; i < pNormalData->size(); i++)
	{
		float Length;
		Length = D3DXVec3Length(&pNormal[i]);
		pNormal[i] /= Length;
	}

	return true;
}

bool NVComplexMesh::NormalizeNormals()
{
	NVVertexData* pNormalData = FindVertexData(NVVERT_NORMAL);

	if (!pNormalData || (pNormalData->GetDataType() != D3DDATATYPE_FLOAT3))
	{
		return false;
	}

	D3DXVECTOR3* pNormal = GetDataPointer<D3DXVECTOR3, D3DDATATYPE_FLOAT3>(pNormalData);
	
	// Normalize
	for (DWORD i = 0; i < pNormalData->size(); i++)
	{
		float Length;
		Length = D3DXVec3Length(&pNormal[i]);
		pNormal[i] /= Length;
	}

	return true;
}

bool NVComplexMesh::GenerateBasis(DWORD dwTexture, DWORD dwOption)
{
	DWORD i, j;
	float epsilon = 1.0e-5f;
	float x,y,z,dist;

	NVVertexData* pPositionData = FindVertexData(NVVERT_POSITION);
	NVVertexData* pNormalData = FindVertexData(NVVERT_NORMAL);
	NVVertexData* pTextureData;
	NVVertexData* pSBasisData;
	NVVertexData* pTBasisData;
	NVVertexData* pSxTBasisData;

	// Find the texture data
	pTextureData = FindVertexData(NVVERT_TEXTURE0 + dwTexture);

	// Find the basis vectors if available
	pSBasisData = FindVertexData(NVVERT_SBASIS0 + dwTexture);
	pTBasisData = FindVertexData(NVVERT_TBASIS0 + dwTexture);
	pSxTBasisData = FindVertexData(NVVERT_SxTBASIS0 + dwTexture);

	if (!pPositionData || !pTextureData || !pSBasisData || !pTBasisData ||
		(pPositionData->GetDataType() != D3DDATATYPE_FLOAT3) ||
		(pTextureData->GetDataType() != D3DDATATYPE_FLOAT2) ||
		(pSBasisData->GetDataType() != D3DDATATYPE_FLOAT3) || 
		(pTBasisData->GetDataType() != D3DDATATYPE_FLOAT3))
	{
		return false;
	}

	if (pSxTBasisData && !pNormalData)
	{
		// Need normal stream if generating SxTBasis.
		return false;
	}

	D3DXVECTOR3* pPosition = GetDataPointer<D3DXVECTOR3, D3DDATATYPE_FLOAT3>(pPositionData);
	D3DXVECTOR2* pTexture = GetDataPointer<D3DXVECTOR2, D3DDATATYPE_FLOAT2>(pTextureData);
	D3DXVECTOR3* pNormal = GetDataPointer<D3DXVECTOR3, D3DDATATYPE_FLOAT3>(pNormalData);
	D3DXVECTOR3* pSBasis = GetDataPointer<D3DXVECTOR3, D3DDATATYPE_FLOAT3>(pSBasisData);
	D3DXVECTOR3* pTBasis = GetDataPointer<D3DXVECTOR3, D3DDATATYPE_FLOAT3>(pTBasisData);

	// Pick up SxT if needed
	D3DXVECTOR3* pSxTBasis = NULL;
	if (pSxTBasisData)
	{
		pSxTBasis = GetDataPointer<D3DXVECTOR3, D3DDATATYPE_FLOAT3>(pSxTBasisData);
	}

	tIndexArray::iterator itrIndices;
	
	// Clear the basis vectors
	for (i = 0; i < m_dwNumVertices; i++)
	{
		pSBasis[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pTBasis[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}

	// Walk through the triangle list and calculate gradiants for each triangle.
	// Sum the results into the S and T components.
	itrIndices = m_Indices.begin();
	while (itrIndices != m_Indices.end())
	{
		DWORD TriIndex[3];
		D3DXVECTOR3 du, dv;
		D3DXVECTOR3 edge01;
		D3DXVECTOR3 edge02;
		D3DXVECTOR3 cp;
				
		TriIndex[0] = *(itrIndices + 0);
		TriIndex[1] = *(itrIndices + 1);
		TriIndex[2] = *(itrIndices + 2);
		itrIndices += 3;

		NVASSERT((TriIndex[0] < m_dwNumVertices) && (TriIndex[1] < m_dwNumVertices) && (TriIndex[2] < m_dwNumVertices), "Index exceeds vertex count");

		// Position locations
		D3DXVECTOR3& v0 = pPosition[TriIndex[0]];
		D3DXVECTOR3& v1 = pPosition[TriIndex[1]];
		D3DXVECTOR3& v2 = pPosition[TriIndex[2]];

		// Texture locations
		D3DXVECTOR2& t0 = pTexture[TriIndex[0]];
		D3DXVECTOR2& t1 = pTexture[TriIndex[1]];
		D3DXVECTOR2& t2 = pTexture[TriIndex[2]];
		
		// Basis locations
		D3DXVECTOR3& sb0 = pSBasis[TriIndex[0]];
		D3DXVECTOR3& tb0 = pTBasis[TriIndex[0]];
		D3DXVECTOR3& sb1 = pSBasis[TriIndex[1]];
		D3DXVECTOR3& tb1 = pTBasis[TriIndex[1]];
		D3DXVECTOR3& sb2 = pSBasis[TriIndex[2]];
		D3DXVECTOR3& tb2 = pTBasis[TriIndex[2]];

		// x, s, t
		edge01 = D3DXVECTOR3( v1.x - v0.x, t1.x - t0.x, t1.y - t0.y );
		edge02 = D3DXVECTOR3( v2.x - v0.x, t2.x - t0.x, t2.y - t0.y );

		D3DXVec3Cross(&cp, &edge01, &edge02);
		if ( fabs(cp.x) > SMALL_FLOAT )
		{
			sb0.x += -cp.y / cp.x;
			tb0.x += -cp.z / cp.x;

			sb1.x += -cp.y / cp.x;
			tb1.x += -cp.z / cp.x;
			
			sb2.x += -cp.y / cp.x;
			tb2.x += -cp.z / cp.x;
		}

		// y, s, t
		edge01 = D3DXVECTOR3( v1.y - v0.y, t1.x - t0.x, t1.y - t0.y );
		edge02 = D3DXVECTOR3( v2.y - v0.y, t2.x - t0.x, t2.y - t0.y );

		D3DXVec3Cross(&cp, &edge01, &edge02);
		if ( fabs(cp.x) > SMALL_FLOAT )
		{
			sb0.y += -cp.y / cp.x;
			tb0.y += -cp.z / cp.x;

			sb1.y += -cp.y / cp.x;
			tb1.y += -cp.z / cp.x;
			
			sb2.y += -cp.y / cp.x;
			tb2.y += -cp.z / cp.x;
		}

		// z, s, t
		edge01 = D3DXVECTOR3( v1.z - v0.z, t1.x - t0.x, t1.y - t0.y );
		edge02 = D3DXVECTOR3( v2.z - v0.z, t2.x - t0.x, t2.y - t0.y );

		D3DXVec3Cross(&cp, &edge01, &edge02);
		if ( fabs(cp.x) > SMALL_FLOAT )
		{
			sb0.z += -cp.y / cp.x;
			tb0.z += -cp.z / cp.x;

			sb1.z += -cp.y / cp.x;
			tb1.z += -cp.z / cp.x;
			
			sb2.z += -cp.y / cp.x;
			tb2.z += -cp.z / cp.x;
		}

    }

	if (dwOption & MeshOption_SmoothBasis)
	{
		////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////
		// Before normalizing the S, T vectors we have to 
		//  find degenerate vertices to average the basis
		//  for points which share position but not UV
		//  coordinate (ie lathed objects with a seam where
		//  vertices are duplicated).
		// If we were to search for these degenerate points after 
		//  normalizing the S & T, the averaging of these normalized
		//  (pre-averaged) vectors would not be correct over the
		//  seam, and a discontinuity might appear.

		
		// duplicate_index is array of indices to bins.
		
		typedef vector< vector<DWORD> > tindexmap;
		tindexmap duplicate_index;
		duplicate_index.resize(m_dwNumVertices);

		// Need to search the mesh for vertices with the same spatial coordinate
		// These are vertices duplicated for lathed/wrapped objects to make a
		//   2nd set of texture coordinates at the point in order to avoid bad
		//   texture wrapping
		// In short:  For each vertex, find any other vertices that share it's 
		//   position.  "Average" the tangent space basis calculated above for
		//   these duplicate vertices.  This is not rigorous, but works well 
		//   to fix up authored models.  ** Models should not have T juntions! **

		// Check each vert with every other.  There's no reason to check
		//   j with i after doing i with j, so start j from i ( so we test
		//   1 with 2 but never 2 with 1 again).
		// This is a model pre-processing step and only done once.  For large
		//   models, compute this off-line if you have to and store the resultant
		//   data.
		// The whole thing could be made much more efficient (linked list, etc)

		for( i=0; i < m_dwNumVertices; i++ )
		{
			for(j=i+1; j < m_dwNumVertices; j++ )
			{
				x = pPosition[i].x - pPosition[j].x;
				y = pPosition[i].y - pPosition[j].y;
				z = pPosition[i].z - pPosition[j].z;

				dist = x*x + y*y + z*z;
					
				if( dist < epsilon )
				{
					// if i matches j and k, just record into i.  j will be 
					//  half full as it will only match k, but this is
					//  taken care of when i is processed.
					duplicate_index[i].push_back(j);
				}
			}
		}

		// Now average the tangent spaces & write the new result to
		//  each duplicated vertex
		D3DXVECTOR3	S_temp, T_temp, SxT_temp, N_temp;

		for( i = 0; i < m_dwNumVertices; i++ )
		{
			if(!duplicate_index[i].empty())
			{
				//	FDebug("Averaging vert prop at %d for %d vertices\n", i, duplicate_count[i]);

				// If there are more than 5 vertices sharing this point then
				//  the point is probably some kind of lathe axis node.  No need to
				//  process it here

				// Set accumulator to value for vertex in question

				S_temp		= pSBasis[i];
				T_temp		= pTBasis[i];
				N_temp		= pNormal[i];

				if (pSxTBasis)
					SxT_temp	= pSxTBasis[i];

				// add in basis vectors for all other vertices which
				//  have the same positon (found above)

				for(j=0; j < duplicate_index[i].size(); j++ )
				{
					S_temp		= S_temp + pSBasis[duplicate_index[i][j]];
					T_temp		= T_temp + pTBasis[duplicate_index[i][j]];
					N_temp		= N_temp + pNormal[duplicate_index[i][j]];

					if (pSxTBasis)
					{
						SxT_temp = SxT_temp + pSxTBasis[duplicate_index[i][j]];
					}				
				}

				// Normalize the basis vectors
				// Note that SxT might not be perpendicular to S and T
				//  anymore.  Not absolutely necessary to re-do the 
				//  cross product.
				// Write the average basis to the first vertex for which
				//   the duplicates were found
				D3DXVec3Normalize( & S_temp, & S_temp );
				D3DXVec3Normalize( & T_temp, & T_temp );
				D3DXVec3Normalize( & N_temp, & N_temp );

				pSBasis[i] = S_temp;
				pTBasis[i] = T_temp;

				if (pSxTBasis)
				{
					D3DXVec3Normalize( & SxT_temp, & SxT_temp );
					pSxTBasis[i] = SxT_temp;
				}

				if(dwOption & MeshOption_UnifyNormals)
					pNormal[i] = N_temp;

				// Now write to all later vertices with the same position
					
				for(j=0; j < duplicate_index[i].size(); j++ )
				{
					// Set the vertices in the same position to
					//  the average basis.

					pSBasis[duplicate_index[i][j]] = S_temp;
					pTBasis[duplicate_index[i][j]] = T_temp;
					if (pSxTBasis)
					{
						pSxTBasis[duplicate_index[i][j]] = SxT_temp;
					}

					if(dwOption & MeshOption_UnifyNormals)
						pNormal[duplicate_index[i][j]] = N_temp;


					// Kill the duplicate index lists of all vertices of
					//  higher index which overlap this one.  This is so
					//  higher index vertices do not average a smaller 
					//  subset of bases.
					// Arrays are de-allocated later

					duplicate_index[duplicate_index[i][j]].clear();
				}
			}
		}
	}

    // Calculate the SxT vector
  	for(i = 0; i < m_dwNumVertices; i++)
  	{		
  		// Normalize the S, T vectors
  		D3DXVec3Normalize(&pSBasis[i], &pSBasis[i]);
  		D3DXVec3Normalize(&pTBasis[i], &pTBasis[i]);

		if (pSxTBasis)
		{
			D3DXVECTOR3 NormalizedNormal;

  			// Get the cross of the S and T vectors
  			D3DXVec3Cross(&pSxTBasis[i], &pSBasis[i], &pTBasis[i]);
 			D3DXVec3Normalize(&pSxTBasis[i], &pSxTBasis[i]);


			// Need a normalized normal
  			D3DXVec3Normalize(&NormalizedNormal, &pNormal[i]);

	  		// v coordinates go in opposite direction from the texture v increase in xyz
	 		pTBasis[i] = -pTBasis[i];

			// Get the direction of the SxT vector
  			if (D3DXVec3Dot(&pSxTBasis[i], &NormalizedNormal) < 0.0f)
  			{
  				pSxTBasis[i] = -pSxTBasis[i];
  			}
		}
  	}

	return true;

}

bool NVComplexMesh::CreateFromXFile(LPDIRECT3DDEVICE8 pDevice, const char* fileName)
{
	HRESULT hr;
	LPD3DXBUFFER pMaterials = NULL;
	LPD3DXMESH tempMesh = NULL;
	LPD3DXMESH tempMeshOpt = NULL;
	DWORD dwNumMaterials;
	LPDIRECT3DVERTEXBUFFER8 pVB = NULL;
	LPDIRECT3DINDEXBUFFER8 pIB = NULL;
	
	Release();

	// If there's a path to this file, jump to this directory to ensure correct loading of textures, etc.
	m_strDirectory = fileName;
	std::string::size_type Pos = m_strDirectory.find_last_of("\\", m_strDirectory.size());
	if (Pos != m_strDirectory.npos)
	{
		m_strDirectory = m_strDirectory.substr(0, Pos);
	}										
	else
	{
		m_strDirectory = ".";
	}

	// Load the mesh into system memory
	hr = D3DXLoadMeshFromX(const_cast<char*>(fileName), D3DXMESH_SYSTEMMEM, pDevice, NULL, (ID3DXBuffer**)&pMaterials, &dwNumMaterials, &tempMesh);
	if (FAILED(hr))
		return false;

	// Sort the attributes.
	hr = tempMesh->Optimize(D3DXMESHOPT_ATTRSORT, NULL, NULL, NULL, NULL, &tempMeshOpt);
	if (FAILED(hr))
		return false;

	hr = tempMeshOpt->GetVertexBuffer(&pVB);
	if (FAILED(hr))
	{
		SAFE_RELEASE(m_pBoneNames);
		SAFE_RELEASE(m_pBoneTransforms);
		return false;
	}		

	hr = tempMeshOpt->GetIndexBuffer(&pIB);
	if (FAILED(hr))
	{
		SAFE_RELEASE(pVB);
		SAFE_RELEASE(m_pBoneNames);
		SAFE_RELEASE(m_pBoneTransforms);
		return false;
	}

	bool bSuccess = CreateFromXFileData(pDevice, pVB, pIB, pMaterials, dwNumMaterials, tempMeshOpt);
	
	SAFE_RELEASE(pMaterials);
	SAFE_RELEASE(tempMesh);
	SAFE_RELEASE(tempMeshOpt);
	SAFE_RELEASE(pIB);
	SAFE_RELEASE(pVB);
	
	return bSuccess;
}

bool NVComplexMesh::CreateFromXof(LPDIRECT3DDEVICE8 pDevice, LPDIRECTXFILEDATA pxofobjCur)
{
	HRESULT hr;
	LPD3DXSKINMESH tempSkinMesh = NULL;
	LPD3DXMESH tempMesh = NULL;
	LPD3DXMESH tempMeshOpt = NULL;
	LPD3DXBUFFER pMaterials = NULL;
	LPD3DXBUFFER pAdjacency = NULL;
	DWORD dwNumMaterials;
	LPDIRECT3DVERTEXBUFFER8 pVB = NULL;
	LPDIRECT3DINDEXBUFFER8 pIB = NULL;

	Release();

	std::string strPathSize;
	DWORD dwPathSize = GetCurrentDirectory(0, &m_strDirectory[0]);
	m_strDirectory.resize(dwPathSize);
	GetCurrentDirectory(dwPathSize, &m_strDirectory[0]);

	// Remove the 0 off the string so we can sew in more data
	std::string::size_type Pos = m_strDirectory.find_last_of('\0', m_strDirectory.size());
	if (Pos != m_strDirectory.npos)
	{
		m_strDirectory = m_strDirectory.substr(0, Pos);
	}										

    hr = D3DXLoadSkinMeshFromXof(pxofobjCur, D3DXMESH_SYSTEMMEM, pDevice, &pAdjacency, (ID3DXBuffer**)&pMaterials, &dwNumMaterials,
        &m_pBoneNames, &m_pBoneTransforms, &tempSkinMesh);
    if (FAILED(hr))
        return false;

	m_dwNumBones = tempSkinMesh->GetNumBones();
	if (m_dwNumBones != 0)
	{
		DWORD dwMaxFaceInfluences;
		DWORD dwNumFaces;
		DWORD dwNumBoneCombinations;

		m_dwBonePaletteSize = m_dwDesiredPaletteSize;
        
		tempSkinMesh->GetMaxFaceInfluences(&dwMaxFaceInfluences);
	    dwNumFaces = tempSkinMesh->GetNumFaces();

        // 12 entry palette guarantees that any triangle (4 independent influences per vertex of a tri)
        // can be handled
        dwMaxFaceInfluences = min(dwMaxFaceInfluences, 12);

		// Expand the palette to fit if we have to
		if (m_dwBonePaletteSize < dwMaxFaceInfluences)
			m_dwBonePaletteSize = dwMaxFaceInfluences;

		// PaletteSize is the min of the requested palette size or the number of bones (if there are less bones)
		m_dwBonePaletteSize = min(m_dwBonePaletteSize, m_dwNumBones);

		m_pBoneMatrices = new const D3DXMATRIX*[m_dwNumBones];

		hr = tempSkinMesh->ConvertToIndexedBlendedMesh(D3DXMESH_SYSTEMMEM, (DWORD*)pAdjacency->GetBufferPointer(), m_dwBonePaletteSize, NULL,
			&dwNumBoneCombinations, &m_pBoneCombinations, &tempMesh);
		if (FAILED(hr))
			return false;

		SAFE_RELEASE(tempSkinMesh);
	}
	else
	{
		// Discard the skinning part
		tempSkinMesh->GetOriginalMesh(&tempMesh);
		SAFE_RELEASE(tempSkinMesh);

	}

	// Sort the attributes.
	hr = tempMesh->Optimize(D3DXMESHOPT_ATTRSORT, NULL, NULL, NULL, NULL, &tempMeshOpt);
	if (FAILED(hr))
		return false;
	
	hr = tempMeshOpt->GetVertexBuffer(&pVB);
	if (FAILED(hr))
	{
		SAFE_RELEASE(m_pBoneNames);
		SAFE_RELEASE(m_pBoneTransforms);
		return false;
	}		

	hr = tempMeshOpt->GetIndexBuffer(&pIB);
	if (FAILED(hr))
	{
		SAFE_RELEASE(pVB);
		SAFE_RELEASE(m_pBoneNames);
		SAFE_RELEASE(m_pBoneTransforms);
		return false;
	}

	bool bSuccess = CreateFromXFileData(pDevice, pVB, pIB, pMaterials, dwNumMaterials, tempMeshOpt);
	if (!bSuccess)
	{
		SAFE_RELEASE(m_pBoneNames);
		SAFE_RELEASE(m_pBoneTransforms);
		SAFE_DELETE_ARRAY(m_pBoneMatrices);
	}
	
	SAFE_RELEASE(pMaterials);
	SAFE_RELEASE(tempMesh);
	SAFE_RELEASE(tempMeshOpt);
	SAFE_RELEASE(pIB);
	SAFE_RELEASE(pVB);

	return bSuccess;

}

bool NVComplexMesh::CreateFromXFileData(LPDIRECT3DDEVICE8 pDevice, LPDIRECT3DVERTEXBUFFER8 pVB, LPDIRECT3DINDEXBUFFER8 pIB, LPD3DXBUFFER pMaterials, DWORD dwNumMaterials, LPD3DXMESH pMesh)
{
	DWORD i;
	DWORD dwNumAttributes;
	D3DXATTRIBUTERANGE* pAttributes;
	D3DVERTEXBUFFER_DESC vbdesc;
	D3DINDEXBUFFER_DESC ibdesc;
	DWORD dwTexCount;
	HRESULT hr;

	hr = pMesh->GetAttributeTable(NULL, &dwNumAttributes);
	if (FAILED(hr))
		return false;

	pAttributes = (D3DXATTRIBUTERANGE*)HeapAlloc(GetProcessHeap(), 0, dwNumAttributes * sizeof(D3DXATTRIBUTERANGE));
	hr = pMesh->GetAttributeTable(pAttributes, &dwNumAttributes);

	pIB->GetDesc(&ibdesc);
	pVB->GetDesc(&vbdesc);

	LPD3DXBONECOMBINATION pBoneComb = NULL;
	
	if (m_pBoneCombinations)
		pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(m_pBoneCombinations->GetBufferPointer());

	typedef std::map<DWORD, NVResourceRef> tMapMaterialToResource;
	tMapMaterialToResource MaterialToResource;

	// First store the material data
	for (i = 0; i < dwNumAttributes; i++)
	{
		NVComplexMeshSection Attrib;
		NVMaterialResource* pMaterial;
		DWORD dwMaterialAttrib;
		NVResourceRef Material;

		// The material either matches the attribute, or it is in referred to in the bone combination buffer.
		if (pBoneComb)
		{
			dwMaterialAttrib = pBoneComb[i].AttribId;
		}
		else
		{
			dwMaterialAttrib = i;
		}

		tMapMaterialToResource::const_iterator itrMaterial = MaterialToResource.find(dwMaterialAttrib);
		if (itrMaterial != MaterialToResource.end())
		{
			Material = itrMaterial->second;
			pMaterial = NVMaterialResource::GetMaterialResourcePtr(Material.GetPointer());
		}
		else
		{
			pMaterial = new NVMaterialResource;
			Material = NVRESOURCEMANAGER.AddResource(pMaterial);
			MaterialToResource.insert(tMapMaterialToResource::value_type(dwMaterialAttrib, Material));

			// Create dummy material if there are none in the mesh
			if (!pMaterials)
			{
				// Create named constants for this mesh
				pMaterial->AddValue("diffuse", NVValue(NVV_VECTOR4, D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f)));
				pMaterial->AddValue("power", NVValue(NVV_VECTOR4, D3DXVECTOR4(0.0f, 0.0f, 0.0f, 50.0f)));
			}
			else
			{
				NVASSERT(pMaterials->GetBufferSize() >= (dwNumMaterials * sizeof(D3DXMATERIAL)), "Materials buffer is too small");
				D3DXMATERIAL* pMat = (D3DXMATERIAL*)pMaterials->GetBufferPointer();
			
				// Create named constants for this mesh
				pMaterial->AddValue("diffuse", NVValue(NVV_VECTOR4, &pMat[i].MatD3D.Diffuse));
				pMaterial->AddValue("ambient", NVValue(NVV_VECTOR4, &pMat[i].MatD3D.Ambient));
				pMaterial->AddValue("emissive", NVValue(NVV_VECTOR4, &pMat[i].MatD3D.Emissive));

				// Only add specular if there's a specular power
				if (pMat[i].MatD3D.Power != 0.0f)
				{
					pMaterial->AddValue("power", NVValue(NVV_VECTOR4, D3DXVECTOR4(0.0f, 0.0f, 0.0f, pMat[i].MatD3D.Power)));
					pMaterial->AddValue("specular", NVValue(NVV_VECTOR4, &pMat[i].MatD3D.Specular));
				}

				if (pMat[i].pTextureFilename)
				{
					string strFilePath = m_strDirectory + "\\" + pMat[i].pTextureFilename;
					HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
					if (hFile == INVALID_HANDLE_VALUE) 
					{
						// Try without the appended directory
						strFilePath = pMat[i].pTextureFilename;
						hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
					}
					
					if (hFile != INVALID_HANDLE_VALUE)
					{
						CloseHandle(hFile);
						
						// Add this texture to the resource manager
						NVTextureFileResource* pTexture = new NVTextureFileResource(pDevice, strFilePath);
						pMaterial->AddValue("basemap", NVValue(NVV_RESOURCE, &NVRESOURCEMANAGER.AddResource(pTexture)));
					}
				}			
			}
		}
			
		Attrib.m_dwAttribId = pAttributes[i].AttribId;
		Attrib.m_TriType = NVComplexMeshSection::IndexedTriangleList;
		Attrib.m_dwIndexStart = pAttributes[i].FaceStart * 3;
		Attrib.m_dwIndexCount = pAttributes[i].FaceCount * 3;
		Attrib.m_dwVertexStart = pAttributes[i].VertexStart;
		Attrib.m_dwVertexCount = pAttributes[i].VertexCount;
		Attrib.m_Material = Material;
		m_Sections.push_back(Attrib);
	}

	// Copy the vertex data
	BYTE* pVertex;
	hr = pVB->Lock(0, vbdesc.Size,(BYTE**)&pVertex, 0);

	DWORD dwTextures = ((vbdesc.FVF & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT);
	DWORD dwStride = D3DXGetFVFVertexSize(vbdesc.FVF);

	// Create array pointers.  May be setup depending on contents of mesh
	D3DXVECTOR3* pPosition = NULL;
	D3DXVECTOR3* pNormal = NULL;
	D3DXVECTOR4* pDiffuse = NULL;
	D3DXVECTOR4* pSpecular = NULL;
	vector<D3DXVECTOR2*> Textures;
	vector<D3DXVECTOR4*> BoneIndices;
	vector<D3DXVECTOR4*> BoneWeights;

	NVVertexData* pData = NULL;

	SetNumVertices(vbdesc.Size / dwStride);

	m_dwNumWeights = 0;
	DWORD dwNumBoneQuadruples = 0;
	if (m_pBoneTransforms)
	{
		if ((vbdesc.FVF & D3DFVF_POSITION_MASK) != D3DFVF_XYZ)
		{
			switch(vbdesc.FVF & D3DFVF_POSITION_MASK)
			{
				case D3DFVF_XYZB1:
					m_dwNumWeights = 1;
					break;
				case D3DFVF_XYZB2:
					m_dwNumWeights = 2;
					break;
				case D3DFVF_XYZB3:
					m_dwNumWeights = 3;
					break;
				case D3DFVF_XYZB4:
					m_dwNumWeights = 4;
					break;
				case D3DFVF_XYZB5:
					m_dwNumWeights = 5;
					break;
				default:
					NVASSERT(0, "ERROR: Num weights not known");
					break;
			}
			dwNumBoneQuadruples = (m_dwNumWeights + 3) / 4;
		}
		else
		{
			m_dwNumWeights = 1;
			dwNumBoneQuadruples = 1;
		}
	}

	// Create vertex data in the pass for the geometry we found in the mesh
	if (vbdesc.FVF & D3DFVF_XYZ || m_pBoneTransforms)
	{
		pData = AddVertexData(NVVERT_POSITION, D3DDATATYPE_FLOAT3);
		pPosition = GetDataPointer<D3DXVECTOR3, D3DDATATYPE_FLOAT3>(pData);

		for (i = 0; i < dwNumBoneQuadruples; i++)
		{
			pData = AddVertexData(NVVERT_BONEINDICES0 + i, D3DDATATYPE_FLOAT4);
			BoneIndices.push_back(GetDataPointer<D3DXVECTOR4, D3DDATATYPE_FLOAT4>(pData));

			pData = AddVertexData(NVVERT_BONEWEIGHTS0 + i, D3DDATATYPE_FLOAT4);
			BoneWeights.push_back(GetDataPointer<D3DXVECTOR4, D3DDATATYPE_FLOAT4>(pData));

		}
	}

	// Normal
	if (vbdesc.FVF & D3DFVF_NORMAL)
	{
		pData = AddVertexData(NVVERT_NORMAL, D3DDATATYPE_FLOAT3);
		pNormal = GetDataPointer<D3DXVECTOR3, D3DDATATYPE_FLOAT3>(pData);
	}
	// Diffuse
	if (vbdesc.FVF & D3DFVF_DIFFUSE)
	{
		pData = AddVertexData(NVVERT_DIFFUSE, D3DDATATYPE_FLOAT4);
		pDiffuse = GetDataPointer<D3DXVECTOR4, D3DDATATYPE_FLOAT4>(pData);
	}
	// Specular
	if (vbdesc.FVF & D3DFVF_SPECULAR)
	{
		pData = AddVertexData(NVVERT_SPECULAR, D3DDATATYPE_FLOAT4);
		pSpecular = GetDataPointer<D3DXVECTOR4, D3DDATATYPE_FLOAT4>(pData);
	}
	// Up to 8 texture coordinates
	for (dwTexCount = 0; dwTexCount < dwTextures; dwTexCount++)
	{
		pData = AddVertexData(NVVERT_TEXTURE0 + dwTexCount, D3DDATATYPE_FLOAT2);
		Textures.push_back(GetDataPointer<D3DXVECTOR2, D3DDATATYPE_FLOAT2>(pData));
	}

	for (i = 0; i < (vbdesc.Size / dwStride); i++)
	{
		BYTE* pVert = pVertex;
		
		if (m_dwNumWeights == 0)
		{
			// Position
			if (vbdesc.FVF & D3DFVF_XYZ)
			{
				pPosition[i] = (*(D3DXVECTOR3*)pVert);
				pVert += sizeof(D3DXVECTOR3);
			}
		}
		else
		{
			DWORD dwWeightCount;
			DWORD dwIndexCount;
			DWORD dwQuadCount;

			pPosition[i] = (*(D3DXVECTOR3*)pVert);
			pVert += sizeof(D3DXVECTOR3);

			// Add the weights.  We retrieve the list of floats and pack them
			// into vec4's.  The last weight is 1 - totalweights
			float ftotalweight = 0.0f;
			dwQuadCount = 0;
			dwWeightCount = 0;

			BoneWeights[dwQuadCount][i] = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);

			while (1)
			{
				float fweight;
				bool bLastWeight = false;
				if (dwWeightCount == (m_dwNumWeights - 1))
				{
					fweight = 1.0f - ftotalweight;
					bLastWeight = true;
				}
				else
				{
					fweight = (*(float*)pVert);
					pVert += sizeof(float);
					ftotalweight += fweight;
				}

				if ((dwWeightCount & 0x3) == 0)
				{
					BoneWeights[dwQuadCount][i].x = fweight;
				}
				else if ((dwWeightCount & 0x3) == 1)
				{
					BoneWeights[dwQuadCount][i].y = fweight;
				}
				else if ((dwWeightCount & 0x3) == 2)
				{
					BoneWeights[dwQuadCount][i].z = fweight;
				}
				else if ((dwWeightCount & 0x3) == 3)
				{
					BoneWeights[dwQuadCount][i].w = fweight;
					
					dwQuadCount++;
					
					if (!bLastWeight)
						BoneWeights[dwQuadCount][i] = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
				}

				if (bLastWeight)
					break;

				dwWeightCount++;
			}

			// Add the indices.  We retrieve the list of dwords and pack them
			// into vec4's.
			dwQuadCount = 0;
			dwIndexCount = 0;
			
			// Reset to 0
			BoneIndices[dwQuadCount][i] = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
			
			do
			{
				float fIndex;
				DWORD dwIndexQuad = (*(DWORD*)pVert);
	
				if ((dwIndexCount & 0x3) == 0)
				{
					fIndex = (float)(dwIndexQuad & 0x000000FF) * 3.f;
					BoneIndices[dwQuadCount][i].x = fIndex;
				}
				else if ((dwIndexCount & 0x3) == 1)
				{
					fIndex = (float)((dwIndexQuad & 0x0000FF00) >> 8) * 3.f;
					BoneIndices[dwQuadCount][i].y = fIndex;
				}
				else if ((dwIndexCount & 0x3) == 2)
				{
					fIndex = (float)((dwIndexQuad & 0x00FF0000) >> 16) * 3.f;
					BoneIndices[dwQuadCount][i].z = fIndex;
				}
				else if ((dwIndexCount & 0x3) == 3)
				{
					fIndex = (float)((dwIndexQuad & 0xFF000000) >> 24) * 3.f;
					BoneIndices[dwQuadCount][i].w = fIndex;
			
					if ((dwIndexCount + 1) < m_dwNumWeights)
					{
						dwQuadCount++;
						pVert += sizeof(DWORD);
						BoneIndices[dwQuadCount][i] = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
					}
				}

				dwIndexCount++;

			} while (dwIndexCount < m_dwNumWeights);

			// Make sure we step over the remaining indices.
			pVert += sizeof(DWORD);
		}

		// Normal
		if (vbdesc.FVF & D3DFVF_NORMAL)
		{
			if (pNormal)
				pNormal[i] = (*(D3DXVECTOR3*)pVert);
			
			pVert += sizeof(D3DXVECTOR3);
		}
		// Diffuse
		if (vbdesc.FVF & D3DFVF_DIFFUSE)
		{
			if (pDiffuse)
				pDiffuse[i] = (*(D3DXVECTOR4*)pVert);

			pVert += sizeof(D3DXVECTOR4);
		}
		// Specular
		if (vbdesc.FVF & D3DFVF_SPECULAR)
		{
			if (pSpecular)
				pSpecular[i] = (*(D3DXVECTOR4*)pVert);
			
			pVert += sizeof(D3DXVECTOR4);
		}
		// Up to 8 texture coordinates
		for (dwTexCount = 0; dwTexCount < dwTextures; dwTexCount++)
		{
			if ((Textures.size() > dwTexCount) && Textures[dwTexCount])
				Textures[dwTexCount][i] = (*(D3DXVECTOR2*)pVert);
		
			pVert += sizeof(D3DXVECTOR2);
		}

		pVertex += dwStride;

		// Sanity check to confirm sizes
		NVASSERT(pVert == pVertex, "Vertex size check wrong");
	}
	pVB->Unlock();
	
	// Copy the index data
	BYTE* pIndex;
	hr = pIB->Lock(0, ibdesc.Size,(BYTE**)&pIndex, 0);
	NVASSERT(ibdesc.Format == D3DFMT_INDEX16, "Index buffer should be INDEX16");
	m_Indices.resize(ibdesc.Size / 2);
	memcpy(&m_Indices[0], pIndex, ibdesc.Size);
	pIB->Unlock();

	if (pAttributes)
		HeapFree(GetProcessHeap(), 0, pAttributes);

	return true;
}

bool NVComplexMesh::CreateFromMFile(LPDIRECT3DDEVICE8 pDevice, const char* fileName)
{
	FILE* fp;
	char line[256];
	char* token;
	DWORD dwIndex = 0;
	DWORD dwNumVertices = 0;
	NVVertexData* pData = NULL;

	typedef std::map<DWORD, DWORD> tPositionIndexLookup;
	tPositionIndexLookup PositionIndexLookup;

	if((fp = fopen(fileName, "r")) == NULL)
		return false;

	Release();

	// Create vertex data in for the geometry we find in the mesh
	tNVData_float3* pVec3Data = static_cast<tNVData_float3*>(AddVertexData(NVVERT_POSITION, D3DDATATYPE_FLOAT3));
	if (!pVec3Data)
		return false;

	fgets(line, 256, fp);

	while(!feof(fp))
	{
		if(line[0] == '#')
		{
			//it's a comment
			fgets(line, 256, fp);
			continue;
		}
		else if(line[0] == 'V')
		{
			//it's a vertex
			float x, y, z;
			DWORD dwID;

			//get Vertex
			token = strtok(line, " ");

			//get the number
			token = strtok(NULL, " ");
			dwID = atoi(token);
			
			//get the vertex info
			token = strtok(NULL, " ");
			x = atof(token);

			token = strtok(NULL, " ");
			y = atof(token);

			token = strtok(NULL, " ");
			z = atof(token);

			pVec3Data->push_back(D3DXVECTOR3(x, y, z));
			PositionIndexLookup.insert(tPositionIndexLookup::value_type(dwID, dwIndex++) );
			dwNumVertices++;
		}
		else if(line[0] == 'F')
		{
			DWORD Index[3];
			//it's a face

			//get Face
			token = strtok(line, " ");

			//get number
			token = strtok(NULL, " ");

			//get index data
			token = strtok(NULL, " ");

			Index[0] = atoi(token);

			token = strtok(NULL, " ");
			Index[1] = atoi(token);

			token = strtok(NULL, " ");
			Index[2] = atoi(token);

			tPositionIndexLookup::iterator itrIndex[3];
			itrIndex[0] = PositionIndexLookup.find(Index[0]);
			itrIndex[1] = PositionIndexLookup.find(Index[1]);
			itrIndex[2] = PositionIndexLookup.find(Index[2]);

			// Some files may have faces, but no vertices to match them
			if ((itrIndex[0] != PositionIndexLookup.end()) &&
				(itrIndex[1] != PositionIndexLookup.end()) &&
				(itrIndex[2] != PositionIndexLookup.end()))
			{
				// Add the indices
				m_Indices.push_back(itrIndex[0]->second);
				m_Indices.push_back(itrIndex[1]->second);
				m_Indices.push_back(itrIndex[2]->second);
			}
		}
		else if(line[0] == 'E')
		{
		//edge, do nothing
		}
		else
		{
		}

		fgets(line, 256, fp);	
	}
	fclose(fp);

	if (pVec3Data->empty() || m_Indices.empty())
		return false;

	SetNumVertices(dwNumVertices);

	// Create named constants for this mesh
	NVMaterialResource* pMaterial = new NVMaterialResource;
	pMaterial->AddValue("diffuse", NVValue(NVV_VECTOR4, D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f)));
	pMaterial->AddValue("ambient", NVValue(NVV_VECTOR4, D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f)));
	pMaterial->AddValue("specular", NVValue(NVV_VECTOR4, D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f)));

	// No sections in a .m file
	NVComplexMeshSection Attrib;
	Attrib.m_dwAttribId = 0;
	Attrib.m_dwIndexStart = 0;
	Attrib.m_dwIndexCount = m_Indices.size();
	Attrib.m_dwVertexStart = 0;
	Attrib.m_dwVertexCount = dwNumVertices;
	Attrib.m_Material = NVRESOURCEMANAGER.AddResource(pMaterial);
	m_Sections.push_back(Attrib);

	return true;
}

bool NVComplexMesh::CreateFromSphere(float fRadius, WORD wNumRings, WORD wNumSections, const D3DXVECTOR3& PositionScaling, const D3DXVECTOR2& TextureScaling)
{
    float x, y, z, v, rsintheta; // Temporary variables
    WORD  i, j, n, m;            // counters
    D3DXVECTOR3 vPoint;
	NVVertexData* pVertexData = NULL;
	NVVertexData* pTextureData = NULL;

	Release();

    //Generate space for the required triangles and vertices.
    WORD       wNumTriangles = (wNumRings + 1) * wNumSections * 2;
    DWORD      dwNumIndices   = wNumTriangles*3;
    DWORD      dwNumVertices  = (wNumRings + 1) * wNumSections + 2;
	SetNumVertices(dwNumVertices);

	// Create vertex data for the geometry we find in the mesh
	pVertexData = AddVertexData(NVVERT_POSITION, D3DDATATYPE_FLOAT3);
	if (!pVertexData)
		return false;

	pTextureData = AddVertexData(NVVERT_TEXTURE0, D3DDATATYPE_FLOAT2);
	if (!pTextureData)
		return false;

	D3DXVECTOR3* pPosition = GetDataPointer<D3DXVECTOR3, D3DDATATYPE_FLOAT3>(pVertexData);
	D3DXVECTOR2* pTexture = GetDataPointer<D3DXVECTOR2, D3DDATATYPE_FLOAT2>(pTextureData);

	m_Indices.resize(dwNumIndices);

    // Generate vertices at the top and bottom points.
    D3DXVECTOR3 vTopPoint  = D3DXVECTOR3( 0.0f, +PositionScaling.y*fRadius, 0.0f);
    D3DXVECTOR3 vBotPoint  = D3DXVECTOR3( 0.0f, -PositionScaling.y*fRadius, 0.0f);
    D3DXVECTOR3 vNormal = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

	pPosition[0] = D3DXVECTOR3(vTopPoint.x, vTopPoint.y, vTopPoint.z);
	pTexture[0] = D3DXVECTOR2(0.0f, 0.0f);

	pPosition[dwNumVertices-1] = D3DXVECTOR3(vBotPoint.x, vBotPoint.y, vBotPoint.z);
	pTexture[dwNumVertices-1] = D3DXVECTOR2(0.0f, 0.0f);

    // Generate vertex points for rings
    float dtheta = (float)(D3DX_PI / (wNumRings + 2));     //Angle between each ring
    float dphi   = (float)(2*D3DX_PI / (wNumSections)); //Angle between each section
    float theta  = dtheta;
    n = 1; //vertex being generated, begins at 1 to skip top point

    for( i = 0; i < (wNumRings+1); i++ )
    {
        y = fRadius * (float)cos(theta); // y is the same for each ring
        v = theta / D3DX_PI;     // v is the same for each ring
        rsintheta = fRadius * (float)sin(theta);
        float phi = 0.0f;

        for( j = 0; j < wNumSections; j++ )
        {
            x = rsintheta * (float)sin(phi);
            z = rsintheta * (float)cos(phi);
        
            float u = 1.0f - (float)(phi / (2 * D3DX_PI) );
            NVASSERT(u <= 1.001f, "Invalid texture coord");
			NVASSERT(u >= 0.0f, "Invalid texture coord");
            vPoint        = D3DXVECTOR3(PositionScaling.x * x, PositionScaling.y * y, PositionScaling.z * z );
			pPosition[n] = D3DXVECTOR3(vPoint.x, vPoint.y, vPoint.z);
			pTexture[n] = D3DXVECTOR2(u * TextureScaling.x, v * TextureScaling.y);

            phi += dphi;
            ++n;
        }
        theta += dtheta;
    }

    // Generate triangles for top and bottom caps.
    for( i = 0; i < wNumSections; i++ )
    {
        m_Indices[3*i+0] = 0;
        m_Indices[3*i+1] = i + 1;
        m_Indices[3*i+2] = 1 + ((i + 1) % wNumSections);

        m_Indices[3*(wNumTriangles - wNumSections + i)+0] = (WORD)( dwNumVertices - 1 );
        m_Indices[3*(wNumTriangles - wNumSections + i)+1] = (WORD)( dwNumVertices - 2 - i );
        m_Indices[3*(wNumTriangles - wNumSections + i)+2] = (WORD)( dwNumVertices - 2 - 
                ((1 + i) % wNumSections) );
    }

    // Generate triangles for the rings
    m = 1;            // first vertex in current ring,begins at 1 to skip top point
    n = wNumSections; // triangle being generated, skip the top cap 
        
    for( i = 0; i < wNumRings; i++ )
    {
        for( j = 0; j < (wNumSections); j++ )
        {
            m_Indices[3*n+0] = m + j;
            m_Indices[3*n+1] = m + wNumSections + j;
            m_Indices[3*n+2] = m + wNumSections + ((j + 1) % wNumSections);
            
            m_Indices[3*(n+1)+0] = m_Indices[3*n+0];
            m_Indices[3*(n+1)+1] = m_Indices[3*n+2];
            m_Indices[3*(n+1)+2] = m + ((j + 1) % wNumSections);
            
            n += 2;
        }
      
        m += wNumSections;
    }

	// Create named constants for this mesh
	NVMaterialResource* pMaterial = new NVMaterialResource;
	pMaterial->AddValue("diffuse", NVValue(NVV_VECTOR4, D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f)));
	pMaterial->AddValue("ambient", NVValue(NVV_VECTOR4, D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f)));
	pMaterial->AddValue("specular", NVValue(NVV_VECTOR4, D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f)));
	pMaterial->AddValue("power", NVValue(NVV_VECTOR4, D3DXVECTOR4(0.0f, 0.0f, 0.0f, 30.0f)));
	

	// No sections in a .m file
	NVComplexMeshSection Attrib;
	Attrib.m_dwAttribId = 0;
	Attrib.m_dwIndexStart = 0;
	Attrib.m_dwIndexCount = m_Indices.size();
	Attrib.m_dwVertexStart = 0;
	Attrib.m_dwVertexCount = dwNumVertices;
	Attrib.m_Material = NVRESOURCEMANAGER.AddResource(pMaterial);
	m_Sections.push_back(Attrib);

    return true;
}

bool NVComplexMesh::Translate(const D3DXVECTOR3& Translation)
{
	DWORD i = 0;
	NVVertexData* pPositionData = FindVertexData(NVVERT_POSITION);
	if (!pPositionData || (pPositionData->GetDataType() != D3DDATATYPE_FLOAT3))
	{
		NVASSERT(0, "No position data in mesh, or wrong format");
		return false;
	}

	D3DXVECTOR3* pPosition = GetDataPointer<D3DXVECTOR3, D3DDATATYPE_FLOAT3>(pPositionData);
	while (i < pPositionData->size())
	{
		pPosition[i] += Translation;
		i++;
	}
	return true;
}
bool NVComplexMesh::Scale(float fScale)
{
	DWORD i = 0;
	NVVertexData* pPositionData = FindVertexData(NVVERT_POSITION);
	if (!pPositionData || (pPositionData->GetDataType() != D3DDATATYPE_FLOAT3))
	{
		NVASSERT(0, "No position data in mesh, or wrong format");
		return false;
	}

	D3DXVECTOR3* pPosition = GetDataPointer<D3DXVECTOR3, D3DDATATYPE_FLOAT3>(pPositionData);
	while (i < pPositionData->size())
	{
		pPosition[i] *= fScale;
		i++;
	}
	
	return true;
}

bool NVComplexMesh::Transform(const D3DXMATRIX& Transform)
{
	DWORD i = 0;
	NVVertexData* pPositionData = FindVertexData(NVVERT_POSITION);
	if (!pPositionData || (pPositionData->GetDataType() != D3DDATATYPE_FLOAT3))
	{
		NVASSERT(0, "No position data in mesh, or wrong format");
		return false;
	}

	D3DXVECTOR3* pPosition = GetDataPointer<D3DXVECTOR3, D3DDATATYPE_FLOAT3>(pPositionData);
	while (i < pPositionData->size())
	{
		D3DXVec3TransformCoord(&pPosition[i], &pPosition[i], &Transform);
		i++;
	}
	
	return true;
}


bool NVComplexMesh::ComputeBounds(NVAABounds* pBounds)
{
	pBounds->m_vecCenter = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pBounds->m_fRadius = 0.0f;
	pBounds->m_vecMinExtents = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
	pBounds->m_vecMaxExtents = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	DWORD i = 0;
   	NVVertexData* pPositionData = FindVertexData(NVVERT_POSITION);
   	if (!pPositionData || (pPositionData->GetDataType() != D3DDATATYPE_FLOAT3))
   	{
   		NVASSERT(0, "No position data in mesh or wrong format");
   		return false;
   	}
   
   	D3DXVECTOR3* pPosition = GetDataPointer<D3DXVECTOR3, D3DDATATYPE_FLOAT3>(pPositionData);
	while (i < pPositionData->size())
	{
   		float x = pPosition[i].x;
   		float y = pPosition[i].y;
   		float z = pPosition[i].z;
   		
   		if (pBounds->m_vecMaxExtents.x < x)
   			pBounds->m_vecMaxExtents.x = x;
   
   		if (pBounds->m_vecMaxExtents.y < y)
   			pBounds->m_vecMaxExtents.y = y;
   
   		if (pBounds->m_vecMaxExtents.z < z)
   			pBounds->m_vecMaxExtents.z = z;
   
   		if (pBounds->m_vecMinExtents.x > x)
   			pBounds->m_vecMinExtents.x = x;
   
   		if (pBounds->m_vecMinExtents.y > y)
   			pBounds->m_vecMinExtents.y = y;
   
   		if (pBounds->m_vecMinExtents.z > z)
   			pBounds->m_vecMinExtents.z = z;
   
   		
   		i++;
   	}
   
	pBounds->CalculateCenterRadius();

	return true;
}

// Setup the data streams for fixed function rendering.
bool NVComplexMesh::SetupFVFStream()
{
	m_dwFVF = 0;
	DWORD dwLocation = 0;
	NVVertexData* pVertexData;
	
	pVertexData = FindVertexData(NVVERT_POSITION);
	if (pVertexData)
	{
		pVertexData->SetLocation(dwLocation++);
		m_dwFVF |= D3DFVF_XYZ;
	}

	pVertexData = FindVertexData(NVVERT_NORMAL);
	if (pVertexData)
	{
		pVertexData->SetLocation(dwLocation++);
		m_dwFVF |= D3DFVF_NORMAL;
	}

	pVertexData = FindVertexData(NVVERT_DIFFUSE);
	if (pVertexData)
	{
		pVertexData->SetLocation(dwLocation++);
		m_dwFVF |= D3DFVF_DIFFUSE;
	}

	pVertexData = FindVertexData(NVVERT_SPECULAR);
	if (pVertexData)
	{
		pVertexData->SetLocation(dwLocation++);
		m_dwFVF |= D3DFVF_SPECULAR;
	}

	for (DWORD i = 0; i < NV_MAX_TEXTURES; i++)
	{
		pVertexData = FindVertexData(NVVERT_TEXTURE0 + i);
		if (pVertexData)
		{
			pVertexData->SetLocation(dwLocation++);
			m_dwFVF &= ~D3DFVF_TEXCOUNT_MASK;
			m_dwFVF |= ((i + 1) << D3DFVF_TEXCOUNT_SHIFT);
		}
	}
	
	return true;
}

class DataSorter
{
public:
	bool operator()(const NVVertexData* lhs, const NVVertexData* rhs) const
	{
		if (lhs->GetLocation() < rhs->GetLocation())
			return true;

		return false;
	}
};
// ComplexMeshPass members
// Store internal info about the current set of buffers for this pass.
bool NVComplexMesh::PrepareDeviceObjects(LPDIRECT3DDEVICE8 pDevice)
{
	DWORD i;
	HRESULT hr;
	vector<NVVertexData*> SortedDataList;
	vector<NVVertexData*>::iterator itrDataList;
	
	m_dwStride = 0;

	SAFE_RELEASE(m_pVB);

	tIndexBufferArray::iterator itrIndices = m_IndexBuffers.begin();
	while (itrIndices != m_IndexBuffers.end())
	{
		SAFE_RELEASE(*itrIndices);
		itrIndices++;
	}
	m_IndexBuffers.clear();
	
	tVertexDataMap::iterator itrVertexData = m_VertexData.begin();
	while (itrVertexData != m_VertexData.end())
	{
		NVVertexData* pVertexData = itrVertexData->second;
		if (pVertexData->GetLocation() != NVCOMPLEXMESH_NO_VERTEX)
		{
			SortedDataList.push_back(pVertexData);
			switch(pVertexData->GetDataType())
			{
				case D3DDATATYPE_FLOAT4:
					m_dwStride += sizeof(D3DXVECTOR4);
					break;
				case D3DDATATYPE_FLOAT3:
					m_dwStride += sizeof(D3DXVECTOR3);
					break;
				case D3DDATATYPE_FLOAT2:
					m_dwStride += sizeof(D3DXVECTOR2);
					break;
				default:
					NVASSERT(0, "Unknown vertex data type");
					break;
			}
		}
		itrVertexData++;
	}

	sort(SortedDataList.begin(), SortedDataList.end(), DataSorter());
	
	NVASSERT(m_dwStride != 0, "Stride shouldn't be 0");
	if (m_dwStride == 0)
		return true;

	// Find the first data element
	DWORD dwArraySize = SortedDataList.front()->size();

	// Create a VB to match
	hr = pDevice->CreateVertexBuffer(m_dwStride * dwArraySize, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_pVB);
	if (FAILED(hr))	
	{
		NVASSERT(0, "Failed VB create");
		return false;
	}

	BYTE* pDest;
	DWORD dwIndicesToCreate = m_Indices.size();
	DWORD dwIndicesCreated = 0;
	while(dwIndicesCreated < dwIndicesToCreate)
	{
		DWORD dwNumIndices;
		LPDIRECT3DINDEXBUFFER8 pIB;

		dwNumIndices = dwIndicesToCreate - dwIndicesCreated;
		if (dwNumIndices > NV_MAX_INDICES_IN_BUFFER)
		{
			dwNumIndices = NV_MAX_INDICES_IN_BUFFER;
		}

		// Create an IB to match
		hr = pDevice->CreateIndexBuffer(dwNumIndices * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &pIB);
		if (FAILED(hr))
		{
			NVASSERT(0, "Failed IB create");
			SAFE_RELEASE(m_pVB);
			return false;
		}

		pIB->Lock(0, dwNumIndices * sizeof(WORD), (BYTE**)&pDest, 0);
		memcpy(pDest, &m_Indices[0] + dwIndicesCreated, dwNumIndices * sizeof(WORD));
		pIB->Unlock();

		m_IndexBuffers.push_back(pIB);

		dwIndicesCreated += dwNumIndices;
	}

	m_pVB->Lock(0, m_dwStride * dwArraySize, (BYTE**)&pDest, 0);
	// For each vertex
	for (i = 0; i < dwArraySize; i++)
	{
		// For each data item
		itrDataList = SortedDataList.begin();
		while (itrDataList != SortedDataList.end())
		{
			NVVertexData* pWriteData = *itrDataList;
			NVASSERT(pWriteData->size() == dwArraySize, "Array size mismatch");

			switch(pWriteData->GetDataType())
			{
				case D3DDATATYPE_DWORD:
					*(DWORD*)pDest = *(((DWORD*)pWriteData->GetPointer()) + i);
					pDest += sizeof(DWORD);
					break;
				case D3DDATATYPE_FLOAT4:
					*(D3DXVECTOR4*)pDest = *(((D3DXVECTOR4*)pWriteData->GetPointer()) + i);
					pDest += sizeof(D3DXVECTOR4);
					break;
				case D3DDATATYPE_FLOAT3:
					*(D3DXVECTOR3*)pDest = *(((D3DXVECTOR3*)pWriteData->GetPointer()) + i);
					pDest += sizeof(D3DXVECTOR3);
					break;
				case D3DDATATYPE_FLOAT2:
					*(D3DXVECTOR2*)pDest = *(((D3DXVECTOR2*)pWriteData->GetPointer()) + i);
					pDest += sizeof(D3DXVECTOR2);
					break;
				case D3DDATATYPE_FLOAT1:
					*(float*)pDest = *(((float*)pWriteData->GetPointer()) + i);
					pDest += sizeof(float);
					break;
			}
			itrDataList++;
		}
	}
	m_pVB->Unlock();
	

	return true;
}

NVVertexData* NVComplexMesh::AddVertexData(NVVERTNAME Name, eD3DDATATYPE DataType)
{
	NVVertexData* pData = FindVertexData(Name);
	if (pData)
		return pData;

	std::pair<tVertexDataMap::iterator, bool> Ret;
	switch(DataType)
	{
		case D3DDATATYPE_DWORD:
			Ret = m_VertexData.insert(tVertexDataMap::value_type(Name, new tNVData_dword()));
			break;
		case D3DDATATYPE_FLOAT1:
			Ret = m_VertexData.insert(tVertexDataMap::value_type(Name, new tNVData_float1()));
			break;
		case D3DDATATYPE_FLOAT2:
			Ret = m_VertexData.insert(tVertexDataMap::value_type(Name, new tNVData_float2()));
			break;
		case D3DDATATYPE_FLOAT3:
			Ret = m_VertexData.insert(tVertexDataMap::value_type(Name, new tNVData_float3()));
			break;
		case D3DDATATYPE_FLOAT4:
			Ret = m_VertexData.insert(tVertexDataMap::value_type(Name, new tNVData_float4()));
			break;
		default:
			NVASSERT(0, "Unknown data type!");
			break;
	}

	if (Ret.second == true)
	{
		pData = ((Ret.first)->second);

		// All streams must be the same length
		pData->resize(m_dwNumVertices);
		return pData;
	}

	return NULL;
}

// Find vertex data
NVVertexData* NVComplexMesh::FindVertexData(NVVERTNAME Name)
{
	tVertexDataMap::iterator itrVertex = m_VertexData.find(Name);
	if (itrVertex != m_VertexData.end())
		return itrVertex->second;
	return NULL;
}

bool NVComplexMesh::ClearVertexLocations()
{
	tVertexDataMap::iterator itrVertex = m_VertexData.begin();
	while (itrVertex != m_VertexData.end())
	{
		itrVertex->second->SetLocation(NVCOMPLEXMESH_NO_VERTEX);
		itrVertex++;
	}

	return true;
}

bool NVComplexMesh::Render(LPDIRECT3DDEVICE8 pDevice)
{
	tSectionList::const_iterator itrSections = m_Sections.begin();
	while (itrSections != m_Sections.end())
	{
		const NVComplexMeshSection& Attrib = *itrSections;

		NVMaterialResource* pMaterial = NVMaterialResource::GetMaterialResourcePtr(Attrib.m_Material);
		NVASSERT(pMaterial, "ERROR: No material");

		// Switch to each section's material.  And apply the section's renderstyle
		NVRenderStyleResource* pRenderStyle = NVRenderStyleResource::GetRenderStyleResourcePtr(Attrib.GetRenderStyle().GetPointer());
		NVASSERT(pRenderStyle, "ERROR: No renderstyle");

		for (DWORD dwPass = 0; dwPass < pRenderStyle->GetNumPasses(); dwPass++)
		{
			NVRenderStylePass* pPass = pRenderStyle->GetPass(dwPass);
			if (pPass)
			{
				pPass->ApplyRenderStyle(Attrib.GetMaterial());

				RenderSection(pDevice, itrSections);
				
				pPass->RestoreRenderStyle();
			}
		}
	
		itrSections++;
	}
	return true;
}

void NVComplexMesh::SetRenderStyle(NVResourceRef RenderStyle)
{
	tSectionList::iterator itrSections = m_Sections.begin();
	while (itrSections != m_Sections.end())
	{
		NVComplexMeshSection& Attrib = *itrSections;

		Attrib.SetRenderStyle(RenderStyle);
		
		itrSections++;
	}
	return;
}

bool NVComplexMesh::RenderSection(LPDIRECT3DDEVICE8 pDevice, tSectionList::const_iterator itrSections)
{

	// Can't render without VB's.
	if (m_IndexBuffers.empty() || !m_pVB)
	{
		NVASSERT(0, "IB Empty or no VB");
		return false;
	}

	const NVComplexMeshSection& Attrib = *itrSections;

	if (m_dwNumBones != 0)
	{
		LPD3DXBONECOMBINATION pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(m_pBoneCombinations->GetBufferPointer());
        D3DXMATRIX* pBoneTransforms = reinterpret_cast<D3DXMATRIX*>(m_pBoneTransforms->GetBufferPointer());


		for (DWORD i = 0; i < m_dwBonePaletteSize; ++i)
		{
			DWORD matid = pBoneComb[Attrib.m_dwAttribId].BoneId[i];
			if (matid != UINT_MAX)
			{
				D3DXMATRIX tempMat;
				D3DXMatrixMultiply(&tempMat, &pBoneTransforms[matid], m_pBoneMatrices[matid]);
				
				D3DXMatrixTranspose(&tempMat, &tempMat);
				NVSTATEMANAGER.SetVertexShaderConstant(i * 3, &tempMat, 3);
			}
		}
	}

	// Setup the stream and index buffers for the mesh
	// We don't Save/Restore this data, because it should always be set - there's no 'default' stream
	NVSTATEMANAGER.SetStreamSource(0, m_pVB, m_dwStride);

	if (Attrib.m_TriType == NVComplexMeshSection::TriangleList)
	{
		// Draw it.
		NVASSERT(0, "FIXME: Check counts and triangles/vertices drawn");
		NVSTATEMANAGER.DrawPrimitive(D3DPT_TRIANGLELIST, Attrib.m_dwIndexStart, Attrib.m_dwIndexCount);
		AddTrianglesDrawn(Attrib.m_dwIndexCount);
		AddVerticesDrawn(Attrib.m_dwIndexCount);
	}
	else
	{
		LONG lAttribIndexStart;
		LONG lAttribIndexCount;

		lAttribIndexStart = Attrib.m_dwIndexStart;
		lAttribIndexCount = Attrib.m_dwIndexCount;

		LONG lMaxEntries = (LONG)NV_MAX_INDICES_IN_BUFFER;
		LONG lThisCount;
		tIndexBufferArray::const_iterator itrIndices = m_IndexBuffers.begin();
		while (itrIndices != m_IndexBuffers.end())
		{
			// Can render in this section?
			if (lAttribIndexStart < lMaxEntries)
			{
				break;
			}
			lAttribIndexStart -= lMaxEntries;
			itrIndices++;
		}

		NVASSERT(lAttribIndexStart >= 0, "No attributes");

		while((itrIndices != m_IndexBuffers.end()) && (lAttribIndexCount > 0))
		{
			if ((lMaxEntries - lAttribIndexCount) < 0)
			{
				lThisCount = lMaxEntries;
			}
			else
			{
				lThisCount = lAttribIndexCount;
			}

			NVSTATEMANAGER.SetIndices(*itrIndices, 0);

			if (Attrib.m_TriType == NVComplexMeshSection::IndexedTriangleList)
			{
				// Draw it.
				NVSTATEMANAGER.DrawIndexedPrimitive(D3DPT_TRIANGLELIST, Attrib.m_dwVertexStart, Attrib.m_dwVertexCount, (DWORD)lAttribIndexStart, (DWORD)(lThisCount / 3));
				AddTrianglesDrawn(lThisCount / 3);
				AddVerticesDrawn(lThisCount);
			}
			else if (Attrib.m_TriType == NVComplexMeshSection::IndexedTriangleStrip)
			{
				// Draw it.
				NVSTATEMANAGER.DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, Attrib.m_dwVertexStart, Attrib.m_dwVertexCount, (DWORD)lAttribIndexStart, (DWORD)(lThisCount - 2));
				AddTrianglesDrawn(lThisCount);
				AddVerticesDrawn(lThisCount);
			}
			else
			{
				NVASSERT(0, "Invalid triangle type!");
			}

			lAttribIndexStart = 0;
			lAttribIndexCount -= lThisCount;

			itrIndices++;
		}
	}

	return true;
}

const char* NVComplexMesh::GetBoneName(DWORD i) const
{
	NVASSERT(m_pBoneNames->GetBufferSize() > i, "Invalid bone ID");
	char** pBoneNames = static_cast<char**>(m_pBoneNames->GetBufferPointer()); 
	return pBoneNames[i];
}

void NVComplexMesh::SetBoneMatrixPtr(DWORD i, const D3DXMATRIX* pMat)
{
	NVASSERT(m_dwNumBones > i, "Invalid bone ID");
	m_pBoneMatrices[i] = pMat;
}

}; // nv_objects