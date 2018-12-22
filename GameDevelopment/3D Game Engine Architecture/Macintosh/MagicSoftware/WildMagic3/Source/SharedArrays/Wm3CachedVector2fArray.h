// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3CACHEDVECTOR2FARRAY_H
#define WM3CACHEDVECTOR2FARRAY_H

#include "Wm3TCachedArray.h"
#include "Wm3Vector2Array.h"

namespace Wm3
{
typedef TCachedArray<Vector2f> CachedVector2fArray;
typedef Pointer<CachedVector2fArray> CachedVector2fArrayPtr;
WM3_REGISTER_STREAM(CachedVector2fArray);
}

#endif
