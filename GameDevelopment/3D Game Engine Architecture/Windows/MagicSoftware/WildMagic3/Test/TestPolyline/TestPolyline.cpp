// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "TestPolyline.h"

WM3_WINDOW_APPLICATION(TestPolyline);

//----------------------------------------------------------------------------
TestPolyline::TestPolyline ()
    :
    WindowApplication3("TestPolyline",0,0,640,480,ColorRGB(0.75f,0.75f,0.75f))
{
}
//----------------------------------------------------------------------------
bool TestPolyline::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    // set up camera
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1000.0f);
    Vector3f kCLoc(4.0f,0.0f,0.0f);
    Vector3f kCDir(-1.0f,0.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // set up scene
    m_spkScene = new Node(1);

    int iVQuantity = 128;
    Vector3f* akVertex = new Vector3f[iVQuantity];
    ColorRGB* akColor = new ColorRGB[iVQuantity];
    for (int i = 0; i < iVQuantity; i++)
    {
        akVertex[i].X() = Mathf::SymmetricRandom();
        akVertex[i].Y() = Mathf::SymmetricRandom();
        akVertex[i].Z() = Mathf::SymmetricRandom();
        akColor[i].R() = Mathf::UnitRandom();
        akColor[i].G() = Mathf::UnitRandom();
        akColor[i].B() = Mathf::UnitRandom();
    }

    Vector3fArray* pkVertices = new Vector3fArray(iVQuantity,akVertex);
    ColorRGBArray* pkColors = new ColorRGBArray(iVQuantity,akColor);

    bool bClosed = false;
    bool bContiguous = true;
    m_spkPolyline = new Polyline(pkVertices,bClosed,bContiguous);
    Effect* pkEffect = new VertexColorEffect(pkColors);
    m_spkPolyline->SetEffect(pkEffect);

    m_spkScene->AttachChild(m_spkPolyline);

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    InitializeCameraMotion(0.01f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void TestPolyline::OnTerminate ()
{
    m_spkScene = 0;
    m_spkPolyline = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void TestPolyline::OnIdle ()
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
bool TestPolyline::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if ( WindowApplication3::OnKeyDown(ucKey,iX,iY) )
        return true;

    switch ( ucKey )
    {
    case 'c':
    case 'C':
        m_spkPolyline->SetClosed(!m_spkPolyline->GetClosed());
        return true;
    case 'k':
    case 'K':
        m_spkPolyline->SetContiguous(!m_spkPolyline->GetContiguous());
        break;
    case '+':
    case '=':
    {
        int iVQuantity = m_spkPolyline->Vertices->GetQuantity();
        int iAQuantity = m_spkPolyline->GetActiveQuantity();
        if ( iAQuantity < iVQuantity-2 )
            m_spkPolyline->SetActiveQuantity(iAQuantity+2);
        return true;
    }
    case '-':
    case '_':
    {
        int iAQuantity = m_spkPolyline->GetActiveQuantity();
        if ( iAQuantity >= 4 )
            m_spkPolyline->SetActiveQuantity(iAQuantity-2);
        return true;
    }

    case 's':
    case 'S':
        TestStream();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void TestPolyline::TestStream ()
{
    Stream kOStream;
    kOStream.Insert(m_spkScene);
    kOStream.Save("TestPolyline.wmof");

    Stream kIStream;
    kIStream.Load("TestPolyline.wmof");
    NodePtr spkScene = (Node*)kIStream.GetObjectAt(0);
}
//----------------------------------------------------------------------------
