// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "TestProjectedTexture.h"

WM3_WINDOW_APPLICATION(TestProjectedTexture);

//----------------------------------------------------------------------------
TestProjectedTexture::TestProjectedTexture ()
    :
    WindowApplication3("TestProjectedTexture",0,0,640,480,
        ColorRGB(1.0f,1.0f,1.0f))
{
}
//----------------------------------------------------------------------------
bool TestProjectedTexture::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    // create scene
    m_spkScene = new Node;
    Node* pkTrnNode = new Node;
    m_spkScene->AttachChild(pkTrnNode);
    pkTrnNode->AttachChild(CreateModel());

    // center-and-fit
    m_spkScene->UpdateGS();
    pkTrnNode->Local.SetTranslate(-m_spkScene->WorldBound->GetCenter());
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1000.0f);
    Vector3f kCDir(0.0f,1.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    Vector3f kCLoc = -3.0f*m_spkScene->WorldBound->GetRadius()*kCDir;
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    InitializeCameraMotion(0.1f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void TestProjectedTexture::OnTerminate ()
{
    m_spkScene = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void TestProjectedTexture::OnIdle ()
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
bool TestProjectedTexture::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if ( WindowApplication3::OnKeyDown(ucKey,iX,iY) )
        return true;

    switch ( ucKey )
    {
    case 's':
    case 'S':
        TestStream();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
NodePtr TestProjectedTexture::CreateModel ()
{
    // load the face model
    Stream kIStream;
    kIStream.Load(System::GetPath("../../Data/Wmof/Face.wmof"));
    TriMeshPtr spkMesh = StaticCast<TriMesh>(kIStream.GetObjectAt(0));
    spkMesh->GenerateNormals();
    spkMesh->UpdateGS();

    // create a material for the face
    MaterialState* pkMaterial = new MaterialState;
    pkMaterial->Emissive = ColorRGBA(0.0f,0.0f,0.0f,1.0f);
    pkMaterial->Ambient = ColorRGBA(0.5f,0.5f,0.5f,1.0f);
    pkMaterial->Diffuse = ColorRGBA(0.99607f,0.83920f,0.67059f,1.0f);
    pkMaterial->Specular = ColorRGBA(0.8f,0.8f,0.8f,1.0f);
    pkMaterial->Shininess = 0.0f;
    spkMesh->SetGlobalState(pkMaterial);

    // create a directional light for the face
    Light* pkLight = new Light(Light::LT_DIRECTIONAL);
    pkLight->Ambient = ColorRGBA(0.25f,0.25f,0.25f,1.0f);
    pkLight->Diffuse = ColorRGBA::WHITE;
    pkLight->Specular = ColorRGBA::BLACK;
    pkLight->On = true;
    pkLight->SetDirection(Vector3f::UNIT_Y);  // scene-camera direction
    pkLight->UpdateGS();
    spkMesh->SetLight(pkLight);

    // the image to project onto the face
    Image* pkImage = Image::Load(
        System::GetPath("../../Data/Wmif/SunFire.wmif"));

    // create a camera to project the texture
    Camera* pkCamera = new Camera;
    pkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,10.0f);
    Vector3f kCDir(0.0f,1.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    Vector3f kCLoc = -303.0f*kCDir;
    pkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // create the effect
    ProjectedTextureEffect* pkEffect = new ProjectedTextureEffect(pkImage,
        pkCamera,Texture::AM_MODULATE);

    // Projected texture is a global effect, so it can only be attached to
    // a Node.
    NodePtr spkModel = new Node;
    spkModel->AttachChild(spkMesh);
    spkModel->SetEffect(pkEffect);

    return spkModel;
}
//----------------------------------------------------------------------------
void TestProjectedTexture::TestStream ()
{
    Stream kOStream;
    kOStream.Insert(m_spkScene);
    kOStream.Save("TestProjectedTexture.wmof");

    Stream kIStream;
    kIStream.Load("TestProjectedTexture.wmof");
    NodePtr spkScene = (Node*)kIStream.GetObjectAt(0);
}
//----------------------------------------------------------------------------
