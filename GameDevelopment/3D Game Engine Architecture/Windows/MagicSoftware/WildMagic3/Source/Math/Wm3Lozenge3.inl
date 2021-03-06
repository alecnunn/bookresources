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
Lozenge3<Real>::Lozenge3 ()
{
    // uninitialized
}
//----------------------------------------------------------------------------
template <class Real>
Lozenge3<Real>::Lozenge3 (const Rectangle3<Real>& rkRectangle, Real fRadius)
    :
    Rectangle(rkRectangle)
{
    Radius = fRadius;
}
//----------------------------------------------------------------------------
