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
using namespace Wm3;

//----------------------------------------------------------------------------
PhysicsModule::PhysicsModule (int iNumParticles, int iNumSprings, float fStep,
    float fViscosity)
    :
    MassSpringArbitrary3f(iNumParticles,iNumSprings,fStep)
{
    m_fViscosity = fViscosity;
}
//----------------------------------------------------------------------------
PhysicsModule::~PhysicsModule ()
{
}
//----------------------------------------------------------------------------
Vector3f PhysicsModule::ExternalAcceleration (int i, float, const Vector3f*,
    const Vector3f* akVelocity)
{
    Vector3f kAcceleration = -m_fViscosity*akVelocity[i];
    return kAcceleration;
}
//----------------------------------------------------------------------------
