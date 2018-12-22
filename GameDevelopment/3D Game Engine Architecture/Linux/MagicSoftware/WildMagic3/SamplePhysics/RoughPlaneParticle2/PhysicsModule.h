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

#include "Wm3OdeSolver.h"

class PhysicsModule
{
public:
    // construction and destruction
    PhysicsModule ();
    ~PhysicsModule ();

    // initialize the differential equation solver
    void Initialize (double dTime, double dDeltaTime, double dX1, double dY1,
        double dX2, double dY2, double dXDot, double dYDot, double dThetaDot);

    // access the current state
    double GetTime () const;
    double GetDeltaTime () const;
    double GetX () const;
    double GetXDot () const;
    double GetY () const;
    double GetYDot () const;
    double GetTheta () const;
    double GetThetaDot () const;
    void Get (double& rdX1, double& rdY1, double& rdX2, double& rdY2) const;

    // take a single step of the solver
    void Update ();

    // physical constants // symbols used in the Game Physics book
    double Gravity;               // g
    double Mass1, Mass2;          // m1, m2
    double Friction1, Friction2;  // c1, c2

protected:
    // state and auxiliary variables
    double m_dTime, m_dDeltaTime;
    double m_adState[6], m_adAux[7];
    double m_dLength1, m_dLength2;

    // ODE solver (specific solver assigned in the cpp file)
    Wm3::OdeSolverd* m_pkSolver;
    static void OdeFunction (double dTime, const double* adState,
        void* pvData, double* adFValue);
};

#include "PhysicsModule.inl"

#endif
