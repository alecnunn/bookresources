// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3IMAGES_H
#define WM3IMAGES_H

#include "Wm3Element.h"
#include "Wm3TImage.h"
#include "Wm3TImage2D.h"
#include "Wm3TImage3D.h"

namespace Wm3
{

typedef TImage<Echar>     ImageChar;
typedef TImage<Euchar>    ImageUChar;
typedef TImage<Eshort>    ImageShort;
typedef TImage<Eushort>   ImageUShort;
typedef TImage<Eint>      ImageInt;
typedef TImage<Euint>     ImageUInt;
typedef TImage<Elong>     ImageLong;
typedef TImage<Eulong>    ImageULong;
typedef TImage<Efloat>    ImageFloat;
typedef TImage<Edouble>   ImageDouble;
typedef TImage<Ergb5>     ImageRGB5;
typedef TImage<Ergb8>     ImageRGB8;

typedef TImage2D<Echar>   ImageChar2D;
typedef TImage2D<Euchar>  ImageUChar2D;
typedef TImage2D<Eshort>  ImageShort2D;
typedef TImage2D<Eushort> ImageUShort2D;
typedef TImage2D<Eint>    ImageInt2D;
typedef TImage2D<Euint>   ImageUInt2D;
typedef TImage2D<Elong>   ImageLong2D;
typedef TImage2D<Eulong>  ImageULong2D;
typedef TImage2D<Efloat>  ImageFloat2D;
typedef TImage2D<Edouble> ImageDouble2D;
typedef TImage2D<Ergb5>   ImageRGB52D;
typedef TImage2D<Ergb8>   ImageRGB82D;

typedef TImage3D<Echar>   ImageChar3D;
typedef TImage3D<Euchar>  ImageUChar3D;
typedef TImage3D<Eshort>  ImageShort3D;
typedef TImage3D<Eushort> ImageUShort3D;
typedef TImage3D<Eint>    ImageInt3D;
typedef TImage3D<Euint>   ImageUInt3D;
typedef TImage3D<Elong>   ImageLong3D;
typedef TImage3D<Eulong>  ImageULong3D;
typedef TImage3D<Efloat>  ImageFloat3D;
typedef TImage3D<Edouble> ImageDouble3D;
typedef TImage3D<Ergb5>   ImageRGB53D;
typedef TImage3D<Ergb8>   ImageRGB83D;

}

#endif
