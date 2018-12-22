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

DWORD DxRenderer::ms_adwFogDensity[FogState::DF_QUANTITY] = 
{
    D3DFOG_LINEAR,  // FogState::DF_LINEAR
    D3DFOG_EXP,     // FogState::DF_EXP
    D3DFOG_EXP2,    // FogState::DF_EXPSQR
};

//----------------------------------------------------------------------------
void DxRenderer::SetFogState (FogState* pkState)
{
    if ( pkState->Enabled )
    {
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_FOGENABLE,TRUE);
        assert( SUCCEEDED(ms_hResult) );

        ms_hResult = m_pqDevice->SetRenderState(D3DRS_FOGSTART,
            *((DWORD*)(&pkState->Start)));
        assert( SUCCEEDED(ms_hResult) );

        ms_hResult = m_pqDevice->SetRenderState(D3DRS_FOGEND,
            *((DWORD*)(&pkState->End)));
        assert( SUCCEEDED(ms_hResult) );

        D3DCOLOR dwFogColor = D3DCOLOR_COLORVALUE(pkState->Color.R(),
            pkState->Color.G(),pkState->Color.B(),pkState->Color.A());
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_FOGCOLOR,dwFogColor);
        assert( SUCCEEDED(ms_hResult) );

        ms_hResult = m_pqDevice->SetRenderState(D3DRS_FOGDENSITY,
            *((DWORD*)(&pkState->Density)));
        assert( SUCCEEDED(ms_hResult) );

        if ( pkState->ApplyFunction == FogState::AF_PER_PIXEL )
        {
            ms_hResult = m_pqDevice->SetRenderState(D3DRS_FOGTABLEMODE,
                ms_adwFogDensity[pkState->DensityFunction]);
            assert( SUCCEEDED(ms_hResult) );
        }
        else
        {
            ms_hResult = m_pqDevice->SetRenderState(D3DRS_FOGVERTEXMODE,
                ms_adwFogDensity[pkState->DensityFunction]);    
            assert( SUCCEEDED(ms_hResult) );
        }
    }
    else
    {
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_FOGENABLE,FALSE);
        assert( SUCCEEDED(ms_hResult) );
   }   
}
//----------------------------------------------------------------------------
