// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3SWITCHNODE_H
#define WM3SWITCHNODE_H

#include "Wm3Node.h"

namespace Wm3
{

class WM3_ITEM SwitchNode : public Node
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    SwitchNode (int iQuantity = 1, int iGrowBy = 1);

    enum { SN_INVALID_CHILD = -1 };

    void SetActiveChild (int iActiveChild);
    int GetActiveChild () const;
    void DisableAllChildren ();

    // Picking support.  The origin and direction of the ray must be in world
    // coordinates.  The application is responsible for deleting the pick
    // records in the array.  This function only picks on the active child.
    // If there is a demand to support inactive children, a data member must
    // be added to indicate whether or not the caller wants active-only
    // or all children to be picked.
    virtual void DoPick (const Ray3f& rkRay, PickArray& rkResults);

protected:
    virtual void Draw (Renderer& rkRenderer, bool bNoCull = false);

    int m_iActiveChild;
};

WM3_REGISTER_STREAM(SwitchNode);
typedef Pointer<SwitchNode> SwitchNodePtr;
#include "Wm3SwitchNode.inl"

}

#endif
