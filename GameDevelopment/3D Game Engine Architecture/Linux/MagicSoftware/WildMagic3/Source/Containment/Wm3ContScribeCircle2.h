// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3CONTSCRIBECIRCLE2_H
#define WM3CONTSCRIBECIRCLE2_H

#include "Wm3Circle2.h"
#include "Wm3LinearSystem.h"

namespace Wm3
{

// All functions return 'true' if circle has been constructed, 'false'
// otherwise (input points are linearly dependent).

// circle containing three 2D points
template <class Real> WM3_ITEM
bool Circumscribe (const Vector2<Real>& rkV0, const Vector2<Real>& rkV1,
    const Vector2<Real>& rkV2, Circle2<Real>& rkCircle);

// circle inscribing triangle of three 2D points
template <class Real> WM3_ITEM
bool Inscribe (const Vector2<Real>& rkV0, const Vector2<Real>& rkV1,
    const Vector2<Real>& rkV2, Circle2<Real>& rkCircle);

}

#endif
