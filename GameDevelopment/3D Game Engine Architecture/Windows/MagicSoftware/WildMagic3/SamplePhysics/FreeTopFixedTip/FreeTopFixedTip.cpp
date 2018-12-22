// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "FreeTopFixedTip.h"

WM3_WINDOW_APPLICATION(FreeTopFixedTip);

//#define SINGLE_STEP

//----------------------------------------------------------------------------
FreeTopFixedTip::FreeTopFixedTip ()
    :
    WindowApplication3("FreeTopFixedTip",0,0,640,480,
        ColorRGB(0.839215f,0.894117f,0.972549f))
{
    m_fLastIdle = 0.0f;
}
//----------------------------------------------------------------------------
bool FreeTopFixedTip::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    // initialize the simulation (TO DO.  Set up mass based on top geometry.)
    m_kModule.Gravity = 10.0;
    m_kModule.Mass = 1.0;
    m_kModule.Length = 8.0;
    m_kModule.Inertia1 = 1.0;
    m_kModule.Inertia3 = 2.0;

    double dTime = 0.0;
    double dDeltaTime = 0.01;
    double dTheta = 0.0;
    double dPhi = 0.001;
    double dPsi = 0.0;
    double dAngVel1 = 1.0;
    double dAngVel2 = 0.0;
    double dAngVel3 = 10.0;
    m_kModule.Initialize(dTime,dDeltaTime,dTheta,dPhi,dPsi,dAngVel1,dAngVel2,
        dAngVel3);

    m_fMaxPhi = Mathf::HALF_PI - Mathf::ATan(2.0f/3.0f);

    // set up the scene graph
    CreateScene();
    m_spkTopRoot->Local.SetRotate(m_kModule.GetBodyAxes());

    // set up camera
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,100.0f);
    float fAngle = 0.1f*Mathf::PI;
    float fCos = Mathf::Cos(fAngle), fSin = Mathf::Sin(fAngle);
    Vector3f kCLoc(4.0f,0.0f,2.0f);
    Vector3f kCDir(-fCos,0.0f,-fSin);
    Vector3f kCUp(-fSin,0.0f,fCos);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS(0.0f);
    m_spkScene->UpdateRS();

    InitializeCameraMotion(0.001f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void FreeTopFixedTip::OnTerminate ()
{
    m_spkScene = 0;
    m_spkWireframe = 0;
    m_spkTopRoot = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void FreeTopFixedTip::OnIdle ()
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
bool FreeTopFixedTip::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
    case 'G':
        DoPhysical();
        return true;
#endif

#ifdef CONTROL_FRAME_RATE
    case 'p':
    case 'P':
        m_bDoPhysics = !m_bDoPhysics;
        return true;
#endif
    }

    return false;
}
//----------------------------------------------------------------------------
void FreeTopFixedTip::CreateScene ()
{
    // scene -+--- floor
    //        |
    //        +--- vertical axis
    //        |
    //        +--- top root ---+--- top
    //                         |
    //                         +--- top axis

    m_spkScene = new Node(3);
    m_spkTopRoot = new Node(2);

    // wireframe mode
    m_spkWireframe = new WireframeState;
    m_spkScene->SetGlobalState(m_spkWireframe);

    m_spkScene->AttachChild(CreateFloor());
    m_spkScene->AttachChild(CreateAxisVertical());
    m_spkScene->AttachChild(m_spkTopRoot);
    m_spkTopRoot->AttachChild(CreateTop());
    m_spkTopRoot->AttachChild(CreateAxisTop());
}
//----------------------------------------------------------------------------
TriMesh* FreeTopFixedTip::CreateFloor ()
{
    StandardMesh kSM;
    kSM.UVs(true,false);
    TriMesh* pkFloor = kSM.Rectangle(2,2,32.0f,32.0f);

    Texture* pkTexture = new Texture(
        Image::Load(System::GetPath("../../Data/Wmif/Wood.wmif")));
    pkTexture->CoordU = Texture::WM_REPEAT;
    pkTexture->CoordV = Texture::WM_REPEAT;

    TextureEffect* pkEffect = new TextureEffect(pkTexture,kSM.GetUVs());
    pkFloor->SetEffect(pkEffect);

    return pkFloor;
}
//----------------------------------------------------------------------------
TriMesh* FreeTopFixedTip::CreateTop ()
{
    StandardMesh kSM;
    kSM.UVs(true,false);
    TriMesh* pkTop = kSM.Cylinder(32,32,1.0f,2.0f,true);

    // adjust shape
    int iVQuantity = pkTop->Vertices->GetQuantity();
    Vector3f* akVertex = pkTop->Vertices->GetData();
    int i;
    for (i = 0; i < iVQuantity; i++)
    {
        float fZ = akVertex[i].Z() + 1.0f;
        float fR = 0.75f*( fZ >= 1.5f ? 4.0f-2.0f*fZ : fZ/1.5f );
        float fInvLength = Mathf::InvSqrt(akVertex[i].X()*akVertex[i].X() +
            akVertex[i].Y()*akVertex[i].Y());
        akVertex[i].X() *= fR*fInvLength;
        akVertex[i].Y() *= fR*fInvLength;
    }
    pkTop->UpdateMS(false);
    pkTop->Local.SetTranslate(Vector3f::UNIT_Z);

    // adjust texture repeat
    Vector2f* akUV = kSM.GetUVs()->GetData();
    for (i = 0; i < iVQuantity; i++)
        akUV[i].X() *= 4.0f;

    Texture* pkTexture = new Texture(
        Image::Load(System::GetPath("../../Data/Wmif/TopTexture.wmif")));
    pkTexture->CoordU = Texture::WM_REPEAT;
    pkTexture->CoordV = Texture::WM_REPEAT;

    TextureEffect* pkEffect = new TextureEffect(pkTexture,kSM.GetUVs());
    pkTop->SetEffect(pkEffect);

    return pkTop;
}
//----------------------------------------------------------------------------
Polyline* FreeTopFixedTip::CreateAxisTop ()
{
    Vector3f* akVertex = new Vector3f[2];
    akVertex[0] = Vector3f::ZERO;
    akVertex[1] = 4.0f*Vector3f::UNIT_Z;
    Vector3fArray* pkVertices = new Vector3fArray(2,akVertex);

    Polyline* pkAxisTop = new Polyline(pkVertices,false,true);

    ColorRGB* akColor = new ColorRGB[2];
    akColor[0] = ColorRGB::WHITE;
    akColor[1] = ColorRGB::WHITE;
    ColorRGBArray* pkColors = new ColorRGBArray(2,akColor);

    VertexColorEffect* pkEffect = new VertexColorEffect(pkColors);
    pkAxisTop->SetEffect(pkEffect);

    return pkAxisTop;
}
//----------------------------------------------------------------------------
Polyline* FreeTopFixedTip::CreateAxisVertical ()
{
    Vector3f* akVertex = new Vector3f[2];
    akVertex[0] = Vector3f::ZERO;
    akVertex[1] = 4.0f*Vector3f::UNIT_Z;
    Vector3fArray* pkVertices = new Vector3fArray(2,akVertex);

    Polyline* pkAxisVertical = new Polyline(pkVertices,false,true);

    ColorRGB* akColor = new ColorRGB[2];
    akColor[0] = ColorRGB::BLACK;
    akColor[1] = ColorRGB::BLACK;
    ColorRGBArray* pkColors = new ColorRGBArray(2,akColor);

    VertexColorEffect* pkEffect = new VertexColorEffect(pkColors);
    pkAxisVertical->SetEffect(pkEffect);

    return pkAxisVertical;
}
//----------------------------------------------------------------------------
void FreeTopFixedTip::DoPhysical ()
{
    // Stop the simulation when the top edge reaches the ground.
    if ( m_kModule.GetPhi() >= m_fMaxPhi )
    {
        // EXERCISE.  Instead of stopping the top, maintain its phi value
        // at m_fMaxPhi so that the top continues to roll on the ground.
        // In addition, arrange for the top to slow down while rolling on
        // the ground, eventually coming to a stop.
        return;
    }

    // move the top
    m_kModule.Update();
    m_spkTopRoot->Local.SetRotate(m_kModule.GetBodyAxes());
    m_spkTopRoot->UpdateGS(0.0f);
}
//----------------------------------------------------------------------------
