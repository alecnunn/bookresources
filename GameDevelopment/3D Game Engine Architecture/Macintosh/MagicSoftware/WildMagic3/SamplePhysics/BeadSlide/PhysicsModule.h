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
    void Initialize (double dTime, double dDeltaTime, double dQ,
        double dQDot);

    // take a single step of the solver
    void Update ();

    // access the current state
    double GetTime () const;
    double GetDeltaTime () const;
    double GetQ () const;
    double GetQDot () const;

    // physical constants
    double Gravity;
    double Mass;

private:
    // state and auxiliary variables
    double m_dTime, m_dDeltaTime, m_dQ, m_dQDer;
    double m_adState[2], m_adAux[1];

    // ODE solver (specific solver assigned in the cpp file)
    Wm3::OdeSolverd* m_pkSolver;
    static void OdeFunction (double dTime, const double* adState,
        void* pvData, double* adFValue);
};

#include "PhysicsModule.inl"

#endif
