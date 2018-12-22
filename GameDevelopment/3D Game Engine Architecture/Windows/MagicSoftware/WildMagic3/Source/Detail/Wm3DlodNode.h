// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3DLODNODE_H
#define WM3DLODNODE_H

#include "Wm3SwitchNode.h"

namespace Wm3
{

class WM3_ITEM DlodNode : public SwitchNode
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    // construction
    DlodNode (int iQuantity = 1, int iGrowBy = 1);

    // center for level of detail
    Vector3f& ModelCenter ();
    const Vector3f& GetModelCenter () const;
    const Vector3f& GetWorldCenter () const;

    // distance intervals for children
    void SetModelDistance (int i, float fMinDist, float fMaxDist);
    float GetModelMinDistance (int i) const;
    float GetModelMaxDistance (int i) const;
    float GetWorldMinDistance (int i) const;
    float GetWorldMaxDistance (int i) const;

protected:
    // switch the child based on distance from world LOD center to camera
    void SelectLevelOfDetail (const Camera* pkCamera);

    // drawing
    virtual void Draw (Renderer& rkRenderer, bool bNoCull = false);

    // point whose distance to camera determines correct child
    Vector3f m_kModelLodCenter;
    Vector3f m_kWorldLodCenter;

    // squared distances for each LOD interval
    TArray<float> m_kModelMinDist;
    TArray<float> m_kModelMaxDist;
    TArray<float> m_kWorldMinDist;
    TArray<float> m_kWorldMaxDist;
};

WM3_REGISTER_STREAM(DlodNode);
typedef Pointer<DlodNode> DlodNodePtr;
#include "Wm3DlodNode.inl"

}

#endif
