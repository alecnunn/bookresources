// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

//----------------------------------------------------------------------------
template <class Real>
Arc2<Real>::Arc2 ()
{
    // uninitialized
}
//----------------------------------------------------------------------------
template <class Real>
Arc2<Real>::Arc2 (const Vector2<Real>& rkCenter, Real fRadius,
    const Vector2<Real>& rkEnd0, const Vector2<Real>& rkEnd1)
    :
    Circle2<Real>(rkCenter,fRadius),
    End0(rkEnd0),
    End1(rkEnd1)
{
}
//----------------------------------------------------------------------------
template <class Real>
bool Arc2<Real>::Contains (const Vector2<Real>& rkP) const
{
    // Assert: |P-C| = R where P is the input point, C is the circle center,
    // and R is the circle radius.  For P to be on the arc from A to B, it
    // must be on the side of the plane containing A with normal N = Perp(B-A)
    // where Perp(u,v) = (v,-u).

    Vector2<Real> kPmE0 = rkP - End0;
    Vector2<Real> kE1mE0 = End1 - End0;
    Real fDotPerp = kPmE0.DotPerp(kE1mE0);
    return fDotPerp >= (Real)0.0;
}
//----------------------------------------------------------------------------
