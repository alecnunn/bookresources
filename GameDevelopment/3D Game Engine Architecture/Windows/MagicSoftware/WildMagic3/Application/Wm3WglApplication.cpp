// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include <windows.h>
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

#include "Wm3WglRenderer.h"
using namespace Wm3;

//----------------------------------------------------------------------------
Renderer* MsCreateRenderer (HWND hWnd, int iWidth, int iHeight)
{
    return new WglRenderer(hWnd,iWidth,iHeight);
}
//----------------------------------------------------------------------------
