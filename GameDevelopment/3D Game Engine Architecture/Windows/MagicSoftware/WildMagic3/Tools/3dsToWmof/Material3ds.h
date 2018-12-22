// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef MATERIAL3DS_H
#define MATERIAL3DS_H

// This class is more or less a struct to hold the data members that a 3ds
// file associates with a "material" together to make it convenient for WM to
// split those into MaterialState, CullState, and Effect when it needs to.

#include "Wm3MaterialState.h"
#include "Wm3Texture.h"
#include "Wm3TriMesh.h"
#include "Wm3Vector2Array.h"
using namespace Wm3;

class Material3ds
{
public:
    Material3ds ();
    Material3ds (bool bTwoSided, MaterialStatePtr spkMat, TexturePtr spkTex,
        float fUScale, float fVScale, float fUOffset, float fVOffset);
    ~Material3ds ();

    // Attach all of the current material state to this TriMesh.  Since
    // TextureEffects are stored with the material here and with the geometry
    // in the file, they must be passed in.
    void AttachStateTo (TriMeshPtr spkGeom, Vector2fArrayPtr spkUVs = 0);

    // culling state parameters
    bool m_bTwoSided;

    // material state parameters
    MaterialStatePtr m_spkMat;
    
    // texture effect parameters
    TexturePtr m_spkTex;
    float m_fUOffset, m_fVOffset, m_fUScale, m_fVScale;
};

#endif
