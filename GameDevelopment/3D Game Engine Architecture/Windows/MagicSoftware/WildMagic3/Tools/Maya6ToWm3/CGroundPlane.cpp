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
void Exporter::CGroundPlane (const MFnDagNode& rkDagNode)
{
    // TO DO. This object type does not show up in the Maya SDK.  The *.ma
    // files appear to ignore the node.  However, in a data set I tested, the
    // Transform parent was not the identity matrix.  So, not certain what to
    // do with this.  Just create a dummy node for now.

    // Ignore for now
    DisplayInfo("Ignoring Ground Plane.");
    return;

    assert( rkDagNode.childCount() == 0 );

    Node* pkNode = new Node;
    const char* acDagType = "GroundPlane: ";
    const char* acDagName = rkDagNode.name().asChar();
    char* acName = new char[strlen(acDagType)+strlen(acDagName)+1];
    strcpy(acName,acDagType);
    strcat(acName,acDagName);
    pkNode->SetName(acName);

    AttachToParent(rkDagNode,pkNode);
}
//----------------------------------------------------------------------------

