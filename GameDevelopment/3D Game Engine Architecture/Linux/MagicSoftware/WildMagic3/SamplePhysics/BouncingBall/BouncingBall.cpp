// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "BouncingBall.h"

WM3_WINDOW_APPLICATION(BouncingBall);

//#define SINGLE_STEP

//----------------------------------------------------------------------------
BouncingBall::BouncingBall ()
    :
    WindowApplication3("BouncingBall",0,0,640,480,ColorRGB(0.5f,0.0f,1.0f))
{
    m_pkBall = 0;
    m_fSimTime = 0.0f;
#ifdef SINGLE_STEP
    m_fSimDelta = 0.05f;
#else
    m_fSimDelta = 0.005f;
#endif
}
//----------------------------------------------------------------------------
bool BouncingBall::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    // set up camera
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1000.0f);
    float fAngle = 0.1f*Mathf::PI;
    float fCos = Mathf::Cos(fAngle), fSin = Mathf::Sin(fAngle);
    Vector3f kCLoc(6.75f,0.0f,2.3f);
    Vector3f kCDir(-fCos,0.0f,-fSin);
    Vector3f kCUp(-fSin,0.0f,fCos);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // create scene
    CreateBall();
    CreateFloor();
    CreateWall();
    m_spkScene = new Node(3);
    m_spkScene->AttachChild(m_spkFloor);
    m_spkScene->AttachChild(m_spkWall);
    m_spkScene->AttachChild(m_spkBall);

    // the floor reflects an image of the ball
    PlanarReflectionEffect* pkEffect = new PlanarReflectionEffect(1);
    pkEffect->SetPlane(0,m_spkFloor);
    pkEffect->SetReflectance(0,0.2f);
    m_spkBall->SetEffect(pkEffect);

    // wireframe
    m_spkWireframeState = new WireframeState;
    m_spkScene->SetGlobalState(m_spkWireframeState);

    // initial update of objects
    m_spkScene->UpdateGS(0.0f);
    m_spkScene->UpdateRS();

    // initialize ball with correct transformations
    DoPhysical();
    m_fSimTime = 0.0f;

    InitializeCameraMotion(0.1f,0.01f);
    return true;
}
//----------------------------------------------------------------------------
void BouncingBall::OnTerminate ()
{
    delete m_pkBall;
    m_spkScene = 0;
    m_spkBall = 0;
    m_spkFloor = 0;
    m_spkWall = 0;
    m_spkWireframeState = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void BouncingBall::OnIdle ()
{
    MeasureTime();
#ifndef SINGLE_STEP
    DoPhysical();
#endif
    DoVisual();
    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool BouncingBall::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if ( WindowApplication3::OnKeyDown(ucKey,iX,iY) )
        return true;

    switch ( ucKey )
    {
    case 'w':  // toggle wireframe
        m_spkWireframeState->Enabled = !m_spkWireframeState->Enabled;
        return true;
    case 's':  // toggle scaling
        m_pkBall->DoAffine() = !m_pkBall->DoAffine();
        return true;
#ifdef SINGLE_STEP
    case 'g':
        m_fSimTime += m_fSimDelta;
        DoPhysical();
        return true;
#endif
    }

    return false;
}
//----------------------------------------------------------------------------
void BouncingBall::CreateBall ()
{
    m_pkBall = new DeformableBall(1.0f,2.0f);
    m_spkBall = new Node(1);
    m_spkBall->AttachChild(m_pkBall->Mesh());
}
//----------------------------------------------------------------------------
void BouncingBall::CreateFloor ()
{
    const float fXExtent = 8.0f;
    const float fYExtent = 16.0f;
    const float fZValue = 0.0f;
    Vector3f* akVertex = new Vector3f[4];
    akVertex[0] = Vector3f(-fXExtent,-fYExtent,fZValue);
    akVertex[1] = Vector3f(+fXExtent,-fYExtent,fZValue);
    akVertex[2] = Vector3f(+fXExtent,+fYExtent,fZValue);
    akVertex[3] = Vector3f(-fXExtent,+fYExtent,fZValue);
    Vector3fArray* pkVertices = new Vector3fArray(4,akVertex);

    Vector2f* akUV = new Vector2f[4];
    akUV[0] = Vector2f(0.0f,0.0f);
    akUV[1] = Vector2f(1.0f,0.0f);
    akUV[2] = Vector2f(1.0f,1.0f);
    akUV[3] = Vector2f(0.0f,1.0f);
    Vector2fArray* pkUVs = new Vector2fArray(4,akUV);

    int* aiIndex = new int[6];
    aiIndex[0] = 0;  aiIndex[1] = 1;  aiIndex[2] = 2;
    aiIndex[3] = 0;  aiIndex[4] = 2;  aiIndex[5] = 3;
    IntArray* pkIndices = new IntArray(6,aiIndex);

    m_spkFloor = new TriMesh(pkVertices,pkIndices,false);

    Texture* pkTexture = new Texture(
        Image::Load(System::GetPath("../../Data/Wmif/Floor.wmif")));
    pkTexture->CoordU = Texture::WM_REPEAT;
    pkTexture->CoordV = Texture::WM_REPEAT;

    TextureEffect* pkEffect = new TextureEffect(pkTexture,pkUVs);
    m_spkFloor->SetEffect(pkEffect);
}
//----------------------------------------------------------------------------
void BouncingBall::CreateWall ()
{
    const float fXValue = -8.0f;
    const float fYExtent = 16.0f;
    const float fZExtent = 16.0f;
    Vector3f* akVertex = new Vector3f[4];
    akVertex[0] = Vector3f(fXValue,-fYExtent,0.0f);
    akVertex[1] = Vector3f(fXValue,+fYExtent,0.0f);
    akVertex[2] = Vector3f(fXValue,+fYExtent,fZExtent);
    akVertex[3] = Vector3f(fXValue,-fYExtent,fZExtent);
    Vector3fArray* pkVertices = new Vector3fArray(4,akVertex);

    const float fMax = 4.0f;
    Vector2f* akUV = new Vector2f[4];
    akUV[0] = Vector2f(0.0f,0.0f);
    akUV[1] = Vector2f(fMax,0.0f);
    akUV[2] = Vector2f(fMax,fMax);
    akUV[3] = Vector2f(0.0f,fMax);
    Vector2fArray* pkUVs = new Vector2fArray(4,akUV);

    int* aiIndex = new int[6];
    aiIndex[0] = 0;  aiIndex[1] = 1;  aiIndex[2] = 2;
    aiIndex[3] = 0;  aiIndex[4] = 2;  aiIndex[5] = 3;
    IntArray* pkIndices = new IntArray(6,aiIndex);

    m_spkWall = new TriMesh(pkVertices,pkIndices,false);

    Texture* pkTexture = new Texture(
        Image::Load(System::GetPath("../../Data/Wmif/Wall1.wmif")));
    pkTexture->CoordU = Texture::WM_REPEAT;
    pkTexture->CoordV = Texture::WM_REPEAT;

    TextureEffect* pkEffect = new TextureEffect(pkTexture,pkUVs);
    m_spkWall->SetEffect(pkEffect);
}
//----------------------------------------------------------------------------
void BouncingBall::DoPhysical ()
{
    // update the ball
    m_pkBall->DoSimulationStep(m_fSimTime);

    // ball parameters
    float fPer = m_pkBall->GetPeriod();
    float fTMin = m_pkBall->GetMinActive();
    float fTMax = m_pkBall->GetMaxActive();

    // translate the ball
    const float fYMax = 2.5f, fZMax = 0.75f;
    float fYTrn, fZTrn, fRatio, fAmp;
    float fTime = fmodf(m_fSimTime,2.0f*fPer);
    if ( fTime < fTMin )
    {
        fRatio = fTime/fTMin;
        fYTrn = fYMax*fRatio;
        fZTrn = fZMax*(1.0f - fRatio*fRatio);
    }
    else if ( fTime < fTMax )
    {
        fYTrn = fYMax;
        fAmp = m_pkBall->GetAmplitude(fTime);
        if ( fAmp <= 0.999f )
            fZTrn = -(1.0f-Mathf::Sqrt(1.0f-fAmp+fAmp*fAmp))/(1.0f-fAmp);
        else
            fZTrn = -0.5f;
    }
    else if ( fTime < fPer + fTMin )
    {
        fYTrn = -fYMax*(fTime-fPer)/fTMin;
        fZTrn = fZMax*(fTime-fTMax)*(fPer+fTMin-fTime)/(fTMin*(fPer-fTMax));
    }
    else if ( fTime < fPer + fTMax )
    {
        fYTrn = -fYMax;
        fAmp = m_pkBall->GetAmplitude(fTime-fPer);
        if ( fAmp <= 0.999f )
            fZTrn = -(1.0f-Mathf::Sqrt(1.0f-fAmp+fAmp*fAmp))/(1.0f-fAmp);
        else
            fZTrn = -0.5f;
    }
    else
    {
        fYTrn = fYMax*(fTime-2.0f*fPer)/(fPer-fTMax);
        fZTrn = fZMax*(fTime-(fPer+fTMax))*(2.0f*fPer+fTMin-fTime)/(fTMin*
            (fPer-fTMax));
    }
    m_spkBall->Local.SetTranslate(Vector3f(0.0f,fYTrn,fZTrn));

    // rotate the ball
    float fAngle = Mathf::HALF_PI+0.5f*fYTrn*Mathf::PI/fYMax;
    m_spkBall->Local.SetRotate(Matrix3f().FromAxisAngle(Vector3f::UNIT_Z,
        fAngle));

    // update the scene graph
    m_spkScene->UpdateGS(0.0f);

    // next simulation time
    m_fSimTime += m_fSimDelta;
}
//----------------------------------------------------------------------------
void BouncingBall::DoVisual ()
{
    m_pkRenderer->ClearBuffers();
    if ( m_pkRenderer->BeginScene() )
    {
        m_pkRenderer->DrawScene(m_spkScene);
        DrawFrameRate(8,GetHeight()-8,ColorRGBA::BLACK);

        char acMsg[256];
        sprintf(acMsg,"time = %6.4f",m_fSimTime);
        m_pkRenderer->Draw(128,GetHeight()-8,ColorRGBA::BLACK,acMsg);

        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();
}
//----------------------------------------------------------------------------
