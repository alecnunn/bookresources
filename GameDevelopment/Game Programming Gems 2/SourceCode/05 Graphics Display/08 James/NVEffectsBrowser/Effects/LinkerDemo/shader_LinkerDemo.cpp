/*********************************************************************NVMH2****
Path:  E:\devrel\NV_SDK_4\DX8\NVEffectsBrowser\Effects\LinkerDemo
File:  shader_LinkerDemo.cpp

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/

#include "eb_effect.h"
#include "shader_LinkerDemo.h"
#include "constants.h"
#include "nvlink.h"
#include "linkercommon.h"
#include "lightobject.h"
#include "nvtextureresource.h"
#include "nvtexture.h"

using namespace nv_objects;
using namespace std;

// State manager and resource manager are singletons.
// This just means that there is only one instance of them in the system.
// Access is controlled by the call to NVStateManager::GetSingleton().
// The singleton is valid when it is created.
NVStateManager* g_pNVStateManager = NULL;
NVResourceManager* g_pNVResourceManager = NULL;


// A debug object
#ifdef _DEBUG
NVDebug g_DebugOutput(3, "linkerdemo.log");
#endif


DECLARE_EFFECT_MAIN()

extern "C"
{
	
	__declspec(dllexport) unsigned int GetNumEffects() { return 1; }
	
	__declspec(dllexport) EBEffect* CreateEffect(unsigned int EffectNum)
	{	
		switch(EffectNum)
		{
		case 0:
			return new CShaderLinkerDemo();
			break;
		default:
			return NULL;
		}
	}
	
};

static const D3DXVECTOR3 EyeStart(0.0f, 0.0f, -1.4f);

// ------------------------------------------------------------------------------
// CShaderLinkerDemo Constructor
//
// Description: Nothing to it -- just straight-forward construction
// ------------------------------------------------------------------------------ 
CShaderLinkerDemo::CShaderLinkerDemo()
: m_bWireframe(false),
  m_pUI(NULL),
  m_pMesh(NULL),
  m_pArrowMesh(NULL),
  m_pSphereMesh(NULL),
  m_bPause(false),
  m_dwPass0Shader(0),
  m_dwPass1Shader(0),
  m_pLinker(NULL),
  m_fAngle(0.0f),
  m_bSetShaderToShow(false),
  m_pWorldBoxVertices(NULL),
  m_pWorldBoxIndices(NULL),
  m_dwBumpPixelShader(0),
  m_hLinker(NULL),
  m_hDialog(NULL),
  m_fBumpScale(1.0f),
  m_AmbientLight(DefaultAmbient),
  m_eDisplayOption(DISPLAY_MODEL),
  m_GeneratedNormalMapID(NVINVALID_RESOURCEID)
{
	m_strEffectLocation = "Tools";
	m_strEffectName = "NVLink Vertex Shader Combiner";
	m_strEffectVertexShader = GetFilePath("miscvertex.nvf");
	m_strEffectPixelShader = "";

	m_strFileName = GetFilePath("bigship1.x");

	// Ensure we have an output file
	std::string strOutFile;
	strOutFile = effect_api::GetModulePath() + "\\" + "output.nvv";
	HANDLE hFile = CreateFile(strOutFile.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	CloseHandle(hFile);

	ZeroMemory(&m_DialogFunc, sizeof(LinkerDialogFunc));

	// Default setup
	ZeroMemory(&m_Settings, sizeof(DialogSettings));
	m_Settings.m_dwNumPointLights = 3;
	m_Settings.m_dwNumDirectionalLights = 2;
	m_Settings.m_bEyeSpace = FALSE;
	m_Settings.m_bLocalViewer = FALSE;
	m_Settings.m_bSpecular = TRUE;
	m_Settings.m_bSpecularRDotL = TRUE;
	m_Settings.m_bLighting = TRUE;
	m_Settings.m_bTextures = TRUE;
	m_Settings.m_TexGen = TEXGEN_BLINNBUMPREFLECTION;
	m_Settings.m_Fog = FOG_LINEAR_RANGE;
}

// ------------------------------------------------------------------------------
// CShaderLinkerDemo Destructor
//
// Description: Nothing to it -- just straight-forward destruction
// ------------------------------------------------------------------------------ 
CShaderLinkerDemo::~CShaderLinkerDemo()
{
	Free(); 
}

// ------------------------------------------------------------------------------
// CShaderLinkerDemo::UpdateProperties
//
// Description: This adds options to the menu (click on "File" or right-click
// on the display.
// ------------------------------------------------------------------------------ 
#define STR_DISPLAYOPTIONS "Display Options"
void CShaderLinkerDemo::UpdateProperties()
{
	EBEffect::UpdateProperties();

	AddProperty(new EBProperty("Wireframe", OBJECT_MEMBER(m_bWireframe), EBTYPE_BOOL_PROP));
	AddProperty(new EBProperty("Pause", OBJECT_MEMBER(m_bPause), EBTYPE_BOOL_PROP));

	// Expose our poly count to the framework
	AddProperty(new EBProperty("TriangleCount", OBJECT_MEMBER(m_dwNumTriangles), EBTYPE_DWORD_PROP));
	AddProperty(new EBProperty("VertexCount", OBJECT_MEMBER(m_dwNumVertices), EBTYPE_DWORD_PROP));

	EBEnumProperty* pEnumProp = new EBEnumProperty(STR_DISPLAYOPTIONS, OBJECT_MEMBER(m_eDisplayOption), EBTYPE_DWORD_PROP, PROPFLAG_CALLBACK);
	pEnumProp->AddEnumerant(new EBEnumValue(pEnumProp, "Show Sphere", (DWORD)DISPLAY_SPHERE, EBTYPE_DWORD_PROP));
	pEnumProp->AddEnumerant(new EBEnumValue(pEnumProp, "Show Model", (DWORD)DISPLAY_MODEL, EBTYPE_DWORD_PROP));
	AddProperty(pEnumProp);

	AddProperty(new EBTriggerProperty("Load another model..."));

	// Vertex shaders
	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Shared", GetFilePath("miscvertex.nvf"), EBTYPE_STRING_PROP));
	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Shared Lighting", GetFilePath("lighting.nvf"), EBTYPE_STRING_PROP));
	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "EyeSpace", GetFilePath("eyespace.nvf"), EBTYPE_STRING_PROP));
	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "ObjectSpace", GetFilePath("objectspace.nvf"), EBTYPE_STRING_PROP));
	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Basis Skinning", GetFilePath("basisskinning.nvf"), EBTYPE_STRING_PROP));
	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Output", GetFilePath("output.nvv"), EBTYPE_STRING_PROP));
}

 

// ------------------------------------------------------------------------------
// CShaderLinkerDemo::ConfirmDevice
//
// Description: Performs caps-bit checking to make sure the selected device 
//		supports this demo.  In this particular case we check for nothing!
// ------------------------------------------------------------------------------ 
HRESULT CShaderLinkerDemo::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
{
	if (!(pCaps->TextureCaps & D3DPTEXTURECAPS_MIPMAP))
	{
		m_strLastError = "Device does not support mipmaps!";
		return E_FAIL;
	}
/*

	if (!(pCaps->TextureCaps & D3DPTEXTURECAPS_PROJECTED))
	{
		m_strLastError = "Device does not support 3 element texture coordinates!";
		return E_FAIL;
	}
*/
	return S_OK;
}

HRESULT CShaderLinkerDemo::CreateCubeEnvironment()
{
	HRESULT hr;
    hr = m_pD3DDev->CreateVertexBuffer(NUM_CUBE_VERTICES * sizeof(WorldBoxVertex), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_pWorldBoxVertices);
	if (FAILED(hr))
	{
		m_strLastError = "Could not create vertex buffer!";
		return hr;
	}

	hr = m_pD3DDev->CreateIndexBuffer(NUM_CUBE_INDICES * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pWorldBoxIndices);
	if (FAILED(hr))
	{
		m_strLastError = "Could not create index buffer!";
		return hr;
	}

	WorldBoxVertex* pVertices = NULL;
	WORD* pIndices = NULL;
	hr = m_pWorldBoxVertices->Lock(0, sizeof(WorldBoxVertex) * NUM_CUBE_VERTICES,(BYTE**)&pVertices, 0);
	if (FAILED(hr))
	{
		m_strLastError = "Could not lock vertex buffer!";
		return hr;
	}

    // -Z face
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f, 1.0f,-1.0f));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f, 1.0f,-1.0f));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f,-1.0f,-1.0f));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f,-1.0f,-1.0f));

    // +Z face
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f, 1.0f, 1.0f));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f,-1.0f, 1.0f));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f,-1.0f, 1.0f));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f, 1.0f, 1.0f));

    // -Y face
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f, 1.0f, 1.0f));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f, 1.0f, 1.0f));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f, 1.0f,-1.0f));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f, 1.0f,-1.0f));

    // +Y face
	*pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f,-1.0f, 1.0f));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f,-1.0f,-1.0f));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f,-1.0f,-1.0f));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f,-1.0f, 1.0f));

    // -X face
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f, 1.0f,-1.0f));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f, 1.0f, 1.0f));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f,-1.0f, 1.0f));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f,-1.0f,-1.0f));

    // +X face
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f, 1.0f,-1.0f));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f,-1.0f,-1.0f));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f,-1.0f, 1.0f));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f, 1.0f, 1.0f));

	m_pWorldBoxVertices->Unlock();

	hr = m_pWorldBoxIndices->Lock(0, sizeof(WORD) * NUM_CUBE_INDICES,(BYTE**)&pIndices, 0);
	if (FAILED(hr))
	{
		m_strLastError = "Could not lock vertex buffer!";
		return hr;
	}

    // Set up the indices for the cube
    *pIndices++ =  0+0;   *pIndices++ =  0+1;   *pIndices++ =  0+2;
    *pIndices++ =  0+2;   *pIndices++ =  0+3;   *pIndices++ =  0+0;
    *pIndices++ =  4+0;   *pIndices++ =  4+1;   *pIndices++ =  4+2;
    *pIndices++ =  4+2;   *pIndices++ =  4+3;   *pIndices++ =  4+0;
    *pIndices++ =  8+0;   *pIndices++ =  8+1;   *pIndices++ =  8+2;
    *pIndices++ =  8+2;   *pIndices++ =  8+3;   *pIndices++ =  8+0;
    *pIndices++ = 12+0;   *pIndices++ = 12+1;   *pIndices++ = 12+2;
    *pIndices++ = 12+2;   *pIndices++ = 12+3;   *pIndices++ = 12+0;
    *pIndices++ = 16+0;   *pIndices++ = 16+1;   *pIndices++ = 16+2;
    *pIndices++ = 16+2;   *pIndices++ = 16+3;   *pIndices++ = 16+0;
    *pIndices++ = 20+0;   *pIndices++ = 20+1;   *pIndices++ = 20+2;
    *pIndices++ = 20+2;   *pIndices++ = 20+3;   *pIndices++ = 20+0;

	m_pWorldBoxIndices->Unlock();

	D3DXMatrixIdentity(&m_matCubeRotation);
	D3DXMatrixRotationY(&m_matCubeRotation, D3DXToRadian(180.0f));

	return S_OK;
}

HRESULT CShaderLinkerDemo::LoadFragmentFile(const std::string& strFragmentFile)
{
	void* pFragments = NULL;
	DWORD dwFileSize;

	// In the initialisation phase we give the linker all the info it needs to
	// assemble fragments.
	try
	{
		HANDLE hFile;

		hFile = CreateFile(GetFilePath(strFragmentFile.c_str()).c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			m_strLastError = "Could not find file " + strFragmentFile;
			return E_FAIL;
		}

		dwFileSize = GetFileSize(hFile, NULL);
		pFragments = (void*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwFileSize);
		if (!pFragments)
		{
			m_strLastError = "Failed to allocate memory for fragments";
			return E_FAIL;
		}

		ReadFile(hFile, pFragments, dwFileSize, &dwFileSize, NULL);
		CloseHandle(hFile);

		DWORD dwRet = m_pLinker->AddFragments(pFragments, dwFileSize);
		if (dwRet != NVLINK_OK)
		{
			m_strLastError = "Failed to give linker fragments";
			return E_FAIL;
		}
		HeapFree(GetProcessHeap(), 0, (void*)pFragments);

	}
	catch(...)
	{
		m_strLastError = "Error opening file " + strFragmentFile;
		return E_FAIL;
	}
	return S_OK;
}



// ------------------------------------------------------------------------------
// CShaderLinkerDemo::Initialize
//
// Description: Initialize render-states, vertex shader and vertex buffers
// ------------------------------------------------------------------------------ 
HRESULT CShaderLinkerDemo::Initialize(IDirect3DDevice8* pDev)
{
	HRESULT hr = S_OK;

	srand(timeGetTime());
	m_strLastError = "";

	m_bSetShaderToShow = false;

	ZeroMemory(&m_DialogFunc, sizeof(LinkerDialogFunc));
	m_hDialog = LoadLibrary(GetFilePath("linkerdialog.dll").c_str());
	if (!m_hDialog)
	{
		m_strLastError = "Could not load linkerdialog.dll";
		return E_FAIL;
	}

	m_DialogFunc.ShowDialog = (LPSHOWDIALOG)GetProcAddress(m_hDialog, "LinkerDialog_ShowDialog");
	m_DialogFunc.Initialise = (LPINITIALISE)GetProcAddress(m_hDialog, "LinkerDialog_Initialise");
	m_DialogFunc.Free = (LPFREE)GetProcAddress(m_hDialog, "LinkerDialog_Free");
	m_DialogFunc.GetSettings = (LPGETSETTINGS)GetProcAddress(m_hDialog, "LinkerDialog_GetSettings");
	m_DialogFunc.SetSettings = (LPSETSETTINGS)GetProcAddress(m_hDialog, "LinkerDialog_SetSettings");

	if (!m_DialogFunc.ShowDialog ||
		!m_DialogFunc.Initialise ||
		!m_DialogFunc.Free ||
		!m_DialogFunc.GetSettings ||
		!m_DialogFunc.SetSettings)
	{
		m_strLastError = "Could not resolve dialog box functions!";
		return E_FAIL;
	}


	D3DDEVICE_CREATION_PARAMETERS CreateParams;	
	pDev->GetCreationParameters(&CreateParams);
	HWND hParent = GetParent(CreateParams.hFocusWindow);
	if (!m_DialogFunc.Initialise(CreateParams.hFocusWindow))
	{
		m_strLastError = "Could not initialize dialog box!";
		return E_FAIL;
	}

	m_DialogFunc.ShowDialog(true);

	// Create the linker
	m_hLinker = LoadLibrary(GetFilePath("nvlinker.dll").c_str());
	if (m_hLinker == NULL)
	{
		m_strLastError = "Could not create linker dll (nvlinker.dll)";
		return E_FAIL;
	}

	LPCREATELINKER pCreate = (LPCREATELINKER)GetProcAddress(m_hLinker, "CreateLinker");
	if (!pCreate)
	{
		m_strLastError = "Could not resolve linker create function in nvlinker.dll";
		return E_FAIL;
	}

	m_pLinker = pCreate();
	hr = LoadFragmentFile("miscvertex.nvo");
	if (FAILED(hr))
		return hr;

	hr = LoadFragmentFile("lighting.nvo");
	if (FAILED(hr))
		return hr;

	hr = LoadFragmentFile("objectspace.nvo");
	if (FAILED(hr))
		return hr;

	hr = LoadFragmentFile("eyespace.nvo");
	if (FAILED(hr))
		return hr;

	hr = LoadFragmentFile("basisskinning.nvo");
	if (FAILED(hr))
		return hr;
	
	// In linker 1.0 we have to do this manually.  In version 2 there will be an option to dump a header file
	// with a table of #defines and names from the link.exe utility.
	// Get the framgent ID's
	ZeroMemory(&m_FragmentID, sizeof(m_FragmentID));
	m_pLinker->GetFragmentID("set_ambient", &m_FragmentID[F_SET_AMBIENT]);
	m_pLinker->GetFragmentID("set_default_texture", &m_FragmentID[F_SET_DEFAULT_TEXTURE]);
	m_pLinker->GetFragmentID("xform_worldviewprojection", &m_FragmentID[F_XFORM_WORLDVIEWPROJECTION]);
	m_pLinker->GetFragmentID("set_power", &m_FragmentID[F_SET_POWER]);
	m_pLinker->GetFragmentID("clamp_diffuse_exponentiate_specular", &m_FragmentID[F_CLAMP_DIFFUSE_EXPONENTIATE_SPECULAR]);
	
	// Point
	m_pLinker->GetFragmentID("pointlight_accumulate_diffuse", &m_FragmentID[F_POINTLIGHT_ACCUMULATE_DIFFUSE]);
	m_pLinker->GetFragmentID("pointlight_accumulate_diffuse_last", &m_FragmentID[F_POINTLIGHT_ACCUMULATE_DIFFUSE_LAST]);
	m_pLinker->GetFragmentID("pointlight_accumulate_specular_first", &m_FragmentID[F_POINTLIGHT_ACCUMULATE_SPECULAR_FIRST]);
	m_pLinker->GetFragmentID("pointlight_accumulate_specular", &m_FragmentID[F_POINTLIGHT_ACCUMULATE_SPECULAR]);
	m_pLinker->GetFragmentID("pointlight_accumulate_specular_last", &m_FragmentID[F_POINTLIGHT_ACCUMULATE_SPECULAR_LAST]);
	m_pLinker->GetFragmentID("pointlight_accumulate_specular_first_and_last", &m_FragmentID[F_POINTLIGHT_ACCUMULATE_SPECULAR_FIRST_AND_LAST]);

	// Directional
	m_pLinker->GetFragmentID("dirlight_accumulate_diffuse", &m_FragmentID[F_DIRLIGHT_ACCUMULATE_DIFFUSE]);
	m_pLinker->GetFragmentID("dirlight_accumulate_diffuse_last", &m_FragmentID[F_DIRLIGHT_ACCUMULATE_DIFFUSE_LAST]);
	m_pLinker->GetFragmentID("dirlight_accumulate_specular_first", &m_FragmentID[F_DIRLIGHT_ACCUMULATE_SPECULAR_FIRST]);
	m_pLinker->GetFragmentID("dirlight_accumulate_specular", &m_FragmentID[F_DIRLIGHT_ACCUMULATE_SPECULAR]);
	m_pLinker->GetFragmentID("dirlight_accumulate_specular_last", &m_FragmentID[F_DIRLIGHT_ACCUMULATE_SPECULAR_LAST]);
	m_pLinker->GetFragmentID("dirlight_accumulate_specular_first_and_last", &m_FragmentID[F_DIRLIGHT_ACCUMULATE_SPECULAR_FIRST_AND_LAST]);

	// Eyespace	
	m_pLinker->GetFragmentID("create_eyespace_vertex", &m_FragmentID[F_CREATE_EYESPACE_VERTEX]);
	m_pLinker->GetFragmentID("create_eyespace_eyevector_local_viewer", &m_FragmentID[F_CREATE_EYESPACE_EYEVECTOR_LOCAL_VIEWER]);
	m_pLinker->GetFragmentID("create_eyespace_normal", &m_FragmentID[F_CREATE_EYESPACE_NORMAL]);
	m_pLinker->GetFragmentID("calculate_eyespace_reflection", &m_FragmentID[F_CALCULATE_EYESPACE_REFLECTION]);
	m_pLinker->GetFragmentID("calculate_eyespace_directionallight_diffuse_and_specular_intensity_nonlocalviewer", &m_FragmentID[F_CALCULATE_EYESPACE_DIRECTIONALLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_NONLOCALVIEWER]);
	m_pLinker->GetFragmentID("calculate_eyespace_directionallight_diffuse_and_specular_intensity_localviewer", &m_FragmentID[F_CALCULATE_EYESPACE_DIRECTIONALLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_LOCALVIEWER]);
	m_pLinker->GetFragmentID("calculate_eyespace_directionallight_diffuse_intensity", &m_FragmentID[F_CALCULATE_EYESPACE_DIRECTIONALLIGHT_DIFFUSE_INTENSITY]);
	m_pLinker->GetFragmentID("calculate_eyespace_directionallight_diffuse_and_specular_intensity_rdotl", &m_FragmentID[F_CALCULATE_EYESPACE_DIRECTIONALLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_RDOTL]);
	m_pLinker->GetFragmentID("calculate_eyespace_pointlight_vector", &m_FragmentID[F_CALCULATE_EYESPACE_POINTLIGHT_VECTOR]);
	m_pLinker->GetFragmentID("calculate_eyespace_pointlight_diffuse_intensity", &m_FragmentID[F_CALCULATE_EYESPACE_POINTLIGHT_DIFFUSE_INTENSITY]);
	m_pLinker->GetFragmentID("calculate_eyespace_pointlight_diffuse_and_specular_intensity_localviewer", &m_FragmentID[F_CALCULATE_EYESPACE_POINTLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_LOCALVIEWER]);
	m_pLinker->GetFragmentID("calculate_eyespace_pointlight_diffuse_and_specular_intensity_nonlocalviewer", &m_FragmentID[F_CALCULATE_EYESPACE_POINTLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_NONLOCALVIEWER]);
	m_pLinker->GetFragmentID("calculate_eyespace_pointlight_diffuse_and_specular_intensity_rdotl", &m_FragmentID[F_CALCULATE_EYESPACE_POINTLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_RDOTL]);

	// Basis skinning
	m_pLinker->GetFragmentID("calculate_basis_vectors", &m_FragmentID[F_CALCULATE_BASIS_VECTORS]);

	// Object space
	m_pLinker->GetFragmentID("create_objectspace_eyevector_local_viewer", &m_FragmentID[F_CREATE_OBJECTSPACE_EYEVECTOR_LOCAL_VIEWER]);
	m_pLinker->GetFragmentID("calculate_objectspace_reflection", &m_FragmentID[F_CALCULATE_OBJECTSPACE_REFLECTION]);
	m_pLinker->GetFragmentID("calculate_cubereflection_vector", &m_FragmentID[F_CALCULATE_CUBEREFLECTION_VECTOR]);
	m_pLinker->GetFragmentID("calculate_linear_range_fog", &m_FragmentID[F_CALCULATE_LINEAR_RANGE_FOG]);
	m_pLinker->GetFragmentID("calculate_objectspace_directionallight_diffuse_and_specular_intensity_nonlocalviewer", &m_FragmentID[F_CALCULATE_OBJECTSPACE_DIRECTIONALLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_NONLOCALVIEWER]);
	m_pLinker->GetFragmentID("calculate_objectspace_directionallight_diffuse_and_specular_intensity_localviewer", &m_FragmentID[F_CALCULATE_OBJECTSPACE_DIRECTIONALLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_LOCALVIEWER]);
	m_pLinker->GetFragmentID("calculate_objectspace_directionallight_diffuse_and_specular_intensity_rdotl", &m_FragmentID[F_CALCULATE_OBJECTSPACE_DIRECTIONALLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_RDOTL]);
	m_pLinker->GetFragmentID("calculate_objectspace_directionallight_diffuse_intensity", &m_FragmentID[F_CALCULATE_OBJECTSPACE_DIRECTIONALLIGHT_DIFFUSE_INTENSITY]);
	m_pLinker->GetFragmentID("calculate_objectspace_pointlight_vector", &m_FragmentID[F_CALCULATE_OBJECTSPACE_POINTLIGHT_VECTOR]);
	m_pLinker->GetFragmentID("calculate_objectspace_pointlight_diffuse_intensity", &m_FragmentID[F_CALCULATE_OBJECTSPACE_POINTLIGHT_DIFFUSE_INTENSITY]);
	m_pLinker->GetFragmentID("calculate_objectspace_pointlight_diffuse_and_specular_intensity_localviewer", &m_FragmentID[F_CALCULATE_OBJECTSPACE_POINTLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_LOCALVIEWER]);
	m_pLinker->GetFragmentID("calculate_objectspace_pointlight_diffuse_and_specular_intensity_nonlocalviewer", &m_FragmentID[F_CALCULATE_OBJECTSPACE_POINTLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_NONLOCALVIEWER]);
	m_pLinker->GetFragmentID("calculate_objectspace_pointlight_diffuse_and_specular_intensity_rdotl", &m_FragmentID[F_CALCULATE_OBJECTSPACE_POINTLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_RDOTL]);
	m_pLinker->GetFragmentID("calculate_basis_vectors", &m_FragmentID[F_CALCULATE_BASIS_VECTORS]);
	m_pLinker->GetFragmentID("write_texture", &m_FragmentID[F_WRITE_TEXTURE]);
	m_pLinker->GetFragmentID("write_default_diffuse", &m_FragmentID[F_WRITE_DEFAULT_DIFFUSE]);
	m_pLinker->GetFragmentID("write_default_texture", &m_FragmentID[F_WRITE_DEFAULT_TEXTURE]);
	m_pLinker->GetFragmentID("write_default_specular", &m_FragmentID[F_WRITE_DEFAULT_SPECULAR]);

	ZeroMemory(&m_ConstantID, sizeof(m_ConstantID));
	m_pLinker->GetConstantID("c_dirlight_halfvector", &m_ConstantID[C_DIRLIGHT_HALFVECTOR]);
	m_pLinker->GetConstantID("c_ambient_light", &m_ConstantID[C_AMBIENT_LIGHT]);
	m_pLinker->GetConstantID("c_constants", &m_ConstantID[C_CONSTANTS]);
	m_pLinker->GetConstantID("c_objectspace_eyevector", &m_ConstantID[C_OBJECTSPACE_EYEVECTOR]);
	m_pLinker->GetConstantID("c_objectspace_eyeposition", &m_ConstantID[C_OBJECTSPACE_EYEPOSITION]);
	m_pLinker->GetConstantID("c_eyespace_eyevector", &m_ConstantID[C_EYESPACE_EYEVECTOR]);
	m_pLinker->GetConstantID("c_worldviewprojection_0", &m_ConstantID[C_WORLDVIEWPROJECTION_0]);
	m_pLinker->GetConstantID("c_worldviewprojection_1", &m_ConstantID[C_WORLDVIEWPROJECTION_1]);
	m_pLinker->GetConstantID("c_worldviewprojection_2", &m_ConstantID[C_WORLDVIEWPROJECTION_2]);
	m_pLinker->GetConstantID("c_worldviewprojection_3", &m_ConstantID[C_WORLDVIEWPROJECTION_3]);
	m_pLinker->GetConstantID("c_worldviewit_0", &m_ConstantID[C_WORLDVIEWIT_0]);
	m_pLinker->GetConstantID("c_worldviewit_1", &m_ConstantID[C_WORLDVIEWIT_1]);
	m_pLinker->GetConstantID("c_worldviewit_2", &m_ConstantID[C_WORLDVIEWIT_2]);
	m_pLinker->GetConstantID("c_worldview_0", &m_ConstantID[C_WORLDVIEW_0]);
	m_pLinker->GetConstantID("c_worldview_1", &m_ConstantID[C_WORLDVIEW_1]);
	m_pLinker->GetConstantID("c_worldview_2", &m_ConstantID[C_WORLDVIEW_2]);
	m_pLinker->GetConstantID("c_worldview_3", &m_ConstantID[C_WORLDVIEW_3]);
	m_pLinker->GetConstantID("c_worldviewcubemap_0", &m_ConstantID[C_WORLDVIEWCUBEMAP_0]);
	m_pLinker->GetConstantID("c_worldviewcubemap_1", &m_ConstantID[C_WORLDVIEWCUBEMAP_1]);
	m_pLinker->GetConstantID("c_worldviewcubemap_2", &m_ConstantID[C_WORLDVIEWCUBEMAP_2]);
	m_pLinker->GetConstantID("c_cubematrix_0", &m_ConstantID[C_CUBEMATRIX_0]);
	m_pLinker->GetConstantID("c_cubematrix_1", &m_ConstantID[C_CUBEMATRIX_1]);
	m_pLinker->GetConstantID("c_cubematrix_2", &m_ConstantID[C_CUBEMATRIX_2]);
	m_pLinker->GetConstantID("c_dirlight_direction", &m_ConstantID[C_DIRLIGHT_DIRECTION]);
	m_pLinker->GetConstantID("c_dirlight_diffuse", &m_ConstantID[C_DIRLIGHT_DIFFUSE]);
	m_pLinker->GetConstantID("c_dirlight_specular", &m_ConstantID[C_DIRLIGHT_SPECULAR]);
	m_pLinker->GetConstantID("c_pointlight_position", &m_ConstantID[C_POINTLIGHT_POSITION]);
	m_pLinker->GetConstantID("c_pointlight_diffuse", &m_ConstantID[C_POINTLIGHT_DIFFUSE]);
	m_pLinker->GetConstantID("c_pointlight_specular", &m_ConstantID[C_POINTLIGHT_SPECULAR]);
	m_pLinker->GetConstantID("c_mat_power", &m_ConstantID[C_MAT_POWER]);
	m_pLinker->GetConstantID("c_fogconst", &m_ConstantID[C_FOGCONST]);

	ZeroMemory(&m_VertexID, sizeof(m_VertexID));
	//m_pLinker->GetVertexID("v_position", &m_VertexID[V_POSITION]);	// Fixed position
	m_pLinker->GetVertexID("v_normal", &m_VertexID[V_NORMAL]);
	m_pLinker->GetVertexID("v_diffuse", &m_VertexID[V_DIFFUSE]);
	m_pLinker->GetVertexID("v_specular", &m_VertexID[V_SPECULAR]);
	m_pLinker->GetVertexID("v_sbasis0", &m_VertexID[V_SBASIS0]);
	m_pLinker->GetVertexID("v_tbasis0", &m_VertexID[V_TBASIS0]);
	m_pLinker->GetVertexID("v_sxtbasis0", &m_VertexID[V_SXTBASIS0]);
	//m_pLinker->GetVertexID("v_texture0", &m_VertexID[V_TEXTURE0]);	// Fixed texture
	m_pLinker->GetVertexID("v_texture1", &m_VertexID[V_TEXTURE1]);
	m_pLinker->GetVertexID("v_texture2", &m_VertexID[V_TEXTURE2]);
	m_pLinker->GetVertexID("v_texture3", &m_VertexID[V_TEXTURE3]);
	m_pLinker->GetVertexID("v_texture4", &m_VertexID[V_TEXTURE4]);
	m_pLinker->GetVertexID("v_texture5", &m_VertexID[V_TEXTURE5]);
	m_pLinker->GetVertexID("v_texture6", &m_VertexID[V_TEXTURE6]);
	m_pLinker->GetVertexID("v_texture7", &m_VertexID[V_TEXTURE7]);

#ifdef _DEBUG
	// Verify we pulled out the registers we expected
	DWORD i;
	DWORD y;
	for (i = 1; i < F_NUM_FRAGMENTS; i++)
	{
		assert(m_FragmentID[i] != 0);
		for (y = 1; y < F_NUM_FRAGMENTS; y++)
		{
			
			if (i != y)
			{
				assert(m_FragmentID[i] != m_FragmentID[y]);
			}
		}
	}
	
	for (i = 1; i < C_NUM_CONSTANTS; i++)
	{
		assert(m_ConstantID[i] != 0);
		for (y = 1; y < C_NUM_CONSTANTS; y++)
		{
			if (i != y)
			{
				assert(m_ConstantID[i] != m_ConstantID[y]);
			}
		}
	}
#endif
	
	// get the D3D device 
	m_pD3DDev = pDev;
	pDev->AddRef();

	g_pNVStateManager = new NVStateManager(m_pD3DDev);
	g_pNVResourceManager = new NVResourceManager(m_pD3DDev);


	//initialize mouse UI
	RECT rect;
	rect.left = rect.top = 0;
	D3DVIEWPORT8 viewport;
	m_pD3DDev->GetViewport(&viewport);
	rect.bottom = viewport.Height;
	rect.right  = viewport.Width;
	m_pUI = new MouseUI((const RECT)rect);
	m_pUI->SetTranslationalSensitivityFactor(0.1f);

	m_Eye = EyeStart;
	m_LookAt.x = m_Eye.x;
	m_LookAt.y = m_Eye.y;
	m_LookAt.z = m_Eye.z + 1.0f;
	m_Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	D3DXMatrixLookAtLH(&m_matView, &m_Eye, &m_LookAt, &m_Up);
	
	D3DXMatrixPerspectiveFovLH(&m_matProjection,
		D3DXToRadian(60.0f),
		1,
		0.2f,
		50.0f);
	NVSTATEMANAGER.SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&m_matProjection);

	hr = LoadLightObjects();
	if (FAILED(hr))
	{
		m_strLastError = "Failed to create light objects";
		return hr;
	}

    D3DCAPS8 DeviceCaps;
    m_pD3DDev->GetDeviceCaps(&DeviceCaps);

	m_bSupportsNormalMap = false;
	if(D3DSHADER_VERSION_MAJOR(DeviceCaps.PixelShaderVersion) >= 1)
	{
		// Create a heightmap resource
		NVResourceID HeightMapID = NVRESOURCEMANAGER.AddResource(new NVTextureFileResource(m_pD3DDev, 
						GetFilePath("bump_map_nvidia.dds"),
						NVTextureFileResource::TextureType_2D,
						D3DX_DEFAULT,
						D3DX_DEFAULT,
						0,
						0,
						D3DFMT_A8R8G8B8), "DefaultHeightMap");
		if (NVTextureResource::GetTexture(HeightMapID) == NULL)
		{
			m_strLastError = "Failed to create HeightMap (bump_map_nvidia.dds)";
			return E_FAIL;
		}

		// Create a normal map resource, based off a height map resource
		NVResourceID NormalMapID = NVRESOURCEMANAGER.AddResource(new NVTextureNormalMapFromResource(m_pD3DDev, HeightMapID, (D3DFORMAT)MAKEFOURCC('N', 'V', 'H', 'S'), D3DPOOL_DEFAULT), "DefaultNormalMap");
		if (NVTextureResource::GetTexture(NormalMapID) != NULL)
		{
			m_bSupportsNormalMap = true;

			// Pixel shader for bumpmaps
			hr = LoadAndCreateShader(GetFilePath("linkerpixels.pso"), NULL, 0, SHADERTYPE_PIXEL, &m_dwBumpPixelShader);
			if (FAILED(hr))
			{
				return NULL;
			}
		}
	}

	NVRESOURCEMANAGER.AddResource(new NVTextureFileResource(m_pD3DDev, GetFilePath("gloss_map_nvidia.dds")), "DefaultGlossMap");
	if (NVTextureResource::GetTexture("DefaultGlossMap") == NULL)
	{
		m_strLastError = "Failed to create glossmap (gloss_map_nvidia.dds)";
		return E_FAIL;
	}

	m_bSupportsCubeMap = true;
	if (!(DeviceCaps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP))
	{
		m_bSupportsCubeMap = false;
	}

	if (m_bSupportsCubeMap)
	{
		NVRESOURCEMANAGER.AddResource(new NVTextureFileResource(m_pD3DDev, GetFilePath("nvlobby_cube_mipmap.dds"), NVTextureFileResource::TextureType_CubeMap), "CubeMap");
		if (NVTextureResource::GetTexture("CubeMap") == NULL)
		{
			m_strLastError = "Failed to create cubemap (nvlobby_cube_mipmap.dds)";
			return E_FAIL;
		}
	}

	if (!m_bSupportsNormalMap && (m_Settings.m_TexGen == TEXGEN_BLINNBUMPREFLECTION))
	{
		m_Settings.m_TexGen = TEXGEN_CAMERASPACEREFLECTION;
	}

	if (!m_bSupportsCubeMap && (m_Settings.m_TexGen == TEXGEN_CAMERASPACEREFLECTION))
	{
		m_Settings.m_TexGen = TEXGEN_NONE;
	}

	m_Settings.m_bEnableBump = m_bSupportsNormalMap;
	m_Settings.m_bEnableReflect = m_bSupportsCubeMap;
	UpdateLights();

	if (m_bSupportsCubeMap)
	{
		hr = CreateCubeEnvironment();
		if (FAILED(hr))
		{
			if (m_strLastError.empty())
				m_strLastError = "Failed to create cube environment";
			return hr;
		}
	}

	hr = LoadMesh(m_strFileName.c_str());
	if (FAILED(hr))
	{
		if (m_strLastError.empty())
			m_strLastError = "Failed to load " + m_strFileName;
		return hr;
	}

	hr = UpdateShader();

	// Don't want culling
	NVSTATEMANAGER.SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// Setup fogging for potential use by the vertex shader
	NVSTATEMANAGER.SetRenderState( D3DRS_FOGCOLOR, 0xFFFFFFFF);
    NVSTATEMANAGER.SetRenderState( D3DRS_FOGTABLEMODE,   D3DFOG_NONE );
    NVSTATEMANAGER.SetRenderState( D3DRS_FOGVERTEXMODE,  D3DFOG_NONE );
    NVSTATEMANAGER.SetRenderState( D3DRS_RANGEFOGENABLE, FALSE );

	m_vecFogData.x = 1.0f;	// Fog Start
	m_vecFogData.y = 4.0f; // Fog End
	m_vecFogData.z = 1.0f / (m_vecFogData.y - m_vecFogData.x); // Fog range
	m_vecFogData.w = 255.0f;


	// Default stage 0 is to have texture in 0, diffuse in 1.
	NVSTATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	NVSTATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);


	return S_OK;
}

void CShaderLinkerDemo::UpdateMeshVertexSlots()
{

	// Fill in the required vertex stream locations
	// Here we are doing 2 things.
	// We are setting up our declation to match the streams.
	// We are telling the mesh object where to put each stream (and therefore 'enabling' that stream)
	NVComplexMeshVertexData* pVertexData;
	DWORD dwLocation;
	DWORD dwStreamPosition = 0;
	
	m_MeshDeclaration.clear();
	m_MeshDeclaration.push_back(D3DVSD_STREAM(0));

	pVertexData = m_pMesh->FindVertexData("position");
	if (pVertexData)
	{
		// Position data is always in v0, because we want it that way
		m_MeshDeclaration.push_back(D3DVSD_REG(FIXED_POSITION_LOCATION, pVertexData->GetDataType()));
		pVertexData->SetLocation(dwStreamPosition++);
	}

	pVertexData = m_pMesh->FindVertexData("normal");
	if (pVertexData)
	{
		m_pLinker->GetVertexSlot(m_VertexID[V_NORMAL], &dwLocation);
		if (dwLocation != NVLINK_NOT_USED)
		{
			m_MeshDeclaration.push_back(D3DVSD_REG(dwLocation, pVertexData->GetDataType()));
			pVertexData->SetLocation(dwStreamPosition++);
		}
		else
		{
			pVertexData->SetLocation(NVCOMPLEXMESH_NO_VERTEX);
		}
	}

	pVertexData = m_pMesh->FindVertexData("diffuse");
	if (pVertexData)
	{
		m_pLinker->GetVertexSlot(m_VertexID[V_DIFFUSE], &dwLocation);
		if (dwLocation != NVLINK_NOT_USED)
		{
			m_MeshDeclaration.push_back(D3DVSD_REG(dwLocation, pVertexData->GetDataType()));
			pVertexData->SetLocation(dwStreamPosition++);
		}
		else
		{
			pVertexData->SetLocation(NVCOMPLEXMESH_NO_VERTEX);
		}
	}

	pVertexData = m_pMesh->FindVertexData("specular");
	if (pVertexData)
	{
		m_pLinker->GetVertexSlot(m_VertexID[V_SPECULAR], &dwLocation);
		if (dwLocation != NVLINK_NOT_USED)
		{
			m_MeshDeclaration.push_back(D3DVSD_REG(dwLocation, pVertexData->GetDataType()));
			pVertexData->SetLocation(dwStreamPosition++);
		}
		else
		{
			pVertexData->SetLocation(NVCOMPLEXMESH_NO_VERTEX);
		}
	}

	pVertexData = m_pMesh->FindVertexData("sbasis0");
	if (pVertexData)
	{
		m_pLinker->GetVertexSlot(m_VertexID[V_SBASIS0], &dwLocation);
		if (dwLocation != NVLINK_NOT_USED)
		{
			m_MeshDeclaration.push_back(D3DVSD_REG(dwLocation, pVertexData->GetDataType()));
			pVertexData->SetLocation(dwStreamPosition++);
		}
		else
		{
			pVertexData->SetLocation(NVCOMPLEXMESH_NO_VERTEX);
		}
	}
	pVertexData = m_pMesh->FindVertexData("tbasis0");
	if (pVertexData)
	{
		m_pLinker->GetVertexSlot(m_VertexID[V_TBASIS0], &dwLocation);
		if (dwLocation != NVLINK_NOT_USED)
		{
			m_MeshDeclaration.push_back(D3DVSD_REG(dwLocation, pVertexData->GetDataType()));
			pVertexData->SetLocation(dwStreamPosition++);
		}
		else
		{
			pVertexData->SetLocation(NVCOMPLEXMESH_NO_VERTEX);
		}
	}

	pVertexData = m_pMesh->FindVertexData("sxtbasis0");
	if (pVertexData)
	{
		m_pLinker->GetVertexSlot(m_VertexID[V_SXTBASIS0], &dwLocation);
		if (dwLocation != NVLINK_NOT_USED)
		{
			m_MeshDeclaration.push_back(D3DVSD_REG(dwLocation, pVertexData->GetDataType()));
			pVertexData->SetLocation(dwStreamPosition++);
		}
		else
		{
			pVertexData->SetLocation(NVCOMPLEXMESH_NO_VERTEX);
		}
	}

	for (DWORD i = 0; i < NV_MAX_TEXTURES; i++)
	{
		string strBuff;
  		strBuff.resize(2);
  		ultoa(i, &strBuff[0], 2);
  		string texname = "texture" + strBuff;
  		texname = texname.substr(0, strlen(texname.c_str()));

		pVertexData = m_pMesh->FindVertexData(texname.c_str());
		if (pVertexData)
		{
			if (i == 0)
			{
				// We have chosen to place texture 0 at a fixed location.
				dwLocation = FIXED_TEXTURE0_LOCATION;
			}
			else
			{
				m_pLinker->GetVertexSlot(m_VertexID[V_TEXTURE0 + i], &dwLocation);
			}

			if (dwLocation != NVLINK_NOT_USED)
			{
				m_MeshDeclaration.push_back(D3DVSD_REG(dwLocation, pVertexData->GetDataType()));
				pVertexData->SetLocation(dwStreamPosition++);
			}
			else
			{
				pVertexData->SetLocation(NVCOMPLEXMESH_NO_VERTEX);
			}
		}
	}

	// Finish the decleration and create the shader
	m_MeshDeclaration.push_back(D3DVSD_END());
}

HRESULT CShaderLinkerDemo::UpdateShader()
{
	HRESULT hr;
	std::string strOutFile;
	HANDLE hFile;
	DWORD dwNumWritten;
	DWORD dwLocation;
	DWORD i;

	if (m_dwPass0Shader)
	{
		m_pD3DDev->DeleteVertexShader(m_dwPass0Shader);
		m_dwPass0Shader = 0;
	}

	if (m_dwPass1Shader)
	{
		m_pD3DDev->DeleteVertexShader(m_dwPass1Shader);
		m_dwPass1Shader = 0;
	}

	// Force a change to a default vertex shader because the deletes above will upset us otherwise.
	NVSTATEMANAGER.SetVertexShader(D3DFVF_XYZ);
	NVSTATEMANAGER.FlushVertexShader();

	// Create the required shader.
	// ThisShader is an array of DWORDS that can grow.  Each push_back call adds another DWORD to the array.
	// STL is nice like this because it eliminates stupid errors with over-filling arrays.  Of course the drawback
	// is dynamic allocation, but this is a demo and I'm going for simple.
	// The DWORD's we put in are the ID's of the shader components we want to build.

	// You can get as intricate as you like about how the shader is layed out.  The linker will automatically remove
	// the redundant instructions, but you should make every effort to remove redundant code for speed.
	
	vector<DWORD>ThisShader;

	DWORD dwNumLights = m_DirectionalLights.size() + m_PointLights.size();
	bool bNeedLighting = m_Settings.m_bLighting && (dwNumLights > 0);
	bool bNeedSpecular = m_Settings.m_bSpecular && bNeedLighting;
	bool bNeedTexture = m_Settings.m_bTextures && m_bHasTextureCoords;
	bool bNeedCubeReflection = (m_Settings.m_TexGen == TEXGEN_CAMERASPACEREFLECTION) && bNeedTexture;
	bool bNeedFog = (m_Settings.m_Fog != FOG_NONE);
	bool bNeedRDotL = bNeedSpecular && m_Settings.m_bSpecularRDotL;
	bool bNeedEyeReflection = bNeedCubeReflection || bNeedRDotL;
	bool bNeedNormal = bNeedLighting || bNeedCubeReflection;
	bool bNeedVertex = bNeedLighting || bNeedCubeReflection || bNeedFog;
	bool bNeedEyeVector = (bNeedSpecular && (m_Settings.m_bLocalViewer || m_Settings.m_bSpecularRDotL)) || (bNeedCubeReflection) || (bNeedFog);
	bool bNeedBasis = (m_Settings.m_TexGen == TEXGEN_BLINNBUMPREFLECTION) && bNeedTexture;
	bool bNeedLit = true;
	bool bFirstLight = true;
	bool bLastLight = false;

	ThisShader.push_back(m_FragmentID[F_XFORM_WORLDVIEWPROJECTION]);

	if (m_Settings.m_bEyeSpace)
	{
		// Create some things we need
		if (bNeedVertex)
			ThisShader.push_back(m_FragmentID[F_CREATE_EYESPACE_VERTEX]);
		
		// Specular and reflections require an eye vector
		if (bNeedEyeVector)
			ThisShader.push_back(m_FragmentID[F_CREATE_EYESPACE_EYEVECTOR_LOCAL_VIEWER]);
		
		// Reflections and lights require a normal
		if (bNeedNormal)
			ThisShader.push_back(m_FragmentID[F_CREATE_EYESPACE_NORMAL]);
		
		if (bNeedFog)
			ThisShader.push_back(m_FragmentID[F_CALCULATE_LINEAR_RANGE_FOG]);

		if (bNeedEyeReflection)
		{
			// Camera space reflection is always done with local viewer.  This is practical because it can look
			// pretty aweful without it.
			ThisShader.push_back(m_FragmentID[F_CALCULATE_EYESPACE_REFLECTION]);
		}

		// If texgen is on, apply it
		if (bNeedCubeReflection)
		{
			ThisShader.push_back(m_FragmentID[F_CALCULATE_CUBEREFLECTION_VECTOR]);
		}

		
		if (bNeedLighting)
		{
			// Add the ambient color, and setup the specular power if needed
			ThisShader.push_back(m_FragmentID[F_SET_AMBIENT]);
			if (bNeedSpecular)
			{
				ThisShader.push_back(m_FragmentID[F_SET_POWER]);
			}

			tLightList::const_iterator itrLights = m_DirectionalLights.begin();
			while (itrLights != m_DirectionalLights.end())
			{
				if (m_PointLights.empty() && ((itrLights + 1) == m_DirectionalLights.end()))
				{
					bLastLight = true;
				}

				// Add a directional light fragment
				if (bNeedSpecular)
				{
					if (m_Settings.m_bSpecularRDotL)
					{
						ThisShader.push_back(m_FragmentID[F_CALCULATE_EYESPACE_DIRECTIONALLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_RDOTL]);
					}
					else
					{
						if (m_Settings.m_bLocalViewer)
						{
							ThisShader.push_back(m_FragmentID[F_CALCULATE_EYESPACE_DIRECTIONALLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_LOCALVIEWER]);
						}
						else
						{
							ThisShader.push_back(m_FragmentID[F_CALCULATE_EYESPACE_DIRECTIONALLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_NONLOCALVIEWER]);
						}
					}
				}
				else
				{
					ThisShader.push_back(m_FragmentID[F_CALCULATE_EYESPACE_DIRECTIONALLIGHT_DIFFUSE_INTENSITY]);
				}

				if (bNeedLit)
				{
					ThisShader.push_back(m_FragmentID[F_CLAMP_DIFFUSE_EXPONENTIATE_SPECULAR]);
				}

				if (bLastLight)
				{
					ThisShader.push_back(m_FragmentID[F_DIRLIGHT_ACCUMULATE_DIFFUSE_LAST]);
				}
				else
				{
					ThisShader.push_back(m_FragmentID[F_DIRLIGHT_ACCUMULATE_DIFFUSE]);
				}

				if (bNeedSpecular)
				{
					if (bFirstLight && bLastLight)
					{
						ThisShader.push_back(m_FragmentID[F_DIRLIGHT_ACCUMULATE_SPECULAR_FIRST_AND_LAST]);
					}
					else if (bFirstLight)
					{
						ThisShader.push_back(m_FragmentID[F_DIRLIGHT_ACCUMULATE_SPECULAR_FIRST]);
					}
					else if (bLastLight)
					{
						ThisShader.push_back(m_FragmentID[F_DIRLIGHT_ACCUMULATE_SPECULAR_LAST]);
					}
					else
					{
						ThisShader.push_back(m_FragmentID[F_DIRLIGHT_ACCUMULATE_SPECULAR]);
					}
				}

				bFirstLight = false;

				itrLights++;
			}

			itrLights = m_PointLights.begin();
			while (itrLights != m_PointLights.end())
			{
				if ((itrLights + 1) == m_PointLights.end())
				{
					bLastLight = true;
				}

				ThisShader.push_back(m_FragmentID[F_CALCULATE_EYESPACE_POINTLIGHT_VECTOR]);
				
				if (bNeedSpecular)
				{
					if (m_Settings.m_bSpecularRDotL)
					{
						ThisShader.push_back(m_FragmentID[F_CALCULATE_EYESPACE_POINTLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_RDOTL]);
					}
					else
					{
						if (m_Settings.m_bLocalViewer)
						{
							ThisShader.push_back(m_FragmentID[F_CALCULATE_EYESPACE_POINTLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_LOCALVIEWER]);
						}
						else
						{
							ThisShader.push_back(m_FragmentID[F_CALCULATE_EYESPACE_POINTLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_NONLOCALVIEWER]);
						}
					}
				}
				else
				{
					ThisShader.push_back(m_FragmentID[F_CALCULATE_EYESPACE_POINTLIGHT_DIFFUSE_INTENSITY]);
				}

				if (bNeedLit)
				{
					ThisShader.push_back(m_FragmentID[F_CLAMP_DIFFUSE_EXPONENTIATE_SPECULAR]);
				}

				if (bLastLight)
				{
					ThisShader.push_back(m_FragmentID[F_POINTLIGHT_ACCUMULATE_DIFFUSE_LAST]);
				}
				else
				{
					ThisShader.push_back(m_FragmentID[F_POINTLIGHT_ACCUMULATE_DIFFUSE]);
				}

				if (bNeedSpecular)
				{
					if (bFirstLight && bLastLight)
					{
						ThisShader.push_back(m_FragmentID[F_POINTLIGHT_ACCUMULATE_SPECULAR_FIRST_AND_LAST]);
					}
					else if (bFirstLight)
					{
						ThisShader.push_back(m_FragmentID[F_POINTLIGHT_ACCUMULATE_SPECULAR_FIRST]);
					}
					else if (bLastLight)
					{
						ThisShader.push_back(m_FragmentID[F_POINTLIGHT_ACCUMULATE_SPECULAR_LAST]);
					}
					else
					{
						ThisShader.push_back(m_FragmentID[F_POINTLIGHT_ACCUMULATE_SPECULAR]);
					}
				}

				bFirstLight = false;

				itrLights++;
			}
		}

		if (bNeedBasis)
		{
			ThisShader.push_back(m_FragmentID[F_CALCULATE_BASIS_VECTORS]);
		}
	}
	else
	// object space
	{
		if (bNeedEyeVector)
			ThisShader.push_back(m_FragmentID[F_CREATE_OBJECTSPACE_EYEVECTOR_LOCAL_VIEWER]);

		if (bNeedEyeReflection)
		{
			// Camera space reflection is always done with local viewer.  This is practical because it can look
			// pretty aweful without it.
			ThisShader.push_back(m_FragmentID[F_CALCULATE_OBJECTSPACE_REFLECTION]);
		}

		// If texgen is on, apply it
		if (bNeedCubeReflection)
		{
			ThisShader.push_back(m_FragmentID[F_CALCULATE_CUBEREFLECTION_VECTOR]);
		}

		if (bNeedFog)
			ThisShader.push_back(m_FragmentID[F_CALCULATE_LINEAR_RANGE_FOG]);

		if (bNeedLighting)
		{
			// Add the ambient color, and setup the specular power if needed
			ThisShader.push_back(m_FragmentID[F_SET_AMBIENT]);
			if (bNeedSpecular)
			{
				ThisShader.push_back(m_FragmentID[F_SET_POWER]);
			}

			tLightList::const_iterator itrLights = m_DirectionalLights.begin();
			while (itrLights != m_DirectionalLights.end())
			{
				if (m_PointLights.empty() && ((itrLights + 1) == m_DirectionalLights.end()))
				{
					bLastLight = true;
				}

				// Add a directional light fragment
				if (bNeedSpecular)
				{
					if (m_Settings.m_bSpecularRDotL)
					{
						ThisShader.push_back(m_FragmentID[F_CALCULATE_OBJECTSPACE_DIRECTIONALLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_RDOTL]);
					}
					else
					{
						if (m_Settings.m_bLocalViewer)
						{
							ThisShader.push_back(m_FragmentID[F_CALCULATE_OBJECTSPACE_DIRECTIONALLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_LOCALVIEWER]);
						}
						else
						{
							ThisShader.push_back(m_FragmentID[F_CALCULATE_OBJECTSPACE_DIRECTIONALLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_NONLOCALVIEWER]);
						}
					}
				}
				else
				{
					ThisShader.push_back(m_FragmentID[F_CALCULATE_OBJECTSPACE_DIRECTIONALLIGHT_DIFFUSE_INTENSITY]);
				}

				if (bNeedLit)
				{
					ThisShader.push_back(m_FragmentID[F_CLAMP_DIFFUSE_EXPONENTIATE_SPECULAR]);
				}

				if (bLastLight)
				{
					ThisShader.push_back(m_FragmentID[F_DIRLIGHT_ACCUMULATE_DIFFUSE_LAST]);
				}
				else
				{
					ThisShader.push_back(m_FragmentID[F_DIRLIGHT_ACCUMULATE_DIFFUSE]);
				}

				if (bNeedSpecular)
				{
					if (bFirstLight && bLastLight)
					{
						ThisShader.push_back(m_FragmentID[F_DIRLIGHT_ACCUMULATE_SPECULAR_FIRST_AND_LAST]);
					}
					else if (bFirstLight)
					{
						ThisShader.push_back(m_FragmentID[F_DIRLIGHT_ACCUMULATE_SPECULAR_FIRST]);
					}
					else if (bLastLight)
					{
						ThisShader.push_back(m_FragmentID[F_DIRLIGHT_ACCUMULATE_SPECULAR_LAST]);
					}
					else
					{
						ThisShader.push_back(m_FragmentID[F_DIRLIGHT_ACCUMULATE_SPECULAR]);
					}

				}
				
				bFirstLight = false;

				itrLights++;
			}

			itrLights = m_PointLights.begin();
			while (itrLights != m_PointLights.end())
			{
				if (((itrLights + 1) == m_PointLights.end()))
				{
					bLastLight = true;
				}

				ThisShader.push_back(m_FragmentID[F_CALCULATE_OBJECTSPACE_POINTLIGHT_VECTOR]);
				
				if (bNeedSpecular)
				{
					if (m_Settings.m_bSpecularRDotL)
					{
						ThisShader.push_back(m_FragmentID[F_CALCULATE_OBJECTSPACE_POINTLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_RDOTL]);
					}
					else
					{
						if (m_Settings.m_bLocalViewer)
						{
							ThisShader.push_back(m_FragmentID[F_CALCULATE_OBJECTSPACE_POINTLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_LOCALVIEWER]);
						}
						else
						{
							ThisShader.push_back(m_FragmentID[F_CALCULATE_OBJECTSPACE_POINTLIGHT_DIFFUSE_AND_SPECULAR_INTENSITY_NONLOCALVIEWER]);
						}
					}
				}
				else
				{
					ThisShader.push_back(m_FragmentID[F_CALCULATE_OBJECTSPACE_POINTLIGHT_DIFFUSE_INTENSITY]);
				}
	
				if (bNeedLit)
				{
					ThisShader.push_back(m_FragmentID[F_CLAMP_DIFFUSE_EXPONENTIATE_SPECULAR]);
				}

				if (bLastLight)
				{
					ThisShader.push_back(m_FragmentID[F_POINTLIGHT_ACCUMULATE_DIFFUSE_LAST]);
				}
				else
				{
					ThisShader.push_back(m_FragmentID[F_POINTLIGHT_ACCUMULATE_DIFFUSE]);
				}

				if (bNeedSpecular)
				{
					if (bFirstLight && bLastLight)
					{
						ThisShader.push_back(m_FragmentID[F_POINTLIGHT_ACCUMULATE_SPECULAR_FIRST_AND_LAST]);
					}
					else if (bFirstLight)
					{
						ThisShader.push_back(m_FragmentID[F_POINTLIGHT_ACCUMULATE_SPECULAR_FIRST]);
					}
					else if (bLastLight)
					{
						ThisShader.push_back(m_FragmentID[F_POINTLIGHT_ACCUMULATE_SPECULAR_LAST]);
					}
					else
					{
						ThisShader.push_back(m_FragmentID[F_POINTLIGHT_ACCUMULATE_SPECULAR]);
					}
				}

				bFirstLight = false;

				itrLights++;
			}
		}

		if (bNeedBasis)
		{
			ThisShader.push_back(m_FragmentID[F_CALCULATE_BASIS_VECTORS]);
		}

	}

	if (!bNeedLighting)
	{
		// Write out the final values.
		ThisShader.push_back(m_FragmentID[F_WRITE_DEFAULT_DIFFUSE]);
	}

	if (bNeedTexture)
	{
		// If no texgen mode is on, write the texture coordinates out directly
		if(m_Settings.m_TexGen == TEXGEN_NONE)
		{
			ThisShader.push_back(m_FragmentID[F_WRITE_DEFAULT_TEXTURE]);
		}
	}

	for (i = 0; i < ThisShader.size(); i++)
	{
		assert(ThisShader[i] != 0);
	}

	ThisShader.push_back(0);

	// Load the shader
	NVLinkBuffer* pShader;

	// Use this to reserve a range of constant/vertex registers - useful for indexed skinning, for example.
	// You can only have one range.
	// m_pLinker->ReserveConstantRange(3, 10);

	// Our vertex format will contain a position and a tex coord at 0 and 1.  To ensure that the linker
	// never allocates these registers to fragments that don't ever use FIXED_TEXTURE0, we reserve the 'slots'
	m_pLinker->ReserveVertexRange(0, 1);

	DWORD dwRet = m_pLinker->CreateBinaryShader(&ThisShader[0], &pShader);
	// In the failure case, update the text in the window
	if (dwRet != NVLINK_OK)
	{
		switch (dwRet)
		{
			case NVLINK_OUT_OF_MEMORY:
				m_strLastError = "Linker out of memory";
				break;

			case NVLINK_TOO_MANY_INSTRUCTIONS:
				m_strLastError = "Too many instructions";
				break;
			case NVLINK_TOO_MANY_ADDRESS_REGISTERS:
				m_strLastError = "Too many address registers required";
				break;

			case NVLINK_TOO_MANY_CONSTANTS:
				m_strLastError = "Too many constants required";
				break;
			
			case NVLINK_TOO_MANY_VERTICES:
				m_strLastError = "Too many vertex slots required";
				break;

			case NVLINK_TOO_MANY_REGISTERS:
				m_strLastError = "Too many temporary registers required";
				break;

			case NVLINK_NO_FRAGMENTS:
				m_strLastError = "Linker has no fragments";
				break;

			case NVLINK_INVALID_FRAGMENT:
				m_strLastError = "Invalid fragment ID passed to linker";
				break;

			default:
				m_strLastError = "Failed to link shader";
				break;
		}


		strOutFile = effect_api::GetModulePath() + "\\" + "output.nvv";
		hFile = CreateFile(strOutFile.c_str(), GENERIC_WRITE, 0, NULL, TRUNCATE_EXISTING, 0, NULL);
		WriteFile(hFile, const_cast<char*>(m_strLastError.c_str()), m_strLastError.length(), &dwNumWritten, NULL);		
		CloseHandle(hFile);

		if (!m_bSetShaderToShow)
		{
			m_strEffectVertexShader = GetFilePath("output.nvv");
			m_bSetShaderToShow = true;
		}

		// Dirty the vertex shaders so the effects browser reloads the file
		m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_VERTEXSHADERS;

		return E_FAIL;
	}

	// Write the created shader's source text to an output file
	NVLinkBuffer* pShaderText = NULL;
	if (m_pLinker->GetShaderSource(&pShaderText) == NVLINK_OK)
	{
		strOutFile = effect_api::GetModulePath() + "\\" + "output.nvv";
		hFile = CreateFile(strOutFile.c_str(), GENERIC_WRITE, 0, NULL, TRUNCATE_EXISTING, 0, NULL);
		WriteFile(hFile, pShaderText->GetPointer(), pShaderText->GetBufferSize(), &dwNumWritten, NULL);		
		CloseHandle(hFile);
		SAFE_RELEASE(pShaderText);

		if (!m_bSetShaderToShow)
		{
			m_strEffectVertexShader = GetFilePath("output.nvv");
			m_bSetShaderToShow = true;
		}

		// Dirty the vertex shaders so the effects browser reloads the file
		m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_VERTEXSHADERS;
	}

	UpdateMeshVertexSlots();

	// Store a list of constant slots for this shader
	for (i = 0; i < C_NUM_CONSTANTS; i++)
	{
		m_pLinker->GetConstantSlot(m_ConstantID[i], 0, &dwLocation);
		m_ConstantSlots[0][CLinkerConstantSlot(m_ConstantID[i], 0)] = dwLocation;
	}

	//Special ones with offsets
	for (i = 0; i < m_PointLights.size(); i++)
	{
		m_pLinker->GetConstantSlot(m_ConstantID[C_POINTLIGHT_POSITION], i, &dwLocation);
		m_ConstantSlots[0][CLinkerConstantSlot(m_ConstantID[C_POINTLIGHT_POSITION], i)] = dwLocation;

		m_pLinker->GetConstantSlot(m_ConstantID[C_POINTLIGHT_DIFFUSE], i, &dwLocation);
		m_ConstantSlots[0][CLinkerConstantSlot(m_ConstantID[C_POINTLIGHT_DIFFUSE], i)] = dwLocation;

		m_pLinker->GetConstantSlot(m_ConstantID[C_POINTLIGHT_SPECULAR], i, &dwLocation);
		m_ConstantSlots[0][CLinkerConstantSlot(m_ConstantID[C_POINTLIGHT_SPECULAR], i)] = dwLocation;

	}

	for (i = 0; i < m_DirectionalLights.size(); i++)
	{
		m_pLinker->GetConstantSlot(m_ConstantID[C_DIRLIGHT_DIRECTION], i, &dwLocation);
		m_ConstantSlots[0][CLinkerConstantSlot(m_ConstantID[C_DIRLIGHT_DIRECTION], i)] = dwLocation;

		m_pLinker->GetConstantSlot(m_ConstantID[C_DIRLIGHT_DIFFUSE], i, &dwLocation);
		m_ConstantSlots[0][CLinkerConstantSlot(m_ConstantID[C_DIRLIGHT_DIFFUSE], i)] = dwLocation;

		m_pLinker->GetConstantSlot(m_ConstantID[C_DIRLIGHT_SPECULAR], i, &dwLocation);
		m_ConstantSlots[0][CLinkerConstantSlot(m_ConstantID[C_DIRLIGHT_SPECULAR], i)] = dwLocation;

		m_pLinker->GetConstantSlot(m_ConstantID[C_DIRLIGHT_HALFVECTOR], i, &dwLocation);
		m_ConstantSlots[0][CLinkerConstantSlot(m_ConstantID[C_DIRLIGHT_HALFVECTOR], i)] = dwLocation;
	}

	DWORD* pBuff = (DWORD*)pShader->GetPointer();
	hr = m_pD3DDev->CreateVertexShader(&m_MeshDeclaration[0], (DWORD*)pShader->GetPointer(), &m_dwPass0Shader, 0);
	SAFE_RELEASE(pShader);
	if (FAILED(hr))
	{
		m_strLastError = "Failed CreateVertexShader on linked shader";
		return hr;
	}

	// Get a list of sections for pass 0
	tSectionList& Sections = m_pMesh->GetPass(0)->GetSections();
	tSectionList::iterator itrSections = Sections.begin();

	// Walk all the model sections in this pass
	while (itrSections != Sections.end())
	{
		// Set the constants and vertex shaders in this section
		NVMaterialResource* pMaterial = static_cast<NVMaterialResource*>(NVRESOURCEMANAGER.FindResource((*itrSections).GetMaterialID()));
		if (pMaterial)
		{
			NVConstant* pConstant;
			tConstantInfoToSlot::iterator itrConstant;

			// Make sure we have a specular and a power constant in the mesh
			pMaterial->AddConstant("specular", D3DXVECTOR4(0.3f, 0.3f, 0.3f, 0.0f), NV_NO_CONSTANT);
			pMaterial->AddConstant("power", D3DXVECTOR4(0.0f, 0.0f, 0.0f, 25.0f), NV_NO_CONSTANT);

			// Specular off by default
			pMaterial->AddRenderState(D3DRS_SPECULARENABLE, FALSE);

			// Tell the mesh where to place it's constants
			pConstant = pMaterial->FindConstant("diffuse");
			if (pConstant)
			{
				// Not loading the material into constant memory - we are premodulating it with
				// the light color - see the tick function.
				pConstant->SetLocation(NV_NO_CONSTANT);
			}

			pConstant = pMaterial->FindConstant("specular");
			if (pConstant)
			{
				// Not loading the material into constant memory - we are premodulating it with
				// the light color - see the tick function.
				pConstant->SetLocation(NV_NO_CONSTANT);

				if (bNeedSpecular)
				{
					// If we require any specular at all, enable the specular renderstate
					itrConstant = m_ConstantSlots[0].find(CLinkerConstantSlot(m_ConstantID[C_POINTLIGHT_SPECULAR], 0));
					assert(itrConstant != m_ConstantSlots[0].end());
					if (itrConstant->second != NVLINK_NOT_USED)
					{
						pMaterial->AddRenderState(D3DRS_SPECULARENABLE, TRUE);
					}

					itrConstant = m_ConstantSlots[0].find(CLinkerConstantSlot(m_ConstantID[C_DIRLIGHT_SPECULAR], 0));
					assert(itrConstant != m_ConstantSlots[0].end());
					if (itrConstant->second != NVLINK_NOT_USED)
					{
						pMaterial->AddRenderState(D3DRS_SPECULARENABLE, TRUE);
					}
				}
			}

			pConstant = pMaterial->FindConstant("power");
			if (pConstant)
			{
				itrConstant = m_ConstantSlots[0].find(CLinkerConstantSlot(m_ConstantID[C_MAT_POWER], 0));
				assert(itrConstant != m_ConstantSlots[0].end());
				if (itrConstant->second != NVLINK_NOT_USED)
				{
					pConstant->SetLocation(itrConstant->second);
				}
				else
				{
					pConstant->SetLocation(NV_NO_CONSTANT);
				}
			}

			// Setup the vertex shader, and turn off pixel shader by default
			pMaterial->AddVertexShader(m_dwPass0Shader);
			pMaterial->AddPixelShader(0);

			pMaterial->RestoreTexture(0);

			pMaterial->AddTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
			pMaterial->AddTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);

			if(bNeedCubeReflection)
			{
				pMaterial->AddTexture(0, NVRESOURCEMANAGER.FindResourceID("CubeMap"));
				pMaterial->AddTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
				pMaterial->AddTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

			}
			else if (bNeedBasis)
			{
				string strNormalMapTexture;
				LPDIRECT3DBASETEXTURE8 pNormalMapTexture = NULL;
				
				// If the mesh has a texture, add a bumpmap for it
				if (NVTextureResource::GetTexture(pMaterial->GetTextureID(0)) != NULL)
				{
					if (m_GeneratedNormalMapID == NVINVALID_RESOURCEID)
					{
						m_GeneratedNormalMapID = NVRESOURCEMANAGER.AddResource(new NVTextureNormalMapFromResource(m_pD3DDev, pMaterial->GetTextureID(0), (D3DFORMAT)MAKEFOURCC('N', 'V', 'H', 'S'), D3DPOOL_DEFAULT));
					}
					pMaterial->AddTexture(0, m_GeneratedNormalMapID);
				}
				else
				{
					pMaterial->AddTexture(0, NVRESOURCEMANAGER.FindResourceID("DefaultNormalMap"));
				}

				pMaterial->AddTexture(3, NVRESOURCEMANAGER.FindResourceID("CubeMap"));
				pMaterial->AddPixelShader(m_dwBumpPixelShader);
				pMaterial->AddTextureStageState(3, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
				pMaterial->AddTextureStageState(3, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

			}
			else
			{
				pMaterial->RestoreTexture(0);
			}

			if ((pMaterial->GetTextureID(0) == NVINVALID_RESOURCEID) || !bNeedTexture)
			{
				pMaterial->AddTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
			}
			else
			{
				pMaterial->AddTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			}
		}
		itrSections++;
	}

	// Find second pass if there is one.  The sphere will have done this.
	if (m_pMesh->GetPass(1))
	{

		// Pass 2 is more simple, we just want the transform and the write of the texture coordinates
		ThisShader.clear();
		if (bNeedTexture)
		{
			ThisShader.push_back(m_FragmentID[F_WRITE_DEFAULT_TEXTURE]);
		}

		// Need to fog the decal
		if (bNeedFog)
		{
			// In eye space put the vertex down and calculate the eye vector
			if (m_Settings.m_bEyeSpace)
			{
				ThisShader.push_back(m_FragmentID[F_CREATE_EYESPACE_VERTEX]);
				ThisShader.push_back(m_FragmentID[F_CREATE_EYESPACE_EYEVECTOR_LOCAL_VIEWER]);
			}
			// In object space get the eye vector 
			else
			{
				ThisShader.push_back(m_FragmentID[F_CREATE_OBJECTSPACE_EYEVECTOR_LOCAL_VIEWER]);
			}
			// Calculate the fog
			ThisShader.push_back(m_FragmentID[F_CALCULATE_LINEAR_RANGE_FOG]);
		}

		ThisShader.push_back(m_FragmentID[F_XFORM_WORLDVIEWPROJECTION]);
		ThisShader.push_back(0);

		dwRet = m_pLinker->CreateBinaryShader(&ThisShader[0], &pShader);
		pBuff = (DWORD*)pShader->GetPointer();
		hr = m_pD3DDev->CreateVertexShader(&m_MeshDeclaration[0], (DWORD*)pShader->GetPointer(), &m_dwPass1Shader, 0);
		SAFE_RELEASE(pShader);
		if (FAILED(hr))
		{
			m_strLastError = "Failed CreateVertexShader on linked shader";
			return hr;
		}

		// Store a list of constant slots for this shader
		for (i = 0; i < C_NUM_CONSTANTS; i++)
		{
			DWORD dwLocation;
			m_pLinker->GetConstantSlot(m_ConstantID[i], 0, &dwLocation);
			m_ConstantSlots[1][CLinkerConstantSlot(m_ConstantID[i], 0)] = dwLocation;
		}

		// Now setup the mesh
		tSectionList& Sections = m_pMesh->GetPass(1)->GetSections();
		tSectionList::iterator itrSections = Sections.begin();

		// Walk all the model sections in this pass
		while (itrSections != Sections.end())
		{
			// Set the constants and vertex shaders in this section
			NVMaterialResource* pMaterial = static_cast<NVMaterialResource*>(NVRESOURCEMANAGER.FindResource((*itrSections).GetMaterialID()));
			if (pMaterial)
			{
				// Setup the vertex shader
				pMaterial->AddVertexShader(m_dwPass1Shader);

				string strGlossTexture;
				LPDIRECT3DTEXTURE8 pGlossTexture = NULL;

				pMaterial->AddRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
				pMaterial->AddRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
				pMaterial->AddRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

				// Add the default gloss map
				pMaterial->AddTexture(0, NVRESOURCEMANAGER.FindResourceID("DefaultGlossMap"));
				
				// Setup texture properties
				pMaterial->AddTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
			}
			itrSections++;
		}
	}

	// Prepare the mesh for rendering
	m_pMesh->PrepareDeviceObjects(m_pD3DDev);

	return S_OK;
}

HRESULT CShaderLinkerDemo::LoadMesh(const string& strFileName)
{
	int Pos = strFileName.find_last_of('.', strFileName.size());
	string strExtension = strFileName.substr(Pos + 1, strFileName.size() - Pos);

	if (m_pMesh)
	{
		SAFE_DELETE(m_pMesh);
	}

	if (m_GeneratedNormalMapID != NVINVALID_RESOURCEID)
	{
		NVRESOURCEMANAGER.RemoveResource(m_GeneratedNormalMapID);
		m_GeneratedNormalMapID = NVINVALID_RESOURCEID;
	}

	m_pMesh = new NVComplexMesh();

	bool bOK;
	if (strFileName == "")
	{
		bOK = m_pMesh->CreateFromSphere(1.0f, 40, 80, D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f));
		if (bOK)
		{
			m_pMesh->DuplicatePass(0, 1);
		}
	}
	else if (strExtension.compare("x") == 0)
	{
		bOK = m_pMesh->CreateFromXFile(m_pD3DDev, strFileName.c_str());
		if (bOK)
		{
			// If the model has texture coords...
			if (m_pMesh->FindVertexData("texture0"))
			{
				bool bFoundTex = false;

				tSectionList& Sections = m_pMesh->GetPass(0)->GetSections();
				tSectionList::iterator itrSections = Sections.begin();	
	
				// Walk all the model sections in this pass
				while (itrSections != Sections.end())
				{
		
					// Set the constants and vertex shaders in this section
					NVMaterialResource* pMaterial = static_cast<NVMaterialResource*>(NVRESOURCEMANAGER.FindResource((*itrSections).GetMaterialID()));
					if (pMaterial)
					{
						if (NVTextureResource::GetTexture(pMaterial->GetTextureID(0)) != NULL)
							bFoundTex = true;
					}
					itrSections++;
				}

				// This mesh has texture coords, but no texture.  We will be using the default NVIDIA bump
				// texture
				if (!bFoundTex)
				{
					// Add a second pass for the gloss map
					m_pMesh->DuplicatePass(0, 1);
				}
			}
		}
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
		SAFE_DELETE(m_pMesh);
		return E_FAIL;
	}
	m_strFileName = strFileName;

	// We require for the shader that these components are present.
	// If they are already there, then these calls won't do anything
	if (!m_pMesh->FindVertexData("normal"))
	{
		m_pMesh->AddVertexData("normal", NVCOMPLEXMESH_FLOAT3);
		m_pMesh->GenerateNormals();
	}

	if (m_pMesh->FindVertexData("texture0"))
	{
		// Add basis vectors.  We use the normal as our SxT vector
		m_pMesh->AddVertexData("sbasis0", NVCOMPLEXMESH_FLOAT3);
		m_pMesh->AddVertexData("tbasis0", NVCOMPLEXMESH_FLOAT3);
		m_pMesh->GenerateBasis(0);

		m_bHasTextureCoords = true;
		m_Settings.m_bEnableBump = m_bSupportsNormalMap;
		m_Settings.m_bEnableTextures = true;
	}
	else
	{
		m_bHasTextureCoords = false;
		m_Settings.m_bEnableBump = false;
		m_Settings.m_bTextures = false;
		m_Settings.m_bEnableTextures = false;
		m_Settings.m_TexGen = TEXGEN_NONE;
	}

	// Get the bounding info and scale the model
	NVComplexMeshBounds Bounds;
	m_pMesh->ComputeBounds(Bounds);
	m_pMesh->Translate(-Bounds.m_vecCenter);
	m_pMesh->Scale(0.8f / Bounds.m_fRadius);

	//m_pMesh->UnIndex();

	m_DialogFunc.SetSettings(&m_Settings);

	return S_OK;
}

void CShaderLinkerDemo::PropertyUpdateCallback(const EBProperty* pProperty, bool bWritten)
{
	if (!bWritten)
		return;

	if (pProperty->IsKindOf(EBTYPE_TRIGGER_PROP))
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
			NVRESOURCEMANAGER.DisposeResources();
			LoadMesh(&FileName[0]);
		}
		else
		{
			// Might be OK if the user hit cancel
			DWORD dwErr = CommDlgExtendedError();
		}

		m_eDisplayOption = DISPLAY_MODEL;
		m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;

		UpdateShader();
		m_pUI->Reset();
	}
	else
	{
		EBString strProp = pProperty->GetPropertyName();
		if (strProp == EBString(STR_DISPLAYOPTIONS))
		{
			switch(m_eDisplayOption)
			{
				case DISPLAY_MODEL:
				{
					NVRESOURCEMANAGER.DisposeResources();
					LoadMesh(GetFilePath("bigship1.x"));
					UpdateShader();
					m_pUI->Reset();
				}
				break;
				case DISPLAY_SPHERE:
				{
					NVRESOURCEMANAGER.DisposeResources();
					LoadMesh("");
					UpdateShader();
					m_pUI->Reset();
				}
				break;
				default:
				{
					assert(0);
				}
				break;
			}
		}
	}
}

// ------------------------------------------------------------------------------
// CShaderLinkerDemo::Free
//
// Description: Called when we switch away from this demo.	Free up all the 
//		memory that is in use.
// ------------------------------------------------------------------------------ 
HRESULT CShaderLinkerDemo::Free()
{
	m_PointLights.clear();
	m_DirectionalLights.clear();
	FreeLightObjects();

	// Delete mesh
	SAFE_DELETE(m_pMesh);

	SAFE_RELEASE(m_pWorldBoxVertices);
	SAFE_RELEASE(m_pWorldBoxIndices);

	if (m_pD3DDev)
	{
		if (m_dwPass0Shader)
			m_pD3DDev->DeleteVertexShader(m_dwPass0Shader);
		m_dwPass0Shader = 0;

		if (m_dwPass1Shader)
			m_pD3DDev->DeleteVertexShader(m_dwPass1Shader);
		m_dwPass1Shader = 0;

		if (m_dwBumpPixelShader)
			m_pD3DDev->DeletePixelShader(m_dwBumpPixelShader);
		m_dwBumpPixelShader = 0;

		SAFE_RELEASE(m_pD3DDev);
	}
	
	// Free the linker and unload the library
	SAFE_RELEASE(m_pLinker);

	if (m_hLinker)
	{
		FreeLibrary(m_hLinker);
		m_hLinker = NULL;
	}

	delete m_pUI, m_pUI = 0;

	// Check the function pointer to ensure we got as far
	// as loading it.
	if (m_DialogFunc.Free)
	{
		m_DialogFunc.Free();
		ZeroMemory(&m_DialogFunc, sizeof(m_DialogFunc));
	}

	if (m_hDialog)
	{
		FreeLibrary(m_hDialog);
		m_hDialog = NULL;
	}

	// Final step, release the resource and state managers
	SAFE_DELETE(g_pNVStateManager);
	SAFE_DELETE(g_pNVResourceManager);

	return S_OK;
}

// ------------------------------------------------------------------------------
// CShaderLinkerDemo::Start
//
// Description: Called to reset
// ------------------------------------------------------------------------------ 
HRESULT CShaderLinkerDemo::Start()
{
	return S_OK;
}


HRESULT CShaderLinkerDemo::DrawCubeEnvironment()
{
	// We used fixed-function pipeline for world environment
	D3DXMATRIX matWorld;
	D3DXMATRIX matIdentity;
	D3DXMatrixScaling(&matWorld, 10.0f, 10.0f, 10.0f);
	matWorld = m_matCubeRotation * matWorld;

	// Setup required state
	NVSTATEMANAGER.SaveTransform(D3DTS_WORLD, &matWorld);
	NVSTATEMANAGER.SaveTransform(D3DTS_VIEW, &m_matView);
	NVSTATEMANAGER.SaveTransform(D3DTS_PROJECTION, &m_matProjection);
	NVSTATEMANAGER.SaveRenderState(D3DRS_ZENABLE, FALSE);
	NVSTATEMANAGER.SaveRenderState(D3DRS_ZWRITEENABLE, FALSE);
	NVSTATEMANAGER.SaveRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	NVSTATEMANAGER.SaveTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
	NVSTATEMANAGER.SaveVertexShader(D3DFVF_XYZ  | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0));
	NVSTATEMANAGER.SavePixelShader(0);
	NVSTATEMANAGER.SaveTexture(0, NVTextureResource::GetTexture("CubeMap"));
	
	NVSTATEMANAGER.SetStreamSource(0, m_pWorldBoxVertices, sizeof(WorldBoxVertex));
	NVSTATEMANAGER.SetIndices(m_pWorldBoxIndices, 0);
 
	NVSTATEMANAGER.DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, NUM_CUBE_VERTICES, 0, 2 * 6);

	// Restore the state
	NVSTATEMANAGER.RestoreTransform(D3DTS_WORLD);
	NVSTATEMANAGER.RestoreTransform(D3DTS_VIEW);
	NVSTATEMANAGER.RestoreTransform(D3DTS_PROJECTION);
	NVSTATEMANAGER.RestoreRenderState(D3DRS_ZENABLE);
	NVSTATEMANAGER.RestoreRenderState(D3DRS_ZWRITEENABLE);
	NVSTATEMANAGER.RestoreRenderState(D3DRS_CULLMODE);
	NVSTATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLOROP);
	NVSTATEMANAGER.RestoreTextureStageState(0, D3DTSS_ALPHAOP);
	NVSTATEMANAGER.RestoreTextureStageState(0, D3DTSS_ADDRESSU);
	NVSTATEMANAGER.RestoreTextureStageState(0, D3DTSS_ADDRESSV);
	NVSTATEMANAGER.RestoreVertexShader();
	NVSTATEMANAGER.RestorePixelShader();
	NVSTATEMANAGER.RestoreTexture(0);


	return S_OK;
}


void CShaderLinkerDemo::ConditionalSetConstant(DWORD dwPass, eConstantName Name, LONG lOffset, const void* pValue)
{
	// Should always find it wether or not it has a slot.
	tConstantInfoToSlot::iterator itrConstant = m_ConstantSlots[dwPass].find(CLinkerConstantSlot(m_ConstantID[Name], lOffset));
	assert(itrConstant != m_ConstantSlots[dwPass].end());

	if (itrConstant->second != NVLINK_NOT_USED)
	{
		NVSTATEMANAGER.SetVertexShaderConstant(itrConstant->second, pValue, 1);
	}
}

// ------------------------------------------------------------------------------
// CShaderLinkerDemo::Tick
//
// Description: This function is called every frame.  Here is where we write
//		and update constants, clear the z- and back-buffer and render our 
//		primitives
// ------------------------------------------------------------------------------ 

HRESULT CShaderLinkerDemo::Tick(EBTimer* pTimer)
{
	DWORD i;
	static const float DEGREES_PER_SECOND = 90.0f;
	D3DXMATRIX WorldViewProjection, WorldViewIT, WorldView, WorldViewI, WorldI, World;
	D3DXMATRIX WorldViewCubeMap;
	HRESULT hr = S_OK;

	NVSTATEMANAGER.SaveRenderState(D3DRS_FILLMODE, (m_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID));
	NVSTATEMANAGER.SaveRenderState(D3DRS_FOGENABLE, FALSE);

	m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00BFBFBF, 1.0f, 0L);

	m_DialogFunc.GetSettings(&m_Settings);
	if (m_Settings.m_bDirty)
	{
		hr = UpdateLights();
		if (FAILED(hr))
			return hr;

		hr = UpdateShader();
		if (FAILED(hr))
			return hr;

	}

	if (m_dwPass0Shader == 0)
		return S_OK;

	if (m_pMesh->GetPass(1) && m_dwPass1Shader == 0)
		return S_OK;

	// Get the matrices
	m_matWorld = m_pUI->GetRotationMatrix() * m_pUI->GetTranslationMatrix();
#define LIMIT_TRANSLATION(mat, xmin, ymin, zmin, xmax, ymax, zmax) \
	if (mat._41 < xmin) mat._41 = xmin;		\
	if (mat._42 < ymin) mat._42 = ymin;		\
	if (mat._43 < zmin) mat._43 = zmin;		\
	if (mat._41 > xmax) mat._41 = xmax;		\
	if (mat._42 > ymax) mat._42 = ymax;		\
	if (mat._43 > zmax) mat._43 = zmax;
	LIMIT_TRANSLATION(m_matWorld, -1.0f, -1.0f, -2.0f, 1.0f, 1.0f, 3.0f);

	D3DXMatrixLookAtLH(&m_matView, &m_Eye, &m_LookAt, &m_Up);

	WorldView = m_matWorld * m_matView;
	WorldViewProjection = WorldView * m_matProjection;

	// Calculate worldview inverse transpose
	D3DXMatrixInverse(&WorldViewI, NULL, &WorldView);
	D3DXMatrixTranspose(&WorldViewIT, &WorldViewI);

	// Inverse world
	D3DXMatrixInverse(&WorldI, NULL, &m_matWorld);


	if((m_Settings.m_TexGen != TEXGEN_NONE) && m_Settings.m_bTextures)
	{
		DrawCubeEnvironment();
	}
	
	// Shader always requires these constants, so add them
	ConditionalSetConstant(0, C_CONSTANTS, 0, D3DXVECTOR4(0.0f, 1.0f, m_fBumpScale, 0.0f));

	D3DXVECTOR4 EyeVectorInObject;
	D3DXVECTOR3 EyeVectorInEye(0.0f, 0.0f, -1.0f);
	D3DXVec3TransformNormal((D3DXVECTOR3*)&EyeVectorInObject, &EyeVectorInEye, &WorldViewI);
	EyeVectorInObject.w = 1.0f;
	D3DXVec3Normalize((D3DXVECTOR3*)&EyeVectorInObject, (D3DXVECTOR3*)&EyeVectorInObject);
	ConditionalSetConstant(0, C_OBJECTSPACE_EYEVECTOR, 0, &EyeVectorInObject);

	D3DXVECTOR4 EyeInObject;
	D3DXVec3Transform(&EyeInObject, &m_Eye, &WorldI);
	ConditionalSetConstant(0, C_OBJECTSPACE_EYEPOSITION, 0, &EyeInObject);

	ConditionalSetConstant(0, C_EYESPACE_EYEVECTOR, 0, &EyeVectorInEye);

	// Store them in the constant area
	D3DXMatrixTranspose(&WorldViewProjection, &WorldViewProjection);
	ConditionalSetConstant(0, C_WORLDVIEWPROJECTION_0, 0, &WorldViewProjection._11);
	ConditionalSetConstant(0, C_WORLDVIEWPROJECTION_1, 0, &WorldViewProjection._21);
	ConditionalSetConstant(0, C_WORLDVIEWPROJECTION_2, 0, &WorldViewProjection._31);
	ConditionalSetConstant(0, C_WORLDVIEWPROJECTION_3, 0, &WorldViewProjection._41);
	D3DXMatrixTranspose(&WorldViewProjection, &WorldViewProjection);
	
	D3DXMatrixTranspose(&WorldViewIT, &WorldViewIT);
	ConditionalSetConstant(0, C_WORLDVIEWIT_0, 0, &WorldViewIT._11);
	ConditionalSetConstant(0, C_WORLDVIEWIT_1, 0, &WorldViewIT._21);
	ConditionalSetConstant(0, C_WORLDVIEWIT_2, 0, &WorldViewIT._31);
	D3DXMatrixTranspose(&WorldViewIT, &WorldViewIT);
	
	D3DXMatrixTranspose(&WorldView, &WorldView);
	ConditionalSetConstant(0, C_WORLDVIEW_0, 0, &WorldView._11);
	ConditionalSetConstant(0, C_WORLDVIEW_1, 0, &WorldView._21);
	ConditionalSetConstant(0, C_WORLDVIEW_2, 0, &WorldView._31);
	ConditionalSetConstant(0, C_WORLDVIEW_3, 0, &WorldView._41);
	D3DXMatrixTranspose(&WorldView, &WorldView);

	WorldViewCubeMap = WorldView * m_matCubeRotation;
	D3DXMatrixTranspose(&WorldViewCubeMap, &WorldViewCubeMap);
	ConditionalSetConstant(0, C_WORLDVIEWCUBEMAP_0, 0, &WorldViewCubeMap._11);
	ConditionalSetConstant(0, C_WORLDVIEWCUBEMAP_1, 0, &WorldViewCubeMap._21);
	ConditionalSetConstant(0, C_WORLDVIEWCUBEMAP_2, 0, &WorldViewCubeMap._31);
	D3DXMatrixTranspose(&WorldViewCubeMap, &WorldViewCubeMap);

	D3DXMATRIX matCubeMatrix = m_matCubeRotation;
	if (!m_Settings.m_bEyeSpace)
	{
		matCubeMatrix = WorldView * matCubeMatrix;
	}
	D3DXMatrixTranspose(&matCubeMatrix, &matCubeMatrix);
	ConditionalSetConstant(0, C_CUBEMATRIX_0, 0, &matCubeMatrix._11);
	ConditionalSetConstant(0, C_CUBEMATRIX_1, 0, &matCubeMatrix._21);
	ConditionalSetConstant(0, C_CUBEMATRIX_2, 0, &matCubeMatrix._31);
	D3DXMatrixTranspose(&matCubeMatrix, &matCubeMatrix);

	NVSTATEMANAGER.SetRenderState(D3DRS_FOGENABLE, (m_Settings.m_Fog == FOG_NONE) ? FALSE : TRUE);
	ConditionalSetConstant(0, C_FOGCONST, 0, &m_vecFogData.x);

	// Update the light positions based on their rotation vectors
	tLightList::iterator itrLights;
	itrLights = m_DirectionalLights.begin();
	while (itrLights != m_DirectionalLights.end())
	{
		D3DXMATRIX matRotate;

		if (!m_bPause)
		{
			// Calculate new light location. First translate back to origin
			(*itrLights).SetLightTranslation(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

			// Calculate new light location
			D3DXVECTOR3 Position = (*itrLights).GetLightPosition();

			D3DXMatrixRotationAxis(&matRotate, &(*itrLights).GetLightRotateAxis(), D3DXToRadian(DEGREES_PER_SECOND / pTimer->GetFramesPerSec()));
			D3DXVec3TransformNormal(&Position, &Position, &matRotate);

			(*itrLights).SetLightPosition(Position);
			(*itrLights).SetLightDirection(-Position);

		}

		// Add the world translation, because we want the lights to move with the model
		(*itrLights).SetLightTranslation(D3DXVECTOR3(m_matWorld._41, m_matWorld._42, m_matWorld._43));

		(*itrLights).SetViewMatrix(m_matView);
		(*itrLights).SetWorldMatrix(m_matWorld);

		itrLights++;
	};
		
	itrLights = m_PointLights.begin();
	while (itrLights != m_PointLights.end())
	{
		D3DXMATRIX matRotate;

		if (!m_bPause)
		{
			// Calculate new light location. First translate back to origin
			(*itrLights).SetLightTranslation(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
			
			D3DXVECTOR3 Position = (*itrLights).GetLightPosition();

			D3DXMatrixRotationAxis(&matRotate, &(*itrLights).GetLightRotateAxis(), D3DXToRadian(DEGREES_PER_SECOND / pTimer->GetFramesPerSec()));
			D3DXVec3TransformNormal(&Position, &Position, &matRotate);
			(*itrLights).SetLightPosition(Position);
			(*itrLights).SetLightDirection(-Position);

		}

		// Add the world translation, because we want the lights to move with the model
		(*itrLights).SetLightTranslation(D3DXVECTOR3(m_matWorld._41, m_matWorld._42, m_matWorld._43));

		(*itrLights).SetViewMatrix(m_matView);
		(*itrLights).SetWorldMatrix(m_matWorld);


		itrLights++;
	};

	// Store the light locations
	LightSpace Space = (m_Settings.m_bEyeSpace) ? LIGHT_EYE : LIGHT_OBJECT;
	for (i = 0; i < m_DirectionalLights.size(); i++)
	{
		D3DXVECTOR4 LightDirection = m_DirectionalLights[i].GetLightSpaceDirection(Space);
		ConditionalSetConstant(0, C_DIRLIGHT_DIRECTION, i, &LightDirection);

		// With non-local viewer infinite (directional) lights, we can have a constant half vector
		if (!m_Settings.m_bLocalViewer)
		{
			D3DXVECTOR4 HalfVector;
			if (m_Settings.m_bEyeSpace)
			{
				// In eye space, eye vector is to the eye
				HalfVector = LightDirection + D3DXVECTOR4(0.0f, 0.0f, -1.0f, 0.0f);
			}
			else
			{
				// In object space, we just calculated the eye vector
				HalfVector = LightDirection + EyeVectorInObject;
			}

			D3DXVec3Normalize((D3DXVECTOR3*)&HalfVector, (D3DXVECTOR3*)&HalfVector);
			HalfVector.w = 1.0f;
			ConditionalSetConstant(0, C_DIRLIGHT_HALFVECTOR, i, &HalfVector);
		}
	}

	for (i = 0; i < m_PointLights.size(); i++)
	{
		ConditionalSetConstant(0, C_POINTLIGHT_POSITION, i, &m_PointLights[i].GetLightSpacePosition(Space));
	}

	if (m_pMesh)
	{
		// Reset the counters
		m_pMesh->ResetStats();

		// Get a list of sections.  We assume we only have pass1, because we created the mesh from
		// a mesh file
		// Walk each diffuse material
		tSectionList& Sections = m_pMesh->GetPass(0)->GetSections();
		tSectionList::iterator itrSections = Sections.begin();

		// Walk all the model sections in this pass
		while (itrSections != Sections.end())
		{
			// Set the constants and vertex shaders in this section
			NVMaterialResource* pMaterial = static_cast<NVMaterialResource*>(NVRESOURCEMANAGER.FindResource((*itrSections).GetMaterialID()));
			assert(pMaterial);

			NVConstant* pConstant;
			D3DXVECTOR4 Diffuse;
			D3DXVECTOR4 Specular;
			D3DXVECTOR4 Ambient;

			// Tell the mesh where to place it's constants
			pConstant = pMaterial->FindConstant("diffuse");
			Diffuse = pConstant->GetConstant();

			pConstant = pMaterial->FindConstant("specular");
			Specular = pConstant->GetConstant();

			for (i = 0; i < m_DirectionalLights.size(); i++)
			{
				D3DXVECTOR4 LightDiffuse = (D3DXVECTOR4)ColorModulate(m_DirectionalLights[i].GetLightDiffuse(), (D3DXVECTOR3)Diffuse);
				D3DXVECTOR4 LightSpecular = (D3DXVECTOR4)ColorModulate(m_DirectionalLights[i].GetLightSpecular(), (D3DXVECTOR3)Specular);
				ConditionalSetConstant(0, C_DIRLIGHT_DIFFUSE, i, &LightDiffuse);
				ConditionalSetConstant(0, C_DIRLIGHT_SPECULAR, i, &LightSpecular);
			}

			for (i = 0; i < m_PointLights.size(); i++)
			{
				D3DXVECTOR4 LightDiffuse = (D3DXVECTOR4)ColorModulate(m_PointLights[i].GetLightDiffuse(), (D3DXVECTOR3)Diffuse);
				D3DXVECTOR4 LightSpecular = (D3DXVECTOR4)ColorModulate(m_PointLights[i].GetLightSpecular(), (D3DXVECTOR3)Specular);
				ConditionalSetConstant(0, C_POINTLIGHT_DIFFUSE, i, &LightDiffuse);
				ConditionalSetConstant(0, C_POINTLIGHT_SPECULAR, i, &LightSpecular);
			}

			// Finaly, setup the ambient light by multiplying it by the material's diffuse color
			// If lighting is off, we setup the ambient to be full on.
			D3DXVECTOR4 LightAmbient;
			if (m_Settings.m_bWhiteLight)
			{
				LightAmbient = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
			}
			else
			{
				if (m_Settings.m_bLighting)
				{
					LightAmbient = (D3DXVECTOR4)ColorModulate(m_AmbientLight, (D3DXVECTOR3)Diffuse);
				}
				else
				{
					LightAmbient = (D3DXVECTOR4)Diffuse;
				}
			}
			ConditionalSetConstant(0, C_AMBIENT_LIGHT, 0, &LightAmbient);

			pMaterial->SaveMaterial();
						
			m_pMesh->GetPass(0)->RenderSection(m_pD3DDev, itrSections);
			
			pMaterial->RestoreMaterial();

			itrSections++;
		}
	}

	// Draw the second pass if we added one.
	// It will be the gloss map, which is applied with a simple transform and a fog application
	if (m_pMesh->GetPass(1) && m_Settings.m_bTextures)
	{
		ConditionalSetConstant(1, C_FOGCONST, 0, &m_vecFogData.x);
		ConditionalSetConstant(1, C_OBJECTSPACE_EYEPOSITION, 0, &EyeInObject);

		// Setup the transform in the constant slots for this pass
		D3DXMatrixTranspose(&WorldViewProjection, &WorldViewProjection);
		ConditionalSetConstant(1, C_WORLDVIEWPROJECTION_0, 0, &WorldViewProjection._11);
		ConditionalSetConstant(1, C_WORLDVIEWPROJECTION_1, 0, &WorldViewProjection._21);
		ConditionalSetConstant(1, C_WORLDVIEWPROJECTION_2, 0, &WorldViewProjection._31);
		ConditionalSetConstant(1, C_WORLDVIEWPROJECTION_3, 0, &WorldViewProjection._41);
		D3DXMatrixTranspose(&WorldViewProjection, &WorldViewProjection);
	
		m_pMesh->GetPass(1)->Render(m_pD3DDev);
	}

	m_dwNumTriangles = m_pMesh->GetTrianglesDrawn();
	m_dwNumVertices = m_pMesh->GetVerticesDrawn();

	NVSTATEMANAGER.SetRenderState(D3DRS_FOGENABLE, FALSE);

	// Draw the lights themselves
	if (m_Settings.m_bLighting)
		DrawLightObjects();

	NVSTATEMANAGER.RestoreRenderState(D3DRS_FILLMODE);
	NVSTATEMANAGER.RestoreRenderState(D3DRS_FOGENABLE);

	return hr;
}


void CShaderLinkerDemo::MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y)
{
	if (m_pUI)
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
	}
	return;
}

void CShaderLinkerDemo::MouseMove(HWND hWnd, int x, int y)
{
	if (m_pUI)
		m_pUI->OnMouseMove(x, y);
	return;
}

void CShaderLinkerDemo::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
{
	eEBKeyAction Action = TranslateEffectKey(dwKey, nFlags, bDown);

	switch(Action)
	{
		case EB_HELP:
		{
	        MessageBoxEx( NULL, " Help : F1 - Help \n\n Home - Reset To Defaults \n\n W - Wireframe Toggle \n\n L - Lighting Toggle \n\n Space\\Pause - Toggle Pause/Resume \n\n Left Button & Mouse - Rotate Object\n\n Shift Left Button & Mouse - Pan Camera \n\n Ctrl Left Button & Mouse - Move Camera In & Out\n\n",
			   "Help", MB_ICONINFORMATION | MB_TASKMODAL, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ) );
		}
		break;

		case EB_RESET:
		{
			m_pUI->Reset();
		}
		break;

		case EB_SUBTRACT:
		{
			m_fBumpScale -= 0.02f;
			if (m_fBumpScale < 0.0f)
				m_fBumpScale = 0.0f;
		}
		break;

		case EB_ADD:
		{
			m_fBumpScale += 0.02f;
			if (m_fBumpScale > 1.0f)
				m_fBumpScale = 1.0f;
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

	return;
}

