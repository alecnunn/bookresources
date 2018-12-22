// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3Polyline.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,Polyline,Geometry);
WM3_IMPLEMENT_STREAM(Polyline);
WM3_IMPLEMENT_DEFAULT_NAME_ID(Polyline,Geometry);

//----------------------------------------------------------------------------
Polyline::Polyline (Vector3fArrayPtr spkVertices, bool bClosed,
    bool bContiguous)
    :
    Geometry(spkVertices)
{
    int iVQuantity = Vertices->GetQuantity();
    assert( iVQuantity >= 2 );

    m_iActiveQuantity = iVQuantity;
    m_bClosed = bClosed;
    m_bContiguous = bContiguous;

    // support for drawing vertex arrays
    int* aiIndex = new int[iVQuantity];
    for (int i = 0; i < iVQuantity; i++)
        aiIndex[i] = i;
    Indices = new IntArray(iVQuantity,aiIndex);

    SetGeometryType();
}
//----------------------------------------------------------------------------
Polyline::Polyline ()
{
    m_iActiveQuantity = 0;
    m_bClosed = false;
    m_bContiguous = false;
    SetGeometryType();
}
//----------------------------------------------------------------------------
Polyline::~Polyline ()
{
}
//----------------------------------------------------------------------------
void Polyline::SetGeometryType ()
{
    if ( m_bContiguous )
    {
        if ( m_bClosed )
            GeometryType = GT_POLYLINE_CLOSED;
        else
            GeometryType = GT_POLYLINE_OPEN;
    }
    else
    {
        GeometryType = GT_POLYLINE_SEGMENTS;
    }
}
//----------------------------------------------------------------------------
void Polyline::SetActiveQuantity (int iActiveQuantity)
{
    int iVQuantity = Vertices->GetQuantity();
    if ( 0 <= iActiveQuantity && iActiveQuantity <= iVQuantity )
        m_iActiveQuantity = iActiveQuantity;
    else
        m_iActiveQuantity = iVQuantity;

    Indices->SetActiveQuantity(m_iActiveQuantity);
}
//----------------------------------------------------------------------------
void Polyline::SetClosed (bool bClosed)
{
    m_bClosed = bClosed;
    SetGeometryType();
}
//----------------------------------------------------------------------------
void Polyline::SetContiguous (bool bContiguous)
{
    m_bContiguous = bContiguous;
    SetGeometryType();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void Polyline::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    Geometry::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_iActiveQuantity);
    rkStream.Read(m_bClosed);
    rkStream.Read(m_bContiguous);

    SetGeometryType();

    WM3_END_DEBUG_STREAM_LOAD(Polyline);
}
//----------------------------------------------------------------------------
void Polyline::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Geometry::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool Polyline::Register (Stream& rkStream) const
{
    return Geometry::Register(rkStream);
}
//----------------------------------------------------------------------------
void Polyline::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    Geometry::Save(rkStream);

    // native data
    rkStream.Write(m_iActiveQuantity);
    rkStream.Write(m_bClosed);
    rkStream.Write(m_bContiguous);

    WM3_END_DEBUG_STREAM_SAVE(Polyline);
}
//----------------------------------------------------------------------------
StringTree* Polyline::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(4,1);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("active =",m_iActiveQuantity));
    pkTree->SetString(2,StringTree::Format("closed =",m_bClosed));
    pkTree->SetString(3,StringTree::Format("contiguous =",m_bContiguous));

    // children
    pkTree->SetChild(0,Geometry::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
int Polyline::GetMemoryUsed () const
{
    return sizeof(Polyline) - sizeof(Geometry) + Geometry::GetMemoryUsed();
}
//----------------------------------------------------------------------------
int Polyline::GetDiskUsed () const
{
    return Geometry::GetDiskUsed() +
        sizeof(m_iActiveQuantity) +
        sizeof(char) + // m_bClosed
        sizeof(char);  // m_bContiguous
}
//----------------------------------------------------------------------------
