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

#ifndef MESHDATA_H
#define MESHDATA_H

#include "Wm3IntArray.h"
#include <maya/MString.h>
using namespace Wm3;

// This class is a "struct" class to hold data about meshes.  This data
// contains the mappings from "new vertices" to original Maya vertices.

class MeshData
{
public:
    MeshData();
    MeshData(int iNumOrigVert, IntArrayPtr spkVertMap);
    ~MeshData();

    MeshData& operator= (const MeshData& rkData);

    int GetNumOrigVerts();
    int GetMapping(int iIndex);

protected:
    // The number of vertices in the original mesh, before any splitting
    int m_iNumOrigVert;

    // A mapping from original vertices to old vertices.
    // if akVert is an array of vertices, then akVert[i] maps to
    // index m_aiVertMap[i-m_iNumOrigVert].  If i < m_iNumOrigVert, then
    // akVert[i] is an original vertex.
    IntArrayPtr m_spkVertMap;
};

#endif