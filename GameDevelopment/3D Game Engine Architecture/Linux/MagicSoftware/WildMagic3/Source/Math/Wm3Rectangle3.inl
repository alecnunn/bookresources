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
Rectangle3<Real>::Rectangle3 ()
{
    // uninitialized
}
//----------------------------------------------------------------------------
template <class Real>
Rectangle3<Real>::Rectangle3 (const Vector3<Real>& rkCenter,
    const Vector3<Real>* akAxis, const Real* afExtent)
    :
    Center(rkCenter)
{
    for (int i = 0; i < 2; i++)
    {
        Axis[i] = akAxis[i];
        Extent[i] = afExtent[i];
    }
}
//----------------------------------------------------------------------------
template <class Real>
Rectangle3<Real>::Rectangle3 (const Vector3<Real>& rkCenter,
    const Vector3<Real>& rkAxis0, const Vector3<Real>& rkAxis1,
    Real fExtent0, Real fExtent1)
    :
    Center(rkCenter)
{
    Axis[0] = rkAxis0;
    Axis[1] = rkAxis1;
    Extent[0] = fExtent0;
    Extent[1] = fExtent1;
}
//----------------------------------------------------------------------------
template <class Real>
void Rectangle3<Real>::ComputeVertices (Vector3<Real> akVertex[4]) const
{
    Vector3<Real> akEAxis[2] =
    {
        Extent[0]*Axis[0],
        Extent[1]*Axis[1]
    };

    akVertex[0] = Center - akEAxis[0] - akEAxis[1];
    akVertex[1] = Center + akEAxis[0] - akEAxis[1];
    akVertex[2] = Center + akEAxis[0] + akEAxis[1];
    akVertex[3] = Center - akEAxis[0] + akEAxis[1];
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> Rectangle3<Real>::GetPPCorner () const
{
    return Center + Extent[0]*Axis[0] + Extent[1]*Axis[1];
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> Rectangle3<Real>::GetPMCorner () const
{
    return Center + Extent[0]*Axis[0] - Extent[1]*Axis[1];
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> Rectangle3<Real>::GetMPCorner () const
{
    return Center - Extent[0]*Axis[0] + Extent[1]*Axis[1];
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> Rectangle3<Real>::GetMMCorner () const
{
    return Center - Extent[0]*Axis[0] - Extent[1]*Axis[1];
}
//----------------------------------------------------------------------------
