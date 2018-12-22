// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3GEOMETRY_H
#define WM3GEOMETRY_H

#include "Wm3Spatial.h"
#include "Wm3Effect.h"
#include "Wm3IntArray.h"
#include "Wm3Vector3Array.h"

namespace Wm3
{

class WM3_ITEM Geometry : public Spatial
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    // abstract base class
    virtual ~Geometry ();

    // member access
    Vector3fArrayPtr Vertices;
    Vector3fArrayPtr Normals;
    BoundingVolumePtr ModelBound;
    IntArrayPtr Indices;

    // geometric updates
    virtual void UpdateMS (bool bUpdateNormals = true);

    // Picking support. This nested class exists only to maintain the
    // class-derivation chain that is parallel to the one whose base is
    // Spatial.
    class WM3_ITEM PickRecord : public Spatial::PickRecord
    {
    protected:
        PickRecord (Geometry* pkIObject, float fT);
    };

protected:
    Geometry ();
    Geometry (Vector3fArrayPtr spkVertices);

    // geometric updates
    virtual void UpdateModelBound ();
    virtual void UpdateModelNormals ();
    virtual void UpdateWorldBound ();

    // render state updates
    virtual void UpdateState (TStack<GlobalState*>* akGStack,
        TStack<Light*>* pkLStack);

    // drawing
    virtual void Draw (Renderer& rkRenderer, bool bNoCull = false);

// internal use
public:
    // Render state and lights in path to this object.  An attached effect
    // provides additional render state, lights, and any other information
    // needed to draw the object.

    enum // GeometryType
    {
        GT_POLYPOINT,
        GT_POLYLINE_SEGMENTS,
        GT_POLYLINE_OPEN,
        GT_POLYLINE_CLOSED,
        GT_TRIMESH,
        GT_MAX_QUANTITY
    };

    int GeometryType;
    GlobalStatePtr States[GlobalState::MAX_STATE];
    TArray<Pointer<Light> > Lights;
};

WM3_REGISTER_STREAM(Geometry);
typedef Pointer<Geometry> GeometryPtr;
#include "Wm3Geometry.inl"

}

#endif
