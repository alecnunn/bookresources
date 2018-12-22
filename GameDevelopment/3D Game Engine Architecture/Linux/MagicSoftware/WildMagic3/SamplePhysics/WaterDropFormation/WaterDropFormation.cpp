// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "WaterDropFormation.h"

WM3_WINDOW_APPLICATION(WaterDropFormation);

//#define SINGLE_STEP

//----------------------------------------------------------------------------
WaterDropFormation::WaterDropFormation ()
    :
    WindowApplication3("WaterDropFormation",0,0,640,480,
        ColorRGB(0.5f,0.0f,1.0f))
{
    m_pkSpline = 0;
    m_pkCircle = 0;
    m_akCtrlPoint = 0;
    m_akTarget = 0;
    m_fSimTime = 0.0f;
    m_fSimDelta = 0.05f;
}
//----------------------------------------------------------------------------
bool WaterDropFormation::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    // create scene
    m_spkScene = new Node;
    m_spkWireframe = new WireframeState;
    m_spkScene->SetGlobalState(m_spkWireframe);
    m_spkTrnNode = new Node(3);
    m_spkScene->AttachChild(m_spkTrnNode);
    CreatePlane();
    CreateWall();
    CreateWaterRoot();

    Configuration0();

    // center-and-fit for camera viewing
    m_spkScene->UpdateGS(0.0f);
    m_spkTrnNode->Local.SetTranslate(-m_spkScene->WorldBound->GetCenter());
    m_spkCamera->SetFrustum(-0.055f,0.055f,-0.04125f,0.04125f,0.1f,1000.0f);
    float fAngle = 0.01f*Mathf::PI;
    float fCos = Mathf::Cos(fAngle), fSin = Mathf::Sin(fAngle);
    Vector3f kCDir(-fCos,0.0f,-fSin);
    Vector3f kCUp(fSin,0.0f,-fCos);
    Vector3f kCRight = kCDir.Cross(kCUp);
    Vector3f kCLoc = -0.9f*m_spkScene->WorldBound->GetRadius()*kCDir;
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS(0.0f);
    m_spkScene->UpdateRS();

    InitializeCameraMotion(0.01f,0.001f);
    InitializeObjectMotion(m_spkScene);

    m_fLastSeconds = (float)System::GetTime();
    return true;
}
//----------------------------------------------------------------------------
void WaterDropFormation::OnTerminate ()
{
    delete m_pkSpline;
    delete m_pkCircle;
    delete[] m_akCtrlPoint;
    delete[] m_akTarget;
    m_spkScene = 0;
    m_spkTrnNode = 0;
    m_spkWaterRoot = 0;
    m_spkWireframe = 0;
    m_spkPlane = 0;
    m_spkWall = 0;
    m_spkWaterSurface = 0;
    m_spkWaterDrop = 0;
    m_spkWater = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void WaterDropFormation::OnIdle ()
{
    MeasureTime();

#ifndef SINGLE_STEP
    float fCurrSeconds = (float)System::GetTime();
    float fDiff = fCurrSeconds - m_fLastSeconds;
    if ( fDiff >= 0.033333f )
    {
        DoPhysical();
        m_fLastSeconds = fCurrSeconds;
    }
#endif

    DoVisual();
    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool WaterDropFormation::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if ( WindowApplication3::OnKeyDown(ucKey,iX,iY) )
        return true;

    switch ( ucKey )
    {
    case 'w':  // toggle wireframe
    case 'W':
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;

#ifdef SINGLE_STEP
    case 'g':
        DoPhysical();
        return true;
#endif
    }

    return false;
}
//----------------------------------------------------------------------------
void WaterDropFormation::CreatePlane ()
{
    // generate the plane
    StandardMesh kSM;
    kSM.UVs(true,false);
    m_spkPlane = kSM.Rectangle(2,2,8.0f,16.0f);

    // add a texture to the plane
    Texture* pkTexture = new Texture(
        Image::Load(System::GetPath("../../Data/Wmif/StoneCeiling.wmif")));
    pkTexture->CoordU = Texture::WM_REPEAT;
    pkTexture->CoordV = Texture::WM_REPEAT;

    TextureEffect* pkEffect = new TextureEffect(pkTexture,kSM.GetUVs());
    m_spkPlane->SetEffect(pkEffect);

    m_spkTrnNode->AttachChild(m_spkPlane);
}
//----------------------------------------------------------------------------
void WaterDropFormation::CreateWall ()
{
    // generate the wall
    StandardMesh kSM;
    kSM.UVs(true,false);
    Transformation kXFrm;
    kXFrm.SetTranslate(Vector3f(-8.0f,0.0f,8.0f));
    kXFrm.SetRotate(Matrix3f(Vector3f::UNIT_Y,Vector3f::UNIT_Z,
        Vector3f::UNIT_X,true));
    kSM.Transform(kXFrm);
    m_spkWall = kSM.Rectangle(2,2,16.0f,8.0f);

    // add a texture to the wall
    Texture* pkTexture = new Texture(
        Image::Load(System::GetPath("../../Data/Wmif/Stone.wmif")));
    pkTexture->CoordU = Texture::WM_REPEAT;
    pkTexture->CoordV = Texture::WM_REPEAT;

    TextureEffect* pkEffect = new TextureEffect(pkTexture,kSM.GetUVs());
    m_spkWall->SetEffect(pkEffect);

    m_spkTrnNode->AttachChild(m_spkWall);
}
//----------------------------------------------------------------------------
void WaterDropFormation::CreateWaterRoot ()
{
    m_spkWaterRoot = new Node(2);
    m_spkTrnNode->AttachChild(m_spkWaterRoot);
    m_spkWaterRoot->Local.SetTranslate(0.1f*Vector3f::UNIT_Z);
    m_spkWaterRoot->Local.SetUniformScale(8.0f);

    // The texture for the water objects.  This will be attached to children
    // of m_spkWaterRoot when the need arises.
    m_spkWater = new Texture(
        Image::Load(System::GetPath("../../Data/Wmif/WaterA.wmif")));
    m_spkWater->CoordU = Texture::WM_REPEAT;
    m_spkWater->CoordV = Texture::WM_REPEAT;

    // the texture has an alpha channel of 1/2
    AlphaState* pkAS = new AlphaState;
    pkAS->BlendEnabled = true;
    m_spkWaterRoot->SetGlobalState(pkAS);
}
//----------------------------------------------------------------------------
void WaterDropFormation::Configuration0 ()
{
    // Application loops between Configuration0() and Configuration1().
    // Delete all the objects from "1" when restarting with "0".
    delete[] m_akCtrlPoint;
    delete[] m_akTarget;
    delete m_pkSpline;
    delete m_pkCircle;
    m_pkCircle = 0;
    m_fSimTime = 0.0f;
    m_fSimDelta = 0.05f;

    m_spkWaterRoot->DetachChildAt(0);
    m_spkWaterRoot->DetachChildAt(1);
    m_spkWaterSurface = 0;
    m_spkWaterDrop = 0;

    // create water surface curve of revolution
    int iNumCtrlPoints = 13;
    int iDegree = 2;
    m_akCtrlPoint = new Vector2f[iNumCtrlPoints];
    m_akTarget = new Vector2f[iNumCtrlPoints];
    int i;
    for (i = 0; i < iNumCtrlPoints; i++)
        m_akCtrlPoint[i] = Vector2f(0.125f+0.0625f*i,0.0625f);

    float fH = 0.5f;
    float fD = 0.0625f;
    float fExtra = 0.1f;

    m_akTarget[ 0] = m_akCtrlPoint[ 0];
    m_akTarget[ 1] = m_akCtrlPoint[ 6];
    m_akTarget[ 2] = Vector2f(m_akCtrlPoint[6].X(),fH-fD-fExtra);
    m_akTarget[ 3] = Vector2f(m_akCtrlPoint[5].X(),fH-fD-fExtra);
    m_akTarget[ 4] = Vector2f(m_akCtrlPoint[5].X(),fH);
    m_akTarget[ 5] = Vector2f(m_akCtrlPoint[5].X(),fH+fD);
    m_akTarget[ 6] = Vector2f(m_akCtrlPoint[6].X(),fH+fD);
    m_akTarget[ 7] = Vector2f(m_akCtrlPoint[7].X(),fH+fD);
    m_akTarget[ 8] = Vector2f(m_akCtrlPoint[7].X(),fH);
    m_akTarget[ 9] = Vector2f(m_akCtrlPoint[7].X(),fH-fD-fExtra);
    m_akTarget[10] = Vector2f(m_akCtrlPoint[6].X(),fH-fD-fExtra);
    m_akTarget[11] = m_akCtrlPoint[ 6];
    m_akTarget[12] = m_akCtrlPoint[12];

    float* afWeight = new float[iNumCtrlPoints];
    for (i = 0; i < iNumCtrlPoints; i++)
        afWeight[i] = 1.0f;

    float fModWeight = 0.3f;
    afWeight[3] = fModWeight;
    afWeight[5] = fModWeight;
    afWeight[7] = fModWeight;
    afWeight[9] = fModWeight;

    m_pkSpline = new NURBSCurve2f(iNumCtrlPoints,m_akCtrlPoint,afWeight,
        iDegree,false,true);

    // restrict evaluation to a subinterval of the domain
    m_pkSpline->SetTimeInterval(0.5f,1.0f);

    delete[] afWeight;

    // create water surface
    m_spkWaterSurface = new RevolutionSurface(m_pkSpline,m_akCtrlPoint[6].X(),
        RevolutionSurface::REV_DISK_TOPOLOGY,32,16,false,false,true,false,
        true);

    // The RevolutionSurface construction attaches an effect to
    // m_spkWaterSurface and generates the uv-coordinates.
    Effect* pkEffect = m_spkWaterSurface->GetEffect();
    pkEffect->Textures.Append(m_spkWater);

    m_spkWaterRoot->AttachChild(m_spkWaterSurface);
    m_spkWaterRoot->UpdateGS(0.0f);
    m_spkWaterRoot->UpdateRS();
}
//----------------------------------------------------------------------------
void WaterDropFormation::Configuration1 ()
{
    delete[] m_akTarget;
    m_akTarget = 0;

    int iNumCtrlPoints = 5+9;
    int iDegree = 2;
    delete[] m_akCtrlPoint;
    m_akCtrlPoint = new Vector2f[iNumCtrlPoints];
    float* afWeight = new float[iNumCtrlPoints];

    // spline
    m_akCtrlPoint[0] = m_pkSpline->GetControlPoint(0);
    m_akCtrlPoint[1] = m_pkSpline->GetControlPoint(1);
    m_akCtrlPoint[2] = 0.5f*(m_pkSpline->GetControlPoint(1) +
        m_pkSpline->GetControlPoint(2));
    m_akCtrlPoint[3] = m_pkSpline->GetControlPoint(11);
    m_akCtrlPoint[4] = m_pkSpline->GetControlPoint(12);

    // circle
    int i, j;
    for (i = 2, j = 5; i <= 10; i++, j++)
        m_akCtrlPoint[j] = m_pkSpline->GetControlPoint(i);
    m_akCtrlPoint[5] = 0.5f*(m_akCtrlPoint[2]+m_akCtrlPoint[5]);
    m_akCtrlPoint[13] = m_akCtrlPoint[5];

    for (i = 0; i < iNumCtrlPoints; i++)
        afWeight[i] = 1.0f;

    afWeight[ 6] = m_pkSpline->GetControlWeight(3);
    afWeight[ 8] = m_pkSpline->GetControlWeight(5);
    afWeight[10] = m_pkSpline->GetControlWeight(7);
    afWeight[12] = m_pkSpline->GetControlWeight(9);

    delete m_pkSpline;
    m_pkSpline = new NURBSCurve2f(5,m_akCtrlPoint,afWeight,iDegree,false,
        true);

    // restrict evaluation to a subinterval of the domain
    m_pkSpline->SetTimeInterval(0.5f,1.0f);

    m_spkWaterSurface->SetCurve(m_pkSpline);

    m_pkCircle = new NURBSCurve2f(9,&m_akCtrlPoint[5],&afWeight[5],iDegree,
        true,false);
    delete[] afWeight;

    // Restrict evaluation to a subinterval of the domain.  Why 0.375?  The
    // circle NURBS is a loop and not open.  The curve is constructed with
    // iDegree (2) replicated control points.  Although the curve is
    // geometrically symmetric about the vertical axis, it is not symmetric
    // in t about the half way point (0.5) of the domain [0,1].
    m_pkCircle->SetTimeInterval(0.375f,1.0f);

    // Create water drop.  The outside view value is set to 'false' because
    // the curve (x(t),z(t)) has the property dz/dt < 0.  If the curve
    // instead had the property dz/dt > 0, then 'true' is the correct value
    // for the outside view.
    m_spkWaterDrop = new RevolutionSurface(m_pkCircle,m_akCtrlPoint[9].X(),
        RevolutionSurface::REV_SPHERE_TOPOLOGY,32,16,false,false,true,false,
        false);

    // The RevolutionSurface construction attaches an effect to
    // m_spkWaterSurface and generates the uv-coordinates.
    Effect* pkEffect = m_spkWaterDrop->GetEffect();
    pkEffect->Textures.Append(m_spkWater);

    m_spkWaterRoot->AttachChild(m_spkWaterDrop);
    m_spkWaterRoot->UpdateRS();
}
//----------------------------------------------------------------------------
void WaterDropFormation::DoPhysical1 ()
{
    // modify control points
    float fT = m_fSimTime, fOmT = 1.0f-fT;
    float fT4 = fT*fT, fOmT4 = 1.0f - fT4;
    int iMax = m_pkSpline->GetNumCtrlPoints();
    for (int i = 0; i < iMax; i++)
    {
        if ( i != 4 )
        {
            m_pkSpline->SetControlPoint(i,fOmT*m_akCtrlPoint[i] +
                fT*m_akTarget[i]);
        }
        else
        {
            m_pkSpline->SetControlPoint(i,fOmT4*m_akCtrlPoint[i] +
                fT4*m_akTarget[i]);
        }
    }

    // modify mesh vertices
    m_spkWaterSurface->UpdateSurface();

    // update the scene graph
    m_spkScene->UpdateGS(0.0f);
}
//----------------------------------------------------------------------------
void WaterDropFormation::DoPhysical2 ()
{
    if ( !m_pkCircle )
        Configuration1();

    m_fSimTime += m_fSimDelta;

    // surface evolves to a disk
    float fT = m_fSimTime - 1.0f, fOmT = 1.0f - fT;
    Vector2f kNewCtrl = fOmT*m_pkSpline->GetControlPoint(2) +
        fT*m_pkSpline->GetControlPoint(1);
    m_pkSpline->SetControlPoint(2,kNewCtrl);

    // sphere floats down a little bit
    int iMax = m_pkCircle->GetNumCtrlPoints();
    for (int i = 0; i < iMax; i++)
    {
        kNewCtrl = m_pkCircle->GetControlPoint(i) +
            Vector2f::UNIT_Y/32.0f;
        m_pkCircle->SetControlPoint(i,kNewCtrl);
    }

    m_spkWaterSurface->UpdateSurface();
    m_spkWaterDrop->UpdateSurface();
    m_spkScene->UpdateGS(0.0f);
}
//----------------------------------------------------------------------------
void WaterDropFormation::DoPhysical3 ()
{
    m_fSimTime += m_fSimDelta;

    // sphere floats down a little bit
    int iMax = m_pkCircle->GetNumCtrlPoints();
    int i;
    for (i = 0; i < iMax; i++)
    {
        Vector2f kNewCtrl = m_pkCircle->GetControlPoint(i);
        if ( i == 0 || i == iMax-1 )
            kNewCtrl += 1.3f*Vector2f::UNIT_Y/32;
        else
            kNewCtrl += Vector2f::UNIT_Y/32;
        m_pkCircle->SetControlPoint(i,kNewCtrl);
    }

    m_spkWaterDrop->UpdateSurface();
    m_spkScene->UpdateGS(0.0f);
}
//----------------------------------------------------------------------------
void WaterDropFormation::DoPhysical ()
{
    m_fSimTime += m_fSimDelta;
    if ( m_fSimTime <= 1.0f )
    {
        // water surface extruded to form a water drop
        DoPhysical1();
    }
    else if ( m_fSimTime <= 2.0f )
    {
        // water drop splits from water surface
        DoPhysical2();
    }
    else if ( m_fSimTime <= 4.0f )
    {
        // water drop continues downward motion, surface no longer changes
        DoPhysical3();
    }
    else
    {
        // restart
        Configuration0();
    }
}
//----------------------------------------------------------------------------
void WaterDropFormation::DoVisual ()
{
    m_pkRenderer->ClearBuffers();
    if ( m_pkRenderer->BeginScene() )
    {
        m_pkRenderer->DrawScene(m_spkScene);
        DrawFrameRate(8,GetHeight()-8,ColorRGBA::WHITE);

        char acMsg[256];
        sprintf(acMsg,"time = %6.4f",m_fSimTime);
        m_pkRenderer->Draw(96,GetHeight()-8,ColorRGBA::WHITE,acMsg);
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();
}
//----------------------------------------------------------------------------
