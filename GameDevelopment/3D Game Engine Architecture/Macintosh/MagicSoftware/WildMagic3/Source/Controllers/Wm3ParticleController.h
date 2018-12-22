// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3PARTICLECONTROLLER_H
#define WM3PARTICLECONTROLLER_H

#include "Wm3Controller.h"
#include "Wm3Particles.h"

namespace Wm3
{

class WM3_ITEM ParticleController : public Controller
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    virtual ~ParticleController ();

    // The system motion, in local coordinates.  The velocity vectors should
    // be unit length.
    float SystemLinearSpeed;
    float SystemAngularSpeed;
    Vector3f SystemLinearAxis;
    Vector3f SystemAngularAxis;

    // Point motion, in the model space of the system.  The velocity vectors
    // should be unit length.  In applications where the points represent a
    // rigid body, you might choose the origin of the system to be the center
    // of mass of the points and the coordinate axes to correspond to the
    // principal directions of the inertia tensor.
    float* PointLinearSpeed ();
    Vector3f* PointLinearAxis ();

    // size change parameters
    float SystemSizeChange;
    float* PointSizeChange ();

    // animation update
    virtual bool Update (double dAppTime);

protected:
    // streaming support
    ParticleController ();

    // for deferred allocation of the point motion arrays
    void Reallocate (int iVertexQuantity);
    virtual void SetObject (Object* pkObject);

    // update the motion parameters
    virtual void UpdateSystemMotion (float fCtrlTime);
    virtual void UpdatePointMotion (float fCtrlTime);

    // point motion (in model space of system)
    int m_iLQuantity;
    float* m_afPointLinearSpeed;
    Vector3f* m_akPointLinearAxis;

    // size change parameters
    float* m_afPointSizeChange;
};

WM3_REGISTER_STREAM(ParticleController);
typedef Pointer<ParticleController> ParticleControllerPtr;
#include "Wm3ParticleController.inl"

}

#endif
