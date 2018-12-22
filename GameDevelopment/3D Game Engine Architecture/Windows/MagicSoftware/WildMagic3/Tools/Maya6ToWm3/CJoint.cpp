 // Magic Software, Inc.
// http://www.magic-software.com
// Copyright (c) 2000-2003.  All Rights Reserved
//
// Source code from Magic Software is supplied under the terms of a license
// agreement and may not be copied or disclosed except in accordance with the
// terms of that agreement.  The various license agreements may be found at
// the Magic Software web site.  This file is subject to the license
//
// 3D GAME ENGINE SOURCE CODE
// http://www.magic-software.com/License/3DGameEngine.pdf

#include "Exporter.h"
#include <maya/MDagPath.h>
using namespace Wm3;

//----------------------------------------------------------------------------
void Exporter::CJoint (const MFnDagNode& rkDagNode)
{
    // Joints are derived from Maya Transform objects.  Thus,
    // we'll create them as Nodes and special case the few additions
    // that Joints have within CTransform(...).
    CTransform(rkDagNode);

    // ...and then put their full dag path (instead of name()) into the
    // hashmap for deferred skin controller creation.  The full info is
    // needed because the node names might not be unique.

    const char* acName = rkDagNode.dagPath().fullPathName().asChar();

    // Must have at least a world and this node that just got added
    assert (m_kParentStack.GetQuantity() > 1);

    // Top of the parent stack will be the node that was just created in
    // CTransform(...).
    Spatial* pkSpat = m_kParentStack[m_kParentStack.GetQuantity()-1];
    if (pkSpat->IsDerived(Node::TYPE))
    {
        Node* pkNode = StaticCast<Node>(pkSpat);
        m_kJointHash.Insert(acName,pkNode);
    }
    else
    {
        DisplayError("Joint is not a node.  Not adding to joint hash.");
    }
}
//----------------------------------------------------------------------------

