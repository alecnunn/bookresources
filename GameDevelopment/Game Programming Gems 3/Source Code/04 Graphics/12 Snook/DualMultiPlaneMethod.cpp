/****************************************************************************************\

	DualMultiPlaneMethod.cpp

	Simple rendering method interface

	Part of the Gems III sample code library.
	Created 12/27/2001 by Greg Snook

    ------------------------------------------------------------------------------------- 
	Notes/Revisions:

\****************************************************************************************/
#include "DualMultiPlaneMethod.h"

struct CUSTOMVERTEX
{
    float x, y, z;
    DWORD color;
    float tu0, tv0;
};

static DWORD g_dwDecl[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0,  D3DVSDT_FLOAT3),
    D3DVSD_REG(1,  D3DVSDT_D3DCOLOR ),
    D3DVSD_REG(2,  D3DVSDT_FLOAT2 ),
    D3DVSD_END()
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#define TOTAL_PLANES 16
#define TOTAL_VERTS (TOTAL_PLANES*4)
#define TOTAL_INDEXES (TOTAL_PLANES*6)

static CUSTOMVERTEX g_Vertices[TOTAL_VERTS];
static CUSTOMVERTEX g_VertexSetB[TOTAL_VERTS];
static WORD			g_IndexValues[TOTAL_INDEXES];

#define UNIT			0.707106781f // the side of a right triangle whose hypotenuse is 1
#define DOUBLE_UNIT		(UNIT*2.0f) 
#define HALF_UNIT		(UNIT*0.5f) 

static CUSTOMVERTEX g_BaseVerts[4]=
{
    //  x      y     z     color      u     v    
    { -0.5f, -0.5f, 0.0f, 0xffffffff, 0.0f, 1.0f},
    { -0.5f, +0.5f, 0.0f, 0xffffffff, 0.0f, 0.0f},
    { +0.5f, +0.5f, 0.0f, 0xffffffff, 1.0f, 0.0f},
    { +0.5f, -0.5f, 0.0f, 0xffffffff, 1.0f, 1.0f},
};

HRESULT DualMultiPlaneMethod::InitDeviceObjects( LPDIRECT3DDEVICE8 pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	DWORD alpha = 0;
	DWORD alpha_step = 0xff000000 / TOTAL_PLANES;

	float z_value = 1.0f;
	float z_step = -1.0f/(float)TOTAL_PLANES;

	for (int plane=0;plane<TOTAL_PLANES;++plane)
	{
		WORD vert = plane*4;
		WORD index = plane*6;
		WORD i;

		// copy in the four basic vertices
		memcpy(&g_Vertices[vert], &g_BaseVerts[0], sizeof(CUSTOMVERTEX)*4);

		// setup the vertex data specific to this plane
		for (i=0;i<4;++i)
		{
			g_Vertices[vert+i].z = z_value;
			g_Vertices[vert+i].color = (alpha & 0xff000000) + 0xffffff;
		}

		// setup the index data for this plane
		g_IndexValues[index] = vert;
		g_IndexValues[index+1] = vert+1;
		g_IndexValues[index+2] = vert+2;
		g_IndexValues[index+3] = vert;
		g_IndexValues[index+4] = vert+2;
		g_IndexValues[index+5] = vert+3;

		z_value += z_step;
		alpha += alpha_step;
	}

	// create the vertex buffers
	HRESULT hr;
	if (SUCCEEDED(hr = m_pd3dDevice->CreateVertexBuffer(sizeof(g_Vertices), D3DUSAGE_WRITEONLY, D3DFVF_CUSTOMVERTEX, D3DPOOL_MANAGED, &m_pVBufferA)))
	{
		CUSTOMVERTEX *pData;

		if (SUCCEEDED(hr = m_pVBufferA->Lock(0, sizeof(g_Vertices), (BYTE**)&pData, D3DLOCK_NOSYSLOCK)))
		{
			memcpy(pData, &g_Vertices[0], sizeof(g_Vertices));
			m_pVBufferA->Unlock();
		}
		else return (hr);
	}
	else return (hr);

	// create the universal index buffer
	if (SUCCEEDED(hr = m_pd3dDevice->CreateIndexBuffer(sizeof(g_IndexValues), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16 , D3DPOOL_MANAGED, &m_pIBuffer)))
	{
		WORD* pIndex;
		if (SUCCEEDED(hr = m_pIBuffer->Lock(0, sizeof(g_IndexValues), (BYTE**)&pIndex, D3DLOCK_NOSYSLOCK)))
		{
			memcpy(pIndex, &g_IndexValues[0], sizeof(g_IndexValues));
			m_pIBuffer->Unlock();
		}
		else return (hr);
	}
	else return (hr);

    // Assemble the pixel shader from the file
    LPD3DXBUFFER pCode;
	if( FAILED( hr = D3DXAssembleShaderFromFile( "pixel_shader2.txt", 0, NULL, &pCode, NULL )))
        return hr;

    // Create the pixel shader
    if( FAILED( hr = m_pd3dDevice->CreatePixelShader((DWORD*)pCode->GetBufferPointer(), &m_hPixelShader)))
         return hr;

	pCode->Release();
    // Assemble the vertex shader from the file
	if( FAILED( hr = D3DXAssembleShaderFromFile( "vertex_shader2.txt", 0, NULL, &pCode, NULL )))
        return hr;

    // Create the vertex shader
    if( FAILED( hr = m_pd3dDevice->CreateVertexShader( g_dwDecl, (DWORD*)pCode->GetBufferPointer(), &m_hVertexShader, 0 )))
         return hr;

	pCode->Release();


    D3DXCreateTextureFromFile( m_pd3dDevice, "bunny_front.tga", &m_pTextureA);
    D3DXCreateTextureFromFile( m_pd3dDevice, "bunny_back.tga", &m_pTextureB);

	return(S_OK);
}

HRESULT DualMultiPlaneMethod::RestoreDeviceObjects()
{
	HRESULT hr;
	CUSTOMVERTEX *pData;

	if (SUCCEEDED(hr = m_pVBufferA->Lock(0, sizeof(g_Vertices), (BYTE**)&pData, D3DLOCK_NOSYSLOCK)))
	{
		memcpy(pData, &g_Vertices[0], sizeof(g_Vertices));
		m_pVBufferA->Unlock();
	}
	else return (hr);

	WORD* pIndex;
	if (SUCCEEDED(hr = m_pIBuffer->Lock(0, sizeof(g_IndexValues), (BYTE**)&pIndex, D3DLOCK_NOSYSLOCK)))
	{
		memcpy(pIndex, &g_IndexValues[0], sizeof(g_IndexValues));
		m_pIBuffer->Unlock();
	}
	else return (hr);

	return(S_OK);
}

HRESULT DualMultiPlaneMethod::InvalidateDeviceObjects()
{
	return(S_OK);
}

HRESULT DualMultiPlaneMethod::DeleteDeviceObjects()
{
	SAFE_RELEASE(m_pVBufferA);
	SAFE_RELEASE(m_pVBufferB);
	SAFE_RELEASE(m_pIBuffer);
	SAFE_RELEASE(m_pTextureA);
	SAFE_RELEASE(m_pTextureB);
	if (m_hPixelShader) m_pd3dDevice->DeletePixelShader(m_hPixelShader);
	if (m_hVertexShader) m_pd3dDevice->DeleteVertexShader(m_hVertexShader);
	return(S_OK);
}

HRESULT DualMultiPlaneMethod::Render(const D3DXMATRIX& world_matrix, const D3DXMATRIX& camera_matrix, const D3DXMATRIX& proj_matrix, bool wireframe)
{
	D3DXMATRIX view_matrix;

	D3DXMatrixMultiply(&view_matrix, &world_matrix, &camera_matrix);
	D3DXMatrixMultiply(&view_matrix, &view_matrix, &proj_matrix);

	// setup the render states
	m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE );
	m_pd3dDevice->SetRenderState(D3DRS_LIGHTING , FALSE );

	if (wireframe)
	{
		m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	}
	else
	{
		m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		m_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, 1);
		m_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

		m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND , D3DBLEND_ONE);
		m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND , D3DBLEND_ZERO);
	}

	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);


	// render the object
	if (SUCCEEDED(m_pd3dDevice->SetPixelShader(m_hPixelShader)))
	{
		if (SUCCEEDED(m_pd3dDevice->SetVertexShader(m_hVertexShader)))
		{
			D3DXMATRIX temp;
			D3DXVECTOR4 neg_half(-0.50f,-0.50f,-0.50f,-0.50f);

			D3DXMatrixTranspose(&temp, &view_matrix);

			m_pd3dDevice->SetVertexShaderConstant( 0, (VOID*)&temp, 4 );
			m_pd3dDevice->SetVertexShaderConstant( 4, (VOID*)&neg_half, 1 );

			if (SUCCEEDED(m_pd3dDevice->SetStreamSource(0,m_pVBufferA, sizeof(CUSTOMVERTEX))))
			{
				m_pd3dDevice->SetTexture( 0, m_pTextureA);
				m_pd3dDevice->SetTexture( 1, m_pTextureB);
				if (SUCCEEDED(m_pd3dDevice->SetIndices(m_pIBuffer, 0)))
				{
					m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, TOTAL_VERTS, 0, TOTAL_PLANES*2);
				}
			}
		}

		m_pd3dDevice->SetPixelShader(NULL);
	}

	return(S_OK);
}