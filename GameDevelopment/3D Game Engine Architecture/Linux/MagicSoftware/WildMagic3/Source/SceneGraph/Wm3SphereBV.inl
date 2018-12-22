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
inline int SphereBV::GetBVType () const
{
    return BoundingVolume::BV_SPHERE;
}
//----------------------------------------------------------------------------
inline void SphereBV::SetCenter (const Vector3f& rkCenter)
{
    m_kSphere.Center = rkCenter;
}
//----------------------------------------------------------------------------
inline void SphereBV::SetRadius (float fRadius)
{
    m_kSphere.Radius = fRadius;
}
//----------------------------------------------------------------------------
inline Vector3f SphereBV::GetCenter () const
{
    return m_kSphere.Center;
}
//----------------------------------------------------------------------------
inline float SphereBV::GetRadius () const
{
    return m_kSphere.Radius;
}
//----------------------------------------------------------------------------
inline Sphere3f& SphereBV::Sphere ()
{
    return m_kSphere;
}
//----------------------------------------------------------------------------
inline const Sphere3f& SphereBV::GetSphere () const
{
    return m_kSphere;
}
//----------------------------------------------------------------------------
