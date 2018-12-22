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
Circle3<Real>::Circle3 ()
{
    // uninitialized
}
//----------------------------------------------------------------------------
template <class Real>
Circle3<Real>::Circle3 (const Vector3<Real>& rkCenter,
    const Vector3<Real>& rkU, const Vector3<Real>& rkV,
    const Vector3<Real>& rkN, Real fRadius)
    :
    Center(rkCenter),
    U(rkU),
    V(rkV),
    N(rkN)
{
    Radius = fRadius;
}
//----------------------------------------------------------------------------
