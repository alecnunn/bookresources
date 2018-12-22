// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "TestNonuniformScale.h"

WM3_WINDOW_APPLICATION(TestNonuniformScale);

//----------------------------------------------------------------------------
TestNonuniformScale::TestNonuniformScale ()
    :
    WindowApplication3("TestNonuniformScale",0,0,640,480,
        ColorRGB(0.5f,0.0f,1.0f))
{
}
//----------------------------------------------------------------------------
bool TestNonuniformScale::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    CreateScene();

    // set camera
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,100.0f);
    float fCos = 0.866025f, fSin = 0.5f;
    Vector3f kCLoc(0.0f,-4.0f,2.0f);
    Vector3f kCDir(0.0f,fCos,-fSin);
    Vector3f kCUp(0.0f,fSin,fCos);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    InitializeCameraMotion(0.01f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void TestNonuniformScale::OnTerminate ()
{
    m_spkScene = 0;
    m_spkMesh = 0;
    m_spkWireframe = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void TestNonuniformScale::OnIdle ()
{
    MeasureTime();

    MoveCamera();
    if ( MoveObject() )
        m_spkScene->UpdateGS();

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
bool TestNonuniformScale::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if ( WindowApplication3::OnKeyDown(ucKey,iX,iY) )
        return true;

    Vector3f kScale;

    switch ( ucKey )
    {
    case 'x':
        kScale = m_spkMesh->Local.GetScale();
        kScale.X() /= 1.1f;
        m_spkMesh->Local.SetScale(kScale);
        m_spkMesh->UpdateGS();
        return true;
    case 'X':
        kScale = m_spkMesh->Local.GetScale();
        kScale.X() *= 1.1f;
        m_spkMesh->Local.SetScale(kScale);
        m_spkMesh->UpdateGS();
        return true;
    case 'y':
        kScale = m_spkMesh->Local.GetScale();
        kScale.Y() /= 1.1f;
        m_spkMesh->Local.SetScale(kScale);
        m_spkMesh->UpdateGS();
        return true;
    case 'Y':
        kScale = m_spkMesh->Local.GetScale();
        kScale.Y() *= 1.1f;
        m_spkMesh->Local.SetScale(kScale);
        m_spkMesh->UpdateGS();
        return true;
    case 'z':
        kScale = m_spkMesh->Local.GetScale();
        kScale.Z() /= 1.1f;
        m_spkMesh->Local.SetScale(kScale);
        m_spkMesh->UpdateGS();
        return true;
    case 'Z':
        kScale = m_spkMesh->Local.GetScale();
        kScale.Z() *= 1.1f;
        m_spkMesh->Local.SetScale(kScale);
        m_spkMesh->UpdateGS();
        return true;
    case 'w':
    case 'W':
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;
    case 's':
    case 'S':
        TestStream();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void TestNonuniformScale::CreateScene ()
{
    m_spkScene = new Node;
    m_spkWireframe = new WireframeState;
    m_spkScene->SetGlobalState(m_spkWireframe);

    StandardMesh kSM;
    kSM.UVs(true,false);
    m_spkMesh = kSM.Dodecahedron();
    Texture* pkTexture = new Texture(
        Image::Load(System::GetPath("../../Data/Wmif/Flower.wmif")));
    Effect* pkEffect = new TextureEffect(pkTexture,kSM.GetUVs());
    m_spkMesh->SetEffect(pkEffect);
    m_spkScene->AttachChild(m_spkMesh);
}
//----------------------------------------------------------------------------
void TestNonuniformScale::TestStream ()
{
    Stream kOStream;
    kOStream.Insert(m_spkScene);
    kOStream.Save("TestNonuniformScale.wmof");

    Stream kIStream;
    kIStream.Load("TestNonuniformScale.wmof");
    NodePtr spkScene = (Node*)kIStream.GetObjectAt(0);
}
//----------------------------------------------------------------------------
