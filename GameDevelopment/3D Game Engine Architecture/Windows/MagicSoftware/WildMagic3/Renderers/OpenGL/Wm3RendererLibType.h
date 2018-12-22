// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3RENDERERLIBTYPE_H
#define WM3RENDERERLIBTYPE_H

// For the DLL library.
#ifdef WM3_RENDERER_DLL_EXPORT
#define WM3_RENDERER_ITEM __declspec(dllexport)

// For a client of the DLL library.
#else
#ifdef WM3_RENDERER_DLL_IMPORT
#define WM3_RENDERER_ITEM __declspec(dllimport)

// For the static library.
#else
#define WM3_RENDERER_ITEM

#endif
#endif
#endif
