/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
        
******************************************************************************/

#include "eb_effect.h"
#include "nvmesh.h"
#include "nvdevice.h"
#include "shader_Shadows.h"
#include "Constants.h"
#include "NVFileDialog.h"

using namespace nv_objects;
using namespace std;

DECLARE_EFFECT_MAIN()

#pragma pack( push, 1 )

class TLVertex
{
public:
	D3DXVECTOR4 Position;
	DWORD Diffuse;
	DWORD Specular;
	D3DXVECTOR2 Texture;
	enum FVF_Flags
	{
		FVF = (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)
	};
};

#pragma pack( pop )

extern "C"
{

__declspec(dllexport) unsigned int GetNumEffects() { return 1; }

__declspec(dllexport) EBEffect* CreateEffect(unsigned int EffectNum)
{
	return new CShaderShadows();
}

}

void CShaderShadows::UpdateProperties()
{
	EBEffect::UpdateProperties();
    AddProperty(new EBProperty("Wireframe", OBJECT_MEMBER(mbWireFrame), EBTYPE_BOOL_PROP));

/*    EBEnumProperty* pEnumProp = new EBEnumProperty("Blur Length Options", OBJECT_MEMBER(meBlurLengthOption), EBTYPE_DWORD_PROP);
	
	pEnumProp->AddEnumerant(new EBEnumValue(pEnumProp, "0.5x Blur Length", (DWORD)HALFxBLURLENGTH,   EBTYPE_DWORD_PROP));
	pEnumProp->AddEnumerant(new EBEnumValue(pEnumProp, "1.0x Blur Length", (DWORD)ONExBLURLENGTH,    EBTYPE_DWORD_PROP));
	pEnumProp->AddEnumerant(new EBEnumValue(pEnumProp, "1.5x Blur Length", (DWORD)ONEHALFxBLURLENGTH,EBTYPE_DWORD_PROP));
	AddProperty(pEnumProp);
*/
	AddProperty(new EBProperty("Load .X File", OBJECT_MEMBER(m_bEnableShadows), EBTYPE_BOOL_PROP));
	AddProperty(new EBProperty("Enable Shadows", OBJECT_MEMBER(m_bEnableShadows), EBTYPE_BOOL_PROP));
	AddProperty(new EBProperty("ShowShadowMap", OBJECT_MEMBER(m_bShowShadowMap), EBTYPE_BOOL_PROP));
    AddProperty(new EBProperty("Pause", OBJECT_MEMBER(m_bPause), EBTYPE_BOOL_PROP));

	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Shadows", GetFilePath("ShadowShow.nvv"), EBTYPE_STRING_PROP));
}

CShaderShadows::CShaderShadows()
    : m_pNVDevice(NULL)
	, m_pFloorMesh(NULL)
	, m_pObjectMesh(NULL)
	, m_pShadowMap(NULL)
	, m_pShadowGen(NULL)
	, m_pShadowZBuffer(NULL)
	, m_pTigerTexture( NULL )
	, m_pShowShadowMapVB( NULL )
	, m_pMouseUI( NULL )
    , m_bEnableShadows( true )
    , m_bShowShadowMap(   false )
    , m_bPause(           false )
    , mbWireFrame       ( false )
{
	m_dwShadowShowShader = 0;
	m_dwShadowGenShader = 0;

	m_strEffectName = "Shadows";
	m_strEffectLocation = "Pixel Shaders";
	m_strEffectPixelShader = "";
	m_strEffectVertexShader = GetFilePath("ShadowShow.nvv");
}

CShaderShadows::~CShaderShadows()
{
	Free();	
}

#pragma optimize ("", off)
HRESULT CShaderShadows::Initialize(IDirect3DDevice8* pDev)
{
	HRESULT     hr;

	m_ShadowSize = 256;

	m_pD3DDev = pDev;
	pDev->AddRef();

	m_pNVDevice = new NVShadowsDevice(pDev, this);

	//initialize mouse UI
	RECT rect;
	rect.left = rect.top = 0;
	D3DVIEWPORT8 viewport;
	m_pD3DDev->GetViewport(&viewport);
	rect.bottom = viewport.Height;
	rect.right  = viewport.Width;
	m_pMouseUI = new MouseUI((const RECT)rect);

	{
		// Create a big square for rendering the fogmap
		if( FAILED( m_pD3DDev->CreateVertexBuffer( 4*sizeof(TLVertex), D3DUSAGE_WRITEONLY, TLVertex::FVF, 
													  D3DPOOL_MANAGED, &m_pShowShadowMapVB ) ) )
			return E_FAIL;
		// Create a quad for the final pass
		TLVertex* v;
		FLOAT sx = m_ShadowSize * 2;
		FLOAT sy = m_ShadowSize * 2;
		m_pShowShadowMapVB->Lock( 0, 0, (BYTE**)&v, 0 );

		v[0].Position = D3DXVECTOR4(0, sy, 0.5f, 1);
		v[0].Diffuse = 0x0;
		v[0].Specular = 0;
		v[0].Texture = D3DXVECTOR2(0, 1);


		v[1].Position = D3DXVECTOR4(0, 0, 0.5f, 1);
		v[1].Diffuse = 0x0;
		v[1].Specular = 0;
		v[1].Texture = D3DXVECTOR2(0, 0);
    

		v[2].Position = D3DXVECTOR4(sx, sy, 0.5f, 1);
		v[2].Diffuse = 0x0;
		v[2].Specular = 0;
		v[2].Texture = D3DXVECTOR2(1, 1);


		v[3].Position = D3DXVECTOR4(sx, 0, 0.5f, 1);
		v[3].Diffuse = 0x0;
		v[3].Specular = 0;
		v[3].Texture = D3DXVECTOR2(1, 0);

		m_pShowShadowMapVB->Unlock();
	}

	DWORD        dwVBFlags = D3DUSAGE_WRITEONLY;
	LPD3DXBUFFER pCode = NULL;

    DWORD declaration[] = 
    {
        D3DVSD_STREAM(0),
        D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3),
        D3DVSD_REG(D3DVSDE_NORMAL,   D3DVSDT_FLOAT3),
        D3DVSD_REG(D3DVSDE_TEXCOORD0,D3DVSDT_FLOAT2),
        D3DVSD_END()
    };


	m_dwShadowGenShader = 0;
	hr = LoadAndCreateShader(GetFilePath("ShadowGen.vso"), &declaration[0], 0, SHADERTYPE_VERTEX, &m_dwShadowGenShader);
	if (FAILED(hr))
		return hr;

	m_dwShadowShowShader = 0;
	hr = LoadAndCreateShader(GetFilePath("ShadowShow.vso"), &declaration[0], 0, SHADERTYPE_VERTEX, &m_dwShadowShowShader);
	if (FAILED(hr))
		return hr;

	m_dwDrawObjectShader = 0;

	hr = LoadAndCreateShader(GetFilePath("DrawObject.vso"), &declaration[0], 0, SHADERTYPE_VERTEX, &m_dwDrawObjectShader);
	if (FAILED(hr))
		return hr;

	std::string  objectNameToLoad("bigship1");

	//m_XFileDialog.Open( objectNameToLoad );

    m_pObjectMesh = new NVMesh();
    hr = m_pObjectMesh->Create(m_pNVDevice, GetFilePath(objectNameToLoad + ".x"));
	if (FAILED(hr))
	{
		m_strLastError = "Could not load " + GetFilePath(objectNameToLoad + ".x");
		return hr;
	}
	m_pObjectMesh->SetFVF(m_pNVDevice, ShadowsVertex::FVF );
	m_pObjectMesh->SetVertexShader(m_dwShadowGenShader);
    m_pObjectMesh->RestoreDeviceObjects(m_pNVDevice);

	m_Models.push_back( ModelInstance( m_pObjectMesh, m_pObjectMesh->m_pTexture ) );
	
	//(THIS_ UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,IDirect3DSurface8** ppSurface) PURE;
	hr = m_pNVDevice->GetDevice()->CreateDepthStencilSurface( m_ShadowSize,
															  m_ShadowSize,
															  D3DFMT_D24X8,
															  D3DMULTISAMPLE_NONE,
															  &m_pShadowZBuffer );
	if (FAILED(hr))
	{
		m_strLastError = "Could not Create Shadow ZBuffer";
		return hr;
	}

    //STDMETHOD(CreateTexture)(THIS_ UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture8** ppTexture) PURE;
	hr = m_pNVDevice->GetDevice()->CreateTexture( m_ShadowSize,
												  m_ShadowSize,
												  1,
												  D3DUSAGE_RENDERTARGET,
												  D3DFMT_A8R8G8B8,
												  D3DPOOL_DEFAULT,
											      &m_pShadowMap );
	if (FAILED(hr))
	{
		m_strLastError = "Could not Create Shadow ZBuffer";
		return hr;
	}

	hr = D3DXCreateTextureFromFileEx(m_pD3DDev, 
		GetFilePath( "ShadowMap.tga").c_str(),
		2048,
		256,
		1,
		0,
		D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT,
		D3DX_FILTER_NONE,
		D3DX_FILTER_NONE,
		0,
		NULL,
		NULL,
		&m_pShadowGen );

	hr = D3DXCreateTextureFromFileEx(m_pD3DDev, 
		GetFilePath( "tiger.dds").c_str(),
		D3DX_DEFAULT, //w
		D3DX_DEFAULT,	// h
		D3DX_DEFAULT,	// mips
		0,
		D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT,
		D3DX_FILTER_LINEAR,
		D3DX_FILTER_LINEAR,
		0,
		NULL,
		NULL,
		&m_pTigerTexture );

	if (FAILED(hr))
	{
		m_strLastError = "Could not Load Shadow GenTexture " + GetFilePath(objectNameToLoad + ".dds");
		return hr;
	}

	D3DLIGHT8 aLight;

	aLight.Type = D3DLIGHT_SPOT;
	aLight.Diffuse.r = 1.0f;
	aLight.Diffuse.g = 1.0f;
	aLight.Diffuse.b = 1.0f;

	aLight.Specular.r = 0.0f;
	aLight.Specular.g = 0.0f;
	aLight.Specular.b = 0.0f;

	aLight.Ambient.r = 0.0f;
	aLight.Ambient.g = 0.0f;
	aLight.Ambient.b = 0.0f;

	aLight.Position.x = 0.0f;
	aLight.Position.y = 100.0f;
	aLight.Position.z = 0.0f;

	aLight.Range = 100.0f;
	aLight.Falloff = 100.0f;

	aLight.Direction.x =  0.0f;
	aLight.Direction.y = -1.0f;
	aLight.Direction.z =  0.0f;

	aLight.Attenuation0 = 0.0f;
	aLight.Attenuation1 = 1.0f;
	aLight.Attenuation2 = 0.0f;

	aLight.Theta = 0.0f;
	aLight.Phi = 45.0f;

	m_Lights[ 0 ] = aLight;
   
    m_numVertices = m_pObjectMesh->GetSysMemMesh()->GetNumVertices();

	// Load the seafloor x file and extract its mesh
    m_pFloorMesh = new NVMesh();
    hr = m_pFloorMesh->Create(m_pNVDevice, GetFilePath("seafloor.x"));
	if (FAILED(hr))
	{
		m_strLastError = "Could not create seafloor.x";
		return hr;
	}
	m_pFloorMesh->SetFVF(m_pNVDevice, ShadowsVertex::FVF );
	m_pFloorMesh->SetVertexShader(m_dwShadowGenShader);

    IDirect3DVertexBuffer8* pVB;

    // Add some "hilliness" to the terrain
    if( SUCCEEDED( m_pFloorMesh->GetSysMemMesh()->GetVertexBuffer( &pVB ) ) )
    {
        ShadowsVertex* pVertices;
        DWORD   dwNumVertices = m_pFloorMesh->GetSysMemMesh()->GetNumVertices();

        pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 );

        for( DWORD i=0; i<dwNumVertices; i++ )
		{
			pVertices[i].Position.y  += (rand()/(FLOAT)RAND_MAX);
			pVertices[i].Position.y  += (rand()/(FLOAT)RAND_MAX);
			pVertices[i].Position.y  += (rand()/(FLOAT)RAND_MAX);
			pVertices[i].Texture.x *= 10;
			pVertices[i].Texture.y *= 10;
		}

        pVB->Unlock();
        pVB->Release();
    }

    m_pFloorMesh->RestoreDeviceObjects(m_pNVDevice);

    // set up render state
    m_pD3DDev->SetRenderState( D3DRS_LIGHTING,	     FALSE );
    m_pD3DDev->SetRenderState( D3DRS_STENCILENABLE,  FALSE );

    m_pD3DDev->SetRenderState( D3DRS_CULLMODE,       D3DCULL_NONE ); 
    m_pD3DDev->SetRenderState( D3DRS_FOGENABLE,      FALSE);

    m_pD3DDev->SetRenderState( D3DRS_DITHERENABLE,   FALSE );
    m_pD3DDev->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
    m_pD3DDev->SetRenderState( D3DRS_ZENABLE,        TRUE );

	m_pD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT );
    m_pD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT );
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE );

    m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pD3DDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    m_pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    // Setup render states
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2);
	m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	m_pD3DDev->SetVertexShaderConstant(CV_CONSTANTS, D3DXVECTOR4(0.0f,1.0f,0.5f,2.0f), 1);
	
    // Projection set up
    D3DXMATRIX        matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(90.0f), 1.0f, 1.0f, 1000.0f);
	m_pNVDevice->SetProjectionTransform(&matProj);
	return S_OK;
}
#pragma optimize ("", on)

HRESULT CShaderShadows::Free()
{
	SAFE_DELETE( m_pMouseUI );

	m_Models.clear();

	SAFE_RELEASE( m_pTigerTexture );
	SAFE_RELEASE(m_pShowShadowMapVB);

	SAFE_DELETE(m_pFloorMesh);

	SAFE_DELETE(m_pObjectMesh);
    SAFE_RELEASE(m_pShadowMap);
    SAFE_RELEASE(m_pShadowGen);
    SAFE_RELEASE(m_pShadowZBuffer);

	SAFE_DELETE(m_pNVDevice);

	if (m_pD3DDev)
	{
		m_pD3DDev->DeleteVertexShader(m_dwShadowGenShader);
		m_pD3DDev->DeleteVertexShader(m_dwShadowShowShader);
		m_pD3DDev->DeleteVertexShader( m_dwDrawObjectShader );
		SAFE_RELEASE(m_pD3DDev);
	}

	return S_OK;
}

HRESULT CShaderShadows::Start()
{
	return S_OK;
}

HRESULT CShaderShadows::SetTransform()
{
	D3DXMATRIX matWorld, matView, matProj;
	D3DXMATRIX matWorldView, matWorldViewIT;
	float      unused;

	matWorld = m_pNVDevice->GetWorldTransform();
	matView  = m_pNVDevice->GetViewTransform();
	matProj  = m_pNVDevice->GetProjectionTransform();

	m_pD3DDev->SetTransform( D3DTS_VIEW, (D3DMATRIX*)(&matView) );
	m_pD3DDev->SetTransform( D3DTS_WORLD, (D3DMATRIX*)(&matWorld));
	m_pD3DDev->SetTransform( D3DTS_PROJECTION, (D3DMATRIX*)(&matProj));

	D3DXMatrixMultiply(&matWorldView,   &matWorld, &matView);
    D3DXMatrixInverse( &matWorldViewIT, &unused, &matWorldView);
	
	 //Projection to clip space
	//D3DXMatrixTranspose(&matProj, &matProj);
	//m_pD3DDev->SetVertexShaderConstant(CV_PROJ_TXF_0, &matProj(0, 0), 4);

    // Projection to view-space
//	D3DXMatrixTranspose(&matWorldView, &matWorldView);
//    m_pD3DDev->SetVertexShaderConstant(CV_CURR_WORLDVIEW_TXF_0, &matWorldView(0, 0), 3);

    return S_OK;
}

D3DXMATRIX CShaderShadows::CreateLightMatrix( const unsigned int& i )
{
	D3DXMATRIX matView; 

	D3DXMatrixIdentity( &matView );

	assert( i < LIGHT_COUNT );
	if ( i < LIGHT_COUNT )
	{
		// Light Camera setup
		D3DXVECTOR3 vEyePt    = m_Lights[ i ].Position;
		D3DXVECTOR3 vLookatPt = m_Lights[ i ].Position;
					vLookatPt += m_Lights[ i ].Direction;

		D3DXVECTOR3 vUp       = D3DXVECTOR3( 0.0f,  0.0f, 1.0f );

		D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUp);
	}
	return matView;
}

HRESULT CShaderShadows::CreateShadowMap()
{
	// Set up Render to Texture

	D3DXMATRIX matView = CreateLightMatrix( 0 );

	m_pNVDevice->SetViewTransform(&matView);

	LPDIRECT3DSURFACE8 pSurface = 0;
	LPDIRECT3DSURFACE8 pBackBuffer = 0;
	LPDIRECT3DSURFACE8 pZBuffer = 0;

	HRESULT hr = D3D_OK;

	{ // save back buffer & Z buffer
		hr = m_pD3DDev->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );

		hr = m_pD3DDev->GetDepthStencilSurface( &pZBuffer );
	}

	hr = m_pShadowMap->GetSurfaceLevel(0, &pSurface );

	// Dx8 will automatically set up the Viewport for us when we call SetRenderTarget()
	hr = m_pD3DDev->SetRenderTarget( pSurface, m_pShadowZBuffer );

	// Clear to objectID # 00, at closest distance

	hr = m_pD3DDev->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0x00000000, 1.0, 0xffffffff );

	m_pD3DDev->SetStreamSource(0, m_pObjectMesh->m_pVB, sizeof(ShadowsVertex));

	m_pD3DDev->SetVertexShaderConstant( CV_CURR_WORLD2LIGHT_TXF_0, &matView,  4 );

	m_pD3DDev->SetVertexShader(m_dwShadowGenShader);
	m_pD3DDev->SetTexture(0, m_pShadowGen);

	m_pObjectMesh->SetVertexShader( m_dwShadowGenShader );
	m_pObjectMesh->SetTexture( m_pShadowGen);

	m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	m_pD3DDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
	m_pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );

	m_pD3DDev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );

	// Setup render states
	m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	m_pD3DDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	m_pD3DDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	//   Set up projection matrix for rendering
	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(90.0f), 1.0f, 1.0f, 1000.0f);
	m_pNVDevice->SetProjectionTransform(&matProj);

	// For each object

	// Set up Squish matrix that will take the Z depth from the light and map into the depth texture
	//  such that 0..1 maps to center - radius .. center + radius

	// Go through each model and draw to the shadow map

	for ( unsigned int i = 0; i < m_Models.size(); ++i )
	{
		// TODO: Handle case where sphere overlaps that light plane, just clamp the range...?
		D3DXMATRIX tempMatrix, modelToLightMatrix;

		D3DXMATRIX matModel = m_Models[ i ].mModelMatrix;

		D3DXMatrixMultiply( &modelToLightMatrix, &matModel, &matView);

		D3DXMatrixMultiply( &tempMatrix, &modelToLightMatrix, &matProj );

		D3DXMatrixTranspose( &tempMatrix, &tempMatrix );
		m_pD3DDev->SetVertexShaderConstant( CV_MVP_TXF_0, &tempMatrix,  4 );

		matView = CreateLightMatrix( 0 );

		D3DXMatrixMultiply( &modelToLightMatrix, &matModel, &matView);

		//D3DXMatrixMultiply( &tempMatrix, &modelToLightMatrix, &matProj );

		// set its objectID in the vertex program constants
		unsigned int theObjectID = 1 + i;
		D3DXVECTOR4 vColorCurrent( (float)( theObjectID ) / 255.0f, 0.0f, 0.0f, 0.0f );
		m_pD3DDev->SetVertexShaderConstant( CV_CURR_OBJECTID_IN_TEXELS, &vColorCurrent,  1 );

		// Compute Squish Matrix - Maps object from Model Space to Light View Space and scales the Z range
		//  to be 0..1 within the object's bounding sphere
		{
//			aBounds.Transform( &modelToLightMatrix );

			NVBounds aBounds = *(m_pObjectMesh->GetBounds());

			aBounds.Transform( &modelToLightMatrix );

			// Only really need radius with respect to light space Z

			// We are overestimating a bit to eliminate clamping
			float theRadius = max( 1.0f, ( aBounds.m_vecMaxExtents.z - aBounds.m_vecMinExtents.z ) / 1.95f );

			D3DXVECTOR4 center( aBounds.m_vecCenter.x, 
								aBounds.m_vecCenter.y,
								aBounds.m_vecCenter.z,
								1.0f );
			// Use center.z - radius to map to a Z of 0

			D3DXMATRIX SquishMatrix( 1.0f, 0.0f, 0.0f, 0.0f,
									 0.0f, 1.0f, 0.0f, 0.0f,
									 0.0f, 0.0f, 1.0f, 0.0f,
									 0.0f, 0.0f, -( center.z - theRadius  ), 1.0f );

			D3DXMatrixMultiply( &tempMatrix, &modelToLightMatrix, &SquishMatrix  );

			D3DXMATRIX SquishMatrix2( 1.0f, 0.0f, 0.0f, 0.0f,
									  0.0f, 1.0f, 0.0f, 0.0f,
									  0.0f, 0.0f, 0.5f / theRadius, 0.0f, 
									  0.0f, 0.0f, 0.0f , 1.0f );

			D3DXMatrixMultiply( &SquishMatrix, &tempMatrix, &SquishMatrix2 );

			// Set the SquishMatrix
			D3DXMatrixTranspose( &SquishMatrix, &SquishMatrix );
			m_pD3DDev->SetVertexShaderConstant( CV_CURR_SQUISH_TXF_0, &SquishMatrix,  4 );

		}

		// Set up Texture States & Render
		{
			// Don't mip map or filter the ShadowGen Texture

			m_pD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT );
			m_pD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT );
			m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_POINT );

			// Clamp outside of range 0..1
			m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
			m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );

			m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
			m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );

			m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

			m_pD3DDev->SetRenderState( D3DRS_TEXTUREFACTOR, 0x80808080 );

			m_pD3DDev->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0 );
			m_pD3DDev->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1 );

			// Setup render states
			m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
			m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

			m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
			m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
			m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

			m_pD3DDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
			m_pD3DDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

			hr = m_pObjectMesh->Render( m_pNVDevice );
		}

	}


	{ // Restore Back & Z buffers

		if ( pSurface )
		{
			pSurface->Release(), pSurface = 0;
		}

		if ( m_pShadowZBuffer )
		{
			//m_pShadowZBuffer->Release();
		}

		// Dx8 will automatically set up the Viewport for us when we call SetRenderTarget()
		hr = m_pD3DDev->SetRenderTarget( pBackBuffer, pZBuffer );

		// Give back refcounts on our local copies
		pBackBuffer->Release();
		pZBuffer->Release();
	}

	return hr;

}


HRESULT	CShaderShadows::DrawObjects()
{
	D3DXMATRIX matView, matTemp, matProj;

    // Projection set up
	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(90.0f), 1.0f, 1.0f, 1000.0f);
	m_pNVDevice->SetProjectionTransform(&matProj);

	D3DXMatrixMultiply( &matView, &m_pMouseUI->GetRotationMatrix(), &m_pMouseUI->GetTranslationMatrix() );

	for ( unsigned int i = 0; i < m_Models.size(); ++i )
	{
		m_Models[ i ].m_pNVMesh->SetVertexShader( m_dwDrawObjectShader );
		m_Models[ i ].m_pNVMesh->SetTexture(m_Models[ i ].m_pOldTexture);

		D3DXMatrixMultiply( &matView, &m_pMouseUI->GetRotationMatrix(), &m_pMouseUI->GetTranslationMatrix() );
		D3DXMatrixMultiply( &matTemp, &m_Models[ i ].mModelMatrix, &matView );
		D3DXMatrixMultiply( &matTemp, &matTemp, &matProj);

		D3DXMatrixTranspose( &matTemp, &matTemp);
		m_pD3DDev->SetVertexShaderConstant( CV_MVP_TXF_0, &matTemp,  4 );

		m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
		m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );

		m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
		m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );

		m_pD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
		m_pD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
		m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );

		m_pD3DDev->SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
		m_pD3DDev->SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
		m_pD3DDev->SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );

		m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
		m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT );

		m_pD3DDev->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT );
		m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE );

		m_pD3DDev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

		m_pNVDevice->SetWorldTransform( & m_Models[ i ].mModelMatrix );

		SetTransform();

		m_Models[ i ].m_pNVMesh->Render(m_pNVDevice);
	}

	return S_OK;
}

HRESULT	CShaderShadows::DrawSeaFloor()
{
	D3DXMATRIX matWorld,matView;
    // Sea floor
	D3DXMatrixScaling( &matWorld, 1.0f, 1.0f, 1.0f);
	m_pNVDevice->SetWorldTransform(&matWorld);

	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );

	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );

	D3DXMatrixMultiply( &matView, &m_pMouseUI->GetRotationMatrix(), &m_pMouseUI->GetTranslationMatrix() );

	m_pNVDevice->SetViewTransform( &matView );

	m_pFloorMesh->SetVertexShader( 0 );

	m_pD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );

	m_pD3DDev->SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	m_pD3DDev->SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );

	SetTransform();

	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT );

	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT );
	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE );

	m_pD3DDev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

	m_pFloorMesh->Render( m_pNVDevice );

	return S_OK;
}

HRESULT CShaderShadows::Tick(EBTimer* pTimer)
{
	HRESULT hr = S_OK;
	D3DXMATRIX matWorld;
	D3DXMATRIX matView;

	m_pD3DDev->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL  );

    float const kSpeed = .25f;
	float const fKickFreq = kSpeed * pTimer->GetDuration(); 
	float const fPhase    = kSpeed * pTimer->GetDuration() / 3.0f;

    // Move the object in a circle
    D3DXMATRIX      matScale, matTrans1, matRotate1, matRotate2;
    D3DXMATRIX      matBank,  matLocalRotate;

    D3DXMatrixRotationY(   &matLocalRotate, D3DX_PI/2.0f);
    D3DXMatrixRotationX(   &matBank,        D3DX_PI/8.0f);
    D3DXMatrixRotationZ(   &matRotate1, 2*D3DX_PI); 
    D3DXMatrixRotationY(   &matRotate2, fPhase );
    D3DXMatrixScaling(     &matScale, 1.0f, 1.0f, 1.0f );
    D3DXMatrixTranslation( &matTrans1, -12*sinf(fPhase) , sinf(fKickFreq) + 4, 10-30*cosf(fPhase) );

	D3DXMatrixIdentity(&matWorld);
    D3DXMatrixMultiply( &matWorld, &matTrans1,      &matWorld);
	D3DXMatrixMultiply( &matWorld, &matScale,       &matWorld);
	D3DXMatrixMultiply( &matWorld, &matRotate2,     &matWorld);
	D3DXMatrixMultiply( &matWorld, &matRotate1,     &matWorld);	
    D3DXMatrixMultiply( &matWorld, &matBank,        &matWorld);
    D3DXMatrixMultiply( &matWorld, &matLocalRotate, &matWorld);

    m_pD3DDev->SetRenderState(D3DRS_FILLMODE, (mbWireFrame) ? D3DFILL_WIREFRAME : D3DFILL_SOLID);

    if (m_bPause)
        return hr;

	m_Models[ 0 ].mModelMatrix = matWorld;

	CreateShadowMap();

    // Camera setup
	D3DXVECTOR3 const vEyePt    = D3DXVECTOR3( 5.0f,  5.0f, 0.0f);
	D3DXVECTOR3 const vLookatPt = D3DXVECTOR3( 0.0f,  0.0f, 0.0f);
	D3DXVECTOR3 const vUp       = D3DXVECTOR3( 0.0f,  0.0f, 1.0f );

	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUp);
	m_pNVDevice->SetViewTransform(&matView);

	hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, BACKGROUND_COLOR, 1.0, 0);

    // Projection set up
    D3DXMATRIX        matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(90.0f), 1.0f, 1.0f, 1000.0f);
	m_pNVDevice->SetProjectionTransform(&matProj);

	D3DXMatrixMultiply( &matView, &m_pMouseUI->GetRotationMatrix(), &m_pMouseUI->GetTranslationMatrix() );

	DrawSeaFloor();
	DrawObjects();

	m_pD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT );
    m_pD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT );
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE );

	m_pD3DDev->SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_POINT );
    m_pD3DDev->SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_POINT );
	m_pD3DDev->SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_NONE );

	unsigned int theObjectID = 0x00;
	D3DXVECTOR4 vColorCurrent( (float)( theObjectID ) / 255.0f, 0.0f, 0.0f, 0.0f );
	m_pD3DDev->SetVertexShaderConstant( CV_CURR_OBJECTID_IN_TEXELS, &vColorCurrent,  1 );

	m_pFloorMesh->SetVertexShader( m_dwDrawObjectShader );
	m_pFloorMesh->SetVertexShader( m_dwShadowShowShader );


	// Set tfactor to r = 1, g = 1, b = 1, a = 0
	m_pD3DDev->SetRenderState( D3DRS_TEXTUREFACTOR, 0x00ffffff );

	// Compare by doing ShadowGen - ShadowMap
	//
	//  Shadow Gen should always be >= ShadowMap
	//

	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT );

	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT );
    m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SUBTRACT );
	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE );

	m_pD3DDev->SetTextureStageState(2, D3DTSS_COLORARG1, D3DTA_CURRENT );
    m_pD3DDev->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DOTPRODUCT3 );
	m_pD3DDev->SetTextureStageState(2, D3DTSS_COLORARG2, D3DTA_TFACTOR );

	m_pD3DDev->SetTextureStageState(2, D3DTSS_ALPHAARG1, D3DTA_CURRENT );
    m_pD3DDev->SetTextureStageState(2, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
	m_pD3DDev->SetTextureStageState(2, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );

    m_pD3DDev->SetTextureStageState(3, D3DTSS_COLOROP, D3DTOP_DISABLE );

	// Reject pixels that are in shadow
	m_pD3DDev->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	m_pD3DDev->SetRenderState( D3DRS_ALPHAREF, 0x0 );
	m_pD3DDev->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_EQUAL  );

	m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	m_pD3DDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
	m_pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );

	// Clamp outside of range 0..1
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );

	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );

	D3DXMATRIX tempMatrix, modelToLightMatrix;

	{
		D3DXMATRIX matView = CreateLightMatrix( 0 ); 

		// TODO: Handle case where sphere overlaps that light plane, just clamp the range...?
		D3DXMATRIX matModel;
		D3DXMatrixIdentity( &matModel );

		D3DXMatrixMultiply( &modelToLightMatrix, &matModel, &matView);

		D3DXMatrixMultiply( &tempMatrix, &modelToLightMatrix, &matProj );

		D3DXMATRIX temp2( 1.0f, 0.0f, 0.0f, 0.0f,
						  0.0f, 1.0f, 0.0f, 0.0f,
						  0.0f, 0.0f, 1.0f, 0.5f, 
						  0.0f, 0.0f, 0.0f, 0.5f );

		D3DXMatrixMultiply( &tempMatrix, &tempMatrix, &temp2 );

		m_pObjectMesh->SetVertexShader( m_dwShadowShowShader );
		m_pObjectMesh->SetTexture( m_pShadowGen);

		D3DXMatrixTranspose( &tempMatrix, &tempMatrix );
		m_pD3DDev->SetVertexShaderConstant( CV_CURR_WORLD2LIGHT_TXF_0, &tempMatrix,  4 );

		//   Set up projection matrix for rendering
		D3DXMATRIX matProj;
		D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(90.0f), 1.0f, 1.0f, 1000.0f);
		m_pNVDevice->SetProjectionTransform(&matProj);

		D3DXMatrixMultiply( &matView, &m_pMouseUI->GetRotationMatrix(), &m_pMouseUI->GetTranslationMatrix() );
		D3DXMatrixMultiply( &matView, &matView, &matProj);

		D3DXMatrixTranspose( &matView, &matView );
		m_pD3DDev->SetVertexShaderConstant( CV_MVP_TXF_0, &matView,  4 );
		D3DXMatrixTranspose( &matView, &matView);

		m_pD3DDev->SetTexture(0, m_pShadowGen);
		m_pD3DDev->SetTexture(1, m_pShadowMap);

//		m_pFloorMesh->Render(m_pNVDevice);
	}
	
	for ( unsigned int i = 0; i < m_Models.size(); ++i )
	{
		D3DXMATRIX matTemp;

		// Projection set up
		D3DXMATRIX        matProj;
		D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(90.0f), 1.0f, 1.0f, 1000.0f);
		m_pNVDevice->SetProjectionTransform(&matProj);

		D3DXMatrixMultiply( &matView, &m_pMouseUI->GetRotationMatrix(), &m_pMouseUI->GetTranslationMatrix() );
		D3DXMatrixMultiply( &matTemp, &m_Models[ i ].mModelMatrix, &matView );
		D3DXMatrixMultiply( &matTemp, &matTemp, &matProj);

		D3DXMatrixTranspose( &matTemp, &matTemp);
		m_pD3DDev->SetVertexShaderConstant( CV_MVP_TXF_0, &matTemp,  4 );

		m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		m_pD3DDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
		m_pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );

		unsigned int theObjectID = 1 + i;
		D3DXVECTOR4 vColorCurrent( (float)( theObjectID ) / 255.0f, 0.0f, 0.0f, 0.0f );
		m_pD3DDev->SetVertexShaderConstant( CV_CURR_OBJECTID_IN_TEXELS, &vColorCurrent,  1 );

		D3DXMATRIX matModel = m_Models[ i ].mModelMatrix;

		matView = CreateLightMatrix( 0 );

		D3DXMatrixMultiply( &modelToLightMatrix, &matModel, &matView);

		//D3DXMatrixMultiply( &tempMatrix, &modelToLightMatrix, &matProj );

		// off set to texture coordinates
		D3DXMATRIX temp2( 1.0f, 0.0f, 0.0f, 0.0f,
						  0.0f, 1.0f, 0.0f, 0.0f,
						  0.0f, 0.0f, 1.0f, 0.5f, 
						  0.0f, 0.0f, 0.0f, 0.5f );

		D3DXMatrixMultiply( &tempMatrix, &modelToLightMatrix, &temp2 );

		D3DXMatrixTranspose( &tempMatrix, &tempMatrix );
		m_pD3DDev->SetVertexShaderConstant( CV_CURR_WORLD2LIGHT_TXF_0, &tempMatrix,  4 );

		m_Models[ i ].m_pNVMesh->SetTexture(m_pShadowGen);

		m_pD3DDev->SetTexture(1, m_pShadowMap);

		m_pD3DDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_SELECTARG2 );
		m_pD3DDev->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE );

	    m_pD3DDev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );

		m_pD3DDev->SetVertexShader(0);
		m_Models[ i ].m_pNVMesh->SetVertexShader(m_dwShadowShowShader);
		m_Models[ i ].m_pNVMesh->Render(m_pNVDevice);
	}

	m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

	if ( m_bShowShadowMap )
	{
		m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
		m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT );

		m_pD3DDev->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT );
		m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE );

	    m_pD3DDev->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	    m_pD3DDev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );

		m_pD3DDev->SetRenderState(D3DRS_ZENABLE, FALSE);
		m_pD3DDev->SetStreamSource(0, m_pShowShadowMapVB, sizeof(TLVertex));
		m_pD3DDev->SetVertexShader(TLVertex::FVF );
		m_pD3DDev->SetTexture(0, m_pShadowMap);
		m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2 );

		// Restore render states
		m_pD3DDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	}

	return hr;
}

HRESULT CShaderShadows::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
{
    // check vertex shading support

    // check simultaneous texture support (only need 2...)
    if(pCaps->MaxSimultaneousTextures < 2)
	{
		m_strLastError = "Device does not support 4 simultaneous textures!";
		return E_FAIL;
	}

    // only need very few, simple register combiners...
	if(pCaps->MaxTextureBlendStages < 2)
	{
		m_strLastError = "Device does not support 2 register combiners!";
		return E_FAIL;
	}

	return S_OK;
}

void CShaderShadows::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
{
	eEBKeyAction Action = TranslateEffectKey(dwKey, nFlags, bDown);
	
    switch ( Action )
    {
		case EB_HELP:
		{
			::MessageBoxEx( NULL, " Help : F1 - Help \n\n Home - Reset To Defaults \n\n W - Wireframe Toggle \n\n Pause/Space - Toggle pausing the animation \n\n +/- - Lengthen/Shorten Blur Length",
				   "Help", MB_ICONINFORMATION | MB_TASKMODAL, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ) );
		}
		break;

		case EB_WIREFRAME:
        {
            mbWireFrame = !mbWireFrame;
            m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
        }
		break;

        case EB_PAUSE:
        {
            m_bPause = !m_bPause;
            m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
        }
        break;

		break;

        case EB_RESET :
        {
            m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
            m_pMouseUI->Reset();
            break;
        }

        case VK_LEFT :
        {
            m_pMouseUI->Translate( -1.0f, 0.0f, 0.0f );
            break;
        }

        case VK_RIGHT :
        {
            m_pMouseUI->Translate(  1.0f, 0.0f, 0.0f );
            break;
        }

        case VK_UP :
        {
            m_pMouseUI->Translate(  0.0f, 0.0f, -1.0f );
            break;
        }

        case VK_DOWN :
        {
            m_pMouseUI->Translate( 0.0f, 0.0f, 1.0f );
            break;
        }

        case EB_PAGEUP :
        {
            m_pMouseUI->Translate(  0.0f, 1.0f, 0.0f );
            break;
        }
        case EB_PAGEDOWN :
        {
            m_pMouseUI->Translate(  0.0f, -1.0f, 0.0f );
            break;
        }

        case EB_ADD :
        {
            m_pMouseUI->Translate( 0.0f, 0.0f, 1.0f );
            break;
        }
        case EB_SUBTRACT :
        {
            m_pMouseUI->Translate( 0.0f, 0.0f, -1.0f );
            break;
        }

        default:
            break;
    }
}


void CShaderShadows::MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y)
{
	if(button == MOUSE_LEFTBUTTON)
	{
		if(bDown)
		{
			m_pMouseUI->OnLButtonDown(x, y);
		}
		else
		{
			m_pMouseUI->OnLButtonUp(x, y);
		}
	}
	return;
}

void CShaderShadows::MouseMove(HWND hWnd, int x, int y)
{
	m_pMouseUI->OnMouseMove(x, y);
	return;
}
