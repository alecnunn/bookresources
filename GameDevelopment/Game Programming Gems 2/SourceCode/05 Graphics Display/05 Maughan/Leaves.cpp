/* Copyright (C) Chris Maughan, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Chris Maughan, 2001"
 */

#include "nvinc.h"
#include "Leaves.h"

using namespace nv_objects;
static const double PI = 3.141592653589;

Leaves::Leaves(NVResourceID LeafFrontTextureID, 
			   NVResourceID LeafBackTextureID,
	       int f, float r)
  : count(0),
    m_LeafFrontTextureID(LeafFrontTextureID),
    m_LeafBackTextureID(LeafBackTextureID),
    facets(f),
    radius(r) 
{

}



void Leaves::addLeaf(const D3DXMATRIX& mat) {

  ++count;

  D3DXVECTOR3 n(1.0f, 0.0f, 0.0f);

  D3DXVECTOR2 t0(1.0f, 0.0f);
  D3DXVECTOR2 t1(0.0f, 0.0f);
  D3DXVECTOR2 t2(0.0f, 1.0f);
  D3DXVECTOR2 t3(1.0f, 1.0f);

  D3DXVECTOR3 v0(0.0f, -1.0f*radius, 0.0f);
  D3DXVECTOR3 v1(0.0f, 1.0f*radius, 0.0f);
  D3DXVECTOR3 v2(0.0f, 1.0f*radius, 2.0f*radius);
  D3DXVECTOR3 v3(0.0f, -1.0f*radius, 2.0f*radius);

  D3DXVec3TransformNormal(&n, &n, &mat);
  normals.push_back(n);

  texcoords.push_back(t2);
  texcoords.push_back(t3);
  texcoords.push_back(t0);
  texcoords.push_back(t1);

  colors.push_back(D3DXVECTOR3(rand(), rand(), rand()));
  colors.push_back(D3DXVECTOR3(rand(), rand(), rand()));
  colors.push_back(D3DXVECTOR3(rand(), rand(), rand()));
  colors.push_back(D3DXVECTOR3(rand(), rand(), rand()));

  D3DXVec3TransformCoord(&v0, &v0, &mat);
  D3DXVec3TransformCoord(&v1, &v1, &mat);
  D3DXVec3TransformCoord(&v2, &v2, &mat);
  D3DXVec3TransformCoord(&v3, &v3, &mat);

  vertices.push_back(v0);
  vertices.push_back(v1);
  vertices.push_back(v2);
  vertices.push_back(v3);

}

void Leaves::Transfer(NVComplexMesh* pMesh)
{
	DWORD i, j, k;

	NVComplexMeshVertexData* pPosition = pMesh->FindVertexData("position");
	NVComplexMeshVertexData* pNormal = pMesh->FindVertexData("normal");
	NVComplexMeshVertexData* pTexture = pMesh->FindVertexData("texture0");

	tVec3Array& PositionArray = pPosition->GetVec3Array();
	tVec3Array& NormalArray = pNormal->GetVec3Array();
	tVec2Array& TextureArray = pTexture->GetVec2Array();
	tIndexArray& IndexArray = pMesh->GetIndexArray();

	m_LeafFrontMaterialID = NVRESOURCEMANAGER.FindResourceID("leaf_front");
	if (m_LeafFrontMaterialID == NVINVALID_RESOURCEID)
	{
		front_material.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		front_material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		front_material.Specular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
		front_material.Power = 50.0f;

		NVMaterialResource* pMaterial = new NVMaterialResource();
		pMaterial->AddConstant("ambient", (D3DXVECTOR4&)front_material.Ambient);
		pMaterial->AddConstant("diffuse", (D3DXVECTOR4&)front_material.Diffuse);
		pMaterial->AddConstant("specular", (D3DXVECTOR4&)front_material.Specular);

		pMaterial->m_TextureResourceOriginalID[0] = m_LeafFrontTextureID;
		pMaterial->m_TextureResourceID[0] = m_LeafFrontTextureID;
		pMaterial->AddRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		pMaterial->AddRenderState(D3DRS_ALPHAREF, 0x7F);
		pMaterial->AddRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

		pMaterial->AddRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pMaterial->AddRenderState(D3DRS_SRCBLEND , D3DBLEND_SRCALPHA);
		pMaterial->AddRenderState(D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA);

		pMaterial->AddRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		pMaterial->AddTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		pMaterial->AddTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		pMaterial->AddTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

		D3DXVECTOR4 Power = D3DXVECTOR4(front_material.Power, front_material.Power, front_material.Power, front_material.Power);
		pMaterial->AddConstant("power", Power);
		m_LeafFrontMaterialID = NVRESOURCEMANAGER.AddResource(pMaterial, "leaf_front");
	}

	m_LeafBackMaterialID = NVRESOURCEMANAGER.FindResourceID("leaf_back");
	if (m_LeafBackMaterialID == NVINVALID_RESOURCEID)
	{
		back_material.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		back_material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		back_material.Specular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
		back_material.Power = 50.0f;

		// Do back leaves
		NVMaterialResource* pMaterial = new NVMaterialResource();
		pMaterial->AddConstant("ambient", (D3DXVECTOR4&)back_material.Ambient);
		pMaterial->AddConstant("diffuse", (D3DXVECTOR4&)back_material.Diffuse);
		pMaterial->AddConstant("specular", (D3DXVECTOR4&)back_material.Specular);

		pMaterial->m_TextureResourceOriginalID[0] = m_LeafBackTextureID;
		pMaterial->m_TextureResourceID[0] = m_LeafBackTextureID;
		pMaterial->AddRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		pMaterial->AddRenderState(D3DRS_ALPHAREF, 0x7F);
		pMaterial->AddRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

		pMaterial->AddRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pMaterial->AddRenderState(D3DRS_SRCBLEND , D3DBLEND_SRCALPHA);
		pMaterial->AddRenderState(D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA);

		pMaterial->AddRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		pMaterial->AddTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		pMaterial->AddTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		pMaterial->AddTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

		D3DXVECTOR4 Power = D3DXVECTOR4(back_material.Power, back_material.Power, back_material.Power, back_material.Power);
		pMaterial->AddConstant("power", Power);
		m_LeafBackMaterialID = NVRESOURCEMANAGER.AddResource(pMaterial, "leaf_back");
	}


	for (int LeafSide = 0; LeafSide < 2; LeafSide++)
	{
		DWORD dwStartVertex = PositionArray.size();
		DWORD dwStartIndex = IndexArray.size();

		DWORD dwCurrentIndexVertex = dwStartVertex;
		for (i=0, j=0; i<count; ++i) 
		{		
			for (k=0; k<4; ++k) 
			{
				if (LeafSide == 0)
				{
					NormalArray.push_back(normals[i]);
				}
				else
				{
					NormalArray.push_back(-normals[i]);
				}

				TextureArray.push_back(texcoords[j+k]);
				PositionArray.push_back(vertices[j+k]);
				/*
				color(colors[j+k]);
				*/
			}
			IndexArray.push_back(dwCurrentIndexVertex + 0);
			IndexArray.push_back(dwCurrentIndexVertex + 1);
			IndexArray.push_back(dwCurrentIndexVertex + 3);
			IndexArray.push_back(dwCurrentIndexVertex + 3);
			IndexArray.push_back(dwCurrentIndexVertex + 1);
			IndexArray.push_back(dwCurrentIndexVertex + 2);

			dwCurrentIndexVertex +=4;

			j += 4;
		}

		NVComplexMeshPass* pPass = pMesh->GetPass(0);
		tSectionList& Sections = pPass->GetSections();


		NVComplexMeshSection ThisSection;
		ThisSection.m_dwAttribId = 0;
		ThisSection.m_dwIndexCount = IndexArray.size() - dwStartIndex;
		ThisSection.m_dwIndexStart = dwStartIndex;
		ThisSection.m_dwVertexCount = PositionArray.size() - dwStartVertex;
		ThisSection.m_dwVertexStart = dwStartVertex;
		ThisSection.m_MaterialID = LeafSide == 0 ? m_LeafFrontMaterialID : m_LeafBackMaterialID;
		ThisSection.m_TriType = NVComplexMeshSection::IndexedTriangleList;
		Sections.push_back(ThisSection);
	}
}

int Leaves::getPolygonCount() const {

  return count * 2 * 2;

  // the last factor of two is because the front and back are being
  // drawn separately
}

int Leaves::getLeafCount() const {

  return count;
}
