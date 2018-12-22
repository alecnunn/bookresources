// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef PHYSICSMODULE_H
#define PHYSICSMODULE_H

#include "Wm3System.h"

class PhysicsModule
{
public:
    // construction and destruction
    PhysicsModule ();
    ~PhysicsModule ();

    // initialize the differential equation solver
    void Initialize (double dTime, double dDeltaTime, double dX, double dY,
        double dTheta, double dXDer, double dYDer, double dThetaDer);

    // access the current state
    double GetTime () const;
    double GetDeltaTime () const;
    double GetX () const;
    double GetXDer () const;
    double GetY () const;
    double GetYDer () const;
    double GetTheta () const;
    double GetThetaDer () const;

    // get rectangle corners in counterclockwise order
    void GetRectangle (
        double& rdX00, double& rdY00,
        double& rdX10, double& rdY10,
        double& rdX11, double& rdY11,
        double& rdX01, double& rdY01) const;

    // take a single step of the solver
    void Update ();

    // physical constants   // symbols used in the Game Physics book
    double MuGravity;       // mu*g
    double XLocExt;         // alpha0
    double YLocExt;         // beta0

protected:
    // state and auxiliary variables
    double m_dTime, m_dDeltaTime;
    double m_dX, m_dY, m_dTheta, m_dXDer, m_dYDer, m_dThetaDer;
    double m_dX0, m_dY0, m_dTheta0, m_dXDer0, m_dYDer0, m_dThetaDer0;
    double m_dLinVelCoeff, m_dAngVelCoeff;
};

#include "PhysicsModule.inl"

#endif
