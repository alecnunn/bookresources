// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3DXUTILITY_H
#define WM3DXUTILITY_H

#include <atlbase.h>
#include <d3d9.h>
#include <d3dx9math.h>
#include <d3dx9tex.h>
#include <dxerr9.h>

namespace Wm3
{

typedef IDirect3D9 IMain;
typedef CComQIPtr<IDirect3D9,&IID_IDirect3D9>
IMainPtr;

typedef IDirect3DDevice9 IDevice;
typedef CComQIPtr<IDirect3DDevice9,&IID_IDirect3DDevice9>
IDevicePtr;

typedef ID3DXFont IFont;
typedef CComQIPtr<ID3DXFont,&IID_ID3DXFont>
IFontPtr;

typedef IDirect3DSurface9 ISurface;
typedef CComQIPtr<IDirect3DSurface9,&IID_IDirect3DSurface9>
ISurfacePtr;

typedef IDirect3DIndexBuffer9 IIndexBuffer;
typedef CComQIPtr<IDirect3DIndexBuffer9,&IID_IDirect3DIndexBuffer9>
IIndexBufferPtr;

typedef IDirect3DVertexBuffer9 IVertexBuffer;
typedef CComQIPtr<IDirect3DVertexBuffer9,&IID_IDirect3DVertexBuffer9>
IVertexBufferPtr;

typedef IDirect3DTexture9 ITexture;

}

#endif
