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

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"dxerr9.lib")

//----------------------------------------------------------------------------
Renderer* MsCreateRenderer (HWND hWnd, int iWidth, int iHeight)
{
    return new DxRenderer(hWnd,iWidth,iHeight);
}
//----------------------------------------------------------------------------
