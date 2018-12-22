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
Cone3<Real>::Cone3 ()
{
    // uninitialized
}
//----------------------------------------------------------------------------
template <class Real>
Cone3<Real>::Cone3 (const Vector3<Real>& rkVertex,
    const Vector3<Real>& rkAxis, Real fAngle)
    :
    Vertex(rkVertex),
    Axis(rkAxis)
{
    CosAngle = Math<Real>::Cos(fAngle);
    SinAngle = Math<Real>::Sin(fAngle);
}
//----------------------------------------------------------------------------
template <class Real>
Cone3<Real>::Cone3 (const Vector3<Real>& rkVertex,
    const Vector3<Real>& rkAxis, Real fCosAngle, Real fSinAngle)
    :
    Vertex(rkVertex),
    Axis(rkAxis)
{
    CosAngle = fCosAngle;
    SinAngle = fSinAngle;
}
//----------------------------------------------------------------------------
