// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "TestBillboardNode.h"

WM3_WINDOW_APPLICATION(TestBillboardNode);

//----------------------------------------------------------------------------
TestBillboardNode::TestBillboardNode ()
    :
    WindowApplication3("TestBillboardNode",0,0,640,480,
        ColorRGB(0.9f,0.9f,0.9f))
{
}
//----------------------------------------------------------------------------
bool TestBillboardNode::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    // set camera
    m_spkCamera->SetFrustum(-0.055f,0.055f,-0.04125f,0.04125f,0.1f,100.0f);
    Vector3f kCLoc(0.0f,-1.0f,0.25f);
    Vector3f kCDir(0.0f,1.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // set up scene
    CreateScene();
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    InitializeCameraMotion(0.001f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void TestBillboardNode::OnTerminate ()
{
    m_spkScene = 0;
    m_spkBillboard0 = 0;
    m_spkBillboard1 = 0;
    m_spkWireframe = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void TestBillboardNode::OnIdle ()
{
    MeasureTime();

    if ( MoveCamera() )
    {
        m_spkBillboard0->UpdateGS();
        m_spkBillboard1->UpdateGS();
    }

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
bool TestBillboardNode::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if ( WindowApplication3::OnKeyDown(ucKey,iX,iY) )
        return true;

    switch ( ucKey )
    {
    case 'w':
    case 'W':
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;
    case 's':
    case 'S':
        TestStream();
        return true;
    case 'c':
    case 'C':
        TestClone();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void TestBillboardNode::CreateScene ()
{
    m_spkScene = new Node;
    m_spkWireframe = new WireframeState;
    m_spkScene->SetGlobalState(m_spkWireframe);

    // Create the ground.  It covers a square with vertices (1,1,0), (1,-1,0),
    // (-1,1,0), and (-1,-1,0).  Multiply the texture coordinates by a factor
    // to enhance the wrap-around.
    StandardMesh kSM;
    kSM.UVs(true,false);
    TriMesh* pkGround = kSM.Rectangle(2,2,16.0f,16.0f);

    Vector2f* akUV = kSM.GetUVs()->GetData();
    for (int i = 0; i < pkGround->Vertices->GetQuantity(); i++)
        akUV[i] *= 128.0f;

    Texture* pkTexture = new Texture(
        Image::Load(System::GetPath("../../Data/Wmif/Horizontal.wmif")));
    pkTexture->CoordU = Texture::WM_REPEAT;
    pkTexture->CoordV = Texture::WM_REPEAT;
    Effect* pkEffect = new TextureEffect(pkTexture,kSM.GetUVs());
    pkGround->SetEffect(pkEffect);
    m_spkScene->AttachChild(pkGround);

    // Create a billboard node that causes a rectangle to always be facing
    // the camera.  This is the type of billboard for an avatar.
    m_spkBillboard0 = new BillboardNode(m_spkCamera);
    m_spkScene->AttachChild(m_spkBillboard0);

    // Create a rectangle mesh.  The mesh is in the xy-plane.  Do not apply
    // local transformations to the mesh.  Use the billboard node transforms
    // to control the mesh location and orientation.
    TriMesh* pkMesh = kSM.Rectangle(2,2,0.125f,0.25f);
    pkTexture = new Texture(
        Image::Load(System::GetPath("../../Data/Wmif/RedSky.wmif")));
    pkEffect = new TextureEffect(pkTexture,kSM.GetUVs());
    pkMesh->SetEffect(pkEffect);
    m_spkBillboard0->AttachChild(pkMesh);

    // The billboard rotation is about its model-space up-vector (0,1,0).  In
    // this application, world-space up is (0,0,1).  Locally rotate the
    // billboard so it's up-vector matches the world's.
    m_spkBillboard0->Local.SetTranslate(Vector3f(-0.25f,0.0f,0.25f));
    m_spkBillboard0->Local.SetRotate(Matrix3f().FromAxisAngle(
        Vector3f::UNIT_X,Mathf::HALF_PI));

    // Create a billboard node that causes an object to always be oriented
    // the same way relative to the camera.
    m_spkBillboard1 = new BillboardNode(m_spkCamera);
    m_spkScene->AttachChild(m_spkBillboard1);

    // Create a torus mesh.  Do not apply local transformations to the mesh.
    // Use the billboard node transforms to control the mesh location and
    // orientation.
    pkMesh = kSM.Torus(16,16,1.0f,0.25f);
    pkMesh->Local.SetUniformScale(0.1f);
    pkEffect = new TextureEffect(pkTexture,kSM.GetUVs());
    pkMesh->SetEffect(pkEffect);
    m_spkBillboard1->AttachChild(pkMesh);

    // The billboard rotation is about its model-space up-vector (0,1,0).  In
    // this application, world-space up is (0,0,1).  Locally rotate the
    // billboard so it's up-vector matches the world's.
    m_spkBillboard1->Local.SetTranslate(Vector3f(0.25f,0.0f,0.25f));
    m_spkBillboard1->Local.SetRotate(Matrix3f().FromAxisAngle(
        Vector3f::UNIT_X,Mathf::HALF_PI));
}
//----------------------------------------------------------------------------
void TestBillboardNode::TestStream ()
{
    Stream kOStream;
    kOStream.Insert(m_spkScene);
    kOStream.Save("TestBillboardNode.wmof");

    Stream kIStream;
    kIStream.Load("TestBillboardNode.wmof");
    NodePtr spkScene = (Node*)kIStream.GetObjectAt(0);
}
//----------------------------------------------------------------------------
void TestBillboardNode::TestClone ()
{
    ObjectPtr spkCopy = m_spkScene->Copy();
    Stream kOStream;
    kOStream.Insert(spkCopy);
    kOStream.Save("TestClone.wmof");
}
//----------------------------------------------------------------------------
