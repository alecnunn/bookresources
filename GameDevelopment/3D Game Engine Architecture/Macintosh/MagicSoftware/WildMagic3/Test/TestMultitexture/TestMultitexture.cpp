// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "TestMultitexture.h"

WM3_WINDOW_APPLICATION(TestMultitexture);

//----------------------------------------------------------------------------
TestMultitexture::TestMultitexture ()
    :
    WindowApplication3("TestMultitexture",0,0,640,480,
        ColorRGB(0.5f,0.0f,1.0f))
{
}
//----------------------------------------------------------------------------
bool TestMultitexture::OnInitialize ()
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

    InitializeCameraMotion(0.001f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void TestMultitexture::OnTerminate ()
{
    m_spkScene = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void TestMultitexture::OnIdle ()
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
bool TestMultitexture::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
void TestMultitexture::CreateScene ()
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

    // texture coordinates
    Vector2f* akUV = new Vector2f[4];
    akUV[0] = Vector2f(1.0f,0.0f);
    akUV[1] = Vector2f(1.0f,1.0f);
    akUV[2] = Vector2f(0.0f,1.0f);
    akUV[3] = Vector2f(0.0f,0.0f);
    Vector2fArray* pkUVs = new Vector2fArray(4,akUV);

    // indices
    int* aiIndex = new int[6];
    aiIndex[0] = 0;  aiIndex[1] = 1; aiIndex[2] = 3;
    aiIndex[3] = 3;  aiIndex[4] = 1; aiIndex[5] = 2;
    IntArray* pkIndices = new IntArray(6,aiIndex);

    // images
    Image* pkDoorImage = Image::Load(
        System::GetPath("../../Data/Wmif/Door.wmif"));
    Image* pkRadialImage = Image::Load(
        System::GetPath("../../Data/Wmif/YellowRadial.wmif"));

    Matrix3f kRot(Vector3f::UNIT_X,-0.25f*Mathf::PI);

    // square with door texture only
    TriMesh* pkSquare0 = new TriMesh(pkVertices,pkIndices,false);
    pkSquare0->Local.SetRotate(kRot);
    pkSquare0->Local.SetTranslate(Vector3f(0.5f,-1.0f,0.0f));
    pkTrnNode->AttachChild(pkSquare0);
    Texture* pkTexture0 = new Texture(pkDoorImage);
    Effect* pkEffect0 = new TextureEffect(pkTexture0,pkUVs);
    pkSquare0->SetEffect(pkEffect0);

    // square with radial texture only
    TriMesh* pkSquare1 = new TriMesh(pkVertices,pkIndices,false);
    pkSquare1->Local.SetRotate(kRot);
    pkSquare1->Local.SetTranslate(Vector3f(-0.5f,-1.0f,0.0f));
    pkTrnNode->AttachChild(pkSquare1);
    Texture* pkTexture1 = new Texture(pkRadialImage);
    Effect* pkEffect1 = new TextureEffect(pkTexture1,pkUVs);
    pkSquare1->SetEffect(pkEffect1);

    // square with dark map (texture1 multiplied with texture0)
    TriMesh* pkSquare2 = new TriMesh(pkVertices,pkIndices,false);
    pkSquare2->Local.SetRotate(kRot);
    pkSquare2->Local.SetTranslate(Vector3f(1.0f,-2.0f,0.0f));
    pkTrnNode->AttachChild(pkSquare2);
    Texture* pkTexture2 = new Texture(pkRadialImage);
    Effect* pkEffect2 = new DarkMapEffect(pkTexture0,pkUVs,pkTexture2,pkUVs);
    pkSquare2->SetEffect(pkEffect2);

    // square with light map (hard addition: texture1 added to texture0)
    TriMesh* pkSquare3 = new TriMesh(pkVertices,pkIndices,false);
    pkSquare3->Local.SetRotate(kRot);
    pkSquare3->Local.SetTranslate(Vector3f(0.0f,-2.0f,0.0f));
    pkTrnNode->AttachChild(pkSquare3);
    Texture* pkTexture3 = new Texture(pkRadialImage);
    Effect* pkEffect3 = new LightMapEffect(pkTexture0,pkUVs,pkTexture3,pkUVs,
        true);
    pkSquare3->SetEffect(pkEffect3);

    // square with light map (soft addition: texture1 added to texture0)
    TriMesh* pkSquare4 = new TriMesh(pkVertices,pkIndices,false);
    pkSquare4->Local.SetRotate(kRot);
    pkSquare4->Local.SetTranslate(Vector3f(-1.0f,-2.0f,0.0f));
    pkTrnNode->AttachChild(pkSquare4);
    Texture* pkTexture4 = new Texture(pkRadialImage);
    Effect* pkEffect4 = new LightMapEffect(pkTexture0,pkUVs,pkTexture4,pkUVs,
        false);
    pkSquare4->SetEffect(pkEffect4);
}
//----------------------------------------------------------------------------
void TestMultitexture::TestStream ()
{
    Stream kOStream;
    kOStream.Insert(m_spkScene);
    kOStream.Save("TestMultitexture.wmof");

    Stream kIStream;
    kIStream.Load("TestMultitexture.wmof");
    NodePtr spkScene = (Node*)kIStream.GetObjectAt(0);
}
//----------------------------------------------------------------------------
