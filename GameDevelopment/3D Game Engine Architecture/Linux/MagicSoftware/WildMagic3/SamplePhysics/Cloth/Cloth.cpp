// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Cloth.h"

WM3_WINDOW_APPLICATION(Cloth);

//#define SINGLE_STEP

//----------------------------------------------------------------------------
Cloth::Cloth ()
    :
    WindowApplication3("Cloth",0,0,640,480,ColorRGB(0.85f,0.85f,1.00f))
{
    m_pkSpline = 0;
    m_pkModule = 0;
}
//----------------------------------------------------------------------------
bool Cloth::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    CreateScene();

    // center-and-fit for camera viewing
    m_spkScene->UpdateGS(0.0f);
    m_spkTrnNode->Local.SetTranslate(-m_spkScene->WorldBound->GetCenter());
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,100.0f);
    Vector3f kCDir(0.0f,1.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    Vector3f kCLoc = -3.0f*m_spkScene->WorldBound->GetRadius()*kCDir;
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS(0.0f);
    m_spkScene->UpdateRS();

    InitializeCameraMotion(0.001f,0.01f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void Cloth::OnTerminate ()
{
    delete m_pkModule;
    m_spkScene = 0;
    m_spkTrnNode = 0;
    m_spkWireframe = 0;
    m_spkCloth = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void Cloth::OnIdle ()
{
    MeasureTime();
    MoveCamera();

    if ( MoveObject() )
        m_spkScene->UpdateGS(0.0f);

#ifndef SINGLE_STEP
    DoPhysical();
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
bool Cloth::OnKeyDown (unsigned char ucKey, int iX, int iY)
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
        DoPhysical();
        return true;
#endif
    }

    return false;
}
//----------------------------------------------------------------------------
void Cloth::CreateSprings ()
{
    // set up the mass-spring system
    int iRows = 8;
    int iCols = 16;
    float fStep = 0.01f;
    Vector3f kGravity(0.0f,0.0f,-1.0f);
    Vector3f kWind(0.5f,0.0f,0.0f);
    float fViscosity = 10.0f;
    float fMaxAmplitude = 2.0f;
    m_pkModule = new PhysicsModule(iRows,iCols,fStep,kGravity,kWind,
        fViscosity,fMaxAmplitude);

    // The top row of the mesh is immovable (infinite mass).  All other
    // masses are constant.
    int iRow, iCol;
    for (iCol = 0; iCol < iCols; iCol++)
        m_pkModule->SetMass(iRows-1,iCol,Mathf::MAX_REAL);
    for (iRow = 0; iRow < iRows-1; iRow++)
    {
        for (iCol = 0; iCol < iCols; iCol++)
            m_pkModule->SetMass(iRow,iCol,1.0f);
    }

    // initial position on a vertical axis-aligned rectangle, zero velocity
    float fRFactor = 1.0f/(float)(iRows-1);
    float fCFactor = 1.0f/(float)(iCols-1);
    for (iRow = 0; iRow < iRows; iRow++)
    {
        for (iCol = 0; iCol < iCols; iCol++)
        {
            m_pkModule->Position(iRow,iCol) =
                Vector3f(iCol*fCFactor,0.0f,iRow*fRFactor);
            m_pkModule->Velocity(iRow,iCol) = Vector3f::ZERO;
        }
    }

    // springs are at rest in the initial configuration
    const float fRConstant = 1000.0f;
    const float fBConstant = 100.0f;
    Vector3f kDiff;
    for (iRow = 0; iRow < iRows; iRow++)
    {
        for (iCol = 0; iCol < iCols-1; iCol++)
        {
            m_pkModule->ConstantC(iRow,iCol) = fRConstant;
            kDiff = m_pkModule->Position(iRow,iCol+1) -
                m_pkModule->Position(iRow,iCol);
            m_pkModule->LengthC(iRow,iCol) = kDiff.Length();
        }
    }

    for (iRow = 0; iRow < iRows-1; iRow++)
    {
        for (iCol = 0; iCol < iCols; iCol++)
        {
            m_pkModule->ConstantR(iRow,iCol) = fBConstant;
            kDiff = m_pkModule->Position(iRow,iCol) -
                m_pkModule->Position(iRow+1,iCol);
            m_pkModule->LengthR(iRow,iCol) = kDiff.Length();
        }
    }
}
//----------------------------------------------------------------------------
void Cloth::CreateCloth ()
{
    // create quadratic spline using particles as control points
    m_pkSpline = new BSplineRectanglef(m_pkModule->GetRows(),
        m_pkModule->GetCols(),m_pkModule->Positions2D(),2,2,false,false,
        true,true);

    // generate a rectangle surface
    int iUSamples = 16;
    int iVSamples = 32;
    bool bWantNormals = false;
    bool bWantColors = false;
    bool bDoubleSided = true;
    Vector2f kTextureMin(0.0f,0.0f), kTextureMax(1.0f,1.0f);
    m_spkCloth = new RectangleSurface(m_pkSpline,iUSamples,iVSamples,
        bWantNormals,bWantColors,bDoubleSided,&kTextureMin,&kTextureMax);

    // attach a texture for the surface
    Texture* pkTexture = new Texture(
        Image::Load(System::GetPath("../../Data/Wmif/Cloth.wmif")));
    pkTexture->CoordU = Texture::WM_REPEAT;
    pkTexture->CoordV = Texture::WM_REPEAT;

    // The Effect and uv-coordinates were already generated by the
    // constructor RectangleSurface.
    Effect* pkEffect = m_spkCloth->GetEffect();
    pkEffect->Textures.Append(pkTexture);

    m_spkTrnNode->AttachChild(m_spkCloth);
}
//----------------------------------------------------------------------------
void Cloth::CreateScene ()
{
    m_spkScene = new Node(1);
    m_spkTrnNode = new Node(1);
    m_spkScene->AttachChild(m_spkTrnNode);
    m_spkWireframe = new WireframeState;
    m_spkScene->SetGlobalState(m_spkWireframe);

    CreateSprings();
    CreateCloth();
}
//----------------------------------------------------------------------------
void Cloth::DoPhysical ()
{
    m_pkModule->Update((float)System::GetTime());

    // Update spline surface.  Remember that the spline maintains its own
    // copy of the control points, so this update is necessary.
    int iRows = m_pkModule->GetRows();
    int iCols = m_pkModule->GetCols();
    Vector3f** aakCtrlPoint = m_pkModule->Positions2D();
    for (int iRow = 0; iRow < iRows; iRow++)
    {
        for (int iCol = 0; iCol < iCols; iCol++)
            m_pkSpline->SetControlPoint(iRow,iCol,aakCtrlPoint[iRow][iCol]);
    }

    m_spkCloth->UpdateSurface();
}
//----------------------------------------------------------------------------
