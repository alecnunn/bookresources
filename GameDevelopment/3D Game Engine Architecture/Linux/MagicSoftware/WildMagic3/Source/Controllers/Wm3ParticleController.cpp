// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3ParticleController.h"
#include "Wm3Particles.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,ParticleController,Controller);
WM3_IMPLEMENT_STREAM(ParticleController);
WM3_IMPLEMENT_DEFAULT_NAME_ID(ParticleController,Controller);

//----------------------------------------------------------------------------
ParticleController::ParticleController ()
    :
    SystemLinearAxis(Vector3f::UNIT_Z),
    SystemAngularAxis(Vector3f::UNIT_Z)
{
    SystemLinearSpeed = 0.0f;
    SystemAngularSpeed = 0.0f;
    m_iLQuantity = 0;
    m_afPointLinearSpeed = 0;
    m_akPointLinearAxis = 0;
    SystemSizeChange = 0.0f;
    m_afPointSizeChange = 0;
}
//----------------------------------------------------------------------------
ParticleController::~ParticleController ()
{
    delete[] m_afPointLinearSpeed;
    delete[] m_akPointLinearAxis;
    delete[] m_afPointSizeChange;
}
//----------------------------------------------------------------------------
void ParticleController::Reallocate (int iLQuantity)
{
    SystemLinearSpeed = 0.0f;
    SystemAngularSpeed = 0.0f;
    SystemLinearAxis = Vector3f::UNIT_Z;
    SystemAngularAxis = Vector3f::UNIT_Z;
    SystemSizeChange = 0.0f;

    delete[] m_afPointLinearSpeed;
    delete[] m_akPointLinearAxis;
    delete[] m_afPointSizeChange;

    m_iLQuantity = iLQuantity;
    if ( m_iLQuantity > 0 )
    {
        m_afPointLinearSpeed = new float[m_iLQuantity];
        m_akPointLinearAxis = new Vector3f[m_iLQuantity];
        m_afPointSizeChange = new float[m_iLQuantity];

        memset(m_afPointLinearSpeed,0,m_iLQuantity*sizeof(float));
        for (int i = 0; i < m_iLQuantity; i++)
            m_akPointLinearAxis[i] = Vector3f::UNIT_Z;

        memset(m_afPointSizeChange,0,m_iLQuantity*sizeof(float));
    }
    else
    {
        m_afPointLinearSpeed = 0;
        m_akPointLinearAxis = 0;
        m_afPointSizeChange = 0;
    }
}
//----------------------------------------------------------------------------
void ParticleController::SetObject (Object* pkObject)
{
    Controller::SetObject(pkObject);

    if ( pkObject )
    {
        assert( pkObject->IsDerived(Particles::TYPE) );
        Particles* pkParticles = StaticCast<Particles>(pkObject);
        Reallocate(pkParticles->Locations->GetQuantity());
    }
    else
    {
        Reallocate(0);
    }
}
//----------------------------------------------------------------------------
void ParticleController::UpdateSystemMotion (float fCtrlTime)
{
    Particles* pkParticle = StaticCast<Particles>(m_pkObject);

    float fDSize = fCtrlTime*SystemSizeChange;
    pkParticle->SizeAdjust += fDSize;
    if ( pkParticle->SizeAdjust < 0.0f )
        pkParticle->SizeAdjust = 0.0f;

    float fDistance = fCtrlTime*SystemLinearSpeed;
    Vector3f kDTrn = fDistance*SystemLinearAxis;
    pkParticle->Local.SetTranslate(pkParticle->Local.GetTranslate() + kDTrn);

    float fAngle = fCtrlTime*SystemAngularSpeed;
    Matrix3f kDRot(SystemAngularAxis,fAngle);
    pkParticle->Local.SetRotate(kDRot*pkParticle->Local.GetRotate());
}
//----------------------------------------------------------------------------
void ParticleController::UpdatePointMotion (float fCtrlTime)
{
    Particles* pkParticle = StaticCast<Particles>(m_pkObject);
    int i, iAQuantity = pkParticle->GetActiveQuantity();
    float* afSize = pkParticle->Sizes->GetData();

    for (i = 0; i < iAQuantity; i++)
    {
        float fDSize = fCtrlTime*m_afPointSizeChange[i];
        afSize[i] += fDSize;
    }

    Vector3f* akLocation = pkParticle->Locations->GetData();
    for (i = 0; i < iAQuantity; i++)
    {
        float fDistance = fCtrlTime*m_afPointLinearSpeed[i];
        Vector3f kDTrn = fDistance*m_akPointLinearAxis[i];
        akLocation[i] += kDTrn;
    }
}
//----------------------------------------------------------------------------
bool ParticleController::Update (double dAppTime)
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
void ParticleController::Load (Stream& rkStream,
    Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    Controller::Load(rkStream,pkLink);

    // native data
    rkStream.Read(SystemLinearSpeed);
    rkStream.Read(SystemAngularSpeed);
    rkStream.Read(SystemLinearAxis);
    rkStream.Read(SystemAngularAxis);
    rkStream.Read(SystemSizeChange);

    rkStream.Read(m_iLQuantity);
    if ( m_iLQuantity > 0 )
    {
        Reallocate(m_iLQuantity);
        rkStream.Read(m_iLQuantity,m_afPointLinearSpeed);
        rkStream.Read(m_iLQuantity,m_akPointLinearAxis);
        rkStream.Read(m_iLQuantity,m_afPointSizeChange);
    }

    WM3_END_DEBUG_STREAM_LOAD(ParticleController);
}
//----------------------------------------------------------------------------
void ParticleController::Link (Stream& rkStream,
    Stream::Link* pkLink)
{
    Controller::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool ParticleController::Register (Stream& rkStream) const
{
    return Controller::Register(rkStream);
}
//----------------------------------------------------------------------------
void ParticleController::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    Controller::Save(rkStream);

    // native data
    rkStream.Write(SystemLinearSpeed);
    rkStream.Write(SystemAngularSpeed);
    rkStream.Write(SystemLinearAxis);
    rkStream.Write(SystemAngularAxis);
    rkStream.Write(SystemSizeChange);

    // Write this to disk so that Load does not have to wait until the
    // controlled object is loaded and linked in order to allocate the
    // arrays.
    rkStream.Write(m_iLQuantity);
    if ( m_iLQuantity > 0 )
    {
        rkStream.Write(m_iLQuantity,m_afPointLinearSpeed);
        rkStream.Write(m_iLQuantity,m_akPointLinearAxis);
        rkStream.Write(m_iLQuantity,m_afPointSizeChange);
    }

    WM3_END_DEBUG_STREAM_SAVE(ParticleController);
}
//----------------------------------------------------------------------------
StringTree* ParticleController::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(6,4);

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
    pkTree->SetString(5,StringTree::Format("sys size change =",
        SystemSizeChange));

    // children
    pkTree->SetChild(0,Controller::SaveStrings());

    pkTree->SetChild(1,StringTree::Format("point lin speed",m_iLQuantity,
        m_afPointLinearSpeed));

    pkTree->SetChild(2,StringTree::Format("point lin axis",m_iLQuantity,
        m_akPointLinearAxis));

    pkTree->SetChild(3,StringTree::Format("point size change",m_iLQuantity,
        m_afPointSizeChange));

    return pkTree;
}
//----------------------------------------------------------------------------
int ParticleController::GetMemoryUsed () const
{
    int iSize = sizeof(ParticleController) - sizeof(Controller) +
        Controller::GetMemoryUsed();

    if ( m_iLQuantity > 0 )
    {
        iSize += m_iLQuantity*sizeof(m_afPointLinearSpeed[0]);
        iSize += m_iLQuantity*sizeof(m_akPointLinearAxis[0]);
        iSize += m_iLQuantity*sizeof(m_afPointSizeChange[0]);
    }

    return iSize;
}
//----------------------------------------------------------------------------
int ParticleController::GetDiskUsed () const
{
    int iSize = Controller::GetDiskUsed() +
        sizeof(SystemLinearSpeed) +
        sizeof(SystemAngularSpeed) +
        sizeof(SystemLinearAxis) +
        sizeof(SystemAngularAxis) +
        sizeof(SystemSizeChange) +
        sizeof(m_iLQuantity);

    if ( m_iLQuantity > 0 )
    {
        iSize += m_iLQuantity*sizeof(m_afPointLinearSpeed[0]);
        iSize += m_iLQuantity*sizeof(m_akPointLinearAxis[0]);
        iSize += m_iLQuantity*sizeof(m_afPointSizeChange[0]);
    }

    return iSize;
}
//----------------------------------------------------------------------------
