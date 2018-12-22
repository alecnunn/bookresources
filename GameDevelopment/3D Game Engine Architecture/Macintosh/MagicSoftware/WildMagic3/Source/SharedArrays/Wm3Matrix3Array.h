// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3MATRIX3ARRAY_H
#define WM3MATRIX3ARRAY_H

#include "Wm3TSharedArray.h"
#include "Wm3Matrix3.h"

namespace Wm3
{
typedef TSharedArray<Matrix3f> Matrix3fArray;
typedef Pointer<Matrix3fArray> Matrix3fArrayPtr;
WM3_REGISTER_STREAM(Matrix3fArray);

typedef TSharedArray<Matrix3d> Matrix3dArray;
typedef Pointer<Matrix3dArray> Matrix3dArrayPtr;
WM3_REGISTER_STREAM(Matrix3dArray);
}

#endif
