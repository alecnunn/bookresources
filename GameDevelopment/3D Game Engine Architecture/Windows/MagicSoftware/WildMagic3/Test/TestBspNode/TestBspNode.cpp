// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "TestBspNode.h"

WM3_WINDOW_APPLICATION(TestBspNode);

//----------------------------------------------------------------------------
TestBspNode::TestBspNode ()
    :
    WindowApplication3("TestBspNode",0,0,640,480,ColorRGB(0.9f,0.9f,0.9f))
{
}
//----------------------------------------------------------------------------
bool TestBspNode::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    CreateScene();

    // set camera
    m_spkCamera->SetFrustum(-0.055f,0.055f,-0.04125f,0.04125f,0.1f,100.0f);
    Vector3f kCLoc(0.0f,-1.0f,0.1f);
    Vector3f kCDir(0.0f,1.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
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
void TestBspNode::OnTerminate ()
{
    m_spkScene = 0;
    m_spkBsp = 0;
    m_spkWireframe = 0;
    m_spkRCull = 0;
    m_spkRWireframe = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void TestBspNode::OnIdle ()
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
bool TestBspNode::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
    }

    return false;
}
//----------------------------------------------------------------------------
void TestBspNode::CreateScene ()
{
    m_spkScene = new Node;
    m_spkWireframe = new WireframeState;
    m_spkScene->SetGlobalState(m_spkWireframe);

    // Disable z-buffering.
    ZBufferState* pkZS = new ZBufferState;
    pkZS->Enabled = false;
    pkZS->Writable = false;
    m_spkScene->SetGlobalState(pkZS);

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

    // Partition the region above the ground into 5 convex pieces.  Each
    // plane is perpendicular to the ground (not required generally).  A
    // rectangle mesh representing the plane is attached to the plane as the
    // coplanar child.  Rectangles will be attached to each binary separating
    // plane, just for visualization purposes.  Disable back-face culling so
    // the rectangles are always visible.  Enable wireframe to see through the
    // rectangles.
    m_spkRCull = new CullState;
    m_spkRCull->Enabled = false;
    m_spkRWireframe = new WireframeState;
    m_spkRWireframe->Enabled = true;

    Vector2f kV0(-1.0f,1.0f);
    Vector2f kV1(1.0f,-1.0f);
    Vector2f kV2(-0.25f,0.25f);
    Vector2f kV3(-1.0f,-1.0f);
    Vector2f kV4(0.0f,0.0f);
    Vector2f kV5(1.0f,0.5f);
    Vector2f kV6(-0.75f,-7.0f/12.0f);
    Vector2f kV7(-0.75f,0.75f);
    Vector2f kV8(1.0f,1.0f);

    BspNode* pkBsp0 = CreateNode(kV0,kV1,ColorRGBA(1.0f,0.0f,0.0f,1.0f));
    BspNode* pkBsp1 = CreateNode(kV2,kV3,ColorRGBA(0.0f,0.5f,0.0f,1.0f));
    BspNode* pkBsp2 = CreateNode(kV4,kV5,ColorRGBA(0.0f,0.0f,1.0f,1.0f));
    BspNode* pkBsp3 = CreateNode(kV6,kV7,ColorRGBA(0.0f,0.0f,0.0f,1.0f));

    m_spkBsp = pkBsp0;
    pkBsp0->AttachPositiveChild(pkBsp1);
    pkBsp0->AttachNegativeChild(pkBsp2);
    pkBsp1->AttachPositiveChild(pkBsp3);

    // Attach an object in each convex region.  Since at least one object
    // is nonconvex, z-buffering must be enabled for that object.  However,
    // only z-writes need to occur for the convex objects.
    pkTexture = new Texture(
        Image::Load(System::GetPath("../../Data/Wmif/Flower.wmif")));

    float fHeight = 0.1f;
    Vector2f kCenter;
    TriMesh* pkMesh;

    // (R0) Create a torus mesh.  The torus is not convex, so z-buffering is
    // required to obtain correct drawing.
    pkMesh = kSM.Torus(16,16,1.0f,0.25f);
    pkMesh->SetGlobalState(new ZBufferState);
    pkEffect = new TextureEffect(pkTexture,kSM.GetUVs());
    pkMesh->SetEffect(pkEffect);
    pkMesh->Local.SetUniformScale(0.1f);
    kCenter = (kV2+kV6+kV7)/3.0f;
    pkMesh->Local.SetTranslate(Vector3f(kCenter.X(),kCenter.Y(),fHeight));
    pkBsp3->AttachPositiveChild(pkMesh);

    // (R1) create a sphere mesh
    pkMesh = kSM.Sphere(32,16,1.0f);
    pkEffect = new TextureEffect(pkTexture,kSM.GetUVs());
    pkMesh->SetEffect(pkEffect);
    pkMesh->Local.SetUniformScale(0.1f);
    kCenter = (kV0+kV3+kV6+kV7)/4.0f;
    pkMesh->Local.SetTranslate(Vector3f(kCenter.X(),kCenter.Y(),fHeight));
    pkBsp3->AttachNegativeChild(pkMesh);

    // (R2) create a tetrahedron
    pkMesh = kSM.Tetrahedron();
    pkEffect = new TextureEffect(pkTexture,kSM.GetUVs());
    pkMesh->SetEffect(pkEffect);
    pkMesh->Local.SetUniformScale(0.1f);
    kCenter = (kV1+kV2+kV3)/3.0f;
    pkMesh->Local.SetTranslate(Vector3f(kCenter.X(),kCenter.Y(),fHeight));
    pkBsp1->AttachNegativeChild(pkMesh);

    // (R3) create a hexahedron
    pkMesh = kSM.Hexahedron();
    pkEffect = new TextureEffect(pkTexture,kSM.GetUVs());
    pkMesh->SetEffect(pkEffect);
    pkMesh->Local.SetUniformScale(0.1f);
    kCenter = (kV1+kV4+kV5)/3.0f;
    pkMesh->Local.SetTranslate(Vector3f(kCenter.X(),kCenter.Y(),fHeight));
    pkBsp2->AttachPositiveChild(pkMesh);

    // (R4) create an octahedron
    pkMesh = kSM.Octahedron();
    pkEffect = new TextureEffect(pkTexture,kSM.GetUVs());
    pkMesh->SetEffect(pkEffect);
    pkMesh->Local.SetUniformScale(0.1f);
    kCenter = (kV0+kV4+kV5+kV8)/4.0f;
    pkMesh->Local.SetTranslate(Vector3f(kCenter.X(),kCenter.Y(),fHeight));
    pkBsp2->AttachNegativeChild(pkMesh);

    m_spkScene->AttachChild(m_spkBsp);
}
//----------------------------------------------------------------------------
BspNode* TestBspNode::CreateNode (const Vector2f& rkV0, const Vector2f& rkV1,
    const ColorRGBA& rkColor)
{
    Vector2f kDir = rkV1 - rkV0;
    Vector3f kNormal(kDir.Y(),-kDir.X(),0.0f);
    kNormal.Normalize();
    float fConstant = kNormal.Dot(Vector3f(rkV0.X(),rkV0.Y(),0.0f));
    float fXExtent = 0.5f*kDir.Length();
    float fYExtent = 0.125f;
    Vector3f kTrn(0.5f*(rkV0.X()+rkV1.X()),0.5f*(rkV0.Y()+rkV1.Y()),
        fYExtent+1e-03f);
    Matrix3f kRot =
        Matrix3f(Vector3f::UNIT_Z,Mathf::ATan2(kDir.Y(),kDir.X())) *
        Matrix3f(Vector3f::UNIT_X,Mathf::HALF_PI);

    BspNode* pkBsp = new BspNode(Plane3f(kNormal,fConstant));

    TriMesh* pkRect = StandardMesh().Rectangle(2,2,fXExtent,fYExtent);
    pkRect->Local.SetTranslate(kTrn);
    pkRect->Local.SetRotate(kRot);

    ColorRGBA* akColor = new ColorRGBA[4];
    for (int i = 0; i < 4; i++)
        akColor[i] = rkColor;

    ColorRGBAArray* pkColors = new ColorRGBAArray(4,akColor);
    VertexColorEffect* pkEffect = new VertexColorEffect(pkColors);
    pkRect->SetEffect(pkEffect);

    pkRect->SetGlobalState(m_spkRCull);
    pkRect->SetGlobalState(m_spkRWireframe);

    pkBsp->AttachCoplanarChild(pkRect);

    return pkBsp;
}
//----------------------------------------------------------------------------
void TestBspNode::TestStream ()
{
    Stream kOStream;
    kOStream.Insert(m_spkScene);
    kOStream.Save("TestBspNode.wmof");

    Stream kIStream;
    kIStream.Load("TestBspNode.wmof");
    NodePtr spkScene = (Node*)kIStream.GetObjectAt(0);
}
//----------------------------------------------------------------------------
