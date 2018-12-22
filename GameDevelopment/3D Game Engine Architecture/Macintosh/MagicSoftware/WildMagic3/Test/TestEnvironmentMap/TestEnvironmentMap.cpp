// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "TestEnvironmentMap.h"

WM3_WINDOW_APPLICATION(TestEnvironmentMap);

//----------------------------------------------------------------------------
TestEnvironmentMap::TestEnvironmentMap ()
    :
    WindowApplication3("TestEnvironmentMap",0,0,640,480,
        ColorRGB(1.0f,1.0f,1.0f))
{
}
//----------------------------------------------------------------------------
bool TestEnvironmentMap::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    // select a font for text drawing
    int iFontID = m_pkRenderer->LoadFont("Verdana",24,false,false);
    m_pkRenderer->SelectFont(iFontID);

    // create scene
    m_spkScene = new Node;
    Node* pkTrnNode = new Node;
    m_spkScene->AttachChild(pkTrnNode);
    m_spkWireframe = new WireframeState;
    m_spkScene->SetGlobalState(m_spkWireframe);
    m_spkScene->SetGlobalState(new ZBufferState);
    pkTrnNode->AttachChild(CreateModel());

    // center-and-fit scene in the view frustum
    m_spkScene->UpdateGS();
    pkTrnNode->Local.SetTranslate(-m_spkScene->WorldBound->GetCenter());
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1000.0f);
    Vector3f kCDir(0.0f,1.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    Vector3f kCLoc = -3.0f*m_spkScene->WorldBound->GetRadius()*kCDir;
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    InitializeCameraMotion(0.1f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void TestEnvironmentMap::OnTerminate ()
{
    m_spkScene = 0;
    m_spkWireframe = 0;
    m_spkTEffect = 0;
    m_spkEMEffect = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void TestEnvironmentMap::OnIdle ()
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
bool TestEnvironmentMap::OnKeyDown (unsigned char ucKey, int iX, int iY)
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

    // Cycle through the various apply modes for the enviroment map:
    // replace, decal, modulate, blend, add
    case '+':
    case '=':
    {
        switch ( m_spkEMEffect->Textures[0]->Apply )
        {
        case Texture::AM_REPLACE:
            m_spkEMEffect->Textures[0]->Apply = Texture::AM_MODULATE;
            return true;
        case Texture::AM_MODULATE:
            m_spkEMEffect->Textures[0]->Apply = Texture::AM_BLEND;
            return true;
        case Texture::AM_BLEND:
            m_spkEMEffect->Textures[0]->Apply = Texture::AM_ADD;
            return true;
        default:
            m_spkEMEffect->Textures[0]->Apply = Texture::AM_REPLACE;
            return true;
        }
        return true;
    }
    }

    return false;
}
//----------------------------------------------------------------------------
Node* TestEnvironmentMap::CreateModel ()
{
    Node* pkModel = new Node;

    // load face model
    Stream kIStream;
    kIStream.Load(System::GetPath("../../Data/Wmof/Face.wmof"));
    TriMesh* pkMesh = StaticCast<TriMesh>(kIStream.GetObjectAt(0));
    pkMesh->GenerateNormals();
    pkModel->AttachChild(pkMesh);

    int iVQ = pkMesh->Vertices->GetQuantity();
    const Vector3f* akNormal = pkMesh->Normals->GetData();
    Vector2f* akUV = new Vector2f[iVQ];
    for (int i = 0; i < iVQ; i++)
    {
        akUV[i].X() = Mathf::ACos(akNormal[i].Z());
        akUV[i].Y() = Mathf::ATan2(akNormal[i].Y(),akNormal[i].X());
    }
    Vector2fArray* pkUVs = new Vector2fArray(iVQ,akUV);

    Texture* pkTexture = new Texture(
        Image::Load(System::GetPath("../../Data/Wmif/SunFire.wmif")));
    m_spkTEffect = new TextureEffect(pkTexture,pkUVs);
    pkMesh->SetEffect(m_spkTEffect);

    // create environment map
    Image* pkImage = Image::Load(
        System::GetPath("../../Data/Wmif/SphereMap.wmif"));
    m_spkEMEffect = new EnvironmentMapEffect(pkImage,Texture::AM_REPLACE);
    pkModel->SetEffect(m_spkEMEffect);

    return pkModel;
}
//----------------------------------------------------------------------------
void TestEnvironmentMap::TestStream ()
{
    Stream kOStream;
    kOStream.Insert(m_spkScene);
    kOStream.Save("TestEnvironmentMap.wmof");

    Stream kIStream;
    kIStream.Load("TestEnvironmentMap.wmof");
    NodePtr spkScene = (Node*)kIStream.GetObjectAt(0);
}
//----------------------------------------------------------------------------
