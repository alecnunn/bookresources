// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "BloodCellController.h"
#include "Wm3Math.h"
#include "Wm3Particles.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,BloodCellController,ParticleController);
WM3_IMPLEMENT_STREAM(BloodCellController);

//----------------------------------------------------------------------------
BloodCellController::BloodCellController ()
{
}
//----------------------------------------------------------------------------
void BloodCellController::UpdatePointMotion (float)
{
    Particles* pkParticle = StaticCast<Particles>(m_pkObject);

    unsigned int uiVMax = pkParticle->Locations->GetQuantity();
    Vector3f* akLocation = pkParticle->Locations->GetData();
    float* afSize = pkParticle->Sizes->GetData();
    for (unsigned int uiV = 0; uiV < uiVMax; uiV++)
    {
        akLocation[uiV].X() += 0.01f*Mathf::SymmetricRandom();
        if ( akLocation[uiV].X() > 1.0f )
            akLocation[uiV].X() = 1.0f;
        else if ( akLocation[uiV].X() < -1.0f )
            akLocation[uiV].X() = -1.0f;

        akLocation[uiV].Y() += 0.01f*Mathf::SymmetricRandom();
        if ( akLocation[uiV].Y() > 1.0f )
            akLocation[uiV].Y() = 1.0f;
        else if ( akLocation[uiV].Y() < -1.0f )
            akLocation[uiV].Y() = -1.0f;

        akLocation[uiV].Z() += 0.01f*Mathf::SymmetricRandom();
        if ( akLocation[uiV].Z() > 1.0f )
            akLocation[uiV].Z() = 1.0f;
        else if ( akLocation[uiV].Z() < -1.0f )
            akLocation[uiV].Z() = -1.0f;

        afSize[uiV] *= (1.0f + 0.01f*Mathf::SymmetricRandom());
        if ( afSize[uiV] > 0.25f )
            afSize[uiV] = 0.25f;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* BloodCellController::GetObjectByName (const String& rkName)
{
    return ParticleController::GetObjectByName(rkName);
}
//----------------------------------------------------------------------------
void BloodCellController::GetAllObjectsByName (const String& rkName,
    TArray<Object*>& rkObjects)
{
    ParticleController::GetAllObjectsByName(rkName,rkObjects);
}
//----------------------------------------------------------------------------
Object* BloodCellController::GetObjectByID (unsigned int uiID)
{
    return ParticleController::GetObjectByID(uiID);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void BloodCellController::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    ParticleController::Load(rkStream,pkLink);

    WM3_END_DEBUG_STREAM_LOAD(BloodCellController);
}
//----------------------------------------------------------------------------
void BloodCellController::Link (Stream& rkStream, Stream::Link* pkLink)
{
    ParticleController::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool BloodCellController::Register (Stream& rkStream) const
{
    return ParticleController::Register(rkStream);
}
//----------------------------------------------------------------------------
void BloodCellController::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    ParticleController::Save(rkStream);

    WM3_END_DEBUG_STREAM_SAVE(BloodCellController);
}
//----------------------------------------------------------------------------
StringTree* BloodCellController::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(1,1);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));

    // children
    pkTree->SetChild(0,ParticleController::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
int BloodCellController::GetMemoryUsed () const
{
    return sizeof(BloodCellController) - sizeof(ParticleController) +
        ParticleController::GetMemoryUsed();
}
//----------------------------------------------------------------------------
int BloodCellController::GetDiskUsed () const
{
    return ParticleController::GetDiskUsed();
}
//----------------------------------------------------------------------------
