// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3VECTOR3ARRAY_H
#define WM3VECTOR3ARRAY_H

#include "Wm3TSharedArray.h"
#include "Wm3Vector3.h"

namespace Wm3
{
typedef TSharedArray<Vector3f> Vector3fArray;
typedef Pointer<Vector3fArray> Vector3fArrayPtr;
WM3_REGISTER_STREAM(Vector3fArray);

typedef TSharedArray<Vector3d> Vector3dArray;
typedef Pointer<Vector3dArray> Vector3dArrayPtr;
WM3_REGISTER_STREAM(Vector3dArray);
}

#endif
