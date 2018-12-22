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
#include "Wm3Matrix3.h"

class PhysicsModule
{
public:
    // construction and destruction
    PhysicsModule ();
    ~PhysicsModule ();

    // Initialize the differential equation solver.  The inputs dTheta, dPhi,
    // and dPsi determine the body coordinate axes Xi1, Xi2, and Xi3.  The
    // angular velocity inputs are the coefficients in the body coordinate
    // system.
    void Initialize (double dTime, double dDeltaTime, double dTheta,
        double dPhi, double dPsi, double dAngVel1, double dAngVel2,
        double dAngVel3);

    // take a single step of the solver
    void Update ();

    double GetTime () const;
    double GetDeltaTime () const;
    double GetTheta () const;
    double GetPhi () const;
    double GetPsi () const;

    // Get the body coordinate axes in world coordinates.  The axes are
    // stored as the columns of a rotation matrix.
    Wm3::Matrix3f GetBodyAxes () const;

    // physical constants
    double Gravity;
    double Mass;
    double Length;
    double Inertia1, Inertia3;  // Inertia2 = Inertia1

private:
    // state and auxiliary variables
    double m_dTime, m_dDeltaTime;
    double m_adState[3], m_adAux[5];

    // ODE solver (specific solver assigned in the cpp file)
    Wm3::OdeSolverd* m_pkSolver;
    static void OdeFunction (double dTime, const double* adState,
        void* pvData, double* adFValue);
};

#include "PhysicsModule.inl"

#endif
