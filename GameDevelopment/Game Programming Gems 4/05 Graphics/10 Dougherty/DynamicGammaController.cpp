//-----------------------------------------------------------------------------
// File: DynamicGammaController.cpp
// Desc: Contains the implementation of the dynamic gamma controller
//
// Copyright © 2003 Michael Dougherty (mdougher@hotmail.com).
// All rights reserved worldwide.
//
// Permission to copy, modify, reproduce or redistribute this source code is
// granted provided the above copyright notice is retained in the resulting 
// source code.
// 
// This software is provided "as is" and without any express or implied
// warranties.
//
//-----------------------------------------------------------------------------


#include "stdafx.h"
#include "helpers.h"
#include "dynamicgammacontroller.h"


//-----------------------------------------------------------------------------
// Name: Luminance pixel shader
// Desc: Transforms RGB to luminance using the luminance transform:
//       lum = 0.3R + 0.59G + 0.11B
// Code: ps.1.1
//
//       def c0, 0.3f, 0.59f, 0.11f, 0.0f ; Luminance transform
//
//       tex t0
//       ; Output t0 dot lum
//       dp3 r0, c0, t0
//-----------------------------------------------------------------------------
DWORD dwLumPixelShaderCode[] = 
{
    0xffff0101, 0x00000051, 0xa00f0000, 0x3e99999a, 0x3f170a3d, 0x3de147ae, 
    0x00000000, 0x00000042, 0xb00f0000, 0x00000008, 0x800f0000, 0xa0e40000, 
    0xb0e40000, 0x0000ffff
};


//-----------------------------------------------------------------------------
// Name: Gamma pixel shader
// Desc: Transforms RGB by a gamma ramp.
//       Used to compensate for gamma ramp adjustments of HUD elements.
//       
// Note: There is a more efficient ps 2.0 way of doing this on is supported
//       on more video cards.
//       
// Code: ps.1.1
//       def c0, 1.0f, 0.0f, 0.0f, 0.0f
//       def c1, 0.0f, 1.0f, 0.0f, 0.0f
//       def c2, 0.0f, 0.0f, 1.0f, 0.0f
//
//       tex t0            ; Color texture
//       texreg2ar t1, t0  ; Look up r  
//       texreg2gb t2, t0  ; Look up gb 

//       ; r0 = gamma(r), gamma(g), gamma(b), 0
//       mul r0, c0, t1
//       mad r0, c1, t2, r0
//       mad r0, c2, t2, r0
//
//       ; Output r0.rgb and t0.a
//       mov r0.a, t0.a
//-----------------------------------------------------------------------------
DWORD dwGammaPixelShaderCode[] = 
{
    0xffff0101, 0x00000051, 0xa00f0000, 0x3f800000, 0x00000000, 0x00000000, 
    0x00000000, 0x00000051, 0xa00f0001, 0x00000000, 0x3f800000, 0x00000000, 
    0x00000000, 0x00000051, 0xa00f0002, 0x00000000, 0x00000000, 0x3f800000, 
    0x00000000, 0x00000042, 0xb00f0000, 0x00000045, 0xb00f0001, 0xb0e40000, 
    0x00000046, 0xb00f0002, 0xb0e40000, 0x00000005, 0x800f0000, 0xa0e40000, 
    0xb0e40001, 0x00000004, 0x800f0000, 0xa0e40001, 0xb0e40002, 0x80e40000, 
    0x00000004, 0x800f0000, 0xa0e40002, 0xb0e40002, 0x80e40000, 0x00000001, 
    0x80080000, 0xb0ff0000, 0x0000ffff
};


//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Must be called before the controller is used
//-----------------------------------------------------------------------------
VOID DynamicGammaController::Initialize()
{
    // Create luminance pixel shader
    VERIFY( g_pD3DDevice->CreatePixelShader( dwLumPixelShaderCode, &m_pLuminancePixelShader ) );
    
    // Create gamma pixel shader
    VERIFY( g_pD3DDevice->CreatePixelShader( dwGammaPixelShaderCode, &m_pGammaPixelShader ) );
    
    // Set default values
    m_fBias             = 0.8f;
    m_fDarkClamp        = 0.3f;
    m_fLightClamp       = 0.4f;
    m_fDarkAdjustDt     = 0.15f;
    m_fLightAdjustDt    = 0.20f;
    m_fContrastAdjustDt = 0.07f;
    m_fDarkThreshold    = 0.010f;
    m_fLightThreshold   = 0.010f;
    m_fMin              = 0.0f;
    m_fMax              = 1.0f;
    m_fAvg              = 2.5f;

    m_pLuminanceSurface = NULL;
    ZeroMemory( &m_Histogram, sizeof(Histogram));

    // Create the inverse gamma ramp texture, for use with the pixel shader
    // which maps colors through an inverse of the gamma ramp. This is so
    // that when the gamma ramp is applied globally, HUD elements will still
    // appear properly. Note that these textures will be addressed with tex
    // coords ranging from [0..1], so the textures must be swizzled.
    VERIFY( g_pD3DDevice->CreateTexture( 64, 64,
                                         1,
                                         D3DUSAGE_DYNAMIC,
                                         D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
                                         &m_pInvGammaRampLUT,
                                         NULL ) );

    // Create the gamma ramp texture, for use with the pixel shader
    // which maps colors through the gamma ramp.  This shader is used
    // for doing the gamma adjustment in a pixel shader vs. the hardware
    // gamma ramp
    VERIFY( g_pD3DDevice->CreateTexture( 64, 64,
                                         1,
                                         D3DUSAGE_DYNAMIC,
                                         D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
                                         &m_pGammaRampLUT,
                                         NULL ) );
}


//-----------------------------------------------------------------------------
// Name: Finalize()
// Desc: Frees resource required by the controller and resets the gamma ramp
//       back to linear
//-----------------------------------------------------------------------------
VOID DynamicGammaController::Finalize()
{
    // Free resources
    SAFE_RELEASE( m_pLuminanceSurface );
    SAFE_RELEASE( m_pLuminancePixelShader );
    SAFE_RELEASE( m_pGammaPixelShader );
    SAFE_RELEASE( m_pInvGammaRampLUT );
    SAFE_RELEASE( m_pGammaRampLUT );
    
    // Set linear gamma ramp
    D3DGAMMARAMP Ramp;
    for( UINT i = 0; i < 256; i++ )
    {
        Ramp.red[i] = Ramp.green[i] = Ramp.blue[i] = WORD(i * 257);
    }
    g_pD3DDevice->SetGammaRamp( 0, D3DSGR_NO_CALIBRATION, &Ramp );
}


//-----------------------------------------------------------------------------
// Name: Clamp()
//-----------------------------------------------------------------------------
template<class T>
__forceinline VOID Clamp( T* pVal, T Min, T Max )
{
    if( *pVal < Min )      *pVal = Min;
    else if( *pVal > Max ) *pVal = Max;
}


//-----------------------------------------------------------------------------
// Name: GetLuminance()
// Desc: Renders a down sampled luminance transformed version of the input
//       texture
//-----------------------------------------------------------------------------
VOID DynamicGammaController::GetLuminance( IDirect3DTexture9* pTextureSrc,
                                           UINT nSuperSampleX,
                                           UINT nSuperSampleY )
{
    // Save current render target and depth buffer
    IDirect3DSurface9 *pRenderTarget, *pZBuffer;
    VERIFY( g_pD3DDevice->GetRenderTarget( 0, &pRenderTarget ) );
    VERIFY( g_pD3DDevice->GetDepthStencilSurface( &pZBuffer ) );
    
    static UINT LastSuperSampleX = 0;
    static UINT LastSuperSampleY = 0;

    // Get the source texture description
    D3DSURFACE_DESC descSrc;
    pTextureSrc->GetLevelDesc( 0, &descSrc );

    // update destination texture if the source or super sample amounts change
    if( m_pLuminanceSurface == NULL ||
        LastSuperSampleX != nSuperSampleX ||
        LastSuperSampleY != nSuperSampleY )
    {
        SAFE_RELEASE( m_pLuminanceSurface );
        VERIFY( g_pD3DDevice->CreateRenderTarget( descSrc.Width / nSuperSampleX,
                                                  descSrc.Height / nSuperSampleY,
                                                  D3DFMT_A8R8G8B8,
                                                  D3DMULTISAMPLE_NONE,
                                                  0,
                                                  TRUE,
                                                  &m_pLuminanceSurface,
                                                  NULL ) );
    }

    // Set destination as render target
    VERIFY( g_pD3DDevice->SetRenderTarget( 0, m_pLuminanceSurface )  );
    VERIFY( g_pD3DDevice->SetDepthStencilSurface( NULL ) ); // no depth-bufferin
        
    // Get description of dest
    D3DSURFACE_DESC descDst;
    VERIFY( m_pLuminanceSurface->GetDesc( &descDst ) );

    //Set render state for filtering
    VERIFY( g_pD3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE ) );
    VERIFY( g_pD3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID ) );
    VERIFY( g_pD3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE ) );
    VERIFY( g_pD3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE ) );
    VERIFY( g_pD3DDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE ) );
    VERIFY( g_pD3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE ) );
    VERIFY( g_pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ) );
    
    VERIFY( g_pD3DDevice->SetTexture( 0, pTextureSrc) );
    
    VERIFY( g_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE ) );
    VERIFY( g_pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE ) );
    
    // Pass texture coords without transformation
    VERIFY( g_pD3DDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );  );
    // Each texture has different tex coords
    VERIFY( g_pD3DDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 ); );
    VERIFY( g_pD3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP ) );
    VERIFY( g_pD3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP ) );
    VERIFY( g_pD3DDevice->SetSamplerState( 0, D3DSAMP_MAXMIPLEVEL, 0 ) );
    VERIFY( g_pD3DDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE ) );
    VERIFY( g_pD3DDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT ) );
    VERIFY( g_pD3DDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT ) );
    
    // Set luminance transform pixel shader
    VERIFY( g_pD3DDevice->SetPixelShader( m_pLuminancePixelShader ) );
    
    // For screen-space texture-mapped quadrilateral
    VERIFY( g_pD3DDevice->SetFVF( D3DFVF_XYZRHW|D3DFVF_TEX1 ) );
    VERIFY( g_pD3DDevice->SetVertexShader( NULL ) );

    // Prepare quadrilateral vertices
    FLOAT x0 = 0.0f - 0.5f;
    FLOAT y0 = 0.0f - 0.5f;
    FLOAT x1 = (FLOAT)descDst.Width - 0.5f;
    FLOAT y1 = (FLOAT)descDst.Height - 0.5f;
    
    // Draw a quad 
    FLOAT tu0 = 0.0f;
    FLOAT tv0 = 0.0f;
    FLOAT tu1 = 1.0f;
    FLOAT tv1 = 1.0f;

    struct QUADVERTEX
    {
        FLOAT x, y, z, w;
        FLOAT tu, tv;
    };
    QUADVERTEX aQuad[6] =
    {   // X   Y   Z     W    tu   tv
        { x0, y0, 1.0f, 1.0f, tu0, tv0 },
        { x1, y0, 1.0f, 1.0f, tu1, tv0 },
        { x1, y1, 1.0f, 1.0f, tu1, tv1 },

        { x0, y0, 1.0f, 1.0f, tu0, tv0 },
        { x1, y1, 1.0f, 1.0f, tu1, tv1 },
        { x0, y1, 1.0f, 1.0f, tu0, tv1 },
    };

    VERIFY( g_pD3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 2, aQuad, sizeof(QUADVERTEX) ); );
    
            
    // Restore render target and z buffer
    VERIFY( g_pD3DDevice->SetRenderTarget( 0, pRenderTarget ) );
    VERIFY( g_pD3DDevice->SetDepthStencilSurface( pZBuffer ) );

    SAFE_RELEASE( pRenderTarget );
    SAFE_RELEASE( pZBuffer );

    // Restore state
    VERIFY( g_pD3DDevice->SetTexture( 0, NULL ) );
    VERIFY( g_pD3DDevice->SetPixelShader( NULL ) );
};


//-----------------------------------------------------------------------------
// Name: GetHistogram()
// Desc: Builds a histogram from the current down sampled luminance texture
//-----------------------------------------------------------------------------
VOID DynamicGammaController::GetHistogram()
{
    ZeroMemory( &m_Histogram, sizeof(Histogram) );

    D3DSURFACE_DESC desc;
    VERIFY( m_pLuminanceSurface->GetDesc( &desc ) );

    // Get sample values and copy them into cached buffer for faster reads
    D3DLOCKED_RECT Rect;
    VERIFY( m_pLuminanceSurface->LockRect( &Rect, NULL, D3DLOCK_READONLY ) );
    
        
    for( UINT i = 0; i < desc.Height; i++ )
    {
        for( UINT j = 0; j < desc.Width; j++ )
        {
            BYTE index = ((BYTE*)Rect.pBits)[i* Rect.Pitch + 4*j];
            m_Histogram.Lums[index] += 1.0f;
        }
    }

    VERIFY( m_pLuminanceSurface->UnlockRect() );
    
    // Normalize histogram
    FLOAT fSum = 0.0f;
    for( UINT i = 0; i < 256; i++ )
        fSum += m_Histogram.Lums[i];
    if( fSum != 0.0f )
    {
        FLOAT fInvSum = 1.0f/fSum;
        for( UINT i = 0; i < 256; i++ )
            m_Histogram.Lums[i] *= fInvSum;
    }

    // Compute current min, max, and average
    FLOAT fMinArea;
    FLOAT fArea;
    
    fMinArea = m_fDarkThreshold;
    fArea = 0.0f;
    for( int i = 0; i < 256; i++ )
    {
        fArea += m_Histogram.Lums[i];
        if( fArea >= fMinArea )
        {
            m_Histogram.fMin = FLOAT(i)/255.0f;
            break;
        }
    }
    
    fMinArea = m_fLightThreshold;
    fArea = 0.0f;
    for( int i = 255; i > -1; i-- )
    {
        fArea += m_Histogram.Lums[i];
        if( fArea >= fMinArea )
        {
            m_Histogram.fMax = FLOAT(i)/255.0f;
            break;
        }
    }

    m_Histogram.fAvg = 0.0f;
    for( int i = 0; i < 256; i++ )
    {
        m_Histogram.fAvg += FLOAT(i) * m_Histogram.Lums[i];
    }
    m_Histogram.fAvg /= 255.0f;
}


//-----------------------------------------------------------------------------
// Name: UpdateGammaValues()
// Desc: Updates the gamma ramp parameters over time
//-----------------------------------------------------------------------------
VOID DynamicGammaController::UpdateGammaValues( FLOAT fDt )
{
    const FLOAT fNoChangeLumThreshold = 0.01f;
    const FLOAT fNoChangeContThreshold = 0.01f;

    // Update if change over dt
    if( m_Histogram.fMin < m_fMin - fNoChangeLumThreshold )
        m_fMin -= m_fDarkAdjustDt * fDt;
    else if( m_Histogram.fMin > m_fMin + fNoChangeLumThreshold )
        m_fMin += m_fLightAdjustDt * fDt; 

    if( m_Histogram.fMax < m_fMax - fNoChangeLumThreshold )
        m_fMax -= m_fDarkAdjustDt * fDt; 
    else if( m_Histogram.fMax > m_fMax + fNoChangeLumThreshold )
        m_fMax += m_fLightAdjustDt * fDt;  

    if( m_Histogram.fAvg < m_fAvg - fNoChangeContThreshold )
        m_fAvg -= m_fContrastAdjustDt * fDt;
    else if( m_Histogram.fAvg > m_fAvg + fNoChangeContThreshold )
        m_fAvg += m_fContrastAdjustDt * fDt;

    Clamp( &m_fMin, 0.0f, 1.0f );
    Clamp( &m_fMax, m_fDarkClamp, 1.0f );
    Clamp( &m_fMin, 0.0f, m_fLightClamp );
    Clamp( &m_fAvg, m_fMin, m_fMax );
}


//-----------------------------------------------------------------------------
// Name: SetGammaRamp()
// Desc: Sets the gamma ramp and updates gamma textures
//-----------------------------------------------------------------------------
VOID DynamicGammaController::SetGammaRamp( BOOL bUseHardwareGammaRamp )
{
    D3DGAMMARAMP Ramp;

    // Compute normalized distance of fAvg [0,1] between min and max
    FLOAT Dist = (m_fAvg - m_fMin)/(m_fMax - m_fMin);

    // Set curve based on dist
    FLOAT P = 1.0f + Dist*m_fBias;

    for( UINT i = 0; i < 256; i++ )
    {
        FLOAT x = i/255.0f;
        FLOAT y;
        if( x <= m_fMin )
            y = 0.0f;
        else if( x >= m_fMax )
            y = 1.0f;
        else
        {
            y = powf( (x - m_fMin) / (m_fMax - m_fMin), P );
            Clamp( &y, 0.0f, 1.0f );
        }
        Ramp.red[i] = Ramp.green[i] = Ramp.blue[i] = WORD( ( y * 255.0f ) * 257.0f);
    }

    SetDebugdrawRamp( &Ramp );

    // if were using hardware gamma, update the hardware gamma ramp and
    // build the inverse gamma texture
    if( bUseHardwareGammaRamp )
    {
        g_pD3DDevice->SetGammaRamp( 0, D3DSGR_NO_CALIBRATION, &Ramp );
    
        D3DLOCKED_RECT LockedRect;
        BYTE* pBits;

        // Update inverse gamma ramp texture
        BYTE  InvGammaRamp[256];
        FLOAT InvP = 1.0f/P;
        for( UINT i = 0; i < 256; i++ )
        {
            FLOAT y = i/255.0f;
            FLOAT x = (m_fMax - m_fMin) * powf( y, InvP ) + m_fMin;
            Clamp( &x, 0.0f, 1.0f );
            InvGammaRamp[i] = (BYTE)(x * 255.0f + 0.5f);
        }

        // Copy the inv gamma ramp to the texture.
        VERIFY( m_pInvGammaRampLUT->LockRect( 0, &LockedRect, NULL, 0) );
        pBits = (BYTE*)LockedRect.pBits;

        
        // Create the LUT texture
        for( DWORD y = 0; y < 64; y++ )
        {
            BYTE b = InvGammaRamp[ y*4 ];

            for( DWORD x = 0; x < 64; x++ )
            {
                BYTE g = InvGammaRamp[ x*4 ];
                *( (DWORD*)(pBits + LockedRect.Pitch * y + 4 * x ) ) = (g<<24)|(b<<16)|(g<<8)|(b<<0);
            }
        }
        VERIFY( m_pInvGammaRampLUT->UnlockRect( 0 ) );
    }
    // otherwise, build the gamma transform texture
    else
    {
        D3DLOCKED_RECT LockedRect;
        BYTE* pBits;

        // update gamma ramp texture
        BYTE GammaRamp[256];
        for( UINT i = 0; i < 256; i++ )
        {
            FLOAT x = i/255.0f;
            FLOAT y;
            if( x <= m_fMin )
                y = 0.0f;
            else if( x >= m_fMax )
                y = 1.0f;
            else
            {
                y = powf( (x - m_fMin) / (m_fMax - m_fMin), P );
                Clamp( &y, 0.0f, 1.0f );
            }
            GammaRamp[i] = BYTE( y * 255.0f + 0.5f );
        }
    
        // Copy the gamma ramp to the texture.
        VERIFY( m_pGammaRampLUT->LockRect( 0, &LockedRect, NULL, 0) );
        pBits = (BYTE*)LockedRect.pBits;

        
        // Create the LUT texture
        for( DWORD y = 0; y < 64; y++ )
        {
            BYTE b = GammaRamp[ y*4 ];

            for( DWORD x = 0; x < 64; x++ )
            {
                BYTE g = GammaRamp[ x*4 ];
                *( (DWORD*)(pBits + LockedRect.Pitch * y + 4 * x ) ) = (g<<24)|(b<<16)|(g<<8)|(b<<0);
            }
        }
        VERIFY( m_pGammaRampLUT->UnlockRect( 0 ) );
    }
}


//-----------------------------------------------------------------------------
// Name: DrawHistogram()
// Desc: Draws the current histogram.  Used for debugging
//-----------------------------------------------------------------------------
VOID DynamicGammaController::DrawHistogram( FLOAT fX1, FLOAT fY1,
                                            FLOAT fX2, FLOAT fY2 )
{
    FLOAT dx = fX2 - fX1;
    FLOAT dy = fY2 - fY1;

    VERIFY( g_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 ) );
    VERIFY( g_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR ) );
    VERIFY( g_pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE ) );
    VERIFY( g_pD3DDevice->SetRenderState( D3DRS_TEXTUREFACTOR,    0x00000000 ) );
    VERIFY( g_pD3DDevice->SetRenderState( D3DRS_ZENABLE,          D3DZB_FALSE ) );
    VERIFY( g_pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ) );
    VERIFY( g_pD3DDevice->SetFVF( D3DFVF_XYZRHW ) );
    VERIFY( g_pD3DDevice->SetVertexShader( NULL ) );

    // Draw black area
    struct QUADVERTEX
    {
        FLOAT x, y, z, w;
    };
    QUADVERTEX aQuad[6] =
    {   // X   Y   Z     W    tu   tv
        { fX1, fY1, 1.0f, 1.0f },
        { fX2, fY1, 1.0f, 1.0f },
        { fX2, fY2, 1.0f, 1.0f },

        { fX1, fY1, 1.0f, 1.0f },
        { fX2, fY2, 1.0f, 1.0f },
        { fX1, fY2, 1.0f, 1.0f },
    };

    VERIFY( g_pD3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 2, aQuad, sizeof(QUADVERTEX) ); );
    
    D3DXVECTOR4 Zones[3];

    for( UINT i = 0; i < 3; i++ )
    {
        FLOAT fVal;
        if( i == 0 )
        {
            fVal = m_Histogram.fMin;
            VERIFY( g_pD3DDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0x00ffffff ) );
        }
        else if( i == 1 )
        {
            fVal = m_Histogram.fMax;
            VERIFY( g_pD3DDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0x00ffffff ) );
        }
        else
        {
            fVal = m_Histogram.fAvg;
            VERIFY( g_pD3DDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0x00aaaaaa ) );
        }

        Zones[0].x = fX1 + dx * ( 0.9f*fVal + 0.1f/2.0f );
        Zones[0].y = fY2 - dy * ( 0.1f/2.0f );
        Zones[0].z = 0.0f;
        Zones[0].w = 0.0f;
        Zones[1].x = fX1 + dx * ( 0.9f*fVal + 0.1f/2.0f + 0.1f/2.0f );
        Zones[1].y = fY2;
        Zones[1].z = 0.0f;
        Zones[1].w = 0.0f;
        Zones[2].x = fX1 + dx * ( 0.9f*fVal + 0.1f/2.0f - 0.1f/2.0f );
        Zones[2].y = fY2;
        Zones[2].z = 0.0f;
        Zones[2].w = 0.0f;

        VERIFY( g_pD3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 1, Zones, sizeof(D3DXVECTOR4) ) );
    }

    // Draw histogram
    D3DXVECTOR4 Lines[256 * 2];
    g_pD3DDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0x00ff0000 );

    FLOAT fMaxLum = 0;
    for( UINT i = 0; i < 256; i++ )
    {
        fMaxLum = (m_Histogram.Lums[i] > fMaxLum) ? m_Histogram.Lums[i] : fMaxLum;
    }

    if( fMaxLum == 0.0f )
        fMaxLum = 1.0f;
    FLOAT fInvMaxLum = 1.0f/fMaxLum;
    
    for( UINT i = 0; i < 256; i++ )
    {
        Lines[2*i+0].x = fX1 + dx * ( i*0.9f/255.0f + 0.1f/2.0f );
        Lines[2*i+0].y = fY2 - dy * ( 0.1f/2.0f );
        Lines[2*i+0].z = 0.0f;
        Lines[2*i+0].w = 0.0f;
        Lines[2*i+1].x = fX1 + dx * ( i*0.9f/255.0f + 0.1f/2.0f );
        Lines[2*i+1].y = fY2 - dy * ( 0.1f/2.0f + 0.9f*m_Histogram.Lums[i]*fInvMaxLum );
        Lines[2*i+1].z = 0.0f;
        Lines[2*i+1].w = 0.0f;
    }
    VERIFY( g_pD3DDevice->DrawPrimitiveUP( D3DPT_LINELIST, 254, Lines, sizeof(Lines[0]) ) );
    
    // Restore state
    VERIFY( g_pD3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE ) );
    VERIFY( g_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE ) );
}


//-----------------------------------------------------------------------------
// Name: DrawGammaRamps()
// Desc: Draws the current gamma ramps and inverse gamma. Used for debugging
//-----------------------------------------------------------------------------
VOID DynamicGammaController::DrawGammaRamps( FLOAT fX1, FLOAT fY1,
                                             FLOAT fX2, FLOAT fY2 )
{
    // Draw black area
    VERIFY( g_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 ) );
    VERIFY( g_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR ) );
    VERIFY( g_pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE ) );
    VERIFY( g_pD3DDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA( 0, 0, 0, 0 ) ) );
    VERIFY( g_pD3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE ) );
    VERIFY( g_pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ) );
    VERIFY( g_pD3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE ) );
    VERIFY( g_pD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE ) );
    VERIFY( g_pD3DDevice->SetFVF( D3DFVF_XYZRHW ) );
    VERIFY( g_pD3DDevice->SetVertexShader( NULL ) );

    // Draw black area
    struct QUADVERTEX
    {
        FLOAT x, y, z, w;
    };
    QUADVERTEX aQuad[6] =
    {   // X   Y   Z     W    tu   tv
        { fX1, fY1, 1.0f, 1.0f },
        { fX2, fY1, 1.0f, 1.0f },
        { fX2, fY2, 1.0f, 1.0f },

        { fX1, fY1, 1.0f, 1.0f },
        { fX2, fY2, 1.0f, 1.0f },
        { fX1, fY2, 1.0f, 1.0f },
    };

    VERIFY( g_pD3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 2, aQuad, sizeof(QUADVERTEX) ); );
    
    // Draw gamma ramp
    VERIFY( g_pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ) );
    VERIFY( g_pD3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE ) );
    VERIFY( g_pD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE ) );

    D3DXVECTOR4 Lines[256];
    FLOAT dx = (fX2 - fX1) / 255.0f;
    FLOAT dy = (fY2 - fY1) / 255.0f;

    // Draw red channel of gamma ramp
    VERIFY( g_pD3DDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA( 255, 0, 0, 0 ) ) );
    for( UINT i = 0; i < 256; i++ )
    {
        Lines[i].x = fX1 + dx * i;
        Lines[i].y = fY2 - dy * (m_Ramp.red[i]/257.0f);
        Lines[i].z = 0.0f;
        Lines[i].w = 0.0f;
    }
    VERIFY( g_pD3DDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, 254, Lines, sizeof(Lines[0]) ) );
    
    // Draw green channel of gamma ramp
    VERIFY( g_pD3DDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA( 0, 255, 0, 0 ) ) );
    for( UINT i = 0; i < 256; i++ )
    {
        Lines[i].y = fY2 - dy * (m_Ramp.green[i]/257.0f);
    }
    VERIFY( g_pD3DDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, 254, Lines, sizeof(Lines[0]) ) );
    
    // Draw blue channel of gamma ramp
    VERIFY( g_pD3DDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA( 0, 0, 255, 0 ) ) );
    for( UINT i = 0; i < 256; i++ )
    {
        Lines[i].y = fY2 - dy * (m_Ramp.blue[i]/257.0f);
    }
    VERIFY( g_pD3DDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, 254, Lines, sizeof(Lines[0]) ) );
    
    // Restore state
    VERIFY( g_pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ) );
    VERIFY( g_pD3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE ) );
    VERIFY( g_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE ) );
}


//-----------------------------------------------------------------------------
// Name: SetInvGammaCorrection()
// Desc: Sets the pixel shader, textures, and state required to draw objects
//       (for example, UI elements) with linear gamma. (In essence, the pixel
//       shader undoes (or compensates for) the dynamic gamma.) See the inv
//       gamma pixel shader code for details.
//-----------------------------------------------------------------------------
VOID DynamicGammaController::SetInvGammaCorrection()
{
    // Setup inverse gamma textures
    VERIFY( g_pD3DDevice->SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_NONE ) );
    VERIFY( g_pD3DDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR ) );
    VERIFY( g_pD3DDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR ) );
    VERIFY( g_pD3DDevice->SetSamplerState( 2, D3DSAMP_MIPFILTER, D3DTEXF_NONE ) );
    VERIFY( g_pD3DDevice->SetSamplerState( 2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR ) );
    VERIFY( g_pD3DDevice->SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR ) );
    VERIFY( g_pD3DDevice->SetSamplerState( 1, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP ) );
    VERIFY( g_pD3DDevice->SetSamplerState( 1, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP ) );
    VERIFY( g_pD3DDevice->SetSamplerState( 2, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP ) );
    VERIFY( g_pD3DDevice->SetSamplerState( 2, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP ) );
    VERIFY( g_pD3DDevice->SetTexture( 1, m_pInvGammaRampLUT ) );
    VERIFY( g_pD3DDevice->SetTexture( 2, m_pInvGammaRampLUT ) );
    
    // Set inverse gamma pixel shader
    VERIFY( g_pD3DDevice->SetPixelShader( m_pGammaPixelShader ) );
}

//-----------------------------------------------------------------------------
// Name: SetGammaCorrection()
// Desc: Sets the pixel shader, textures, and state required to draw objects
//       adusted gamma
//-----------------------------------------------------------------------------
VOID DynamicGammaController::SetGammaCorrection()
{
    // Setup inverse gamma textures
    VERIFY( g_pD3DDevice->SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_NONE ) );
    VERIFY( g_pD3DDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR ) );
    VERIFY( g_pD3DDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR ) );
    VERIFY( g_pD3DDevice->SetSamplerState( 2, D3DSAMP_MIPFILTER, D3DTEXF_NONE ) );
    VERIFY( g_pD3DDevice->SetSamplerState( 2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR ) );
    VERIFY( g_pD3DDevice->SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR ) );
    VERIFY( g_pD3DDevice->SetSamplerState( 1, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP ) );
    VERIFY( g_pD3DDevice->SetSamplerState( 1, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP ) );
    VERIFY( g_pD3DDevice->SetSamplerState( 2, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP ) );
    VERIFY( g_pD3DDevice->SetSamplerState( 2, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP ) );
    VERIFY( g_pD3DDevice->SetTexture( 1, m_pGammaRampLUT ) );
    VERIFY( g_pD3DDevice->SetTexture( 2, m_pGammaRampLUT ) );
    
    // Set inverse gamma pixel shader
    VERIFY( g_pD3DDevice->SetPixelShader( m_pGammaPixelShader ) );
}

//-----------------------------------------------------------------------------
// Name: SetDebugdrawRamp()
// Desc: sets the ramp used to draw the debug gamma ramp
//-----------------------------------------------------------------------------
VOID DynamicGammaController::SetDebugdrawRamp( D3DGAMMARAMP* pRamp )
{
    m_Ramp = *pRamp;
}
