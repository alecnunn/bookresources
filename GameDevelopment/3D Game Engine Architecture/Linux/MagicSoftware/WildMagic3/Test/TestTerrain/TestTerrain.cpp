// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "TestTerrain.h"

WM3_WINDOW_APPLICATION(TestTerrain);

//----------------------------------------------------------------------------
TestTerrain::TestTerrain ()
    :
    WindowApplication3("TestTerrain",0,0,640,480,ColorRGB(0.5f,0.0f,1.0f)),
        m_kFogColor(ColorRGBA(145.0f/255.0f,185.0f/255.0f,213.0f/255.0f,1.0f))
{
    m_fUVBias = 0.0f;
    m_fHeightAboveTerrain = 20.0f;
}
//----------------------------------------------------------------------------
bool TestTerrain::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    // Position the camera in the middle of page[0][0].  Orient it to look
    // diagonally into the terrain.
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1500.0f);
    Vector3f kCLoc(64.0f,64.0f,m_fHeightAboveTerrain);
    Vector3f kCDir(1.0f,1.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    kCDir.Normalize();
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);
    m_spkCamera->UpdateGS();

    // create scene graph
    m_spkScene = new Node(2);
    m_spkWireframe = new WireframeState;
    m_spkScene->SetGlobalState(m_spkWireframe);
    CreateSkyDome();
    CreateTerrain();

    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    InitializeCameraMotion(1.0f,0.01f);
    MoveForward();
    return true;
}
//----------------------------------------------------------------------------
void TestTerrain::OnTerminate ()
{
    m_spkWireframe = 0;
    m_spkScene = 0;
    m_spkSkyDome = 0;
    m_spkTerrain = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void TestTerrain::OnIdle ()
{
    MeasureTime();

    if ( MoveCamera() )
    {
        // The sky dome moves with the camera so that it is always in view.
        Vector3f kTrn = m_spkCamera->GetLocation();
        kTrn.Z() = 0.0f;
        m_spkSkyDome->Local.SetTranslate(kTrn);
        m_spkSkyDome->UpdateGS();

        // update the active terrain pages
        m_spkTerrain->OnCameraMotion();
    }

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
bool TestTerrain::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
void TestTerrain::CreateSkyDome ()
{
    // Load hemispherical sky dome.  The bounding box is [-1,1]x[-1,1]x[0,1].
    // The dome is scaled to the proper size for this application.  The sky
    // dome uses cached arrays for the graphics card.  If the application
    // crashes, try Load with "SkyDome.nocache.wmof" instead.
    Stream kStream;
    kStream.Load(System::GetPath("../../Data/Wmof/SkyDome.cache.wmof"));
    m_spkSkyDome = DynamicCast<TriMesh>(kStream.GetObjectAt(0));
    assert( m_spkSkyDome );
    m_spkScene->AttachChild(m_spkSkyDome);

    // scale the sky dome so that it just fits inside the far plane
    m_spkSkyDome->Local.SetUniformScale(m_spkCamera->GetDMax());

    // sky dome follows camera
    Vector3f kCLoc = m_spkCamera->GetWorldLocation();
    m_spkSkyDome->Local.SetTranslate(Vector3f(kCLoc.X(),kCLoc.Y(),0.0f));
}
//----------------------------------------------------------------------------
void TestTerrain::CreateTerrain ()
{
    // create the full terrain
    m_spkTerrain = new Terrain(
        System::GetPath("../../Data/Terrain/Height128/height"),
        System::GetPath("../../Data/Terrain/Image128/image"),
        m_spkCamera,m_fUVBias,&m_kFogColor);
    m_spkScene->AttachChild(m_spkTerrain);

    // Set up for modulating by a detail texture.  This texture is set to
    // repeat four times per page.
    Vector2fArrayPtr spkUVs = m_spkTerrain->GetPage(0,0)->GetUVs();
    int iVQuantity = spkUVs->GetQuantity();
    Vector2f* akUV = spkUVs->GetData();
    Vector2f* akDetailUV = new Vector2f[iVQuantity];
    memcpy(akDetailUV,akUV,iVQuantity*sizeof(Vector2f));
    for (int i = 0; i < iVQuantity; i++)
        akUV[i] = akDetailUV[i];

    Vector2fArray* pkDetailUVs = new Vector2fArray(iVQuantity,akDetailUV);
    Texture* pkDetailTexture = new Texture(
        Image::Load(System::GetPath("../../Data/Terrain/Detail.wmif")));

    // replace the textures by dark maps
    int iRows = m_spkTerrain->GetRowQuantity();
    int iCols = m_spkTerrain->GetColQuantity();
    for (int iRow = 0; iRow < iRows; iRow++)
    {
        for (int iCol = 0; iCol < iCols; iCol++)
        {
            TerrainPage* pkPage = m_spkTerrain->GetPage(iRow,iCol);
            Effect* pkEffect = pkPage->GetEffect();
            TexturePtr spkTexture = pkEffect->Textures[0];
            Vector2fArrayPtr spkUVs = pkEffect->UVs[0];
            DarkMapEffect* pkDMEffect = new DarkMapEffect(spkTexture,spkUVs,
                pkDetailTexture,pkDetailUVs);
            pkPage->SetEffect(pkDMEffect);
        }
    }

    // use fog to hide horizon
    FogState* pkFS = new FogState;
    pkFS->Enabled = true;
    pkFS->Density = 0.0015f;
    pkFS->Color = m_kFogColor;
    pkFS->DensityFunction = FogState::DF_EXPSQR;
    m_spkTerrain->SetGlobalState(pkFS);
}
//----------------------------------------------------------------------------
void TestTerrain::MoveForward ()
{
    WindowApplication3::MoveForward();

    Vector3f kLoc = m_spkCamera->GetLocation();
    float fHeight = m_spkTerrain->GetHeight(kLoc.X(),kLoc.Y());
    kLoc.Z() = fHeight + m_fHeightAboveTerrain;
    m_spkCamera->SetLocation(kLoc);
    m_spkCamera->UpdateGS();
}
//----------------------------------------------------------------------------
void TestTerrain::MoveBackward ()
{
    WindowApplication3::MoveBackward();

    Vector3f kLoc = m_spkCamera->GetLocation();
    float fHeight = m_spkTerrain->GetHeight(kLoc.X(),kLoc.Y());
    kLoc.Z() = fHeight + m_fHeightAboveTerrain;
    m_spkCamera->SetLocation(kLoc);
    m_spkCamera->UpdateGS();
}
//----------------------------------------------------------------------------
void TestTerrain::MoveDown ()
{
    if ( m_fHeightAboveTerrain >= m_fTrnSpeed )
        m_fHeightAboveTerrain -= m_fTrnSpeed;

    Vector3f kLoc = m_spkCamera->GetLocation();
    float fHeight = m_spkTerrain->GetHeight(kLoc.X(),kLoc.Y());
    kLoc.Z() = fHeight + m_fHeightAboveTerrain;
    m_spkCamera->SetLocation(kLoc);
    m_spkCamera->UpdateGS();
}
//----------------------------------------------------------------------------
void TestTerrain::MoveUp ()
{
    m_fHeightAboveTerrain += m_fTrnSpeed;

    Vector3f kLoc = m_spkCamera->GetLocation();
    float fHeight = m_spkTerrain->GetHeight(kLoc.X(),kLoc.Y());
    kLoc.Z() = fHeight + m_fHeightAboveTerrain;
    m_spkCamera->SetLocation(kLoc);
    m_spkCamera->UpdateGS();
}
//----------------------------------------------------------------------------
void TestTerrain::TestStream ()
{
    Stream kOStream;
    kOStream.Insert(m_spkScene);
    kOStream.Save("TestTerrain.wmof");

    Stream kIStream;
    kIStream.Load("TestTerrain.wmof");
    NodePtr spkScene = (Node*)kIStream.GetObjectAt(0);
}
//----------------------------------------------------------------------------
