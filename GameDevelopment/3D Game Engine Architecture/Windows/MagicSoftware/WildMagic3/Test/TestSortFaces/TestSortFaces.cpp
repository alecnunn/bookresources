// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "TestSortFaces.h"
#include "SortedCube.h"

WM3_WINDOW_APPLICATION(TestSortFaces);

//----------------------------------------------------------------------------
TestSortFaces::TestSortFaces ()
    :
    WindowApplication3("TestSortFaces",0,0,640,480,ColorRGB(0.9f,0.9f,0.9f))
{
}
//----------------------------------------------------------------------------
bool TestSortFaces::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    // set up camera
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,100.0f);
    Vector3f kCLoc(0.0f,0.0f,8.0f);
    Vector3f kCDir(0.0f,0.0f,-1.0f);
    Vector3f kCUp(0.0f,1.0f,0.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    m_spkScene = new SortedCube(m_spkCamera,
        Image::Load(System::GetPath("../../Data/Wmif/XpImage.wmif")),
        Image::Load(System::GetPath("../../Data/Wmif/XmImage.wmif")),
        Image::Load(System::GetPath("../../Data/Wmif/YpImage.wmif")),
        Image::Load(System::GetPath("../../Data/Wmif/YmImage.wmif")),
        Image::Load(System::GetPath("../../Data/Wmif/ZpImage.wmif")),
        Image::Load(System::GetPath("../../Data/Wmif/ZmImage.wmif")));

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    InitializeCameraMotion(0.01f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void TestSortFaces::OnTerminate ()
{
    m_spkScene = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void TestSortFaces::OnIdle ()
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
bool TestSortFaces::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
void TestSortFaces::TestStream ()
{
    Stream kOStream;
    kOStream.Insert(m_spkScene);
    kOStream.Save("TestSortFaces.wmof");

    Stream kIStream;
    kIStream.Load("TestSortFaces.wmof");
    NodePtr spkScene = (Node*)kIStream.GetObjectAt(0);
}
//----------------------------------------------------------------------------
