// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "TestCharcoalEffect.h"

WM3_WINDOW_APPLICATION(TestCharcoalEffect);

//----------------------------------------------------------------------------
TestCharcoalEffect::TestCharcoalEffect ()
    :
    WindowApplication3("TestCharcoalEffect",0,0,640,480,
        ColorRGB(1.0f,1.0f,1.0f))
{
    m_bSmudge = true;
    m_bDisplayPaper = true;
    m_bDisplayLighting = true;
}
//----------------------------------------------------------------------------
bool TestCharcoalEffect::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    // set up camera
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1000.0f);
    Vector3f kCLoc(80.0f,0.0f,23.0f);
    Vector3f kCDir(-1.0f,0.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // set up scene
    CreateScene();

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    InitializeCameraMotion(0.1f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void TestCharcoalEffect::OnTerminate ()
{
    m_spkScene = 0;
    m_spkEffect = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void TestCharcoalEffect::OnIdle ()
{
    MeasureTime();

    MoveCamera();
    if ( MoveObject() )
        m_spkScene->UpdateGS();

    m_pkRenderer->ClearBuffers();
    if ( m_pkRenderer->BeginScene() )
    {
        m_pkRenderer->DrawScene(m_spkScene);
        DrawFrameRate(8,GetHeight()-8,ColorRGBA::BLACK);
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool TestCharcoalEffect::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if ( WindowApplication3::OnKeyDown(ucKey,iX,iY) )
        return true;

    switch ( ucKey )
    {
    case 'p':
    case 'P':
        m_bDisplayPaper = !m_bDisplayPaper;
        UpdateConstants();
        return true;
    case 'm':
    case 'M':
        m_bSmudge = !m_bSmudge;
        UpdateConstants();
        return true;
    case 'l':
    case 'L':
        m_bDisplayLighting = !m_bDisplayLighting;
        UpdateConstants();
        return true;
    case 's':
    case 'S':
        TestStream();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void TestCharcoalEffect::CreateScene ()
{
    Stream kStream;
    bool bLoaded = kStream.Load(
        System::GetPath("../../Data/Wmof/SkinnedBiped.wmof"));
    assert( bLoaded );
    m_spkScene = (Node*)kStream.GetObjectAt(0);

    Light* pkLight0 = new Light(Light::LT_DIRECTIONAL);
    Vector3f kDir0 = Vector3f(-1.0f,0.3f,-0.8f);
    kDir0.Normalize();
    pkLight0->SetDirection(kDir0);
    m_spkScene->SetLight(pkLight0);

    Light* pkLight1 = new Light(Light::LT_DIRECTIONAL);
    Vector3f kDir1 = Vector3f(0.4f,-1.0f,-0.5f);
    kDir1.Normalize();
    pkLight1->SetDirection(kDir1);
    m_spkScene->SetLight(pkLight1);

    // contrast image (to be filled in by the CharcoalEffect object)
    int iWidth = 1024, iHeight = 1024, iQuantity = 4*iWidth*iHeight;
    unsigned char* aucData = new unsigned char[iQuantity];
    Image* pkContrastImage = new Image(Image::IT_RGBA8888,iWidth,iHeight,
        aucData,"ConstrastImage");

    // random image
    aucData = new unsigned char[iQuantity];
    for (int i = 0 ; i < iQuantity; i++)
        aucData[i] = (unsigned char)Mathf::IntervalRandom(0.0f,256.0f);
 
    Image* pkRandomImage = new Image(Image::IT_RGBA8888,iWidth,iHeight,
        aucData,"RandomImage");

    // paper image
    Image* pkPaperImage = Image::Load(
        System::GetPath("../../Data/Wmif/paper.wmif"));
    assert( pkPaperImage );

    m_spkEffect = new CharcoalEffect(pkContrastImage,pkRandomImage,
        pkPaperImage);
    m_spkEffect->SetAmbientIntensity(0.2f);
    m_spkEffect->SetContrastExponent(3.5f);
    m_spkEffect->SetSmudgeFactor(0.0f);
    m_spkEffect->SetPaperFactor(0.0f);

    AttachShader(m_spkScene);
}
//----------------------------------------------------------------------------
void TestCharcoalEffect::UpdateConstants ()
{
    if ( m_bDisplayLighting )
    {
        if ( m_bSmudge )
            m_spkEffect->SetSmudgeFactor(0.5f);
        else
            m_spkEffect->SetSmudgeFactor(1.0f);
    }
    else
    {
        m_spkEffect->SetSmudgeFactor(0.0f);
    }

    if ( m_bDisplayPaper )
        m_spkEffect->SetPaperFactor(0.0f);
    else
        m_spkEffect->SetPaperFactor(1.0f);
}
//----------------------------------------------------------------------------
void TestCharcoalEffect::AttachShader (Spatial* pkObject)
{
    Geometry* pkGeom = DynamicCast<Geometry>(pkObject);
    if ( pkGeom )
    {
        CharcoalEffect* pkClone = (CharcoalEffect*)m_spkEffect->Clone();
        pkClone->SetContrastTextureCoordinates(
            pkGeom->Vertices->GetQuantity());
        pkGeom->SetEffect(pkClone);
        return;
    }

    Node* pkNode = DynamicCast<Node>(pkObject);
    assert( pkNode );
    for (int i = 0; i < pkNode->GetQuantity(); i++)
    {
        Spatial* pkChild = pkNode->GetChild(i);
        if ( pkChild )
            AttachShader(pkChild);
    }
}
//----------------------------------------------------------------------------
void TestCharcoalEffect::TestStream ()
{
    Stream kOStream;
    kOStream.Insert(m_spkScene);
    kOStream.Save("TestCharcoalEffect.wmof");

    Stream kIStream;
    kIStream.Load("TestCharcoalEffect.wmof");
    NodePtr spkScene = (Node*)kIStream.GetObjectAt(0);
}
//----------------------------------------------------------------------------
