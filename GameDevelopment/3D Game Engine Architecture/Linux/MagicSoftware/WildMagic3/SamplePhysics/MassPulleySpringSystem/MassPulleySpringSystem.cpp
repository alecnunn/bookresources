// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "MassPulleySpringSystem.h"

WM3_WINDOW_APPLICATION(MassPulleySpringSystem);

//#define SINGLE_STEP

//----------------------------------------------------------------------------
MassPulleySpringSystem::MassPulleySpringSystem ()
    :
    WindowApplication3("MassPulleySpringSystem",0,0,640,480,
        ColorRGB(0.819607f,0.909803f,0.713725f))
{
    m_pkHelixSpline = 0;
    m_pkCableSpline = 0;
    m_fLastIdle = 0.0f;
}
//----------------------------------------------------------------------------
bool MassPulleySpringSystem::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    // initialize the physics module
    m_kModule.Gravity = 1.0;
    m_kModule.Mass1 = 1.0;
    m_kModule.Mass2 = 2.0;
    m_kModule.Mass3 = 3.0;
    m_kModule.Radius = 32.0;
    m_kModule.Inertia = Mathd::HALF_PI*Mathd::Pow(m_kModule.Radius,4.0);
    m_kModule.WireLength = 375.0 + Mathd::PI*m_kModule.Radius;
    m_kModule.SpringLength = 100.0;
    m_kModule.SpringConstant = 10.0;

    double dTime = 0.0;
    double dDeltaTime = 0.1;
    double dY1 = 200.0;
    double dDY1 = -10.0;
    double dDY3 = -20.0;
    m_kModule.Initialize(dTime,dDeltaTime,dY1,dDY1,dDY3);

    // set up the scene graph
    CreateScene();
    m_spkWireframe = new WireframeState;
    m_spkScene->SetGlobalState(m_spkWireframe);

    // set up the camera
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1000.0f);
    float fAngle = 0.1f*Mathf::PI;
    float fCos = Mathf::Cos(fAngle), fSin = Mathf::Sin(fAngle);
    Vector3f kCLoc(0.0f,48.0f,326.0f);
    Vector3f kCDir(0.0f,fSin,-fCos);
    Vector3f kCUp(0.0f,-fCos,-fSin);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS(0.0f);
    m_spkScene->UpdateRS();

    InitializeCameraMotion(0.1f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void MassPulleySpringSystem::OnTerminate ()
{
    m_spkScene = 0;
    m_spkFloor = 0;
    m_spkWireframe = 0;
    m_spkAssembly = 0;
    m_spkCableRoot = 0;
    m_spkCable = 0;
    m_spkMass1 = 0;
    m_spkMass2 = 0;
    m_spkPulleyRoot = 0;
    m_spkPulley = 0;
    m_spkSpring = 0;
    m_spkCable = 0;
    m_spkPlate0 = 0;
    m_spkPlate1 = 0;
    m_spkCylinder = 0;
    m_spkMetal = 0;
    m_spkSpring = 0;
    m_spkSide0 = 0;
    m_spkSide1 = 0;
    m_spkTop = 0;
    m_spkHelix = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void MassPulleySpringSystem::OnIdle ()
{
    MeasureTime();
    MoveCamera();
    if ( MoveObject() )
        m_spkScene->UpdateGS(0.0f);

    float fCurrIdle = (float)System::GetTime();
    float fDiff = fCurrIdle - m_fLastIdle;
    if ( fDiff >= 1.0f/30.0f )
    {
        m_fLastIdle = fCurrIdle;

        DoPhysical();

        m_pkRenderer->ClearBuffers();
        if ( m_pkRenderer->BeginScene() )
        {
            m_pkRenderer->DrawScene(m_spkScene);
            m_pkRenderer->EndScene();
        }
        m_pkRenderer->DisplayBackBuffer();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool MassPulleySpringSystem::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
    case 'g':  // single-step simulation
    case 'G':
        DoPhysical();
        return true;
#endif
    }

    return false;
}
//----------------------------------------------------------------------------
void MassPulleySpringSystem::CreateScene ()
{
    // set up the scene graph
    // scene -+- floor
    //        |
    //        +- assembly -+- cableRoot -+- cable
    //                     |             |
    //                     |             +- mass0
    //                     |             |
    //                     |             +- mass1
    //                     |
    //                     +- pulleyRoot -+- pulley -+- plate0
    //                                    |          |
    //                                    |          +- plate1
    //                                    |          |
    //                                    |          +- cylinder
    //                                    |
    //                                    +- spring -+- side0
    //                                               |
    //                                               +- side1
    //                                               |
    //                                               +- top
    //                                               |
    //                                               +- wire

    m_spkScene = new Node(2);
    m_spkAssembly = new Node(2);
    m_spkCableRoot = new Node(3);
    m_spkPulleyRoot = new Node(2);
    m_spkPulley = new Node(3);
    m_spkSpring = new Node(4);
    m_spkFloor = CreateFloor();
    m_spkCable = CreateCable();
    m_spkMass1 = CreateMass(1.0f);
    m_spkMass2 = CreateMass(2.0f);
    CreatePulley();
    CreateSpring();
    m_spkHelix = CreateHelix();

    m_spkScene->AttachChild(m_spkFloor);
    m_spkScene->AttachChild(m_spkAssembly);
    m_spkAssembly->AttachChild(m_spkCableRoot);
    m_spkAssembly->AttachChild(m_spkPulleyRoot);
    m_spkCableRoot->AttachChild(m_spkCable);
    m_spkCableRoot->AttachChild(m_spkMass1);
    m_spkCableRoot->AttachChild(m_spkMass2);
    m_spkPulleyRoot->AttachChild(m_spkPulley);
    m_spkPulleyRoot->AttachChild(m_spkSpring);
    m_spkPulley->AttachChild(m_spkPlate0);
    m_spkPulley->AttachChild(m_spkPlate1);
    m_spkPulley->AttachChild(m_spkCylinder);
    m_spkSpring->AttachChild(m_spkSide0);
    m_spkSpring->AttachChild(m_spkSide1);
    m_spkSpring->AttachChild(m_spkTop);
    m_spkSpring->AttachChild(m_spkHelix);

    m_spkPulleyRoot->Local.SetTranslate(
        Vector3f(0.0f,(float)m_kModule.GetCurrentY3(),0.0f));
    UpdateCable();
    UpdateHelix();
}
//----------------------------------------------------------------------------
TriMesh* MassPulleySpringSystem::CreateFloor ()
{
    Transformation kXFrm;
    kXFrm.SetRotate(Matrix3f(-Vector3f::UNIT_Z,Vector3f::UNIT_X,
        -Vector3f::UNIT_Y,true));
    kXFrm.SetTranslate(Vector3f(0.0f,255.0f,0.0f));

    StandardMesh kSM;
    kSM.UVs(true,false).Transform(kXFrm);
    TriMesh* pkFloor = kSM.Rectangle(2,2,1024.0f,1024.0f);

    Texture* pkTexture = new Texture(
        Image::Load(System::GetPath("../../Data/Wmif/Wood.wmif")));
    pkTexture->CoordU = Texture::WM_REPEAT;
    pkTexture->CoordV = Texture::WM_REPEAT;

    TextureEffect* pkEffect = new TextureEffect(pkTexture,kSM.GetUVs());
    pkFloor->SetEffect(pkEffect);

    return pkFloor;
}
//----------------------------------------------------------------------------
float MassPulleySpringSystem::CableRadial (float)
{
    return 0.5f;
}
//----------------------------------------------------------------------------
TubeSurface* MassPulleySpringSystem::CreateCable ()
{
    // create quadratic spline for the medial axis
    const int iNumCtrlPoints = 1024;
    Vector3f* akCtrlPoint = new Vector3f[iNumCtrlPoints];
    const int iDegree = 2;
    m_pkCableSpline = new BSplineCurve3f(iNumCtrlPoints,akCtrlPoint,iDegree,
        false,true);
    delete[] akCtrlPoint;

    // generate a tube surface whose medial axis is the spline
    Vector2f kUVMin(0.0f,0.0f), kUVMax(1.0f,1.0f);
    TubeSurface* pkCable = new TubeSurface(m_pkCableSpline,CableRadial,
        false,Vector3f::UNIT_Z,128,16,false,
        false,false,false,&kUVMin,&kUVMax);

    Texture* pkTexture = new Texture(
        Image::Load(System::GetPath("../../Data/Wmif/Rope.wmif")));
    pkTexture->CoordU = Texture::WM_REPEAT;
    pkTexture->CoordV = Texture::WM_REPEAT;

    // The TubeSurface constructor attaches an effect to pkCable and creates
    // the uv-coordinates.
    Effect* pkEffect = pkCable->GetEffect();
    pkEffect->Textures.Append(pkTexture);

    return pkCable;
}
//----------------------------------------------------------------------------
TriMesh* MassPulleySpringSystem::CreateMass (float fRadius)
{
    StandardMesh kSM;
    TriMesh* pkMass = kSM.Sphere(8,8,fRadius);

    int iVQuantity = pkMass->Vertices->GetQuantity();
    ColorRGB* akColor = new ColorRGB[iVQuantity];
    ColorRGBArray* pkColors = new ColorRGBArray(iVQuantity,akColor);
    for (int i = 0; i < iVQuantity; i++)
        akColor[i] = 0.75f*ColorRGB::WHITE;

    VertexColorEffect* pkEffect = new VertexColorEffect(pkColors);
    pkMass->SetEffect(pkEffect);

    return pkMass;
}
//----------------------------------------------------------------------------
void MassPulleySpringSystem::CreatePulley ()
{
    float fThickness = 4.0f, fRadius = (float)m_kModule.Radius;

    // shared texture for plates and cylinder of pulley
    m_spkMetal = new Texture(
        Image::Load(System::GetPath("../../Data/Wmif/Metal.wmif")));
    m_spkMetal->CoordU = Texture::WM_REPEAT;
    m_spkMetal->CoordV = Texture::WM_REPEAT;

    StandardMesh kSM;
    kSM.UVs(true,false);
    Transformation kXFrm;

    m_spkPlate0 = kSM.Disk(4,32,fRadius);
    m_spkPlate0->SetEffect(new TextureEffect(m_spkMetal,kSM.GetUVs()));
    m_spkPlate0->Local.SetTranslate(Vector3f(0.0f,0.0f,0.5f*fThickness));

    kXFrm.SetRotate(Matrix3f(-Vector3f::UNIT_X,Vector3f::UNIT_Y,
        -Vector3f::UNIT_Z,true));
    kSM.Transform(kXFrm);
    m_spkPlate1 = kSM.Disk(4,32,fRadius);
    m_spkPlate1->SetEffect(new TextureEffect(m_spkMetal,kSM.GetUVs()));
    m_spkPlate1->Local.SetTranslate(Vector3f(0.0f,0.0f,-0.5f*fThickness));

    kXFrm.MakeIdentity();
    kSM.Transform(kXFrm);
    m_spkCylinder = kSM.Cylinder(2,32,fRadius,fThickness,true);
    m_spkCylinder->SetEffect(new TextureEffect(m_spkMetal,kSM.GetUVs()));
}
//----------------------------------------------------------------------------
void MassPulleySpringSystem::CreateSpring ()
{
    float fThick = 4.0f, fRadius = (float)m_kModule.Radius;
    float fXExtent = 2.0f, fYExtent = 18.0f, fZExtent = 1.0f;

    StandardMesh kSM;
    kSM.UVs(true,false);
    Transformation kXFrm;

    int iVQuantity;
    ColorRGB* akColor;
    ColorRGBArray* pkColors;
    VertexColorEffect* pkEffect;

    kXFrm.SetTranslate(Vector3f(0.0f,-0.5f*fRadius,0.5f*fThick+fZExtent));
    kSM.Transform(kXFrm);
    m_spkSide0 = kSM.Box(fXExtent,fYExtent,fZExtent);
    iVQuantity = m_spkSide0->Vertices->GetQuantity();
    akColor = new ColorRGB[iVQuantity];
    memset(akColor,0,iVQuantity*sizeof(ColorRGB));
    pkColors = new ColorRGBArray(iVQuantity,akColor);
    pkEffect = new VertexColorEffect(pkColors);
    m_spkSide0->SetEffect(pkEffect);

    kXFrm.SetTranslate(Vector3f(0.0f,-0.5f*fRadius,-0.5f*fThick-fZExtent));
    kXFrm.SetRotate(Matrix3f(-Vector3f::UNIT_X,Vector3f::UNIT_Y,
        -Vector3f::UNIT_Z,true));
    kSM.Transform(kXFrm);
    m_spkSide1 = kSM.Box(fXExtent,fYExtent,fZExtent);
    iVQuantity = m_spkSide1->Vertices->GetQuantity();
    akColor = new ColorRGB[iVQuantity];
    memset(akColor,0,iVQuantity*sizeof(ColorRGB));
    pkColors = new ColorRGBArray(iVQuantity,akColor);
    pkEffect = new VertexColorEffect(pkColors);
    m_spkSide1->SetEffect(pkEffect);

    kXFrm.SetTranslate(Vector3f(0.0f,-0.5f*fRadius-fYExtent-0.5f,0.0f));
    kXFrm.SetRotate(Matrix3f(Vector3f::UNIT_Z,Vector3f::UNIT_X,
        Vector3f::UNIT_Y,true));
    kSM.Transform(kXFrm);
    fYExtent = fXExtent;
    fXExtent = 0.5f*fThick + 2.0f;
    fZExtent = 1.0f;
    m_spkTop = kSM.Box(fXExtent,fYExtent,fZExtent);
    iVQuantity = m_spkTop->Vertices->GetQuantity();
    akColor = new ColorRGB[iVQuantity];
    memset(akColor,0,iVQuantity*sizeof(ColorRGB));
    pkColors = new ColorRGBArray(iVQuantity,akColor);
    pkEffect = new VertexColorEffect(pkColors);
    m_spkTop->SetEffect(pkEffect);
}
//----------------------------------------------------------------------------
float MassPulleySpringSystem::HelixRadial (float)
{
    return 0.25f;
}
//----------------------------------------------------------------------------
TubeSurface* MassPulleySpringSystem::CreateHelix ()
{
    // create quadratic spline for the medial axis
    const int iNumCtrlPoints = 1024;
    Vector3f* akCtrlPoint = new Vector3f[iNumCtrlPoints];
    const int iDegree = 2;
    m_pkHelixSpline = new BSplineCurve3f(iNumCtrlPoints,akCtrlPoint,iDegree,
        false,true);
    delete[] akCtrlPoint;

    // generate a tube surface whose medial axis is the spline
    Vector2f kUVMin(0.0f,0.0f), kUVMax(1.0f,1.0f);
    TubeSurface* pkHelix = new TubeSurface(m_pkHelixSpline,CableRadial,
        false,Vector3f::UNIT_Z,128,16,false,false,false,false,&kUVMin,
        &kUVMax);

    // The TubeSurface constructor attaches an effect to pkHelix and creates
    // the uv-coordinates.
    Effect* pkEffect = pkHelix->GetEffect();
    pkEffect->Textures.Append(m_spkMetal);

    return pkHelix;
}
//----------------------------------------------------------------------------
void MassPulleySpringSystem::DoPhysical ()
{
    m_kModule.Update();

    UpdatePulley();
    UpdateCable();
    UpdateHelix();
    m_spkAssembly->UpdateGS(0.0f);
}
//----------------------------------------------------------------------------
void MassPulleySpringSystem::UpdatePulley ()
{
    m_spkPulley->Local.SetRotate(Matrix3f(Vector3f::UNIT_Z,
        (float)m_kModule.GetAngle()));
    m_spkPulleyRoot->Local.SetTranslate(Vector3f(0.0f,
        (float)m_kModule.GetCurrentY3(),0.0f));
}
//----------------------------------------------------------------------------
void MassPulleySpringSystem::UpdateCable ()
{
    // partition control points between two vertical wires and circle piece
    const int iCQuantity = m_pkCableSpline->GetNumCtrlPoints();
    float fFraction1 = (float)m_kModule.GetCableFraction1();
    float fFraction2 = (float)m_kModule.GetCableFraction2();
    float fFractionC = 1.0f - fFraction1 - fFraction2;

    int iMin, iMax, i;
    float fMult, fT;
    Vector3f kCtrl;
    kCtrl.Z() = 0.0f;

    // set control points for wire from mass 1 to pulley midline
    iMin = 0;
    iMax = (int)(fFraction1*iCQuantity);
    if ( iMin < iMax )
    {
        fMult = 1.0f/(iMax - iMin);
        kCtrl.X() = -(float)m_kModule.Radius;
        for (i = iMin; i <= iMax; i++)
        {
            fT = fMult*(i-iMin);
            kCtrl.Y() = (1.0f - fT)*(float)m_kModule.GetCurrentY1() +
                fT*(float)m_kModule.GetCurrentY3();
            m_pkCableSpline->SetControlPoint(i,kCtrl);
        }
    }
    else
    {
        m_pkCableSpline->SetControlPoint(iMin,kCtrl);
    }

    // set control points for wire along hemicircle of pulley
    iMin = iMax+1;
    iMax += (int)(fFractionC*iCQuantity);
    fMult = 1.0f/(iMax - iMin);
    for (i = iMin; i <= iMax; i++)
    {
        fT = -1.0f + fMult*(i-iMin);
        float fAngle = fT*Mathf::PI;
        kCtrl.X() = Mathf::Cos(fAngle)*(float)m_kModule.Radius;
        kCtrl.Y() = (float)m_kModule.GetCurrentY3() +
            Mathf::Sin(fAngle)*(float)m_kModule.Radius;
        m_pkCableSpline->SetControlPoint(i,kCtrl);
    }

    // set control points for wire from pulley midline to mass 2
    iMin = iMax+1;
    iMax = iCQuantity-1;
    if ( iMin < iMax )
    {
        fMult = 1.0f/(iMax - iMin);
        kCtrl.X() = (float)m_kModule.Radius;
        for (i = iMin; i <= iMax; i++)
        {
            fT = fMult*(i-iMin);
            kCtrl.Y() = (1.0f - fT)*(float)m_kModule.GetCurrentY3() +
                fT*(float)m_kModule.GetCurrentY2();
            m_pkCableSpline->SetControlPoint(i,kCtrl);
        }
    }
    else
    {
        m_pkCableSpline->SetControlPoint(iMin,kCtrl);
    }

    m_spkCable->UpdateSurface();

    // update the mass positions
    m_spkMass1->Local.SetTranslate(Vector3f(-(float)m_kModule.Radius,
        (float)m_kModule.GetCurrentY1(),0.0f));
    m_spkMass2->Local.SetTranslate(Vector3f((float)m_kModule.Radius,
        (float)m_kModule.GetCurrentY2(),0.0f));
}
//----------------------------------------------------------------------------
void MassPulleySpringSystem::UpdateHelix ()
{
    // current span of helix
    float fSpan = (float)(m_kModule.GetCurrentY3() - m_kModule.Radius - 4.0);

    const int iHQuantity = m_pkHelixSpline->GetNumCtrlPoints();
    const float fHRadius = 2.0f;
    const float fTMax = 14.0f;
    float fYMult = fSpan/fTMax;
    float fDelta = fTMax/(iHQuantity-1);
    for (int i = 0; i < iHQuantity; i++)
    {
        float fT = i*fDelta;
        float fAngle = Mathf::TWO_PI*fT;
        float fCos = Mathf::Cos(fAngle);
        float fSin = Mathf::Sin(fAngle);
        Vector3f kCtrl(fHRadius*fCos,-(float)m_kModule.Radius-4.0f-fYMult*fT,
            fHRadius*fSin);
        m_pkHelixSpline->SetControlPoint(i,kCtrl);
    }

    m_spkHelix->UpdateSurface();
}
//----------------------------------------------------------------------------
