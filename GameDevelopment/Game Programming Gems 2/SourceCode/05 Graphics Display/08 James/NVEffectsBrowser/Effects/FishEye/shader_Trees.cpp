/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/

#include "eb_effect.h"
#include "nvmesh.h"
#include "nvdevice.h"
#include "shader_Trees.h"
#include "trees.h"
#include <algorithm>

using namespace nv_objects;
using namespace std;


DECLARE_EFFECT_MAIN()

extern "C"
{

__declspec(dllexport) unsigned int GetNumEffects() { return 1; }

__declspec(dllexport) EBEffect* CreateEffect(unsigned int EffectNum)
{
	return new CShaderFishEye();
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


// Function to compare two trees relative to the eye.
class CompareToEye
{
private:
	D3DXVECTOR3 m_EyePoint;

public:
	CompareToEye(D3DXVECTOR3 EyePoint)
		: m_EyePoint(EyePoint)
	{}

	bool operator()(const Tree& lhs, const Tree& rhs) const
	{
		D3DXVECTOR3 lPos = lhs.vPos;
		lPos.y = 0.0f;

		D3DXVECTOR3 rPos = rhs.vPos;
		rPos.y = 0.0f;

		D3DXVECTOR3 vEye = m_EyePoint;
		vEye.y = 0.0f;

		lPos -= vEye;
		rPos -= vEye;

		return D3DXVec3LengthSq(&lPos) < D3DXVec3LengthSq(&rPos);
	}
};

CShaderTrees::CShaderTrees(TreeType Type)
      : mbWireFrame(false),
        m_TreeType(Type),
	    m_pTerrain(NULL),
		m_pTreeTexture(NULL),
		m_pFishViewVB(NULL),
		m_pTreeVB(NULL),
		m_pNVDevice(NULL),
		m_pFishViewTexture(NULL),
		m_dwTerrainShader(0),
		m_dwTreeShader(0),
		m_bPause(false),
		m_fDuration(0.0f)
{
    ;
}

CShaderTrees::~CShaderTrees()
{
	Free();	
}

// Simple function to define "hilliness" for terrain
float CShaderTrees::HeightField( FLOAT x, FLOAT y )
{
    return 9*(cosf(x/20+0.2f)*cosf(y/15-0.2f)+1.0f);
}

HRESULT CShaderTrees::Initialize(IDirect3DDevice8* pDev)
{
	HRESULT hr;
	vector<DWORD> Declaration;
	D3DXMATRIX matWorld;
	D3DXMATRIX matView;
	D3DXMATRIX matProj;

	m_pD3DDev = pDev;
	pDev->AddRef();

	m_pNVDevice = new NVTreeDevice(pDev, this);

	DWORD dwVBFlags = D3DUSAGE_WRITEONLY;

	// Create the shader
	Declaration.clear();
	Declaration.push_back(D3DVSD_STREAM(0));
	Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(1, D3DVSDT_D3DCOLOR));
	Declaration.push_back(D3DVSD_REG(2, D3DVSDT_FLOAT2));
	Declaration.push_back(D3DVSD_END());
	hr = LoadAndCreateShader(GetFilePath("FishEye_Tree.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwTreeShader);
	if (FAILED(hr))
		return hr;

	Declaration.clear();
	Declaration.push_back(D3DVSD_STREAM(0));
	Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(1, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(2, D3DVSDT_FLOAT2));
	Declaration.push_back(D3DVSD_END());
	hr = LoadAndCreateShader(GetFilePath("FishEye_Terrain.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwTerrainShader);
	if (FAILED(hr))
		return hr;

	// Create the fisheye view
	hr = D3DXCreateTextureFromFileEx(m_pD3DDev, 
		GetFilePath("FishView.dds").c_str(),
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		1,
		0,
		D3DFMT_UNKNOWN,
		D3DPOOL_MANAGED,
		D3DX_FILTER_LINEAR,
		D3DX_FILTER_LINEAR,
		0,
		NULL,
		NULL,
		&m_pFishViewTexture);
	if (FAILED(hr))
	{
		m_strLastError = "Could not load texture FishView.dds";
		return hr;
	}
	hr = D3DXCreateTextureFromFileEx(m_pD3DDev, 
		GetFilePath("tree0.dds").c_str(),
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
		&m_pTreeTexture);
	if (FAILED(hr))
	{
		m_strLastError = "Could not load texture tree0.dds";
		return hr;
	}

    // Load a ground object
    m_pTerrain = new NVMesh();
    if( FAILED( m_pTerrain->Create(m_pNVDevice, GetFilePath("seafloor.x") ) ) )
	{
		m_strLastError = "Could not load seafloor.x";
        return E_FAIL;
	}
	DWORD dwFVF = m_pTerrain->GetSysMemMesh()->GetFVF();
	m_pTerrain->SetFVF(m_pNVDevice, D3DFVF_TEX1 | D3DFVF_NORMAL | D3DFVF_XYZ);

    // Add some "hilliness" to the terrain
    LPDIRECT3DVERTEXBUFFER8 pVB;
    if( SUCCEEDED( m_pTerrain->GetSysMemMesh()->GetVertexBuffer( &pVB ) ) )
    {
        Vertex* pVertices;
        DWORD   dwNumVertices = m_pTerrain->GetSysMemMesh()->GetNumVertices();

        pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 );

        for( DWORD i=0; i<dwNumVertices; i++ )
            pVertices[i].Position.y = HeightField( pVertices[i].Position.x, pVertices[i].Position.z );

        pVB->Unlock();
        pVB->Release();
    }

	m_vecTrees.resize(NUM_TREES);
    // Initialize the tree data
    for( WORD i=0; i<NUM_TREES; i++ )
    {
        // Position the trees randomly
        FLOAT fTheta  = 2.0f*D3DX_PI*(FLOAT)rand()/RAND_MAX;
        FLOAT fRadius = 25.0f + 55.0f * (FLOAT)rand()/RAND_MAX;
        m_vecTrees[i].vPos.x  = fRadius * sinf(fTheta);
        m_vecTrees[i].vPos.z  = fRadius * cosf(fTheta);
        m_vecTrees[i].vPos.y  = HeightField( m_vecTrees[i].vPos.x, m_vecTrees[i].vPos.z );

        // Size the trees randomly
        FLOAT fWidth  = 1.0f + 0.2f * (FLOAT)(rand()-rand())/RAND_MAX;
        FLOAT fHeight = 1.4f + 0.4f * (FLOAT)(rand()-rand())/RAND_MAX;

        // Each tree is a random color between red and green
        DWORD r = (255-190) + (DWORD)(190*(FLOAT)(rand())/RAND_MAX);
        DWORD g = (255-190) + (DWORD)(190*(FLOAT)(rand())/RAND_MAX);
        DWORD b = 0;
        DWORD dwColor = 0xff000000 + (r<<16) + (g<<8) + (b<<0);

        m_vecTrees[i].v[0].p     = D3DXVECTOR3(-fWidth, 0*fHeight, 0.0f );
        m_vecTrees[i].v[0].color = dwColor;
        m_vecTrees[i].v[0].tu    = 0.0f;   m_vecTrees[i].v[0].tv = 1.0f;
        m_vecTrees[i].v[1].p     = D3DXVECTOR3(-fWidth, 2*fHeight, 0.0f  );
        m_vecTrees[i].v[1].color = dwColor;
        m_vecTrees[i].v[1].tu    = 0.0f;   m_vecTrees[i].v[1].tv = 0.0f;
        m_vecTrees[i].v[2].p     = D3DXVECTOR3( fWidth, 0*fHeight, 0.0f  );
        m_vecTrees[i].v[2].color = dwColor;
        m_vecTrees[i].v[2].tu    = 1.0f;   m_vecTrees[i].v[2].tv = 1.0f;
        m_vecTrees[i].v[3].p     = D3DXVECTOR3( fWidth, 2*fHeight, 0.0f  );
        m_vecTrees[i].v[3].color = dwColor;
        m_vecTrees[i].v[3].tu    = 1.0f;   m_vecTrees[i].v[3].tv = 0.0f;
    }

    // Check the alpha test caps of the device. (The alpha test offers a 
    // performance boost to not render pixels less than some alpha threshold.)
    //m_bUseAlphaTest = ( m_d3dCaps.TriCaps.dwAlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL );
        
    // Create a big square for rendering the fish view mask
    if( FAILED( m_pD3DDev->CreateVertexBuffer( 4*sizeof(TLVertex),
                                                  D3DUSAGE_WRITEONLY, D3DFVF_TLVERTEX, 
                                                  D3DPOOL_MANAGED, &m_pFishViewVB ) ) )
        return E_FAIL;

    // Create a square for rendering each tree.  We modify, but only write
    if( FAILED( m_pD3DDev->CreateVertexBuffer( NUM_TREES * 4*sizeof(TREEVERTEX),
                                                  D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TREEVERTEX,
                                                  D3DPOOL_DEFAULT, &m_pTreeVB ) ) )
        return E_FAIL;


	IDirect3DSurface8* pBackBuffer;
	m_pD3DDev->GetRenderTarget(&pBackBuffer);
	D3DSURFACE_DESC ddsd;

	pBackBuffer->GetDesc(&ddsd);

	m_dwWidth = ddsd.Width;
	m_dwHeight = ddsd.Height;
	SAFE_RELEASE(pBackBuffer);

	// Create a quad for the final pass
    TLVertex* v;
    FLOAT sx = (FLOAT)m_dwWidth;
    FLOAT sy = (FLOAT)m_dwHeight / 2;
    m_pFishViewVB->Lock( 0, 0, (BYTE**)&v, 0 );

    v[0].Position = D3DXVECTOR4(0, sy, 0, 1);
	v[0].Diffuse = 0xffffffff;
	v[0].Specular = 0;
	v[0].Texture = D3DXVECTOR2(1, 1);

    v[1].Position = D3DXVECTOR4(0, 0, 0, 1);
	v[1].Diffuse = 0xffffffff;
	v[1].Specular = 0;
	v[1].Texture = D3DXVECTOR2(1, 0);
    
	v[2].Position = D3DXVECTOR4(sx, sy, 0, 1);
	v[2].Diffuse = 0xffffffff;
	v[2].Specular = 0;
	v[2].Texture = D3DXVECTOR2(0, 1);

	v[3].Position = D3DXVECTOR4(sx, 0, 0, 1);
	v[3].Diffuse = 0xffffffff;
	v[3].Specular = 0;
	v[3].Texture = D3DXVECTOR2(0, 0);

    m_pFishViewVB->Unlock();

   	// Projection doesn't change
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 0.3f, 100.0f );
	m_pNVDevice->SetProjectionTransform(&matProj);


	// Load values for the distortion shader
	float fXZRatio       = 2.5f;
	float fkDistortion   = 2.0f;
	float fkp1Distortion = fkDistortion + 1.0f;
	m_pD3DDev->SetVertexShaderConstant(CV_FISHEYEINFO, D3DXVECTOR4(fXZRatio, fkDistortion, 0.0f, 1.0f), 1);
	m_pD3DDev->SetVertexShaderConstant(CV_CONSTANTS,   D3DXVECTOR4(fkp1Distortion, 1.0f, 0.0f, 0.0f), 1);

	m_pTerrain->RestoreDeviceObjects(m_pNVDevice);

	return S_OK;

}

HRESULT CShaderTrees::Free()
{
	SAFE_DELETE(m_pNVDevice);
	SAFE_DELETE(m_pTerrain);

    SAFE_RELEASE(m_pFishViewVB )
    SAFE_RELEASE(m_pTreeVB )
    
    SAFE_RELEASE(m_pTreeTexture);
	SAFE_RELEASE(m_pFishViewTexture);

	if (m_pD3DDev)
	{
		m_pD3DDev->DeleteVertexShader(m_dwTreeShader);
		m_pD3DDev->DeleteVertexShader(m_dwTerrainShader);
		SAFE_RELEASE(m_pD3DDev);
	}
	
	return S_OK;
}

HRESULT CShaderTrees::Start()
{

	return S_OK;
}

  
HRESULT CShaderTrees::SetTransform()
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

HRESULT CShaderTrees::DrawFishViewport()
{
    // Draw the background
    m_pD3DDev->SetTexture(0, m_pFishViewTexture);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

    m_pD3DDev->SetRenderState(D3DRS_ZENABLE, FALSE);
    m_pD3DDev->SetStreamSource(0, m_pFishViewVB, sizeof(TLVertex));
    m_pD3DDev->SetVertexShader(D3DFVF_TLVERTEX);

    // Enable alpha testing to overdraw the outer pixels
    m_pD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pD3DDev->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_INVSRCALPHA);
    m_pD3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCALPHA);

    m_pD3DDev->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
    m_pD3DDev->SetRenderState( D3DRS_ALPHAREF,        0x80);
    m_pD3DDev->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_LESS);

    m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2 );

    // Restore render states
    m_pD3DDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pD3DDev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

HRESULT CShaderTrees::DrawTrees(bool bDistort)
{
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);

	m_pNVDevice->SetWorldTransform(&matWorld);
	SetTransform();

    // Set state for drawing trees
    m_pD3DDev->SetTexture( 0, m_pTreeTexture);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

    // Set diffuse blending for alpha set in vertices. 
    m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
    m_pD3DDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    m_pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

    // Enable alpha testing (skips pixels with less than a certain alpha.)
    m_pD3DDev->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
    m_pD3DDev->SetRenderState( D3DRS_ALPHAREF,        0x80);
    m_pD3DDev->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	if (bDistort)
	{
		m_pD3DDev->SetVertexShader(m_dwTreeShader);
	}
	else
	{
		m_pD3DDev->SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	}

    // Copy tree mesh into vertexbuffer
    TREEVERTEX* v;
    m_pTreeVB->Lock( 0, 0, (BYTE**)&v, D3DLOCK_DISCARD );
    for( DWORD i=0; i<NUM_TREES; i++ )
    {
        memcpy( v, m_vecTrees[i].v, 4*sizeof(TREEVERTEX) );
        v += 4;
    }
    m_pTreeVB->Unlock();
    m_pD3DDev->SetStreamSource( 0, m_pTreeVB, sizeof(TREEVERTEX) );

    // Loop through and render all trees    
    for( i=0; i<NUM_TREES; i++ )
    {
        // Translate the billboard into place
        m_matBillboardMatrix._41 = m_vecTrees[i].vPos.x;
        m_matBillboardMatrix._42 = m_vecTrees[i].vPos.y;
        m_matBillboardMatrix._43 = m_vecTrees[i].vPos.z;

		// Load values
		m_pNVDevice->SetWorldTransform(&m_matBillboardMatrix);
		SetTransform();

        // Render the billboard
        m_pD3DDev->DrawPrimitive( D3DPT_TRIANGLESTRIP, i*4, 2 );
    }

    m_pD3DDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_pD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    return S_OK;
}

HRESULT CShaderTrees::DrawTerrain(bool bDistort)
{
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	m_pNVDevice->SetWorldTransform(&matWorld);
	SetTransform();

	// Set terrain state
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	// Draw the terrain
	if (bDistort)
	{
		m_pTerrain->SetVertexShader(m_dwTerrainShader);
	}
	else
	{
		m_pTerrain->SetVertexShader(0);
	}

	// Call the terrain object to do the work
	m_pTerrain->Render(m_pNVDevice);

    return S_OK;
}

HRESULT CShaderTrees::Tick(EBTimer* pTimer)
{
	HRESULT hr = S_OK;
	D3DXMATRIX matTemp;
	D3DXMATRIX matWorld;
	D3DXMATRIX matView;
	D3DVIEWPORT8 Viewport;
	D3DVIEWPORT8 NewViewport;

	D3DXVECTOR3 vEyePt;
	D3DXVECTOR3 vLookatPt;
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );

    m_pD3DDev->SetRenderState(D3DRS_FILLMODE, (mbWireFrame) ? D3DFILL_WIREFRAME : D3DFILL_SOLID);
	hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0xAA, 0xAA, 0xAA ), 1.0, 0);

    float const     kSpeed = 20.0f;
	
	if (!m_bPause)
	{
		m_fDuration = pTimer->GetDuration();
	}

    // Move the camera about a large circle through the trees
    vEyePt.x = 30.0f*cosf( ((m_fDuration+0.0f)/kSpeed) * 2 * D3DX_PI );
    vEyePt.z = 30.0f*sinf( ((m_fDuration+0.0f)/kSpeed) * 2 * D3DX_PI );
    vEyePt.y = 5.5f + HeightField( vEyePt.x, vEyePt.z );

    vLookatPt.x = 30.0f*cosf( ((m_fDuration+0.5f)/kSpeed) * 2 * D3DX_PI );
    vLookatPt.z = 30.0f*sinf( ((m_fDuration+0.5f)/kSpeed) * 2 * D3DX_PI );
    vLookatPt.y = 4.2f + HeightField(vEyePt.x, vEyePt.z);

	// Identity world transform
	D3DXMatrixIdentity(&matWorld);

    // Set the app view matrix for normal viewing
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );

    // Set up a rotation matrix to orient the billboard towards the camera.
    D3DXVECTOR3 vDir = vLookatPt - vEyePt;
    if( vDir.x > 0.0f )
        D3DXMatrixRotationY( &m_matBillboardMatrix, -atanf(vDir.z/vDir.x)+D3DX_PI/2 );
    else
        D3DXMatrixRotationY( &m_matBillboardMatrix, -atanf(vDir.z/vDir.x)-D3DX_PI/2 );

    // Sort trees in back-to-front order
	sort(m_vecTrees.begin(), m_vecTrees.end(), CompareToEye(vEyePt) );

	// Get the current viewport
	m_pD3DDev->GetViewport(&Viewport);
	memcpy(&NewViewport, &Viewport, sizeof(D3DVIEWPORT8));
	
	// Draw fish-eye pass
	// Setup the viewport to the top half
	NewViewport.Height = Viewport.Height / 2;
	m_pD3DDev->SetViewport(&NewViewport);

	// Set the current transform
	m_pNVDevice->SetWorldTransform(&matWorld);
	m_pNVDevice->SetViewTransform(&matView);
	SetTransform();
	
	// Draw the terrain
	DrawTerrain(true);

	// Draw the trees
	DrawTrees(true);

	// Draw the fisheye view
	DrawFishViewport();

	// Draw the undistorted view of the world
	// Set viewport to be the bottom
	NewViewport.Y = (Viewport.Height / 2) + 2;
	NewViewport.Height -= 2;
	m_pD3DDev->SetViewport(&NewViewport);
	SetTransform();

	// Draw terrain
	DrawTerrain(false);

	// Draw the trees
	DrawTrees(false);

	// Restore the viewport
	m_pD3DDev->SetViewport(&Viewport);

	return hr;
}

void NVTreeDevice::SetWorldTransform(const D3DXMATRIX* pValue) 
{
	NVDevice::SetWorldTransform(pValue);
	m_pTreeShader->SetTransform();
}

HRESULT CShaderTrees::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
{
	return S_OK;
}

void CShaderTrees::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
{
	eEBKeyAction Action = TranslateEffectKey(dwKey, nFlags, bDown);
	
    switch ( Action )
    {
		case EB_HELP:
		{
			::MessageBoxEx( NULL, " Help : F1 - Help \n\n W - Wireframe Toggle \n\n P\\Pause - Pause animation\n\n",
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

		default:
		break;
    }
}
