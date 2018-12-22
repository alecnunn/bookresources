// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3DXAPPLYPARAMETERS_H
#define WM3DXAPPLYPARAMETERS_H

#include "Wm3RendererLibType.h"
#include "Wm3DxUtility.h"

class WM3_RENDERER_ITEM ApplyParameters
{
public:
    ApplyParameters ();
    ApplyParameters (DWORD dwArg1);
    ApplyParameters (D3DTEXTUREOP eOp, DWORD dwArg1, DWORD dwArg2);
    ApplyParameters (D3DTEXTUREOP eOp, DWORD dwArg1, DWORD dwArg2,
        DWORD dwArg3);

    void Set (DWORD dwArg1);
    void Set (D3DTEXTUREOP eOp, DWORD dwArg1, DWORD dwArg2);
    void Set (D3DTEXTUREOP eOp, DWORD dwArg1, DWORD dwArg2, DWORD dwArg3);

    // Convert WildMagic texture apply combine parameters into DirectX
    // parameters.  The return value indicates whether or not the conversion
    // was successful.
    bool ConvertWmToDx (int eCombineFunction, int eCombineScale, DWORD dwArg0,
        DWORD dwArg1, DWORD dwArg2);

    int Arguments;    // 1, 2, or 3 (set by the constructors)
    D3DTEXTUREOP Op;  // texture operation
    DWORD Arg0;       // DX Arg0 is WM Arg2
    DWORD Arg1;       // DX Arg1 is WM Arg0
    DWORD Arg2;       // DX Arg2 is WM Arg1
};

#endif
