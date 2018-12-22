// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3COLORRGBARRAY_H
#define WM3COLORRGBARRAY_H

#include "Wm3TSharedArray.h"
#include "Wm3ColorRGB.h"

namespace Wm3
{
typedef TSharedArray<ColorRGB> ColorRGBArray;
typedef Pointer<ColorRGBArray> ColorRGBArrayPtr;
WM3_REGISTER_STREAM(ColorRGBArray);
}

#endif
