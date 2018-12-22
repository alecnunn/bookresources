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
#include <maya/MGlobal.h>
using namespace Wm3;

//----------------------------------------------------------------------------
void Exporter::Placeholder (const MFnDagNode& rkDagNode)
{
    // Add a Node to the Wild Magic scene whose name indicates the type of
    // the dag node.  This is just a placeholder until the function that
    // handles the dag node type is implemented.

    DisplayWarning(("Unsupported node: " + 
        rkDagNode.name()).asChar());

    Node* pkNode = new Node;
    const char* acDagType = "Placeholder: ";
    const char* acDagName = rkDagNode.name().asChar();
    char* acName = new char[strlen(acDagType)+strlen(acDagName)+1];
    strcpy(acName,acDagType);
    strcat(acName,acDagName);
    pkNode->SetName(acName);
    AttachToParent(rkDagNode,pkNode);
}
//----------------------------------------------------------------------------

