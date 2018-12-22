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
inline double PhysicsModule::GetTheta () const
{
    return m_adState[0];
}
//----------------------------------------------------------------------------
inline double PhysicsModule::GetThetaDot () const
{
    return m_adState[1];
}
//----------------------------------------------------------------------------
inline double PhysicsModule::GetRadius () const
{
    return m_adState[2];
}
//----------------------------------------------------------------------------
inline double PhysicsModule::GetRadiusDot () const
{
    return m_adState[3];
}
//----------------------------------------------------------------------------
inline double PhysicsModule::GetEccentricity () const
{
    return m_dEccentricity;
}
//----------------------------------------------------------------------------
inline double PhysicsModule::GetMajorAxis () const
{
    return m_dMajorAxis;
}
//----------------------------------------------------------------------------
inline double PhysicsModule::GetMinorAxis () const
{
    return m_dMinorAxis;
}
//----------------------------------------------------------------------------
