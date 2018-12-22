// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "TestMaterialTexture.h"

WM3_WINDOW_APPLICATION(TestMaterialTexture);

//----------------------------------------------------------------------------
TestMaterialTexture::TestMaterialTexture ()
    :
    WindowApplication3("TestMaterialTexture",0,0,640,480,
        ColorRGB(0.5f,0.0f,1.0f))
{
}
//----------------------------------------------------------------------------
bool TestMaterialTexture::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    CreateScene();

    // center-and-fit
    m_spkScene->UpdateGS();
    m_spkScene->GetChild(0)->Local.SetTranslate(
        -m_spkScene->WorldBound->GetCenter());
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,100.0f);
    Vector3f kCDir(0.0f,0.0f,1.0f);
    Vector3f kCUp(0.0f,1.0f,0.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    Vector3f kCLoc = -2.0f*m_spkScene->WorldBound->GetRadius()*kCDir;
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    InitializeCameraMotion(0.01f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void TestMaterialTexture::OnTerminate ()
{
    m_spkScene = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void TestMaterialTexture::OnIdle ()
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
bool TestMaterialTexture::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
void TestMaterialTexture::CreateScene ()
{
    m_spkScene = new Node;
    Node* pkTrnNode = new Node;
    m_spkScene->AttachChild(pkTrnNode);

    // vertices
    Vector3f* akVertex = new Vector3f[4];
    akVertex[0] = Vector3f(-0.5f,0.0f,-0.5f);
    akVertex[1] = Vector3f(-0.5f,0.0f, 0.5f);
    akVertex[2] = Vector3f( 0.5f,0.0f, 0.5f);
    akVertex[3] = Vector3f( 0.5f,0.0f,-0.5f);
    Vector3fArray* pkVertices = new Vector3fArray(4,akVertex);

    // indices
    int* aiIndex = new int[6];
    aiIndex[0] = 0;  aiIndex[1] = 1; aiIndex[2] = 3;
    aiIndex[3] = 3;  aiIndex[4] = 1; aiIndex[5] = 2;
    IntArray* pkIndices = new IntArray(6,aiIndex);

    // square with door texture only
    TriMesh* pkSquare = new TriMesh(pkVertices,pkIndices,false);
    pkSquare->Local.SetRotate(Matrix3f(Vector3f::UNIT_X,-0.25f*Mathf::PI));
    pkSquare->Local.SetTranslate(Vector3f(0.0f,-1.0f,0.0f));
    pkTrnNode->AttachChild(pkSquare);

    // create a texture effect
    Vector2f* akUV = new Vector2f[4];
    akUV[0] = Vector2f(1.0f,0.0f);
    akUV[1] = Vector2f(1.0f,1.0f);
    akUV[2] = Vector2f(0.0f,1.0f);
    akUV[3] = Vector2f(0.0f,0.0f);
    Vector2fArray* pkUVs = new Vector2fArray(4,akUV);

    Texture* pkTexture = new Texture(
        Image::Load(System::GetPath("../../Data/Wmif/Door.wmif")));
    pkTexture->Apply = Texture::AM_ADD;
    Effect* pkEffect = new TextureEffect(pkTexture,pkUVs);
    pkSquare->SetEffect(pkEffect);

    // create a material
    MaterialState* pkMaterial = new MaterialState;
    pkMaterial->Emissive = ColorRGBA(0.0f,0.0f,0.0f,1.0f);
    pkMaterial->Ambient = ColorRGBA(0.5f,0.5f,0.5f,1.0f);
    pkMaterial->Diffuse = ColorRGBA(0.0f,0.0f,1.0f,1.0f);
    pkMaterial->Specular = ColorRGBA(0.8f,0.8f,0.8f,1.0f);
    pkMaterial->Shininess = 0.0f;
    pkSquare->SetGlobalState(pkMaterial);

    // create a directional light
    Light* pkLight = new Light(Light::LT_DIRECTIONAL);
    pkLight->Ambient = ColorRGBA(0.25f,0.25f,0.25f,1.0f);
    pkLight->Diffuse = ColorRGBA::WHITE;
    pkLight->Specular = ColorRGBA::BLACK;
    pkLight->On = true;
    pkLight->SetDirection(-Vector3f::UNIT_Y);
    pkLight->UpdateGS();
    pkSquare->SetLight(pkLight);
}
//----------------------------------------------------------------------------
void TestMaterialTexture::TestStream ()
{
    Stream kOStream;
    kOStream.Insert(m_spkScene);
    kOStream.Save("TestMaterialTexture.wmof");

    Stream kIStream;
    kIStream.Load("TestMaterialTexture.wmof");
    NodePtr spkScene = (Node*)kIStream.GetObjectAt(0);
}
//----------------------------------------------------------------------------
