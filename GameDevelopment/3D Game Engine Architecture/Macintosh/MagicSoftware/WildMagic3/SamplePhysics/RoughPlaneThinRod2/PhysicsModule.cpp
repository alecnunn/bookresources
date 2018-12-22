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
{
    MuGravity = 0.0;
    Length = 0.0;
    m_dTime = 0.0;
    m_dDeltaTime = 0.0;
    m_dX = 0.0;
    m_dXDer = 0.0;
    m_dY = 0.0;
    m_dYDer = 0.0;
    m_dTheta = 0.0;
    m_dThetaDer = 0.0;
}
//----------------------------------------------------------------------------
void PhysicsModule::Initialize (double dTime, double dDeltaTime, double dX,
    double dY, double dTheta, double dXDer, double dYDer, double dThetaDer)
{
    m_dTime = dTime;
    m_dDeltaTime = dDeltaTime;
    m_dX = dX;
    m_dY = dY;
    m_dTheta = dTheta;
    m_dXDer = dXDer;
    m_dYDer = dYDer;
    m_dThetaDer = dThetaDer;

    m_dX0 = dX;
    m_dY0 = dY;
    m_dTheta0 = dTheta;
    m_dXDer0 = dXDer;
    m_dYDer0 = dYDer;
    m_dThetaDer0 = dThetaDer;

    double dLinSpeed = Mathd::Sqrt(dXDer*dXDer+dYDer*dYDer);
    if ( dLinSpeed > Mathd::EPSILON )
        m_dLinVelCoeff = MuGravity/dLinSpeed;
    else
        m_dLinVelCoeff = 0.0;

    double dAngSpeed = Mathd::FAbs(dThetaDer);
    if ( dAngSpeed > Mathd::EPSILON )
        m_dAngVelCoeff = 3.0*MuGravity/(dAngSpeed*Length);
    else
        m_dAngVelCoeff = 0.0;

    m_dHalfLength = 0.5*Length;
}
//----------------------------------------------------------------------------
void PhysicsModule::Get (double& rdX1, double& rdY1, double& rdX2,
    double& rdY2) const
{
    double dCos = Mathd::Cos(m_dTheta);
    double dSin = Mathd::Sin(m_dTheta);
    rdX1 = m_dX + m_dHalfLength*dCos;
    rdY1 = m_dY + m_dHalfLength*dSin;
    rdX2 = m_dX - m_dHalfLength*dCos;
    rdY2 = m_dY - m_dHalfLength*dSin;
}
//----------------------------------------------------------------------------
void PhysicsModule::Update ()
{
    m_dTime += m_dDeltaTime;

    double dLTmp = m_dLinVelCoeff*m_dTime;
    double dLinVelMult = 1.0 - dLTmp;
    if ( dLinVelMult > 0.0 )
    {
        m_dXDer = dLinVelMult*m_dXDer0;
        m_dYDer = dLinVelMult*m_dYDer0;
        m_dX = m_dX0 + m_dTime*(1.0-0.5*dLTmp)*m_dXDer0;
        m_dY = m_dY0 + m_dTime*(1.0-0.5*dLTmp)*m_dYDer0;
    }
    else
    {
        m_dXDer = 0.0;
        m_dYDer = 0.0;
    }

    double dATmp = m_dAngVelCoeff*m_dTime;
    double dAngVelMult = 1.0 - dATmp;
    if ( dAngVelMult > 0.0 )
    {
        m_dThetaDer = dAngVelMult*m_dThetaDer0;
        m_dTheta = m_dTheta0 + m_dTime*(1.0-0.5*dATmp)*m_dThetaDer0;
    }
    else
    {
        m_dThetaDer = 0.0;
    }
}
//----------------------------------------------------------------------------
