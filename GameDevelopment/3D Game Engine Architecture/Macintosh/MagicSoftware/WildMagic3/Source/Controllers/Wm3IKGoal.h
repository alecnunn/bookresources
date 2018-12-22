// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3IKGOAL_H
#define WM3IKGOAL_H

#include "Wm3Spatial.h"

namespace Wm3
{

class WM3_ITEM IKGoal : public Object
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    IKGoal (Spatial* pkTarget, Spatial* pkEffector, float fWeight);

    SpatialPtr GetTarget ();
    SpatialPtr GetEffector ();
    float Weight;  // default = 1.0f

    Vector3f GetTargetPosition () const;
    Vector3f GetEffectorPosition () const;

protected:
    IKGoal ();

    SpatialPtr m_spkTarget;
    SpatialPtr m_spkEffector;
};

WM3_REGISTER_STREAM(IKGoal);
typedef Pointer<IKGoal> IKGoalPtr;
#include "Wm3IKGoal.inl"

}

#endif
