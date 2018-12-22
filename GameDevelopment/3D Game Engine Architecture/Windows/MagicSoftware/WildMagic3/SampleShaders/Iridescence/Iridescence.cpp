// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Iridescence.h"

WM3_WINDOW_APPLICATION(Iridescence);

//----------------------------------------------------------------------------
Iridescence::Iridescence ()
    :
    WindowApplication3("Iridescence",0,0,640,480,ColorRGB(0.5f,0.0f,1.0f))
{
}
//----------------------------------------------------------------------------
bool Iridescence::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,100.0f);
    Vector3f kCLoc(0.0f,0.0f,-8.0f);
    Vector3f kCDir(0.0f,0.0f,1.0f);
    Vector3f kCUp(0.0f,1.0f,0.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    CreateScene();
    m_spkScene->UpdateGS(0.0f);
    m_spkScene->UpdateRS();

    InitializeCameraMotion(0.01f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void Iridescence::OnTerminate ()
{
    m_spkScene = 0;
    m_spkEffect = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void Iridescence::OnIdle ()
{
    MeasureTime();
    MoveCamera();

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
bool Iridescence::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if ( WindowApplication3::OnKeyDown(ucKey,iX,iY) )
        return true;

    float fInterpolateFactor;

    switch ( ucKey )
    {
    case '+':
    case '=':
        fInterpolateFactor = m_spkEffect->GetInterpolateFactor();
        fInterpolateFactor += 0.1f;
        if ( fInterpolateFactor > 1.0f )
            fInterpolateFactor = 1.0f;

        m_spkEffect->SetInterpolateFactor(fInterpolateFactor);
        return true;

    case '-':
    case '_':
        fInterpolateFactor = m_spkEffect->GetInterpolateFactor();
        fInterpolateFactor -= 0.1f;
        if ( fInterpolateFactor < 0.0f )
            fInterpolateFactor = 0.0f;

        m_spkEffect->SetInterpolateFactor(fInterpolateFactor);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void Iridescence::CreateScene ()
{
    m_spkScene = new Node;

    StandardMesh kSM;
    kSM.Normals(true,false).UVs(true,false);
    TriMesh* pkMesh = kSM.Torus(20,20,2.0f,1.0f);
    m_spkScene->AttachChild(pkMesh);

    Image* pkLeaf =
        Image::Load(System::GetPath("../../Data/Wmif/Leaf.wmif"));

    Image* pkGradient =
        Image::Load(System::GetPath("../../Data/Wmif/Gradient.wmif"));

    m_spkEffect = new IridescenceEffect(pkLeaf,pkGradient);
    m_spkEffect->SetInterpolateFactor(0.5f);

    pkMesh->SetEffect(m_spkEffect);

    m_spkEffect->Textures[0]->CoordU = Texture::WM_REPEAT;
    m_spkEffect->Textures[0]->CoordV = Texture::WM_REPEAT;
    m_spkEffect->UVs.Append(kSM.GetUVs());
}
//----------------------------------------------------------------------------
