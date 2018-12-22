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
inline double PhysicsModule::GetTime () const
{
    return m_dTime;
}
//----------------------------------------------------------------------------
inline double PhysicsModule::GetDeltaTime () const
{
    return m_dDeltaTime;
}
//----------------------------------------------------------------------------
inline const Wm3::Vector2d& PhysicsModule::GetPosition () const
{
    return m_kPosition;
}
//----------------------------------------------------------------------------
inline const Wm3::Vector2d& PhysicsModule::GetVelocity () const
{
    return m_kVelocity;
}
//----------------------------------------------------------------------------
inline double PhysicsModule::GetFrequency () const
{
    return m_dFrequency;
}
//----------------------------------------------------------------------------
