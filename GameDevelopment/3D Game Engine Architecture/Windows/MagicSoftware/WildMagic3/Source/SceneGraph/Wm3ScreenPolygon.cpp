// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3ScreenPolygon.h"
#include "Wm3Renderer.h"
#include "Wm3ZBufferState.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,ScreenPolygon,TriMesh);
WM3_IMPLEMENT_STREAM(ScreenPolygon);
WM3_IMPLEMENT_DEFAULT_NAME_ID(ScreenPolygon,TriMesh);

//----------------------------------------------------------------------------
ScreenPolygon::ScreenPolygon (int iVQuantity, Vector2f* akVertex,
    bool bForeground)
    :
    TriMesh(
        new Vector3fArray(iVQuantity,new Vector3f[iVQuantity]),
        new IntArray(3*(iVQuantity-2),new int[3*(iVQuantity-2)]),
        false)
{
    // assert:  The vertices form a convex polygon and are listed in
    // counterclockwise order.
    assert( iVQuantity >= 3 && akVertex );

    m_akVertex = akVertex;
    m_bForeground = bForeground;

    // construct the 3D vertices
    Vector3f* akSVertex = Vertices->GetData();
    for (int i = 0; i < iVQuantity; i++)
    {
        akSVertex[i].X() = m_akVertex[i].X();
        akSVertex[i].Y() = m_akVertex[i].Y();
        akSVertex[i].Z() = ( m_bForeground ? 0.0f : 1.0f );
    }

    // represent the screen polygon as a triangle fan
    int iTQuantity = iVQuantity - 2;
    int* aiIndex = Indices->GetData();
    for (int i0 = 1, i1 = 2, j = 0; i0 <= iTQuantity; i0++, i1++)
    {
        aiIndex[j++] = 0;
        aiIndex[j++] = i0;
        aiIndex[j++] = i1;
    }

    // disable z-buffering
    ZBufferState* pkZS = new ZBufferState;
    pkZS->Enabled = false;
    pkZS->Writable = false;
    SetGlobalState(pkZS);
    UpdateRS();
}
//----------------------------------------------------------------------------
ScreenPolygon::ScreenPolygon ()
{
    m_akVertex = 0;
    m_bForeground = false;
}
//----------------------------------------------------------------------------
ScreenPolygon::~ScreenPolygon ()
{
    delete[] m_akVertex;
}
//----------------------------------------------------------------------------
void ScreenPolygon::SetVertex (int i, const Vector2f& rkV)
{
    assert( 0 <= i && i < Vertices->GetQuantity() );
    m_akVertex[i] = rkV;

    Vector3f* akSVertex = Vertices->GetData();
    akSVertex[i].X() = rkV.X();
    akSVertex[i].Y() = rkV.Y();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void ScreenPolygon::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    Object::Load(rkStream,pkLink);

    // native data
    int iVQuantity;
    rkStream.Read(iVQuantity);
    m_akVertex = new Vector2f[iVQuantity];
    rkStream.Read(iVQuantity,m_akVertex);
    rkStream.Read(m_bForeground);

    WM3_END_DEBUG_STREAM_LOAD(ScreenPolygon);
}
//----------------------------------------------------------------------------
void ScreenPolygon::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Object::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool ScreenPolygon::Register (Stream& rkStream) const
{
    return Object::Register(rkStream);
}
//----------------------------------------------------------------------------
void ScreenPolygon::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    Object::Save(rkStream);

    // native data
    int iVQuantity = Vertices->GetQuantity();
    rkStream.Write(iVQuantity);
    rkStream.Write(iVQuantity,m_akVertex);
    rkStream.Write(m_bForeground);

    WM3_END_DEBUG_STREAM_SAVE(ScreenPolygon);
}
//----------------------------------------------------------------------------
StringTree* ScreenPolygon::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(2,2);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("foreground = ",m_bForeground));

    // children
    pkTree->SetChild(0,Geometry::SaveStrings());
    pkTree->SetChild(1,StringTree::Format("vertices",Vertices->GetQuantity(),
        m_akVertex));

    return pkTree;
}
//----------------------------------------------------------------------------
int ScreenPolygon::GetMemoryUsed () const
{
    return sizeof(ScreenPolygon) - sizeof(Object) + Object::GetMemoryUsed() +
        Vertices->GetQuantity()*sizeof(m_akVertex[0]);
}
//----------------------------------------------------------------------------
int ScreenPolygon::GetDiskUsed () const
{
    return Object::GetDiskUsed() +
        Vertices->GetQuantity()*sizeof(m_akVertex[0]) +
        sizeof(char);  // m_bForeground
}
//----------------------------------------------------------------------------
