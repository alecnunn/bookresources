/*********************************************************************NVMH2****
Path:  E:\devrel\NV_SDK_4\DX8\NVEffectsBrowser\Effects\LinkerDemo
File:  lightobjects.cpp

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:
Support code to display actual light objects

******************************************************************************/

#include "eb_effect.h"
#include "shader_LinkerDemo.h"
#include "lightobject.h"

using namespace std;
using namespace nv_objects;


void CShaderLinkerDemo::FreeLightObjects()
{
	SAFE_DELETE(m_pArrowMesh);
	SAFE_DELETE(m_pSphereMesh);
	if (m_pD3DDev)
	{
		if (m_dwLightArrowShader)
			m_pD3DDev->DeleteVertexShader(m_dwLightArrowShader);
		m_dwLightArrowShader = 0;

		if (m_dwLightSphereShader)
			m_pD3DDev->DeleteVertexShader(m_dwLightSphereShader);
		m_dwLightSphereShader = 0;
	}
}

float RandPlusMinusPI()
{
	return D3DXToRadian(-360.0f + (720.0f * ((float)rand() / (float)RAND_MAX)));
}

D3DXVECTOR3 RandPlusMinusOneNormalized()
{
	D3DXVECTOR3 vecReturn;
	vecReturn.x = -1.0f + (((float)rand() / (float)RAND_MAX) * 2.0f);
	vecReturn.y = -1.0f + (((float)rand() / (float)RAND_MAX) * 2.0f);
	vecReturn.z = -1.0f + (((float)rand() / (float)RAND_MAX) * 2.0f);
	D3DXVec3Normalize(&vecReturn, &vecReturn);
	return vecReturn;
}

HRESULT CShaderLinkerDemo::UpdateLights()
{
	D3DXVECTOR3 Color;
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Rotation;


	// Add/Remove directional lights
	while(m_DirectionalLights.size() > m_Settings.m_dwNumDirectionalLights)
	{
		m_DirectionalLights.pop_back();
	}

	while(m_DirectionalLights.size() < m_Settings.m_dwNumDirectionalLights)
	{
		CLight NewLight(LIGHT_DIRECTIONAL);
		Position = RandPlusMinusOneNormalized();
		NewLight.SetLightPosition(Position);
		NewLight.SetLightDirection(-Position);

		Color.x = (float)rand() / (float)RAND_MAX;
		Color.y = (float)rand() / (float)RAND_MAX;
		Color.z = (float)rand() / (float)RAND_MAX;
		Color *= 1.3f;
		NewLight.SetLightDiffuse(Color);

		/*Color.x = (float)rand() / (float)RAND_MAX;
		Color.y = (float)rand() / (float)RAND_MAX;
		Color.z = (float)rand() / (float)RAND_MAX;
		Color *= 1.3f;
		*/
		NewLight.SetLightSpecular(Color);

		// Create a rotation axis about the center
		D3DXVECTOR3 Rand;
		Rand.x = (float)rand();
		Rand.y = (float)rand();
		Rand.z = (float)rand();

		D3DXVECTOR3 r1, r2;
		D3DXVec3Cross(&r1, &Position, &Rand);
		D3DXVec3Cross(&r2, &Position, &r1);
		D3DXVec3Normalize(&r1, &r1);
		D3DXVec3Normalize(&r2, &r2);
		
		float CosTheta = cos(RandPlusMinusPI());
		float SinTheta = sin(RandPlusMinusPI());;
		Rotation.x = (r1.x * CosTheta) + (r2.x * SinTheta);
		Rotation.y = (r1.y * CosTheta) + (r2.y * SinTheta);
		Rotation.z = (r1.z * CosTheta) + (r2.z * SinTheta);
		
		NewLight.SetLightRotateAxis(Rotation);

		m_DirectionalLights.push_back(NewLight);
	}

	while(m_PointLights.size() > m_Settings.m_dwNumPointLights)
	{
		m_PointLights.pop_back();
	}

	while(m_PointLights.size() < m_Settings.m_dwNumPointLights)
	{
		CLight NewLight(LIGHT_POINT);

		Position = RandPlusMinusOneNormalized();
		NewLight.SetLightPosition(Position);
		NewLight.SetLightDirection(-Position);

		Color.x = (float)rand() / (float)RAND_MAX;
		Color.y = (float)rand() / (float)RAND_MAX;
		Color.z = (float)rand() / (float)RAND_MAX;
		Color *= 1.3f;
		NewLight.SetLightDiffuse(Color);

		/*
		Color.x = (float)rand() / (float)RAND_MAX;
		Color.y = (float)rand() / (float)RAND_MAX;
		Color.z = (float)rand() / (float)RAND_MAX;
		Color *= 1.3f;
		*/
		NewLight.SetLightSpecular(Color);

		// Create a rotation axis about the center
		D3DXVECTOR3 Rand;
		Rand.x = (float)rand();
		Rand.y = (float)rand();
		Rand.z = (float)rand();

		D3DXVECTOR3 r1, r2;
		D3DXVec3Cross(&r1, &Position, &Rand);
		D3DXVec3Cross(&r2, &Position, &r1);
		D3DXVec3Normalize(&r1, &r1);
		D3DXVec3Normalize(&r2, &r2);
		

		float CosTheta = cos(RandPlusMinusPI());
		float SinTheta = sin(RandPlusMinusPI());;
		Rotation.x = (r1.x * CosTheta) + (r2.x * SinTheta);
		Rotation.y = (r1.y * CosTheta) + (r2.y * SinTheta);
		Rotation.z = (r1.z * CosTheta) + (r2.z * SinTheta);
		
		NewLight.SetLightRotateAxis(Rotation);

		m_PointLights.push_back(NewLight);
	}

	return S_OK;
}

HRESULT CShaderLinkerDemo::DrawLight(CLight& Light)
{
	D3DXMATRIX matWorldViewProjection;
	D3DXMATRIX matTranslate;
	D3DXMATRIX matRotate;
	D3DXMATRIX matScale;

	// Get the light position, and translate it by the world translation to move it with the object
	D3DXMatrixTranslation(&matTranslate, Light.GetLightPosition().x,  Light.GetLightPosition().y,  Light.GetLightPosition().z);

	// Orientation of the light object (the arrow)
	D3DXVECTOR3 vLightObject = D3DXVECTOR3(0.0f, -1.0f, 0.0f);

	// Create the rotation axis
	D3DXVECTOR3 vOrthoNormal;
	D3DXVec3Cross(&vOrthoNormal, &Light.GetLightDirection(), &vLightObject);
	D3DXVec3Normalize(&vOrthoNormal, &vOrthoNormal);

	D3DXVECTOR3 vLightDirection;
	D3DXVec3Normalize(&vLightDirection, &Light.GetLightDirection());

	// Calculate the angle between the two vectors.
	float fAngle = acos(D3DXVec3Dot(&vLightDirection, &vLightObject));

	// Rotate the object about our rotation axis to map one vector onto another
	D3DXMatrixRotationAxis(&matRotate, &vOrthoNormal, -fAngle);

	switch(Light.GetLightType())
	{
		case LIGHT_DIRECTIONAL:
			D3DXMatrixScaling(&matScale, 0.3f, 0.3f, 0.3f);
			break;
		default:
			D3DXMatrixScaling(&matScale, 0.05f, 0.05f, 0.05f);
			break;
	}

	matWorldViewProjection =  matRotate * matScale * matTranslate * m_matView * m_matProjection;
	D3DXMatrixTranspose(&matWorldViewProjection, &matWorldViewProjection);

	// Load up the transform
	NVSTATEMANAGER.SaveVertexShaderConstant(CV_LIGHTOBJECT_WORLDVIEWPROJECTION_0, &matWorldViewProjection, 4);

	NVComplexMesh* pThisMesh;
	switch(Light.GetLightType())
	{
		case LIGHT_DIRECTIONAL:
			pThisMesh = m_pArrowMesh;
			break;
		case LIGHT_POINT:
			pThisMesh = m_pSphereMesh;
			break;
		case LIGHT_SPOT:
			break;
		default:
			assert(0);
			return E_FAIL;
			break;
	}


	tSectionList& Sections = pThisMesh->GetPass(0)->GetSections();
	tSectionList::iterator itrSections = Sections.begin();

	// Walk all the model sections in this pass
	while (itrSections != Sections.end())
	{
		// Get the mesh material
		NVMaterialResource* pMaterial = static_cast<NVMaterialResource*>(NVRESOURCEMANAGER.FindResource((*itrSections).GetMaterialID()));
		NVConstant* pConstant;
		if (pMaterial)
			pConstant = pMaterial->FindConstant("diffuse");

		if (pConstant)
		{
			if ((itrSections == Sections.begin()) && (pThisMesh == m_pSphereMesh))
			{
				// Tell the mesh where to place it's constants
				pConstant->SetConstant(Light.GetLightDiffuse());
			}
			else if (itrSections != Sections.begin())
			{
				pConstant->SetConstant(Light.GetLightDiffuse());
			}
		}
		itrSections++;
	}
	pThisMesh->ResetStats();
	pThisMesh->Render(m_pD3DDev);
	m_dwNumTriangles += pThisMesh->GetTrianglesDrawn();
	m_dwNumVertices += pThisMesh->GetVerticesDrawn();

	NVSTATEMANAGER.RestoreVertexShaderConstant(CV_LIGHTOBJECT_WORLDVIEWPROJECTION_0, 4);

	return S_OK;
}

HRESULT CShaderLinkerDemo::DrawLightObjects()
{
	tLightList::iterator itrLights = m_DirectionalLights.begin();
	while (itrLights != m_DirectionalLights.end())
	{
		DrawLight(*itrLights);	
		itrLights++;
	}
	
	itrLights = m_PointLights.begin();
	while (itrLights != m_PointLights.end())
	{
		DrawLight(*itrLights);	
		itrLights++;
	}

	return S_OK;
}

// Creates the light meshes
HRESULT CShaderLinkerDemo::LoadLightObjects()
{
	HRESULT hr;
	bool bOK;
	vector<DWORD> Declaration;
	Declaration.push_back(D3DVSD_STREAM(0));

	// First create the sphere
	m_pSphereMesh = new NVComplexMesh();
	bOK = m_pSphereMesh->CreateFromSphere(1.0f, 5, 10, D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f));
	if (!bOK)
	{
		SAFE_DELETE(m_pSphereMesh);
		return E_FAIL;
	}

	// Fill in the required vertex stream locations
	// Here we are doing 2 things.
	// We are setting up our declation to match the streams.
	// We are telling the mesh object where to put each stream (and therefore 'enabling' that stream)
	DWORD dwLocation = 0;
	NVComplexMeshVertexData* pVertexData;
	
	pVertexData = m_pSphereMesh->FindVertexData("position");
	if (pVertexData)
	{
		Declaration.push_back(D3DVSD_REG(CV_LIGHTOBJECT_POSITION_LOCATION, pVertexData->GetDataType()));
		pVertexData->SetLocation(0);
	}

	pVertexData = m_pSphereMesh->FindVertexData("texture0");
	if (pVertexData)
	{
		Declaration.push_back(D3DVSD_REG(CV_LIGHTOBJECT_TEXTURE_LOCATION, pVertexData->GetDataType()));
		pVertexData->SetLocation(1);
	}

	// Finish the decleration and create the shader
	Declaration.push_back(D3DVSD_END());

	hr = LoadAndCreateShader(GetFilePath("lightobject.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwLightSphereShader);
	if (FAILED(hr))
		return hr;

	// Get a list of sections.  We assume we only have pass1, because we created the mesh from
	// a mesh file
	tSectionList* pSections = &(m_pSphereMesh->GetPass(0))->GetSections();
	tSectionList::iterator itrSections = pSections->begin();

	// Walk all the model sections in this pass
	while (itrSections != pSections->end())
	{
		// Set the constants and vertex shaders in this section
		NVMaterialResource* pMaterial = static_cast<NVMaterialResource*>(NVRESOURCEMANAGER.FindResource((*itrSections).GetMaterialID()));
		if (pMaterial)
		{
			NVConstant* pConstant;

			// Tell the mesh where to place it's constants
			pConstant = pMaterial->FindConstant("diffuse");
			if (pConstant)
				pConstant->SetLocation(CV_LIGHTOBJECT_DIFFUSE);
			
			// Setup the vertex shader
			pMaterial->AddVertexShader(m_dwLightSphereShader);

			// Renderstates
			pMaterial->AddRenderState(D3DRS_SPECULARENABLE, FALSE);

			// textures
			pMaterial->AddTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pMaterial->AddTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			pMaterial->AddTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
		}
		itrSections++;
	}

	// Get the bounding info and scale the model
	NVComplexMeshBounds Bounds;
	m_pSphereMesh->ComputeBounds(Bounds);
	m_pSphereMesh->Translate(-Bounds.m_vecCenter);
	m_pSphereMesh->Scale(1.0f / Bounds.m_fRadius);

	// Prepare the mesh for rendering
	m_pSphereMesh->PrepareDeviceObjects(m_pD3DDev);

	Declaration.clear();
	Declaration.push_back(D3DVSD_STREAM(0));

	// First create the sphere
	m_pArrowMesh = new NVComplexMesh();
	bOK = m_pArrowMesh->CreateFromXFile(m_pD3DDev, GetFilePath("arrow.x").c_str());
	if (!bOK)
	{
		SAFE_DELETE(m_pArrowMesh);
		return E_FAIL;
	}

	// Fill in the required vertex stream locations
	// Here we are doing 2 things.
	// We are setting up our declation to match the streams.
	// We are telling the mesh object where to put each stream (and therefore 'enabling' that stream)
	dwLocation = 0;
	pVertexData = m_pArrowMesh->FindVertexData("position");
	if (pVertexData)
	{
		Declaration.push_back(D3DVSD_REG(CV_LIGHTOBJECT_POSITION_LOCATION, pVertexData->GetDataType()));
		pVertexData->SetLocation(0);
	}

	m_pArrowMesh->AddVertexData("texture0", NVCOMPLEXMESH_FLOAT2);
	pVertexData = m_pArrowMesh->FindVertexData("texture0");
	if (pVertexData)
	{
		Declaration.push_back(D3DVSD_REG(CV_LIGHTOBJECT_TEXTURE_LOCATION, pVertexData->GetDataType()));
		pVertexData->SetLocation(1);
	}

	// Finish the decleration and create the shader
	Declaration.push_back(D3DVSD_END());

	hr = LoadAndCreateShader(GetFilePath("lightobject.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwLightArrowShader);
	if (FAILED(hr))
		return hr;

	// Get a list of sections.  We assume we only have pass1, because we created the mesh from
	// a mesh file
	pSections = &(m_pArrowMesh->GetPass(0))->GetSections();
	itrSections = pSections->begin();

	// Walk all the model sections in this pass
	while (itrSections != pSections->end())
	{
		// Set the constants and vertex shaders in this section
		NVMaterialResource* pMaterial = static_cast<NVMaterialResource*>(NVRESOURCEMANAGER.FindResource((*itrSections).GetMaterialID()));
		if (pMaterial)
		{
			NVConstant* pConstant;

			// Tell the mesh where to place it's constants
			pConstant = pMaterial->FindConstant("diffuse");
			if (pConstant)
				pConstant->SetLocation(CV_LIGHTOBJECT_DIFFUSE);

			// Setup the vertex shader
			pMaterial->AddVertexShader(m_dwLightArrowShader);

			// Renderstates
			pMaterial->AddRenderState(D3DRS_SPECULARENABLE, FALSE);

			// textures
			pMaterial->AddTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pMaterial->AddTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			pMaterial->AddTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
		}
		itrSections++;
	}

	// Get the bounding info and scale the model
	NVComplexMeshBounds ArrowBounds;
	m_pArrowMesh->ComputeBounds(ArrowBounds);
	m_pArrowMesh->Translate(-ArrowBounds.m_vecCenter);
	m_pArrowMesh->Scale(0.2f / ArrowBounds.m_fRadius);

	// Prepare the mesh for rendering
	m_pArrowMesh->PrepareDeviceObjects(m_pD3DDev);


	return S_OK;
}
