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
using namespace Wm3;

//----------------------------------------------------------------------------
void Exporter::CWorld (const MFnDagNode& rkDagNode)
{
    // root node of scene

    // Can only have one root node
    if ( m_spkWMScene )
    {
        DisplayError("Scene already created before this world node!"\
            "  Ignoring this world node.");
        return;
    }

    int iChildQuantity = (int)rkDagNode.childCount();
    if (iChildQuantity <= 0)
        DisplayError("Root node has no children.");

    // Set up some state
    bool m_bParentVisible = true;
    bool m_bTexturesEnabled = true;
    bool m_bMaterialsEnabled = true;

    Node* pkNode =  new Node(iChildQuantity);
    pkNode->SetName(rkDagNode.name().asChar());
    m_spkWMScene = pkNode;
    AttachToParent(rkDagNode,pkNode);
}
//----------------------------------------------------------------------------

