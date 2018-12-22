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
inline Vector3f Light::GetLocation () const
{
    return Local.GetTranslate();
}
//----------------------------------------------------------------------------
inline Vector3f Light::GetDVector () const
{
    return Local.GetRotate().GetColumn(0);
}
//----------------------------------------------------------------------------
inline Vector3f Light::GetUVector () const
{
    return Local.GetRotate().GetColumn(1);
}
//----------------------------------------------------------------------------
inline Vector3f Light::GetRVector () const
{
    return Local.GetRotate().GetColumn(2);
}
//----------------------------------------------------------------------------
inline Vector3f Light::GetWorldLocation () const
{
    return World.GetTranslate();
}
//----------------------------------------------------------------------------
inline Vector3f Light::GetWorldDVector () const
{
    return World.GetRotate().GetColumn(0);
}
//----------------------------------------------------------------------------
inline Vector3f Light::GetWorldUVector () const
{
    return World.GetRotate().GetColumn(1);
}
//----------------------------------------------------------------------------
inline Vector3f Light::GetWorldRVector () const
{
    return World.GetRotate().GetColumn(2);
}
//----------------------------------------------------------------------------
