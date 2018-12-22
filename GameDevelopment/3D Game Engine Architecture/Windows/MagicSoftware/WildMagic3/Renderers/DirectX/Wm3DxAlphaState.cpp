// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3DxRenderer.h"
using namespace Wm3;

DWORD DxRenderer::ms_adwAlphaSrcBlend[AlphaState::SBF_QUANTITY] =
{
    D3DBLEND_ZERO,          // AlphaState::SBF_ZERO
    D3DBLEND_ONE,           // AlphaState::SBF_ONE
    D3DBLEND_DESTCOLOR,     // AlphaState::SBF_DST_COLOR
    D3DBLEND_INVDESTCOLOR,  // AlphaState::SBF_ONE_MINUS_DST_COLOR
    D3DBLEND_SRCALPHA,      // AlphaState::SBF_SRC_ALPHA
    D3DBLEND_INVSRCALPHA,   // AlphaState::SBF_ONE_MINUS_SRC_ALPHA
    D3DBLEND_DESTALPHA,     // AlphaState::SBF_DST_ALPHA
    D3DBLEND_INVDESTALPHA,  // AlphaState::SBF_ONE_MINUS_DST_ALPHA
    D3DBLEND_SRCALPHASAT,   // AlphaState::SBF_SRC_ALPHA_SATURATE
};

DWORD DxRenderer::ms_adwAlphaDstBlend[AlphaState::DBF_QUANTITY] =
{
    D3DBLEND_ZERO,          // AlphaState::DBF_ZERO
    D3DBLEND_ONE,           // AlphaState::DBF_ONE
    D3DBLEND_SRCCOLOR,      // AlphaState::DBF_SRC_COLOR
    D3DBLEND_INVSRCCOLOR,   // AlphaState::DBF_ONE_MINUS_SRC_COLOR
    D3DBLEND_SRCALPHA,      // AlphaState::DBF_SRC_ALPHA
    D3DBLEND_INVSRCALPHA,   // AlphaState::DBF_ONE_MINUS_SRC_ALPHA
    D3DBLEND_DESTALPHA,     // AlphaState::DBF_DST_ALPHA
    D3DBLEND_INVDESTALPHA,  // AlphaState::DBF_ONE_MINUS_DST_ALPHA
};

DWORD DxRenderer::ms_adwAlphaTest[AlphaState::TF_QUANTITY] =
{
    D3DCMP_NEVER,           // AlphaState::TF_NEVER
    D3DCMP_LESS,            // AlphaState::TF_LESS
    D3DCMP_EQUAL,           // AlphaState::TF_EQUAL
    D3DCMP_LESSEQUAL,       // AlphaState::TF_LEQUAL
    D3DCMP_GREATER,         // AlphaState::TF_GREATER
    D3DCMP_NOTEQUAL,        // AlphaState::TF_NOTEQUAL
    D3DCMP_GREATEREQUAL,    // AlphaState::TF_GEQUAL
    D3DCMP_ALWAYS,          // AlphaState::TF_ALWAYS
};

//----------------------------------------------------------------------------
void DxRenderer::SetAlphaState (AlphaState* pkState)
{
    if ( pkState->BlendEnabled )
    {
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
        assert( SUCCEEDED(ms_hResult) );

        ms_hResult = m_pqDevice->SetRenderState(D3DRS_SRCBLEND,
            ms_adwAlphaSrcBlend[pkState->SrcBlend]);
        assert( SUCCEEDED(ms_hResult) );

        ms_hResult = m_pqDevice->SetRenderState(D3DRS_DESTBLEND,
            ms_adwAlphaDstBlend[pkState->DstBlend]);
        assert( SUCCEEDED(ms_hResult) );
    }
    else
    {
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
        assert( SUCCEEDED(ms_hResult) );
    }

    if ( pkState->TestEnabled )
    {
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
        assert( SUCCEEDED(ms_hResult) );

        ms_hResult = m_pqDevice->SetRenderState(D3DRS_ALPHAFUNC,
            ms_adwAlphaTest[pkState->Test]);
        assert( SUCCEEDED(ms_hResult) );

        ms_hResult = m_pqDevice->SetRenderState(D3DRS_ALPHAREF,
            (DWORD)(255.0f*pkState->Reference));
        assert( SUCCEEDED(ms_hResult) );
    }
    else
    {
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
        assert( SUCCEEDED(ms_hResult) );
    }
}
//----------------------------------------------------------------------------
