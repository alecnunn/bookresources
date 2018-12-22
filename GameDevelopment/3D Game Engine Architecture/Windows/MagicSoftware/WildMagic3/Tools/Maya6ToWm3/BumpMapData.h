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

#ifndef BUMPMAPDATA_H
#define BUMPMAPDATA_H

// This class is a "struct" class to hold the data necessary to create a
// BumpMapEffect.  The reason this class is needed is because the creation
// of the effect is deferred until it is known if (and which) light needs
// to be used with the BumpMap.

#include "Wm3Image.h"
#include "Wm3Vector2Array.h"
#include "Wm3ColorRGBArray.h"
#include "Wm3TriMesh.h"
#include "Wm3Light.h"

#include <maya/MString.h>

using namespace Wm3;

class BumpMapData
{
public:
    ImagePtr Primary;
    Vector2fArrayPtr PrimaryUVs;
    ImagePtr NormalMap;
    Vector2fArrayPtr NormalUVs;
    ColorRGBArrayPtr Colors;

    // A handle to the trimesh this bumpmap should be attached to
    int MeshKey;

    bool Modulated;
    MString Name;
};

#endif