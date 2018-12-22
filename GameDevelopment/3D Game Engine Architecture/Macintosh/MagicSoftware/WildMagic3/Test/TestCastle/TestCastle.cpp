// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "TestCastle.h"

WM3_WINDOW_APPLICATION(TestCastle);

//----------------------------------------------------------------------------
TestCastle::TestCastle ()
    :
    WindowApplication3("TestCastle",0,0,640,480,ColorRGB(0.9f,0.9f,0.9f))
{
    m_fVerticalDistance = 5.0f;
    m_acPicked[0] = 0;
}
//----------------------------------------------------------------------------
bool TestCastle::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    // Load the castle data set.  The default set is one that uses cached
    // arrays for the graphics card.  If the application crashes, change
    // the Load call to use "castle.nocache.wmof".
    Stream kIStream;
    kIStream.Load(System::GetPath("../../Data/Wmof/castle.cache.wmof"));
    m_spkModel = (Node*)kIStream.GetObjectAt(0);
    SetAllWireframe(m_spkModel);

    m_spkScene = new Node;
    Node* pkTrnNode = new Node;
    m_spkScene->AttachChild(pkTrnNode);
    pkTrnNode->AttachChild(m_spkModel);

    // center-and-fit (based on old castle scene)
    Vector3f kWCenter(1.3778250f,-0.70154405f,2205.9973f);
    float fWRadius = 5561.5498f;
    pkTrnNode->Local.SetTranslate(-kWCenter);

    // set up camera
    float fNear = 1.0f;
    float fFar = 8.0f*fWRadius;
    float fRight = 0.44f*fNear;
    float fLeft = -fRight;
    float fTop = 0.4125f*fNear;
    float fBottom = -fTop;
    m_spkCamera->SetFrustum(fLeft,fRight,fBottom,fTop,fNear,fFar);
    Vector3f kCLoc(527.394f,86.8992f,-2136.00f);
    Vector3f kCDir(1.0f,0.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    InitializeCameraMotion(1.0f,0.01f);
    InitializeObjectMotion(m_spkScene);
    MoveForward();
    return true;
}
//----------------------------------------------------------------------------
void TestCastle::OnTerminate ()
{
    m_spkScene = 0;
    m_spkModel = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void TestCastle::OnIdle ()
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
        m_pkRenderer->Draw(320,GetHeight()-8,ColorRGBA::WHITE,m_acPicked);
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool TestCastle::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if ( WindowApplication3::OnKeyDown(ucKey,iX,iY) )
        return true;

    switch ( ucKey )
    {
    case 'w':
    case 'W':
        ToggleAllWireframe(m_spkModel);
        return true;
    case '+':
    case '=':
        m_fVerticalDistance += 0.1f;
        AdjustVerticalDistance();
        return true;
    case '-':
    case '_':
        m_fVerticalDistance -= 0.1f;
        AdjustVerticalDistance();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
bool TestCastle::OnMouseClick (int iButton, int iState, int iX, int iY,
    unsigned int)
{
    if ( iState != MOUSE_DOWN )
        return false;

    // do a picking operation
    Ray3f kRay;
    m_spkCamera->GetPickRay(iX,iY,GetWidth(),GetHeight(),kRay);
    Spatial::PickArray kResults;
    m_spkScene->DoPick(kRay,kResults);

    if ( kResults.GetQuantity() > 0 )
    {
        Spatial::PickRecord* pkClosest = Spatial::GetClosest(kResults);
        Spatial* pkIObject = pkClosest->IObject;
        assert( pkIObject->GetName().GetLength() > 0 );

        // display the selected object's name
        strcpy(m_acPicked,pkIObject->GetName());

        if ( iButton == MOUSE_RIGHT_BUTTON )
        {
            // toggle the selected object's wireframe
            WireframeState* pkWireframe = StaticCast<WireframeState>(
                pkIObject->GetGlobalState(GlobalState::WIREFRAME));
            assert( pkWireframe );
            pkWireframe->Enabled = !pkWireframe->Enabled;
        }

        for (int i = 0; i < kResults.GetQuantity(); i++)
            delete kResults[i];
    }

    return true;
}
//----------------------------------------------------------------------------
void TestCastle::AdjustVerticalDistance ()
{
    // retain vertical distance above "ground"
    Spatial::PickArray kResults;
    Ray3f kRay(m_spkCamera->GetLocation(),Vector3f(0.0f,0.0f,-1.0f));
    m_spkScene->DoPick(kRay,kResults);

    if ( kResults.GetQuantity() > 0 )
    {
        Spatial::PickRecord* pkRecord = Spatial::GetClosest(kResults);
        assert( pkRecord );
        TriMesh* pkMesh = DynamicCast<TriMesh>(pkRecord->IObject);
        assert( pkMesh );
        TriMesh::PickRecord* pkTMRecord = (TriMesh::PickRecord*)pkRecord;

        Triangle3f kTri;
        pkMesh->GetWorldTriangle(pkTMRecord->Triangle,kTri);
        Vector3f kClosest =
            pkTMRecord->Bary0*kTri.V[0] +
            pkTMRecord->Bary1*kTri.V[1] +
            pkTMRecord->Bary2*kTri.V[2];

        kClosest.Z() += m_fVerticalDistance;
        m_spkCamera->SetLocation(kClosest);

        for (int i = 0; i < kResults.GetQuantity(); i++)
            delete kResults[i];
    }
}
//----------------------------------------------------------------------------
void TestCastle::MoveForward ()
{
    WindowApplication3::MoveForward();
    AdjustVerticalDistance();
}
//----------------------------------------------------------------------------
void TestCastle::MoveBackward ()
{
    WindowApplication3::MoveBackward();
    AdjustVerticalDistance();
}
//----------------------------------------------------------------------------
void TestCastle::SetAllWireframe (Spatial* pkSpatial)
{
    TriMesh* pkMesh = DynamicCast<TriMesh>(pkSpatial);
    if ( pkMesh )
    {
        pkMesh->SetGlobalState(new WireframeState);
        return;
    }

    Node* pkNode = DynamicCast<Node>(pkSpatial);
    assert( pkNode );
    for (int i = 0; i < pkNode->GetQuantity(); i++)
        SetAllWireframe(pkNode->GetChild(i));
}
//----------------------------------------------------------------------------
void TestCastle::ToggleAllWireframe (Spatial* pkSpatial)
{
    TriMesh* pkMesh = DynamicCast<TriMesh>(pkSpatial);
    if ( pkMesh )
    {
        WireframeState* pkWireframe = StaticCast<WireframeState>(
            pkMesh->GetGlobalState(GlobalState::WIREFRAME));
        assert( pkWireframe );
        pkWireframe->Enabled = !pkWireframe->Enabled;
        return;
    }

    Node* pkNode = DynamicCast<Node>(pkSpatial);
    assert( pkNode );
    for (int i = 0; i < pkNode->GetQuantity(); i++)
        ToggleAllWireframe(pkNode->GetChild(i));
}
//----------------------------------------------------------------------------
