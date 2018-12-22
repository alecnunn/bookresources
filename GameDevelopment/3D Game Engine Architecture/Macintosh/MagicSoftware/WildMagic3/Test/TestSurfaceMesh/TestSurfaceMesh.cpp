// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "TestSurfaceMesh.h"

WM3_WINDOW_APPLICATION(TestSurfaceMesh);

//----------------------------------------------------------------------------
TestSurfaceMesh::TestSurfaceMesh ()
    :
    WindowApplication3("TestSurfaceMesh",0,0,640,480,ColorRGB(0.5f,0.0f,1.0f))
{
}
//----------------------------------------------------------------------------
bool TestSurfaceMesh::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    CreateScene();

    // set up camera
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,100.0f);
    Vector3f kCLoc(0.0f,0.0f,12.0f);
    Vector3f kCDir(0.0f,0.0f,-1.0f);
    Vector3f kCUp(0.0f,1.0f,0.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    InitializeCameraMotion(0.01f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void TestSurfaceMesh::OnTerminate ()
{
    m_spkScene = 0;
    m_spkWireframe = 0;
    m_spkSegment = 0;
    m_spkCurve = 0;
    m_spkPatch = 0;
    m_spkSurface = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void TestSurfaceMesh::OnIdle ()
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
bool TestSurfaceMesh::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if ( WindowApplication3::OnKeyDown(ucKey,iX,iY) )
        return true;

    switch ( ucKey )
    {
    case 'w':
    case 'W':
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;
    case '+':
    case '=':
        if ( m_iLevel < 5 )
        {
            m_iLevel++;

            int iOrigQuantity, iVQuantity;
            const float* afOrigColors;
            ColorRGB* akColor;
            Effect* pkEffect;

            // Increase the number of curve vertices.  The vertex colors
            // must be computed for the new vertices.  The vertex tree
            // stored in the curve object handles the interpolation.
            m_spkCurve->SetLevel(m_iLevel);
            pkEffect = m_spkCurve->GetEffect();
            iOrigQuantity = pkEffect->ColorRGBs->GetQuantity();
            afOrigColors = (const float*)pkEffect->ColorRGBs->GetData();
            akColor = (ColorRGB*) m_spkCurve->GetAttributes(3,afOrigColors);
            iVQuantity = m_spkCurve->Vertices->GetQuantity();
            pkEffect->ColorRGBs = new ColorRGBArray(iVQuantity,akColor);

            // Increase the number of surface vertices.  The vertex colors
            // must be computed for the new vertices.  The vertex tree
            // stored in the curve object handles the interpolation.
            m_spkSurface->SetLevel(m_iLevel);
            pkEffect = m_spkSurface->GetEffect();
            iOrigQuantity = pkEffect->ColorRGBs->GetQuantity();
            afOrigColors = (const float*)pkEffect->ColorRGBs->GetData();
            akColor = (ColorRGB*) m_spkSurface->GetAttributes(3,afOrigColors);
            iVQuantity = m_spkSurface->Vertices->GetQuantity();
            pkEffect->ColorRGBs = new ColorRGBArray(iVQuantity,akColor);

            m_spkSurface->UpdateGS();
        }
        return true;
    case '-':
    case '_':
        if ( m_iLevel > 0 )
        {
            m_iLevel--;

            // Decrease the number of curve vertices.  The vertex color
            // array in the effect has more elements than the new vertex
            // quantity, so it does no harm not to reallocate the colors.
            // If you want to reallocate, mimic the code that is in the
            // "increase" handler.
            m_spkCurve->SetLevel(m_iLevel);

            // Decrease the number of surface vertices.  The vertex color
            // array in the effect has more elements than the new vertex
            // quantity, so it does no harm not to reallocate the colors.
            // If you want to reallocate, mimic the code that is in the
            // "increase" handler.
            m_spkSurface->SetLevel(m_iLevel);

            m_spkSurface->UpdateGS();
        }
        return true;
    case 'l':
    case 'L':
        m_spkCurve->Lock();
        m_spkSurface->Lock();
        return true;
    case 'a':
        m_spkPatch->Amplitude -= 0.1f;
        m_spkSurface->OnDynamicChange();
        m_spkSurface->UpdateGS();
        return true;
    case 'A':
        m_spkPatch->Amplitude += 0.1f;
        m_spkSurface->OnDynamicChange();
        m_spkSurface->UpdateGS();
        return true;
    case 's':
    case 'S':
        TestStream();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void TestSurfaceMesh::CreateScene ()
{
    m_spkScene = new Node;
    m_spkWireframe = new WireframeState;
    m_spkWireframe->Enabled = true;
    m_spkScene->SetGlobalState(m_spkWireframe);

    CullState* pkCS = new CullState;
    pkCS->Enabled = false;
    m_spkScene->SetGlobalState(pkCS);

    CreateSimpleSegment();
    CreateSimplePatch();

    // start with level zero on both segment and patch
    m_iLevel = 0;
}
//----------------------------------------------------------------------------
void TestSurfaceMesh::CreateSimpleSegment ()
{
    m_spkSegment = new SimpleSegment;

    // The simple segment is used for all curve segments.
    int i, iSegmentQuantity = 3;
    CurveSegmentPtr* aspkSegment = new CurveSegmentPtr[iSegmentQuantity];
    for (i = 0; i < iSegmentQuantity; i++)
        aspkSegment[i] = m_spkSegment;

    // Each edge has its own parameters associated with its vertices.
    // A vertex can have different parameters for the edges sharing it.
    float* afParam = new float[2*iSegmentQuantity];
    afParam[0] = 0.0f;
    afParam[1] = 1.0f/3.0f;
    afParam[2] = afParam[1];
    afParam[3] = 2.0f/3.0f;
    afParam[4] = afParam[3];
    afParam[5] = 1.0f;
    FloatArray* pkParams = new FloatArray(2*iSegmentQuantity,afParam);

    // create a curve mesh
    m_spkCurve = new CurveMesh(iSegmentQuantity,aspkSegment,pkParams,true,
        true);

    // use vertex colors
    int iVQuantity = m_spkCurve->Vertices->GetQuantity();
    ColorRGB* akColor = new ColorRGB[iVQuantity];
    for (i = 0; i < iVQuantity; i++)
    {
        akColor[i].R() = Mathf::UnitRandom();
        akColor[i].G() = Mathf::UnitRandom();
        akColor[i].B() = Mathf::UnitRandom();
    }
    ColorRGBArray* pkColors = new ColorRGBArray(iVQuantity,akColor);
    VertexColorEffect* pkEffect = new VertexColorEffect(pkColors);
    m_spkCurve->SetEffect(pkEffect);

    m_spkScene->AttachChild(m_spkCurve);
}
//----------------------------------------------------------------------------
void TestSurfaceMesh::CreateSimplePatch ()
{
    m_spkPatch = new SimplePatch;

    // parameters for the initial vertex construction
    int iVQuantity = 5;
    Vector2f* akVParam = new Vector2f[iVQuantity];
    akVParam[0] = Vector2f(-1.0f,-1.0f);
    akVParam[1] = Vector2f(1.0f,-1.0f);
    akVParam[2] = Vector2f(1.0f,1.0f);
    akVParam[3] = Vector2f(-1.0f,1.0f);
    akVParam[4] = Vector2f(2.0f,0.0f);

    // vertices
    Vector3f* akVertex = new Vector3f[iVQuantity];
    int i;
    for (i = 0; i < iVQuantity; i++)
        akVertex[i] = m_spkPatch->P(akVParam[i].X(),akVParam[i].Y());
    Vector3fArray* pkVertices = new Vector3fArray(iVQuantity,akVertex);

    // triangles
    int iTQuantity = 3, iIQuantity = 3*iTQuantity;
    int* aiIndex = new int[iIQuantity];
    aiIndex[0] = 0;  aiIndex[1] = 1;  aiIndex[2] = 2;
    aiIndex[3] = 0;  aiIndex[4] = 2;  aiIndex[5] = 3;
    aiIndex[6] = 1;  aiIndex[7] = 4;  aiIndex[8] = 2;
    IntArray* pkIndices = new IntArray(iIQuantity,aiIndex);

    // Each triangle has its own parameters associated with its vertices.
    // A vertex can have different parameters for the triangles sharing it.
    Vector2f* akParam = new Vector2f[iIQuantity];
    for (i = 0; i < iIQuantity; i++)
        akParam[i] = akVParam[aiIndex[i]];
    Vector2fArray* pkParams = new Vector2fArray(iVQuantity,akParam);
    delete[] akVParam;

    // the simple patch is used for all triangles
    SurfacePatchPtr* aspkPatch = new SurfacePatchPtr[iTQuantity];
    for (i = 0; i < iTQuantity; i++)
        aspkPatch[i] = m_spkPatch;

    // start with level 0 (original triangle mesh)
    m_iLevel = 0;
    m_spkSurface = new SurfaceMesh(pkVertices,pkIndices,false,pkParams,
        aspkPatch,true,true);

    // use vertex colors
    iVQuantity = m_spkSurface->Vertices->GetQuantity();
    ColorRGB* akColor = new ColorRGB[iVQuantity];
    for (i = 0; i < iVQuantity; i++)
    {
        akColor[i].R() = Mathf::UnitRandom();
        akColor[i].G() = Mathf::UnitRandom();
        akColor[i].B() = Mathf::UnitRandom();
    }
    ColorRGBArray* pkColors = new ColorRGBArray(iVQuantity,akColor);
    VertexColorEffect* pkEffect = new VertexColorEffect(pkColors);
    m_spkSurface->SetEffect(pkEffect);

    m_spkScene->AttachChild(m_spkSurface);
}
//----------------------------------------------------------------------------
void TestSurfaceMesh::TestStream ()
{
    Stream kOStream;
    kOStream.Insert(m_spkScene);
    kOStream.Save("TestSurfaceMesh.wmof");

    Stream kIStream;
    kIStream.Load("TestSurfaceMesh.wmof");
    NodePtr spkScene = (Node*)kIStream.GetObjectAt(0);
}
//----------------------------------------------------------------------------
