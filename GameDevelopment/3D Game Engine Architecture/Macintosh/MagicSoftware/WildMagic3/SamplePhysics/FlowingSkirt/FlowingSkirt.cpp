// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "FlowingSkirt.h"

WM3_WINDOW_APPLICATION(FlowingSkirt);

//#define SINGLE_STEP

//----------------------------------------------------------------------------
FlowingSkirt::FlowingSkirt ()
    :
    WindowApplication3("FlowingSkirt",0,0,640,480,ColorRGB(0.75f,0.75f,0.75f))
{
    m_iNumCtrl = 32;
    m_iDegree = 3;
    m_fATop = 1.0f;
    m_fBTop = 1.5f;
    m_fABot = 2.0f;
    m_fBBot = 3.0f;
    m_pkSkirtTop = 0;
    m_pkSkirtBot = 0;
    m_afFrequency = new float[m_iNumCtrl];
}
//----------------------------------------------------------------------------
FlowingSkirt::~FlowingSkirt ()
{
    delete[] m_afFrequency;
}
//----------------------------------------------------------------------------
bool FlowingSkirt::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    m_spkScene = new Node(1);
    m_spkTrnNode = new Node(1);
    m_spkScene->AttachChild(m_spkTrnNode);
    m_spkWireframe = new WireframeState;
    m_spkScene->SetGlobalState(m_spkWireframe);

    CreateSkirt();

    // center-and-fit for camera viewing
    m_spkScene->UpdateGS(0.0f);
    m_spkTrnNode->Local.SetTranslate(-m_spkScene->WorldBound->GetCenter());

    // sample camera code
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,100.0f);
    Vector3f kCLeft(1.0f,0.0f,0.0f);
    Vector3f kCDir(0.0f,0.0f,1.0f);
    Vector3f kCUp(0.0f,1.0f,0.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    Vector3f kCLoc = -3.0f*m_spkScene->WorldBound->GetRadius()*kCDir;
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS(0.0f);
    m_spkScene->UpdateRS();

    InitializeCameraMotion(0.005f,0.01f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void FlowingSkirt::OnTerminate ()
{
    delete m_pkSkirtTop;
    delete m_pkSkirtBot;
    m_spkScene = 0;
    m_spkTrnNode = 0;
    m_spkSkirt = 0;
    m_spkWireframe = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void FlowingSkirt::OnIdle ()
{
    MeasureTime();
    MoveCamera();

    if ( MoveObject() )
        m_spkScene->UpdateGS(0.0f);

#ifndef SINGLE_STEP
    ModifyCurves();
#endif

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
bool FlowingSkirt::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if ( WindowApplication3::OnKeyDown(ucKey,iX,iY) )
        return true;

    switch ( ucKey )
    {
    case 'w':  // toggle wireframe
    case 'W':
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;
#ifdef SINGLE_STEP
    case 'g':
    case 'G':
        ModifyCurves();
        return true;
#endif
    }

    return false;
}
//----------------------------------------------------------------------------
void FlowingSkirt::CreateSkirt ()
{
    // The skirt top and bottom boundary curves are chosen to be periodic,
    // looped B-spline curves.  The top control points are generated on an
    // ellipse (x/a0)^2 + (z/b0)^2 = 1 with y = 4.  The bottom control points
    // are generated on an ellipse (x/a1)^2 + (z/b1)^2 = 1 with y = 0.

    // The vertex storage is used for the B-spline control points.  The
    // curve objects make a copy of the input points.  The vertex storage is
    // then used for the skirt mesh vertices themselves.
    int iVQuantity = 2*m_iNumCtrl;
    Vector3f* akVertex = new Vector3f[iVQuantity];
    Vector3fArray* pkVertices = new Vector3fArray(iVQuantity,akVertex);
    Vector2f* akUV = new Vector2f[iVQuantity];
    Vector2fArray* pkUVs = new Vector2fArray(iVQuantity,akUV);

    int i, j;
    for (i = 0, j = m_iNumCtrl; i < m_iNumCtrl; i++, j++)
    {
        float fRatio = ((float)i)/((float)m_iNumCtrl);
        float fAngle = Mathf::TWO_PI*fRatio;
        float fSin = Mathf::Sin(fAngle);
        float fCos = Mathf::Cos(fAngle);
        float fV = 1.0f - Mathf::FAbs(2.0f*fRatio - 1.0f);

        // skirt top
        akVertex[i].X() = m_fATop*fCos;
        akVertex[i].Y() = 4.0f;
        akVertex[i].Z() = m_fBTop*fSin;
        akUV[i].X() = 1.0f;
        akUV[i].Y() = fV;

        // skirt bottom
        akVertex[j].X() = m_fABot*fCos;
        akVertex[j].Y() = 0.0f;
        akVertex[j].Z() = m_fBBot*fSin;
        akUV[j].X() = 0.0f;
        akUV[j].Y() = fV;

        // frequency of sinusoidal motion for skirt bottom
        m_afFrequency[i] = 0.5f*(1.0f+Mathf::UnitRandom());
    }

    // the control points are copied by the curve objects
    m_pkSkirtTop = new BSplineCurve3f(m_iNumCtrl,akVertex,m_iDegree,true,
        false);

    m_pkSkirtBot = new BSplineCurve3f(m_iNumCtrl,&akVertex[m_iNumCtrl],
        m_iDegree,true,false);

    // generate the triangle connectivity (cylinder connectivity)
    int iTQuantity = iVQuantity;
    int iIQuantity = 3*iTQuantity;
    int* aiIndex = new int[iIQuantity];
    int i0 = 0, i1 = 1, i2 = m_iNumCtrl, i3 = m_iNumCtrl+1;
    for (i = 0; i1 < m_iNumCtrl; i0 = i1++, i2 = i3++)
    {
        aiIndex[i++] = i0;
        aiIndex[i++] = i1;
        aiIndex[i++] = i3;
        aiIndex[i++] = i0;
        aiIndex[i++] = i3;
        aiIndex[i++] = i2;
    }
    aiIndex[i++] = m_iNumCtrl-1;
    aiIndex[i++] = 0;
    aiIndex[i++] = m_iNumCtrl;
    aiIndex[i++] = m_iNumCtrl-1;
    aiIndex[i++] = m_iNumCtrl;
    aiIndex[i++] = 2*m_iNumCtrl-1;

    IntArray* pkIndices = new IntArray(iIQuantity,aiIndex);

    m_spkSkirt = new TriMesh(pkVertices,pkIndices,false);

    // attach a texture for the skirt
    Texture* pkTexture = new Texture(
        Image::Load(System::GetPath("../../Data/Wmif/Flower.wmif")));
    pkTexture->CoordU = Texture::WM_REPEAT;
    pkTexture->CoordV = Texture::WM_REPEAT;

    TextureEffect* pkEffect = new TextureEffect(pkTexture,pkUVs);
    m_spkSkirt->SetEffect(pkEffect);

    // double-sided triangles
    CullState* pkCS = new CullState;
    pkCS->Enabled = false;
    m_spkSkirt->SetGlobalState(pkCS);

    // compute the vertex values for the current B-spline curves
    UpdateSkirt();

    m_spkTrnNode->AttachChild(m_spkSkirt);
}
//----------------------------------------------------------------------------
void FlowingSkirt::UpdateSkirt ()
{
    Vector3f* akVertex = m_spkSkirt->Vertices->GetData();
    for (int i = 0, j = m_iNumCtrl; i < m_iNumCtrl; i++, j++)
    {
        float fT = ((float)i)/((float)m_iNumCtrl);
        akVertex[i] = m_pkSkirtTop->GetPosition(fT);
        akVertex[j] = m_pkSkirtBot->GetPosition(fT);
    }

    m_spkSkirt->UpdateMS(false);
    m_spkSkirt->UpdateGS(0.0f);
}
//----------------------------------------------------------------------------
void FlowingSkirt::ModifyCurves ()
{
    // perturb the skirt bottom
    float fTime = (float)System::GetTime();
    for (int i = 0; i < m_iNumCtrl; i++)
    {
        float fRatio = ((float)i)/((float)m_iNumCtrl);
        float fAngle = Mathf::TWO_PI*fRatio;
        float fSin = Mathf::Sin(fAngle);
        float fCos = Mathf::Cos(fAngle);
        float fAmplitude = 1.0f + 0.25f*Mathf::Cos(m_afFrequency[i]*fTime);
        m_pkSkirtBot->SetControlPoint(i,
            Vector3f(fAmplitude*m_fABot*fCos,0.0f,fAmplitude*m_fBBot*fSin));
    }

    UpdateSkirt();
}
//----------------------------------------------------------------------------
