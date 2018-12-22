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

//----------------------------------------------------------------------------
void DxRenderer::SetDitherState (DitherState* pkState)
{
    if ( pkState->Enabled )
    {
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_DITHERENABLE,TRUE);
        assert( SUCCEEDED(ms_hResult) );
    }
    else
    {
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_DITHERENABLE,FALSE);
        assert( SUCCEEDED(ms_hResult) );
    }
}
//----------------------------------------------------------------------------
