// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "PhysicsModule.h"
#include "Wm3Math.h"
using namespace Wm3;

//----------------------------------------------------------------------------
PhysicsModule::PhysicsModule ()
    :
    m_kPosition(Vector2d::ZERO),
    m_kVelocity(Vector2d::ZERO)
{
    SpringConstant = 0.0;
    Mass = 0.0;
    m_dTime = 0.0;
    m_dDeltaTime = 0.0;
    m_dFrequency = 0.0;
}
//----------------------------------------------------------------------------
PhysicsModule::~PhysicsModule ()
{
}
//----------------------------------------------------------------------------
void PhysicsModule::Evaluate ()
{
    double dArg = m_dFrequency*m_dTime;
    double dSin = Mathd::Sin(dArg);
    double dCos = Mathd::Cos(dArg);
    m_kPosition = dCos*m_kInitPos + dSin*m_kVelDivFreq;
    m_kVelocity = (m_kVelDivFreq*dCos - m_kInitPos*dSin)*m_dFrequency;
}
//----------------------------------------------------------------------------
void PhysicsModule::Initialize (double dTime, double dDeltaTime,
    const Vector2d& rkInitPos, const Vector2d& rkInitVel)
{
    m_dTime = dTime;
    m_dDeltaTime = dDeltaTime;
    m_kInitPos = rkInitPos;
    m_dFrequency = Mathd::Sqrt(SpringConstant/Mass);
    m_kVelDivFreq = rkInitVel/m_dFrequency;

    Evaluate();
}
//----------------------------------------------------------------------------
void PhysicsModule::Update ()
{
    m_dTime += m_dDeltaTime;
    Evaluate();
}
//----------------------------------------------------------------------------
