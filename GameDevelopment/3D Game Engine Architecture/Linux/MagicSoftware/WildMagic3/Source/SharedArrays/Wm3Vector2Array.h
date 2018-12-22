// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3VECTOR2ARRAY_H
#define WM3VECTOR2ARRAY_H

#include "Wm3TSharedArray.h"
#include "Wm3Vector2.h"

namespace Wm3
{
typedef TSharedArray<Vector2f> Vector2fArray;
typedef Pointer<Vector2fArray> Vector2fArrayPtr;
WM3_REGISTER_STREAM(Vector2fArray);

typedef TSharedArray<Vector2d> Vector2dArray;
typedef Pointer<Vector2dArray> Vector2dArrayPtr;
WM3_REGISTER_STREAM(Vector2dArray);
}

#endif
