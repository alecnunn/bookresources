/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
          
******************************************************************************/

#include <crtdbg.h>

#include "eb_effect.h"
#include "nvtex.h"
#include "nvdevice.h"
#include "..\CommonSrc\NV_Error.h"

#include "DepthOfField.h"
#include "Constants.h"

using namespace nv_objects;
using namespace std;

DECLARE_EFFECT_MAIN()

extern "C"
{

__declspec(dllexport) unsigned int GetNumEffects() { return 1; }

__declspec(dllexport) EBEffect* CreateEffect(unsigned int EffectNum)
{
	return new CDepthOfField();
}

}


float const       CDepthOfField::kCloseClip  = 0.1f;      // in m
float const       CDepthOfField::kFarClip    = 520.0f;    // in m

// min and max focus distance in cm 
// (focusing beyond 200m is equivalent to infinity, even for 600mm lenses.)
// (focusing closer than the texture-resolution does not make sense)
// Even so, this technique does not work well for wide-angle lenses with
// close focusing: the distance to camera texture resolution is just not there 
// to do that (in addition total depth-resolution is only linear 8bits), so this 
// technique as coded here is really mostly for tele-photo zooms with far-focuses, i.e., 
// following rally-cars, race-cars, etc., etc.)
float const       CDepthOfField::kMinFocusDistance  = kMaxFocusDistance/kConfusionLookupHeight;
float const       CDepthOfField::kMaxFocusDistance  = 20000.0;

// only allow "normal" thru tele-photo lenses
float const       CDepthOfField::kMinFocalLength =  3.0f;       // in cm
float const       CDepthOfField::kMaxFocalLength = 60.0f;       // in cm

// 35mm film is actually 3.6 cm wide!
float const       CDepthOfField::kFilmDimension  = 3.6f;             

// this constant describes how much the circle of confusion espands 
// when applying the 9-sample box-filter to the texture 
float const       CDepthOfField::kBlurFactor = 1.5f; 

CDepthOfField::CDepthOfField()
    : m_pNVDevice(NULL)
    , mbWireFrame   (false)
    , meDisplayOption(SHOW_COLORS)
    , mbUsesVolumes(false)
    , mpMouseUI(NULL)
    , mCamLookDirection(0.0f, 0.0f,   1.0f)
    , mFStop        (    1.0f)       
    , mFocalLength  (   20.0f)       // in cm
    , mFocusDistance( 2536.0f)       // in cm
    , mWorldBoxDimensions( 150.0f, 150.0f, 150.0f )  // in m!
	, mpWorldBoxVertices(NULL)
	, mpWorldBoxIndices (NULL)
	, m_dwBlurVertexShader(0)
	, m_dwBlurPixelShader(0)
	, m_dwWorldVertexShader(0)
	, m_dwWorldPixelShader(0)
	, m_dwWorldNoDoFPixelShader(0)
	, m_dwTetraVertexShader(0)
	, m_dwTetraPixelShader(0)
	, m_dwTetraNoDoFPixelShader(0)
	, m_dwDepthOfFieldPixelShader(0)
    , m_dwShowBlurrinessShader(0)
    , mpQuadVertexBuffer(NULL)
	, mpTetrahedronVertices(NULL)
	, mpTetrahedronIndices (NULL)
    , mpBackbufferColor(NULL) 
    , mpBackbufferDepth(NULL)
    , mpDepthTarget(NULL)
    , mpCircleOfConfusionLookup(NULL)
    , mpVolCircleOfConfusionLookup(NULL)
	, mpObjectTexture(NULL)
{
	m_strEffectName         = "Depth of Field";
	m_strEffectLocation     = "Pixel Shaders\\Image Processing";
	m_strEffectPixelShader  = GetFilePath("Tetrahedron.nvp");
	m_strEffectVertexShader = GetFilePath("World.nvv");

	for (DWORD i = 0; i < 6; i++)
		mpWorldTextures[i] = NULL;
}

CDepthOfField::~CDepthOfField()
{
	Free();	
}

HRESULT CDepthOfField::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
{
    // check vertex shading support
    if(D3DSHADER_VERSION_MAJOR(pCaps->VertexShaderVersion) < 1)
	{
		m_strLastError = "Device does not support vertex shaders!";
		return E_FAIL;
	}
    if(D3DSHADER_VERSION_MINOR(pCaps->VertexShaderVersion) < 1)
	{
		m_strLastError = "Device does not support 1.1 vertex shaders!";
		return E_FAIL;
	}

    // check simultaneous texture support
    if(pCaps->MaxSimultaneousTextures < 4)
	{
		m_strLastError = "Device does not support 4 simultaneous textures!";
		return E_FAIL;
	}

    // check pixel shader support
    if(D3DSHADER_VERSION_MAJOR(pCaps->PixelShaderVersion) < 1)
	{
		m_strLastError = "Device does not support pixel shaders!";
		return E_FAIL;
	}
	if(D3DSHADER_VERSION_MINOR(pCaps->PixelShaderVersion) < 1)
	{
		m_strLastError = "Device does not support 1.1 pixel shaders!";
		return E_FAIL;
	}
	if(pCaps->MaxTextureBlendStages < 8)
	{
		m_strLastError = "Device does not support 8 register combiners!";
		return E_FAIL;
	}
    mbUsesVolumes = (pCaps->TextureCaps & D3DPTEXTURECAPS_VOLUMEMAP) != 0;

    return S_OK;
}

void CDepthOfField::UpdateProperties()
{
	EBEffect::UpdateProperties();

    // Options
    AddProperty(new EBProperty("Wireframe",       OBJECT_MEMBER(mbWireFrame),      EBTYPE_BOOL_PROP));

	EBEnumProperty* pEnumProp = new EBEnumProperty("Display Options", OBJECT_MEMBER(meDisplayOption), EBTYPE_DWORD_PROP);
	
    pEnumProp->AddEnumerant(new EBEnumValue(pEnumProp, "Show Colors",     (DWORD)SHOW_COLORS,     EBTYPE_DWORD_PROP));
    pEnumProp->AddEnumerant(new EBEnumValue(pEnumProp, "Show Depth",      (DWORD)SHOW_DEPTH,      EBTYPE_DWORD_PROP));
    pEnumProp->AddEnumerant(new EBEnumValue(pEnumProp, "Show Blurriness", (DWORD)SHOW_BLURRINESS, EBTYPE_DWORD_PROP));
	AddProperty(pEnumProp);

	// Vertex shaders
	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "World Rendering",       GetFilePath("World.nvv"),       EBTYPE_STRING_PROP));
	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Object Rendering",      GetFilePath("Tetrahedron.nvv"), EBTYPE_STRING_PROP));
	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Screen-Quad Rendering", GetFilePath("Blur.nvv"),        EBTYPE_STRING_PROP));

	// Pixel shaders
	m_pPixelShaderEnum->AddEnumerant(new EBEnumValue(m_pPixelShaderEnum, "Object Rendering",           GetFilePath("Tetrahedron.nvp"),       EBTYPE_STRING_PROP));
	m_pPixelShaderEnum->AddEnumerant(new EBEnumValue(m_pPixelShaderEnum, "Screen-Quad Rendering",      GetFilePath("World.nvp"),             EBTYPE_STRING_PROP));
	m_pPixelShaderEnum->AddEnumerant(new EBEnumValue(m_pPixelShaderEnum, "Texture Blurring",           GetFilePath("Blur.nvp"),              EBTYPE_STRING_PROP));
	m_pPixelShaderEnum->AddEnumerant(new EBEnumValue(m_pPixelShaderEnum, "Depth of Field Combination", GetFilePath("DepthOfFieldBlend.nvp"), EBTYPE_STRING_PROP));
}

HRESULT CDepthOfField::Initialize(IDirect3DDevice8* pDev)
{
	HRESULT hr;
    ZeroMemory(&m_bKey[0], sizeof(bool) * kMaxVKey);

    // get the device
	m_pD3DDev = pDev;
	pDev->AddRef();

	m_pNVDevice = new NVDevice(pDev);

    //initialize mouse UI
	D3DVIEWPORT8    viewport;
	RECT            rect;

	m_pD3DDev->GetViewport(&viewport);
	rect.left   = rect.top = 0;
	rect.bottom = viewport.Height;
	rect.right  = viewport.Width;
	mpMouseUI   = new MouseUI(rect, true);
    mpMouseUI->SetRotationalSensitivityFactor(1.0f);

    CreateTextureRenderTarget();

    hr = InitBlurRendering();
	if (FAILED(hr))
	{
		m_strLastError = "Failed to initialise blur rendering";
		return hr;
	}

	hr = InitWorldRendering();
	if (FAILED(hr))
	{
		m_strLastError = "Failed to initialise world rendering";
		return hr;
	}

	hr = InitTetrahedronRendering();
	if (FAILED(hr))
	{
		m_strLastError = "Failed to initialise tetrahedron rendering";
		return hr;
	}

    UpdateCameraParameters();
    GenerateCircleOfConfusionTexture();

	return S_OK;
}

HRESULT CDepthOfField::InitBlurRendering()
{
	HRESULT hr;

    // create the vertex and pixel shaders for filtering the rendering target 
	vector<DWORD> Declaration;
	Declaration.push_back(D3DVSD_STREAM(0));
	Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(1, D3DVSDT_FLOAT2));
	Declaration.push_back(D3DVSD_END());

	hr = LoadAndCreateShader(GetFilePath("Blur.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwBlurVertexShader);
	if (FAILED(hr))
		return hr;
	
	hr = LoadAndCreateShader(GetFilePath("Blur.pso"), NULL, 0, SHADERTYPE_PIXEL, &m_dwBlurPixelShader);
	if (FAILED(hr))
		return hr;
	hr = LoadAndCreateShader(GetFilePath("DepthOfFieldBlend.pso"), NULL, 0, SHADERTYPE_PIXEL, &m_dwDepthOfFieldPixelShader);
	if (FAILED(hr))
		return hr;
	hr = LoadAndCreateShader(GetFilePath("ShowBlurriness.pso"), NULL, 0, SHADERTYPE_PIXEL, &m_dwShowBlurrinessShader);
	if (FAILED(hr))
		return hr;

	// create vertex buffer 
	hr = m_pD3DDev->CreateVertexBuffer( 4 * sizeof(tQuadVertex), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &mpQuadVertexBuffer);
	if (FAILED(hr))
		return hr;

	tQuadVertex      *pBuff;

	if (mpQuadVertexBuffer)
	{
		hr = mpQuadVertexBuffer->Lock(0, 4 * sizeof(tQuadVertex),(BYTE**)&pBuff, 0);
		if (FAILED(hr))
		{
			m_strLastError = "Couldn't lock buffer!";
			return hr;
		}

        for (int i = 0; i < 4; ++i)
        {
            pBuff->mPosition = D3DXVECTOR3((i==0 || i==1) ? -1.0f : 1.0f,
                                           (i==0 || i==3) ? -1.0f : 1.0f,
                                          0.0f);
		    pBuff->mTexture  = D3DXVECTOR2((i==0 || i==1) ? 0.0f : 1.0f, 
                                           (i==0 || i==3) ? 1.0f : 0.0f);
		    pBuff++; 
        }
        mpQuadVertexBuffer->Unlock();
    }

    D3DSURFACE_DESC ddsd;
    mpTextureFiltered[0]->GetLevelDesc(0, &ddsd);

    CreateAndWriteUVOffsets(ddsd.Width, ddsd.Height);
	
    return S_OK;
}

HRESULT CDepthOfField::InitWorldRendering()
{
	HRESULT hr;

    // create the vertex and pixel shaders for rendering of world objects
	vector<DWORD> Declaration;
	Declaration.push_back(D3DVSD_STREAM(0));
	Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(2, D3DVSDT_FLOAT2));
	Declaration.push_back(D3DVSD_END());

	hr = LoadAndCreateShader(GetFilePath("World.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwWorldVertexShader);
	if (FAILED(hr))
		return hr;
	
	hr = LoadAndCreateShader(GetFilePath("World.pso"), NULL, 0, SHADERTYPE_PIXEL, &m_dwWorldPixelShader);
	if (FAILED(hr))
		return hr;
	hr = LoadAndCreateShader(GetFilePath("WorldNoDoFLookup.pso"), NULL, 0, SHADERTYPE_PIXEL, &m_dwWorldNoDoFPixelShader);
	if (FAILED(hr))
		return hr;

	CreateWorldCube();

    return S_OK;
}

HRESULT CDepthOfField::InitTetrahedronRendering()
{
	HRESULT hr;

    // create the vertex and pixel shaders for rendering of objects within the world
	vector<DWORD> Declaration;
	Declaration.push_back(D3DVSD_STREAM(0));
	Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(1, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(2, D3DVSDT_FLOAT2));
	Declaration.push_back(D3DVSD_END());

	hr = LoadAndCreateShader(GetFilePath("Tetrahedron.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwTetraVertexShader);
	if (FAILED(hr))
		return hr;
	
	hr = LoadAndCreateShader(GetFilePath("Tetrahedron.pso"), NULL, 0, SHADERTYPE_PIXEL, &m_dwTetraPixelShader);
	if (FAILED(hr))
		return hr;
	hr = LoadAndCreateShader(GetFilePath("TetrahedronNoDoFLookup.pso"), NULL, 0, SHADERTYPE_PIXEL, &m_dwTetraNoDoFPixelShader);
	if (FAILED(hr))
		return hr;

	CreateTetrahedron();

    return S_OK;
}

HRESULT CDepthOfField::SetBlurRenderState()
{
    m_pD3DDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

    for (int i = 0; i < 4; ++i)
    {
	    m_pD3DDev->SetTextureStageState(i, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	    m_pD3DDev->SetTextureStageState(i, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	    m_pD3DDev->SetTextureStageState(i, D3DTSS_MIPFILTER, D3DTEXF_NONE);
	    m_pD3DDev->SetTextureStageState(i, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP);
	    m_pD3DDev->SetTextureStageState(i, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP);
    }

    m_pD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pD3DDev->SetRenderState(D3DRS_BLENDOP,   D3DBLENDOP_ADD);                 
    m_pD3DDev->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ONE);
    m_pD3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

    m_pD3DDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	m_pD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pD3DDev->SetStreamSource(0, mpQuadVertexBuffer, sizeof(tQuadVertex));

    // turn on our special filtering pixel shader
	m_pD3DDev->SetVertexShader(m_dwBlurVertexShader);
    m_pD3DDev->SetPixelShader(m_dwBlurPixelShader);	
    
    return S_OK;
}

HRESULT CDepthOfField::SetMatrices(D3DXMATRIX  const &matWorld)
{
    // set up vertex shading constants to contain proper
    // transformation matrices etc.
    D3DXMATRIX  matLookRotations;

    D3DXMatrixTranspose(&matLookRotations, &(mpMouseUI->GetRotationMatrix()));
    mCamLookDirection = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
    D3DXVec3TransformNormal(&mCamLookDirection, &mCamLookDirection, &matLookRotations);

    D3DXVECTOR3 const vLookatPt = mCamPosition + mCamLookDirection;
	D3DXVECTOR3 const vUp       = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    float const     fieldOfView = 2.0f * atanf(.5f*kFilmDimension/mFocalLength);

    D3DXMATRIX  matView;
	D3DXMATRIX  matProj;

    D3DXMatrixLookAtLH(&matView, &mCamPosition, &vLookatPt, &vUp);
    D3DXMatrixPerspectiveFovLH( &matProj, fieldOfView, 1.0f, kCloseClip, kFarClip );

    D3DXMATRIX matWorldView;
	D3DXMATRIX matWorldViewIT;
	D3DXMATRIX matWorldViewProj;

	D3DXMatrixMultiply(&matWorldView,     &matWorld,     &matView);
	D3DXMatrixMultiply(&matWorldViewProj, &matWorldView, &matProj);
	
	// Write Projection to clip space matrix to constant memory
	D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matWorldViewProj(0, 0), 4);

	// Create a 3x3 inverse of the world matrix for the normal transformation 
    // (we transpose it as we load it into the constant store)
	D3DXMatrixInverse(&matWorldViewIT, NULL, &matWorldView);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWIT_0, &matWorldViewIT(0, 0), 3);

    // finally write the worldView matrix: it takes vertices to view-space 
	D3DXMatrixTranspose(&matWorldView, &matWorldView);
    m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEW_0, &matWorldView(0, 0), 4);

	// Create a directional light and transform it to eye-space
	// Shader math requires that the vector is to the light
	D3DXVECTOR3 vLight( 0.0f, 1.0f, 0.25f);

	// Transform direction vector into eye space
	D3DXVec3Normalize(      &vLight, &vLight);
	D3DXVec3TransformNormal(&vLight, &vLight, &matView);
	D3DXVec3Normalize(      &vLight, &vLight);

	m_pD3DDev->SetVertexShaderConstant(CV_LIGHT_DIRECTION, &vLight, 1);
 
    // color of the light
    D3DXVECTOR4     lightDiffuse(0.9f, 0.9f, 1.0f, 0.0f);
    m_pD3DDev->SetVertexShaderConstant(CV_LIGHT_DIFFUSE, &lightDiffuse, 1);    

    return S_OK;
}
    
HRESULT CDepthOfField::Free()
{
    SAFE_RELEASE(mpTetrahedronVertices);
    SAFE_RELEASE(mpTetrahedronIndices);
	SAFE_RELEASE(mpQuadVertexBuffer);
    SAFE_RELEASE(mpWorldBoxVertices);
    SAFE_RELEASE(mpWorldBoxIndices);

	SAFE_DELETE(m_pNVDevice);
	
    if (m_pD3DDev)
	{
        // reset buffers properly
        if ((mpBackbufferColor) != NULL && (mpBackbufferDepth != NULL))
        {
            HRESULT hr = m_pD3DDev->SetRenderTarget(mpBackbufferColor, mpBackbufferDepth);
            _ASSERT(hr == S_OK);
        }
        SAFE_RELEASE(mpBackbufferColor);
		SAFE_RELEASE(mpBackbufferDepth);

		m_pD3DDev->DeletePixelShader (m_dwShowBlurrinessShader);
		m_pD3DDev->DeleteVertexShader(m_dwBlurVertexShader);
		m_pD3DDev->DeletePixelShader (m_dwBlurPixelShader );
		m_pD3DDev->DeletePixelShader (m_dwDepthOfFieldPixelShader);
		m_pD3DDev->DeleteVertexShader(m_dwWorldVertexShader);
		m_pD3DDev->DeletePixelShader (m_dwWorldNoDoFPixelShader );
		m_pD3DDev->DeletePixelShader (m_dwWorldPixelShader );
		m_pD3DDev->DeleteVertexShader(m_dwTetraVertexShader);
		m_pD3DDev->DeletePixelShader (m_dwTetraNoDoFPixelShader );
		m_pD3DDev->DeletePixelShader (m_dwTetraPixelShader );

        for (int i = 0; i < 3; ++i)
        {
            SAFE_RELEASE(mpFilterTarget[i]);
            SAFE_RELEASE(mpTextureFiltered[i]);
        }
        for (i = 0; i < 2; ++i)
        {
            SAFE_RELEASE(mpTempTexture[i]);
            SAFE_RELEASE(mpTempTarget [i]);
        }
        for (i = 0; i < 6; i++)
            SAFE_DELETE(mpWorldTextures[i]);

        SAFE_DELETE(mpObjectTexture);

        SAFE_RELEASE(mpDepthTarget);
        SAFE_RELEASE(mpCircleOfConfusionLookup);
        SAFE_RELEASE(mpVolCircleOfConfusionLookup);

        SAFE_RELEASE(m_pD3DDev);
	}	
    SAFE_DELETE(mpMouseUI);

	return S_OK;
}

HRESULT CDepthOfField::Start()
{
	return S_OK;
}

HRESULT CDepthOfField::Tick(EBTimer* pTimer)
{
	HRESULT hr;
    int     i, j, k;

    // update view-state from the keyboard
    SetViewFromKeyboardInput(pTimer);

    // set a bunch of render-state for rendering the world
    m_pD3DDev->SetRenderState(D3DRS_FILLMODE, (mbWireFrame) ? D3DFILL_WIREFRAME : D3DFILL_SOLID);

    m_pD3DDev->SetTexture(0, mpObjectTexture->GetTexture());
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
    m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );

    if (mbUsesVolumes) 
        m_pD3DDev->SetTexture(1, mpVolCircleOfConfusionLookup);
    else
        m_pD3DDev->SetTexture(1, mpCircleOfConfusionLookup);

	m_pD3DDev->SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_NONE);
    m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);

    m_pD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pD3DDev->SetRenderState(D3DRS_BLENDOP,   D3DBLENDOP_ADD);                 
    m_pD3DDev->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ONE);
    m_pD3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

    m_pD3DDev->SetRenderState(D3DRS_ZENABLE, TRUE);
    m_pD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    // first render the world "normally" but into a texture
    hr = m_pD3DDev->SetRenderTarget(mpFilterTarget[0], mpDepthTarget);
	hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB( 0x0, 0x0, 0x0, 0x60 ), 1.0, 0);

    // draw lots of tetrahedra here
    D3DXVECTOR4     ambientLight(0.3f, 0.3f, 0.15f, 1.0f);
    m_pD3DDev->SetVertexShaderConstant(CV_AMBIENT_LIGHT, &ambientLight, 1);

    m_pD3DDev->SetVertexShader(m_dwTetraVertexShader);
    if (meDisplayOption != SHOW_DEPTH)
        m_pD3DDev->SetPixelShader (m_dwTetraPixelShader);
    else
        m_pD3DDev->SetPixelShader (m_dwTetraNoDoFPixelShader);


    m_pD3DDev->SetStreamSource(0, mpTetrahedronVertices, sizeof(tTetrahedronVertex));
	m_pD3DDev->SetIndices(mpTetrahedronIndices, 0);

	D3DXVECTOR3 const   vUp       = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	D3DXMATRIX          matWorld;
	D3DXMATRIX          matTemp;
    float               x, z, s, r;
    float               cr, cg, cb;
    float const         kRandMax  = static_cast<float>(RAND_MAX);

    // init random number generator (we need to get the same sequence every frame)
    srand( 3 );

    for (i = 0; i < kNumTetrahedra; ++i)
    {
        s = 1.0f + 0.5f * static_cast<float>(rand())/kRandMax;
        r = 2.0f * D3DX_PI * static_cast<float>(rand())/kRandMax;
        x = mWorldBoxDimensions.x * (0.5f - static_cast<float>(rand())/kRandMax);
        z = mWorldBoxDimensions.z * (0.5f - static_cast<float>(rand())/kRandMax);

        D3DXMatrixScaling     (&matWorld, s, s, s);
        D3DXMatrixRotationAxis(&matTemp,  &vUp, r);
        D3DXMatrixMultiply    (&matWorld, &matWorld, &matTemp);
        D3DXMatrixTranslation (&matTemp,  x, 0.0f, z);
        D3DXMatrixMultiply    (&matWorld, &matWorld, &matTemp);

        hr = SetMatrices( matWorld );

        cr = 0.7f + 0.5f * static_cast<float>(rand())/kRandMax;
        cg = 0.7f + 0.5f * static_cast<float>(rand())/kRandMax;
        cb = 0.7f + 0.5f * static_cast<float>(rand())/kRandMax;

        D3DXVECTOR4     materialColor(cr, cg, cb, 1.0f);
        m_pD3DDev->SetVertexShaderConstant(CV_MATERIAL, &materialColor, 1);       

	    m_pD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 12, 0, 4);
    }

    // draw the floor next
	// Scale the world box and put it outside the world
	D3DXVECTOR3 const vecScale = mWorldBoxDimensions;
	D3DXMatrixScaling(&matWorld, vecScale.x, vecScale.y, vecScale.z);

    hr = SetMatrices( matWorld );

    m_pD3DDev->SetVertexShader(m_dwWorldVertexShader);
    if (meDisplayOption != SHOW_DEPTH)
        m_pD3DDev->SetPixelShader (m_dwWorldPixelShader);
    else
        m_pD3DDev->SetPixelShader (m_dwWorldNoDoFPixelShader);

    m_pD3DDev->SetStreamSource(0, mpWorldBoxVertices, sizeof(tQuadVertex));
	m_pD3DDev->SetIndices(mpWorldBoxIndices, 0);

    // change the color texture, but all other stages and settings stay the same
    m_pD3DDev->SetTexture(0, mpWorldTextures[1]->GetTexture());

    m_pD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    // draw the floor a bit different
    for (j = 0; j < kNumQuadsPerSide; ++j)   // for each strip
	    m_pD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, kNumVertices, 
                                        kNumIndicesPerFace + j*2*(kNumQuadsPerSide+1), 
                                        kNumTrisPerStrip);

    // draw the walls and ceiling
    m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );

    for (i = 0; i < 6; ++i)         // for each cube face, except the floor
        if (i != 1)
        {
            m_pD3DDev->SetTexture(0, mpWorldTextures[i]->GetTexture());
            for (j = 0; j < kNumQuadsPerSide; ++j)   // for each strip
                m_pD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, kNumVertices, 
                                                i*kNumIndicesPerFace + j*2*(kNumQuadsPerSide+1), 
                                                kNumTrisPerStrip);
        }

    // then take the just rendered texture and generate blurred versions
    D3DXMATRIX matView;
	D3DXMATRIX matProj;
	D3DXMATRIX matViewProj;
	D3DXMATRIX matWorldViewProj;

    hr = SetBlurRenderState();

	D3DXVECTOR3 const vEyePt    = D3DXVECTOR3( 0.0f, 0.0f, -5.0f );
	D3DXVECTOR3 const vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	// Set World, View, Projection, and combination matrices.
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUp);
	D3DXMatrixOrthoLH(&matProj, 4.0f, 4.0f, 0.2f, 20.0f);

    D3DXMatrixMultiply(&matViewProj, &matView, &matProj);

    // draw a single quad to texture: the quad covers the whole "screen" exactly
	D3DXMatrixScaling(&matWorld, 2.0f, 2.0f, 1.0f);
	D3DXMatrixMultiply(&matWorldViewProj, &matWorld, &matViewProj);
    D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matWorldViewProj(0, 0), 4);

    // write to constant memory to use the box9-filter
    D3DXVECTOR4     offset(1.0f, 0.0f, 0.0f, 0.0f);
    m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);

    // mpFilterTarget[0] is the full-res texture
    // make mpFilterTarget[1] = blur^kNumOfFilterSteps( mpFilterTarget[0] )
    // make mpFilterTarget[2] = blur^kNumOfFilterSteps( mpFilterTarget[1] )
    IDirect3DTexture8       *pSource;
    IDirect3DSurface8       *pDestination;

    for (i = 1; i < 3; ++i)     // this loop is for generating mpFilterTarget[i]
    {
        for (j = 0; j < kNumOfFilterSteps; ++j) // this loop does several blur passes
        {
            // alternate source and destination
            pSource      = mpTempTexture[ j   %2];
            pDestination = mpTempTarget [(j+1)%2]; 
            if (j == 0)                         // first time thru
                pSource = mpTextureFiltered[i-1];
            else if (j == kNumOfFilterSteps-1)  // last time thru
                pDestination = mpFilterTarget[i]; 
            
            // using the source and destination pointers set-up the render-state
            hr = m_pD3DDev->SetRenderTarget(pDestination, NULL);
            for (k = 0; k < 4; ++k)
                m_pD3DDev->SetTexture(k, pSource);
    
            // no need to clear the buffer: z-test is off: thus everything is overwritten
            // now just draw the quad with displaced texture coordinates
            hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
        }
    }

    // then render a single quad to the back-buffer.
    // this quad uses a special depth of field blend pixel filter
    hr = m_pD3DDev->SetRenderTarget(mpBackbufferColor, NULL);
	// again, z-test is still off, so all is overwritten -- no need to clear

	m_pD3DDev->SetVertexShader(m_dwBlurVertexShader);

    if (meDisplayOption == SHOW_COLORS)
        m_pD3DDev->SetPixelShader(m_dwDepthOfFieldPixelShader);	
    else
        m_pD3DDev->SetPixelShader(m_dwShowBlurrinessShader);	

    m_pD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    // use the original and blurred textures
    for (i = 0; i < 3; ++i)
    {
        m_pD3DDev->SetTexture(i, mpTextureFiltered[i]);
        m_pD3DDev->SetTextureStageState(i, D3DTSS_MIPFILTER, D3DTEXF_NONE   );
	    m_pD3DDev->SetTextureStageState(i, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	    m_pD3DDev->SetTextureStageState(i, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    }
    m_pD3DDev->SetTexture(3, NULL);

	D3DXMatrixScaling(&matWorld, 2.0f, 2.0f, 1.0f);
	D3DXMatrixMultiply(&matWorldViewProj, &matWorld, &matViewProj);
    D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matWorldViewProj(0, 0), 4);

    // reset offsets to 0
    offset.x = 0.0f;
    m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);

	hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

    return hr;
}

HRESULT CDepthOfField::CreateTextureRenderTarget()
{
    HRESULT         hr;

    // first simply create the circle of confusion look up map
    // This texture could be 3D (if available) or 2D or 1D.  Trade-off
    // which parameters vary by how much and how often what size texture
    // can be regenerated.
    if (mbUsesVolumes)
    {
        hr = m_pD3DDev->CreateVolumeTexture( kConfusionLookupWidth, kConfusionLookupHeight, kConfusionLookupDepth,
                                             1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
                                             &mpVolCircleOfConfusionLookup );
        if( FAILED(hr) )
	    {
		    m_strLastError = "Could not create volume texture!";
            return E_FAIL;
	    }
    }
    else
    {
        hr = m_pD3DDev->CreateTexture( kConfusionLookupWidth, kConfusionLookupHeight, 
                                         1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
                                         &mpCircleOfConfusionLookup );
        if( FAILED(hr) )
	    {
		    m_strLastError = "Could not create 2D look-up texture!";
            return E_FAIL;
	    }
    }

    // get a pointer to the current back-buffer (so we can restore it later)
	m_pD3DDev->GetRenderTarget(        &mpBackbufferColor );
	m_pD3DDev->GetDepthStencilSurface( &mpBackbufferDepth );
	_ASSERT( mpBackbufferColor != NULL );
	_ASSERT( mpBackbufferDepth != NULL );

    // get the description for the texture we want to filter
	D3DSURFACE_DESC ddsd;

    mpBackbufferColor->GetDesc(&ddsd);

    // set ddsd width/height to next smaller power of 2
    // we loose some precision, but it is hard to tell, yet the smaller 
    // size is much higher performance as we have to filter many fewer pixels...
    int logWidth  = static_cast<int>(logf(static_cast<float>(ddsd.Width)) /logf(2.0f));
    int logHeight = static_cast<int>(logf(static_cast<float>(ddsd.Height))/logf(2.0f));

    ddsd.Width  = pow(2, logWidth );
    ddsd.Height = pow(2, logHeight);

    // make a depth buffer to go with the first texture
    hr = m_pD3DDev->CreateDepthStencilSurface(ddsd.Width, ddsd.Height,       // use color's width/height!!!
                                  D3DFMT_D24X8, D3DMULTISAMPLE_NONE, 
                                  &mpDepthTarget);

    // create new textures just like the current texture
    // these will be used as filter-targets and sources
    for (int i = 0; i < 3; ++i)
    {
        hr = m_pD3DDev->CreateTexture(ddsd.Width, ddsd.Height, 1, 
                                      D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, 
                                      D3DPOOL_DEFAULT, &mpTextureFiltered[i]);
        hr = mpTextureFiltered[i]->GetSurfaceLevel(0, &mpFilterTarget[i]);

        // set our render target to the new and shiny textures without depth
        hr = m_pD3DDev->SetRenderTarget(mpFilterTarget[i], (i==0) ? mpDepthTarget : NULL);
        if (FAILED(hr))
        {
		    m_strLastError = "Can't SetRenderTarget to new surface!\n";
            _ASSERT(false);
            return E_FAIL;
        }
    }
    for (i = 0; i < 2; ++i)
    {
        hr = m_pD3DDev->CreateTexture(ddsd.Width, ddsd.Height, 1, 
                                      D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, 
                                      D3DPOOL_DEFAULT, &mpTempTexture[i]);
        hr = mpTempTexture[i]->GetSurfaceLevel(0, &mpTempTarget[i]);

        // set our render target to the new and shiny textures without depth
        hr = m_pD3DDev->SetRenderTarget(mpTempTarget[i], NULL);
        if (FAILED(hr))
        {
		    m_strLastError = "Can't SetRenderTarget to new surface!\n";
            _ASSERT(false);
            return E_FAIL;
        }
    }

    // switch back to conventional back-buffer
    hr = m_pD3DDev->SetRenderTarget(mpBackbufferColor, mpBackbufferDepth);
    if (FAILED(hr))
    {
		m_strLastError = "Can't SetRenderTarget to original back-buffer surfaces!\n";
        _ASSERT(false);
        return E_FAIL;
    }

    return S_OK;
}

void CDepthOfField::CreateAndWriteUVOffsets(int width, int height)
{
    // set-up offset to achieve a nine-sample box-filter centered 
    // over each texel.  
    float const     kPerTexelWidth  = 1.0f/static_cast<float>(width);
    float const     kPerTexelHeight = 1.0f/static_cast<float>(height);
    float const     noOffsetX[4]    = { 0.0f, 0.0f, 0.0f, 0.0f}; 
    float const     noOffsetY[4]    = { 0.0f, 0.0f, 0.0f, 0.0f};
    int             i;

    // nine sample box filter
    float const s = 2.0f/3.0f;
    float const box9OffsetX[4] = { 0.0f, 
                                   0.0f,  
                                   2.0f * s * kPerTexelWidth,   
                                   2.0f * s * kPerTexelWidth };
    float const box9OffsetY[4] = { 0.0f, 
                                   2.0f * s * kPerTexelHeight, 
                                   2.0f * s * kPerTexelHeight, 
                                   0.0f };

    // write all these offsets to constant memory
    for (i = 0; i < 4; ++i)
    {
        D3DXVECTOR4  noOffset(    noOffsetX[i],   noOffsetY[i],   0.0f, 0.0f);
        D3DXVECTOR4  actualOffset(box9OffsetX[i], box9OffsetY[i], 0.0f, 0.0f);

    	m_pD3DDev->SetVertexShaderConstant(CV_UV_T0_NO_OFFSET + 2*i, &noOffset,     1);
    	m_pD3DDev->SetVertexShaderConstant(CV_UV_T0_TYPE1     + 2*i, &actualOffset, 1);
    }
}

HRESULT CDepthOfField::CreateWorldCube()
{
    HRESULT          hr;
    tQuadVertex     *pVertices;
	WORD            *pIndices;
    int              i, j, k;

    // first do the textures
    string cubeFaceTextures[6] = { "skybox_left.dds",
                                   "wood.dds", 
                                   "skybox_front.dds", 
                                   "skybox_back.dds", 
                                   "skybox_top.dds", 
                                   "skybox_right.dds"
                                  };
	for (i = 0; i < 6; i++)
    {
		mpWorldTextures[i] = new NVTexture();
        hr = mpWorldTextures[i]->CreateFromFile(m_pD3DDev, GetFilePath(cubeFaceTextures[i]));
	    if (FAILED(hr))
	    {
		    m_strLastError = cubeFaceTextures[i];
		    _ASSERT(false);
            return hr;
	    }
    }

    // now lets allocate vertices and indices for the cube faces
    hr = m_pD3DDev->CreateVertexBuffer(kNumVertices * sizeof(tQuadVertex), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &mpWorldBoxVertices);
	hr = mpWorldBoxVertices->Lock(0,   kNumVertices * sizeof(tQuadVertex),(BYTE**)&pVertices, 0);
	if (FAILED(hr))
	{
		m_strLastError = "Could not create/lock vertex buffer!";
		return hr;
	}

	hr = m_pD3DDev->CreateIndexBuffer(kNumIndices * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &mpWorldBoxIndices);
	hr = mpWorldBoxIndices->Lock(0,   kNumIndices * sizeof(WORD),(BYTE**)&pIndices, 0);
	if (FAILED(hr))
	{
		m_strLastError = "Could not create/lock index buffer!";
		return hr;
	}

    // Set up the vertices for the cube engulfing the world.
    // Note: to prevent tiling problems, the u/v coords are knocked slightly inwards.
    float       const kFloorTextureRepeat = 2.0f * mWorldBoxDimensions.x;

    D3DXVECTOR3 vertexCube[8] = { D3DXVECTOR3(-1.0f,-1.0f,-1.0f), 
                                  D3DXVECTOR3(-1.0f,-1.0f, 1.0f),
                                  D3DXVECTOR3(-1.0f, 1.0f,-1.0f),
                                  D3DXVECTOR3(-1.0f, 1.0f, 1.0f),
                                  D3DXVECTOR3( 1.0f,-1.0f,-1.0f),
                                  D3DXVECTOR3( 1.0f,-1.0f, 1.0f),
                                  D3DXVECTOR3( 1.0f, 1.0f,-1.0f),
                                  D3DXVECTOR3( 1.0f, 1.0f, 1.0f)  };
    D3DXVECTOR2 uvCube[4]     = { D3DXVECTOR2( 0.0f, 0.0f), 
                                  D3DXVECTOR2( 0.0f, 1.0f), 
                                  D3DXVECTOR2( 1.0f, 0.0f), 
                                  D3DXVECTOR2( 1.0f, 1.0f) };
    // right:   0 1 2 3
    // top:     0 1 4 5 
    // front:   0 2 4 6
    // back:    1 3 5 7
    // bottom:  2 3 6 7
    // left:    4 5 6 7
 
    D3DXVECTOR3 ul, ur, ll, lr;
    D3DXVECTOR3 upper, lower, position;
    D3DXVECTOR2 ulUV, urUV, llUV, lrUV;
    D3DXVECTOR2 upperUV, lowerUV, uv;
    float       x, y;

    // each face is tesselated (see kNumQuadsPerSide in .h file)
    // to avoid interpolation errors for computing per-pixel distances
    for (i = 0; i < 6; ++i)         // for each face
    {
        if (i == 1) // bottom face is special
        {
            ul   = D3DXVECTOR3(-1.0f, 0.0001f,-1.0f);
            ur   = D3DXVECTOR3(-1.0f, 0.0001f, 1.0f);
            ll   = D3DXVECTOR3( 1.0f, 0.0001f,-1.0f);
            lr   = D3DXVECTOR3( 1.0f, 0.0001f, 1.0f);

            ulUV = D3DXVECTOR2(0.0f,                0.0f);
            urUV = D3DXVECTOR2(0.0f,                kFloorTextureRepeat);
            llUV = D3DXVECTOR2(kFloorTextureRepeat, 0.0f);
            lrUV = D3DXVECTOR2(kFloorTextureRepeat, kFloorTextureRepeat);
        }
        else
        {
            ul = vertexCube[(i<3)? 0 : (int)pow(2, i-3)];
            ur = vertexCube[(i<3)? 1 + i/2 :3+2*((i-3)/2)];
            ll = vertexCube[(i<3)? 2 + 2*((i+1)/2) : 5+((i-2)/2)];
            lr = vertexCube[(i<3)? 3+(3*i+1)/2 : 7]; 

            ulUV = uvCube[0];
            urUV = uvCube[1];
            llUV = uvCube[2];
            lrUV = uvCube[3];
        }


        for (j = 0; j < kNumQuadsPerSide+1; ++j)        // march in y
        {
            y = static_cast<float>(j)/static_cast<float>(kNumQuadsPerSide); 
            for (k = 0; k < kNumQuadsPerSide+1; ++k)    // march in x
            {
                x = static_cast<float>(k)/static_cast<float>(kNumQuadsPerSide); 

                upper    = (1.f-x)*ul    + x*ur;
                lower    = (1.f-x)*ll    + x*lr;
                position = (1.f-y)*upper + y*lower;

                upperUV  = (1.f-x)*ulUV    + x*urUV;
                lowerUV  = (1.f-x)*llUV    + x*lrUV;
                uv       = (1.f-y)*upperUV + y*lowerUV;

                *pVertices++ = tQuadVertex( position, uv );
            }
        }
    }

    // Set up the indices for the cube
    for (i = 0; i < 6; ++i)         // for each cube face
        for (j = 0; j < kNumQuadsPerSide; ++j)     // for each strip 
            for (k = 0; k < (kNumQuadsPerSide + 1); ++k)
            {
                *pIndices++ = k + j    *(kNumQuadsPerSide+1) + i*kNumVerticesPerFace;
                *pIndices++ = k + (j+1)*(kNumQuadsPerSide+1) + i*kNumVerticesPerFace;
            }

	mpWorldBoxVertices->Unlock();
	mpWorldBoxIndices->Unlock();

    return S_OK;
}

HRESULT CDepthOfField::CreateTetrahedron()
{
    HRESULT             hr;
    tTetrahedronVertex *pVertices;
	WORD               *pIndices;

    // get the texture
	mpObjectTexture = new NVTexture();
    hr = mpObjectTexture->CreateFromFile(m_pD3DDev, GetFilePath("ObjectTexture.dds"));
	if (FAILED(hr))
	{
		m_strLastError = "Could not find ObjectTexture.dds";
		_ASSERT(false);
        return hr;
	}

    // now lets allocate vertices and indices for the tetrahedron
    hr = m_pD3DDev->CreateVertexBuffer( 12 * sizeof(tTetrahedronVertex), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &mpTetrahedronVertices);
	hr = mpTetrahedronVertices->Lock(0, 12 * sizeof(tTetrahedronVertex),(BYTE**)&pVertices, 0);
	if (FAILED(hr))
	{
		m_strLastError = "Could not create/lock vertex buffer!";
		return hr;
	}

	hr = m_pD3DDev->CreateIndexBuffer( 12 * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &mpTetrahedronIndices);
	hr = mpTetrahedronIndices->Lock(0, 12 * sizeof(WORD),(BYTE**)&pIndices, 0);
	if (FAILED(hr))
	{
		m_strLastError = "Could not create/lock index buffer!";
		return hr;
	}

    D3DXVECTOR2 uvCoord[3] =        { D3DXVECTOR2(0.0f, 0.0f), 
                                      D3DXVECTOR2(0.0f, 1.0f),
                                      D3DXVECTOR2(1.0f, 1.0f)  };
    D3DXVECTOR3 vertexPosition[4] = { D3DXVECTOR3(0.0f, 0.0f, 0.0f), 
                                      D3DXVECTOR3(0.0f, 1.0f, 0.0f),
                                      D3DXVECTOR3(0.5f, 0.5f, 0.0f),
                                      D3DXVECTOR3(0.0f, 0.5f,-0.5f)  };
    float   const   oneOverSqrt3 = 1.0f/sqrt(3.0f);
    D3DXVECTOR3 triangleNormal[4] = { D3DXVECTOR3(-1.0f, 0.0f,  0.0f), 
                                      D3DXVECTOR3( 0.0f, 0.0f,  1.0f),
                                      D3DXVECTOR3( oneOverSqrt3, -oneOverSqrt3, -oneOverSqrt3),
                                      D3DXVECTOR3( oneOverSqrt3,  oneOverSqrt3, -oneOverSqrt3)  };

    // first triangle
    *pVertices++ = tTetrahedronVertex(vertexPosition[0], triangleNormal[0], uvCoord[0]);
    *pVertices++ = tTetrahedronVertex(vertexPosition[1], triangleNormal[0], uvCoord[1]);
    *pVertices++ = tTetrahedronVertex(vertexPosition[3], triangleNormal[0], uvCoord[2]);

    // second triangle
    *pVertices++ = tTetrahedronVertex(vertexPosition[0], triangleNormal[1], uvCoord[0]);
    *pVertices++ = tTetrahedronVertex(vertexPosition[2], triangleNormal[1], uvCoord[1]);
    *pVertices++ = tTetrahedronVertex(vertexPosition[1], triangleNormal[1], uvCoord[2]);

    // third  triangle
    *pVertices++ = tTetrahedronVertex(vertexPosition[0], triangleNormal[2], uvCoord[0]);
    *pVertices++ = tTetrahedronVertex(vertexPosition[3], triangleNormal[2], uvCoord[1]);
    *pVertices++ = tTetrahedronVertex(vertexPosition[2], triangleNormal[2], uvCoord[2]);

    // last triangle
    *pVertices++ = tTetrahedronVertex(vertexPosition[1], triangleNormal[3], uvCoord[0]);
    *pVertices++ = tTetrahedronVertex(vertexPosition[2], triangleNormal[3], uvCoord[1]);
    *pVertices++ = tTetrahedronVertex(vertexPosition[3], triangleNormal[3], uvCoord[2]);

    // indices are easy: simple triangle list in order 
    for (int i = 0; i < 12; ++i)         
        *pIndices++ = i;

	mpTetrahedronVertices->Unlock();
	mpTetrahedronIndices->Unlock();

    return S_OK;
}

HRESULT CDepthOfField::GenerateCircleOfConfusionTexture()
{
    float               inputx, inputy, inputz;
    float               distance, focusDistance, focalLength;
    float               minDistance, maxDistance;
    float               interpolator,  circle;
    DWORD               dInterpolator, output;
 
    FDebug("Camera parameters:\n");
    FDebug("\tF-Stop:         %7.2f  \n",   mFStop);
    FDebug("\tFocal-Length:   %6.1fmm\n",   10.0f*mFocalLength);
    FDebug("\tFocus-Distance: %7.2fm \n\n", mFocusDistance/100.0f);

    D3DSURFACE_DESC ddsd;
    mpTextureFiltered[0]->GetLevelDesc(0, &ddsd);

    float const     c0      = kFilmDimension/static_cast<float>( max(ddsd.Width, ddsd.Height) );
    float const     ratio1  = pow(kBlurFactor,      kNumOfFilterSteps);
    float const     ratio2  = pow(kBlurFactor, 2.0f*kNumOfFilterSteps);

    // lock the volume texture
    D3DLOCKED_BOX   locked;
    D3DLOCKED_RECT  lockedRect;

    if (mbUsesVolumes)
        mpVolCircleOfConfusionLookup->LockBox( 0, &locked, NULL, 0);
    else
        mpCircleOfConfusionLookup->LockRect( 0, &lockedRect, NULL, 0);

    // Here we compute the circle of confusion look-up table
    // The x-dimension represents distance to the camera.  
    //    Input ranges from 0 to 1 with 0 meaning minDistance 
    //    and 1 means maximum distance .
    //    So we compute
    //    distance = input*(MaxDist-minDist) + minDistance      // this is actual distance to camera
    //    circle of confusion formula is:
    //    circle = (focusDistance/distance - 1) * focalLength*focalLength /(FStop*(focusDistance - focalLength))
    //    circle = abs(circle)
    // 
    // The y-dimension represents focusDistance
    //    Input ranges from 0 to 1, with 0 meaning minFocusDistance (kCloseClip)
    //    and 1 meaning maxFocusDistance (farClip)
    //
    // The z-dimension represents focalLength
    //    Input ranges from 0 to 1, with 0 meaning minFocalLength 
    //    and 1 meaning maxFocalLength
    // 
    // At the heart of it all, we figure out what the min- and max-distance is for
    // the depth of field for a circle of confusion of ratio2*c0.  Stuff *blurrier*
    // than that we cannot represent anyway, so there is no need to waste 
    // interpolator-range on it.  Then we simply iterate from min- to max-distance
    // and compute the circle of confusion for that particular distance. 
    //
    // The circle of confusion diameter computed above is then mapped into an 
    // interpolator ranging from 0 to 1, with 0 corresponding to circles of diameter
    // c0 or less, 0.5 corresponding to diameters ratio1*c0, and 1 corresponding to 
    // diameters ratio2*c0 or more. 
    //
    // All formulas come from "Photographic Lenses Tutorial" by David M. Jacobson
    // (www.graflex.org/lenses/photographic-lenses-tutorial.html) -- a most excellent 
    // reference guide.
    
    int      x, y, z;
    DWORD   *pBase    = static_cast<DWORD *>((mbUsesVolumes) ? locked.pBits : lockedRect.pBits);
    DWORD   *pCurrent = pBase;

    for (z = 0; z < ((mbUsesVolumes) ? kConfusionLookupDepth : 1); ++z)
    {
        inputz      = static_cast<float>(z)/static_cast<float>(kConfusionLookupDepth-1);
        focalLength = (kMaxFocalLength - kMinFocalLength) * inputz + kMinFocalLength;
        if (! mbUsesVolumes)
            focalLength = mFocalLength;

        for (y = 0; y < kConfusionLookupHeight; ++y)
        {
            inputy        = static_cast<float>(y)/static_cast<float>(kConfusionLookupHeight-1);
            focusDistance = (kMaxFocusDistance - kMinFocusDistance) * inputy + kMinFocusDistance;

            float const hyperFocal  = focalLength*focalLength/(mFStop * ratio2 * c0);
            float const denominator = hyperFocal - (focusDistance - focalLength);

            minDistance = 0.01f * hyperFocal * focusDistance/(hyperFocal + focusDistance - focalLength);
            minDistance = max(minDistance, kCloseClip);
            maxDistance = kFarClip;
            if (denominator > 0.0f)
                maxDistance = min(maxDistance, 0.01f * focusDistance * (hyperFocal/denominator));
            if (minDistance >= maxDistance)
                maxDistance = minDistance + minDistance - maxDistance;

            for (x = 0; x < kConfusionLookupWidth; ++x)
	        {
                inputx   = static_cast<float>(x)/static_cast<float>(kConfusionLookupWidth-1);
                distance = (maxDistance - minDistance) * inputx + minDistance;
                distance = 100.0f * distance;       // convert from meters to cm
                circle   = (focusDistance/distance - 1.0f) * focalLength * focalLength;
                circle   = circle/(mFStop * (focusDistance - focalLength));
                circle   = fabs(circle);

                if (circle <= c0)
                {
                    interpolator = 0.0f;
                }
                else if (circle <= ratio1*c0)
                {                       // interpolator is in 0.0 .. 0.5 range
                    interpolator = (circle-c0)/((ratio1-1.0f)*c0);
                    interpolator = max(0.0f, interpolator);
                    interpolator = min(1.0f, interpolator);
                    interpolator = 0.5f * interpolator;
                }
                else                    // interpolator is in 0.5 .. 1.0 range
                {
                    interpolator = (circle-ratio1*c0)/((ratio2-ratio1)*c0);
                    interpolator = max(0.0f, interpolator);
                    interpolator = min(1.0f, interpolator);
                    interpolator = 0.5f * (1.0f + interpolator); 
                }
                dInterpolator = static_cast<DWORD>(interpolator * 255.0f) & 0xff;

                output =   (dInterpolator << 24) | (dInterpolator << 16) 
                         | (dInterpolator <<  8) | (dInterpolator <<  0);
		        *pCurrent++ = output;
            }
        }
    }

    // done generating texture: unlock it
    if (mbUsesVolumes)
        mpVolCircleOfConfusionLookup->UnlockBox( 0 );
    else
        mpCircleOfConfusionLookup->UnlockRect( 0 );

    return S_OK;
}

HRESULT CDepthOfField::UpdateCameraParameters()
{
    // c0 is the acceptable circle of confusion diameter (in cm) for the 
    // original resolution texture (in essence it is the size of a pixel in cm).
    // The ratio is how much we can blur this original circle of confusion: 
    // each 9-sample box-filter operation enlarges the circle of confusion 
    // diameter.  Because we are generating two blur targets, we get:
    // ratio = enlargement^(2*kNumOfFilterSteps) 

    D3DSURFACE_DESC ddsd;
    mpTextureFiltered[0]->GetLevelDesc(0, &ddsd);

    float const     c0      = kFilmDimension/static_cast<float>( max(ddsd.Width, ddsd.Height) );
    float const     ratio   = pow(kBlurFactor, 2.0f*kNumOfFilterSteps);

    // ratio * c0 is the most blurred circle of confusion which we are able to 
    // represent.  Thus, lets compute the hyperfocal distance h with respect to 
    // ratio * c0, and derive MinDist and MaxDist with respect to h.  Then we 
    // simply scale all distances in the vertex shader with respect to these 
    // minDist and maxDist.
    // Note: 0.01 multiplier transforms from cm to meters.
    float const hyperFocal  = mFocalLength*mFocalLength/(mFStop * ratio * c0);
    float const denominator = hyperFocal - (mFocusDistance - mFocalLength);
    float       minDistance, maxDistance;

    minDistance = 0.01f * hyperFocal * mFocusDistance/(hyperFocal + mFocusDistance - mFocalLength);
    minDistance = max(minDistance, kCloseClip);

    // far distance is only valid if the denominator is > 0 (otherwise it is infinite)
    maxDistance = kFarClip;    // this value is an ok representation for infinity
    if (denominator > 0.0f)
        maxDistance = min(maxDistance, 0.01f * hyperFocal * mFocusDistance/denominator);

    if (minDistance >= maxDistance)
        maxDistance = minDistance + minDistance - maxDistance;

    // write min- and max-distance to vertex shader memory: the vertex shader
    // converts and stores distance-to-camera to a 0 thru 1 range (used to 
    // look up circle of confusion in the texture stage)
    float const     minMaxDistance[4] = {minDistance/(maxDistance-minDistance), 
                                            1.0f    /(maxDistance-minDistance), 
                                         2.0f, 
                                         1.0f};
    m_pD3DDev->SetVertexShaderConstant(CV_MINMAX_DIST, minMaxDistance, 1);

    // write the current focus distance and focla length to memory 
    // (normalized to between 0 and 1)
    float const     focusConstants[4] = {(mFocusDistance-kMinFocusDistance)/(kMaxFocusDistance-kMinFocusDistance), 
                                         (mFocalLength  -kMinFocalLength)  /(kMaxFocalLength  -kMinFocalLength),
                                         0.0f, 
                                         0.0f };

    m_pD3DDev->SetVertexShaderConstant(CV_FOCUS_CONST, focusConstants, 1);

    return S_OK;
}

void CDepthOfField::MouseMove(HWND hWnd, int x, int y)
{
    mpMouseUI->OnMouseMove(x, y);
}

void CDepthOfField::MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y)
{
	if(button == MOUSE_LEFTBUTTON)
	{
		if(bDown)
			mpMouseUI->OnLButtonDown(x, y);
		else
			mpMouseUI->OnLButtonUp(x, y);
	}
}

void CDepthOfField::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
{
	eEBKeyAction Action = TranslateEffectKey(dwKey, nFlags, bDown);

    m_bKey['P'] = m_bKey['U'] = m_bKey['L'] = m_bKey['K'] = false;

    switch ( Action )
    {
		case EB_HELP:
		{
			::MessageBoxEx( NULL, " Help : F1 - Help \n\n Home - Reset To Defaults \n\n W - Wireframe Toggle \n\n I/O - Zoom Camera In/Out\n\n U/P - Change Focus-Distance\n\n K/L or PageUp/PageDn - Change F-Stop\n\n Left Button & Mouse - Rotate Camera\n\n Shift Left Button & Mouse - Pan Camera \n\n Ctrl Left Button & Mouse - Move Camera In & Out\n\n",
				   "Help", MB_ICONINFORMATION | MB_TASKMODAL, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ) );
		}
		break;

		case EB_WIREFRAME:
        {
            mbWireFrame = !mbWireFrame;
			m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;            
        }
		break;

		case EB_RESET:
        {
            mpMouseUI->Reset();
            mFStop                = 1.0f;       
            mFocalLength          = 20.0f;
            mFocusDistance        = 2536.0f;
            m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
			UpdateCameraParameters();
            GenerateCircleOfConfusionTexture();
        }
		break;

        case EB_ADD:
            m_bKey['P'] = true;
            break;

        case EB_SUBTRACT:
            m_bKey['U'] = true;
            break;

        case EB_PAGEUP:
            m_bKey['L'] = true;
            break;

        case EB_PAGEDOWN:
            m_bKey['K'] = true;
            break;
            
        default:
            m_bKey[dwKey] = bDown;
            break;
    }
}

void CDepthOfField::SetViewFromKeyboardInput(EBTimer* pTimer)
{
    float   const   kSpeed              = pTimer->GetSecsPerFrame() * 10.0f;
    float   const   moveSpeed           = kSpeed * 2.0f;
    float   const   fStopChange         = kSpeed * 0.5f; // multiply fStop         by this value on every key-press
    float   const   minFStop            = 0.6f;
    float   const   maxFStop            = 22.0f;
    float   const   focusDistanceChange = kSpeed * 0.02f; // multiply focusDistance by this value on every key-press
    float   const   focalLengthChange   = kSpeed * 0.05f;  // multiply focalLength   by this value on every key-press

    D3DXVECTOR3 const   yVector(0.0f, 1.0f, 0.0f);
    float       const   h = D3DXVec3Dot(&yVector, &mCamLookDirection);
    D3DXVECTOR3         xzProjectedLookDirection(mCamLookDirection - h*yVector);

    D3DXVec3Normalize(&xzProjectedLookDirection, &xzProjectedLookDirection);
    
    D3DXVECTOR3         rightVector;         
    D3DXVec3Cross(&rightVector, &yVector, &xzProjectedLookDirection);
    
    bool    regenerateTexture = false;
    bool    updateConstants   = false;

    if(m_bKey[VK_UP]    || m_bKey[VK_NUMPAD8])
        mpMouseUI->Translate(moveSpeed*xzProjectedLookDirection);
	if(m_bKey[VK_DOWN]  || m_bKey[VK_NUMPAD2])
        mpMouseUI->Translate(- moveSpeed*xzProjectedLookDirection);
	if(m_bKey[VK_LEFT]  || m_bKey[VK_NUMPAD4])
        mpMouseUI->Translate(- 0.25f*moveSpeed*rightVector);
	if(m_bKey[VK_RIGHT] || m_bKey[VK_NUMPAD6])              
        mpMouseUI->Translate(0.25f*moveSpeed*rightVector);
	if(m_bKey[VK_PRIOR])
        mpMouseUI->Translate(0.25f*moveSpeed*yVector);
	if(m_bKey[VK_NEXT])
        mpMouseUI->Translate(- 0.25f*moveSpeed*yVector);
    
    D3DXVECTOR4 position;

    D3DXVec3Transform(&position, &D3DXVECTOR3(0.0f, 1.0f, -25.0f), &mpMouseUI->GetTranslationMatrix());
    mCamPosition = D3DXVECTOR3(position.x, max(0.1f, position.y), position.z);

    if (m_bKey['P'])
    {
        mFocusDistance += focusDistanceChange*mFocusDistance;
        if (mFocusDistance > kMaxFocusDistance)
            mFocusDistance = kMaxFocusDistance;
        updateConstants = true;
    }
    if (m_bKey['U'])
    {
        mFocusDistance -= focusDistanceChange*mFocusDistance;
        if (mFocusDistance < kMinFocusDistance)
            mFocusDistance = kMinFocusDistance;
        updateConstants = true;
    }

    if (m_bKey['L'])
    {
        mFStop += fStopChange*mFStop;
        if (mFStop > maxFStop)
            mFStop = maxFStop;
        regenerateTexture = true;
    }
    if (m_bKey['K'])
    {
        mFStop -= fStopChange*mFStop;
        if (mFStop < minFStop )
            mFStop = minFStop ;
        regenerateTexture = true;
    }
    if (m_bKey['I'])
    {
        mFocalLength += focalLengthChange*mFocalLength;
        if (mFocalLength > kMaxFocalLength)
            mFocalLength = kMaxFocalLength;
        updateConstants = true;
        if (! mbUsesVolumes) 
            regenerateTexture = true;
    }
    if (m_bKey['O'])
    {
        mFocalLength -= focalLengthChange*mFocalLength;
        if (mFocalLength < kMinFocalLength)
            mFocalLength = kMinFocalLength;
        updateConstants = true;
        if (! mbUsesVolumes) 
            regenerateTexture = true;
    }

    // write constants
    if (regenerateTexture || updateConstants)
        UpdateCameraParameters();

    // regenerate texture and print status
    if (regenerateTexture)
        GenerateCircleOfConfusionTexture();
}

