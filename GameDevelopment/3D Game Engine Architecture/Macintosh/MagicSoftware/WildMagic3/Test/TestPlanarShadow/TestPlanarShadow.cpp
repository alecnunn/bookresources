// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "TestPlanarShadow.h"

WM3_WINDOW_APPLICATION(TestPlanarShadow);

//----------------------------------------------------------------------------
TestPlanarShadow::TestPlanarShadow ()
    :
    WindowApplication3("TestPlanarShadow",0,0,640,480,
        ColorRGB(0.5f,0.0f,1.0f))
{
    m_dUpdateTime = 0.0;
}
//----------------------------------------------------------------------------
bool TestPlanarShadow::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    // scene graph:
    // Root
    //     BipedNode (attach PlanarShadowEffect)
    //     PlaneMesh0 (attach TextureEffect)
    //     PlaneMesh1 (attach TextureEffect)
    //     ProjectorLight

    m_spkScene = new Node(3);
    m_spkScene->SetGlobalState(new ZBufferState);
    m_spkWireframe = new WireframeState;
    m_spkScene->SetGlobalState(m_spkWireframe);

    LoadBiped();
    CreatePlanes();
    CreatePlanarShadow();

    // set up camera
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1000.0f);
    Vector3f kCLoc(80.0f,0.0f,23.0f);
    Vector3f kCDir(-1.0f,0.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS(m_dUpdateTime);
    m_spkScene->UpdateRS();

    InitializeCameraMotion(0.1f,0.01f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void TestPlanarShadow::OnTerminate ()
{
    m_spkScene = 0;
    m_spkBiped = 0;
    m_spkPlane0 = 0;
    m_spkPlane1 = 0;
    m_spkProjector0 = 0;
    m_spkProjector1 = 0;
    m_spkWireframe = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void TestPlanarShadow::OnIdle ()
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
bool TestPlanarShadow::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
        m_dUpdateTime += 0.01;
        m_spkScene->UpdateGS(m_dUpdateTime);
        return true;
    case 'G':
        m_dUpdateTime = 0.0;
        m_spkScene->UpdateGS(m_dUpdateTime);
        return true;

    case 's':
    case 'S':
        TestStream();
        return true;
    }

    const float fTrn = 1.0f;
    Vector3f kLoc;

    switch ( ucKey )
    {
    case 'x':
        kLoc = m_spkProjector0->GetLocation();
        kLoc.X() -= fTrn;
        m_spkProjector0->SetLocation(kLoc);
        break;
    case 'X':
        kLoc = m_spkProjector0->GetLocation();
        kLoc.X() += fTrn;
        m_spkProjector0->SetLocation(kLoc);
        break;
    case 'y':
        kLoc = m_spkProjector0->GetLocation();
        kLoc.Y() -= fTrn;
        m_spkProjector0->SetLocation(kLoc);
        break;
    case 'Y':
        kLoc = m_spkProjector0->GetLocation();
        kLoc.Y() += fTrn;
        m_spkProjector0->SetLocation(kLoc);
        break;
    case 'z':
        kLoc = m_spkProjector0->GetLocation();
        kLoc.Z() -= fTrn;
        m_spkProjector0->SetLocation(kLoc);
        break;
    case 'Z':
        kLoc = m_spkProjector0->GetLocation();
        kLoc.Z() += fTrn;
        m_spkProjector0->SetLocation(kLoc);
        break;
    default:
        return false;
    }

    m_spkProjector0->UpdateGS();
    return true;
}
//----------------------------------------------------------------------------
void TestPlanarShadow::LoadBiped ()
{
    Stream kIStream;
    kIStream.Load(System::GetPath("../../Data/Wmof/SkinnedBiped.wmof"));
    m_spkBiped = (Node*)kIStream.GetObjectAt(0);
    m_spkScene->AttachChild(m_spkBiped);

    Light* pkLight = new Light(Light::LT_DIRECTIONAL);
    pkLight->Ambient = ColorRGBA(0.5f,0.5f,0.5f,1.0f);
    pkLight->Diffuse = pkLight->Ambient;
    pkLight->Specular = ColorRGBA::BLACK;
    pkLight->SetDirection(-Vector3f::UNIT_X);
    pkLight->UpdateGS();
    m_spkBiped->SetLight(pkLight);
}
//----------------------------------------------------------------------------
void TestPlanarShadow::CreatePlanes ()
{
    // create the floor mesh
    float fXValue = 128.0f;
    float fYValue = 256.0f;
    float fZValue = 0.0f;
    Vector3f* akVertex = new Vector3f[4];
    akVertex[0] = Vector3f(-fXValue,-fYValue,fZValue);
    akVertex[1] = Vector3f(+fXValue,-fYValue,fZValue);
    akVertex[2] = Vector3f(+fXValue,+fYValue,fZValue);
    akVertex[3] = Vector3f(-fXValue,+fYValue,fZValue);
    Vector3fArray* pkVertices = new Vector3fArray(4,akVertex);

    int* aiIndex = new int[6];
    aiIndex[0] = 0;  aiIndex[1] = 1;  aiIndex[2] = 2;
    aiIndex[3] = 0;  aiIndex[4] = 2;  aiIndex[5] = 3;
    IntArray* pkIndices = new IntArray(6,aiIndex);

    m_spkPlane0 = new TriMesh(pkVertices,pkIndices,false);

    // create a texture effect for the plane
    unsigned char* aucData = new unsigned char[4*sizeof(int)];
    unsigned int uiColor = System::MakeRGBA(0x8F,0x7F,0x00,0xFF);
    unsigned int* auiData = (unsigned int*)aucData;
    auiData[0] = uiColor;
    auiData[1] = uiColor;
    auiData[2] = uiColor;
    auiData[3] = uiColor;
    Image* pkImage = new Image(Image::IT_RGBA8888,2,2,aucData);

    Vector2f* akUV = new Vector2f[4];
    akUV[0] = Vector2f(0.0f,0.0f);
    akUV[1] = Vector2f(1.0f,0.0f);
    akUV[2] = Vector2f(1.0f,1.0f);
    akUV[3] = Vector2f(0.0f,1.0f);
    Vector2fArray* pkUVs = new Vector2fArray(4,akUV);

    Texture* pkTexture0 = new Texture(pkImage);
    TextureEffect* pkEffect = new TextureEffect(pkTexture0,pkUVs);
    m_spkPlane0->SetEffect(pkEffect);

    m_spkScene->AttachChild(m_spkPlane0);

    // create the wall mesh
    fXValue = -128.0f;
    fYValue = 256.0f;
    fZValue = 128.0f;
    akVertex = new Vector3f[4];
    akVertex[0] = Vector3f(fXValue,-fYValue,0.0f);
    akVertex[1] = Vector3f(fXValue,+fYValue,0.0f);
    akVertex[2] = Vector3f(fXValue,+fYValue,fZValue);
    akVertex[3] = Vector3f(fXValue,-fYValue,fZValue);
    pkVertices = new Vector3fArray(4,akVertex);

    m_spkPlane1 = new TriMesh(pkVertices,pkIndices,false);

    // create a texture effect for the plane
    aucData = new unsigned char[4*sizeof(int)];
    uiColor = System::MakeRGBA(0x7F,0x7F,0x7F,0xFF);
    auiData = (unsigned int*)aucData;
    auiData[0] = uiColor;
    auiData[1] = uiColor;
    auiData[2] = uiColor;
    auiData[3] = uiColor;
    pkImage = new Image(Image::IT_RGBA8888,2,2,aucData);

    Texture* pkTexture1 = new Texture(pkImage);
    pkEffect = new TextureEffect(pkTexture1,pkUVs);
    m_spkPlane1->SetEffect(pkEffect);

    m_spkScene->AttachChild(m_spkPlane1);
}
//----------------------------------------------------------------------------
void TestPlanarShadow::CreatePlanarShadow ()
{
#if 1
    m_spkProjector0 = new Light(Light::LT_POINT);
    m_spkProjector0->SetLocation(Vector3f(152.0f,-55.0f,53.0f));
#else
    m_spkProjector0 = new Light(Light::LT_DIRECTIONAL);
    m_spkProjector0->SetDirection(Vector3f(0.25f,0.25f,-1.0f));
#endif

    m_spkProjector0->Ambient = ColorRGBA(0.125f,0.125f,0.125f,1.0f);
    m_spkProjector0->Diffuse = ColorRGBA(0.5f,0.5f,0.5f,1.0f);
    m_spkProjector0->Specular = ColorRGBA(0.1f,0.1f,0.1f,1.0f);

    PlanarShadowEffect* pkEffect = new PlanarShadowEffect(2);
    ColorRGBA kShadowColor(0.0f,0.0f,0.0f,0.5f);
    pkEffect->SetPlane(0,m_spkPlane0);
    pkEffect->SetProjector(0,m_spkProjector0);
    pkEffect->SetShadowColor(0,kShadowColor);
    pkEffect->SetPlane(1,m_spkPlane1);
    pkEffect->SetProjector(1,m_spkProjector0);
    pkEffect->SetShadowColor(1,kShadowColor);
    m_spkBiped->SetEffect(pkEffect);

    m_spkScene->AttachChild(m_spkProjector0);
}
//----------------------------------------------------------------------------
void TestPlanarShadow::TestStream ()
{
    Stream kOStream;
    kOStream.Insert(m_spkScene);
    kOStream.Save("TestPlanarShadow.wmof");

    Stream kIStream;
    kIStream.Load("TestPlanarShadow.wmof");
    NodePtr spkScene = (Node*)kIStream.GetObjectAt(0);
}
//----------------------------------------------------------------------------
