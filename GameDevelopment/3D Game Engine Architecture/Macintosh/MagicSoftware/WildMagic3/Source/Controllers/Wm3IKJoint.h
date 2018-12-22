// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3IKJOINT_H
#define WM3IKJOINT_H

#include "Wm3IKGoal.h"

namespace Wm3
{

class Spatial;

class WM3_ITEM IKJoint : public Object
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    // Construction and destruction.  IKJoint assumes responsibility for the
    // input array of pointers to goals and will delete it.  The array should
    // therefore be dynamically allocated.  The goals themselves are managed
    // by the IKController and will be deleted by it.
    IKJoint (Spatial* pkObject, int iGoalQuantity, IKGoalPtr* aspkGoal);
    virtual ~IKJoint ();

    // index i is for the joint's parent's world axis[i]
    bool AllowTranslation[3];
    float MinTranslation[3];
    float MaxTranslation[3];
    bool AllowRotation[3];
    float MinRotation[3];
    float MaxRotation[3];

protected:
    // streaming
    IKJoint ();

    // support for the IK update
    friend class IKController;

    // the managed object
    Spatial* m_pkObject;

    // joint update
    Vector3f GetAxis (int i);
    void UpdateWorldSRT ();
    void UpdateWorldRT ();
    bool UpdateLocalT (int i);
    bool UpdateLocalR (int i);

    // the goals affected by this joint
    int m_iGoalQuantity;
    IKGoalPtr* m_aspkGoal;
};

WM3_REGISTER_STREAM(IKJoint);
typedef Pointer<IKJoint> IKJointPtr;

}

#endif
