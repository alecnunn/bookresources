// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "TestCollision.h"

WM3_WINDOW_APPLICATION(TestCollision);

Vector2f TestCollision::ms_kBlueUV(0.25f,0.25f);
Vector2f TestCollision::ms_kCyanUV(0.75f,0.25f);
Vector2f TestCollision::ms_kRedUV(0.25f,0.75f);
Vector2f TestCollision::ms_kYellowUV(0.75f,0.75f);

//----------------------------------------------------------------------------
TestCollision::TestCollision ()
    :
    WindowApplication3("TestCollision",0,0,640,480,ColorRGB(1.0f,1.0f,1.0f))
{
}
//----------------------------------------------------------------------------
bool TestCollision::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    CreateScene();

    // set camera
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1000.0f);
    Vector3f kCLoc(4.0f,0.0f,0.0f);
    Vector3f kCDir(-1.0f,0.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    InitializeCameraMotion(0.01f,0.01f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void TestCollision::OnTerminate ()
{
    delete m_pkGroup;
    m_spkScene = 0;
    m_spkCyln0 = 0;
    m_spkCyln1 = 0;
    m_spkWireframe = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void TestCollision::OnIdle ()
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
bool TestCollision::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    // I really do mean to bypass the WindowApplication3 callback.
    if ( WindowApplication::OnKeyDown(ucKey,iX,iY) )
        return true;

    // move the tall/thin cylinder and activate the collision system
    if ( Transform(ucKey) )
        return true;

    switch ( ucKey )
    {
    case 'w':
    case 'W':
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void TestCollision::CreateScene ()
{
    // root of scene will have two cylinders as children
    m_spkScene = new Node(2);
    m_spkWireframe = new WireframeState;
    m_spkScene->SetGlobalState(m_spkWireframe);

    // set up texture image, texture, and texture state
    unsigned int* auiData = new unsigned int[4];
    auiData[0] = System::MakeRGB(0,0,255);    // blue
    auiData[1] = System::MakeRGB(0,255,255);  // cyan
    auiData[2] = System::MakeRGB(255,0,0);    // red
    auiData[3] = System::MakeRGB(255,255,0);  // yellow
    Image* pkImage = new Image(Image::IT_RGBA8888,2,2,
        (unsigned char*)auiData);
    Texture* pkTexture = new Texture(pkImage);
    TextureEffect* pkEffect;

    // create two cylinders, one short/think, one tall/thin
    StandardMesh kSM;
    kSM.UVs(true,false);
    Vector2fArrayPtr spkUVs;
    int i;

    m_spkCyln0 = kSM.Cylinder(8,16,1.0f,2.0f,false);
    spkUVs = kSM.GetUVs();
    for (i = 0; i < spkUVs->GetQuantity(); i++)
        spkUVs->GetData()[i] = ms_kBlueUV;
    pkEffect = new TextureEffect(pkTexture,spkUVs);
    m_spkCyln0->SetEffect(pkEffect);
    m_spkScene->AttachChild(m_spkCyln0);

    m_spkCyln1 = kSM.Cylinder(16,8,0.25,4.0,false);
    spkUVs = kSM.GetUVs();
    for (i = 0; i < spkUVs->GetQuantity(); i++)
        spkUVs->GetData()[i] = ms_kRedUV;
    pkEffect = new TextureEffect(pkTexture,spkUVs);
    m_spkCyln1->SetEffect(pkEffect);
    m_spkScene->AttachChild(m_spkCyln1);

    m_spkScene->UpdateGS(0.0f);
    m_spkScene->UpdateRS();

    // Set up collision stuff.  Record0 handles the collision response.
    // Record1 is not given a callback so that 'double processing' of the
    // events does not occur.

    CollisionRecord* pkRec0 = new CollisionRecord(m_spkCyln0,
        new BoxBVTree(m_spkCyln0,1,false),0,Response,this);

    CollisionRecord* pkRec1 = new CollisionRecord(m_spkCyln1,
        new BoxBVTree(m_spkCyln1,1,false),0,0,0);

    m_pkGroup = new CollisionGroup;
    m_pkGroup->Add(pkRec0);
    m_pkGroup->Add(pkRec1);
    m_pkGroup->TestIntersection();
}
//----------------------------------------------------------------------------
bool TestCollision::Transform (unsigned char ucKey)
{
    // Move the tall/thin cylinder.  After each motion, reset the texture
    // coordinates to the "no intersection" state, then let the collision
    // system test for intersection.  Any intersecting triangles have their
    // texture coordinates changed to the "intersection" state.

    float fTrnSpeed = 0.1f;
    float fRotSpeed = 0.1f;

    Matrix3f kRot, kIncr;
    Vector3f kTrn;

    switch ( ucKey )
    {
        case 'x':
            kTrn = m_spkCyln1->Local.GetTranslate();
            kTrn.X() -= fTrnSpeed;
            m_spkCyln1->Local.SetTranslate(kTrn);
            break;
        case 'X':
            kTrn = m_spkCyln1->Local.GetTranslate();
            kTrn.X() += fTrnSpeed;
            m_spkCyln1->Local.SetTranslate(kTrn);
            break;
        case 'y':
            kTrn = m_spkCyln1->Local.GetTranslate();
            kTrn.Y() -= fTrnSpeed;
            m_spkCyln1->Local.SetTranslate(kTrn);
            break;
        case 'Y':
            kTrn = m_spkCyln1->Local.GetTranslate();
            kTrn.Y() += fTrnSpeed;
            m_spkCyln1->Local.SetTranslate(kTrn);
            break;
        case 'z':
            kTrn = m_spkCyln1->Local.GetTranslate();
            kTrn.Z() -= fTrnSpeed;
            m_spkCyln1->Local.SetTranslate(kTrn);
            break;
        case 'Z':
            kTrn = m_spkCyln1->Local.GetTranslate();
            kTrn.Z() += fTrnSpeed;
            m_spkCyln1->Local.SetTranslate(kTrn);
            break;
        case 'r':
            kRot = m_spkCyln1->Local.GetRotate();
            kIncr.FromAxisAngle(Vector3f::UNIT_X,fRotSpeed);
            m_spkCyln1->Local.SetRotate(kIncr*kRot);
            break;
        case 'R':
            kRot = m_spkCyln1->Local.GetRotate();
            kIncr.FromAxisAngle(Vector3f::UNIT_X,-fRotSpeed);
            m_spkCyln1->Local.SetRotate(kIncr*kRot);
            break;
        case 'a':
            kRot = m_spkCyln1->Local.GetRotate();
            kIncr.FromAxisAngle(Vector3f::UNIT_Y,fRotSpeed);
            m_spkCyln1->Local.SetRotate(kIncr*kRot);
            break;
        case 'A':
            kRot = m_spkCyln1->Local.GetRotate();
            kIncr.FromAxisAngle(Vector3f::UNIT_Y,-fRotSpeed);
            m_spkCyln1->Local.SetRotate(kIncr*kRot);
            break;
        case 'p':
            kRot = m_spkCyln1->Local.GetRotate();
            kIncr.FromAxisAngle(Vector3f::UNIT_Z,fRotSpeed);
            m_spkCyln1->Local.SetRotate(kIncr*kRot);
            break;
        case 'P':
            kRot = m_spkCyln1->Local.GetRotate();
            kIncr.FromAxisAngle(Vector3f::UNIT_Z,-fRotSpeed);
            m_spkCyln1->Local.SetRotate(kIncr*kRot);
            break;
        default:
            return false;
    }

    // activate the collision system
    m_spkCyln1->UpdateGS(0.0f);
    ResetColors();
    m_pkGroup->TestIntersection();
    return true;
}
//----------------------------------------------------------------------------
void TestCollision::ResetColors ()
{
    int i, iVQuantity = m_spkCyln0->Vertices->GetQuantity();
    Vector2f* akUV = m_spkCyln0->GetEffect()->UVs[0]->GetData();
    for (i = 0; i < iVQuantity; i++)
        akUV[i] = ms_kBlueUV;

    iVQuantity = m_spkCyln1->Vertices->GetQuantity();
    akUV = m_spkCyln1->GetEffect()->UVs[0]->GetData();
    for (i = 0; i < iVQuantity; i++)
        akUV[i] = ms_kRedUV;
}
//----------------------------------------------------------------------------
void TestCollision::Response (CollisionRecord& rkRecord0, int iT0,
    CollisionRecord& rkRecord1, int iT1, Intersector<float,Vector3f>*)
{
    TriMesh* pkMesh;
    const int* aiIndex;
    Vector2f* akUV;
    int i0, i1, i2;

    // mesh0 triangles that are intersecting change from blue to cyan
    pkMesh = rkRecord0.GetMesh();
    aiIndex = pkMesh->Indices->GetData();
    i0 = aiIndex[3*iT0];
    i1 = aiIndex[3*iT0+1];
    i2 = aiIndex[3*iT0+2];
    akUV = pkMesh->GetEffect()->UVs[0]->GetData();
    akUV[i0] = ms_kCyanUV;
    akUV[i1] = ms_kCyanUV;
    akUV[i2] = ms_kCyanUV;

    // mesh1 triangles that are intersecting change from red to yellow
    pkMesh = rkRecord1.GetMesh();
    aiIndex = pkMesh->Indices->GetData();
    i0 = aiIndex[3*iT1];
    i1 = aiIndex[3*iT1+1];
    i2 = aiIndex[3*iT1+2];
    akUV = pkMesh->GetEffect()->UVs[0]->GetData();
    akUV[i0] = ms_kYellowUV;
    akUV[i1] = ms_kYellowUV;
    akUV[i2] = ms_kYellowUV;
}
//----------------------------------------------------------------------------
