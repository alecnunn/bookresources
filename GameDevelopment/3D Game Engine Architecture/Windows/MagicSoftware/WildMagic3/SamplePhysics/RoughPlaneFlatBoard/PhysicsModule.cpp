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
    XLocExt = 0.0;
    YLocExt = 0.0;
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
PhysicsModule::~PhysicsModule ()
{
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

    double dXX = XLocExt*XLocExt;
    double dXY = XLocExt*YLocExt;
    double dYY = YLocExt*YLocExt;
    double dTmp1 = dXX + dYY;
    double dTmp2 = Mathd::Sqrt(dTmp1);
    double dTmp3 = 4.0*dXY/3.0;
    double dTmp4 = 0.5*Mathd::Log((dTmp2+XLocExt)/(dTmp2-XLocExt));
    double dTmp5 = 0.5*Mathd::Log((dTmp2+YLocExt)/(dTmp2-YLocExt));
    double dNumer = dTmp3*dTmp2 + XLocExt*dXX*dTmp5 + YLocExt*dYY*dTmp4;
    double dDenom = dTmp3*dTmp1;
    double dCoeff = MuGravity*dNumer/dDenom;

    double dAngSpeed = Mathd::FAbs(dThetaDer);
    if ( dAngSpeed > Mathd::EPSILON )
        m_dAngVelCoeff = dCoeff/dAngSpeed;
    else
        m_dAngVelCoeff = 0.0;
}
//----------------------------------------------------------------------------
void PhysicsModule::GetRectangle (double& rdX00, double& rdY00, double& rdX10,
    double& rdY10, double& rdX11, double& rdY11, double& rdX01, double& rdY01)
    const
{
    // P = (x,y) + sx*XLocExt*(cos(A),sin(A)) + sy*YLocExt*(-sin(A),cos(A))
    // where |sx| = 1 and |sy| = 1 (four choices on sign)

    double dCos = Mathd::Cos(m_dTheta);
    double dSin = Mathd::Sin(m_dTheta);

    // sx = -1, sy = -1
    rdX00 = m_dX - XLocExt*dCos + YLocExt*dSin;
    rdY00 = m_dY - XLocExt*dSin - YLocExt*dCos;

    // sx = +1, sy = -1
    rdX10 = m_dX + XLocExt*dCos + YLocExt*dSin;
    rdY10 = m_dY + XLocExt*dSin - YLocExt*dCos;

    // sx = +1, sy = +1
    rdX11 = m_dX + XLocExt*dCos - YLocExt*dSin;
    rdY11 = m_dY + XLocExt*dSin + YLocExt*dCos;

    // sx = -1, sy = +1
    rdX01 = m_dX - XLocExt*dCos - YLocExt*dSin;
    rdY01 = m_dY - XLocExt*dSin + YLocExt*dCos;
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
