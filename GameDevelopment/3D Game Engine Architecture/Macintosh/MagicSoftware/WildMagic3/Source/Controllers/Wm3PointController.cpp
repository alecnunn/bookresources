// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3PointController.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,PointController,Controller);
WM3_IMPLEMENT_STREAM(PointController);
WM3_IMPLEMENT_DEFAULT_NAME_ID(PointController,Controller);

//----------------------------------------------------------------------------
PointController::PointController ()
    :
    SystemLinearAxis(Vector3f::UNIT_Z),
    SystemAngularAxis(Vector3f::UNIT_Z)
{
    SystemLinearSpeed = 0.0f;
    SystemAngularSpeed = 0.0f;
    m_iPQuantity = 0;
    m_afPointLinearSpeed = 0;
    m_afPointAngularSpeed = 0;
    m_akPointLinearAxis = 0;
    m_akPointAngularAxis = 0;
}
//----------------------------------------------------------------------------
PointController::~PointController ()
{
    delete[] m_afPointLinearSpeed;
    delete[] m_afPointAngularSpeed;
    delete[] m_akPointLinearAxis;
    delete[] m_akPointAngularAxis;
}
//----------------------------------------------------------------------------
void PointController::Reallocate (int iPQuantity)
{
    SystemLinearSpeed = 0.0f;
    SystemAngularSpeed = 0.0f;
    SystemLinearAxis = Vector3f::UNIT_Z;
    SystemAngularAxis = Vector3f::UNIT_Z;

    delete[] m_afPointLinearSpeed;
    delete[] m_afPointAngularSpeed;
    delete[] m_akPointLinearAxis;
    delete[] m_akPointAngularAxis;

    m_iPQuantity = iPQuantity;
    if ( m_iPQuantity > 0 )
    {
        m_afPointLinearSpeed = new float[m_iPQuantity];
        m_afPointAngularSpeed = new float[m_iPQuantity];
        m_akPointLinearAxis = new Vector3f[m_iPQuantity];
        m_akPointAngularAxis = new Vector3f[m_iPQuantity];

        memset(m_afPointLinearSpeed,0,m_iPQuantity*sizeof(float));
        memset(m_afPointAngularSpeed,0,m_iPQuantity*sizeof(float));
        for (int i = 0; i < m_iPQuantity; i++)
        {
            m_akPointLinearAxis[i] = Vector3f::UNIT_Z;
            m_akPointAngularAxis[i] = Vector3f::UNIT_Z;
        }
    }
    else
    {
        m_afPointLinearSpeed = 0;
        m_afPointAngularSpeed = 0;
        m_akPointLinearAxis = 0;
        m_akPointAngularAxis = 0;
    }
}
//----------------------------------------------------------------------------
void PointController::SetObject (Object* pkObject)
{
    Controller::SetObject(pkObject);

    if ( pkObject )
    {
        assert( pkObject->IsDerived(Polypoint::TYPE) );
        Polypoint* pkPoints = StaticCast<Polypoint>(pkObject);
        Reallocate(pkPoints->Vertices->GetQuantity());
    }
    else
    {
        Reallocate(0);
    }
}
//----------------------------------------------------------------------------
void PointController::UpdateSystemMotion (float fCtrlTime)
{
    Polypoint* pkPoint = StaticCast<Polypoint>(m_pkObject);

    float fDistance = fCtrlTime*SystemLinearSpeed;
    Vector3f kDTrn = fDistance*SystemLinearAxis;
    pkPoint->Local.SetTranslate(pkPoint->Local.GetTranslate() + kDTrn);

    float fAngle = fCtrlTime*SystemAngularSpeed;
    Matrix3f kDRot(SystemAngularAxis,fAngle);
    pkPoint->Local.SetRotate(kDRot*pkPoint->Local.GetRotate());
}
//----------------------------------------------------------------------------
void PointController::UpdatePointMotion (float fCtrlTime)
{
    Polypoint* pkPoint = StaticCast<Polypoint>(m_pkObject);
    int i, iAQuantity = pkPoint->GetActiveQuantity();
    Vector3f* akVertex = pkPoint->Vertices->GetData();

    for (i = 0; i < iAQuantity; i++)
    {
        float fDistance = fCtrlTime*m_afPointLinearSpeed[i];
        Vector3f kDTrn = fDistance*m_akPointLinearAxis[i];
        akVertex[i] += kDTrn;
    }

    Vector3f* akNormal = pkPoint->Normals->GetData();
    if ( akNormal )
    {
        for (i = 0; i < iAQuantity; i++)
        {
            float fAngle = fCtrlTime*m_afPointAngularSpeed[i];
            Matrix3f kDRot(m_akPointAngularAxis[i],fAngle);
            akNormal[i] = kDRot*akNormal[i];
            akNormal[i].Normalize();
        }
    }
}
//----------------------------------------------------------------------------
bool PointController::Update (double dAppTime)
{
    if ( !Controller::Update(dAppTime) )
        return false;

    float fCtrlTime = (float)GetControlTime(dAppTime);

    UpdateSystemMotion(fCtrlTime);
    UpdatePointMotion(fCtrlTime);
    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void PointController::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    Controller::Load(rkStream,pkLink);

    // native data
    rkStream.Read(SystemLinearSpeed);
    rkStream.Read(SystemAngularSpeed);
    rkStream.Read(SystemLinearAxis);
    rkStream.Read(SystemAngularAxis);

    int iVertexQuantity;
    rkStream.Read(iVertexQuantity);
    if ( iVertexQuantity > 0 )
    {
        Reallocate(iVertexQuantity);
        rkStream.Read(iVertexQuantity,m_afPointLinearSpeed);
        rkStream.Read(iVertexQuantity,m_afPointAngularSpeed);
        rkStream.Read(iVertexQuantity,m_akPointLinearAxis);
        rkStream.Read(iVertexQuantity,m_akPointAngularAxis);
    }

    WM3_END_DEBUG_STREAM_LOAD(PointController);
}
//----------------------------------------------------------------------------
void PointController::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Controller::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool PointController::Register (Stream& rkStream) const
{
    return Controller::Register(rkStream);
}
//----------------------------------------------------------------------------
void PointController::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    Controller::Save(rkStream);

    // native data
    rkStream.Write(SystemLinearSpeed);
    rkStream.Write(SystemAngularSpeed);
    rkStream.Write(SystemLinearAxis);
    rkStream.Write(SystemAngularAxis);

    // Write this to disk so that Load does not have to wait until the
    // controlled object is loaded and linked in order to allocate the
    // arrays.
    rkStream.Write(m_iPQuantity);
    if ( m_iPQuantity > 0 )
    {
        rkStream.Write(m_iPQuantity,m_afPointLinearSpeed);
        rkStream.Write(m_iPQuantity,m_afPointAngularSpeed);
        rkStream.Write(m_iPQuantity,m_akPointLinearAxis);
        rkStream.Write(m_iPQuantity,m_akPointAngularAxis);
    }

    WM3_END_DEBUG_STREAM_SAVE(PointController);
}
//----------------------------------------------------------------------------
StringTree* PointController::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(5,5);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("sys lin speed =",
        SystemLinearSpeed));
    pkTree->SetString(2,StringTree::Format("sys ang speed =",
        SystemAngularSpeed));
    pkTree->SetString(3,StringTree::Format("sys lin axis =",
        SystemLinearAxis));
    pkTree->SetString(4,StringTree::Format("sys ang axis =",
        SystemAngularAxis));

    // children
    pkTree->SetChild(0,Controller::SaveStrings());

    pkTree->SetChild(1,StringTree::Format("point lin speed",m_iPQuantity,
        m_afPointLinearSpeed));

    pkTree->SetChild(2,StringTree::Format("point ang speed",m_iPQuantity,
        m_afPointAngularSpeed));

    pkTree->SetChild(3,StringTree::Format("point lin axis",m_iPQuantity,
        m_akPointLinearAxis));

    pkTree->SetChild(4,StringTree::Format("point ang axis",m_iPQuantity,
        m_akPointAngularAxis));

    return pkTree;
}
//----------------------------------------------------------------------------
int PointController::GetMemoryUsed () const
{
    int iSize = sizeof(PointController) - sizeof(Controller) +
        Controller::GetMemoryUsed();

    if ( m_iPQuantity > 0 )
    {
        iSize += m_iPQuantity*sizeof(m_afPointLinearSpeed[0]);
        iSize += m_iPQuantity*sizeof(m_afPointAngularSpeed[0]);
        iSize += m_iPQuantity*sizeof(m_akPointLinearAxis[0]);
        iSize += m_iPQuantity*sizeof(m_akPointAngularAxis[0]);
    }

    return iSize;
}
//----------------------------------------------------------------------------
int PointController::GetDiskUsed () const
{
    int iSize = Controller::GetDiskUsed() +
        sizeof(SystemLinearSpeed) +
        sizeof(SystemAngularSpeed) +
        sizeof(SystemLinearAxis) +
        sizeof(SystemAngularAxis) +
        sizeof(m_iPQuantity);

    if ( m_iPQuantity > 0 )
    {
        iSize += m_iPQuantity*sizeof(m_afPointLinearSpeed[0]);
        iSize += m_iPQuantity*sizeof(m_afPointAngularSpeed[0]);
        iSize += m_iPQuantity*sizeof(m_akPointLinearAxis[0]);
        iSize += m_iPQuantity*sizeof(m_akPointAngularAxis[0]);
    }

    return iSize;
}
//----------------------------------------------------------------------------
