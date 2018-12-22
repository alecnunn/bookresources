// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3CACHEDCOLORRGBAARRAY_H
#define WM3CACHEDCOLORRGBAARRAY_H

#include "Wm3TCachedArray.h"
#include "Wm3ColorRGBAArray.h"

namespace Wm3
{
typedef TCachedArray<ColorRGBA> CachedColorRGBAArray;
typedef Pointer<CachedColorRGBAArray> CachedColorRGBAArrayPtr;
WM3_REGISTER_STREAM(CachedColorRGBAArray);
}

#endif
