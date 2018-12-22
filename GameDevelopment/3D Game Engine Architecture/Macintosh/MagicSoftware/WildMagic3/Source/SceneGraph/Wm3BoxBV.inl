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
inline int BoxBV::GetBVType () const
{
    return BoundingVolume::BV_BOX;
}
//----------------------------------------------------------------------------
inline void BoxBV::SetCenter (const Vector3f& rkCenter)
{
    m_kBox.Center = rkCenter;
}
//----------------------------------------------------------------------------
inline Vector3f BoxBV::GetCenter () const
{
    return m_kBox.Center;
}
//----------------------------------------------------------------------------
inline Box3f& BoxBV::Box ()
{
    return m_kBox;
}
//----------------------------------------------------------------------------
inline const Box3f& BoxBV::GetBox () const
{
    return m_kBox;
}
//----------------------------------------------------------------------------
