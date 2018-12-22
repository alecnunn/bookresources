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
inline float Camera::GetDMin () const
{
    return m_fDMin;
}
//----------------------------------------------------------------------------
inline float Camera::GetDMax () const
{
    return m_fDMax;
}
//----------------------------------------------------------------------------
inline float Camera::GetUMin () const
{
    return m_fUMin;
}
//----------------------------------------------------------------------------
inline float Camera::GetUMax () const
{
    return m_fUMax;
}
//----------------------------------------------------------------------------
inline float Camera::GetRMin () const
{
    return m_fRMin;
}
//----------------------------------------------------------------------------
inline float Camera::GetRMax () const
{
    return m_fRMax;
}
//----------------------------------------------------------------------------
inline Vector3f Camera::GetLocation () const
{
    return Local.GetTranslate();
}
//----------------------------------------------------------------------------
inline Vector3f Camera::GetDVector () const
{
    return Local.GetRotate().GetColumn(0);
}
//----------------------------------------------------------------------------
inline Vector3f Camera::GetUVector () const
{
    return Local.GetRotate().GetColumn(1);
}
//----------------------------------------------------------------------------
inline Vector3f Camera::GetRVector () const
{
    return Local.GetRotate().GetColumn(2);
}
//----------------------------------------------------------------------------
inline Vector3f Camera::GetWorldLocation () const
{
    return World.GetTranslate();
}
//----------------------------------------------------------------------------
inline Vector3f Camera::GetWorldDVector () const
{
    return World.GetRotate().GetColumn(0);
}
//----------------------------------------------------------------------------
inline Vector3f Camera::GetWorldUVector () const
{
    return World.GetRotate().GetColumn(1);
}
//----------------------------------------------------------------------------
inline Vector3f Camera::GetWorldRVector () const
{
    return World.GetRotate().GetColumn(2);
}
//----------------------------------------------------------------------------
inline void Camera::SetPlaneState (unsigned int uiPlaneState)
{
    m_uiPlaneState = uiPlaneState;
}
//----------------------------------------------------------------------------
inline unsigned int Camera::GetPlaneState () const
{
    return m_uiPlaneState;
}
//----------------------------------------------------------------------------
inline int Camera::GetPlaneQuantity () const
{
    return m_iPlaneQuantity;
}
//----------------------------------------------------------------------------
inline const Plane3f* Camera::GetPlanes () const
{
    return m_akWPlane;
}
//----------------------------------------------------------------------------
