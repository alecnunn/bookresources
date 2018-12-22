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
Capsule3<Real>::Capsule3 ()
{
    // uninitialized
}
//----------------------------------------------------------------------------
template <class Real>
Capsule3<Real>::Capsule3 (const Segment3<Real>& rkSegment, Real fRadius)
    :
    Segment(rkSegment)
{
    Radius = fRadius;
}
//----------------------------------------------------------------------------
