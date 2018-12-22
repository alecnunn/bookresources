// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "TestSkinnedBiped.h"

WM3_WINDOW_APPLICATION(TestSkinnedBiped);

//----------------------------------------------------------------------------
TestSkinnedBiped::TestSkinnedBiped ()
    :
    WindowApplication3("TestSkinnedBiped",0,0,640,480,
        ColorRGB(0.5f,0.0f,1.0f))
{
    m_dAnimTime = 0.0;
    m_dAnimTimeDelta = 0.01;
}
//----------------------------------------------------------------------------
bool TestSkinnedBiped::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    // set up scene
    LoadModel();

    m_spkWireframe = new WireframeState;
    m_spkScene->SetGlobalState(m_spkWireframe);

    ZBufferState* pkZS = new ZBufferState;
    m_spkScene->SetGlobalState(pkZS);

    // camera and view frustum
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1000.0f);
    Vector3f kCLoc(80.0f,0.0f,23.0f);
    Vector3f kCDir(-1.0f,0.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    Light* pkLight = new Light(Light::LT_DIRECTIONAL);
    pkLight->Ambient = ColorRGBA(0.5f,0.5f,0.5f,1.0f);
    pkLight->Diffuse = pkLight->Ambient;
    pkLight->Specular = ColorRGBA::BLACK;
    pkLight->SetDirection(kCDir);
    pkLight->UpdateGS();
    m_spkScene->SetLight(pkLight);

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    InitializeCameraMotion(0.01f,0.01f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void TestSkinnedBiped::OnTerminate ()
{
    m_spkScene = 0;
    m_spkWireframe = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void TestSkinnedBiped::OnIdle ()
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
bool TestSkinnedBiped::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if ( WindowApplication3::OnKeyDown(ucKey,iX,iY) )
        return true;

    switch ( ucKey )
    {
    case 'w':
    case 'W':
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;

    case 'g':
        m_dAnimTime += m_dAnimTimeDelta;
        m_spkScene->UpdateGS(m_dAnimTime);
        return true;
    case 'G':
        m_dAnimTime = 0.0;
        m_spkScene->UpdateGS(m_dAnimTime);
        return true;

    case 's':
    case 'S':
        TestStream();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void TestSkinnedBiped::LoadModel ()
{
    Node* pkBiped = GetNode("Biped");
    Node* pkPelvis = GetNode("Pelvis");
    Node* pkSpine = GetNode("Spine");
    Node* pkSpine1 = GetNode("Spine1");
    Node* pkSpine2 = GetNode("Spine2");
    Node* pkSpine3 = GetNode("Spine3");
    Node* pkNeck = GetNode("Neck");
    Node* pkHead = GetNode("Head");
    Node* pkLClavicle = GetNode("L_Clavicle");
    Node* pkLUpperArm = GetNode("L_UpperArm");
    Node* pkLForeArm = GetNode("L_Forearm");
    Node* pkLHand = GetNode("L_Hand");
    Node* pkRClavicle = GetNode("R_Clavicle");
    Node* pkRUpperArm = GetNode("R_UpperArm");
    Node* pkRForeArm = GetNode("R_Forearm");
    Node* pkRHand = GetNode("R_Hand");
    Node* pkLThigh = GetNode("L_Thigh");
    Node* pkLCalf = GetNode("L_Calf");
    Node* pkLFoot = GetNode("L_Foot");
    Node* pkLToe = GetNode("L_Toe");
    Node* pkRThigh = GetNode("R_Thigh");
    Node* pkRCalf = GetNode("R_Calf");
    Node* pkRFoot = GetNode("R_Foot");
    Node* pkRToe = GetNode("R_Toe");

    pkBiped->AttachChild(pkPelvis);
        pkPelvis->AttachChild(pkSpine);
            pkSpine->AttachChild(pkSpine1);
                pkSpine1->AttachChild(pkSpine2);
                    pkSpine2->AttachChild(pkSpine3);
                        pkSpine3->AttachChild(pkNeck);
                            pkNeck->AttachChild(pkHead);
                                // pkHead->AttachChild(pkHair);
                            pkNeck->AttachChild(pkLClavicle);
                                pkLClavicle->AttachChild(pkLUpperArm);
                                    pkLUpperArm->AttachChild(pkLForeArm);
                                        pkLForeArm->AttachChild(pkLHand);
                                    // pkLUpperArm->AttachChild(pkLArm);
                            pkNeck->AttachChild(pkRClavicle);
                                pkRClavicle->AttachChild(pkRUpperArm);
                                    pkRUpperArm->AttachChild(pkRForeArm);
                                        pkRForeArm->AttachChild(pkRHand);
                                    // pkRUpperArm->AttachChild(pkRArm);
                        // pkSpine3->AttachChild(pkFace);
        pkPelvis->AttachChild(pkLThigh);
            pkLThigh->AttachChild(pkLCalf);
                pkLCalf->AttachChild(pkLFoot);
                    pkLFoot->AttachChild(pkLToe);
                // pkLCalf->AttachChild(pkLShoe);
            // pkLThigh->AttachChild(pkLLeg);
            // pkLThigh->AttachChild(pkLAnkle);
        pkPelvis->AttachChild(pkRThigh);
            pkRThigh->AttachChild(pkRCalf);
                pkRCalf->AttachChild(pkRFoot);
                    pkRFoot->AttachChild(pkRToe);
                // pkRCalf->AttachChild(pkRShoe);
            // pkRThigh->AttachChild(pkRLeg);
            // pkRThigh->AttachChild(pkRAnkle);
        // pkPelvis->AttachChild(pkShirt);
        // pkPelvis->AttachChild(pkPants);

    TriMesh* pkHair = GetMesh("Hair",pkBiped);
    TriMesh* pkLArm = GetMesh("L_Arm",pkBiped);
    TriMesh* pkRArm = GetMesh("R_Arm",pkBiped);
    TriMesh* pkFace = GetMesh("Face",pkBiped);
    TriMesh* pkLShoe = GetMesh("L_Shoe",pkBiped);
    TriMesh* pkLLeg = GetMesh("L_Leg",pkBiped);
    TriMesh* pkLAnkle = GetMesh("L_Ankle",pkBiped);
    TriMesh* pkRShoe = GetMesh("R_Shoe",pkBiped);
    TriMesh* pkRLeg = GetMesh("R_Leg",pkBiped);
    TriMesh* pkRAnkle = GetMesh("R_Ankle",pkBiped);
    TriMesh* pkShirt = GetMesh("Shirt",pkBiped);
    TriMesh* pkPants = GetMesh("Pants",pkBiped);

    pkHead->AttachChild(pkHair);
    pkLUpperArm->AttachChild(pkLArm);
    pkRUpperArm->AttachChild(pkRArm);
    pkSpine3->AttachChild(pkFace);
    pkLCalf->AttachChild(pkLShoe);
    pkLThigh->AttachChild(pkLLeg);
    pkLThigh->AttachChild(pkLAnkle);
    pkRCalf->AttachChild(pkRShoe);
    pkRThigh->AttachChild(pkRLeg);
    pkRThigh->AttachChild(pkRAnkle);
    pkPelvis->AttachChild(pkShirt);
    pkPelvis->AttachChild(pkPants);

    m_spkScene = new Node;
    m_spkScene->Local.SetRotate(Matrix3f().FromAxisAngle(Vector3f::UNIT_Z,
        0.25f*Mathf::PI));
    m_spkScene->AttachChild(pkBiped);
    m_spkScene->UpdateGS();

    //For regenerating the biped WMOF whenever engine streaming changes.
    //Stream kStream;
    //kStream.Insert(m_spkScene);
    //kStream.Save("SkinnedBiped.wmof");
}
//----------------------------------------------------------------------------
Node* TestSkinnedBiped::GetNode (const char* acName)
{
    Node* pkNode = new Node;
    pkNode->SetName(acName);

    // load the keyframe controller
    char acFilename[256];
    sprintf(acFilename,System::GetPath("%s.keyf.raw"),acName);
    FILE* pkIFile = fopen(acFilename,"rb");
    assert( pkIFile );

    KeyframeController* pkCtrl = new KeyframeController;

    int iRepeatType;
    float fMinTime, fMaxTime, fPhase, fFrequency;
    System::Read4(pkIFile,1,&iRepeatType);
    System::Read4(pkIFile,1,&fMinTime);
    System::Read4(pkIFile,1,&fMaxTime);
    System::Read4(pkIFile,1,&fPhase);
    System::Read4(pkIFile,1,&fFrequency);

    pkCtrl->RepeatType = iRepeatType;
    pkCtrl->MinTime = fMinTime;
    pkCtrl->MaxTime = fMaxTime;
    pkCtrl->Phase = fPhase;
    pkCtrl->Frequency = fFrequency;

    int iTQ, iRQ, iSQ;
    System::Read4(pkIFile,1,&iTQ);
    System::Read4(pkIFile,1,&iRQ);
    System::Read4(pkIFile,1,&iSQ);

    if ( iTQ > 0 )
    {
        float* afTTime = new float[iTQ];
        Vector3f* akTData = new Vector3f[iTQ];
        System::Read4(pkIFile,iTQ,afTTime);
        System::Read4(pkIFile,3*iTQ,(float*)akTData);
        pkCtrl->TranslationTimes = new FloatArray(iTQ,afTTime);
        pkCtrl->TranslationData = new Vector3fArray(iTQ,akTData);
    }
    else
    {
        Vector3f kTrn;
        System::Read4(pkIFile,3,(float*)kTrn);
        pkNode->Local.SetTranslate(kTrn);
    }

    if ( iRQ > 0 )
    {
        float* afRTime = new float[iRQ];
        Quaternionf* akRData = new Quaternionf[iRQ];

        System::Read4(pkIFile,iRQ,afRTime);
        System::Read4(pkIFile,4*iRQ,(float*)akRData);
        pkCtrl->RotationTimes = new FloatArray(iRQ,afRTime);
        pkCtrl->RotationData = new QuaternionfArray(iRQ,akRData);
    }
    else
    {
        Matrix3f kRot;
        System::Read4(pkIFile,9,(float*)kRot);
        pkNode->Local.SetRotate(kRot);
    }

    if ( iSQ > 0 )
    {
        float* afSTime = new float[iSQ];
        float* afSData = new float[iSQ];

        System::Read4(pkIFile,iSQ,afSTime);
        System::Read4(pkIFile,iSQ,afSData);
        pkCtrl->ScaleTimes = new FloatArray(iSQ,afSTime);
        pkCtrl->ScaleData = new FloatArray(iSQ,afSData);
    }
    else
    {
        float fScale;
        System::Read4(pkIFile,1,&fScale);
        pkNode->Local.SetUniformScale(fScale);
    }

    fclose(pkIFile);

    pkNode->SetController(pkCtrl);
    return pkNode;
}
//----------------------------------------------------------------------------
TriMesh* TestSkinnedBiped::GetMesh (const char* acName, Node* pkBiped)
{
    // load the triangle indices and material
    char acFilename[256];
    sprintf(acFilename,System::GetPath("%s.triangle.raw"),acName);
    FILE* pkIFile = fopen(acFilename,"rb");
    assert( pkIFile );

    int iTQ;
    System::Read4(pkIFile,1,&iTQ);

    int* aiIndex = new int[3*iTQ];
    System::Read4(pkIFile,3*iTQ,aiIndex);

    ColorRGB kEmissive, kAmbient, kDiffuse, kSpecular;
    System::Read4(pkIFile,3,(float*)kEmissive);
    System::Read4(pkIFile,3,(float*)kAmbient);
    System::Read4(pkIFile,3,(float*)kDiffuse);
    System::Read4(pkIFile,3,(float*)kSpecular);

    fclose(pkIFile);

    MaterialState* pkMS = new MaterialState;
    pkMS->Emissive.R() = kEmissive.R();
    pkMS->Emissive.G() = kEmissive.G();
    pkMS->Emissive.B() = kEmissive.B();
    pkMS->Emissive.A() = 1.0f;
    pkMS->Ambient.R() = kAmbient.R();
    pkMS->Ambient.G() = kAmbient.G();
    pkMS->Ambient.B() = kAmbient.B();
    pkMS->Ambient.A() = 1.0f;
    pkMS->Diffuse.R() = kDiffuse.R();
    pkMS->Diffuse.G() = kDiffuse.G();
    pkMS->Diffuse.B() = kDiffuse.B();
    pkMS->Diffuse.A() = 1.0f;
    pkMS->Specular.R() = kSpecular.R();
    pkMS->Specular.G() = kSpecular.G();
    pkMS->Specular.B() = kSpecular.B();
    pkMS->Specular.A() = 1.0f;

    // load the skin controller
    sprintf(acFilename,System::GetPath("%s.skin.raw"),acName);
    pkIFile = fopen(acFilename,"rb");
    assert( pkIFile );

    int iRepeatType;
    float fMinTime, fMaxTime, fPhase, fFrequency;
    System::Read4(pkIFile,1,&iRepeatType);
    System::Read4(pkIFile,1,&fMinTime);
    System::Read4(pkIFile,1,&fMaxTime);
    System::Read4(pkIFile,1,&fPhase);
    System::Read4(pkIFile,1,&fFrequency);

    int iVertexQuantity, iBoneQuantity;
    System::Read4(pkIFile,1,&iVertexQuantity);
    System::Read4(pkIFile,1,&iBoneQuantity);

    Node** apkBones = new Node*[iBoneQuantity];
    int i;
    for (i = 0; i < iBoneQuantity; i++)
    {
        int iLength;
        System::Read4(pkIFile,1,&iLength);

        char* acBoneName = new char[iLength+1];
        System::Read1(pkIFile,iLength,acBoneName);
        acBoneName[iLength] = 0;

        apkBones[i] = (Node*)pkBiped->GetObjectByName(acBoneName);
        assert( apkBones[i] );
        delete[] acBoneName;
    }

    float** aafWeight;
    System::Allocate(iBoneQuantity,iVertexQuantity,aafWeight);
    Vector3f** aakOffset;
    System::Allocate(iBoneQuantity,iVertexQuantity,aakOffset);

    for (int j = 0; j < iVertexQuantity; j++)
    {
        for (i = 0; i < iBoneQuantity; i++)
        {
            System::Read4(pkIFile,1,&aafWeight[j][i]);
            System::Read4(pkIFile,3,&aakOffset[j][i]);
        }
    }

    fclose(pkIFile);

    Vector3f* akVertex = new Vector3f[iVertexQuantity];
    TriMesh* pkMesh = new TriMesh(
        new Vector3fArray(iVertexQuantity,akVertex),
        new IntArray(3*iTQ,aiIndex),false);

    SkinController* pkCtrl = new SkinController(iVertexQuantity,
        iBoneQuantity,apkBones,aafWeight,aakOffset);
    pkCtrl->RepeatType = iRepeatType;
    pkCtrl->MinTime = fMinTime;
    pkCtrl->MaxTime = fMaxTime;
    pkCtrl->Phase = fPhase;
    pkCtrl->Frequency = fFrequency;

    pkMesh->SetName(acName);
    pkMesh->SetController(pkCtrl);
    pkMesh->SetGlobalState(pkMS);
    pkMesh->GenerateNormals();
    return pkMesh;
}
//----------------------------------------------------------------------------
void TestSkinnedBiped::TestStream ()
{
    Stream kOStream;
    kOStream.Insert(m_spkScene);
    kOStream.Save("TestSkinnedBiped.wmof");

    Stream kIStream;
    kIStream.Load("TestSkinnedBiped.wmof");
    NodePtr spkScene = (Node*)kIStream.GetObjectAt(0);
}
//----------------------------------------------------------------------------
