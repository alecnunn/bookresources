// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3BILLBOARDNODE_H
#define WM3BILLBOARDNODE_H

#include "Wm3Node.h"

namespace Wm3
{

class Camera;

class WM3_ITEM BillboardNode : public Node
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    // The model space of the billboard has an up vector of (0,1,0) that is
    // chosen to be the billboard's axis of rotation.

    // construction
    BillboardNode (Camera* pkCamera = 0, int iQuantity = 1, int iGrowBy = 1);

    // the camera to which the billboard is aligned
    void AlignTo (Camera* pkCamera);

protected:
    // geometric updates
    virtual void UpdateWorldData (double dAppTime);

    Pointer<Camera> m_spkCamera;
};

WM3_REGISTER_STREAM(BillboardNode);
typedef Pointer<BillboardNode> BillboardNodePtr;
#include "Wm3BillboardNode.inl"

}

#endif
