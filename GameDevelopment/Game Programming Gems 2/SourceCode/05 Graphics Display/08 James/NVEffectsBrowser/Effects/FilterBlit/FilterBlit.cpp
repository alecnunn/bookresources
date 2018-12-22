/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/

#include <crtdbg.h>

#include "eb_effect.h"
#include "nvtex.h"
#include "nvmesh.h"
#include "nvdevice.h"
#include "FilterBlit.h"

#include "Constants.h"

using namespace nv_objects;
using namespace std;

DECLARE_EFFECT_MAIN()

extern "C"
{

__declspec(dllexport) unsigned int GetNumEffects() 
{ 
    return static_cast<unsigned int>(CFilterBlit::NUM_FILTER_OPTIONS); 
}

__declspec(dllexport) EBEffect* CreateEffect(unsigned int EffectNum)
{
	return new CFilterBlit(static_cast<CFilterBlit::eFilterOptions>(EffectNum));
}

}

CFilterBlit::CFilterBlit(eFilterOptions initDisplayOption)
	: meDisplayOption    (initDisplayOption)
    , meInitDisplayOption(initDisplayOption)
    , mbWireFrame        (false)
    , mpTextureToFilter  (NULL)
	, m_pNVDevice(NULL)
	, m_dwVertexShader(0)
    , m_dwLuminancePixelShader(0)
    , m_dwLuminanceDiagEdgePixelShader(0)
    , m_dwLuminanceSensitiveDiagEdgePixelShader(0)
    , m_dwBlurPixelShader   (0)
    , m_dwSharpenPixelShader(0)
	, m_pVertexBuffer(NULL)
	, mpBackbufferColor( 0 )
	, mpBackbufferDepth( 0 )
{
	for ( int i = 0; i < kMaxNumPasses; ++i )
	{
        mpTextureFiltered[i] = 0;
		mpFilterTarget   [i] = 0;
	}

	m_strEffectLocation     = "Pixel Shaders\\Image Processing";
	m_strEffectVertexShader = GetFilePath("FilterBlit.nvv");

    switch (meDisplayOption)
	{
		case CONE_FILTER:
            m_strEffectName        = "Filter Blitting (9-sample cone filter)";
			m_strEffectPixelShader = GetFilePath("BlurBlit.nvp");
			break;
		case BOX9_FILTER:
            m_strEffectName        = "Filter Blitting (9-sample box filter)";
			m_strEffectPixelShader = GetFilePath("BlurBlit.nvp");
			break;
		case BOX16_FILTER:
            m_strEffectName        = "Filter Blitting (16-sample box filter)";
			m_strEffectPixelShader = GetFilePath("BlurBlit.nvp");
			break;
		case SHARPEN_FILTER:
            m_strEffectName        = "Filter Blitting (sharpening filter)";
			m_strEffectPixelShader = GetFilePath("SharpenBlit.nvp");
            break;
		case LUMINANCE_EDGE:
            m_strEffectName        = "Filter Blitting (luminance edge detection)";
			m_strEffectPixelShader = GetFilePath("LuminanceDiagEdgeBlit.nvp");
			break;
		default:
            m_strEffectName        = "Filter Blitting (unknown)";
			m_strEffectPixelShader = GetFilePath("BlurBlit.nvp");
		    break;
    }
}

CFilterBlit::~CFilterBlit()
{
	Free();	
}

HRESULT CFilterBlit::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
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


	return S_OK;
}

void CFilterBlit::UpdateProperties()
{
	EBEffect::UpdateProperties();

    AddProperty(new EBProperty("Wireframe", OBJECT_MEMBER(mbWireFrame), EBTYPE_BOOL_PROP));

	EBEnumProperty* pEnumProp = new EBEnumProperty("Display Options", OBJECT_MEMBER(meDisplayOption), EBTYPE_DWORD_PROP, PROPFLAG_CALLBACK | PROPFLAG_PUBLIC);
	
	pEnumProp->AddEnumerant(new EBEnumValue(pEnumProp, "Show  9-sample Cone Filtering", (DWORD)CONE_FILTER, EBTYPE_DWORD_PROP));
	pEnumProp->AddEnumerant(new EBEnumValue(pEnumProp, "Show  9-sample Box  Filtering", (DWORD)BOX9_FILTER, EBTYPE_DWORD_PROP));
	pEnumProp->AddEnumerant(new EBEnumValue(pEnumProp, "Show 16-sample Box  Filtering", (DWORD)BOX16_FILTER, EBTYPE_DWORD_PROP));
	pEnumProp->AddEnumerant(new EBEnumValue(pEnumProp, "Show Sharpening Filter",        (DWORD)SHARPEN_FILTER, EBTYPE_DWORD_PROP));
	pEnumProp->AddEnumerant(new EBEnumValue(pEnumProp, "Show Luminance Edge Detection", (DWORD)LUMINANCE_EDGE, EBTYPE_DWORD_PROP));
	AddProperty(pEnumProp);

	// Vertex shaders
	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Filter Blit", GetFilePath("FilterBlit.nvv"), EBTYPE_STRING_PROP));

	// Pixel shaders
	m_pPixelShaderEnum->AddEnumerant(new EBEnumValue(m_pPixelShaderEnum, "Blur Blit", GetFilePath("BlurBlit.nvp"), EBTYPE_STRING_PROP));
	m_pPixelShaderEnum->AddEnumerant(new EBEnumValue(m_pPixelShaderEnum, "Luminance Blit", GetFilePath("LuminanceBlit.nvp"), EBTYPE_STRING_PROP));
	m_pPixelShaderEnum->AddEnumerant(new EBEnumValue(m_pPixelShaderEnum, "Luminance Edge Blit", GetFilePath("LuminanceDiagEdgeBlit.nvp"), EBTYPE_STRING_PROP));
	m_pPixelShaderEnum->AddEnumerant(new EBEnumValue(m_pPixelShaderEnum, "SharpenBlit Blit", GetFilePath("SharpenBlit.nvp"), EBTYPE_STRING_PROP));

}

HRESULT CFilterBlit::Initialize(IDirect3DDevice8* pDev)
{
    int     i;
	HRESULT hr;

	// get the device
	m_pD3DDev = pDev;
	pDev->AddRef();

	m_pNVDevice = new NVDevice(pDev);

	// create the vertex and pixel shaders
	vector<DWORD> Declaration;
	Declaration.push_back(D3DVSD_STREAM(0));
	Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_REG(1, D3DVSDT_FLOAT2));
	Declaration.push_back(D3DVSD_END());

	m_dwVertexShader = 0;
	hr = LoadAndCreateShader(GetFilePath("FilterBlit.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &m_dwVertexShader);
	if (FAILED(hr))
		return hr;
	
	hr = LoadAndCreateShader(GetFilePath("LuminanceBlit.pso"), NULL, 0, SHADERTYPE_PIXEL, &m_dwLuminancePixelShader);
	if (FAILED(hr))
		return hr;
	hr = LoadAndCreateShader(GetFilePath("LuminanceDiagEdgeBlit.pso"), NULL, 0, SHADERTYPE_PIXEL, &m_dwLuminanceDiagEdgePixelShader);
	if (FAILED(hr))
		return hr;
	hr = LoadAndCreateShader(GetFilePath("LuminanceSensitiveDiagEdgeBlit.pso"), NULL, 0, SHADERTYPE_PIXEL, &m_dwLuminanceSensitiveDiagEdgePixelShader);
	if (FAILED(hr))
		return hr;

    hr = LoadAndCreateShader(GetFilePath("BlurBlit.pso"), NULL, 0, SHADERTYPE_PIXEL, &(m_dwBlurPixelShader));
	if (FAILED(hr))
		return hr;
	hr = LoadAndCreateShader(GetFilePath("SharpenBlit.pso"), NULL, 0, SHADERTYPE_PIXEL, &(m_dwSharpenPixelShader));
	if (FAILED(hr))
		return hr;

	// load the texture to filter
	mpTextureToFilter = new NVTexture();
    hr = mpTextureToFilter->CreateFromFile(m_pD3DDev, GetFilePath("MSLobby.dds"));
	if (FAILED(hr))
	{
		m_strLastError = "Could not load texture MSLobby.dds";
		return hr;
	}

	D3DSURFACE_DESC ddsd;
    mpTextureToFilter->GetTexture()->GetLevelDesc(0, &ddsd);

	// create vertex buffer 
	hr = m_pD3DDev->CreateVertexBuffer( 4 * sizeof(QuadVertex), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &m_pVertexBuffer);
	if (FAILED(hr))
		return hr;

	QuadVertex      *pBuff;

	if (m_pVertexBuffer)
	{
		hr = m_pVertexBuffer->Lock(0, 4 * sizeof(QuadVertex),(BYTE**)&pBuff, 0);
		if (FAILED(hr))
		{
			m_strLastError = "Couldn't lock buffer!";
			return hr;
		}

        for (i = 0; i < 4; ++i)
        {
            pBuff->Position = D3DXVECTOR3((i==0 || i==3) ? -1.0f : 1.0f,
                                          (i<2)          ? -1.0f : 1.0f,
                                          0.0f);
		    pBuff->Tex      = D3DXVECTOR2((i==0 || i==3) ? 0.0f : 1.0f, 
                                          (i<2)          ? 1.0f : 0.0f);
		    pBuff++;
        }
        m_pVertexBuffer->Unlock();
    }
    CreateAndWriteUVOffsets(ddsd.Width, ddsd.Height);

    // set up render state: disable all except texture stage 0 (see below)
    for (i = 0; i < 4; ++i)
    {
	    m_pD3DDev->SetTextureStageState(i, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	    m_pD3DDev->SetTextureStageState(i, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	    m_pD3DDev->SetTextureStageState(i, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

        m_pD3DDev->SetTextureStageState(i, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	    m_pD3DDev->SetTextureStageState(i, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	    m_pD3DDev->SetTextureStageState(i, D3DTSS_MIPFILTER, D3DTEXF_NONE);
	    m_pD3DDev->SetTextureStageState(i, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP);
	    m_pD3DDev->SetTextureStageState(i, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP);
    }
    m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);

    m_pD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pD3DDev->SetRenderState(D3DRS_ZENABLE, FALSE);

    return CreateTextureRenderTarget();
}

HRESULT CFilterBlit::Free()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_DELETE(m_pNVDevice);
	SAFE_DELETE(mpTextureToFilter);
	
	if (m_pD3DDev)
	{
		m_pD3DDev->DeleteVertexShader(m_dwVertexShader);
		m_pD3DDev->DeletePixelShader(m_dwLuminancePixelShader);
		m_pD3DDev->DeletePixelShader(m_dwLuminanceDiagEdgePixelShader);
		m_pD3DDev->DeletePixelShader(m_dwLuminanceSensitiveDiagEdgePixelShader);

		m_pD3DDev->DeletePixelShader(m_dwBlurPixelShader);
		m_pD3DDev->DeletePixelShader(m_dwSharpenPixelShader);

        for (int i = 0; i < kMaxNumPasses; ++i)
        {
            
            SAFE_RELEASE(mpTextureFiltered[i]);
            SAFE_RELEASE(mpFilterTarget[i]);
        }

        SAFE_RELEASE(mpBackbufferColor);
		SAFE_RELEASE(mpBackbufferDepth);

        SAFE_RELEASE(m_pD3DDev);
	}
	
	return S_OK;
}

HRESULT CFilterBlit::Start()
{
	return S_OK;
}

// Our properties were updated, set the correct shader to match
void CFilterBlit::PropertyUpdateCallback(const EBProperty* pProperty, bool bWritten)
{
	// If somebody changed the filter we are doing, set the dirty flag and change the 
    // pixel shader enum to reflect this.
	if (pProperty->IsKindOf(EBTYPE_ENUM_PROP))
	{
		if (bWritten)
		{
			if (pProperty->GetOffset() == OBJECT_MEMBER(m_strEffectPixelShader))
			{
				if (m_strEffectPixelShader.compare(GetFilePath("BlurBlit.nvp")) == 0)
				{
					meDisplayOption = CONE_FILTER;
				}
				else if (m_strEffectPixelShader.compare(GetFilePath("SharpenBlit.nvp")) == 0)
				{
					meDisplayOption = SHARPEN_FILTER;
				}
				else if (   (m_strEffectPixelShader.compare(GetFilePath("LuminanceDiagEdgeBlit.nvp")) == 0)
						 || (m_strEffectPixelShader.compare(GetFilePath("LuminanceBlit.nvp")) == 0))
				{
					meDisplayOption = LUMINANCE_EDGE;
				}

				m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
			}
			else
			{
				switch (meDisplayOption)
				{
					case CONE_FILTER:
					case BOX9_FILTER:
					case BOX16_FILTER:
						m_strEffectPixelShader = GetFilePath("BlurBlit.nvp");
						break;
					case SHARPEN_FILTER:
						m_strEffectPixelShader = GetFilePath("SharpenBlit.nvp");
							break;
					case LUMINANCE_EDGE:
						m_strEffectPixelShader = GetFilePath("LuminanceDiagEdgeBlit.nvp");
						break;
					default:
					;
				}

				m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PIXELSHADERS;
			}
		}		
	}
}

HRESULT CFilterBlit::Tick(EBTimer* pTimer)
{
	HRESULT hr;

    // set a bunch of render-state for rendering the world
    m_pD3DDev->SetRenderState(D3DRS_FILLMODE, (mbWireFrame) ? D3DFILL_WIREFRAME : D3DFILL_SOLID);

	D3DXMATRIX matWorld;
	D3DXMATRIX matView;
	D3DXMATRIX matProj;
	D3DXMATRIX matViewProj;
	D3DXMATRIX matWorldViewProj;

    // write to constant memory which uv-offsets to use
    D3DXVECTOR4     offset(0.0f, 0.0f, 0.0f, 0.0f);

	switch (meDisplayOption)
    {
        case BOX9_FILTER:
            offset.x = 4.0f;
            break;
        case SHARPEN_FILTER:
            offset.x = 3.0f;
            break;
        case BOX16_FILTER:
            offset.x = 2.0f;
            break;
        default:
            offset.x = 1.0f;
            break;
    }
    m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);

	// Disable culling
	m_pD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    // set render state 
	m_pD3DDev->SetVertexShader(m_dwVertexShader);
	m_pD3DDev->SetStreamSource(0, m_pVertexBuffer, sizeof(QuadVertex));


	D3DXVECTOR3 const vEyePt    = D3DXVECTOR3( 0.0f, 0.0f, -5.0f );
	D3DXVECTOR3 const vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 const vUp       = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

	// Set World, View, Projection, and combination matrices.
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUp);
	D3DXMatrixOrthoLH(&matProj, 4.0f, 4.0f, 0.2f, 20.0f);

    D3DXMatrixMultiply(&matViewProj, &matView, &matProj);

    // draw a single quad to texture:
    // the quad covers the whole "screen" exactly
	D3DXMatrixScaling(&matWorld, 2.0f, 2.0f, 1.0f);
	D3DXMatrixMultiply(&matWorldViewProj, &matWorld, &matViewProj);
    D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matWorldViewProj(0, 0), 4);

    // turn on our special filtering pixel shader
    if (meDisplayOption != LUMINANCE_EDGE)
        m_pD3DDev->SetPixelShader( (meDisplayOption == SHARPEN_FILTER) ? 
                                      m_dwSharpenPixelShader
                                    : m_dwBlurPixelShader);	
    
    // draw multiple passes        
    for (int i = 0; i < kMaxNumPasses; ++i)
    {
        hr = m_pD3DDev->SetRenderTarget(mpFilterTarget[i], NULL);
	    hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0xFF, 0x0, 0x0 ), 1.0, 0);

        if (meDisplayOption == LUMINANCE_EDGE)
        {
            switch (i)
            {
                case 0:
                    m_pD3DDev->SetPixelShader(m_dwLuminancePixelShader);	
                    m_pD3DDev->SetTexture(0, mpTextureToFilter->GetTexture());
                    m_pD3DDev->SetTexture(1, mpTextureToFilter->GetTexture());
                    m_pD3DDev->SetTexture(2, mpTextureToFilter->GetTexture());
                    m_pD3DDev->SetTexture(3, mpTextureToFilter->GetTexture());
                    break;
                case 1:
                    m_pD3DDev->SetPixelShader(m_dwLuminanceSensitiveDiagEdgePixelShader);	
                    break;
                case 2:
                    m_pD3DDev->SetPixelShader(m_dwLuminanceDiagEdgePixelShader);	
                    break;
                default:
                    break;
            }
        }
        else
        {
            switch (i)
            {
                case 0:
                    m_pD3DDev->SetTexture(0, mpTextureToFilter->GetTexture());
                    m_pD3DDev->SetTexture(1, mpTextureToFilter->GetTexture());
                    m_pD3DDev->SetTexture(2, mpTextureToFilter->GetTexture());
                    m_pD3DDev->SetTexture(3, mpTextureToFilter->GetTexture());
                    break;
                default:
                    m_pD3DDev->SetTexture(0, mpTextureFiltered[i-1]);
                    m_pD3DDev->SetTexture(1, mpTextureFiltered[i-1]);
                    m_pD3DDev->SetTexture(2, mpTextureFiltered[i-1]);
                    m_pD3DDev->SetTexture(3, mpTextureFiltered[i-1]);
                    break;
            }
        }
        
        // draw the fan with displaced texture coordinates
        hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
    }

    // then switch back to normal rendering 
    hr = m_pD3DDev->SetRenderTarget(mpBackbufferColor, mpBackbufferDepth);
	hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0xAA, 0xAA, 0xAA ), 1.0, 0);

    // turn off pixel shading
    m_pD3DDev->SetPixelShader(0);	

    // draw quad in upper left corner: original texture
	D3DXMatrixTranslation(&matWorld, -1.0f, 1.0f, 0.0f);
	D3DXMatrixMultiply(&matWorldViewProj, &matWorld, &matViewProj);
    D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
	m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matWorldViewProj(0, 0), 4);

    // reset offsets to 0 (ie no offsets)
    offset.x = 0.0f;
    m_pD3DDev->SetVertexShaderConstant(CV_UV_OFFSET_TO_USE, &offset, 1);

    m_pD3DDev->SetTexture(0, mpTextureToFilter->GetTexture());
	hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

    // draw quads in the other corners, use generated textures
    for (int j = 0; j < 3; ++j)
    {
        D3DXMatrixTranslation(&matWorld, (j == 2) ? -1.0f :  1.0f, 
                                         (j == 0) ?  1.0f : -1.0,
                                         0.0f);
	    D3DXMatrixMultiply(&matWorldViewProj, &matWorld, &matViewProj);
        D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);
	    m_pD3DDev->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &matWorldViewProj(0, 0), 4);

        m_pD3DDev->SetTexture(0, mpTextureFiltered[j]);
	    hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
    }
	return hr;
}

HRESULT CFilterBlit::CreateTextureRenderTarget()
{
    HRESULT         hr;

    // get a pointer to the current back-buffer (so we can restore it later)
	m_pD3DDev->GetRenderTarget(        &mpBackbufferColor );
	m_pD3DDev->GetDepthStencilSurface( &mpBackbufferDepth );
	_ASSERT( mpBackbufferColor != NULL );
	_ASSERT( mpBackbufferDepth != NULL );

    // get the description for the texture we want to filter
	D3DSURFACE_DESC ddsd;
    mpTextureToFilter->GetTexture()->GetLevelDesc(0, &ddsd);

    // create new textures just like the current texture
    for (int i = 0; i < kMaxNumPasses; ++i)
    {
        hr = m_pD3DDev->CreateTexture(ddsd.Width, ddsd.Height, 1, 
                                      D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, 
                                      D3DPOOL_DEFAULT, &mpTextureFiltered[i]);
        if (FAILED(hr))
        {
		    m_strLastError = "Can't CreateTexture!\n";
            _ASSERT(false);
            return E_FAIL;
        }

        hr = mpTextureFiltered[i]->GetSurfaceLevel(0, &mpFilterTarget[i]);
        if (FAILED(hr))
        {
		    m_strLastError = "Can't Get to top-level texture!\n";
            _ASSERT(false);
            return E_FAIL;
        }

        // set our render target to the new and shiny textures without depth
        hr = m_pD3DDev->SetRenderTarget(mpFilterTarget[i], NULL);
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

void CFilterBlit::CreateAndWriteUVOffsets(int width, int height)
{
	// displace texture-uvs so that the sample points on the 
    // texture describe 
    // i)   a square around the texel to sample.
    //      the edges of the square are distance s from the center texel.
    //      Due to bilinear filtering and application of equal weights (1/4) 
    //      in the pixel shader, the following filter is implemented for the 9 samples
    //          abc
    //          def
    //          ghi:
    //      filtered pixel = (s*s)/4 (a+c+g+i) + (s-s*s)/2 (b+d+f+h) + (1-s)^2 e
    //         Thus, choosing s = 0 means no filtering (also no offsets)
    //      s = 2/3 results in an equally weighted, 9-sample box-filter (and is called
    //      type4) and s = 1/2 results in a circular cone-filter (and is called type1).
    // ii) a square around the texel to sample, so as to include sixteen texels:
    //          abcd
    //          efgh
    //          ijkl
    //          mnop
    //      Center texel is assumed to be "j", and offsets are made so that the texels
    //      are the combinations of (a, b, e, f), (c, d, g, h), (i, j, m, n), and 
    //      (k, l, o, p)
    // iii) A quad-sample filter:
    //         a
    //         b
    //        cde
    //      Center texel is "b" and sampled dead center.  The second sample is 
    //      dead-center "a", and the last two samples are interpolations between
    //      (c,d) and (d,e).  Connecting the samples with the center pixel should 
    //      produce three lines that measure the same angle (120 deg) between them.
    //      This sampling pattern may be rotated around "b".

    // first the easy one: no offsets
    float const     noOffsetX[4] = { 0.0f, 0.0f, 0.0f, 0.0f}; 
    float const     noOffsetY[4] = { 0.0f, 0.0f, 0.0f, 0.0f};

    float const     kPerTexelWidth  = 1.0f/static_cast<float>(width);
    float const     kPerTexelHeight = 1.0f/static_cast<float>(height);
    float           s               = 0.5f;
    float const     eps             = 10.0e-4f;
    float const     rotAngle1       = D3DXToRadian( 0.0f );
    float const     rotAngle2       = rotAngle1 + D3DXToRadian(120.0f); 
    float const     rotAngle3       = rotAngle1 + D3DXToRadian(240.0f); 

    // Change filter kernel for 9-sample box filtering, but for edge-detection we are 
    // going to use interpolated texels.  Why?  Because we detect diagonal edges only
    // and the vertical and horizontal filtering seems to help.
        
    float const type1OffsetX[4] = { -s * kPerTexelWidth, 
                                    -s * kPerTexelWidth,  
                                     s * kPerTexelWidth,   
                                     s * kPerTexelWidth  };
    float const type1OffsetY[4] = { -s * kPerTexelHeight, 
                                     s * kPerTexelHeight, 
                                     s * kPerTexelHeight, 
                                    -s * kPerTexelHeight };

    // we have to bring the 16 texel-sample-filter a bit closer to the center to avoid 
    // separation due to floating point inaccuracies.
    float const type2OffsetX[4] = { -.5f * kPerTexelWidth + eps,  
                                    -.5f * kPerTexelWidth + eps, 
                                    1.5f * kPerTexelWidth - eps, 
                                    1.5f * kPerTexelWidth - eps };
    float const type2OffsetY[4] = { -.5f * kPerTexelHeight+ eps, 
                                    1.5f * kPerTexelHeight- eps, 
                                    1.5f * kPerTexelHeight- eps, 
                                    -.5f * kPerTexelHeight+ eps };

    float const type3OffsetX[4] = {0.0f,  sinf(rotAngle1)*kPerTexelWidth,  
                                          sinf(rotAngle2)*kPerTexelWidth,  
                                          sinf(rotAngle3)*kPerTexelWidth  };
    float const type3OffsetY[4] = {0.0f, -cosf(rotAngle1)*kPerTexelHeight, 
                                         -cosf(rotAngle2)*kPerTexelHeight, 
                                         -cosf(rotAngle3)*kPerTexelHeight };

    s = 2.0f/3.0f;      // same as type 1, except s is different
    float const type4OffsetX[4] = { -s * kPerTexelWidth, 
                                    -s * kPerTexelWidth,  
                                     s * kPerTexelWidth,   
                                     s * kPerTexelWidth  };
    float const type4OffsetY[4] = { -s * kPerTexelHeight, 
                                     s * kPerTexelHeight, 
                                     s * kPerTexelHeight, 
                                    -s * kPerTexelHeight };

    // write all these offsets to constant memory
    for (int i = 0; i < 4; ++i)
    {
        D3DXVECTOR4  noOffset(      noOffsetX[i],    noOffsetY[i], 0.0f, 0.0f);
        D3DXVECTOR4  type1Offset(type1OffsetX[i], type1OffsetY[i], 0.0f, 0.0f);
        D3DXVECTOR4  type2Offset(type2OffsetX[i], type2OffsetY[i], 0.0f, 0.0f);
        D3DXVECTOR4  type3Offset(type3OffsetX[i], type3OffsetY[i], 0.0f, 0.0f);
        D3DXVECTOR4  type4Offset(type4OffsetX[i], type4OffsetY[i], 0.0f, 0.0f);

    	m_pD3DDev->SetVertexShaderConstant(CV_UV_T0_NO_OFFSET + 5*i, &noOffset,    1);
    	m_pD3DDev->SetVertexShaderConstant(CV_UV_T0_TYPE1     + 5*i, &type1Offset, 1);
    	m_pD3DDev->SetVertexShaderConstant(CV_UV_T0_TYPE2     + 5*i, &type2Offset, 1);
    	m_pD3DDev->SetVertexShaderConstant(CV_UV_T0_TYPE3     + 5*i, &type3Offset, 1);
    	m_pD3DDev->SetVertexShaderConstant(CV_UV_T0_TYPE4     + 5*i, &type4Offset, 1);
    }
}

void CFilterBlit::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
{
    unsigned int option = static_cast<unsigned int>(meDisplayOption); 
	eEBKeyAction Action = TranslateEffectKey(dwKey, nFlags, bDown);
	
    switch ( Action )
    {
		case EB_HELP:
		{
			::MessageBoxEx( NULL, " Help : F1 - Help \n\n Home - Reset To Defaults \n\n W - Wireframe Toggle \n\n +/- - Change Filter Mode",
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
	        meDisplayOption = meInitDisplayOption;
            mbWireFrame = false;
            m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
        }
		break;

        case EB_ADD:
            if (++option >= NUM_FILTER_OPTIONS)
                option = FIRST_FILTER_OPTION;
            meDisplayOption = static_cast<eFilterOptions>(option);
            break;

        case EB_SUBTRACT:
            if (option == FIRST_FILTER_OPTION)
                option = NUM_FILTER_OPTIONS-1;
            else
                --option;
            meDisplayOption = static_cast<eFilterOptions>(option);
            break;

        default:
            break;
    }
    
    switch (meDisplayOption)
	{
		case CONE_FILTER:
		case BOX9_FILTER:
		case BOX16_FILTER:
			m_strEffectPixelShader = GetFilePath("BlurBlit.nvp");
			break;
		case SHARPEN_FILTER:
			m_strEffectPixelShader = GetFilePath("SharpenBlit.nvp");
				break;
		case LUMINANCE_EDGE:
			m_strEffectPixelShader = GetFilePath("LuminanceDiagEdgeBlit.nvp");
			break;
		default:
		;
	}
	m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PIXELSHADERS;

}



