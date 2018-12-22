/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/
#include "eb_effect.h"
#include "nvdevice.h"
#include "nvfile.h"
#include "shader_Fog.h"
#include "nvfile.h"
#include "fog.h"

using namespace nv_objects;
using namespace std;

static D3DXVECTOR3 CameraStart(0.0f, -2.0f, -5.0f);
DECLARE_EFFECT_MAIN()

extern "C"
{

__declspec(dllexport) unsigned int GetNumEffects() { return 1; }

__declspec(dllexport) EBEffect* CreateEffect(unsigned int EffectNum)
{	
	switch(EffectNum)
	{
		case 0:
			return new CShaderFog();
			break;
		default:
			return NULL;
	}
}

}

// Old-style D3D vertices
class Vertex
{
public:
	Vertex(const D3DXVECTOR3& Pos, const D3DXVECTOR3& Norm, const D3DXVECTOR2& Tex)
		: Position(Pos), Normal(Norm), Texture(Tex)
	{}

	Vertex()
	{}

	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
	D3DXVECTOR2 Texture;
};

class TLVertex
{
public:
	D3DXVECTOR4 Position;
	DWORD Diffuse;
	DWORD Specular;
	D3DXVECTOR2 Texture;
};

#define D3DFVF_VERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
#define D3DFVF_TLVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)

CShaderFog::CShaderFog()
	: m_vecRoomDimensions(15.0f, 7.0f, 15.0f),
	m_vecChairDimensions(0.75f, 1.5f, 0.75f),
	m_vecTableDimensions(3.5f, 1.2f, 3.5f),
	m_pRoom(NULL),
	m_pChair(NULL),
	m_pTable(NULL),
	m_pWorldBoxVertices(NULL),
	m_pWorldBoxIndices(NULL),
	m_dwFogShader(0),
	m_dwWorldShader(0),
	m_pFogMap(0),
	m_pFogMapVB(NULL),
	m_pNVDevice(NULL),
	m_bShowFogMap(false),
	m_bEnableFog(true),
	m_bWireframe(false),
	m_dwFogMapWidth(64),
	m_dwFogMapHeight(64),
	m_pCamera(NULL),
	m_CameraLookDirection(0.0f, 0.0f, 1.0f)

{
	for (int i = 0; i < 6; i++)
	{
		m_pWorldTextures[i] = NULL;
	}

	m_strEffectLocation = "Vertex Shaders\\Texture Coordinate Generation";
	m_strEffectName = "Height Fog";
	m_strEffectVertexShader = GetFilePath("fog.nvv");
	m_strEffectPixelShader = "";

 
}

CShaderFog::~CShaderFog()
{
	Free();	
}

void CShaderFog::UpdateProperties()
{
	EBEffect::UpdateProperties();
	AddProperty(new EBProperty("Show FogMap", OBJECT_MEMBER(m_bShowFogMap), EBTYPE_BOOL_PROP));
	AddProperty(new EBProperty("Enable Fog", OBJECT_MEMBER(m_bEnableFog), EBTYPE_BOOL_PROP));
	AddProperty(new EBProperty("Wireframe", OBJECT_MEMBER(m_bWireframe), EBTYPE_BOOL_PROP));

	// Vertex shaders
	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Texture Coordinates from Distances", GetFilePath("fog.nvv"), EBTYPE_STRING_PROP));

}

#define NUM_CUBE_VERTICES (4*6)
#define NUM_CUBE_INDICES  (6*6)
HRESULT CShaderFog::CreateCube(WorldBoxVertex* pVertices, WORD* pIndices)
{
    // Set up the vertices for the cube. Note: to prevent tiling problems,
    // the u/v coords are knocked slightly inwards.

    // Front face
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f, 1.0f,-1.0f), D3DXVECTOR2(0.0f, 0.0f));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f, 1.0f,-1.0f), D3DXVECTOR2(1.0f, 0.0f ));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f,-1.0f,-1.0f), D3DXVECTOR2(1.0f, 1.0f ));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f,-1.0f,-1.0f), D3DXVECTOR2(0.0f, 1.0f ));

    // Back face
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f ));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f,-1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f ));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f,-1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f ));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f ));

    // Top face
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f ));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f ));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f, 1.0f,-1.0f), D3DXVECTOR2(0.0f, 0.0f ));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f, 1.0f,-1.0f), D3DXVECTOR2(1.0f, 0.0f ));

    // Bottom face
	*pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f,-1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f ));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f,-1.0f,-1.0f), D3DXVECTOR2(1.0f, 1.0f ));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f,-1.0f,-1.0f), D3DXVECTOR2(0.0f, 1.0f ));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f,-1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f ));

    // Right face
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f, 1.0f,-1.0f), D3DXVECTOR2(0.0f, 0.0f ));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f ));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f,-1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f ));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3( 1.0f,-1.0f,-1.0f), D3DXVECTOR2(0.0f, 1.0f ));

    // Left face
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f, 1.0f,-1.0f), D3DXVECTOR2(1.0f, 0.0f ));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f,-1.0f,-1.0f), D3DXVECTOR2(1.0f, 1.0f ));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f,-1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f ));
    *pVertices++ = WorldBoxVertex(D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f ));

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

    return S_OK;
}

HRESULT CShaderFog::Initialize(IDirect3DDevice8* pDev)
{
	HRESULT hr;
	vector<DWORD> Declaration;

	m_pD3DDev = pDev;
	pDev->AddRef();

	m_pNVDevice = new NVFogDevice(pDev, this);

	// Clamp the viewer to the inside of the room.
	D3DXVECTOR3 vecMinExtents = D3DXVECTOR3((-m_vecRoomDimensions.x * 0.5f), (-m_vecRoomDimensions.y * 0.5f), (-m_vecRoomDimensions.z * 0.5f));
	D3DXVECTOR3 vecMaxExtents = D3DXVECTOR3((m_vecRoomDimensions.x * 0.5f), (m_vecRoomDimensions.y * 0.5f), (m_vecRoomDimensions.z * 0.5f));

#define WALL_OFFSET 1.6f
	vecMaxExtents -= D3DXVECTOR3(WALL_OFFSET, WALL_OFFSET - .8f, WALL_OFFSET);
	vecMinExtents +=  D3DXVECTOR3(WALL_OFFSET, WALL_OFFSET - .8f, WALL_OFFSET);

  	// Projection doesn't change
	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/2.5f, 1.0f, 0.3f, 35.0f );
	m_pCamera = new NVWalkCamera(matProj, CameraStart, 0.0f, 0.0f, vecMinExtents, vecMaxExtents);

	DWORD dwVBFlags = D3DUSAGE_WRITEONLY;

	Declaration.clear();
	Declaration.push_back(D3DVSD_STREAM(0));
	Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(1, D3DVSDT_FLOAT2));
	Declaration.push_back(D3DVSD_END());

	m_dwWorldShader = 0;
	hr = LoadAndCreateShader(GetFilePath("fog_world.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwWorldShader);
	if (FAILED(hr))
		return hr;

	Declaration.clear();
	Declaration.push_back(D3DVSD_STREAM(0));
	Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(1, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(2, D3DVSDT_FLOAT2));
	Declaration.push_back(D3DVSD_END());

	m_dwFogShader = 0;
	hr = LoadAndCreateShader(GetFilePath("fog.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwFogShader);
	if (FAILED(hr))
		return hr;

	// Setup constants
	m_pD3DDev->SetVertexShaderConstant(CV_ZERO, D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_ONE, D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), 1);

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

	hr = m_pWorldBoxIndices->Lock(0, sizeof(WORD) * NUM_CUBE_INDICES,(BYTE**)&pIndices, 0);
	if (FAILED(hr))
	{
		m_strLastError = "Could not lock vertex buffer!";
		return hr;
	}
	CreateCube(pVertices, pIndices);

	
	hr = D3DXCreateTextureFromFileEx(m_pD3DDev, 
		GetFilePath("sunol_back_mipmap.dds").c_str(),
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		0,
		0,
		D3DFMT_UNKNOWN,
		D3DPOOL_MANAGED,
		D3DX_FILTER_LINEAR,
		D3DX_FILTER_LINEAR,
		0,
		NULL,
		NULL,
		&m_pWorldTextures[D3DCUBEMAP_FACE_NEGATIVE_Z]);
	if (FAILED(hr))
	{
		m_strLastError = "Could not create sunol_back_mipmap.dds";
		return hr;
	}
	hr = D3DXCreateTextureFromFileEx(m_pD3DDev, 
		GetFilePath("sunol_front_mipmap.dds").c_str(),
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		0,
		0,
		D3DFMT_UNKNOWN,
		D3DPOOL_MANAGED,
		D3DX_FILTER_LINEAR,
		D3DX_FILTER_LINEAR,
		0,
		NULL,
		NULL,
		&m_pWorldTextures[D3DCUBEMAP_FACE_POSITIVE_Z]);
	if (FAILED(hr))
	{
		m_strLastError = "Could not create sunol_front_mipmap.dds";
		return hr;
	}
	hr = D3DXCreateTextureFromFileEx(m_pD3DDev, 
		GetFilePath("sunol_bottom_mipmap.dds").c_str(),
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		0,
		0,
		D3DFMT_UNKNOWN,
		D3DPOOL_MANAGED,
		D3DX_FILTER_LINEAR,
		D3DX_FILTER_LINEAR,
		0,
		NULL,
		NULL,
		&m_pWorldTextures[D3DCUBEMAP_FACE_POSITIVE_Y]);
	if (FAILED(hr))
	{
		m_strLastError = "Could not create sunol_bottom_mipmap.dds";
		return hr;
	}
	hr = D3DXCreateTextureFromFileEx(m_pD3DDev, 
		GetFilePath("sunol_top_mipmap.dds").c_str(),
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		0,
		0,
		D3DFMT_UNKNOWN,
		D3DPOOL_MANAGED,
		D3DX_FILTER_LINEAR,
		D3DX_FILTER_LINEAR,
		0,
		NULL,
		NULL,
		&m_pWorldTextures[D3DCUBEMAP_FACE_NEGATIVE_Y]);
	if (FAILED(hr))
	{
		m_strLastError = "Could not create sunol_top_mipmap.dds";
		return hr;
	}
	hr = D3DXCreateTextureFromFileEx(m_pD3DDev, 
		GetFilePath("sunol_right_mipmap.dds").c_str(),
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		0,
		0,
		D3DFMT_UNKNOWN,
		D3DPOOL_MANAGED,
		D3DX_FILTER_LINEAR,
		D3DX_FILTER_LINEAR,
		0,
		NULL,
		NULL,
		&m_pWorldTextures[D3DCUBEMAP_FACE_POSITIVE_X]);
	if (FAILED(hr))
	{
		m_strLastError = "Could not create sunol_right_mipmap.dds";
		return hr;
	}
	hr = D3DXCreateTextureFromFileEx(m_pD3DDev, 
		GetFilePath("sunol_left_mipmap.dds").c_str(),
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		0,
		0,
		D3DFMT_UNKNOWN,
		D3DPOOL_MANAGED,
		D3DX_FILTER_LINEAR,
		D3DX_FILTER_LINEAR,
		0,
		NULL,
		NULL,
		&m_pWorldTextures[D3DCUBEMAP_FACE_NEGATIVE_X]);
	if (FAILED(hr))
	{
		m_strLastError = "Could not create sunol_left_mipmap.dds";
		return hr;
	}

	m_pWorldBoxVertices->Unlock();
	m_pWorldBoxIndices->Unlock();

	hr = m_pD3DDev->CreateTexture(m_dwFogMapWidth, m_dwFogMapHeight, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pFogMap);
	if (FAILED(hr))
		return hr;

	// Create a big square for rendering the fogmap
    if( FAILED( m_pD3DDev->CreateVertexBuffer( 4*sizeof(TLVertex), D3DUSAGE_WRITEONLY, D3DFVF_TLVERTEX, 
                                                  D3DPOOL_MANAGED, &m_pFogMapVB ) ) )
        return E_FAIL;
	// Create a quad for the final pass
    TLVertex* v;
    FLOAT sx = 100.0f;
    FLOAT sy = 100.0f;
    m_pFogMapVB->Lock( 0, 0, (BYTE**)&v, 0 );

    v[0].Position = D3DXVECTOR4(0, sy, 0, 1);
	v[0].Diffuse = 0x0;
	v[0].Specular = 0;
	v[0].Texture = D3DXVECTOR2(1, 1);

    v[1].Position = D3DXVECTOR4(0, 0, 0, 1);
	v[1].Diffuse = 0x0;
	v[1].Specular = 0;
	v[1].Texture = D3DXVECTOR2(1, 0);
    
	v[2].Position = D3DXVECTOR4(sx, sy, 0, 1);
	v[2].Diffuse = 0x0;
	v[2].Specular = 0;
	v[2].Texture = D3DXVECTOR2(0, 1);

	v[3].Position = D3DXVECTOR4(sx, 0, 0, 1);
	v[3].Diffuse = 0x0;
	v[3].Specular = 0;
	v[3].Texture = D3DXVECTOR2(0, 0);

    m_pFogMapVB->Unlock();

	 // Load a ground object
    m_pRoom = new NVFile();
    if( FAILED( m_pRoom->Create(m_pNVDevice, GetFilePath("room.x") ) ) )
	{
		m_strLastError = "Could not load room.x";
        return E_FAIL;
	}

	ZeroMemory(&m_RoomBounds, sizeof(NVBounds));
	m_pRoom->GetBoundingInfo(&m_RoomBounds);

    m_pChair = new NVFile();
    if( FAILED( m_pChair->Create(m_pNVDevice, GetFilePath("chair.x") ) ) )
	{
		m_strLastError = "Could not load chair.x";
        return E_FAIL;
	}
	ZeroMemory(&m_ChairBounds, sizeof(NVBounds));
	m_pChair->GetBoundingInfo(&m_ChairBounds);

    m_pTable = new NVFile();
    if( FAILED( m_pTable->Create(m_pNVDevice, GetFilePath("table.x") ) ) )
	{
		m_strLastError = "Could not load table.x";
        return E_FAIL;
	}
	ZeroMemory(&m_TableBounds, sizeof(NVBounds));
	m_pTable->GetBoundingInfo(&m_TableBounds);


	m_pChair->SetTexture(m_pFogMap);
	m_pTable->SetTexture(m_pFogMap);


	m_pNVDevice->SetProjectionTransform(&m_pCamera->GetProjectionMatrix());

	for (DWORD i = 0; i < NUM_CHAIRS; i++)
	{
		m_vecChairs[i] = D3DXVECTOR4((2.5f * sin((i / (float)NUM_CHAIRS) * (D3DX_PI * 2.0f))), 
									-(m_vecRoomDimensions.y * 0.5f) + (m_vecChairDimensions.y * 0.5f),
									(2.5f * cos((i / (float)NUM_CHAIRS) * (D3DX_PI * 2.0f))) ,
									((i / (float)NUM_CHAIRS) * (D3DX_PI * 2.0f)) );

	}


	ZeroMemory(&m_bKey[0], sizeof(bool) * MAX_VKEY);

	D3DXVECTOR4 vecFogScale = m_vecRoomDimensions;
	
	// Height scale and shift
	vecFogScale.y = 1.0f / (vecFogScale.y);
	vecFogScale.x = (m_vecRoomDimensions.y * 0.5f);

	// Distance scale
	vecFogScale.z = (1.0f / (sqrt((vecFogScale.x * vecFogScale.x) + (vecFogScale.z * vecFogScale.z))));

	m_pD3DDev->SetVertexShaderConstant(CV_FOG_SCALE, &vecFogScale.x, 1);

	m_pRoom->SetFVF(m_pNVDevice, FOGVERTEX_FVF);
	m_pTable->SetFVF(m_pNVDevice, FOGVERTEX_FVF);
	m_pChair->SetFVF(m_pNVDevice, FOGVERTEX_FVF);

	m_pRoom->RestoreDeviceObjects(m_pNVDevice);
	m_pTable->RestoreDeviceObjects(m_pNVDevice);
	m_pChair->RestoreDeviceObjects(m_pNVDevice);
	return S_OK;

}

HRESULT CShaderFog::BuildFogMap()
{
	LPDIRECT3DTEXTURE8 pTexture;
	m_pFogMap->QueryInterface(IID_IDirect3DTexture8, (void**)&pTexture);

	D3DLOCKED_RECT LockedRect;
	pTexture->LockRect(0, &LockedRect, NULL, 0);

	const float fHeightScale = m_vecRoomDimensions.y;
	const float fRangeScale = sqrt((m_vecRoomDimensions.x * m_vecRoomDimensions.x) + 
							(m_vecRoomDimensions.z * m_vecRoomDimensions.z));

	D3DXVECTOR3 CameraPosition = m_pCamera->GetCameraPosition();
	const float fEyeHeight = (CameraPosition.y + (m_vecRoomDimensions.y * 0.5f));
	const float fFogBottom = 0.0f;
	const float fFogTop = ((m_vecChairDimensions.y) + fFogBottom);
	const float fFogHeight = (fFogTop - fFogBottom);

	float fDeltaD;
	float fDeltaY;
	DWORD dwColor = 0xFFFFFF;
	float fDensityIntegral;
	float fVertexHeight;
	float fVertexRange;
	float fFog;

	for (int y = 0; y < m_dwFogMapHeight; y++)
	{
		fVertexHeight = ((float)y / (float)m_dwFogMapHeight) * fHeightScale;
		for (int x = 0; x < m_dwFogMapWidth; x++)
		{
			fVertexRange = ((float)x / (float)m_dwFogMapWidth) * fRangeScale;
			fDeltaD = fVertexRange;

			// fVertexHeight = height above ground of vertex
			// fVertexRange = distance from viewer to vertex
			if (fEyeHeight > fFogTop)
			{
				if (fVertexHeight < fFogTop)
				{
					fDeltaY = fFogTop - fVertexHeight;
					fDensityIntegral = (fDeltaY * fDeltaY * 0.5f);
					dwColor = 0xFF;
				}
				else
				{
					fDeltaY = 0.0f;
					fDensityIntegral = 0.0f;
				}
			}
			else
			{
				if (fVertexHeight < fFogTop)
				{
					float fDeltaA = fFogTop - fEyeHeight;
					float fDeltaB = fFogTop - fVertexHeight;
					fDeltaY = fabs(fDeltaA - fDeltaB);
					fDensityIntegral = fabs((fDeltaA * fDeltaA * 0.5f) - (fDeltaB * fDeltaB * 0.5f));
					dwColor = 0xFF00;
				}
				else
				{
					fDeltaY = fabs(fFogTop - fEyeHeight);
					fDensityIntegral = fabs(fDeltaY * fDeltaY * 0.5f);
					dwColor = 0xFF0000;
				}
			}
			dwColor = 0xFFFFFF;

			assert(fDeltaD >= 0.0f);
			assert(fDeltaY >= 0.0f);
			assert(fDensityIntegral >= 0.0f);

			fDensityIntegral*= 0.25f;
			
			if (fDeltaY != 0.0f)
			{
				fFog = (sqrt(1.0f + ((fDeltaD / fDeltaY) * (fDeltaD / fDeltaY)))) * fDensityIntegral;
			}
			else
			{
				fFog = 0.0f;//fDeltaD * fEyeHeight;
			}
			fFog = 1.0f - exp(-fFog);
		
			DWORD dwOutput = dwColor | (((DWORD)((fFog) * (float)0xFF)) << 24);
			*(((DWORD*)LockedRect.pBits) + (y * m_dwFogMapHeight) + x) = m_bEnableFog ? dwOutput : 0;
		}
	}

	pTexture->UnlockRect(0);
	SAFE_RELEASE(pTexture);

	return S_OK;
}

HRESULT CShaderFog::DrawFogMap()
{
    // Draw the background
    m_pD3DDev->SetTexture(0, m_pFogMap );
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);

    m_pD3DDev->SetRenderState(D3DRS_ZENABLE, FALSE);
    m_pD3DDev->SetStreamSource(0, m_pFogMapVB, sizeof(TLVertex));
    m_pD3DDev->SetVertexShader(D3DFVF_TLVERTEX);

    m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2 );

    // Restore render states
    m_pD3DDev->SetRenderState(D3DRS_ZENABLE, TRUE);

	return S_OK;
}

HRESULT CShaderFog::Free()
{
	for (int i = 0; i < 6; i++)
	{
		SAFE_RELEASE(m_pWorldTextures[i]);
	}
	SAFE_RELEASE(m_pWorldBoxVertices);
	SAFE_RELEASE(m_pWorldBoxIndices);
	SAFE_RELEASE(m_pFogMapVB);
	SAFE_RELEASE(m_pFogMap);
	SAFE_DELETE(m_pRoom);
	SAFE_DELETE(m_pChair);
	SAFE_DELETE(m_pTable);
	SAFE_DELETE(m_pNVDevice);
	SAFE_DELETE(m_pCamera);
	if (m_pD3DDev)
	{
		m_pD3DDev->DeleteVertexShader(m_dwWorldShader);
		m_pD3DDev->DeleteVertexShader(m_dwFogShader);
		SAFE_RELEASE(m_pD3DDev);
	}
	
	return S_OK;
}

HRESULT CShaderFog::Start()
{


	return S_OK;
}

HRESULT CShaderFog::SetTransform()
{
	D3DXMATRIX matTemp;
	D3DXMATRIX matWorldViewProj;
	D3DXMATRIX matWorldView;
	D3DXMATRIX matWorldViewIT;

	D3DXMATRIX matWorld = m_pNVDevice->GetWorldTransform();
	D3DXMATRIX matView = m_pNVDevice->GetViewTransform();
	D3DXMATRIX matProj = m_pNVDevice->GetProjectionTransform();

	D3DXMatrixMultiply(&matTemp, &matWorld, &matView);
	D3DXMatrixMultiply(&matWorldViewProj, &matTemp, &matProj);
	D3DXMatrixMultiply(&matWorldView, &matWorld, &matView);
	
	// Create a 3x3 invertse of the worldview for the normal transformation (we transpose it as we load it into
	// the constant store)
	D3DXMatrixInverse(&matWorldViewIT, NULL, &matWorldView);
		
	// Projection to clip space
	D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matWorldViewProj(0, 0), 4);
	D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);

	// Transform to eye space
	D3DXMatrixTranspose(&matWorld, &matWorld);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLD_0, &matWorld(0, 0), 4);
	D3DXMatrixTranspose(&matWorld, &matWorld);

	// Transform to eye space
	D3DXMatrixTranspose(&matWorldView, &matWorldView);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEW_0, &matWorldView(0, 0), 4);
	D3DXMatrixTranspose(&matWorldView, &matWorldView);

	// Transform from eye to projection
	D3DXMatrixTranspose(&matProj, &matProj);
	m_pD3DDev->SetVertexShaderConstant(CV_PROJ_0, &matProj(0, 0), 4);
	D3DXMatrixTranspose(&matProj, &matProj);

	// Transform for normals
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWIT_0, &matWorldViewIT(0, 0), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWIT_1, &matWorldViewIT(1, 0), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWIT_2, &matWorldViewIT(2, 0), 1);

	// Sometimes we don't need the shader - just draw with the current world/view/projection matrices
	m_pD3DDev->SetTransform(D3DTS_WORLD, &matWorld);
	m_pD3DDev->SetTransform(D3DTS_VIEW, &matView);
	m_pD3DDev->SetTransform(D3DTS_PROJECTION, &matProj);


	return S_OK;

}

HRESULT CShaderFog::DrawRoom()
{
	D3DXMATRIX matTemp;
	D3DXMATRIX matWorld;

	D3DXVECTOR4 vLightAmbient(0.5f, 0.5f, 0.5f, 1.0f);
	D3DXVECTOR4 vLightDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
	m_pD3DDev->SetVertexShaderConstant(CV_LIGHT1_AMBIENT, &vLightAmbient.x, 1);
	m_pD3DDev->SetVertexShaderConstant(CV_LIGHT1_DIFFUSE, &vLightDiffuse.x, 1);

	// Draw the room
	// Translate to center and scale
	D3DXMatrixTranslation(&matWorld, -m_RoomBounds.m_vecCenter.x, -m_RoomBounds.m_vecCenter.y, -m_RoomBounds.m_vecCenter.z);
	
	D3DXVECTOR3 vecScale = m_RoomBounds.m_vecMaxExtents - m_RoomBounds.m_vecMinExtents;
	vecScale.x = (1.0f / (float)fabs(vecScale.x)) * (m_vecRoomDimensions.x + 1.0f);
	vecScale.y = (1.0f / (float)fabs(vecScale.y)) * (m_vecRoomDimensions.y + 1.0f);
	vecScale.z = (1.0f / (float)fabs(vecScale.z)) * (m_vecRoomDimensions.z + 1.0f);
	
	D3DXMatrixScaling(&matTemp, vecScale.x, vecScale.y, vecScale.z);
	matWorld = matWorld * matTemp;

	m_pNVDevice->SetWorldTransform(&matWorld);
	SetTransform();

	m_pD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);

	m_dwCurrentShader = m_dwFogShader;
	m_pD3DDev->SetVertexShader(m_dwFogShader);

	// Set room state.  Want to apply fog in second pass
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
			
	m_pD3DDev->SetTexture(1, m_pFogMap);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_BLENDTEXTUREALPHA);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
	m_pD3DDev->SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_NONE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
	
	// Draw the room
	m_pRoom->Render(m_pNVDevice);

	// Add the chairs & table
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
	m_pD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	vLightAmbient = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
	m_pD3DDev->SetVertexShaderConstant(CV_LIGHT1_AMBIENT, &vLightAmbient.x, 1);

	// First, the chairs
	D3DXMATRIX matTranslate1;
	D3DXMATRIX matScale;
	D3DXMATRIX matRotate;
	D3DXMATRIX matTranslate2;
	
	D3DXMatrixTranslation(&matTranslate1, -m_ChairBounds.m_vecCenter.x, 
		-m_ChairBounds.m_vecCenter.y,
		-m_ChairBounds.m_vecCenter.z);

	vecScale = m_ChairBounds.m_vecMaxExtents - m_ChairBounds.m_vecMinExtents;
	vecScale.x = (1.0f / (float)fabs(vecScale.x)) * m_vecChairDimensions.x;
	vecScale.y = (1.0f / (float)fabs(vecScale.y)) * m_vecChairDimensions.y;
	vecScale.z = (1.0f / (float)fabs(vecScale.z)) * m_vecChairDimensions.z;
	D3DXMatrixScaling(&matScale, vecScale.x, vecScale.y, vecScale.z);

	for (int i = 0; i < NUM_CHAIRS; i++)
	{
		D3DXMatrixRotationY(&matRotate, m_vecChairs[i].w);

		D3DXMatrixTranslation(&matTranslate2, m_vecChairs[i].x, m_vecChairs[i].y, m_vecChairs[i].z);
		matWorld = matTranslate1 * matRotate * matScale * matTranslate2;

		m_pNVDevice->SetWorldTransform(&matWorld);
		SetTransform();

		// Draw the chair
		m_pChair->Render(m_pNVDevice);
	}	

	// Second, the table
	D3DXMatrixTranslation(&matTranslate1, -m_TableBounds.m_vecCenter.x, 
		-m_TableBounds.m_vecCenter.y,
		-m_TableBounds.m_vecCenter.z);

	vecScale = m_TableBounds.m_vecMaxExtents - m_TableBounds.m_vecMinExtents;
	vecScale.x = (1.0f / (float)fabs(vecScale.x)) * m_vecTableDimensions.x;
	vecScale.y = (1.0f / (float)fabs(vecScale.y)) * m_vecTableDimensions.y;
	vecScale.z = (1.0f / (float)fabs(vecScale.z)) * m_vecTableDimensions.z;
	D3DXMatrixScaling(&matScale, vecScale.x, vecScale.y, vecScale.z);
	D3DXMatrixTranslation(&matTranslate2, 0.0f, -(m_vecRoomDimensions.y * 0.5f) + (m_vecTableDimensions.y * 0.5f), 0.0f);

	matWorld = matTranslate1 * matScale * matTranslate2;
	m_pNVDevice->SetWorldTransform(&matWorld);
	SetTransform();

	m_pTable->Render(m_pNVDevice);


    return S_OK;
}

HRESULT CShaderFog::DrawWorldBox()
{
	D3DXVECTOR3 vecScale;
	D3DXMATRIX matWorld;

	// Scale the world box and put it outside the world
	vecScale.x = (m_vecRoomDimensions.x * 0.5f) + 5.0f;
	vecScale.y = (m_vecRoomDimensions.y * 0.5f) + 5.0f;
	vecScale.z = (m_vecRoomDimensions.z * 0.5f) + 5.0f;
	D3DXMatrixScaling(&matWorld, vecScale.x, vecScale.y, vecScale.z);

	m_pNVDevice->SetWorldTransform(&matWorld);
	SetTransform();
	
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

	m_pD3DDev->SetTexture(1, m_pFogMap);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_BLENDTEXTUREALPHA);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
	m_pD3DDev->SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_NONE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);

	m_pD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    
	m_pD3DDev->SetStreamSource(0, m_pWorldBoxVertices, sizeof(WorldBoxVertex));
	m_pD3DDev->SetIndices(m_pWorldBoxIndices, 0);
	m_pD3DDev->SetVertexShader(m_dwWorldShader);

	m_pD3DDev->SetTexture(0, m_pWorldTextures[D3DCUBEMAP_FACE_NEGATIVE_Z]);
	m_pD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, NUM_CUBE_VERTICES, 0, 2);

	m_pD3DDev->SetTexture(0, m_pWorldTextures[D3DCUBEMAP_FACE_POSITIVE_Z]);
	m_pD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, NUM_CUBE_VERTICES, 6, 2);
	
	m_pD3DDev->SetTexture(0, m_pWorldTextures[D3DCUBEMAP_FACE_NEGATIVE_Y]);
	m_pD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, NUM_CUBE_VERTICES, 12, 2);

	m_pD3DDev->SetTexture(0, m_pWorldTextures[D3DCUBEMAP_FACE_POSITIVE_Y]);
	m_pD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, NUM_CUBE_VERTICES, 18, 2);

	m_pD3DDev->SetTexture(0, m_pWorldTextures[D3DCUBEMAP_FACE_NEGATIVE_X]);
	m_pD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, NUM_CUBE_VERTICES, 24, 2);

	m_pD3DDev->SetTexture(0, m_pWorldTextures[D3DCUBEMAP_FACE_POSITIVE_X]);
	m_pD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, NUM_CUBE_VERTICES, 30, 2);
		
	m_pD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	
	return S_OK;
}

HRESULT CShaderFog::Tick(EBTimer* pTimer)
{
	HRESULT hr = S_OK;

	hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0xFF, 0x0, 0x0 ), 1.0, 0);
	
	m_pD3DDev->SetRenderState(D3DRS_FILLMODE, m_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID);

	m_pCamera->CheckKeyboard();
	m_pNVDevice->SetViewTransform(&m_pCamera->GetCameraMatrix());

	// Build the fog map.  Only needs to be done when the eye changes height
	BuildFogMap();

	// Create a directional light
	D3DXVECTOR3 vLightToEye;
	D3DXVECTOR3 vLight(1.0f, -1.0f, 0.35f);
	D3DXVECTOR4 vLightEye;

	// Transform direction vector into eye space
	D3DXVec3Normalize(&vLightToEye, &vLight);
	D3DXVec3TransformNormal(&vLightToEye, &vLightToEye, &m_pCamera->GetCameraMatrix());
	D3DXVec3Normalize(&vLightToEye, &vLightToEye);

	// Shader math requires that the vector is to the light
	vLightEye.x = -vLightToEye.x;
	vLightEye.y = -vLightToEye.y;
	vLightEye.z = -vLightToEye.z;
	vLightEye.w = 1.0f;
	
	m_pD3DDev->SetVertexShaderConstant(CV_LIGHT1_DIRECTION, &vLightEye.x, 1);

	D3DXVECTOR3 CameraPosition = m_pCamera->GetCameraPosition();

	m_pD3DDev->SetVertexShaderConstant(CV_WORLD_EYE, D3DXVECTOR4(CameraPosition.x, CameraPosition.y, CameraPosition.z, 1.0f), 1);

	// Set the current transform
	SetTransform();

	// Draw the room and the chairs/table
	DrawRoom();

	// Draw the 'outside' world
	DrawWorldBox();

	// Show the fog map
	if (m_bShowFogMap)
		DrawFogMap();

	return hr;
}

// Overrides for mesh renderer
HRESULT NVFogDevice::SetVertexShader(DWORD dwVertexShader)
{
	if (m_pFogShader->m_dwCurrentShader != (DWORD)-1)
		dwVertexShader = m_pFogShader->m_dwCurrentShader;

	return m_pD3DDev->SetVertexShader(dwVertexShader);
}

HRESULT NVFogDevice::SetMaterial(D3DMATERIAL8* pMat)
{
	m_pD3DDev->SetVertexShaderConstant(CV_LIGHT1_DIFFUSE, D3DXVECTOR4(pMat->Diffuse.r, pMat->Diffuse.g, pMat->Diffuse.b, pMat->Diffuse.a), 1);

	return S_OK;
}

// Device API's
void NVFogDevice::SetWorldTransform(const D3DXMATRIX* pValue) 
{
	NVDevice::SetWorldTransform(pValue);
	m_pFogShader->SetTransform();
}

void CShaderFog::Keyboard(DWORD vKey, UINT nFlags, bool bDown)
{
	eEBKeyAction Action = TranslateEffectKey(vKey, nFlags, bDown);
	
	static const float fTranslateMagnitude = 0.1f;
    switch ( Action )
    {
		case EB_HELP:
		{
			::MessageBoxEx( NULL, " Help : F1 - Help \n\n Home - Reset To Defaults \n\n W - Wireframe Toggle \n\n Space\\Pause - Toggle Pause/Resume \n\n Left Button & Mouse - Rotate Object\n\n Shift Left Button & Mouse - Pan Camera \n\n Ctrl Left Button & Mouse - Move Camera In & Out\n\n",
				   "Help", MB_ICONINFORMATION | MB_TASKMODAL, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ) );
		}
		break;

		case EB_WIREFRAME:
        {
            m_bWireframe = !m_bWireframe;
			m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
            
        }
		break;

		case EB_RESET:
        {
            m_pCamera->Reset();
            m_bWireframe = false;
			m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
            
        }
		break;

        default :
            break;
    }
}

void CShaderFog::MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y)
{
	if(button == MOUSE_LEFTBUTTON)
	{
		if(bDown)
		{
			m_pCamera->OnLButtonDown(hWnd, x, y);
		}
		else
		{
			m_pCamera->OnLButtonUp(hWnd, x, y);
		}
	}
	return;
}

void CShaderFog::MouseMove(HWND hWnd, int x, int y)
{
	m_pCamera->OnMouseMove(hWnd, x, y);
	return;
}

// ------------------------------------------------------------------------------
// CShaderSineWave::ConfirmDevice
//
// Description: Performs caps-bit checking to make sure the selected device 
//      supports this demo. 
// ------------------------------------------------------------------------------ 
HRESULT CShaderFog::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
{
	return S_OK;
}
