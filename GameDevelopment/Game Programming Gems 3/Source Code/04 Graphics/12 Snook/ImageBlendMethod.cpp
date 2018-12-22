/****************************************************************************************\

	ImageBlendMethod.cpp

	Simple rendering method interface

	Part of the Gems III sample code library.
	Created 12/27/2001 by Greg Snook

    ------------------------------------------------------------------------------------- 
	Notes/Revisions:

\****************************************************************************************/
#include "ImageBlendMethod.h"
#include <tchar.h>
#include <stdio.h>
#include <assert.h>

struct CUSTOMVERTEX
{
    float x, y, z;
    DWORD color;
    float tu_0, tv_0;
    float tu_1, tv_1;
};

struct PIXEL
{ 
	char a,r,g,b;
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#define TOTAL_PLANES 1
#define TOTAL_VERTS (TOTAL_PLANES*4)
#define TOTAL_INDEXES (TOTAL_PLANES*6)

#define WIDTH 0.34375f

static CUSTOMVERTEX g_Vertices[TOTAL_VERTS] = 
{
    //  x      y     z     color       u0     v0   u1     v1
    { -WIDTH, -0.5f, 0.0f, 0xffffffff, 0.0f, 1.0f, 0.0f, 1.0f},
    { -WIDTH, +0.5f, 0.0f, 0xffffffff, 0.0f, 0.0f, 0.0f, 0.0f},
    { +WIDTH, +0.5f, 0.0f, 0xffffffff, 0.25f, 0.0f, 1.0f, 0.0f},
    { +WIDTH, -0.5f, 0.0f, 0xffffffff, 0.25f, 1.0f, 1.0f, 1.0f},
};

static DWORD g_dwDecl[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0,  D3DVSDT_FLOAT3),
    D3DVSD_REG(1,  D3DVSDT_D3DCOLOR ),
	D3DVSD_REG(2,  D3DVSDT_FLOAT2 ),
    D3DVSD_REG(3,  D3DVSDT_FLOAT2 ),
    D3DVSD_END()
};


HRESULT ImageBlendMethod::InitDeviceObjects( LPDIRECT3DDEVICE8 pd3dDevice )
{
    LPD3DXBUFFER pCode;
	m_pd3dDevice = pd3dDevice;

	// create the vertex buffer
	HRESULT hr;
	if (SUCCEEDED(hr = m_pd3dDevice->CreateVertexBuffer(sizeof(g_Vertices), D3DUSAGE_WRITEONLY, D3DFVF_CUSTOMVERTEX, D3DPOOL_MANAGED, &m_pVBuffer)))
	{
		CUSTOMVERTEX *pData;

		if (SUCCEEDED(hr = m_pVBuffer->Lock(0, sizeof(g_Vertices), (BYTE**)&pData, D3DLOCK_NOSYSLOCK)))
		{
			memcpy(pData, &g_Vertices[0], sizeof(g_Vertices));
			m_pVBuffer->Unlock();
		}
		else return (hr);
	}
	else return (hr);

    D3DXCreateTextureFromFile( m_pd3dDevice, "bunny_blend.tga", &m_pTexture);
    D3DXCreateTextureFromFile( m_pd3dDevice, "bunny_blend.tga", &m_pOffsetTexture);

	// get descriptions of our surfaces
	D3DSURFACE_DESC srcDesc;
	m_pTexture->GetLevelDesc(0, &srcDesc);

	m_pixelStep = 4.0f/(float)srcDesc.Width;


	// lock and fill the offset texture
	D3DSURFACE_DESC desc;
	D3DLOCKED_RECT rect;

	m_pOffsetTexture->GetLevelDesc(0, &desc);
	if (SUCCEEDED(m_pOffsetTexture->LockRect(0, &rect, 0, 0)))
	{
		PIXEL *pixel =  (PIXEL *)rect.pBits;

		for (unsigned int y=0;y<desc.Height; ++y)
		{
			char *rowStart=(char *)pixel;
			for (unsigned int x=0;x<desc.Width; ++x)
			{
				unsigned char value = x*4;//(x & ~3);
				float range = (float)value/(float)desc.Width;

				if (range>1.0f) range = 1.0f;

				value = (unsigned char)(range*128.0f);

				pixel->a = value;
				pixel->r = 0;
				pixel->g = 0;
				pixel->b = 0;
				pixel++;

			}
			pixel = (PIXEL *)(rowStart+rect.Pitch);
		}
		m_pOffsetTexture->UnlockRect(0);
	}



    // Assemble the vertex shader from the file
	if( FAILED( hr = D3DXAssembleShaderFromFile( "vertex_shader.txt", 0, NULL, &pCode, NULL )))
        return hr;

    // Create the vertex shader
    if( FAILED( hr = m_pd3dDevice->CreateVertexShader( g_dwDecl, (DWORD*)pCode->GetBufferPointer(), &m_hVertexShader, 0 )))
         return hr;

	pCode->Release();

    // Assemble the pixel shader from the file
	if( FAILED( hr = D3DXAssembleShaderFromFile( "pixel_shader.txt", 0, NULL, &pCode, NULL )))
        return hr;

    // Create the pixel shader
    if( FAILED( hr = m_pd3dDevice->CreatePixelShader((DWORD*)pCode->GetBufferPointer(), &m_hPixelShader)))
         return hr;

	pCode->Release();

	return(S_OK);
}

HRESULT ImageBlendMethod::RestoreDeviceObjects()
{
	HRESULT hr;
	CUSTOMVERTEX *pData;

	if (SUCCEEDED(hr = m_pVBuffer->Lock(0, sizeof(g_Vertices), (BYTE**)&pData, D3DLOCK_NOSYSLOCK)))
	{
		memcpy(pData, &g_Vertices[0], sizeof(g_Vertices));
		m_pVBuffer->Unlock();
	}
	else return (hr);

	return(S_OK);
}

HRESULT ImageBlendMethod::InvalidateDeviceObjects()
{
	return(S_OK);
}

HRESULT ImageBlendMethod::DeleteDeviceObjects()
{
	SAFE_RELEASE(m_pVBuffer);
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pOffsetTexture);

	if (m_hVertexShader) m_pd3dDevice->DeleteVertexShader(m_hVertexShader);
	if (m_hPixelShader) m_pd3dDevice->DeletePixelShader(m_hPixelShader);

	return(S_OK);
}

DWORD F2DW( FLOAT f ) { return *((DWORD*)&f); }

HRESULT ImageBlendMethod::Render(const D3DXMATRIX& world_matrix, const D3DXMATRIX& camera_matrix, const D3DXMATRIX& proj_matrix, bool wireframe)
{
	D3DXMATRIX view_matrix;

	D3DXMatrixMultiply(&view_matrix, &world_matrix, &camera_matrix);

	// setup the render states
	m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE );
	m_pd3dDevice->SetRenderState(D3DRS_LIGHTING , FALSE );

	if (wireframe)
	{
		m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	}
	else
	{
		m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		m_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, 0);
		m_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

		m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND , D3DBLEND_SRCALPHA);
		m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA);
	}

	m_pd3dDevice->SetTextureStageState(0, D3DTSS_BUMPENVMAT00, F2DW(1.0f));
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_BUMPENVMAT10, F2DW(0.0f));
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_BUMPENVMAT01, F2DW(0.0f));
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_BUMPENVMAT11, F2DW(0.0f));

	m_pd3dDevice->SetTextureStageState(1, D3DTSS_BUMPENVMAT00, F2DW(1.0f));
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_BUMPENVMAT10, F2DW(0.0f));
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_BUMPENVMAT01, F2DW(0.0f));
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_BUMPENVMAT11, F2DW(0.0f));

	m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER , D3DTEXF_POINT  );
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER , D3DTEXF_POINT  );
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER , D3DTEXF_NONE );

	m_pd3dDevice->SetTextureStageState(1, D3DTSS_MAGFILTER , D3DTEXF_LINEAR );
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_MINFILTER , D3DTEXF_LINEAR );
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_MIPFILTER , D3DTEXF_NONE );
	

    // Set the textures
    m_pd3dDevice->SetTexture( 0, m_pOffsetTexture );
    m_pd3dDevice->SetTexture( 1, m_pTexture );


    // Store render target surface desc
    LPDIRECT3DSURFACE8 pBackBuffer;
    D3DSURFACE_DESC   m_d3dsdBackBuffer;   // Surface desc of the backbuffer
    m_pd3dDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
    pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
    pBackBuffer->Release();

	// render the object
	if (SUCCEEDED(m_pd3dDevice->SetPixelShader(m_hPixelShader)))
	{
		if (SUCCEEDED(m_pd3dDevice->SetVertexShader(m_hVertexShader)))
		{
			if (SUCCEEDED(m_pd3dDevice->SetStreamSource(0,m_pVBuffer, sizeof(CUSTOMVERTEX))))
			{
				D3DXMATRIX temp;
				D3DXMatrixTranspose(&temp, &view_matrix);

				m_pd3dDevice->SetVertexShaderConstant( 0, (VOID*)&temp, 4 );

				D3DXMatrixTranspose(&temp, &proj_matrix);
				m_pd3dDevice->SetVertexShaderConstant( 4, (VOID*)&temp, 4 );

				D3DXCOLOR norm(0.0f,0.0f,0.0f,0.0f);
				m_pd3dDevice->SetVertexShaderConstant( 8, (VOID*)&norm, 1 );

				float screen_factor = 1.0f/(float)m_d3dsdBackBuffer.Width;

				float x_range = (D3DX_PI/12);

				D3DXVECTOR4 constants(m_pixelStep*2.0f, m_pixelStep*0.5f, 0.0f, m_pixelStep);
				m_pd3dDevice->SetVertexShaderConstant( 9, (VOID*)&constants, 1 );

				return(m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 ));
			}
		}
	}

    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetPixelShader(NULL);

	return(S_OK);
}

