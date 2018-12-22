// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "BSplineLeastSquares.h"

WM3_WINDOW_APPLICATION(BSplineLeastSquares);

//----------------------------------------------------------------------------
BSplineLeastSquares::BSplineLeastSquares ()
    :
    WindowApplication3("BSplineLeastSquares",0,0,512,512,ColorRGB::WHITE)
{
    m_iDegree = 3;
}
//----------------------------------------------------------------------------
bool BSplineLeastSquares::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    // create model
    m_spkScene = new Node(1);
    m_spkTrnNode = new Node(2);
    m_spkScene->AttachChild(m_spkTrnNode);

    // get control points for input curve
    std::ifstream kIStr(System::GetPath("ControlPoints.txt"));
    int iCtrlQuantity = 0;
    kIStr >> iCtrlQuantity;
    Vector3d* akCtrl = new Vector3d[iCtrlQuantity];
    for (int i = 0; i < iCtrlQuantity; i++)
    {
        kIStr >> akCtrl[i].X();
        kIStr >> akCtrl[i].Y();
        kIStr >> akCtrl[i].Z();
    }

    // create polyline connecting control points of input B-spline curve
    double dFraction = 0.10;
    Polyline* pkPoly = OriginalPolyline(iCtrlQuantity,akCtrl);
    m_spkTrnNode->AttachChild(pkPoly);

    // create polyline that approximates the reduced B-spline curve
    pkPoly = ReducedPolyline(iCtrlQuantity,akCtrl,dFraction);
    m_spkTrnNode->AttachChild(pkPoly);
    delete[] akCtrl;

    m_spkScene->UpdateGS(0.0f);
    m_spkTrnNode->Local.SetTranslate(-m_spkScene->WorldBound->GetCenter());

    // sample camera code
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,10000.0f);
    Vector3f kCDir(0.0f,0.0f,1.0f);
    Vector3f kCUp(0.0f,1.0f,0.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    Vector3f kCLoc = -3.0f*m_spkScene->WorldBound->GetRadius()*kCDir;
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS(0.0f);
    m_spkScene->UpdateRS();

    InitializeCameraMotion(10.0f,0.01f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void BSplineLeastSquares::OnTerminate ()
{
    m_spkScene = 0;
    m_spkTrnNode = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void BSplineLeastSquares::OnIdle ()
{
    MeasureTime();
    MoveCamera();

    if ( MoveObject() )
        m_spkScene->UpdateGS(0.0f);

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
Polyline* BSplineLeastSquares::OriginalPolyline (int iCtrlQuantity,
    Vector3d* akCtrl)
{
    BSplineCurve3d kSpline(iCtrlQuantity,akCtrl,m_iDegree,false,true);

    Vector3f* akVertex = new Vector3f[iCtrlQuantity];
    ColorRGB* akColor = new ColorRGB[iCtrlQuantity];
    for (int i = 0; i < iCtrlQuantity; i++)
    {
        double dTime = i/(double)iCtrlQuantity;
        Vector3d kPos = kSpline.GetPosition(dTime);
        akVertex[i].X() = (float)kPos.X();
        akVertex[i].Y() = (float)kPos.Y();
        akVertex[i].Z() = (float)kPos.Z();
        akColor[i] = ColorRGB(1.0f,0.0f,0.0f);
    }

    Vector3fArray* pkVertices = new Vector3fArray(iCtrlQuantity,akVertex);
    ColorRGBArray* pkColors = new ColorRGBArray(iCtrlQuantity,akColor);
    VertexColorEffect* pkEffect = new VertexColorEffect(pkColors);

    Polyline* pkPLine = new Polyline(pkVertices,false,true);
    pkPLine->SetEffect(pkEffect);
    return pkPLine;
}
//----------------------------------------------------------------------------
Polyline* BSplineLeastSquares::ReducedPolyline (int iCtrlQuantity,
    Vector3d* akCtrl, double dFraction)
{
    int iLSCtrlQuantity;
    Vector3d* akLSCtrl;
    BSplineReduction3d(iCtrlQuantity,akCtrl,m_iDegree,dFraction,
        iLSCtrlQuantity,akLSCtrl);

    BSplineCurve3d kSpline(iLSCtrlQuantity,akLSCtrl,m_iDegree,false,true);
    delete[] akLSCtrl;

    Vector3f* akVertex = new Vector3f[iCtrlQuantity];
    ColorRGB* akColor = new ColorRGB[iCtrlQuantity];
    for (int i = 0; i < iCtrlQuantity; i++)
    {
        double dTime = i/(double)iCtrlQuantity;
        Vector3d kPos = kSpline.GetPosition(dTime);
        akVertex[i].X() = (float)kPos.X();
        akVertex[i].Y() = (float)kPos.Y();
        akVertex[i].Z() = (float)kPos.Z();
        akColor[i] = ColorRGB(0.0f,0.0f,1.0f);
    }

    Vector3fArray* pkVertices = new Vector3fArray(iCtrlQuantity,akVertex);
    ColorRGBArray* pkColors = new ColorRGBArray(iCtrlQuantity,akColor);
    VertexColorEffect* pkEffect = new VertexColorEffect(pkColors);

    Polyline* pkPLine = new Polyline(pkVertices,false,true);
    pkPLine->SetEffect(pkEffect);
    return pkPLine;
}
//----------------------------------------------------------------------------
