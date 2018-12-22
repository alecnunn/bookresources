// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3FLOATARRAY_H
#define WM3FLOATARRAY_H

#include "Wm3TSharedArray.h"

namespace Wm3
{
typedef TSharedArray<float> FloatArray;
typedef Pointer<FloatArray> FloatArrayPtr;
WM3_REGISTER_STREAM(FloatArray);
}

#endif
