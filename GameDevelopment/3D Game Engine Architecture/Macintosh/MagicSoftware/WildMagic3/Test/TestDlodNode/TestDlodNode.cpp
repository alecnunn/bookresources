// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "TestDlodNode.h"

WM3_WINDOW_APPLICATION(TestDlodNode);

//----------------------------------------------------------------------------
TestDlodNode::TestDlodNode ()
    :
    WindowApplication3("TestDlodNode",0,0,640,480,ColorRGB(0.5f,0.0f,1.0f))
{
}
//----------------------------------------------------------------------------
bool TestDlodNode::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    CreateScene();

    // set camera
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,100.0f);
    Vector3f kCLoc(0.0f,-4.0f,0.0f);
    Vector3f kCDir(0.0f,1.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
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
void TestDlodNode::OnTerminate ()
{
    m_spkScene = 0;
    m_spkDlod = 0;
    m_spkWireframe = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void TestDlodNode::OnIdle ()
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
bool TestDlodNode::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
void TestDlodNode::CreateScene ()
{
    m_spkScene = new Node;
    m_spkWireframe = new WireframeState;
    m_spkScene->SetGlobalState(m_spkWireframe);

    m_spkDlod = new DlodNode;
    m_spkScene->AttachChild(m_spkDlod);

    // material to be shared by all objects
    MaterialState* pkMS = new MaterialState;
    pkMS->Diffuse = ColorRGBA(0.5f,0.0f,0.5f,1.0f);

    // light for the material
    Light* pkLight = new Light(Light::LT_DIRECTIONAL);
    pkLight->Ambient = ColorRGBA(0.5f,0.5f,0.5f,1.0f);
    pkLight->Diffuse = ColorRGBA::WHITE;
    pkLight->Specular = ColorRGBA::BLACK;
    pkLight->On = true;
    pkLight->SetDirection(Vector3f::UNIT_Y);
    pkLight->UpdateGS();
    m_spkDlod->SetLight(pkLight);

    // mesh creator
    StandardMesh kSM;
    kSM.Normals(true,false);
    TriMesh* pkMesh;

    // create a sphere mesh (child 0)
    pkMesh = kSM.Sphere(32,16,1.0f);
    pkMesh->SetGlobalState(pkMS);
    m_spkDlod->AttachChild(pkMesh);

    // create an icosahedron (child 1)
    pkMesh = kSM.Icosahedron();
    pkMesh->SetGlobalState(pkMS);
    m_spkDlod->AttachChild(pkMesh);

    // create a dodecahedron (child 2)
    pkMesh = kSM.Dodecahedron();
    pkMesh->SetGlobalState(pkMS);
    m_spkDlod->AttachChild(pkMesh);

    // create an octahedron (child 3)
    pkMesh = kSM.Octahedron();
    pkMesh->SetGlobalState(pkMS);
    m_spkDlod->AttachChild(pkMesh);

    // create a hexahedron (child 4)
    pkMesh = kSM.Hexahedron();
    pkMesh->SetGlobalState(pkMS);
    m_spkDlod->AttachChild(pkMesh);

    // create a tetrahedron (child 5)
    pkMesh = kSM.Tetrahedron();
    pkMesh->SetGlobalState(pkMS);
    m_spkDlod->AttachChild(pkMesh);

    // set distance intervals
    m_spkDlod->SetModelDistance(0,0.0f,10.0f);
    m_spkDlod->SetModelDistance(1,10.0f,20.0f);
    m_spkDlod->SetModelDistance(2,20.0f,30.0f);
    m_spkDlod->SetModelDistance(3,30.0f,40.0f);
    m_spkDlod->SetModelDistance(4,40.0f,50.0f);
    m_spkDlod->SetModelDistance(5,50.0f,100.0f);

    // set model LOD center
    m_spkDlod->ModelCenter() = Vector3f::ZERO;
}
//----------------------------------------------------------------------------
void TestDlodNode::TestStream ()
{
    Stream kOStream;
    kOStream.Insert(m_spkScene);
    kOStream.Save("TestDlodNode.wmof");

    Stream kIStream;
    kIStream.Load("TestDlodNode.wmof");
    NodePtr spkScene = (Node*)kIStream.GetObjectAt(0);
}
//----------------------------------------------------------------------------
