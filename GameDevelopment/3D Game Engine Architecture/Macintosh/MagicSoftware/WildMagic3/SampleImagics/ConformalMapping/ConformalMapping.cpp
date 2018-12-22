// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "ConformalMapping.h"

WM3_WINDOW_APPLICATION(ConformalMapping);

//----------------------------------------------------------------------------
ConformalMapping::ConformalMapping ()
    :
    WindowApplication3("ConformalMapping",0,0,640,480,
        ColorRGB(0.5f,0.0f,1.0f))
{
}
//----------------------------------------------------------------------------
bool ConformalMapping::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    // set up camera
    m_spkCamera->SetFrustum(-0.055f,0.055f,-0.04125f,0.04125f,0.1f,100.0f);
    Vector3f kCLoc(0.0f,0.0f,-6.5f);
    Vector3f kCDir(0.0f,0.0f,1.0f);
    Vector3f kCUp(0.0f,1.0f,0.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // load brain mesh
    Stream kInfile;
    kInfile.Load(System::GetPath("../../Data/Wmof/Brain.wmof"));
    if ( kInfile.GetObjectCount() == 0 )
        return true;

    m_spkMesh = DynamicCast<TriMesh>(kInfile.GetObjectAt(0));
    if ( !m_spkMesh )
    {
        // Object must be a TriMesh for this application.  Moreover, the
        // TriMesh must be a closed mesh that is topologically equivalent
        // to a sphere.
        return true;
    }

    // use pseudocoloring based on mean curvature
    PseudocolorVertices();

    // Numerical preconditioning for computing the conformal map.  The choice
    // of fExtreme is based on knowledge of the size of the vertices in the
    // input mesh m_spkMesh.
    float fExtreme = 10.0f;
    ScaleToCube(fExtreme);

    DoMapping();

    // set up scene graph
    m_spkScene = new Node(2);
    m_spkWireframe = new WireframeState;
    m_spkScene->SetGlobalState(m_spkWireframe);

    // subtree for mesh
    m_spkMeshTree = new Node;
    m_spkMeshTree->Local.SetTranslate(Vector3f(2.0f,0.0f,0.0f));
    m_spkMeshTree->Local.SetUniformScale(1.0f/fExtreme);
    m_spkScene->AttachChild(m_spkMeshTree);
    Node* pkMeshParent = new Node;
    m_spkMeshTree->AttachChild(pkMeshParent);
    pkMeshParent->AttachChild(m_spkMesh);
    pkMeshParent->Local.SetTranslate(-m_spkMesh->ModelBound->GetCenter());

    // subtree for sphere
    m_spkSphereTree = new Node;
    m_spkSphereTree->Local.SetTranslate(Vector3f(-2.0f,0.0f,0.0f));
    m_spkScene->AttachChild(m_spkSphereTree);
    Node* pkSphereParent = new Node;
    m_spkSphereTree->AttachChild(pkSphereParent);
    pkSphereParent->AttachChild(m_spkSphere);
    pkSphereParent->Local.SetTranslate(-m_spkSphere->ModelBound->GetCenter());

    // initial update of objects
    m_spkScene->UpdateGS(0.0f);
    m_spkScene->UpdateRS();

    InitializeCameraMotion(0.01f,0.01f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void ConformalMapping::OnTerminate ()
{
    m_spkScene = 0;
    m_spkWireframe = 0;
    m_spkMeshTree = 0;
    m_spkSphereTree = 0;
    m_spkMesh = 0;
    m_spkSphere = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void ConformalMapping::OnIdle ()
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
bool ConformalMapping::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if ( WindowApplication3::OnKeyDown(ucKey,iX,iY) )
        return true;

    switch ( ucKey )
    {
    case 'w':
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;
    case 'm':
        m_spkMotionObject = m_spkMeshTree;
        return true;
    case 's':
        m_spkMotionObject = m_spkSphereTree;
        return true;
    case 'r':
        m_spkMotionObject = m_spkScene;
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void ConformalMapping::ScaleToCube (float fExtreme)
{
    // Uniformly scale the cube to [-extreme,extreme]^3 for numerical
    // preconditioning for the conformal mapping.

    int iVQuantity = m_spkMesh->Vertices->GetQuantity();
    Vector3f* akPoint = m_spkMesh->Vertices->GetData();

    float fMin = akPoint[0].X(), fMax = fMin;
    int i;
    for (i = 0; i < iVQuantity; i++)
    {
        if ( akPoint[i].X() < fMin )
            fMin = akPoint[i].X();
        else if ( akPoint[i].X() > fMax )
            fMax = akPoint[i].X();

        if ( akPoint[i].Y() < fMin )
            fMin = akPoint[i].Y();
        else if ( akPoint[i].Y() > fMax )
            fMax = akPoint[i].Y();

        if ( akPoint[i].Z() < fMin )
            fMin = akPoint[i].Z();
        else if ( akPoint[i].Z() > fMax )
            fMax = akPoint[i].Z();
    }
    float fHalfRange = 0.5f*(fMax - fMin);
    float fMult = fExtreme/fHalfRange;

    Vector3f kOne(1.0f,1.0f,1.0f);
    for (i = 0; i < iVQuantity; i++)
        akPoint[i] = -fExtreme*kOne + fMult*(akPoint[i] - fMin*kOne);

    m_spkMesh->UpdateMS(false);
}
//----------------------------------------------------------------------------
void ConformalMapping::PseudocolorVertices ()
{
    // color the vertices according to mean curvature

    int iVQuantity = m_spkMesh->Vertices->GetQuantity();
    const Vector3f* akPoint = m_spkMesh->Vertices->GetData();
    int iTQuantity = m_spkMesh->Indices->GetQuantity()/3;
    const int* aiIndex = m_spkMesh->Indices->GetData();

    ColorRGB* akColor = new ColorRGB[iVQuantity];
    ColorRGBArray* pkColors = new ColorRGBArray(iVQuantity,akColor);
    VertexColorEffect* pkEffect = new VertexColorEffect(pkColors);
    m_spkMesh->SetEffect(pkEffect);

    MeshCurvaturef kMC(iVQuantity,akPoint,iTQuantity,aiIndex);
    const float* afMinCurv = kMC.GetMinCurvatures();
    const float* afMaxCurv = kMC.GetMaxCurvatures();
    float fMinMeanCurvature = afMinCurv[0] + afMaxCurv[0];
    float fMaxMeanCurvature = fMinMeanCurvature;
    float* afMeanCurvature = new float[iVQuantity];
    int i;
    for (i = 0; i < iVQuantity; i++)
    {
        afMeanCurvature[i] = afMinCurv[i] + afMaxCurv[i];
        if ( afMeanCurvature[i] < fMinMeanCurvature )
            fMinMeanCurvature = afMeanCurvature[i];
        else if ( afMeanCurvature[i] > fMaxMeanCurvature )
            fMaxMeanCurvature = afMeanCurvature[i];
    }

    for (i = 0; i < iVQuantity; i++)
    {
        if ( afMeanCurvature[i] > 0.0f )
        {
            akColor[i].R() =
                0.5f*(1.0f + afMeanCurvature[i]/fMaxMeanCurvature);
            akColor[i].G() = akColor[i].R();
            akColor[i].B() = 0.0f;
        }
        else if ( afMeanCurvature[i] < 0.0f )
        {
            akColor[i].R() = 0.0f;
            akColor[i].G() = 0.0f;
            akColor[i].B() =
                0.5f*(1.0f - afMeanCurvature[i]/fMinMeanCurvature);
        }
        else
        {
            akColor[i] = ColorRGB::BLACK;
        }
    }

    delete[] afMeanCurvature;
}
//----------------------------------------------------------------------------
void ConformalMapping::DoMapping ()
{
    int iVQ = m_spkMesh->Vertices->GetQuantity();
    Vector3f* akVertex = m_spkMesh->Vertices->GetData();
    int iTQ = m_spkMesh->Indices->GetQuantity()/3;
    const int* aiIndex = m_spkMesh->Indices->GetData();

    // color the punctured triangle red
    ColorRGB* akColor = m_spkMesh->GetEffect()->ColorRGBs->GetData();
    akColor[aiIndex[0]] = ColorRGB(1.0f,0.0f,0.0f);
    akColor[aiIndex[1]] = ColorRGB(1.0f,0.0f,0.0f);
    akColor[aiIndex[2]] = ColorRGB(1.0f,0.0f,0.0f);

    // conformally map mesh to plane, sphere, and cylinder
    ConformalMapf kCMap(iVQ,akVertex,iTQ,aiIndex);

    // create a representation of the conformal sphere
    Vector3f* akSVertex = new Vector3f[iVQ];
    memcpy(akSVertex,kCMap.GetSphereCoordinates(),iVQ*sizeof(Vector3f));
    Vector3fArray* pkVertices = new Vector3fArray(iVQ,akSVertex);

    m_spkSphere = new TriMesh(pkVertices,m_spkMesh->Indices,false);
    m_spkSphere->SetEffect(m_spkMesh->GetEffect());
}
//----------------------------------------------------------------------------
