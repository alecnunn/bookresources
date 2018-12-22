// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "TestGlossMap.h"

WM3_WINDOW_APPLICATION(TestGlossMap);

//----------------------------------------------------------------------------
TestGlossMap::TestGlossMap ()
    :
    WindowApplication3("TestGlossMap",0,0,640,480,
        ColorRGB(0.85f,0.85f,0.85f))
{
}
//----------------------------------------------------------------------------
bool TestGlossMap::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    // create the scene
    m_spkScene = new Node;
    Node* pkTrnNode = CreateModel();
    m_spkScene->AttachChild(pkTrnNode);

    // center-and-fit
    m_spkScene->UpdateGS();
    pkTrnNode->Local.SetTranslate(-m_spkScene->WorldBound->GetCenter());
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,100.0f);
    Vector3f kCDir(0.0f,0.0f,1.0f);
    Vector3f kCUp(0.0f,1.0f,0.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    Vector3f kCLoc = -3.0f*m_spkScene->WorldBound->GetRadius()*kCDir;
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    InitializeCameraMotion(0.01f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void TestGlossMap::OnTerminate ()
{
    m_spkScene = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void TestGlossMap::OnIdle ()
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
bool TestGlossMap::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
Node* TestGlossMap::CreateModel ()
{
    Node* pkModel = new Node(2);

    // attach a material
    MaterialState* pkMS = new MaterialState;
    pkMS->Ambient = ColorRGBA(0.2f,0.2f,0.2f,1.0f);
    pkMS->Diffuse = ColorRGBA(0.7f,0.7f,0.7f,1.0f);
    pkMS->Specular = ColorRGBA::WHITE;
    pkMS->Shininess = 25.0f;
    pkModel->SetGlobalState(pkMS);

    // attach a light
    Light* pkLight = new Light(Light::LT_DIRECTIONAL);
    pkLight->Ambient = ColorRGBA(0.1f,0.1f,0.1f,1.0f);
    pkLight->Diffuse = ColorRGBA(0.6f,0.6f,0.6f,1.0f);
    pkLight->Specular = ColorRGBA::WHITE;
    pkLight->SetDirection(Vector3f(0.0f,-1.0f,0.0f));
    pkLight->UpdateGS();
    pkModel->SetLight(pkLight);

    // create a square mesh
    Vector3f* akVertex = new Vector3f[4];
    akVertex[0] = Vector3f(-0.5f,0.0f,-0.5f);
    akVertex[1] = Vector3f(-0.5f,0.0f, 0.5f);
    akVertex[2] = Vector3f( 0.5f,0.0f, 0.5f);
    akVertex[3] = Vector3f( 0.5f,0.0f,-0.5f);
    Vector3fArray* pkVertices = new Vector3fArray(4,akVertex);

    Vector3f* akNormal = new Vector3f[4];
    akNormal[0] = Vector3f::UNIT_Y;
    akNormal[1] = Vector3f::UNIT_Y;
    akNormal[2] = Vector3f::UNIT_Y;
    akNormal[3] = Vector3f::UNIT_Y;
    Vector3fArray* pkNormals = new Vector3fArray(4,akNormal);

    Vector2f* akUV = new Vector2f[4];
    akUV[0] = Vector2f(1.0f,0.0f);
    akUV[1] = Vector2f(1.0f,1.0f);
    akUV[2] = Vector2f(0.0f,1.0f);
    akUV[3] = Vector2f(0.0f,0.0f);
    Vector2fArray* pkUVs = new Vector2fArray(4,akUV);

    int* aiIndex = new int[6];
    aiIndex[0] = 0;  aiIndex[1] = 1;  aiIndex[2] = 3;
    aiIndex[3] = 3;  aiIndex[4] = 1;  aiIndex[5] = 2;
    IntArray* pkIndices = new IntArray(6,aiIndex);

    // create a regular square
    TriMesh* pkRSquare = new TriMesh(pkVertices,pkIndices,false);
    pkRSquare->Normals = pkNormals;
    pkRSquare->Local.SetRotate(Matrix3f().FromAxisAngle(Vector3f::UNIT_X,
        -0.25f*Mathf::PI));
    pkRSquare->Local.SetTranslate(Vector3f(1.0f,-1.0f,0.0f));
    pkModel->AttachChild(pkRSquare);

    // create a gloss-mapped square
    TriMesh* pkGSquare = new TriMesh(pkVertices,pkIndices,false);
    pkGSquare->Normals = pkNormals;
    pkGSquare->Local.SetRotate(Matrix3f().FromAxisAngle(Vector3f::UNIT_X,
        -0.25f*Mathf::PI));
    pkGSquare->Local.SetTranslate(Vector3f(-1.0f,-1.0f,0.0f));
    pkModel->AttachChild(pkGSquare);

    Texture* pkTexture = new Texture(
        Image::Load(System::GetPath("../../Data/Wmif/Magic.wmif")));
    GlossMapEffect* pkEffect = new GlossMapEffect(pkTexture,pkUVs);
    pkGSquare->SetEffect(pkEffect);

    return pkModel;
}
//----------------------------------------------------------------------------
void TestGlossMap::TestStream ()
{
    Stream kOStream;
    kOStream.Insert(m_spkScene);
    kOStream.Save("TestGlossMap.wmof");

    Stream kIStream;
    kIStream.Load("TestGlossMap.wmof");
    NodePtr spkScene = (Node*)kIStream.GetObjectAt(0);
}
//----------------------------------------------------------------------------
