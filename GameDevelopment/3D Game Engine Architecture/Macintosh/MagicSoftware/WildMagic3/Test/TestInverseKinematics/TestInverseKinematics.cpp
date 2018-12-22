// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "TestInverseKinematics.h"

WM3_WINDOW_APPLICATION(TestInverseKinematics);

//----------------------------------------------------------------------------
TestInverseKinematics::TestInverseKinematics ()
    :
    WindowApplication3("TestInverseKinematics",0,0,640,480,
        ColorRGB(0.9f,0.9f,0.9f))
{
}
//----------------------------------------------------------------------------
bool TestInverseKinematics::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    // Scene
    //     GroundMesh
    //     IKSystem
    //         Goal
    //             GoalCube
    //         Joint0
    //             OriginCube
    //             Rod
    //             Joint1
    //                 EndCube

    // create objects
    m_spkScene = new Node(2);
    m_spkWireframe = new WireframeState;
    m_spkScene->SetGlobalState(m_spkWireframe);

    m_spkIKSystem = new Node(3);
    m_spkGoal = new Node(1);
    m_spkJoint0 = new Node(2);
    m_spkJoint1 = new Node(1);
    m_spkRod = CreateRod();

    // transform objects
    m_spkGoal->Local.SetTranslate(2.0f*Vector3f::UNIT_Y);
    m_spkJoint1->Local.SetTranslate(Vector3f::UNIT_X);

    // set parent-child links
    m_spkScene->AttachChild(CreateGround());
    m_spkScene->AttachChild(m_spkIKSystem);
    m_spkIKSystem->AttachChild(m_spkGoal);
    m_spkGoal->AttachChild(CreateCube());
    m_spkIKSystem->AttachChild(m_spkJoint0);
    m_spkJoint0->AttachChild(CreateCube());
    m_spkJoint0->AttachChild(m_spkRod);
    m_spkJoint0->AttachChild(m_spkJoint1);
    m_spkJoint1->AttachChild(CreateCube());

    // create goal
    IKGoalPtr* aspkGoal = new IKGoalPtr[1];
    aspkGoal[0] = new IKGoal(m_spkGoal,m_spkJoint1,1.0f);

    // create joints
    IKJointPtr* aspkJoint = new IKJointPtr[2];

    IKGoalPtr* aspkJGoal0 = new IKGoalPtr[1];
    aspkJGoal0[0] = aspkGoal[0];
    aspkJoint[0] = new IKJoint(m_spkJoint0,1,aspkJGoal0);
    aspkJoint[0]->AllowRotation[2] = true;

    IKGoalPtr* aspkJGoal1 = new IKGoalPtr[1];
    aspkJGoal1[0] = aspkGoal[0];
    aspkJoint[1] = new IKJoint(m_spkJoint1,1,aspkJGoal1);
    aspkJoint[1]->AllowTranslation[2] = true;

    // create IK controller
    IKController* pkIKCtrl = new IKController(2,aspkJoint,1,aspkGoal);
    pkIKCtrl->Iterations = 1;
    m_spkJoint0->SetController(pkIKCtrl);

    // set up camera
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1000.0f);
    Vector3f kCLoc(0.0f,-2.0f,0.5f);
    Vector3f kCDir(0.0f,1.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();
    UpdateRod();

    InitializeCameraMotion(0.1f,0.01f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void TestInverseKinematics::OnTerminate ()
{
    m_spkScene = 0;
    m_spkIKSystem = 0;
    m_spkGoal = 0;
    m_spkJoint0 = 0;
    m_spkJoint1 = 0;
    m_spkWireframe = 0;
    m_spkRod = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void TestInverseKinematics::OnIdle ()
{
    MeasureTime();

    MoveCamera();

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
bool TestInverseKinematics::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if ( WindowApplication3::OnKeyDown(ucKey,iX,iY) )
        return true;

    if ( Transform(ucKey) )
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
bool TestInverseKinematics::OnMotion (int iButton, int iX, int iY)
{
    bool bMoved = WindowApplication3::OnMotion(iButton,iX,iY);
    if ( bMoved )
        UpdateRod();
    return bMoved;
}
//----------------------------------------------------------------------------
TriMesh* TestInverseKinematics::CreateCube ()
{
    int iVertexQuantity = 8;
    int iTriangleQuantity = 12;

    Vector3f* akVertex = new Vector3f[iVertexQuantity];
    float fSize = 0.1f;
    akVertex[0] = Vector3f(-fSize,-fSize,-fSize);
    akVertex[1] = Vector3f(+fSize,-fSize,-fSize);
    akVertex[2] = Vector3f(+fSize,+fSize,-fSize);
    akVertex[3] = Vector3f(-fSize,+fSize,-fSize);
    akVertex[4] = Vector3f(-fSize,-fSize,+fSize);
    akVertex[5] = Vector3f(+fSize,-fSize,+fSize);
    akVertex[6] = Vector3f(+fSize,+fSize,+fSize);
    akVertex[7] = Vector3f(-fSize,+fSize,+fSize);
    Vector3fArray* pkVertices = new Vector3fArray(iVertexQuantity,akVertex);

    ColorRGB* akColor = new ColorRGB[iVertexQuantity];
    akColor[0] = ColorRGB(0.0f,0.0f,1.0f);
    akColor[1] = ColorRGB(0.0f,1.0f,0.0f);
    akColor[2] = ColorRGB(1.0f,0.0f,0.0f);
    akColor[3] = ColorRGB(0.0f,0.0f,0.0f);
    akColor[4] = ColorRGB(0.0f,0.0f,1.0f);
    akColor[5] = ColorRGB(1.0f,0.0f,1.0f);
    akColor[6] = ColorRGB(1.0f,1.0f,0.0f);
    akColor[7] = ColorRGB(1.0f,1.0f,1.0f);
    ColorRGBArray* pkColors = new ColorRGBArray(iVertexQuantity,akColor);

    int* aiIndex = new int[3*iTriangleQuantity];
    aiIndex[ 0] = 0; aiIndex[ 1] = 2; aiIndex[ 2] = 1;
    aiIndex[ 3] = 0; aiIndex[ 4] = 3; aiIndex[ 5] = 2;
    aiIndex[ 6] = 4; aiIndex[ 7] = 5; aiIndex[ 8] = 6;
    aiIndex[ 9] = 4; aiIndex[10] = 6; aiIndex[11] = 7;
    aiIndex[12] = 1; aiIndex[13] = 6; aiIndex[14] = 5;
    aiIndex[15] = 1; aiIndex[16] = 2; aiIndex[17] = 6;
    aiIndex[18] = 0; aiIndex[19] = 7; aiIndex[20] = 3;
    aiIndex[21] = 0; aiIndex[22] = 4; aiIndex[23] = 7;
    aiIndex[24] = 0; aiIndex[25] = 1; aiIndex[26] = 5;
    aiIndex[27] = 0; aiIndex[28] = 5; aiIndex[29] = 4;
    aiIndex[30] = 3; aiIndex[31] = 6; aiIndex[32] = 2;
    aiIndex[33] = 3; aiIndex[34] = 7; aiIndex[35] = 6;
    IntArray* pkIndices = new IntArray(3*iTriangleQuantity,aiIndex);

    TriMesh* pkCube = new TriMesh(pkVertices,pkIndices,false);
    VertexColorEffect* pkEffect = new VertexColorEffect(pkColors);
    pkCube->SetEffect(pkEffect);

    return pkCube;
}
//----------------------------------------------------------------------------
Polyline* TestInverseKinematics::CreateRod ()
{
    Vector3f* akVertex = new Vector3f[2];
    akVertex[0] = Vector3f::ZERO;
    akVertex[1] = Vector3f::UNIT_X;
    Vector3fArray* pkVertices = new Vector3fArray(2,akVertex);

    ColorRGB* akColor = new ColorRGB[2];
    akColor[0] = ColorRGB::WHITE;
    akColor[1] = ColorRGB::WHITE;
    ColorRGBArray* pkColors = new ColorRGBArray(2,akColor);

    Polyline* pkLine = new Polyline(pkVertices,false,true);
    VertexColorEffect* pkEffect = new VertexColorEffect(pkColors);
    pkLine->SetEffect(pkEffect);

    return pkLine;
}
//----------------------------------------------------------------------------
TriMesh* TestInverseKinematics::CreateGround ()
{
    int iVertexQuantity = 4;
    int iTriangleQuantity = 2;

    Vector3f* akVertex = new Vector3f[iVertexQuantity];
    float fSize = 16.0f;
    akVertex[0] = Vector3f(-fSize,-fSize,-0.1f);
    akVertex[1] = Vector3f(+fSize,-fSize,-0.1f);
    akVertex[2] = Vector3f(+fSize,+fSize,-0.1f);
    akVertex[3] = Vector3f(-fSize,+fSize,-0.1f);
    Vector3fArray* pkVertices = new Vector3fArray(iVertexQuantity,akVertex);

    ColorRGB* akColor = new ColorRGB[iVertexQuantity];
    akColor[0] = ColorRGB(0.5f,0.5f,0.70f);
    akColor[1] = ColorRGB(0.5f,0.5f,0.80f);
    akColor[2] = ColorRGB(0.5f,0.5f,0.90f);
    akColor[3] = ColorRGB(0.5f,0.5f,1.00f);
    ColorRGBArray* pkColors = new ColorRGBArray(iVertexQuantity,akColor);

    int* aiIndex = new int[3*iTriangleQuantity];
    aiIndex[0] = 0; aiIndex[1] = 1; aiIndex[2] = 2;
    aiIndex[3] = 0; aiIndex[4] = 2; aiIndex[5] = 3;
    IntArray* pkIndices = new IntArray(3*iTriangleQuantity,aiIndex);

    TriMesh* pkPlane = new TriMesh(pkVertices,pkIndices,false);
    VertexColorEffect* pkEffect = new VertexColorEffect(pkColors);
    pkPlane->SetEffect(pkEffect);

    return pkPlane;
}
//----------------------------------------------------------------------------
void TestInverseKinematics::UpdateRod ()
{
    // The vertex[0] never moves.  The rod mesh is in the coordinate system
    // of joint0, so use the local translation of joint1 for the rod mesh's
    // moving end point.
    Vector3f* pkVertex = m_spkRod->Vertices->GetData();
    pkVertex[1] = m_spkJoint1->Local.GetTranslate();
    m_spkRod->UpdateMS();
    m_spkRod->UpdateGS();
}
//----------------------------------------------------------------------------
bool TestInverseKinematics::Transform (unsigned char ucKey)
{
    Matrix3f kRot, kIncr;
    Vector3f kTrn;

    switch ( ucKey )
    {
    case 'x':
        kTrn = m_spkGoal->Local.GetTranslate();
        kTrn.X() -= m_fTrnSpeed;
        m_spkGoal->Local.SetTranslate(kTrn);
        break;
    case 'X':
        kTrn = m_spkGoal->Local.GetTranslate();
        kTrn.X() += m_fTrnSpeed;
        m_spkGoal->Local.SetTranslate(kTrn);
        break;
    case 'y':
        kTrn = m_spkGoal->Local.GetTranslate();
        kTrn.Y() -= m_fTrnSpeed;
        m_spkGoal->Local.SetTranslate(kTrn);
        break;
    case 'Y':
        kTrn = m_spkGoal->Local.GetTranslate();
        kTrn.Y() += m_fTrnSpeed;
        m_spkGoal->Local.SetTranslate(kTrn);
        break;
    case 'z':
        kTrn = m_spkGoal->Local.GetTranslate();
        kTrn.Z() -= m_fTrnSpeed;
        m_spkGoal->Local.SetTranslate(kTrn);
        break;
    case 'Z':
        kTrn = m_spkGoal->Local.GetTranslate();
        kTrn.Z() += m_fTrnSpeed;
        m_spkGoal->Local.SetTranslate(kTrn);
        break;
    default:
        return false;
    }

    m_spkIKSystem->UpdateGS();
    UpdateRod();
    return true;
}
//----------------------------------------------------------------------------
void TestInverseKinematics::TestStream ()
{
    Stream kOStream;
    kOStream.Insert(m_spkScene);
    kOStream.Save("TestInverseKinematics.wmof");

    Stream kIStream;
    kIStream.Load("TestInverseKinematics.wmof");
    NodePtr spkScene = (Node*)kIStream.GetObjectAt(0);
}
//----------------------------------------------------------------------------
