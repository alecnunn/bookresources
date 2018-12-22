// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3DxApplyParameters.h"
#include "Wm3Texture.h"
using namespace Wm3;

//----------------------------------------------------------------------------
ApplyParameters::ApplyParameters ()
{
    Arguments = 0;
    Op = D3DTOP_SELECTARG1;
    Arg0 = 0;
    Arg1 = 0;
    Arg2 = 0;
}
//----------------------------------------------------------------------------
ApplyParameters::ApplyParameters (DWORD dwArg1)
{
    Set(dwArg1);
}
//----------------------------------------------------------------------------
ApplyParameters::ApplyParameters (D3DTEXTUREOP eOp, DWORD dwArg1,
    DWORD dwArg2)
{
    Set(eOp,dwArg1,dwArg2);
}
//----------------------------------------------------------------------------
ApplyParameters::ApplyParameters (D3DTEXTUREOP eOp, DWORD dwArg1,
    DWORD dwArg2, DWORD dwArg3)
{
    Set(eOp,dwArg1,dwArg2,dwArg3);
}
//----------------------------------------------------------------------------
void ApplyParameters::Set (DWORD dwArg1)
{
    Arguments = 1;
    Op = D3DTOP_SELECTARG1;
    Arg1 = dwArg1;
}
//----------------------------------------------------------------------------
void ApplyParameters::Set (D3DTEXTUREOP eOp, DWORD dwArg1, DWORD dwArg2)
{
    Arguments = 2;
    Op = eOp;
    Arg1 = dwArg1;
    Arg2 = dwArg2;
}
//----------------------------------------------------------------------------
void ApplyParameters::Set (D3DTEXTUREOP eOp, DWORD dwArg1, DWORD dwArg2,
    DWORD dwArg3)
{
    // IMPORTANT!  For triadic operations, the DX documentation refers
    // to Arg1 which is really stored in *ARG0, Arg2 which is really
    // stored in *ARG1, and Arg3 which is really stored in *ARG2.
    Arguments = 3;
    Op = eOp;
    Arg0 = dwArg1;
    Arg1 = dwArg2;
    Arg2 = dwArg3;
}
//----------------------------------------------------------------------------
bool ApplyParameters::ConvertWmToDx (int eCombineFunction, int eCombineScale,
    DWORD dwArg0, DWORD dwArg1, DWORD dwArg2)
{
    if ( eCombineScale == Texture::ACSC_FOUR )
    {
        if ( eCombineFunction == Texture::ACF_MODULATE )
        {
            Set(D3DTOP_MODULATE4X,dwArg0,dwArg1);
            return true;
        }
    }
    else if ( eCombineScale == Texture::ACSC_TWO )
    {
        if ( eCombineFunction == Texture::ACF_MODULATE )
        {
            Set(D3DTOP_MODULATE2X,dwArg0,dwArg1);
            return true;
        }
        if ( eCombineFunction == Texture::ACF_ADD_SIGNED )
        {
            Set(D3DTOP_ADDSIGNED2X,dwArg0,dwArg1);
            return true;
        }
    }
    else  // eCombineScale == Texture::ACSC_ONE
    {
        switch ( eCombineFunction )
        {
        case Texture::ACF_REPLACE:
            Set(dwArg0);
            return true;
        case Texture::ACF_MODULATE:
            Set(D3DTOP_MODULATE,dwArg0,dwArg1);
            return true;
        case Texture::ACF_ADD:
            Set(D3DTOP_ADD,dwArg0,dwArg1);
            return true;
        case Texture::ACF_ADD_SIGNED:
            Set(D3DTOP_ADDSIGNED,dwArg0,dwArg1);
            return true;
        case Texture::ACF_SUBTRACT:
            Set(D3DTOP_SUBTRACT,dwArg0,dwArg1);
            return true;
        case Texture::ACF_INTERPOLATE:
            Set(D3DTOP_LERP,dwArg2,dwArg0,dwArg1);
            return true;
        case Texture::ACF_DOT3_RGB:
            Set(D3DTOP_DOTPRODUCT3,dwArg0,dwArg1);
            return true;
        case Texture::ACF_DOT3_RGBA:
            Set(D3DTOP_DOTPRODUCT3,dwArg0,dwArg1);
            return true;
        default:
            assert( false );
            break;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
