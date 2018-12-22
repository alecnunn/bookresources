// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef LOADER3DS_H
#define LOADER3DS_H

#include "Stream3ds.h"
#include "Material3ds.h"

#include "Wm3Node.h"
#include "Wm3Light.h"
#include "Wm3Vector2Array.h"
#include "Wm3Vector3Array.h"
#include "Wm3IntArray.h"
#include "Wm3Transformation.h"
#include "Wm3KeyframeController.h"
using namespace Wm3;

enum
{
    // color chunks
    L3DS_COL_RGBFLOAT = 0x0010,
    L3DS_COL_RGBINT = 0x0011,
    L3DS_COL_RGBLININT = 0x0012,
    L3DS_COL_RGBLINFLOAT = 0x0013,

    // percent chunk
    L3DS_INT_PERCENT = 0x0030,
    L3DS_FLOAT_PERCENT = 0x0031,

    // primary chunk
    L3DS_PRIMARY = 0x4D4D,

    // main chunks
    L3DS_EDIT = 0x3D3D,
    L3DS_KEYF = 0xB000,

    // subchunks of L3DS_EDIT
    L3DS_EDIT_MAT = 0xAFFF,
    L3DS_EDIT_OBJ = 0x4000,

    // subchunks of L3DS_EDIT_MAT
    L3DS_EDIT_MAT_NAME = 0xA000,
    L3DS_EDIT_MAT_AMBIENT = 0xA010,
    L3DS_EDIT_MAT_DIFFUSE = 0xA020,
    L3DS_EDIT_MAT_SPECULAR = 0xA030,
    L3DS_EDIT_MAT_SHINY = 0xA040,
    L3DS_EDIT_MAT_TWOSIDE = 0xA081,
    L3DS_EDIT_MAT_TEX = 0xA200,

    // subchunks of L3DS_EDIT_MAT_TEX
    L3DS_EDIT_MAT_TEX_NAME = 0xA300,
    L3DS_EDIT_MAT_TEX_TILING = 0xA351,
    L3DS_EDIT_MAT_TEX_BLUR = 0xA353,
    L3DS_EDIT_MAT_TEX_USCALE = 0xA354,
    L3DS_EDIT_MAT_TEX_VSCALE = 0xA356,
    L3DS_EDIT_MAT_TEX_UOFFSET = 0xA358,
    L3DS_EDIT_MAT_TEX_VOFFSET = 0xA35A,

    // subchunks of L3DS_EDIT_OBJ
    L3DS_EDIT_OBJ_MESH = 0x4100,
    L3DS_EDIT_OBJ_LIGHT = 0x4600,

    // subchunks of L3DS_EDIT_OBJ_LIGHT
    L3DS_EDIT_OBJ_LIGHT_OFF = 0x4620,
    L3DS_EDIT_OBJ_LIGHT_SPOT = 0x4610,

    // subchunks of L3DS_EDIT_OBJ_MESH
    L3DS_EDIT_OBJ_MESH_VERTEXL = 0x4110,
    L3DS_EDIT_OBJ_MESH_FACES = 0x4120,
    L3DS_EDIT_OBJ_MESH_MAT = 0x4130,
    L3DS_EDIT_OBJ_MESH_UVS = 0x4140,
    L3DS_EDIT_OBJ_MESH_SMOOTH = 0x4150,
    L3DS_EDIT_OBJ_MESH_WORLD = 0x4160,
    L3DS_EDIT_OBJ_MESH_VISIBLE = 0x4165,

    // subchunks of L3DS_KEYF
    L3DS_KEYF_FRAMES = 0xB008,
    L3DS_KEYF_OBJDES = 0xB002,
    L3DS_KEYF_LIGHTDES = 0xB007,

    // subchunks of L3DS_KEYF_OBJDES
    L3DS_KEYF_OBJDES_HIER = 0xB010,
    L3DS_KEYF_OBJDES_NAME = 0xB011,
    L3DS_KEYF_OBJDES_PIVOT = 0xB013,
    L3DS_KEYF_OBJDES_POS = 0xB020,
    L3DS_KEYF_OBJDES_ROT = 0xB021,
    L3DS_KEYF_OBJDES_SCL = 0xB022,
    L3DS_KEYF_OBJDES_ID = 0xB030
};

class Loader3ds
{
public:
    Loader3ds ();
    bool LoadFile (const char* acFilename);
    NodePtr LoadSceneGraph(bool bWarn = false);
    void SetWarn (bool bWarn);
    void SetNoKeyframes (bool bNoKeyf);

private:
    // warnings (printf style)
    void Warn (const char* acFormat, ...);

    // chunk reading functions
    void ReadUnknownChunk ();

    void ReadEditChunk ();
    void ReadMaterialChunk ();
    TexturePtr ReadTextureChunk (float& rfUScale, float& rfVScale,
        float& rfUOffset, float& rfVOffset);
    ColorRGBA ReadColorChunks (const ColorRGBA& rkDefault);
    float ReadPercentageChunks (const float fDefault);
    void ReadObjectChunk ();
    LightPtr ReadObjLightChunk ();
    SpatialPtr ReadObjMeshChunk ();
    void ReadObjMatChunk (int iNextPos, int iNumFaces, int iNumVertices,
        int*& raiMaterials, bool& rbMultiMaterial);
    void ReadKeyfChunk ();
    void ReadObjDesChunk (TArray<NodePtr>& rkHier, int iStart, int iEnd);
    void ReadKeyfFlagChunk ();
    void ReadKeyfHierPosChunk (KeyframeControllerPtr& rspkCon,
        NodePtr spkPNode, NodePtr spkCNode, SpatialPtr spkCurrGeom);
    void ReadKeyfHierRotChunk (KeyframeControllerPtr& rspkCon,
        NodePtr spkPNode, NodePtr spkCNode, SpatialPtr spkCurrGeom);
    void ReadKeyfHierSclChunk (KeyframeControllerPtr& rspkCon,
        NodePtr spkPNode, NodePtr spkCNode, SpatialPtr spkCurrGeom);

    // convenience functions
    NodePtr SplitMesh (Vector3fArrayPtr spkVertices, IntArrayPtr spkFaces,
        int* aiMatGroup, Vector2fArrayPtr spkUVs, Transformation& rkTrans);
    TriMeshPtr CloneTriMesh (const TriMesh* pkOrig);

    // input stream
    Stream3ds m_kStream;

    // output node
    NodePtr m_spkRootNode;

    // If this is 'true', Warn will print to the console, otherwise it will
    // be silent.
    bool m_bDoWarn;

    // Some models have keyframe data that do not load.  This will load the
    // first frame as the transform without actually having keyframe
    // controllers.
    bool m_bNoKeyf;

    // Hash tables and arrays for objects that are read once, but referenced
    // in many places.  The hash tables store indices into the arrays.
    TArray<SpatialPtr> m_aspkMeshArray;
    TArray<LightPtr> m_aspkLightArray;
    TArray<Material3ds> m_aspkMatArray;
    THashTable<String,int> m_kMeshNameHash;
    THashTable<String,int> m_kLightNameHash;
    THashTable<String,int> m_kMatNameHash;
};

#endif
