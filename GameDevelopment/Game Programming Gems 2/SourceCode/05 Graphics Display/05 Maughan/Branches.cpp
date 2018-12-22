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
#include "nvstatemanager.h"
#include "nvresourcemanager.h"
#include "nvmaterialresource.h"
#include "Branches.h"

using namespace nv_objects;

static const double PI = 3.141592653589;

Branches::Branches(NVResourceID TextureID, int f) 
	: m_BranchTextureID(TextureID), 
		facets(f) 
{
  material.Ambient = D3DXCOLOR(0.5, 0.5, 0.5, 0.5);
  material.Diffuse = D3DXCOLOR(1.0, 1.0, 1.0, 1.0);
  material.Specular = D3DXCOLOR(0.0, 0.0, 0.0, 0.0);
  material.Power = 50.0f;

  geom = new D3DXVECTOR2[facets+1];

  for (int i=0; i<facets+1; ++i) {

    float u = 1.0/facets*i;

    float s = sinf(PI*2.0*u);
    float c = cosf(PI*2.0*u);

    geom[i] = D3DXVECTOR2(c, s);
  }
}


Branches::~Branches() {

  delete geom;
}


int Branches::add(const D3DXVECTOR3& n, const D3DXVECTOR2& tc, const D3DXVECTOR3& v) {

  int index = vertices.size();
  assert((normals.size()==index)&&(texcoords.size()==index));

  vertices.push_back(v);
  normals.push_back(n);
  texcoords.push_back(tc);

  return index;
}

void Branches::addIndex(int i) {

  indecies.push_back(i);
}

void Branches::Transfer(NVComplexMesh* pMesh)
{

	DWORD i;

	NVComplexMeshVertexData* pPosition = pMesh->FindVertexData("position");
	NVComplexMeshVertexData* pNormal = pMesh->FindVertexData("normal");
	NVComplexMeshVertexData* pTexture = pMesh->FindVertexData("texture0");

	tVec3Array& PositionArray = pPosition->GetVec3Array();
	tVec3Array& NormalArray = pNormal->GetVec3Array();
	tVec2Array& TextureArray = pTexture->GetVec2Array();
	tIndexArray& IndexArray = pMesh->GetIndexArray();

	DWORD dwStartVertex = PositionArray.size();
	DWORD dwStartIndex = IndexArray.size();
	for (i = 0; i < vertices.size(); i++)
	{
		PositionArray.push_back(vertices[i]);
		NormalArray.push_back(normals[i]);
		TextureArray.push_back(texcoords[i]);
	}

	int size = indecies.size();
	int stride = (facets+1)*2;
	assert((size%stride)==0);

	NVComplexMeshPass* pPass = pMesh->GetPass(0);
	tSectionList& Sections = pPass->GetSections();

	NVResourceID BranchMaterialID;
	BranchMaterialID = NVRESOURCEMANAGER.FindResourceID("branch_material");

	if (BranchMaterialID == NVINVALID_RESOURCEID)
	{
		NVMaterialResource* pMaterial = new NVMaterialResource();
		pMaterial->AddConstant("ambient", (D3DXVECTOR4&)material.Ambient);
		pMaterial->AddConstant("diffuse", (D3DXVECTOR4&)material.Diffuse);
		pMaterial->AddConstant("specular", (D3DXVECTOR4&)material.Specular);
		pMaterial->m_TextureResourceOriginalID[0] = m_BranchTextureID;
		pMaterial->m_TextureResourceID[0] = m_BranchTextureID;

		D3DXVECTOR4 Power = D3DXVECTOR4(material.Power, material.Power, material.Power, material.Power);
		pMaterial->AddConstant("power", Power);
		BranchMaterialID = NVRESOURCEMANAGER.AddResource(pMaterial, "branch_material");
	}

	DWORD CurrentIndex;
	for (i = 0; i < size; i+= stride)
	{
		if (i == 0)
			CurrentIndex = 0;
		else
			CurrentIndex = 2;
		for (; CurrentIndex < stride; CurrentIndex++)
		{
			IndexArray.push_back(dwStartVertex + indecies[CurrentIndex + i]);
		}

		// Not the last strip?
		// Add 4 degenerate triangles to sew the strips together
		if ((i + stride) < size)
		{
			IndexArray.push_back(dwStartVertex + indecies[CurrentIndex + i - 1]);
			IndexArray.push_back(dwStartVertex + indecies[CurrentIndex + i]);
			IndexArray.push_back(dwStartVertex + indecies[CurrentIndex + i]);
			IndexArray.push_back(dwStartVertex + indecies[CurrentIndex + i + 1]);
		}
	}
	NVComplexMeshSection ThisSection;
	ThisSection.m_dwAttribId = 0;
	ThisSection.m_dwIndexCount = IndexArray.size() - dwStartIndex;
	ThisSection.m_dwIndexStart = dwStartIndex;
	ThisSection.m_dwVertexCount = PositionArray.size() - dwStartVertex;
	ThisSection.m_dwVertexStart = dwStartVertex;
	ThisSection.m_MaterialID = BranchMaterialID;
	ThisSection.m_TriType = NVComplexMeshSection::IndexedTriangleStrip;
	Sections.push_back(ThisSection);


}


int Branches::getPolygonCount() const {

  int stride = (facets+1)*2;
  int cyl_count = indecies.size()/stride;
  return cyl_count * facets * 2;
}


int Branches::getBranchCount() const {

  int stride = (facets+1)*2;
  int cyl_count = indecies.size()/stride;
  return (cyl_count+1) / 2;
}

