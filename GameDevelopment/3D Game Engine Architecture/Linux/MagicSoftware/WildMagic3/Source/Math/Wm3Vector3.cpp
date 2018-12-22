// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3Vector3.h"
using namespace Wm3;

template<> const Vector3<float> Vector3<float>::ZERO(0.0f,0.0f,0.0f);
template<> const Vector3<float> Vector3<float>::UNIT_X(1.0f,0.0f,0.0f);
template<> const Vector3<float> Vector3<float>::UNIT_Y(0.0f,1.0f,0.0f);
template<> const Vector3<float> Vector3<float>::UNIT_Z(0.0f,0.0f,1.0f);

template<> const Vector3<double> Vector3<double>::ZERO(0.0,0.0,0.0);
template<> const Vector3<double> Vector3<double>::UNIT_X(1.0,0.0,0.0);
template<> const Vector3<double> Vector3<double>::UNIT_Y(0.0,1.0,0.0);
template<> const Vector3<double> Vector3<double>::UNIT_Z(0.0,0.0,1.0);
