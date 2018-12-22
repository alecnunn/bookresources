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
Sphere3<Real>::Sphere3 ()
{
    // uninitialized
}
//----------------------------------------------------------------------------
template <class Real>
Sphere3<Real>::Sphere3 (const Vector3<Real>& rkCenter, Real fRadius)
    :
    Center(rkCenter),
    Radius(fRadius)
{
}
//----------------------------------------------------------------------------
template <class Real>
Sphere3<Real>::Sphere3 (const Sphere3& rkSphere)
    :
    Center(rkSphere.Center),
    Radius(rkSphere.Radius)
{
}
//----------------------------------------------------------------------------
template <class Real>
Sphere3<Real>& Sphere3<Real>::operator= (const Sphere3& rkSphere)
{
    Center = rkSphere.Center;
    Radius = rkSphere.Radius;
    return *this;
}
//----------------------------------------------------------------------------
