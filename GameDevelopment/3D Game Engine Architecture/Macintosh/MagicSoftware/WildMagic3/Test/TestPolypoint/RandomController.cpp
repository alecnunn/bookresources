// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "RandomController.h"
#include "Wm3Math.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,RandomController,PointController);
WM3_IMPLEMENT_STREAM(RandomController);

//----------------------------------------------------------------------------
RandomController::RandomController ()
{
}
//----------------------------------------------------------------------------
void RandomController::UpdatePointMotion (float)
{
    Polypoint* pkPoints = StaticCast<Polypoint>(m_pkObject);

    unsigned int uiVMax = pkPoints->Vertices->GetQuantity();
    Vector3f* akVertex = pkPoints->Vertices->GetData();
    for (unsigned int uiV = 0; uiV < uiVMax; uiV++)
    {
        akVertex[uiV].X() += 0.01f*Mathf::SymmetricRandom();
        if ( akVertex[uiV].X() > 1.0f )
            akVertex[uiV].X() = 1.0f;
        else if ( akVertex[uiV].X() < -1.0f )
            akVertex[uiV].X() = -1.0f;

        akVertex[uiV].Y() += 0.01f*Mathf::SymmetricRandom();
        if ( akVertex[uiV].Y() > 1.0f )
            akVertex[uiV].Y() = 1.0f;
        else if ( akVertex[uiV].Y() < -1.0f )
            akVertex[uiV].Y() = -1.0f;

        akVertex[uiV].Z() += 0.01f*Mathf::SymmetricRandom();
        if ( akVertex[uiV].Z() > 1.0f )
            akVertex[uiV].Z() = 1.0f;
        else if ( akVertex[uiV].Z() < -1.0f )
            akVertex[uiV].Z() = -1.0f;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* RandomController::GetObjectByName (const String& rkName)
{
    return PointController::GetObjectByName(rkName);
}
//----------------------------------------------------------------------------
void RandomController::GetAllObjectsByName (const String& rkName,
    TArray<Object*>& rkObjects)
{
    PointController::GetAllObjectsByName(rkName,rkObjects);
}
//----------------------------------------------------------------------------
Object* RandomController::GetObjectByID (unsigned int uiID)
{
    return PointController::GetObjectByID(uiID);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void RandomController::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    PointController::Load(rkStream,pkLink);

    WM3_END_DEBUG_STREAM_LOAD(RandomController);
}
//----------------------------------------------------------------------------
void RandomController::Link (Stream& rkStream, Stream::Link* pkLink)
{
    PointController::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool RandomController::Register (Stream& rkStream) const
{
    return PointController::Register(rkStream);
}
//----------------------------------------------------------------------------
void RandomController::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    PointController::Save(rkStream);

    WM3_END_DEBUG_STREAM_SAVE(RandomController);
}
//----------------------------------------------------------------------------
StringTree* RandomController::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(1,1);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));

    // children
    pkTree->SetChild(0,PointController::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
int RandomController::GetMemoryUsed () const
{
    return sizeof(RandomController) - sizeof(PointController) +
        PointController::GetMemoryUsed();
}
//----------------------------------------------------------------------------
int RandomController::GetDiskUsed () const
{
    return PointController::GetDiskUsed();
}
//----------------------------------------------------------------------------
