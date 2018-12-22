// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "FoucaultPendulum.h"

WM3_WINDOW_APPLICATION(FoucaultPendulum);

//#define SINGLE_STEP

//----------------------------------------------------------------------------
FoucaultPendulum::FoucaultPendulum ()
    :
    WindowApplication3("FoucaultPendulum",0,0,640,480,
        ColorRGB(0.819607f,0.909803f,0.713725f))
{
}
//----------------------------------------------------------------------------
bool FoucaultPendulum::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    // initialize the physics module
    m_kModule.AngularSpeed = 0.0001;
    m_kModule.Latitude = 0.25*Mathd::PI;
    m_kModule.GDivL = 1.0;

    double dTime = 0.0;
    double dDeltaTime = 0.001;
    double dTheta = 0.0;
    double dThetaDot = 0.1;
    double dPhi = 0.75;
    double dPhiDot = 0.0;
    m_kModule.Initialize(dTime,dDeltaTime,dTheta,dPhi,dThetaDot,dPhiDot);

    // set up the scene graph
    m_spkScene = new Node(3);
    m_spkScene->AttachChild(CreateFloor());
    m_spkScene->AttachChild(CreatePath());
    m_spkScene->AttachChild(CreatePendulum());
    m_spkWireframe = new WireframeState;
    m_spkScene->SetGlobalState(m_spkWireframe);
    ZBufferState* pkZS = new ZBufferState;
    pkZS->Enabled = false;
    m_spkScene->SetGlobalState(pkZS);

    // set up camera
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,100.0f);
    float fAngle = 0.1f*Mathf::PI;
    float fCos = Mathf::Cos(fAngle), fSin = Mathf::Sin(fAngle);
    Vector3f kCLoc(23.0f,0.0f,8.0f);
    Vector3f kCDir(-fCos,0.0f,-fSin);
    Vector3f kCUp(-fSin,0.0f,fCos);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS(0.0f);
    m_spkScene->UpdateRS();

    InitializeCameraMotion(0.01f,0.001f);
    InitializeObjectMotion(m_spkScene);

    DoPhysical();
    return true;
}
//----------------------------------------------------------------------------
void FoucaultPendulum::OnTerminate ()
{
    m_spkScene = 0;
    m_spkPendulum = 0;
    m_spkWireframe = 0;
    m_spkPath = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void FoucaultPendulum::OnIdle ()
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
bool FoucaultPendulum::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
TriMesh* FoucaultPendulum::CreateFloor ()
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
Polypoint* FoucaultPendulum::CreatePath ()
{
    // particles used to display path of pendulum
    const int iPQuantity = 8192;
    Vector3f* akPVertex = new Vector3f[iPQuantity];
    Vector3fArray* pkVertices = new Vector3fArray(iPQuantity,akPVertex);
    ColorRGB* akPColor = new ColorRGB[iPQuantity];
    ColorRGBArray* pkColors = new ColorRGBArray(iPQuantity,akPColor);
    for (int i = 0; i < iPQuantity; i++)
    {
        akPVertex[i] = Vector3f::ZERO;
        akPColor[i] = ColorRGB::WHITE;
    }
    m_spkPath = new Polypoint(pkVertices);
    m_spkPath->SetActiveQuantity(0);
    m_iNextPoint = 0;
    m_fColorDiff = 1.0f/(float)iPQuantity;

    VertexColorEffect* pkEffect = new VertexColorEffect(pkColors);
    m_spkPath->SetEffect(pkEffect);

    return m_spkPath;
}
//----------------------------------------------------------------------------
Node* FoucaultPendulum::CreatePendulum ()
{
    StandardMesh kSM;

    // pendulum rod
    kSM.Normals(true,false);
    TriMesh* pkRod = kSM.Cylinder(2,8,0.05f,12.0f,true);
    pkRod->Local.SetTranslate(Vector3f(0.0f,0.0f,10.0f));

    // pendulum bulb
    TriMesh* pkBulb = kSM.Sphere(16,32,2.0f);
    int iVQuantity = pkBulb->Vertices->GetQuantity();
    Vector3f* akVertex = pkBulb->Vertices->GetData();
    int i;
    for (i = 0; i < iVQuantity; i++)
        akVertex[i].Z() += 2.0f;

    // adjust vertices to form a pair of joined cones
    iVQuantity = pkBulb->Vertices->GetQuantity();
    akVertex = pkBulb->Vertices->GetData();
    for (i = 0; i < iVQuantity; i++)
    {
        float fR = Mathf::Sqrt(akVertex[i].X()*akVertex[i].X() +
            akVertex[i].Y()*akVertex[i].Y());
        float fZ = akVertex[i].Z();

        if ( fZ >= 2.0f )
            fZ = 4.0f - fR;
        else
            fZ = fR;

        akVertex[i].Z() = fZ;
    }

    // translate pendulum joint to origin (for purposes of rotation)
    iVQuantity = pkRod->Vertices->GetQuantity();
    akVertex = pkRod->Vertices->GetData();
    for (i = 0; i < iVQuantity; i++)
        akVertex[i].Z() -= 16.0f;
    pkRod->UpdateMS(true);

    iVQuantity = pkBulb->Vertices->GetQuantity();
    akVertex = pkBulb->Vertices->GetData();
    for (i = 0; i < iVQuantity; i++)
        akVertex[i].Z() -= 16.0f;
    pkBulb->UpdateMS(true);

    // group the objects into a single subtree
    m_spkPendulum = new Node(2);
    m_spkPendulum->AttachChild(pkRod);
    m_spkPendulum->AttachChild(pkBulb);

    // translate back to original model position
    m_spkPendulum->Local.SetTranslate(Vector3f(0.0f,0.0f,16.0f));

    // add a material for coloring purposes
    MaterialState* pkMS = new MaterialState;
    pkMS->Emissive = ColorRGBA::BLACK;
    pkMS->Ambient = ColorRGBA(0.1f,0.1f,0.1f,1.0f);
    pkMS->Diffuse = ColorRGBA(0.99607f,0.83920f,0.67059f,1.0f);
    pkMS->Specular = ColorRGBA::BLACK;
    pkMS->Shininess = 1.0f;
    m_spkPendulum->SetGlobalState(pkMS);

    Light* pkLight = new Light(Light::LT_DIRECTIONAL);
    pkLight->Ambient = ColorRGBA::WHITE;
    pkLight->Diffuse = ColorRGBA::WHITE;
    pkLight->Specular = ColorRGBA::BLACK;
    Vector3f kDir(-1.0f,-1.0f,0.0f);
    kDir.Normalize();
    pkLight->SetDirection(kDir);
    m_spkPendulum->SetLight(pkLight);

    pkLight = new Light(Light::LT_DIRECTIONAL);
    pkLight->Ambient = ColorRGBA::WHITE;
    pkLight->Diffuse = ColorRGBA::WHITE;
    pkLight->Specular = ColorRGBA::BLACK;
    kDir = Vector3f(1.0f,-1.0f,0.0f);
    kDir.Normalize();
    pkLight->SetDirection(kDir);
    m_spkPendulum->SetLight(pkLight);

    // depth buffering only for this part of the scene graph
    m_spkPendulum->SetGlobalState(new ZBufferState);

    return m_spkPendulum;
}
//----------------------------------------------------------------------------
void FoucaultPendulum::DoPhysical ()
{
    m_kModule.Update();

    // Update the pendulum mechanism.  The pendulum rod is attached at
    // (x,y,z) = (0,0,16).  The update here has the 16 hard-coded.
    m_spkPendulum->Local.SetRotate(m_kModule.GetOrientation());
    m_spkPendulum->UpdateGS(0.0f);

    // Draw only the active quantity of pendulum points for the initial
    // portion of the simulation.  Once all points are activated, then all
    // are drawn.
    int iVQuantity = m_spkPath->Vertices->GetQuantity();
    int iAQuantity = m_spkPath->GetActiveQuantity();
    if ( iAQuantity < iVQuantity )
        m_spkPath->SetActiveQuantity(++iAQuantity);

    // Add the new pendulum point to the point system.  The initial color is
    // white.  All previously known points have their colors decremented to
    // cause them to become dim over time.
    Vector3f kProj = m_spkPendulum->World.ApplyForward(
        Vector3f(0.0f,0.0f,-16.0f));
    kProj.Z() = 0.0f;
    Vector3f* akVertex = m_spkPath->Vertices->GetData();
    ColorRGB* akColor = m_spkPath->GetEffect()->ColorRGBs->GetData();
    akVertex[m_iNextPoint] = kProj;
    akColor[m_iNextPoint] = ColorRGB::WHITE;
    int i;
    for (i = 0; i < m_iNextPoint; i++)
    {
        akColor[i].R() -= m_fColorDiff;
        akColor[i].G() -= m_fColorDiff;
        akColor[i].B() -= m_fColorDiff;
    }
    for (i = m_iNextPoint+1; i < iVQuantity; i++)
    {
        akColor[i].R() -= m_fColorDiff;
        akColor[i].G() -= m_fColorDiff;
        akColor[i].B() -= m_fColorDiff;
    }

    // prepare for the next pendulum point
    if ( ++m_iNextPoint == iVQuantity )
        m_iNextPoint = 0;
}
//----------------------------------------------------------------------------
