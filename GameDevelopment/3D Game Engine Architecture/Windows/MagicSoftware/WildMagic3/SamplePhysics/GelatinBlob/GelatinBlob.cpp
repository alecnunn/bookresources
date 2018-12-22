// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "GelatinBlob.h"
#include "Wm3EdgeKey.h"
#include <set>

WM3_WINDOW_APPLICATION(GelatinBlob);

//#define SINGLE_STEP

//----------------------------------------------------------------------------
GelatinBlob::GelatinBlob ()
    :
    WindowApplication3("GelatinBlob",0,0,640,480,
        ColorRGB(0.713725f,0.807843f,0.929411f))
{
    m_pkModule = 0;
}
//----------------------------------------------------------------------------
bool GelatinBlob::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    CreateScene();

    // center-and-fit for camera viewing
    m_spkScene->UpdateGS(0.0f);
    m_spkTrnNode->Local.SetTranslate(-m_spkScene->WorldBound->GetCenter());
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,100.0f);
    Vector3f kCDir(0.0f,1.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    Vector3f kCLoc = -3.0f*m_spkScene->WorldBound->GetRadius()*kCDir;
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS(0.0f);
    m_spkScene->UpdateRS();

    // camera turret and tumble mode
    InitializeCameraMotion(0.01f,0.01f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void GelatinBlob::OnTerminate ()
{
    delete m_pkModule;
    m_spkScene = 0;
    m_spkTrnNode = 0;
    m_spkSegments = 0;
    m_spkWireframe = 0;
    m_spkSphere = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void GelatinBlob::OnIdle ()
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
bool GelatinBlob::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
void GelatinBlob::CreateSphere ()
{
    StandardMesh kSM;
    kSM.UVs(true,false);
    m_spkSphere = kSM.Icosahedron();

    // texture for the water objects
    Texture* pkTexture = new Texture(
        Image::Load(System::GetPath("../../Data/Wmif/WaterA.wmif")));
    pkTexture->CoordU = Texture::WM_REPEAT;
    pkTexture->CoordV = Texture::WM_REPEAT;

    TextureEffect* pkEffect = new TextureEffect(pkTexture,kSM.GetUVs());
    m_spkSphere->SetEffect(pkEffect);

    // the texture has an alpha channel of 1/2
    AlphaState* pkAS = new AlphaState;
    pkAS->BlendEnabled = true;
    m_spkSphere->SetGlobalState(pkAS);
}
//----------------------------------------------------------------------------
void GelatinBlob::CreateSprings ()
{
    // The icosahedron has 12 vertices and 30 edges.  Each vertex is a
    // particle in the system.  Each edge represents a spring.  To keep the
    // icosahedron from collapsing, 12 immovable particles are added, each
    // outside the icosahedron in the normal direction above a vertex.  The
    // immovable particles are connected to their corresponding vertices with
    // springs.
    int iNumParticles = 24, iNumSprings = 42;

    // Viscous forces applied.  If you set viscosity to zero, the cuboid
    // wiggles indefinitely since there is no dissipation of energy.  If
    // the viscosity is set to a positive value, the oscillations eventually
    // stop.  The length of time to steady state is inversely proportional
    // to the viscosity.
#ifdef _DEBUG
    float fStep = 0.1f;
#else
    float fStep = 0.01f;  // simulation needs to run slower in release mode
#endif
    float fViscosity = 0.01f;
    m_pkModule = new PhysicsModule(iNumParticles,iNumSprings,fStep,
        fViscosity);

    // Set positions and velocities.  The first 12 positions are the vertices
    // of the icosahedron.  The last 12 are the extra particles added to
    // stabilize the system.
    const Vector3f* akVertex = m_spkSphere->Vertices->GetData();
    int i;
    for (i = 0; i < 12; i++)
    {
        m_pkModule->SetMass(i,1.0f);
        m_pkModule->Position(i) = akVertex[i];
        m_pkModule->Velocity(i) = 0.1f*Vector3f(Mathf::SymmetricRandom(),
            Mathf::SymmetricRandom(),Mathf::SymmetricRandom());
    }
    for (i = 12; i < 24; i++)
    {
        m_pkModule->SetMass(i,Mathf::MAX_REAL);
        m_pkModule->Position(i) = 2.0f*akVertex[i-12];
        m_pkModule->Velocity(i) = Vector3f::ZERO;
    }

    // get unique set of edges for icosahedron
    std::set<EdgeKey> kEdge;
    int iTQuantity = m_spkSphere->Indices->GetQuantity()/3;
    const int* piIndex = m_spkSphere->Indices->GetData();
    for (i = 0; i < iTQuantity; i++)
    {
        int iV0 = *piIndex++;
        int iV1 = *piIndex++;
        int iV2 = *piIndex++;
        kEdge.insert(EdgeKey(iV0,iV1));
        kEdge.insert(EdgeKey(iV1,iV2));
        kEdge.insert(EdgeKey(iV2,iV0));
    }

    // springs are at rest in the initial configuration
    const float fConstant = 10.0f;
    Vector3f kDiff;
    int iSpring = 0;
    std::set<EdgeKey>::iterator pkIter;
    for (pkIter = kEdge.begin(); pkIter != kEdge.end(); pkIter++)
    {
        const EdgeKey& rkKey = *pkIter;
        int iV0 = rkKey.V[0], iV1 = rkKey.V[1];
        kDiff = m_pkModule->Position(iV1) - m_pkModule->Position(iV0);
        m_pkModule->SetSpring(iSpring,iV0,iV1,fConstant,kDiff.Length());
        iSpring++;
    }
    for (i = 0; i < 12; i++)
    {
        kDiff = m_pkModule->Position(i+12) - m_pkModule->Position(i);
        m_pkModule->SetSpring(iSpring,i,i+12,fConstant,kDiff.Length());
        iSpring++;
    }
}
//----------------------------------------------------------------------------
void GelatinBlob::CreateSegments ()
{
    int iNumSprings = m_pkModule->GetNumSprings();
    m_spkSegments = new Node(iNumSprings);
    for (int i = 0; i < iNumSprings; i++)
    {
        int iV0, iV1;
        float fConstant, fLength;
        m_pkModule->GetSpring(i,iV0,iV1,fConstant,fLength);

        Vector3f* akVertex = new Vector3f[2];
        akVertex[0] = m_pkModule->Position(iV0);
        akVertex[1] = m_pkModule->Position(iV1);
        Vector3fArray* pkVertices = new Vector3fArray(2,akVertex);
        Polyline* pkPoly = new Polyline(pkVertices,false,true);

        ColorRGB* akColor = new ColorRGB[2];
        akColor[0] = ColorRGB::WHITE;
        akColor[1] = ColorRGB::WHITE;
        ColorRGBArray* pkColors = new ColorRGBArray(2,akColor);
        VertexColorEffect* pkEffect = new VertexColorEffect(pkColors);
        pkPoly->SetEffect(pkEffect);

        m_spkSegments->AttachChild(pkPoly);
    }
}
//----------------------------------------------------------------------------
void GelatinBlob::CreateScene ()
{
    m_spkScene = new Node(1);
    m_spkTrnNode = new Node(2);
    m_spkScene->AttachChild(m_spkTrnNode);
    m_spkWireframe = new WireframeState;
    m_spkScene->SetGlobalState(m_spkWireframe);

    CreateSphere();
    CreateSprings();
    CreateSegments();

    // segments are opaque--draw first (sphere is transparent)
    m_spkTrnNode->AttachChild(m_spkSegments);
    m_spkTrnNode->AttachChild(m_spkSphere);
}
//----------------------------------------------------------------------------
void GelatinBlob::DoPhysical ()
{
    m_pkModule->Update((float)System::GetTime());

    // Update sphere surface.  The particle system and sphere maintain their
    // own copy of the vertices, so this update is necessary.
    Vector3f* akVertex = m_spkSphere->Vertices->GetData();
    int i;
    for (i = 0; i < 12; i++)
        akVertex[i] = m_pkModule->Position(i);

    m_spkSphere->UpdateMS(false);

    // update the segments representing the springs
    int iNumSprings = m_pkModule->GetNumSprings();
    for (i = 0; i < iNumSprings; i++)
    {
        int iV0, iV1;
        float fConstant, fLength;
        m_pkModule->GetSpring(i,iV0,iV1,fConstant,fLength);

        Polyline* pkPoly = StaticCast<Polyline>(m_spkSegments->GetChild(i));
        Vector3f* akVertex = pkPoly->Vertices->GetData();
        akVertex[0] = m_pkModule->Position(iV0);
        akVertex[1] = m_pkModule->Position(iV1);
    }
}
//----------------------------------------------------------------------------
