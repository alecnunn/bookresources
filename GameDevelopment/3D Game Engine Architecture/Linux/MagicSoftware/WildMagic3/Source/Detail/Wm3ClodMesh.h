// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3CLODMESH_H
#define WM3CLODMESH_H

#include "Wm3CollapseRecord.h"
#include "Wm3TriMesh.h"

namespace Wm3
{

class WM3_ITEM ClodMesh : public TriMesh
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    // Construction and destruction.  ClodMesh accepts responsibility for
    // deleting the input arrays.
    ClodMesh (Vector3fArrayPtr spkVertices, IntArrayPtr spkIndices,
        bool bGenerateNormals, int iRecordQuantity, CollapseRecord* akRecord);

    virtual ~ClodMesh ();

    // LOD selection is based on manual selection by the application.  To
    // use distance from camera or screen space coverage, derive a class
    // from Wm3ClodMesh and override 'GetAutomatedTargetRecord'.
    int GetRecordQuantity () const;
    int& TargetRecord ();
    virtual int GetAutomatedTargetRecord ();

    // Geometric updates.  The Draw method will call this update and adjust
    // the TriMesh quantities according to the current value of the target
    // record.  You can call this manually in an application that does not
    // need to display the mesh.
    void SelectLevelOfDetail ();

protected:
    ClodMesh ();

    // drawing
    virtual void Draw (Renderer& rkRenderer, bool bNoCull = false);

    // selection of LOD
    int m_iCurrentRecord, m_iTargetRecord;
    int m_iRecordQuantity;
    CollapseRecord* m_akRecord;
};

WM3_REGISTER_STREAM(ClodMesh);
typedef Pointer<ClodMesh> ClodMeshPtr;
#include "Wm3ClodMesh.inl"

}

#endif
