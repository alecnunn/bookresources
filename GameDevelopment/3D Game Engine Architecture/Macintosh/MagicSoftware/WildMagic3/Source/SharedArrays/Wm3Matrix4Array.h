// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3MATRIX4ARRAY_H
#define WM3MATRIX4ARRAY_H

#include "Wm3TSharedArray.h"
#include "Wm3Matrix4.h"

namespace Wm3
{
typedef TSharedArray<Matrix4f> Matrix4fArray;
typedef Pointer<Matrix4fArray> Matrix4fArrayPtr;
WM3_REGISTER_STREAM(Matrix4fArray);

typedef TSharedArray<Matrix4d> Matrix4dArray;
typedef Pointer<Matrix4dArray> Matrix4dArrayPtr;
WM3_REGISTER_STREAM(Matrix4dArray);
}

#endif
