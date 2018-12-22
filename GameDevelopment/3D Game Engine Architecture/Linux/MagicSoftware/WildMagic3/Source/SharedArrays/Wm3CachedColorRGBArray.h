// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3CACHEDCOLORRGBARRAY_H
#define WM3CACHEDCOLORRGBARRAY_H

#include "Wm3TCachedArray.h"
#include "Wm3ColorRGBArray.h"

namespace Wm3
{
typedef TCachedArray<ColorRGB> CachedColorRGBArray;
typedef Pointer<CachedColorRGBArray> CachedColorRGBArrayPtr;
WM3_REGISTER_STREAM(CachedColorRGBArray);
}

#endif
