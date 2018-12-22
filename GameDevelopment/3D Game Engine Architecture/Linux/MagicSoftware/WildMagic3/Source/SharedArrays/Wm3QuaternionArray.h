// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3QUATERNIONARRAY_H
#define WM3QUATERNIONARRAY_H

#include "Wm3TSharedArray.h"
#include "Wm3Quaternion.h"

namespace Wm3
{
typedef TSharedArray<Quaternionf> QuaternionfArray;
typedef Pointer<QuaternionfArray> QuaternionfArrayPtr;
WM3_REGISTER_STREAM(QuaternionfArray);

typedef TSharedArray<Quaterniond> QuaterniondArray;
typedef Pointer<QuaterniondArray> QuaterniondArrayPtr;
WM3_REGISTER_STREAM(QuaterniondArray);
}

#endif
