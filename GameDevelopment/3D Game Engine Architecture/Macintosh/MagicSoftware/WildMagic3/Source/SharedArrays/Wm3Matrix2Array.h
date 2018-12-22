// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3MATRIX2ARRAY_H
#define WM3MATRIX2ARRAY_H

#include "Wm3TSharedArray.h"
#include "Wm3Matrix2.h"

namespace Wm3
{
typedef TSharedArray<Matrix2f> Matrix2fArray;
typedef Pointer<Matrix2fArray> Matrix2fArrayPtr;
WM3_REGISTER_STREAM(Matrix2fArray);

typedef TSharedArray<Matrix2d> Matrix2dArray;
typedef Pointer<Matrix2dArray> Matrix2dArrayPtr;
WM3_REGISTER_STREAM(Matrix2dArray);
}

#endif
