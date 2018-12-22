/*********************************************************************NVMH2****
Path:  E:\devrel\NV_SDK_4\DX8\NVEffectsBrowser\Effects\ModelView
File:  shader_ModelView.cpp

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:
A simple shader that can load and display any x file with correct textures/materials.


******************************************************************************/

#include "eb_effect.h"
#include "nvcomplexmesh.h"
#include "nvstatemanager.h"
#include "nvresourcemanager.h"
#include "nvtextureresource.h"
#include "nvtexture.h"
#include "shader_ModelView.h"
#include "DirectionalLight.h"

using namespace nv_objects;
using namespace std;

static NVResourceManager* g_pResourceManager = NULL;
static NVStateManager* g_pStateManager = NULL;
static NVDebug g_Debug(3, "modelview.log");

DECLARE_EFFECT_MAIN()

extern "C"
{
	
	__declspec(dllexport) unsigned int GetNumEffects() { return 1; }
	
	__declspec(dllexport) EBEffect* CreateEffect(unsigned int EffectNum)
	{	
		switch(EffectNum)
		{
		case 0:
			return new CShaderModelView();
			break;
		default:
			return NULL;
		}
	}
	
};

// ------------------------------------------------------------------------------
// CShaderModelView Constructor
//
// Description: Nothing to it -- just straight-forward construction
// ------------------------------------------------------------------------------ 
CShaderModelView::CShaderModelView()
: m_bWireframe(false),
  m_bPause(false),
  m_pUI(NULL),
  m_pMesh(NULL),
  m_dwDirectionalLightShader(0),
  m_dwDirectionalLightBumpShader(0),
  m_bDirectionalBump(false)
{
	m_strEffectLocation = "Tools";
	m_strEffectName = "Model Viewer";
	m_strEffectVertexShader = GetFilePath("DirectionalLight.nvv");
	m_strEffectPixelShader = "";

	m_strFileName = GetFilePath("bigship1.x");

}

// ------------------------------------------------------------------------------
// CShaderModelView Destructor
//
// Description: Nothing to it -- just straight-forward destruction
// ------------------------------------------------------------------------------ 
CShaderModelView::~CShaderModelView()
{
	Free(); 
}

// ------------------------------------------------------------------------------
// CShaderModelView::UpdateProperties
//
// Description: This adds options to the menu (click on "File" or right-click
// on the display.
// ------------------------------------------------------------------------------ 
#define STR_DIRECTIONALBUMP "Directional Bump"
void CShaderModelView::UpdateProperties()
{
	EBEffect::UpdateProperties();

	AddProperty(new EBTriggerProperty("Load another model..."));
	AddProperty(new EBProperty("Wireframe", OBJECT_MEMBER(m_bWireframe), EBTYPE_BOOL_PROP));
	AddProperty(new EBProperty(STR_DIRECTIONALBUMP, OBJECT_MEMBER(m_bDirectionalBump), EBTYPE_BOOL_PROP, PROPFLAG_CALLBACK));
	AddProperty(new EBTriggerProperty("Flip Normals"));


	// Vertex shaders
	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Lighting Shader", GetFilePath("DirectionalLight.nvv"), EBTYPE_STRING_PROP));
}



// ------------------------------------------------------------------------------
// CShaderModelView::ConfirmDevice
//
// Description: Performs caps-bit checking to make sure the selected device 
//		supports this demo.  In this particular case we check for nothing!
// ------------------------------------------------------------------------------ 
HRESULT CShaderModelView::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
{
	return S_OK;
}

// ------------------------------------------------------------------------------
// CShaderModelView::Initialize
//
// Description: Initialize render-states, vertex shader and vertex buffers
// ------------------------------------------------------------------------------ 
HRESULT CShaderModelView::Initialize(IDirect3DDevice8* pDev)
{
	HRESULT hr = S_OK;
	
	// get the D3D device 
	m_pD3DDev = pDev;
	pDev->AddRef();
	
	g_pStateManager = new NVStateManager(pDev);
	g_pResourceManager = new NVResourceManager(pDev);

	//initialize mouse UI
	RECT rect;
	rect.left = rect.top = 0;
	D3DVIEWPORT8 viewport;
	m_pD3DDev->GetViewport(&viewport);
	rect.bottom = viewport.Height;
	rect.right  = viewport.Width;
	m_pUI = new MouseUI((const RECT)rect);
	m_pUI->SetTranslationalSensitivityFactor(0.1f);
	
	
	D3DXVECTOR3 eye, lookAt, up;
	eye.x    = 0.0f; eye.y	  = 0.0f; eye.z	   = -1.5f;
	lookAt.x = 0.0f; lookAt.y = 0.0f; lookAt.z = 0.0f;
	up.x	 = 0.0f; up.y 	  = 1.0f; up.z	   = 0.0f;

	D3DXMatrixLookAtLH(&m_matView, &eye, &lookAt, &up);
	
	D3DXMatrixPerspectiveFovLH(&m_matProjection,
		D3DXToRadian(60.0f),
		1,
		0.1f,
		30.0f);
	

	// Add the bumpmap to the mesh
	NVRESOURCEMANAGER.AddResource(new NVTextureFileResource(m_pD3DDev, GetFilePath("brickb.tga"), NVTextureFileResource::TextureType_2D, D3DX_DEFAULT, D3DX_DEFAULT, 0, 0, 	D3DFMT_A8R8G8B8), "DefaultBumpMap");
	LPDIRECT3DTEXTURE8 pNormalMap = (LPDIRECT3DTEXTURE8)NVTextureResource::GetTexture("DefaultBumpMap");
	if (!pNormalMap)
	{
		m_strLastError = "Failed to load brickb.tga";
		return E_FAIL;
	}

	NVTexture2::FilterNormalMap(m_pD3DDev, pNormalMap);
	NVRESOURCEMANAGER.AddResource(new NVTextureFileResource(m_pD3DDev, GetFilePath("brick.dds")), "DefaultBaseTexture");

	hr = LoadMesh(m_strFileName.c_str());
	if (FAILED(hr))
	{
		m_strLastError = "Failed to load " + m_strFileName;
		return hr;
	}

	//matrices
	SetVertexShaderMatrices();

	NVSTATEMANAGER.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// Global constants
	NVSTATEMANAGER.SetVertexShaderConstant(CV_EYE_VECTOR, D3DXVECTOR4(0.0f, 0.0f, -1.0f, 0.0f), 1);
	NVSTATEMANAGER.SetVertexShaderConstant(CV_HALF, D3DXVECTOR4(0.5f, 0.5f, 0.5f, 0.5f), 1);
	NVSTATEMANAGER.SetVertexShaderConstant(CV_ONE, D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), 1);
	NVSTATEMANAGER.SetVertexShaderConstant(CV_ZERO, D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f), 1);


	return S_OK;
}

HRESULT CShaderModelView::LoadMesh(const string& strFileName)
{
	DWORD i;
	HRESULT hr;
	vector<DWORD> Declaration;
	Declaration.push_back(D3DVSD_STREAM(0));

	int Pos = strFileName.find_last_of('.', strFileName.size());
	string strExtension = strFileName.substr(Pos + 1, strFileName.size() - Pos);

	if (m_pMesh)
	{
		SAFE_DELETE(m_pMesh);
	}

	NVRESOURCEMANAGER.DisposeResources();

	if (m_dwDirectionalLightShader)
	{
		m_pD3DDev->DeleteVertexShader(m_dwDirectionalLightShader);
		m_dwDirectionalLightShader = 0;
	}
	if (m_dwDirectionalLightBumpShader)
	{
		m_pD3DDev->DeleteVertexShader(m_dwDirectionalLightBumpShader);
		m_dwDirectionalLightBumpShader = 0;
	}

	m_pMesh = new NVComplexMesh();

	bool bOK;
	if (strExtension.compare("x") == 0)
	{
		bOK = m_pMesh->CreateFromXFile(m_pD3DDev, strFileName.c_str());
	}
	else if (strExtension.compare("m") == 0)
	{
		bOK = m_pMesh->CreateFromMFile(m_pD3DDev, strFileName.c_str());
	}
	else
	{
		assert(0);
		return E_FAIL;
	}

	if (!bOK)
	{
		m_strLastError = "Failed to load " + m_strFileName;
		MessageBox(NULL, m_strLastError.c_str(), "Error", MB_ICONEXCLAMATION);

		SAFE_DELETE(m_pMesh);
		return E_FAIL;
	}
	m_strFileName = strFileName;

	// We require for the DirectionalLight shader that these components are present.
	// If they are already there, then these calls won't do anything
	if (!m_pMesh->FindVertexData("normal"))
	{
		m_pMesh->AddVertexData("normal", NVCOMPLEXMESH_FLOAT3);
		m_pMesh->GenerateNormals();
	}

	if (!m_pMesh->FindVertexData("texture0"))
	{
		m_bHasTexture = false;
	}
	else
	{
		m_bHasTexture = true;
	}
	m_pMesh->AddVertexData("texture0", NVCOMPLEXMESH_FLOAT2);

	// Add basis vectors.  We might need them.
	m_pMesh->AddVertexData("sbasis0", NVCOMPLEXMESH_FLOAT3);
	m_pMesh->AddVertexData("tbasis0", NVCOMPLEXMESH_FLOAT3);
	m_pMesh->AddVertexData("sxtbasis0", NVCOMPLEXMESH_FLOAT3);
	m_pMesh->GenerateBasis(0);

	// Fill in the required vertex stream locations
	// Here we are doing 2 things.
	// We are setting up our declation to match the streams.
	// We are telling the mesh object where to put each stream (and therefore 'enabling' that stream)
	DWORD dwLocation = 0;
	NVComplexMeshVertexData* pVertexData;
	
	pVertexData = m_pMesh->FindVertexData("position");
	if (pVertexData)
	{
		Declaration.push_back(D3DVSD_REG(CV_POSITION_LOCATION, pVertexData->GetDataType()));
		pVertexData->SetLocation(dwLocation++);
	}

	pVertexData = m_pMesh->FindVertexData("normal");
	if (pVertexData)
	{
		Declaration.push_back(D3DVSD_REG(CV_NORMAL_LOCATION, pVertexData->GetDataType()));
		pVertexData->SetLocation(dwLocation++);
	}

	pVertexData = m_pMesh->FindVertexData("diffuse");
	if (pVertexData)
	{
		Declaration.push_back(D3DVSD_REG(CV_DIFFUSE_LOCATION, pVertexData->GetDataType()));
		pVertexData->SetLocation(dwLocation++);
	}

	pVertexData = m_pMesh->FindVertexData("specular");
	if (pVertexData)
	{
		Declaration.push_back(D3DVSD_REG(CV_SPECULAR_LOCATION, pVertexData->GetDataType()));
		pVertexData->SetLocation(dwLocation++);
	}

	pVertexData = m_pMesh->FindVertexData("sbasis0");
	if (pVertexData)
	{
		Declaration.push_back(D3DVSD_REG(CV_S_LOCATION, pVertexData->GetDataType()));
		pVertexData->SetLocation(dwLocation++);
	}

	pVertexData = m_pMesh->FindVertexData("tbasis0");
	if (pVertexData)
	{
		Declaration.push_back(D3DVSD_REG(CV_T_LOCATION, pVertexData->GetDataType()));
		pVertexData->SetLocation(dwLocation++);
	}

	pVertexData = m_pMesh->FindVertexData("sxtbasis0");
	if (pVertexData)
	{
		Declaration.push_back(D3DVSD_REG(CV_SxT_LOCATION, pVertexData->GetDataType()));
		pVertexData->SetLocation(dwLocation++);
	}

	for (i = 0; i < NV_MAX_TEXTURES; i++)
	{
		string strBuff;
		strBuff.resize(2);
		ultoa(i, &strBuff[0], 2);
		string texname = "texture" + strBuff;
		texname = texname.substr(0, strlen(texname.c_str()));

		pVertexData = m_pMesh->FindVertexData(texname);
		if (pVertexData)
		{
			Declaration.push_back(D3DVSD_REG(CV_TEXTURE0_LOCATION + i, pVertexData->GetDataType()));
			pVertexData->SetLocation(dwLocation++);
		}

	}


	// Finish the decleration and create the shader
	Declaration.push_back(D3DVSD_END());
	hr = LoadAndCreateShader(GetFilePath("DirectionalLight.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwDirectionalLightShader);
	if (FAILED(hr))
		return hr;

	hr = LoadAndCreateShader(GetFilePath("DirectionalLightBump.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwDirectionalLightBumpShader);
	if (FAILED(hr))
		return hr;


	// Get the bounding info and scale the model
	NVComplexMeshBounds Bounds;
	m_pMesh->ComputeBounds(Bounds);
	m_pMesh->Translate(-Bounds.m_vecCenter);
	m_pMesh->Scale(1.0f / Bounds.m_fRadius);

	UpdateMeshMaterials();

	// Prepare the mesh for rendering
	hr = m_pMesh->PrepareDeviceObjects(m_pD3DDev);
	if (FAILED(hr))
	{
		m_strLastError = "Loaded, but can't render " + m_strFileName;
		MessageBox(NULL, m_strLastError.c_str(), "Error", MB_ICONEXCLAMATION);
		SAFE_DELETE(m_pMesh);
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CShaderModelView::UpdateMeshMaterials()
{
	// Get a list of sections.  We assume we only have pass1, because we created the mesh from
	// a mesh file
	tSectionList& Sections = m_pMesh->GetPass(0)->GetSections();
	tSectionList::iterator itrSections = Sections.begin();

	// Walk all the model sections in this pass
	while (itrSections != Sections.end())
	{
		// Set the constants and vertex shaders in this section
		NVMaterialResource* pMaterial = NVMaterialResource::GetMaterialResource(itrSections->GetMaterialID());
		if (pMaterial)
		{
			NVConstant* pConstant;

			// Tell the mesh where to place it's constants
			pConstant = pMaterial->FindConstant("diffuse");
			if (pConstant)
				pConstant->SetLocation(CV_MAT_DIFFUSE);

			pConstant = pMaterial->FindConstant("ambient");
			if (pConstant)
				pConstant->SetLocation(CV_MAT_AMBIENT);
			
			pConstant = pMaterial->FindConstant("specular");
			if (pConstant)
			{
				pMaterial->AddRenderState(D3DRS_SPECULARENABLE, TRUE);
				pConstant->SetLocation(CV_MAT_SPECULAR);
			}
			else
			{
				pMaterial->AddRenderState(D3DRS_SPECULARENABLE, FALSE);
			}
			
			pConstant = pMaterial->FindConstant("power");
			if (pConstant)
				pConstant->SetLocation(CV_MAT_POWER);

			pMaterial->RestoreTexture(0);
			if (m_bHasTexture)
			{
				if (m_bDirectionalBump)
				{
					// Setup the vertex shader
					pMaterial->AddVertexShader(m_dwDirectionalLightBumpShader);

					// Setup texture properties
					pMaterial->AddTexture(0, NVRESOURCEMANAGER.FindResourceID("DefaultBumpMap"));
					pMaterial->AddTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
					pMaterial->AddTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
					pMaterial->AddTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
					pMaterial->AddTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DOTPRODUCT3);
					pMaterial->AddTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
					pMaterial->AddTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
					pMaterial->AddTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
					pMaterial->AddTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
			
					// Stage 1 has the base texture.
					// Modulate the bump output from the previous stage by the current texture
					// Use 2x to brighten it up a bit.
					pMaterial->AddTexture(1, NVRESOURCEMANAGER.FindResourceID("DefaultBaseTexture"));
					pMaterial->AddTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1  );
					pMaterial->AddTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
					pMaterial->AddTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
					pMaterial->AddTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
					pMaterial->AddTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
					pMaterial->AddTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
					pMaterial->AddTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
				}
				else
				{
					pMaterial->AddTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
					pMaterial->AddTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
					pMaterial->AddTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
					pMaterial->AddTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
					pMaterial->AddTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
					pMaterial->AddTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);

					pMaterial->AddVertexShader(m_dwDirectionalLightShader);

					if (NVTextureResource::GetTexture(pMaterial->GetTextureID(0)) != NULL)
					{
						pMaterial->AddTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);	
					}
					else
					{
						pMaterial->AddTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);	
					}

					pMaterial->AddTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
				}
			}
			else
			{
				pMaterial->AddVertexShader(m_dwDirectionalLightShader);

				pMaterial->AddTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
				pMaterial->AddTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				pMaterial->AddTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
				pMaterial->AddTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);

				pMaterial->AddTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
			}

		}
		itrSections++;
	}

	return S_OK;
}

HRESULT CShaderModelView::SetVertexShaderMatrices()
{
	D3DXMATRIX WorldViewProjection, WorldViewIT, World;
	
	D3DXMatrixMultiply(&World, &m_pUI->GetRotationMatrix(), &m_pUI->GetTranslationMatrix());
	D3DXMatrixMultiply(&WorldViewProjection, &World, &m_matView);
	D3DXMatrixInverse(&WorldViewIT, NULL, &WorldViewProjection);

	D3DXMatrixMultiply(&WorldViewProjection, &WorldViewProjection, &m_matProjection);
	D3DXMatrixTranspose(&WorldViewProjection, &WorldViewProjection);
	
	NVStateManager::GetSingleton().SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &WorldViewProjection, 4);
	NVStateManager::GetSingleton().SetVertexShaderConstant(CV_WORLDVIEWIT_0, &WorldViewIT, 4);
	
	// Transform direction vector into eye space
	D3DXVECTOR3 vLight = D3DXVECTOR3(0.3f, 0.0f, 1.0f);
	
	D3DXVECTOR3 vLightToEye;
	D3DXVec3Normalize(&vLightToEye, &vLight);
	D3DXVec3TransformNormal(&vLightToEye, &vLightToEye, &m_matView);
	D3DXVec3Normalize(&vLightToEye, &vLightToEye);

	// Shader math requires vector towards light
	D3DXVECTOR4 vLightEye;
	vLightEye.x = -vLightToEye.x;
	vLightEye.y = -vLightToEye.y;
	vLightEye.z = -vLightToEye.z;
	vLightEye.w = 1.0f;

	NVStateManager::GetSingleton().SetVertexShaderConstant(CV_LIGHT1_DIRECTION, &vLightEye, 1);

	D3DXMatrixTranspose(&World, &World);
	NVStateManager::GetSingleton().SetVertexShaderConstant(CV_WORLD_0, &World, 4);

	return S_OK;
}

void CShaderModelView::PropertyUpdateCallback(const EBProperty* pProperty, bool bWritten)
{
	if (!bWritten)
		return;

	if (pProperty->IsKindOf(EBTYPE_TRIGGER_PROP))
	{
		if (pProperty->GetPropertyName().compare("Flip Normals") == 0)
		{
			if (m_pMesh)
				m_pMesh->FlipNormals();
		
			// Prepare the mesh for rendering
			m_pMesh->PrepareDeviceObjects(m_pD3DDev);

		}
		else
		{
			OPENFILENAME ofn;
			ZeroMemory(&ofn, sizeof(OPENFILENAME));
		
	#define PATH_SIZE 2048
			char FileName[PATH_SIZE] = "";

			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = NULL;
			ofn.hInstance = NULL;
			ofn.lpstrFilter = "Model Files (*.x, *.m)\0*.x;*.m\0\0";
			ofn.lpstrCustomFilter = NULL;
			ofn.nMaxCustFilter = 0;
			ofn.nFilterIndex = 0;
			ofn.lpstrFile = &FileName[0];
			ofn.nMaxFile = PATH_SIZE;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.lpstrTitle = NULL;
			ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
			ofn.nFileOffset = 0;
			ofn.nFileExtension = 0;
			ofn.lpstrDefExt = NULL;

			int iSuccess = GetOpenFileName(&ofn);
			if (iSuccess > 0)
			{
				LoadMesh(&FileName[0]);
			}
			else
			{
				// Might be OK if the user hit cancel
				DWORD dwErr = CommDlgExtendedError();
			}
		}
	}
	else
	{
		if (pProperty->GetPropertyName().compare(STR_DIRECTIONALBUMP) == 0)
		{
			UpdateMeshMaterials();
		}
	}

}

// ------------------------------------------------------------------------------
// CShaderModelView::Free
//
// Description: Called when we switch away from this demo.	Free up all the 
//		memory that is in use.
// ------------------------------------------------------------------------------ 
HRESULT CShaderModelView::Free()
{
	SAFE_DELETE(m_pMesh);

	if (m_pD3DDev)
	{
		if (m_dwDirectionalLightShader)
			m_pD3DDev->DeleteVertexShader(m_dwDirectionalLightShader);
		if (m_dwDirectionalLightBumpShader)
			m_pD3DDev->DeleteVertexShader(m_dwDirectionalLightBumpShader);

		m_dwDirectionalLightShader = 0;
		m_dwDirectionalLightBumpShader = 0;

		SAFE_RELEASE(m_pD3DDev);
	}
	delete m_pUI, m_pUI = 0;

	SAFE_DELETE(g_pStateManager);
	SAFE_DELETE(g_pResourceManager);
	
	return S_OK;
}

// ------------------------------------------------------------------------------
// CShaderModelView::Start
//
// Description: Called to reset
// ------------------------------------------------------------------------------ 
HRESULT CShaderModelView::Start()
{
	return S_OK;
}

// ------------------------------------------------------------------------------
// CShaderModelView::Tick
//
// Description: This function is called every frame.  Here is where we write
//		and update constants, clear the z- and back-buffer and render our 
//		primitives
// ------------------------------------------------------------------------------ 
HRESULT CShaderModelView::Tick(EBTimer* pTimer)
{
	HRESULT hr = S_OK;

	SetVertexShaderMatrices();

	NVSTATEMANAGER.SetRenderState(D3DRS_FILLMODE, (m_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID));

	m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00BFBFBF, 1.0f, 0L);

	if (m_pMesh)
		m_pMesh->Render(m_pD3DDev);

	return hr;
}

void CShaderModelView::MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y)
{
	if(button == MOUSE_LEFTBUTTON)
	{
		if(bDown)
		{
			m_pUI->OnLButtonDown(x, y);
		}
		else
		{
			m_pUI->OnLButtonUp(x, y);
		}
	}
	return;
}

void CShaderModelView::MouseMove(HWND hWnd, int x, int y)
{
	m_pUI->OnMouseMove(x, y);
	return;
}

void CShaderModelView::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
{
	eEBKeyAction Action = TranslateEffectKey(dwKey, nFlags, bDown);

	switch(Action)
	{
		case EB_HELP:
		{
	        MessageBoxEx( NULL, " Help : F1 - Help \n\n Home - Reset To Defaults \n\n W - Wireframe Toggle \n\n Space\\Pause - Toggle Pause/Resume \n\n Left Button & Mouse - Rotate Object\n\n Shift Left Button & Mouse - Pan Camera \n\n Ctrl Left Button & Mouse - Move Camera In & Out\n\n",
			   "Help", MB_ICONINFORMATION | MB_TASKMODAL, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ) );
		}
		break;

		case EB_RESET:
		{
			m_pUI->Reset();
		}
		break;

		case EB_PAUSE:
		{
			m_bPause = !m_bPause;
			m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
		}
		break;

		case EB_WIREFRAME:
		{
			m_bWireframe = !m_bWireframe;
			m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
		
		}
		break;

		default:
			break;
	}
}

