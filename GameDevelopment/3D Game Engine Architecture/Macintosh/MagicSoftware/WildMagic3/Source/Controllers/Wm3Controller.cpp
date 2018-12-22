// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3Controller.h"
#include "Wm3Math.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,Controller,Object);
WM3_IMPLEMENT_ABSTRACT_STREAM(Controller);
WM3_IMPLEMENT_DEFAULT_NAME_ID(Controller,Object);

//----------------------------------------------------------------------------
Controller::Controller ()
{
    m_pkObject = 0;
    RepeatType = RT_CLAMP;
    MinTime = 0.0;
    MaxTime = 0.0;
    Phase = 0.0;
    Frequency = 1.0;
    Active = true;
    m_dLastAppTime = -DBL_MAX;
}
//----------------------------------------------------------------------------
Controller::~Controller ()
{
}
//----------------------------------------------------------------------------
double Controller::GetControlTime (double dAppTime)
{
    double dCtrlTime = Frequency*dAppTime + Phase;

    if ( RepeatType == RT_CLAMP )
    {
        if ( dCtrlTime < MinTime )
            return MinTime;
        if ( dCtrlTime > MaxTime )
            return MaxTime;
        return dCtrlTime;
    }

    double dRange = MaxTime - MinTime;
    if ( dRange > 0.0 )
    {
        double dMultiples = (dCtrlTime - MinTime)/dRange;
        double dIntTime = Mathd::Floor(dMultiples);
        double dFrcTime = dMultiples - dIntTime;
        if ( RepeatType == RT_WRAP )
            return MinTime + dFrcTime*dRange;

        // Repeat == RT_CYCLE
        if ( int(dIntTime) & 1 )
        {
            // backward time
            return MaxTime - dFrcTime*dRange;
        }
        else
        {
            // forward time
            return MinTime + dFrcTime*dRange;
        }
    }
    else
    {
        return MinTime;
    }
}
//----------------------------------------------------------------------------
bool Controller::Update (double dAppTime)
{
    if ( Active
    &&   (dAppTime == -Mathd::MAX_REAL || dAppTime != m_dLastAppTime) )
    {
        m_dLastAppTime = dAppTime;
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void Controller::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    Object::Load(rkStream,pkLink);

    // native data
    rkStream.Read(RepeatType);
    rkStream.Read(MinTime);
    rkStream.Read(MaxTime);
    rkStream.Read(Phase);
    rkStream.Read(Frequency);
    rkStream.Read(Active);
    // m_dLastAppTime is not streamed

    // link data
    Object* pkObject;
    rkStream.Read(pkObject);
    pkLink->Add(pkObject);

    WM3_END_DEBUG_STREAM_LOAD(Controller);
}
//----------------------------------------------------------------------------
void Controller::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Object::Link(rkStream,pkLink);

    Object* pkLinkID = pkLink->GetLinkID();
    m_pkObject = rkStream.GetFromMap(pkLinkID);
}
//----------------------------------------------------------------------------
bool Controller::Register (Stream& rkStream) const
{
    if ( !Object::Register(rkStream) )
        return false;

    if ( m_pkObject )
        m_pkObject->Register(rkStream);

    return true;
}
//----------------------------------------------------------------------------
void Controller::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    Object::Save(rkStream);

    // native data
    rkStream.Write(RepeatType);
    rkStream.Write(MinTime);
    rkStream.Write(MaxTime);
    rkStream.Write(Phase);
    rkStream.Write(Frequency);
    rkStream.Write(Active);

    // link data
    rkStream.Write(m_pkObject);

    WM3_END_DEBUG_STREAM_SAVE(Controller);
}
//----------------------------------------------------------------------------
StringTree* Controller::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(7,1);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("repeat type = ",RepeatType));
    pkTree->SetString(2,StringTree::Format("min time =",MinTime));
    pkTree->SetString(3,StringTree::Format("max time =",MaxTime));
    pkTree->SetString(4,StringTree::Format("phase =",Phase));
    pkTree->SetString(5,StringTree::Format("frequency =",Frequency));
    pkTree->SetString(6,StringTree::Format("active =",Active));

    // children
    pkTree->SetChild(0,Object::SaveStrings());

    // Object will iterate over controllers to save strings

    return pkTree;
}
//----------------------------------------------------------------------------
int Controller::GetMemoryUsed () const
{
    return sizeof(Controller) - sizeof(Object) + Object::GetMemoryUsed();
}
//----------------------------------------------------------------------------
int Controller::GetDiskUsed () const
{
    return Object::GetDiskUsed() +
        sizeof(RepeatType) +
        sizeof(MinTime) +
        sizeof(MaxTime) +
        sizeof(Phase) +
        sizeof(Frequency) +
        sizeof(char) +  // Active
        sizeof(m_pkObject);
}
//----------------------------------------------------------------------------
