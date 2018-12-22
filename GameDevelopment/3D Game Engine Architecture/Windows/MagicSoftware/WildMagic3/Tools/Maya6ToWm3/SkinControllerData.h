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

#ifndef SKINCONTROLLERDATA_H
#define SKINCONTROLLERDATA_H

// This class is a "struct" class to hold the data necessary to create a
// Skin Controller.  It has everything except the bones list.  In its
// place is an array of Strings which will be used to match up with
// the joint list.

#include "Wm3TriMesh.h"
#include "Wm3String.h"

using namespace Wm3;

class SkinControllerData
{
public:
    SkinControllerData();
    ~SkinControllerData();

    // Data handles
    TriMesh* Mesh;
    int VertexQuantity;
    int BoneQuantity;
    float** Weight;
    Vector3f** Offset;

    // Needs to be deleted
    String* Bones;
};

#endif