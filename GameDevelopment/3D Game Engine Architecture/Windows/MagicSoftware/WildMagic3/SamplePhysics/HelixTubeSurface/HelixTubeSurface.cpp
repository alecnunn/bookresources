// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "HelixTubeSurface.h"

WM3_WINDOW_APPLICATION(HelixTubeSurface);

//----------------------------------------------------------------------------
HelixTubeSurface::HelixTubeSurface ()
    :
    WindowApplication3("HelixTubeSurface",0,0,640,480,
        ColorRGB(0.9f,0.9f,0.9f))
{
    m_fDeltaTime = 0.01f;
}
//----------------------------------------------------------------------------
MultipleCurve3f* HelixTubeSurface::CreateCurve ()
{
    // sample points on a looped helix (first and last point must match)
    const float fFourPi = 4.0f*Mathf::PI;
    int iSegments = 32;
    int iSegmentsP1 = iSegments + 1;
    float* afTime = new float[iSegmentsP1];
    Vector3f* akPoint = new Vector3f[iSegmentsP1];
    int i;
    for (i = 0; i <= iSegmentsP1/2; i++)
    {
        afTime[i] = i*fFourPi/iSegmentsP1;
        akPoint[i].X() = Mathf::Cos(afTime[i]);
        akPoint[i].Y() = Mathf::Sin(afTime[i]);
        akPoint[i].Z() = afTime[i];
    }

    for (i = iSegmentsP1/2 + 1; i < iSegments; i++)
    {
        afTime[i] = i*fFourPi/iSegments;
        akPoint[i].X() = 2.0f - Mathf::Cos(afTime[i]);
        akPoint[i].Y() = Mathf::Sin(afTime[i]);
        akPoint[i].Z() = fFourPi - afTime[i];
    }

    afTime[iSegments] = fFourPi;
    akPoint[iSegments] = akPoint[0];

    // save min and max times
    m_fMinCurveTime = 0.0f;
    m_fMaxCurveTime = fFourPi;
    m_fCurvePeriod = m_fMaxCurveTime - m_fMinCurveTime;
    m_fCurveTime = m_fMinCurveTime;

    // create a closed cubic curve containing the sample points
    NaturalSpline3f* pkCurve = new NaturalSpline3f(NaturalSpline3f::BT_CLOSED,
        iSegments,afTime,akPoint);

    return pkCurve;
}
//----------------------------------------------------------------------------
float HelixTubeSurface::Radial (float)
{
    return 0.0625f;
}
//----------------------------------------------------------------------------
bool HelixTubeSurface::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    // Set up the camera.  The coordinate frame will be set by the MoveCamera
    // member function.
    m_spkCamera->SetFrustum(-0.0055f,0.0055f,-0.004125f,0.004125f,
        0.01f,10.0f);

    m_spkScene = new Node;
    m_spkWireframe = new WireframeState;
    m_spkScene->SetGlobalState(m_spkWireframe);

    m_pkCurve = CreateCurve();
    Vector2f kUVMin(0.0f,0.0f), kUVMax(1.0f,32.0f);
    TubeSurface* pkTube = new TubeSurface(m_pkCurve,Radial,true,
        Vector3f::UNIT_Z,256,32,false,false,false,true,&kUVMin,&kUVMax);

    Texture* pkTexture = new Texture(
        Image::Load(System::GetPath("../../Data/Wmif/Grating.wmif")));
    pkTexture->CoordU = Texture::WM_REPEAT;
    pkTexture->CoordV = Texture::WM_REPEAT;

    // The TubeSurface constructor sets the effect on pkTube and provides the
    // uv-coordinates.
    Effect* pkEffect = pkTube->GetEffect();
    pkEffect->Textures.Append(pkTexture);

    m_spkScene->AttachChild(pkTube);

    MoveCamera();

    // initial update of objects
    m_spkScene->UpdateGS(0.0f);
    m_spkScene->UpdateRS();
    return true;
}
//----------------------------------------------------------------------------
void HelixTubeSurface::OnTerminate ()
{
    m_spkScene = 0;
    m_spkWireframe = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
bool HelixTubeSurface::MoveCamera ()
{
    Vector3f kP = m_pkCurve->GetPosition(m_fCurveTime);
    Vector3f kT = m_pkCurve->GetTangent(m_fCurveTime);
    Vector3f kB = kT.UnitCross(Vector3f::UNIT_Z);
    Vector3f kN = kB.UnitCross(kT);
    m_spkCamera->SetFrame(kP,kT,kN,kB);
    return true;
}
//----------------------------------------------------------------------------
void HelixTubeSurface::OnIdle ()
{
    MeasureTime();

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
bool HelixTubeSurface::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if ( WindowApplication3::OnKeyDown(ucKey,iX,iY) )
        return true;

    switch ( ucKey )
    {
    case 'w':
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;
    case '+':
    case '=':
        m_fDeltaTime *= 2.0f;
        return true;
    case '-':
    case '_':
        m_fDeltaTime *= 0.5f;
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
bool HelixTubeSurface::OnSpecialKeyDown (int iKey, int, int)
{
    if ( iKey == KEY_UP_ARROW )
    {
        m_fCurveTime += m_fDeltaTime;
        if ( m_fCurveTime > m_fMaxCurveTime )
            m_fCurveTime -= m_fCurvePeriod;
        MoveCamera();
        return true;
    }

    if ( iKey == KEY_DOWN_ARROW )
    {
        m_fCurveTime -= m_fDeltaTime;
        if ( m_fCurveTime < m_fMinCurveTime )
            m_fCurveTime += m_fCurvePeriod;
        MoveCamera();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
