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

#include "Wm3Vector2.h"

class PhysicsModule
{
public:
    // construction and destruction
    PhysicsModule ();
    ~PhysicsModule ();

    // initialize the system
    void Initialize (double dTime, double dDeltaTime,
        const Wm3::Vector2d& rkInitPos, const Wm3::Vector2d& rkInitVel);

    // take a single step of the simulation
    void Update ();

    // access the current state
    double GetTime () const;
    double GetDeltaTime () const;
    const Wm3::Vector2d& GetPosition () const;
    const Wm3::Vector2d& GetVelocity () const;
    double GetFrequency () const;

    // physical constants
    double SpringConstant;  // c
    double Mass;  // m

private:
    void Evaluate ();

    // state variables
    double m_dTime, m_dDeltaTime;
    Wm3::Vector2d m_kPosition, m_kVelocity;

    // auxiliary variables
    Wm3::Vector2d m_kInitPos;  // initial position
    double m_dFrequency;  // sqrt(c/m)
    Wm3::Vector2d m_kVelDivFreq;  // initial_velocity/frequency
};

#include "PhysicsModule.inl"

#endif
