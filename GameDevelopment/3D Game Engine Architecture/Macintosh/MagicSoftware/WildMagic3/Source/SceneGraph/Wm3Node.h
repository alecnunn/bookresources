// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3NODE_H
#define WM3NODE_H

#include "Wm3Spatial.h"

namespace Wm3
{

class WM3_ITEM Node : public Spatial
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    // construction and destruction
    Node (int iQuantity = 1, int iGrowBy = 1);
    virtual ~Node ();

    // children
    int GetQuantity () const;
    int GetUsed () const;
    int AttachChild (Spatial* pkChild);
    int DetachChild (Spatial* pkChild);
    SpatialPtr DetachChildAt (int i);
    SpatialPtr SetChild (int i, Spatial* pkChild);
    SpatialPtr GetChild (int i);

    // Picking support.  The origin and direction of the ray must be in world
    // coordinates.  The application is responsible for deleting the pick
    // records in the array.
    virtual void DoPick (const Ray3f& rkRay, PickArray& rkResults);

protected:
    // geometric updates
    virtual void UpdateWorldData (double dAppTime);
    virtual void UpdateWorldBound ();

    // render state updates
    virtual void UpdateState (TStack<GlobalState*>* akGStack,
        TStack<Light*>* pkLStack);

    // children
    TArray<SpatialPtr> m_kChild;
    int m_iUsed;

// internal use
public:
    // Drawing.  The Renderer needs to call this when the Node has a global
    // effect attached to it.
    virtual void Draw (Renderer& rkRenderer, bool bNoCull = false);
};

WM3_REGISTER_STREAM(Node);
typedef Pointer<Node> NodePtr;
#include "Wm3Node.inl"

}

#endif
