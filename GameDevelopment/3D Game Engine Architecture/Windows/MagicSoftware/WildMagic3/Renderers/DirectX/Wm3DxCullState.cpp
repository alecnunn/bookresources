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

DWORD DxRenderer::ms_adwCullType[CullState::FT_QUANTITY] = 
{
    D3DCULL_CCW,  // CullState::FT_CCW (front CCW -> cull backface CCW in DX)
    D3DCULL_CW,   // CullState::FT_CW  (front CW -> cull backface CW in DX)
};

//----------------------------------------------------------------------------
void DxRenderer::SetCullState (CullState* pkState)
{
    if ( pkState->Enabled )
    {
        if ( m_bReverseCullFace )
        {
            if ( ms_adwCullType[pkState->CullFace] == D3DCULL_CW )
            {
                ms_hResult = m_pqDevice->SetRenderState(D3DRS_CULLMODE,
                    D3DCULL_CCW);
                assert( SUCCEEDED(ms_hResult) );
            }
            else
            {
                ms_hResult = m_pqDevice->SetRenderState(D3DRS_CULLMODE,
                    D3DCULL_CW);
                assert( SUCCEEDED(ms_hResult) );
            }
        } 
        else
        {
            ms_hResult = m_pqDevice->SetRenderState(D3DRS_CULLMODE,
                ms_adwCullType[pkState->CullFace]);
            assert( SUCCEEDED(ms_hResult) );
        }
    }
    else
    {
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
        assert( SUCCEEDED(ms_hResult) );
    }
}
//----------------------------------------------------------------------------
