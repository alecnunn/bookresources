// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "BallHill.h"

WM3_WINDOW_APPLICATION(BallHill);

//#define SINGLE_STEP

//----------------------------------------------------------------------------
BallHill::BallHill ()
    :
    WindowApplication3("BallHill",0,0,640,480,
        ColorRGB(0.839215f,0.894117f,0.972549f))
{
}
//----------------------------------------------------------------------------
bool BallHill::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    // set up the physics module
    m_kModule.Gravity = 1.0;
    m_kModule.A1 = 2.0;
    m_kModule.A2 = 1.0;
    m_kModule.A3 = 1.0;
    m_kModule.Radius = 0.1;

    double dTime = 0.0;
    double dDeltaTime = 0.01;
    double dY1 = 0.0;
    double dY2 = 0.0;
    double dY1Dot = 0.1;
    double dY2Dot = 0.1;
    m_kModule.Initialize(dTime,dDeltaTime,dY1,dY2,dY1Dot,dY2Dot);

    // set up the scene graph
    m_spkScene = new Node(4);

    // wireframe mode
    m_spkWireframe = new WireframeState;
    m_spkScene->SetGlobalState(m_spkWireframe);

    m_spkScene->AttachChild(CreateGround());
    m_spkScene->AttachChild(CreateHill());
    m_spkScene->AttachChild(CreateBall());
    m_spkScene->AttachChild(CreatePath());

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
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    InitializeCameraMotion(0.001f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void BallHill::OnTerminate ()
{
    m_spkScene = 0;
    m_spkWireframe = 0;
    m_spkBall = 0;
    m_spkPath = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void BallHill::OnIdle ()
{
    MeasureTime();
    MoveCamera();
    if ( MoveObject() )
        m_spkScene->UpdateGS(0.0f);

#ifndef SINGLE_STEP
    DoPhysical();
#endif

    m_pkRenderer->ClearBuffers();
    if ( m_pkRenderer->BeginScene() )
    {
        m_pkRenderer->DrawScene(m_spkScene);
        DrawFrameRate(8,GetHeight()-8,ColorRGBA::WHITE);
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool BallHill::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
    }

    return false;
}
//----------------------------------------------------------------------------
TriMesh* BallHill::CreateGround ()
{
    StandardMesh kSM;
    kSM.UVs(true,false);
    TriMesh* pkGround = kSM.Rectangle(2,2,32.0f,32.0f);

    // change the texture repeat
    Vector2fArrayPtr spkUVs = kSM.GetUVs();
    int iVQuantity = spkUVs->GetQuantity();
    Vector2f* akUV = spkUVs->GetData();
    for (int i = 0; i < iVQuantity; i++)
        akUV[i] *= 8.0f;

    Texture* pkTexture = new Texture(
        Image::Load(System::GetPath("../../Data/Wmif/Grass.wmif")));
    pkTexture->CoordU = Texture::WM_REPEAT;
    pkTexture->CoordV = Texture::WM_REPEAT;

    TextureEffect* pkEffect = new TextureEffect(pkTexture,spkUVs);
    pkGround->SetEffect(pkEffect);

    return pkGround;
}
//----------------------------------------------------------------------------
TriMesh* BallHill::CreateHill ()
{
    StandardMesh kSM;
    kSM.UVs(true,false);
    TriMesh* pkHill = kSM.Disk(32,32,2.0f);

    // change the texture repeat
    Vector2fArrayPtr spkUVs = kSM.GetUVs();
    int iVQuantity = spkUVs->GetQuantity();
    Vector2f* akUV = spkUVs->GetData();
    int i;
    for (i = 0; i < iVQuantity; i++)
        akUV[i] *= 8.0f;

    Texture* pkTexture = new Texture(
        Image::Load(System::GetPath("../../Data/Wmif/Gravel.wmif")));
    pkTexture->CoordU = Texture::WM_REPEAT;
    pkTexture->CoordV = Texture::WM_REPEAT;

    TextureEffect* pkEffect = new TextureEffect(pkTexture,spkUVs);
    pkHill->SetEffect(pkEffect);

    // adjust disk vertices to form elliptical paraboloid for the hill
    Vector3f* akVertex = pkHill->Vertices->GetData();
    for (i = 0; i < iVQuantity; i++)
    {
        akVertex[i].Z() = m_kModule.GetHeight(akVertex[i].X(),
            akVertex[i].Y());
    }
    pkHill->UpdateMS(true);

    return pkHill;
}
//----------------------------------------------------------------------------
TriMesh* BallHill::CreateBall ()
{
    StandardMesh kSM;
    kSM.UVs(true,false);
    m_fRadius = (float)m_kModule.Radius;
    m_spkBall = kSM.Sphere(16,16,m_fRadius);

    Vector3f kTrn = m_spkBall->Local.GetTranslate();
    kTrn.Z() = (float)m_kModule.A3 + m_fRadius;
    m_spkBall->Local.SetTranslate(kTrn);

    Texture* pkTexture = new Texture(
        Image::Load(System::GetPath("../../Data/Wmif/BallTexture.wmif")));
    pkTexture->CoordU = Texture::WM_REPEAT;
    pkTexture->CoordV = Texture::WM_REPEAT;

    TextureEffect* pkEffect = new TextureEffect(pkTexture,kSM.GetUVs());
    m_spkBall->SetEffect(pkEffect);

    UpdateBall();

    return m_spkBall;
}
//----------------------------------------------------------------------------
Polyline* BallHill::CreatePath ()
{
    // points used to display path of ball
    const int iPQuantity = 1024;
    Vector3f* akPVertex = new Vector3f[iPQuantity];
    ColorRGB* akPColor = new ColorRGB[iPQuantity];
    for (int i = 0; i < iPQuantity; i++)
    {
        akPVertex[i] = Vector3f::ZERO;
        akPColor[i] = ColorRGB::WHITE;
    }

    Vector3fArray* pkVertices = new Vector3fArray(iPQuantity,akPVertex);
    ColorRGBArray* pkColors = new ColorRGBArray(iPQuantity,akPColor);

    m_spkPath = new Polyline(pkVertices,false,true);
    m_spkPath->SetActiveQuantity(0);
    m_iNextPoint = 0;

    VertexColorEffect* pkEffect = new VertexColorEffect(pkColors);
    m_spkPath->SetEffect(pkEffect);

    return m_spkPath;
}
//----------------------------------------------------------------------------
void BallHill::DoPhysical ()
{
    // allow motion only while ball is above the ground level
    if ( m_spkBall->Local.GetTranslate().Z() <= m_fRadius )
        return;

    // move the ball
    m_kModule.Update();
    Vector3f kCenter = UpdateBall();

    // Draw only the active quantity of pendulum points for the initial
    // portion of the simulation.  Once all points are activated, then all
    // are drawn.
    int iVQuantity = m_spkPath->Vertices->GetQuantity();
    int iAQuantity = m_spkPath->GetActiveQuantity();
    if ( iAQuantity < iVQuantity )
        m_spkPath->SetActiveQuantity(++iAQuantity);

    // Update the path that the ball has followed.  The points are stored in
    // a circular queue, so the oldest points eventually disappear and are
    // reused for the new points.
    m_spkPath->Vertices->GetData()[m_iNextPoint] = kCenter;
    if ( ++m_iNextPoint == iVQuantity )
        m_iNextPoint = 0;
}
//----------------------------------------------------------------------------
Vector3f BallHill::UpdateBall ()
{
    // Compute the location of the center of the ball and the incremental
    // rotation implied by its motion.
    Vector3f kCenter;
    Matrix3f kIncrRot;
    m_kModule.GetData(kCenter,kIncrRot);

    // update the ball position and orientation
    m_spkBall->Local.SetTranslate(kCenter);
    m_spkBall->UpdateGS();
    m_spkBall->Local.SetRotate(kIncrRot*m_spkBall->Local.GetRotate());

    // return the new ball center for further use by application
    return kCenter;
}
//----------------------------------------------------------------------------
