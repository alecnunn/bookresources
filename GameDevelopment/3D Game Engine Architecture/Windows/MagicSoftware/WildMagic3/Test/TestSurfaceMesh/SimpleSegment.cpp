// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "SimpleSegment.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,SimpleSegment,CurveSegment);
WM3_IMPLEMENT_STREAM(SimpleSegment);

//----------------------------------------------------------------------------
SimpleSegment::SimpleSegment ()
    :
    CurveSegment(0.0f,1.0f)
{
    Amplitude = 1.0f;
    Frequency = Mathf::TWO_PI;
    Height = 1.0f;
}
//----------------------------------------------------------------------------
SimpleSegment::~SimpleSegment ()
{
}
//----------------------------------------------------------------------------
Vector3f SimpleSegment::P (float fU) const
{
    // P(u) = (A*cos(F*u),A*sin(F*u),H*u)
    float fAngle = Frequency*fU;
    return Vector3f(Amplitude*Mathf::Cos(fAngle),Amplitude*Mathf::Sin(fAngle),
        Height*fU);
}
//----------------------------------------------------------------------------
Vector3f SimpleSegment::PU (float fU) const
{
    // P'(u) = (-A*F*sin(F*u),A*F*cos(F*u),H)
    float fAF = Amplitude*Frequency;
    float fAngle = Frequency*fU;
    return Vector3f(-fAF*Mathf::Sin(fAngle),fAF*Mathf::Cos(fAngle),Height);
}
//----------------------------------------------------------------------------
Vector3f SimpleSegment::PUU (float fU) const
{
    // P"(u) = (-A*F*F*cos(F*u),-A*F*F*sin(F*u),0)
    float fAFF = Amplitude*Frequency*Frequency;
    float fAngle = Frequency*fU;
    return Vector3f(-fAFF*Mathf::Cos(fAngle),-fAFF*Mathf::Sin(fAngle),0.0f);
}
//----------------------------------------------------------------------------
Vector3f SimpleSegment::PUUU (float fU) const
{
    // P"'(u) = (A*F*F*F*sin(F*u),-A*F*F*F*cos(F*u),0)
    float fAFFF = Amplitude*Frequency*Frequency*Frequency;
    float fAngle = Frequency*fU;
    return Vector3f(fAFFF*Mathf::Sin(fAngle),-fAFFF*Mathf::Cos(fAngle),0.0f);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* SimpleSegment::GetObjectByName (const String& rkName)
{
    return CurveSegment::GetObjectByName(rkName);
}
//----------------------------------------------------------------------------
void SimpleSegment::GetAllObjectsByName (const String& rkName,
    TArray<Object*>& rkObjects)
{
    CurveSegment::GetAllObjectsByName(rkName,rkObjects);
}
//----------------------------------------------------------------------------
Object* SimpleSegment::GetObjectByID (unsigned int uiID)
{
    return CurveSegment::GetObjectByID(uiID);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void SimpleSegment::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    CurveSegment::Load(rkStream,pkLink);
    rkStream.Read(Amplitude);
    rkStream.Read(Frequency);
    rkStream.Read(Height);

    WM3_END_DEBUG_STREAM_LOAD(SimpleSegment);
}
//----------------------------------------------------------------------------
void SimpleSegment::Link (Stream& rkStream, Stream::Link* pkLink)
{
    CurveSegment::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool SimpleSegment::Register (Stream& rkStream) const
{
    return CurveSegment::Register(rkStream);
}
//----------------------------------------------------------------------------
void SimpleSegment::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    CurveSegment::Save(rkStream);
    rkStream.Write(Amplitude);
    rkStream.Write(Frequency);
    rkStream.Write(Height);

    WM3_END_DEBUG_STREAM_SAVE(SimpleSegment);
}
//----------------------------------------------------------------------------
StringTree* SimpleSegment::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(4,1);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("amplitude =",Amplitude));
    pkTree->SetString(2,StringTree::Format("frequency =",Frequency));
    pkTree->SetString(3,StringTree::Format("height =",Height));

    // children
    pkTree->SetChild(0,CurveSegment::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
int SimpleSegment::GetMemoryUsed () const
{
    return sizeof(SimpleSegment) - sizeof(CurveSegment) +
        CurveSegment::GetMemoryUsed();
}
//----------------------------------------------------------------------------
int SimpleSegment::GetDiskUsed () const
{
    return CurveSegment::GetDiskUsed() +
        sizeof(Amplitude) +
        sizeof(Frequency) +
        sizeof(Height);
}
//----------------------------------------------------------------------------
