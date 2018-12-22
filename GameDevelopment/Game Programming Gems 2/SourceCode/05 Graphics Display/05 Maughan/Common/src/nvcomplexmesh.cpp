/* Copyright (C) Chris Maughan, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Chris Maughan, 2001"
 */
#include "nvcomplexmesh.h"
#include "nvresourcemanager.h"
#include "nvtextureresource.h"
#include <algorithm>
using namespace std;
namespace nv_objects
{

NVComplexMesh::NVComplexMesh()
: m_dwNumVertices(0),
 m_dwStride(0),
 m_pVB(NULL)
{
}

NVComplexMesh::~NVComplexMesh()
{
	Release();
}

void NVComplexMesh::Release()
{
	tMeshPass::iterator itrPasses = m_Passes.begin();
	while(itrPasses != m_Passes.end())
	{
		SAFE_DELETE(*itrPasses);
		itrPasses++;
	}
	m_Passes.clear();

	// Remove the vertex data associated with this mesh
	tVertexDataMap::iterator itrVertex = m_VertexData.begin();
	while(itrVertex != m_VertexData.end())
	{
		NVComplexMeshVertexData& VertexData = itrVertex->second;
		VertexData.Release();
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
}

bool NVComplexMesh::FlipNormals()
{
	NVComplexMeshVertexData* pNormalData = FindVertexData("normal");
	if (!pNormalData)
		return false;

	tVec3Array& Normal = pNormalData->GetVec3Array();
	tVec3Array::iterator itrData;

	// Clear the normals
	itrData = Normal.begin();
	while (itrData != Normal.end())
	{
		*itrData = -*itrData;
		itrData++;
	}

	return true;
}


bool NVComplexMesh::GenerateNormals()
{
	NVComplexMeshVertexData* pPositionData = FindVertexData("position");
	NVComplexMeshVertexData* pNormalData = FindVertexData("normal");

	if (!pPositionData || !pNormalData ||
		(pPositionData->GetDataType() != NVCOMPLEXMESH_FLOAT3) ||
		(pNormalData->GetDataType() != NVCOMPLEXMESH_FLOAT3))
	{
		return false;
	}
	tVec3Array& Position = pPositionData->GetVec3Array();
	tVec3Array& Normal = pNormalData->GetVec3Array();
	tVec3Array::iterator itrData;
	tIndexArray::iterator itrIndices;
	D3DXVECTOR3 norm;

	// Clear the normals
	itrData = Normal.begin();
	while (itrData != Normal.end())
	{
		*itrData = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		itrData++;
	}

	// Generate normals
	itrIndices = m_Indices.begin();
	while (itrIndices != m_Indices.end())
	{
		D3DXVECTOR3 vec1 = Position[*(itrIndices + 0)] - Position[*(itrIndices + 1)];
		D3DXVECTOR3 vec2 = Position[*(itrIndices + 0)] - Position[*(itrIndices + 2)];
				
		D3DXVec3Cross(&norm, &vec1, &vec2);
		Normal[*(itrIndices + 0)] += norm;
		Normal[*(itrIndices + 1)] += norm;
		Normal[*(itrIndices + 2)] += norm;

		itrIndices += 3;
	}

	// Normalize
	itrData = Normal.begin();
	while (itrData != Normal.end())
	{
		// I have no idea why D3DXVec3Normalize failed here with the bunny...
		float Length;
		Length = D3DXVec3Length(&(*itrData));
		*itrData = *itrData / Length;
		
		itrData++;
	}
	return true;
}

bool NVComplexMesh::UnIndex()
{
	NVASSERT(0, "Broken when triangles could be different types per section - FIXME!");
	NVComplexMeshVertexData* pPositionData = FindVertexData("position");

	if (!pPositionData)
		return false;

	if (pPositionData->GetDataType() != NVCOMPLEXMESH_FLOAT3)
		return false;

	tVertexDataMap::iterator itrVertexData = m_VertexData.begin();
	while (itrVertexData != m_VertexData.end())
	{
		((*itrVertexData).second).UnIndex(m_Indices);
		itrVertexData++;
	}

	m_Indices.resize(pPositionData->GetVec3Array().size());
	for (DWORD i = 0; i < pPositionData->GetVec3Array().size(); i++)
	{
		m_Indices[i] = i;
	}

	SetNumVertices(pPositionData->GetVec3Array().size());

	return true;
}


bool NVComplexMesh::GenerateBasis(DWORD dwTexture)
{
	DWORD i;
	NVComplexMeshVertexData* pPositionData = FindVertexData("position");
	NVComplexMeshVertexData* pNormalData = FindVertexData("normal");
	NVComplexMeshVertexData* pTextureData;
	NVComplexMeshVertexData* pSBasisData;
	NVComplexMeshVertexData* pTBasisData;
	NVComplexMeshVertexData* pSxTBasisData;

	// Find the texture data
	string strBuff;
	strBuff.resize(2);
	ultoa(dwTexture, &strBuff[0], 2);
	string texname = "texture" + strBuff;
	texname = texname.substr(0, strlen(texname.c_str()));
	pTextureData = FindVertexData(texname);

	// Find the basis vectors if available
	string sbasis = "sbasis" + strBuff;
	sbasis = sbasis.substr(0, strlen(sbasis.c_str()));
	pSBasisData = FindVertexData(sbasis);

	string tbasis = "tbasis" + strBuff;
	tbasis = tbasis.substr(0, strlen(tbasis.c_str()));
	pTBasisData = FindVertexData(tbasis);

	string sxtbasis = "sxtbasis" + strBuff;
	sxtbasis = sxtbasis.substr(0, strlen(sxtbasis.c_str()));
	pSxTBasisData = FindVertexData(sxtbasis);

	if (!pPositionData || !pTextureData || !pSBasisData || !pTBasisData ||
		(pPositionData->GetDataType() != NVCOMPLEXMESH_FLOAT3) ||
		(pTextureData->GetDataType() != NVCOMPLEXMESH_FLOAT2) ||
		(pSBasisData->GetDataType() != NVCOMPLEXMESH_FLOAT3) || 
		(pTBasisData->GetDataType() != NVCOMPLEXMESH_FLOAT3))
	{
		return false;
	}

	if (pSxTBasisData && !pNormalData)
	{
		// Need normal stream if generating SxTBasis.
		return false;
	}

	tVec3Array& Position = pPositionData->GetVec3Array();
	tVec2Array& Texture = pTextureData->GetVec2Array();
	tVec3Array& Normal = pNormalData->GetVec3Array();
	tVec3Array& SBasis = pSBasisData->GetVec3Array();
	tVec3Array& TBasis = pTBasisData->GetVec3Array();

	// Pick up SxT if needed
	tVec3Array* pSxTBasis = NULL;
	if (pSxTBasisData)
	{
		pSxTBasis = &pSxTBasisData->GetVec3Array();
	}

	tIndexArray::iterator itrIndices;
	
	// Clear the basis vectors
	for (i = 0; i < m_dwNumVertices; i++)
	{
		SBasis[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		TBasis[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
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
		D3DXVECTOR3& v0 = Position[TriIndex[0]];
		D3DXVECTOR3& v1 = Position[TriIndex[1]];
		D3DXVECTOR3& v2 = Position[TriIndex[2]];

		// Texture locations
		D3DXVECTOR2& t0 = Texture[TriIndex[0]];
		D3DXVECTOR2& t1 = Texture[TriIndex[1]];
		D3DXVECTOR2& t2 = Texture[TriIndex[2]];
		
		// Basis locations
		D3DXVECTOR3& sb0 = SBasis[TriIndex[0]];
		D3DXVECTOR3& tb0 = TBasis[TriIndex[0]];
		D3DXVECTOR3& sb1 = SBasis[TriIndex[1]];
		D3DXVECTOR3& tb1 = TBasis[TriIndex[1]];
		D3DXVECTOR3& sb2 = SBasis[TriIndex[2]];
		D3DXVECTOR3& tb2 = TBasis[TriIndex[2]];

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

    // Calculate the SxT vector
  	for(i = 0; i < m_dwNumVertices; i++)
  	{		
  		// Normalize the S, T vectors
  		D3DXVec3Normalize(&SBasis[i], &SBasis[i]);
  		D3DXVec3Normalize(&TBasis[i], &TBasis[i]);

		if (pSxTBasis)
		{
			D3DXVECTOR3 NormalizedNormal;

  			// Get the cross of the S and T vectors
  			D3DXVec3Cross(&(*pSxTBasis)[i], &SBasis[i], &TBasis[i]);
 			D3DXVec3Normalize(&(*pSxTBasis)[i], &(*pSxTBasis)[i]);


			// Need a normalized normal
  			D3DXVec3Normalize(&NormalizedNormal, &Normal[i]);

	  		// v coordinates go in opposite direction from the texture v increase in xyz
	 		TBasis[i] = -TBasis[i];

			// Get the direction of the SxT vector
  			if (D3DXVec3Dot(&(*pSxTBasis)[i], &NormalizedNormal) < 0.0f)
  			{
  				(*pSxTBasis)[i] = -(*pSxTBasis)[i];
  			}
		}
  	}

	return true;

}

bool NVComplexMesh::CreateFromXFile(LPDIRECT3DDEVICE8 pDevice, const char* fileName)
{
	HRESULT hr;
	DWORD i;
	DWORD dwTexCount;
	LPD3DXMESH tempMesh, tempMeshOpt;
	LPD3DXBUFFER pMaterials;
	DWORD dwNumAttributes;
	DWORD dwNumMaterials;
	LPDIRECT3DVERTEXBUFFER8 pVB;
	LPDIRECT3DINDEXBUFFER8 pIB;
	D3DVERTEXBUFFER_DESC vbdesc;
	D3DINDEXBUFFER_DESC ibdesc;
	
	Release();
	m_Passes.push_back(new NVComplexMeshPass(this));

	// If there's a path to this file, jump to this directory to ensure correct loading of textures, etc.
	std::string strDirectory = fileName;
	std::string::size_type Pos = strDirectory.find_last_of("\\", strDirectory.size());
	if (Pos != strDirectory.npos)
	{
		strDirectory = strDirectory.substr(0, Pos);
	}										
	else
	{
		strDirectory = ".";
	}

	// Load the mesh into system memory
	hr = D3DXLoadMeshFromX(const_cast<char*>(fileName), D3DXMESH_SYSTEMMEM, pDevice, NULL, (ID3DXBuffer**)&pMaterials, &dwNumMaterials, &tempMesh);
	if (FAILED(hr))
		return false;

	// Sort the attributes.
	hr = tempMesh->Optimize(D3DXMESHOPT_ATTRSORT, NULL, NULL, NULL, NULL, &tempMeshOpt);
	if (FAILED(hr))
		return false;
	
	hr = tempMeshOpt->GetAttributeTable(NULL, &dwNumAttributes);
	if (FAILED(hr))
		return false;

	D3DXATTRIBUTERANGE* pAttributes = (D3DXATTRIBUTERANGE*)HeapAlloc(GetProcessHeap(), 0, dwNumAttributes * sizeof(D3DXATTRIBUTERANGE));

	hr = tempMeshOpt->GetAttributeTable(pAttributes, &dwNumAttributes);

	hr = tempMeshOpt->GetVertexBuffer(&pVB);
	if (FAILED(hr))
	{
	  	SAFE_RELEASE(tempMesh);
		SAFE_RELEASE(tempMeshOpt);
		SAFE_RELEASE(pMaterials);
		return false;
	}

	hr = tempMeshOpt->GetIndexBuffer(&pIB);
	if (FAILED(hr))
	{
		SAFE_RELEASE(pVB);
		SAFE_RELEASE(pMaterials);
	  	SAFE_RELEASE(tempMesh);
		SAFE_RELEASE(tempMeshOpt);
			
		return false;
	}

	pIB->GetDesc(&ibdesc);
	pVB->GetDesc(&vbdesc);

	// First store the material data
	for (i = 0; i < dwNumMaterials; i++)
	{
		NVComplexMeshSection Attrib;
		NVMaterialResource* pMaterial = new NVMaterialResource;
		NVASSERT(pMaterials->GetBufferSize() >= (dwNumMaterials * sizeof(D3DXMATERIAL)), "Materials buffer is too small");
		D3DXMATERIAL* pMat = (D3DXMATERIAL*)pMaterials->GetBufferPointer();
		
		// Create named constants for this mesh
		pMaterial->AddConstant("diffuse", *(D3DXVECTOR4*)&pMat[i].MatD3D.Diffuse);
		pMaterial->AddConstant("ambient", *(D3DXVECTOR4*)&pMat[i].MatD3D.Ambient);
		pMaterial->AddConstant("emissive", *(D3DXVECTOR4*)&pMat[i].MatD3D.Emissive);

		// Only add specular if there's a specular power
		if (pMat[i].MatD3D.Power != 0.0f)
		{
			pMaterial->AddConstant("power", D3DXVECTOR4(0.0f, 0.0f, 0.0f, pMat[i].MatD3D.Power));
			pMaterial->AddConstant("specular", *(D3DXVECTOR4*)&pMat[i].MatD3D.Specular);
		}

		if (pMat[i].pTextureFilename)
		{
			string strFilePath = strDirectory + "\\" + pMat[i].pTextureFilename;
			HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
			if (hFile == INVALID_HANDLE_VALUE) 
			{
				DISPDBG(0, "Couldn't find: " << strFilePath);
				pMaterial->AddOriginalTexture(0, NVINVALID_RESOURCEID);
			}
			else
			{
				CloseHandle(hFile);

				// Add this texture to the resource manager
				NVTextureFileResource* pTexture = new NVTextureFileResource(pDevice, strDirectory + "\\" + pMat[i].pTextureFilename);
				pMaterial->AddOriginalTexture(0, NVRESOURCEMANAGER.AddResource(pTexture));
			}
			
		}			
		else
		{
			pMaterial->AddOriginalTexture(0, NVINVALID_RESOURCEID);
		}

		Attrib.m_dwAttribId = pAttributes[i].AttribId;
		Attrib.m_TriType = NVComplexMeshSection::IndexedTriangleList;
		Attrib.m_dwIndexStart = pAttributes[i].FaceStart * 3;
		Attrib.m_dwIndexCount = pAttributes[i].FaceCount * 3;
		Attrib.m_dwVertexStart = pAttributes[i].VertexStart;
		Attrib.m_dwVertexCount = pAttributes[i].VertexCount;
		Attrib.m_MaterialID = NVRESOURCEMANAGER.AddResource(pMaterial);
		m_Passes[0]->m_Sections.push_back(Attrib);
	}

	// Copy the vertex data
	BYTE* pVertex;
	hr = pVB->Lock(0, vbdesc.Size,(BYTE**)&pVertex, 0);

	DWORD dwTextures = ((vbdesc.FVF & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT);
	DWORD dwStride = D3DXGetFVFVertexSize(vbdesc.FVF);

	// Create array pointers.  May be setup depending on contents of mesh
	tVec3Array* pPosition = NULL;
	tVec3Array* pNormal = NULL;
	tVec4Array* pDiffuse = NULL;
	tVec4Array* pSpecular = NULL;
	tVec2Array* pTexture[NV_MAX_TEXTURES];
	for (i = 0; i < NV_MAX_TEXTURES; i++)
	{
		pTexture[i] = NULL;
	}

	NVComplexMeshVertexData* pData = NULL;

	SetNumVertices(vbdesc.Size / dwStride);

	// Create vertex data in the pass for the geometry we found in the mesh
	if (vbdesc.FVF & D3DFVF_XYZ)
	{
		pData = AddVertexData("position", NVCOMPLEXMESH_FLOAT3);
		if (pData)
			pPosition = &pData->GetVec3Array();
	}

	// Normal
	if (vbdesc.FVF & D3DFVF_NORMAL)
	{
		pData = AddVertexData("normal", NVCOMPLEXMESH_FLOAT3);
		if (pData)
			pNormal = &pData->GetVec3Array();
	}
	// Diffuse
	if (vbdesc.FVF & D3DFVF_DIFFUSE)
	{
		pData = AddVertexData("diffuse", NVCOMPLEXMESH_FLOAT4);
		if (pData)
			pDiffuse = &pData->GetVec4Array();
	}
	// Specular
	if (vbdesc.FVF & D3DFVF_SPECULAR)
	{
		pData = AddVertexData("specular", NVCOMPLEXMESH_FLOAT4);
		if (pData)
			pSpecular = &pData->GetVec4Array();
	}
	// Up to 8 texture coordinates
	for (dwTexCount = 0; dwTexCount < dwTextures; dwTexCount++)
	{
		string strBuff;
		strBuff.resize(2);
		ultoa(dwTexCount, &strBuff[0], 2);
		string texname = "texture" + strBuff;
		texname = texname.substr(0, strlen(texname.c_str()));

		pData = AddVertexData(texname, NVCOMPLEXMESH_FLOAT2);
		if (pData)
			pTexture[dwTexCount] = &pData->GetVec2Array();
	}

	
	for (i = 0; i < (vbdesc.Size / dwStride); i++)
	{
		BYTE* pVert = pVertex;
		// Position
		if (vbdesc.FVF & D3DFVF_XYZ)
		{
			if (pPosition)
				(*pPosition)[i] = (*(D3DXVECTOR3*)pVert);

			pVert += sizeof(D3DXVECTOR3);
		}
		// Normal
		if (vbdesc.FVF & D3DFVF_NORMAL)
		{
			if (pNormal)
				(*pNormal)[i] = (*(D3DXVECTOR3*)pVert);
			
			pVert += sizeof(D3DXVECTOR3);
		}
		// Diffuse
		if (vbdesc.FVF & D3DFVF_DIFFUSE)
		{
			if (pDiffuse)
				(*pDiffuse)[i] = (*(D3DXVECTOR4*)pVert);

			pVert += sizeof(D3DXVECTOR4);
		}
		// Specular
		if (vbdesc.FVF & D3DFVF_SPECULAR)
		{
			if (pSpecular)
				(*pSpecular)[i] = (*(D3DXVECTOR4*)pVert);
			
			pVert += sizeof(D3DXVECTOR4);
		}
		// Up to 8 texture coordinates
		for (dwTexCount = 0; dwTexCount < dwTextures; dwTexCount++)
		{
			if (pTexture[dwTexCount])
				(*pTexture[dwTexCount])[i] = (*(D3DXVECTOR2*)pVert);
		
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

  	SAFE_RELEASE(tempMesh);
	SAFE_RELEASE(tempMeshOpt);
	SAFE_RELEASE(pMaterials);
	SAFE_RELEASE(pIB);
	SAFE_RELEASE(pVB);
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
	NVComplexMeshVertexData* pData = NULL;

	typedef std::map<DWORD, DWORD> tPositionIndexLookup;
	tPositionIndexLookup PositionIndexLookup;

	if((fp = fopen(fileName, "r")) == NULL)
		return false;

	Release();
	m_Passes.push_back(new NVComplexMeshPass(this));

	// Create vertex data in for the geometry we find in the mesh
	pData = AddVertexData("position", NVCOMPLEXMESH_FLOAT3);
	if (!pData)
		return false;

	tVec3Array& PositionData = pData->GetVec3Array();

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

			PositionData.push_back(D3DXVECTOR3(x, y, z));
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

	if (PositionData.empty() || m_Indices.empty())
		return false;

	SetNumVertices(dwNumVertices);

	// Create named constants for this mesh
	NVMaterialResource* pMaterial = new NVMaterialResource;
	pMaterial->AddConstant("diffuse", D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
	pMaterial->AddConstant("ambient", D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f));
	pMaterial->AddConstant("specular", D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f));

	// No sections in a .m file
	NVComplexMeshSection Attrib;
	Attrib.m_dwAttribId = 0;
	Attrib.m_dwIndexStart = 0;
	Attrib.m_dwIndexCount = m_Indices.size();
	Attrib.m_dwVertexStart = 0;
	Attrib.m_dwVertexCount = dwNumVertices;
	Attrib.m_MaterialID = NVRESOURCEMANAGER.AddResource(pMaterial);
	m_Passes[0]->m_Sections.push_back(Attrib);

	return true;
}

bool NVComplexMesh::CreateFromSphere(float fRadius, WORD wNumRings, WORD wNumSections, const D3DXVECTOR3& PositionScaling, const D3DXVECTOR2& TextureScaling)
{
    float x, y, z, v, rsintheta; // Temporary variables
    WORD  i, j, n, m;            // counters
    D3DXVECTOR3 vPoint;
	NVComplexMeshVertexData* pVertexData = NULL;
	NVComplexMeshVertexData* pTextureData = NULL;

	Release();
	m_Passes.push_back(new NVComplexMeshPass(this));

    //Generate space for the required triangles and vertices.
    WORD       wNumTriangles = (wNumRings + 1) * wNumSections * 2;
    DWORD      dwNumIndices   = wNumTriangles*3;
    DWORD      dwNumVertices  = (wNumRings + 1) * wNumSections + 2;
	SetNumVertices(dwNumVertices);

	// Create vertex data for the geometry we find in the mesh
	pVertexData = AddVertexData("position", NVCOMPLEXMESH_FLOAT3);
	if (!pVertexData)
		return false;

	pTextureData = AddVertexData("texture0", NVCOMPLEXMESH_FLOAT2);
	if (!pTextureData)
		return false;

	tVec3Array& PositionData = pVertexData->GetVec3Array();
	tVec2Array& TextureData = pTextureData->GetVec2Array();

	m_Indices.resize(dwNumIndices);

    // Generate vertices at the top and bottom points.
    D3DXVECTOR3 vTopPoint  = D3DXVECTOR3( 0.0f, +PositionScaling.y*fRadius, 0.0f);
    D3DXVECTOR3 vBotPoint  = D3DXVECTOR3( 0.0f, -PositionScaling.y*fRadius, 0.0f);
    D3DXVECTOR3 vNormal = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

	PositionData[0] = D3DXVECTOR3(vTopPoint.x, vTopPoint.y, vTopPoint.z);
	TextureData[0] = D3DXVECTOR2(0.0f, 0.0f);

	PositionData[dwNumVertices-1] = D3DXVECTOR3(vBotPoint.x, vBotPoint.y, vBotPoint.z);
	TextureData[dwNumVertices-1] = D3DXVECTOR2(0.0f, 0.0f);

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
			PositionData[n] = D3DXVECTOR3(vPoint.x, vPoint.y, vPoint.z);
			TextureData[n] = D3DXVECTOR2(u * TextureScaling.x, v * TextureScaling.y);

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
	pMaterial->AddConstant("diffuse", D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
	pMaterial->AddConstant("ambient", D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f));
	pMaterial->AddConstant("specular", D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f));
	

	// No sections in a .m file
	NVComplexMeshSection Attrib;
	Attrib.m_dwAttribId = 0;
	Attrib.m_dwIndexStart = 0;
	Attrib.m_dwIndexCount = m_Indices.size();
	Attrib.m_dwVertexStart = 0;
	Attrib.m_dwVertexCount = dwNumVertices;
	Attrib.m_MaterialID = NVRESOURCEMANAGER.AddResource(pMaterial);
	m_Passes[0]->m_Sections.push_back(Attrib);

    return true;
}

bool NVComplexMesh::Render(LPDIRECT3DDEVICE8 pDevice)
{
	// Draw each pass.
	DWORD dwPass = 0;
	while (dwPass < m_Passes.size())
	{
		NVComplexMeshPass* pPass = m_Passes[dwPass];
		if (pPass)
		{
			pPass->Render(pDevice);
		}
		dwPass++;
	}
	return true;
}

bool NVComplexMesh::Translate(const D3DXVECTOR3& Translation)
{
	DWORD i = 0;
	NVComplexMeshVertexData* pPositionData = FindVertexData("position");
	if (!pPositionData || (pPositionData->GetDataType() != NVCOMPLEXMESH_FLOAT3))
	{
		NVASSERT(0, "No position data in mesh, or wrong format");
		return false;
	}

	tVec3Array& Position = pPositionData->GetVec3Array();
	while (i < Position.size())
	{
		Position[i] += Translation;
		i++;
	}
	return true;
}
bool NVComplexMesh::Scale(float fScale)
{
	DWORD i = 0;
	NVComplexMeshVertexData* pPositionData = FindVertexData("position");
	if (!pPositionData || (pPositionData->GetDataType() != NVCOMPLEXMESH_FLOAT3))
	{
		NVASSERT(0, "No position data in mesh, or wrong format");
		return false;
	}

	tVec3Array& Position = pPositionData->GetVec3Array();
	while (i < Position.size())
	{
		Position[i] *= fScale;
		i++;
	}
	
	return true;
}

bool NVComplexMesh::Transform(const D3DXMATRIX& Transform)
{
	DWORD i = 0;
	NVComplexMeshVertexData* pPositionData = FindVertexData("position");
	if (!pPositionData || (pPositionData->GetDataType() != NVCOMPLEXMESH_FLOAT3))
	{
		NVASSERT(0, "No position data in mesh, or wrong format");
		return false;
	}

	tVec3Array& Position = pPositionData->GetVec3Array();
	while (i < Position.size())
	{
		D3DXVec3TransformCoord(&Position[i], &Position[i], &Transform);
		i++;
	}
	
	return true;
}
bool NVComplexMesh::ComputeBounds(NVComplexMeshBounds& Bounds)
{
	Bounds.m_vecCenter = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	Bounds.m_fRadius = 0.0f;
	Bounds.m_vecMinExtents = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
	Bounds.m_vecMaxExtents = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	DWORD i = 0;
	NVComplexMeshVertexData* pPositionData = FindVertexData("position");
	if (!pPositionData || (pPositionData->GetDataType() != NVCOMPLEXMESH_FLOAT3))
	{
		NVASSERT(0, "No position data in mesh or wrong format");
		return false;
	}

	tVec3Array& Position = pPositionData->GetVec3Array();

	while (i < Position.size())
	{
		float x = Position[i].x;
		float y = Position[i].y;
		float z = Position[i].z;
		
		if (Bounds.m_vecMaxExtents.x < x)
			Bounds.m_vecMaxExtents.x = x;

		if (Bounds.m_vecMaxExtents.y < y)
			Bounds.m_vecMaxExtents.y = y;

		if (Bounds.m_vecMaxExtents.z < z)
			Bounds.m_vecMaxExtents.z = z;

		if (Bounds.m_vecMinExtents.x > x)
			Bounds.m_vecMinExtents.x = x;

		if (Bounds.m_vecMinExtents.y > y)
			Bounds.m_vecMinExtents.y = y;

		if (Bounds.m_vecMinExtents.z > z)
			Bounds.m_vecMinExtents.z = z;

		
		i++;
	}

	Bounds.m_vecCenter = (Bounds.m_vecMinExtents + Bounds.m_vecMaxExtents) / 2.0f;

	// The bounding radius will either be from the center to the min or the center to the max
	// The max & min distance should be about the same, but I have seen differences, so we will select
	//  the max of the two anyway to ensure we get it right
	D3DXVECTOR3 maxDirection(Bounds.m_vecMaxExtents - Bounds.m_vecCenter);
	D3DXVECTOR3 minDirection(Bounds.m_vecMinExtents - Bounds.m_vecCenter);

	float fRadiusSquared1 = D3DXVec3Dot(&maxDirection, &maxDirection);
	float fRadiusSquared2 = D3DXVec3Dot(&minDirection, &minDirection);
	Bounds.m_fRadius = (fRadiusSquared1 > fRadiusSquared2) ? sqrtf(fRadiusSquared1) : sqrtf(fRadiusSquared2);

	return true;
}

// Setup the data streams for fixed function rendering.
bool NVComplexMesh::SetupFVFStream()
{
	DWORD dwFVF = 0;
	DWORD dwLocation = 0;
	NVComplexMeshVertexData* pVertexData;
	
	pVertexData = FindVertexData("position");
	if (pVertexData)
	{
		pVertexData->SetLocation(dwLocation++);
		dwFVF |= D3DFVF_XYZ;
	}

	pVertexData = FindVertexData("normal");
	if (pVertexData)
	{
		pVertexData->SetLocation(dwLocation++);
		dwFVF |= D3DFVF_NORMAL;
	}

	pVertexData = FindVertexData("diffuse");
	if (pVertexData)
	{
		pVertexData->SetLocation(dwLocation++);
		dwFVF |= D3DFVF_DIFFUSE;
	}

	pVertexData = FindVertexData("specular");
	if (pVertexData)
	{
		pVertexData->SetLocation(dwLocation++);
		dwFVF |= D3DFVF_SPECULAR;
	}

	for (DWORD i = 0; i < NV_MAX_TEXTURES; i++)
	{
		string strBuff;
		strBuff.resize(2);
		ultoa(i, &strBuff[0], 2);
		string texname = "texture" + strBuff;
		texname = texname.substr(0, strlen(texname.c_str()));

		pVertexData = FindVertexData(texname);
		if (pVertexData)
		{
			pVertexData->SetLocation(dwLocation++);
			dwFVF &= ~D3DFVF_TEXCOUNT_MASK;
			dwFVF |= ((i + 1) << D3DFVF_TEXCOUNT_SHIFT);
		}
	}
	

	// Get a list of sections.  We assume we only have pass1 for now
	tSectionList& Sections = GetPass(0)->GetSections();
	tSectionList::iterator itrSections = Sections.begin();

	// Walk all the model sections in this pass
	while (itrSections != Sections.end())
	{
		D3DMATERIAL8 D3DMaterial;
		ZeroMemory(&D3DMaterial, sizeof(D3DMATERIAL8));

		// Set the constants and vertex shaders in this section
		NVMaterialResource* pMaterial = static_cast<NVMaterialResource*>(NVRESOURCEMANAGER.FindResource((*itrSections).GetMaterialID()));
		if (pMaterial)
		{
			NVConstant* pConstant;

			// Tell the mesh where to place it's constants
			pConstant = pMaterial->FindConstant("diffuse");
			if (pConstant)
			{
				D3DMaterial.Diffuse.r = pConstant->GetConstant().x;
				D3DMaterial.Diffuse.g = pConstant->GetConstant().y;
				D3DMaterial.Diffuse.b = pConstant->GetConstant().z;
				D3DMaterial.Diffuse.a = pConstant->GetConstant().w;
			}

			pConstant = pMaterial->FindConstant("ambient");
			if (pConstant)
			{
				D3DMaterial.Ambient.r = pConstant->GetConstant().x;
				D3DMaterial.Ambient.g = pConstant->GetConstant().y;
				D3DMaterial.Ambient.b = pConstant->GetConstant().z;
				D3DMaterial.Ambient.a = pConstant->GetConstant().w;
			}

			pConstant = pMaterial->FindConstant("specular");
			if (pConstant)
			{
				D3DMaterial.Specular.r = pConstant->GetConstant().x;
				D3DMaterial.Specular.g = pConstant->GetConstant().y;
				D3DMaterial.Specular.b = pConstant->GetConstant().z;
				D3DMaterial.Specular.a = pConstant->GetConstant().w;

				pMaterial->AddRenderState(D3DRS_SPECULARENABLE, TRUE);
			}
			else
			{
				pMaterial->AddRenderState(D3DRS_SPECULARENABLE, FALSE);
			}
			
			pConstant = pMaterial->FindConstant("power");
			if (pConstant)
			{
				D3DMaterial.Power = pConstant->GetConstant().w;
			}
						
		    pMaterial->AddD3DMaterial(&D3DMaterial);

		    pMaterial->AddRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
			pMaterial->AddTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pMaterial->AddTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			pMaterial->AddTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
			pMaterial->AddVertexShader(dwFVF);

			if ((pMaterial->GetTextureID(0) != NVINVALID_RESOURCEID) &&
				(NVTextureResource::GetTexture(pMaterial->GetTextureID(0)) != NULL))
			{
				pMaterial->AddTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);	
			}
			else
			{
				pMaterial->AddTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);	
			}
			pMaterial->AddTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		}
		itrSections++;
	}

	return true;
}

class DataSorter
{
public:
	bool operator()(const NVComplexMeshVertexData* lhs, const NVComplexMeshVertexData* rhs) const
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
	vector<NVComplexMeshVertexData*> SortedDataList;
	vector<NVComplexMeshVertexData*>::iterator itrDataList;
	
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
		NVComplexMeshVertexData* pVertexData = &itrVertexData->second;
		if (pVertexData->GetLocation() != NVCOMPLEXMESH_NO_VERTEX)
		{
			SortedDataList.push_back(pVertexData);
			switch(pVertexData->GetDataType())
			{
				case NVCOMPLEXMESH_FLOAT4:
					m_dwStride += sizeof(D3DXVECTOR4);
					break;
				case NVCOMPLEXMESH_FLOAT3:
					m_dwStride += sizeof(D3DXVECTOR3);
					break;
				case NVCOMPLEXMESH_FLOAT2:
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
	DWORD dwArraySize = 0;
	switch(SortedDataList.front()->GetDataType())
	{
		case NVCOMPLEXMESH_FLOAT4:
			dwArraySize = ((SortedDataList.front())->GetVec4Array()).size();
			break;
		case NVCOMPLEXMESH_FLOAT3:
			dwArraySize = ((SortedDataList.front())->GetVec3Array()).size();
			break;
		case NVCOMPLEXMESH_FLOAT2:
			dwArraySize = ((SortedDataList.front())->GetVec2Array()).size();
			break;
		default:
			NVASSERT(0, "Unknown vertex data format");
			break;
	}

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
			NVComplexMeshVertexData* pWriteData = *itrDataList;
			
			switch(pWriteData->GetDataType())
			{
				case NVCOMPLEXMESH_FLOAT4:
					NVASSERT(pWriteData->GetVec4Array().size() == dwArraySize, "Array size mismatch");
					*(D3DXVECTOR4*)pDest = pWriteData->GetVec4Array()[i];
					pDest += sizeof(D3DXVECTOR4);
					break;
				case NVCOMPLEXMESH_FLOAT3:
					NVASSERT(pWriteData->GetVec3Array().size() == dwArraySize, "Array size mismatch");
					*(D3DXVECTOR3*)pDest = pWriteData->GetVec3Array()[i];
					pDest += sizeof(D3DXVECTOR3);
					break;
				case NVCOMPLEXMESH_FLOAT2:
					NVASSERT(pWriteData->GetVec2Array().size() == dwArraySize, "Array size mismatch");
					*(D3DXVECTOR2*)pDest = pWriteData->GetVec2Array()[i];
					pDest += sizeof(D3DXVECTOR2);
					break;
			}
			itrDataList++;
		}
	}
	m_pVB->Unlock();
	

	return true;
}

NVComplexMeshVertexData* NVComplexMesh::AddVertexData(const std::string& strName, eNVComplexMeshVertexDataType DataType, DWORD dwLocation)
{
	NVComplexMeshVertexData* pData = FindVertexData(strName);
	if (pData)
		return pData;

	std::pair<tVertexDataMap::iterator, bool> Ret = m_VertexData.insert(tVertexDataMap::value_type(strName, NVComplexMeshVertexData(DataType, dwLocation)));
	if (Ret.second == true)
	{
		pData = &((Ret.first)->second);

		// All streams must be the same length
		switch(pData->GetDataType())
		{
			case NVCOMPLEXMESH_FLOAT4:
				pData->GetVec4Array().resize(m_dwNumVertices);
				break;
			case NVCOMPLEXMESH_FLOAT3:
				pData->GetVec3Array().resize(m_dwNumVertices);
				break;
			case NVCOMPLEXMESH_FLOAT2:
				pData->GetVec2Array().resize(m_dwNumVertices);
				break;
		}

		return pData;
	}

	return NULL;
}

// Find vertex data
NVComplexMeshVertexData* NVComplexMesh::FindVertexData(const std::string& strName)
{
	tVertexDataMap::iterator itrVertex = m_VertexData.find(strName);
	if (itrVertex != m_VertexData.end())
		return &itrVertex->second;
	return NULL;
}

NVComplexMeshPass* NVComplexMesh::DuplicatePass(DWORD dwSource, DWORD dwDest)
{
	if (m_Passes.size() <= dwSource)
		return false;

	if (m_Passes.size() <= dwDest)
	{
		m_Passes.resize(dwDest + 1);
	}

	if (m_Passes[dwDest] != NULL)
	{
		*m_Passes[dwDest] = *m_Passes[dwSource];
	}
	else
	{
		// Allocate new pass and copy it
		m_Passes[dwDest] = new NVComplexMeshPass(*m_Passes[dwSource]);
	}

	typedef map<NVResourceID, NVResourceID> tMapMaterialToNewMaterial;
	tMapMaterialToNewMaterial mapMat;
	
	tSectionList& Sections = m_Passes[dwDest]->GetSections();
	tSectionList::iterator itrSections = Sections.begin();

	// Walk all the model sections in this pass
	while (itrSections != Sections.end())
	{
		NVResourceID MaterialID = (*itrSections).GetMaterialID();
		// Set the constants and vertex shaders in this section
		if (MaterialID != NVINVALID_RESOURCEID)
		{
			// Find this material in our mapping
			tMapMaterialToNewMaterial::iterator itrMaterials = mapMat.find(MaterialID);

			// If there, then simply remap the sections material to the new material
			if (itrMaterials != mapMat.end())
			{
				(*itrSections).SetMaterialID(itrMaterials->second);
			}
			// Otherwise create a new material, add it to the map, and map the section to it.
			else
			{
				NVMaterialResource* pMaterialNew = new NVMaterialResource(*static_cast<NVMaterialResource*>(NVRESOURCEMANAGER.FindResource(MaterialID)));
				NVResourceID NewMaterialID = NVRESOURCEMANAGER.AddResource(pMaterialNew);
				mapMat[MaterialID] = NewMaterialID;
				(*itrSections).SetMaterialID(NewMaterialID);
			}
		}
		itrSections++;
	}
	return m_Passes[dwDest];
}

}; // nv_objects