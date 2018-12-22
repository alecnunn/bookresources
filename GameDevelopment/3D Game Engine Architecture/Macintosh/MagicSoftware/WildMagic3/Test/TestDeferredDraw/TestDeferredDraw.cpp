// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "TestDeferredDraw.h"

WM3_WINDOW_APPLICATION(TestDeferredDraw);

//----------------------------------------------------------------------------
TestDeferredDraw::TestDeferredDraw ()
    :
    WindowApplication3("TestDeferredDraw",0,0,640,480,
        ColorRGB(0.5f,0.0f,1.0f))
{
}
//----------------------------------------------------------------------------
bool TestDeferredDraw::OnInitialize ()
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
    m_spkScene->SetGlobalState(new ZBufferState);
    pkTrnNode->AttachChild(CreateModel());
    pkTrnNode->AttachChild(CreateFlat());

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
void TestDeferredDraw::OnTerminate ()
{
    m_spkScene = 0;
    m_spkTEffect = 0;
    m_spkEMEffect = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void TestDeferredDraw::OnIdle ()
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
bool TestDeferredDraw::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if ( WindowApplication3::OnKeyDown(ucKey,iX,iY) )
        return true;

    switch ( ucKey )
    {
    // Cycle through the various apply modes for the enviroment map:
    // replace, decal, modulate, blend, add
    case '+':
    case '=':
    {
        switch ( m_spkEMEffect->Textures[0]->Apply )
        {
        case Texture::AM_REPLACE:
            m_spkEMEffect->Textures[0]->Apply = Texture::AM_DECAL;
            return true;
        case Texture::AM_DECAL:
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

    // cycle through some deferred drawers
    case '0':
        m_pkRenderer->DrawDeferred = 0;
        return true;
    case '1':
        m_pkRenderer->DrawDeferred = &Renderer::DrawDeferredNoSort;
        return true;
    case '2':
        m_pkRenderer->DrawDeferred = &Renderer::DrawDeferredNoDraw;
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
Node* TestDeferredDraw::CreateModel ()
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
Node* TestDeferredDraw::CreateFlat ()
{
    Node* pkNode = new Node(4);

    CullState* pkCS = new CullState;
    pkCS->Enabled = false;
    pkNode->SetGlobalState(pkCS);
    Vector3f kTrn = pkNode->Local.GetTranslate();
    kTrn.Y() -= 30.0f;
    pkNode->Local.SetTranslate(kTrn);
    pkNode->Local.SetUniformScale(20.0f);

    // vertices
    Vector3f* akVertex = new Vector3f[4];
    akVertex[0] = Vector3f(-0.5f,0.0f,-0.5f);
    akVertex[1] = Vector3f(-0.5f,0.0f, 0.5f);
    akVertex[2] = Vector3f( 0.5f,0.0f, 0.5f);
    akVertex[3] = Vector3f( 0.5f,0.0f,-0.5f);
    Vector3fArray* pkVertices = new Vector3fArray(4,akVertex);

    // texture coordinates
    Vector2f* akUV = new Vector2f[4];
    akUV[0] = Vector2f(1.0f,0.0f);
    akUV[1] = Vector2f(1.0f,1.0f);
    akUV[2] = Vector2f(0.0f,1.0f);
    akUV[3] = Vector2f(0.0f,0.0f);
    Vector2fArray* pkUVs = new Vector2fArray(4,akUV);

    // indices
    int* aiIndex = new int[6];
    aiIndex[0] = 0;  aiIndex[1] = 1; aiIndex[2] = 3;
    aiIndex[3] = 3;  aiIndex[4] = 1; aiIndex[5] = 2;
    IntArray* pkIndices = new IntArray(6,aiIndex);

    // images
    Image* pkDoorImage = Image::Load(
        System::GetPath("../../Data/Wmif/Door.wmif"));
    Image* pkRadialImage = Image::Load(
        System::GetPath("../../Data/Wmif/YellowRadial.wmif"));

    Matrix3f kRot(Vector3f::UNIT_X,-0.25f*Mathf::PI);

    // square with door texture only
    TriMesh* pkSquare0 = new TriMesh(pkVertices,pkIndices,false);
    pkSquare0->Local.SetRotate(kRot);
    pkSquare0->Local.SetTranslate(Vector3f(1.0f,-1.0f,0.0f));
    pkNode->AttachChild(pkSquare0);
    Texture* pkTexture0 = new Texture(pkDoorImage);
    Effect* pkEffect0 = new TextureEffect(pkTexture0,pkUVs);
    pkSquare0->SetEffect(pkEffect0);

    // square with radial texture only
    TriMesh* pkSquare1 = new TriMesh(pkVertices,pkIndices,false);
    pkSquare1->Local.SetRotate(kRot);
    pkSquare1->Local.SetTranslate(Vector3f(0.0f,-1.0f,0.0f));
    pkNode->AttachChild(pkSquare1);
    Texture* pkTexture1 = new Texture(pkRadialImage);
    Effect* pkEffect1 = new TextureEffect(pkTexture1,pkUVs);
    pkSquare1->SetEffect(pkEffect1);

    // square with dark map (texture1 multiplied with texture0)
    TriMesh* pkSquare2 = new TriMesh(pkVertices,pkIndices,false);
    pkSquare2->Local.SetRotate(kRot);
    pkSquare2->Local.SetTranslate(Vector3f(-1.0f,-1.0f,0.0f));
    pkNode->AttachChild(pkSquare2);
    Texture* pkTexture2 = new Texture(pkRadialImage);
    Effect* pkEffect2 = new DarkMapEffect(pkTexture0,pkUVs,pkTexture2,pkUVs);
    pkSquare2->SetEffect(pkEffect2);

    // square with light map (texture1 added to texture0)
    TriMesh* pkSquare3 = new TriMesh(pkVertices,pkIndices,false);
    pkSquare3->Local.SetRotate(kRot);
    pkSquare3->Local.SetTranslate(Vector3f(-2.0f,-1.0f,0.0f));
    pkNode->AttachChild(pkSquare3);
    Texture* pkTexture3 = new Texture(pkRadialImage);
    Effect* pkEffect3 = new LightMapEffect(pkTexture0,pkUVs,pkTexture3,pkUVs,
        false);
    pkSquare3->SetEffect(pkEffect3);

    return pkNode;
}
//----------------------------------------------------------------------------
