// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3Matrix3.h"
using namespace Wm3;

template<> const Matrix3<float> Matrix3<float>::ZERO(
    0.0f,0.0f,0.0f,
    0.0f,0.0f,0.0f,
    0.0f,0.0f,0.0f);
template<> const Matrix3<float> Matrix3<float>::IDENTITY(
    1.0f,0.0f,0.0f,
    0.0f,1.0f,0.0f,
    0.0f,0.0f,1.0f);

template<> const Matrix3<double> Matrix3<double>::ZERO(
    0.0,0.0,0.0,
    0.0,0.0,0.0,
    0.0,0.0,0.0);
template<> const Matrix3<double> Matrix3<double>::IDENTITY(
    1.0,0.0,0.0,
    0.0,1.0,0.0,
    0.0,0.0,1.0);
