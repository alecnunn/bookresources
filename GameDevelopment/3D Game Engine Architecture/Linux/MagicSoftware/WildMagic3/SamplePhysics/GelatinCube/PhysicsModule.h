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

#include "Wm3MassSpringVolume.h"

class PhysicsModule : public Wm3::MassSpringVolume3f
{
public:
    // construction and destruction
    PhysicsModule (int iSlices, int iRows, int iCols, float fStep,
        float fViscosity);

    virtual ~PhysicsModule ();

    float& Viscosity ();

    // external acceleration is due to viscous forces
    virtual Wm3::Vector3f ExternalAcceleration (int i, float fTime,
        const Wm3::Vector3f* akPosition, const Wm3::Vector3f* akVelocity);

protected:
    float m_fViscosity;
};

#endif
