// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Fresnel.h"

WM3_WINDOW_APPLICATION(Fresnel);

//----------------------------------------------------------------------------
Fresnel::Fresnel ()
    :
    WindowApplication3("Fresnel",0,0,640,480,ColorRGB(0.5f,0.0f,1.0f))
{

}
//----------------------------------------------------------------------------
bool Fresnel::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1000.0f);
    Vector3f kCLoc(0.0f,-200.0f,0.0f);
    Vector3f kCDir(0.0f,1.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    CreateScene();

    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    InitializeCameraMotion(0.1f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void Fresnel::OnTerminate ()
{
    m_spkScene = 0;
    m_spkEffect = 0;
    m_spkAlpha = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void Fresnel::OnIdle ()
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
bool Fresnel::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if ( WindowApplication3::OnKeyDown(ucKey,iX,iY) )
        return true;

    switch ( ucKey )
    {
    case 'a':
    case 'A':
        m_spkAlpha->BlendEnabled = !m_spkAlpha->BlendEnabled;
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void Fresnel::CreateScene ()
{
    Stream kStream;
    bool bLoaded = kStream.Load(System::GetPath("../../Data/Wmof/Face.wmof"));
    assert( bLoaded );
    m_spkScene = (Node*)kStream.GetObjectAt(0);

    m_spkAlpha = new AlphaState();
    m_spkAlpha->BlendEnabled = false;
    m_spkAlpha->SrcBlend = AlphaState::SBF_ONE;
    m_spkAlpha->DstBlend = AlphaState::DBF_ONE;
    m_spkScene->SetGlobalState(m_spkAlpha);
    
    m_spkEffect = new FresnelEffect;

    AttachShader(m_spkScene);
}
//----------------------------------------------------------------------------
void Fresnel::AttachShader (Spatial* pkObject)
{
    Geometry* pkGeom = DynamicCast<Geometry>(pkObject);
    if ( pkGeom )
    {
        FresnelEffect* pkClone = (FresnelEffect*)m_spkEffect->Clone();
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
