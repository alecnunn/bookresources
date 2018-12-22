// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "TestMorphController.h"
#include <fstream>
using namespace std;

WM3_WINDOW_APPLICATION(TestMorphController);

//----------------------------------------------------------------------------
TestMorphController::TestMorphController ()
    :
    WindowApplication3("TestMorphController",0,0,640,480,
        ColorRGB(0.5f,0.0f,1.0f))
{
    m_dBaseTime = -1.0;
    m_dCurrTime = -1.0;
    m_iXPick = -1;
    m_iYPick = -1;
    sprintf(m_acPickMessage,"no selection");
    m_bPickPending = false;
}
//----------------------------------------------------------------------------
bool TestMorphController::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    // set up camera
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1000.0f);
    Vector3f kCLoc(0.0f,0.0f,0.0f);
    Vector3f kCDir(1.0f,0.0f,0.0f);
    Vector3f kCUp(0.0f,1.0f,0.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // Set up scene.  The transformations are selected using prior knowledge
    // of the input data sets.
    m_spkScene = new Node;
    m_spkScene->Local.SetRotate(Matrix3f(Vector3f::UNIT_Z,Vector3f::UNIT_X,
        Vector3f::UNIT_Y,true));
    m_spkScene->Local.SetTranslate(Vector3f(250.0f,0.0f,0.0f));

    m_spkWireframe = new WireframeState;
    m_spkScene->SetGlobalState(m_spkWireframe);

    ZBufferState* pkZS = new ZBufferState;
    m_spkScene->SetGlobalState(pkZS);

    LoadTargets();
    CreateFace();
    CreateController();

    // Attach the face to the scene.  Also attach the targets so we can
    // display them above the animated face.
    m_spkFace->ForceCull = true;
    m_spkScene->AttachChild(m_spkFace);
    for (int i = 0; i < TARGET_QUANTITY; i++)
    {
        m_aspkTarget[i]->ForceCull = true;
        m_spkScene->AttachChild(m_aspkTarget[i]);
    }

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    InitializeCameraMotion(0.1f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void TestMorphController::OnTerminate ()
{
    m_spkScene = 0;
    m_spkWireframe = 0;
    m_spkFace = 0;
    m_spkIndices = 0;
    m_spkMaterial = 0;
    m_spkDLight = 0;
    for (int i = 0; i < TARGET_QUANTITY; i++)
        m_aspkTarget[i] = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void TestMorphController::OnIdle ()
{
    MeasureTime();

    MoveCamera();
    MoveObject();

    // update the morph animation
    if ( m_dBaseTime == -1.0 )
    {
        m_dBaseTime = System::GetTime();
        m_dCurrTime = m_dBaseTime;
    }
    else
    {
        m_dCurrTime = System::GetTime();
    }
    m_spkScene->UpdateGS(m_dCurrTime - m_dBaseTime);

    m_pkRenderer->ClearBuffers();
    if ( m_pkRenderer->BeginScene() )
    {
        Ray3f kRay;
        Spatial::PickArray kResults;
        bool bPickingAttempted = false;
        int j;

        // draw the 5 targets in the upper half of the screen
        for (int i = 0; i < TARGET_QUANTITY; i++)
        {
            float fLPort = 0.2f*i, fRPort = 0.2f*(i+1);
            m_spkCamera->SetViewPort(fLPort,fRPort,1.0f,0.8f);
            m_aspkTarget[i]->ForceCull = false;
            m_pkRenderer->DrawScene(m_spkScene);
            m_aspkTarget[i]->ForceCull = true;

            if ( m_bPickPending )
            {
                if ( m_spkCamera->GetPickRay(m_iXPick,m_iYPick,GetWidth(),
                    GetHeight(),kRay) )
                {
                    m_bPickPending = false;
                    bPickingAttempted = true;
                    m_spkScene->DoPick(kRay,kResults);
                    if ( kResults.GetQuantity() )
                    {
                        sprintf(m_acPickMessage,"target %d",i);
                        for (j = 0; j < kResults.GetQuantity(); j++)
                            delete kResults[j];
                    }
                }
            }
        }

        // draw the animated face in the lower half of the screen
        m_spkCamera->SetViewPort(0.1f,0.9f,0.8f,0.0f);
        m_spkFace->ForceCull = false;
        m_pkRenderer->DrawScene(m_spkScene);
        m_spkFace->ForceCull = true;

        if ( m_bPickPending )
        {
            if ( m_spkCamera->GetPickRay(m_iXPick,m_iYPick,GetWidth(),
                GetHeight(),kRay) )
            {
                m_bPickPending = false;
                bPickingAttempted = true;
                m_spkScene->DoPick(kRay,kResults);
                if ( kResults.GetQuantity() )
                {
                    sprintf(m_acPickMessage,"morphed");
                    for (j = 0; j < kResults.GetQuantity(); j++)
                        delete kResults[j];
                }
            }
        }

        if ( bPickingAttempted && kResults.GetQuantity() == 0 )
            sprintf(m_acPickMessage,"no selection");

        m_spkCamera->SetViewPort(0.0f,1.0f,1.0f,0.0f);
        DrawFrameRate(8,GetHeight()-8,ColorRGBA::WHITE);
        m_pkRenderer->Draw(320,GetHeight()-8,ColorRGBA::WHITE,
            m_acPickMessage);
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool TestMorphController::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if ( WindowApplication3::OnKeyDown(ucKey,iX,iY) )
        return true;

    static int iFace = -1;

    switch ( ucKey )
    {
    case 'w':
    case 'W':
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;

    case ' ':
        iFace++;
        if ( iFace == 0 )
        {
            m_spkFace->ForceCull = true;
            m_aspkTarget[0]->ForceCull = false;
        }
        else if ( iFace == TARGET_QUANTITY )
        {
            m_aspkTarget[iFace-1]->ForceCull = true;
            m_spkFace->ForceCull = false;
            iFace = -1;
        }
        else
        {
            m_aspkTarget[iFace-1]->ForceCull = true;
            m_aspkTarget[iFace]->ForceCull = false;
        }
        return true;

    case 's':
    case 'S':
        TestStream();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
bool TestMorphController::OnMouseClick (int iButton, int iState, int iX,
    int iY, unsigned int)
{
    if ( iState != MOUSE_DOWN || iButton != MOUSE_LEFT_BUTTON )
        return false;

    m_iXPick = iX;
    m_iYPick = iY;
    m_bPickPending = true;
    return true;
}
//----------------------------------------------------------------------------
void TestMorphController::LoadTargets ()
{
    // Create triangle meshes for the targets just for the purposes of this
    // application.  The targets will be displayed directly above the animated
    // face.
    
    // create a material
    m_spkMaterial = new MaterialState;
    m_spkMaterial->Emissive = ColorRGBA(0.0f,0.0f,0.0f,1.0f);
    m_spkMaterial->Ambient = ColorRGBA(0.5f,0.5f,0.5f,1.0f);
    m_spkMaterial->Diffuse = ColorRGBA(0.99607f,0.83920f,0.67059f,1.0f);
    m_spkMaterial->Specular = ColorRGBA(0.8f,0.8f,0.8f,1.0f);
    m_spkMaterial->Shininess = 0.0f;

    // create a directional light
    m_spkDLight = new Light(Light::LT_DIRECTIONAL);
    m_spkDLight->Ambient = ColorRGBA(0.25f,0.25f,0.25f,1.0f);
    m_spkDLight->Diffuse = ColorRGBA::WHITE;
    m_spkDLight->Specular = ColorRGBA::BLACK;
    m_spkDLight->On = true;
    m_spkDLight->SetDirection(Vector3f::UNIT_X);
    m_spkDLight->UpdateGS();

    // load triangle indices
    FILE* pkIFile = fopen(System::GetPath("FaceIndices.raw"),"rb");
    assert( pkIFile );

    int i, iTQuantity;
    System::Read4(pkIFile,1,&iTQuantity);

    int* aiIndex = new int[3*iTQuantity];
    int* piIndex = aiIndex;
    for (i = 0; i < iTQuantity; i++)
    {
        System::Read4(pkIFile,3,piIndex);
        piIndex += 3;
    }

    fclose(pkIFile);

    m_spkIndices = new IntArray(3*iTQuantity,aiIndex);

    // Load the targets.  Create triangle meshes for each of the targets just
    // for the purposes of this application.  Normally target0 and the
    // differences between the other targets and target0 are stored only in
    // the controller.
    for (int j = 0; j < TARGET_QUANTITY; j++)
    {
        char acFilename[64];
        sprintf(acFilename,System::GetPath("FaceTarget%d.raw"),j);
        pkIFile = fopen(acFilename,"rb");
        assert( pkIFile );

        int iVQuantity;
        System::Read4(pkIFile,1,&iVQuantity);

        Vector3f* akVertex = new Vector3f[iVQuantity];
        for (int i = 0; i < iVQuantity; i++)
            System::Read4(pkIFile,3,(float*)akVertex[i]);

        fclose(pkIFile);

        Vector3fArray* pkVertices = new Vector3fArray(iVQuantity,akVertex);
        m_aspkTarget[j] = new TriMesh(pkVertices,m_spkIndices,true);
        m_aspkTarget[j]->SetGlobalState(m_spkMaterial);
        m_aspkTarget[j]->SetLight(m_spkDLight);
        m_aspkTarget[j]->SetName(acFilename);
    }
}
//----------------------------------------------------------------------------
void TestMorphController::CreateFace ()
{
    // This is the triangle mesh that will be controlled.  Make a copy of the
    // vertices, own your normals, but share the connectivity, material
    // effect, and directional light.

    m_spkFace = new TriMesh(new Vector3fArray(*m_aspkTarget[0]->Vertices),
        m_aspkTarget[0]->Indices,true);
    m_spkFace->SetGlobalState(m_spkMaterial);

#if 1
    m_spkFace->SetLight(m_spkDLight);
    m_spkFace->SetName("original");
#else
    //For regenerating the face WMOF whenever the engine streaming changes.
    Stream kStream;
    kStream.Insert(m_spkFace);
    kStream.Save("Face.wmof");
#endif
}
//----------------------------------------------------------------------------
void TestMorphController::CreateController ()
{
    // Create the controller.  The parameters set here are known information
    // about the model.
    int iVQuantity = m_spkFace->Vertices->GetQuantity();
    MorphController* pkCtrl = new MorphController(iVQuantity,TARGET_QUANTITY,
        KEY_QUANTITY);
    pkCtrl->MinTime = 0.0;
    pkCtrl->MaxTime = 16000.0;
    pkCtrl->Phase = 0.0;
    pkCtrl->Frequency = 4800.0;
    pkCtrl->RepeatType = Controller::RT_WRAP;

    // create the targets (target 0 is base, others are difference from base)
    Vector3fArray* pkBaseVertices =
        new Vector3fArray(*m_aspkTarget[0]->Vertices);
    pkCtrl->SetVertices(0,pkBaseVertices);
    Vector3f* akBaseVertex = pkBaseVertices->GetData();
    int j;
    for (j = 1; j < TARGET_QUANTITY; j++)
    {
        Vector3fArray* pkDeltaVertices =
            new Vector3fArray(*m_aspkTarget[j]->Vertices);
        Vector3f* akDelta = pkDeltaVertices->GetData();
        for (int i = 0; i < iVQuantity; i++)
            akDelta[i] -= akBaseVertex[i];
        pkCtrl->SetVertices(j,pkDeltaVertices);
    }

    // load the keys
    float* afTimes = new float[KEY_QUANTITY];
    FloatArrayPtr spkTimes = new FloatArray(KEY_QUANTITY,afTimes);
    pkCtrl->SetTimes(spkTimes);
    for (j = 0; j < KEY_QUANTITY; j++)
    {
        char acFilename[64];
        sprintf(acFilename,System::GetPath("FaceKey%d.raw"),j);
        FILE* pkIFile = fopen(acFilename,"rb");
        assert( pkIFile );

        int iTargetQuantity;
        System::Read4(pkIFile,1,&iTargetQuantity);

        float* afWeight = new float[iTargetQuantity-1];
        FloatArrayPtr spkWeight = new FloatArray(iTargetQuantity-1,afWeight);
        pkCtrl->SetWeights(j,spkWeight);
        System::Read4(pkIFile,1,&afTimes[j]);

        float fW0;
        System::Read4(pkIFile,1,&fW0);  // discard, not needed
        System::Read4(pkIFile,iTargetQuantity-1,afWeight);

        fclose(pkIFile);
    }

    m_spkFace->SetController(pkCtrl);
}
//----------------------------------------------------------------------------
void TestMorphController::TestStream ()
{
    Stream kOStream;
    kOStream.Insert(m_spkScene);
    kOStream.Save("TestMorphController.wmof");

    Stream kIStream;
    kIStream.Load("TestMorphController.wmof");
    NodePtr spkScene = (Node*)kIStream.GetObjectAt(0);
}
//----------------------------------------------------------------------------
