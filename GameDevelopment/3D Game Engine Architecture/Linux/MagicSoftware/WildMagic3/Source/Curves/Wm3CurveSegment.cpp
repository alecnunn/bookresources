// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3CurveSegment.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,CurveSegment,Object);
WM3_IMPLEMENT_ABSTRACT_STREAM(CurveSegment);
WM3_IMPLEMENT_DEFAULT_NAME_ID(CurveSegment,Object);

//----------------------------------------------------------------------------
CurveSegment::CurveSegment (float fUMin, float fUMax)
{
    assert( fUMin < fUMax );

    m_fUMin = fUMin;
    m_fUMax = fUMax;
}
//----------------------------------------------------------------------------
CurveSegment::CurveSegment ()
{
    m_fUMin = 0.0f;
    m_fUMax = 0.0f;
}
//----------------------------------------------------------------------------
CurveSegment::~CurveSegment ()
{
}
//----------------------------------------------------------------------------
Vector3f CurveSegment::Tangent (float fU) const
{
    Vector3f kVelocity = PU(fU);
    kVelocity.Normalize();
    return kVelocity;
}
//----------------------------------------------------------------------------
Vector3f CurveSegment::Normal (float fU) const
{
    Vector3f kVelocity = PU(fU);
    Vector3f kAcceleration = PUU(fU);
    float fVDotV = kVelocity.Dot(kVelocity);
    float fVDotA = kVelocity.Dot(kAcceleration);
    Vector3f kNormal = fVDotV*kAcceleration - fVDotA*kVelocity;
    kNormal.Normalize();
    return kNormal;
}
//----------------------------------------------------------------------------
Vector3f CurveSegment::Binormal (float fU) const
{
    Vector3f kVelocity = PU(fU);
    Vector3f kAcceleration = PUU(fU);
    float fVDotV = kVelocity.Dot(kVelocity);
    float fVDotA = kVelocity.Dot(kAcceleration);
    Vector3f kNormal = fVDotV*kAcceleration - fVDotA*kVelocity;
    kNormal.Normalize();
    kVelocity.Normalize();
    Vector3f kBinormal = kVelocity.Cross(kNormal);
    return kBinormal;
}
//----------------------------------------------------------------------------
void CurveSegment::GetFrame (float fU, Vector3f& rkPosition,
    Vector3f& rkTangent, Vector3f& rkNormal, Vector3f& rkBinormal) const
{
    rkPosition = P(fU);
    Vector3f kVelocity = PU(fU);
    Vector3f kAcceleration = PUU(fU);
    float fVDotV = kVelocity.Dot(kVelocity);
    float fVDotA = kVelocity.Dot(kAcceleration);
    rkNormal = fVDotV*kAcceleration - fVDotA*kVelocity;
    rkNormal.Normalize();
    rkTangent = kVelocity;
    rkTangent.Normalize();
    rkBinormal = rkTangent.Cross(rkNormal);
}
//----------------------------------------------------------------------------
float CurveSegment::Curvature (float fU) const
{
    Vector3f kVelocity = PU(fU);
    float fSpeedSqr = kVelocity.SquaredLength();

    if ( fSpeedSqr >= Mathf::ZERO_TOLERANCE )
    {
        Vector3f kAcceleration = PUU(fU);
        Vector3f kCross = kVelocity.Cross(kAcceleration);
        float fNumer = kCross.Length();
        float fDenom = Mathf::Pow(fSpeedSqr,1.5f);
        return fNumer/fDenom;
    }
    else
    {
        // curvature is indeterminate, just return 0
        return 0.0f;
    }
}
//----------------------------------------------------------------------------
float CurveSegment::Torsion (float fU) const
{
    Vector3f kVelocity = PU(fU);
    Vector3f kAcceleration = PUU(fU);
    Vector3f kCross = kVelocity.Cross(kAcceleration);
    float fDenom = kCross.SquaredLength();

    if ( fDenom >= Mathf::ZERO_TOLERANCE )
    {
        Vector3f kJerk = PUUU(fU);
        float fNumer = kCross.Dot(kJerk);
        return fNumer/fDenom;
    }
    else
    {
        // torsion is indeterminate, just return 0
        return 0.0f;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void CurveSegment::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    Object::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_fUMin);
    rkStream.Read(m_fUMax);

    WM3_END_DEBUG_STREAM_LOAD(CurveSegment);
}
//----------------------------------------------------------------------------
void CurveSegment::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Object::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool CurveSegment::Register (Stream& rkStream) const
{
    return Object::Register(rkStream);
}
//----------------------------------------------------------------------------
void CurveSegment::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    Object::Save(rkStream);

    // native data
    rkStream.Write(m_fUMin);
    rkStream.Write(m_fUMax);

    WM3_END_DEBUG_STREAM_SAVE(CurveSegment);
}
//----------------------------------------------------------------------------
StringTree* CurveSegment::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(3,1);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("umin =",m_fUMin));
    pkTree->SetString(2,StringTree::Format("umax =",m_fUMax));

    // children
    pkTree->SetChild(0,Object::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
int CurveSegment::GetMemoryUsed () const
{
    return sizeof(CurveSegment) - sizeof(Object) + Object::GetMemoryUsed();
}
//----------------------------------------------------------------------------
int CurveSegment::GetDiskUsed () const
{
    return Object::GetDiskUsed() +
        sizeof(m_fUMin) +
        sizeof(m_fUMax);
}
//----------------------------------------------------------------------------
