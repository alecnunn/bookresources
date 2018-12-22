// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "PolyhedronDistance.h"

WM3_WINDOW_APPLICATION(PolyhedronDistance);

//----------------------------------------------------------------------------
PolyhedronDistance::PolyhedronDistance ()
    :
    WindowApplication3("PolyhedronDistance",0,0,640,480,
        ColorRGB(0.5f,0.0f,1.0f))
{
}
//----------------------------------------------------------------------------
bool PolyhedronDistance::OnInitialize()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    // ** layout of scene graph **
    // scene
    //     tetra[4]
    //     plane
    //     line[2]

    // create objects
    m_spkScene = new Node(7);
    TriMesh* pkPlane = CreatePlane();
    int i;
    for (i = 0; i < 2; i++)
    {
        // build the display tetrahedra
        float fSize = 0.3f+0.2f*(i+1);
        if ( i == 0 )
            m_fEdgeLength = fSize;
        m_apkTetra[i] = CreateTetra(fSize,false);
        m_aspkLine[i] = CreateLine();

        // build the point tetrahedra;
        m_fSmall = 0.02f;
        m_apkTetra[i+2] = CreateTetra(m_fSmall,true);
    }

    // convenience for line manipulations
    for(i = 0; i < 2; i++)
        m_apkVertex[i] = m_aspkLine[i]->Vertices->GetData();

    // tetrahedra faces
    m_akFace = new Tuple<3>[4];
    m_akFace[0][0] = 1, m_akFace[0][1] = 2, m_akFace[0][2] = 0;
    m_akFace[1][0] = 0, m_akFace[1][1] = 3, m_akFace[1][2] = 2;
    m_akFace[2][0] = 0, m_akFace[2][1] = 1, m_akFace[2][2] = 3;
    m_akFace[3][0] = 1, m_akFace[3][1] = 2, m_akFace[3][2] = 3;

    InitialState();

    // set parent-child links
    m_spkScene->AttachChild(pkPlane);
    for (i = 0; i < 2; i++)
    {
        m_spkScene->AttachChild(m_apkTetra[i]);
        m_spkScene->AttachChild(m_aspkLine[i]);
        m_spkScene->AttachChild(m_apkTetra[i+2]);
    }

    // wireframe
    m_spkWireframe = new WireframeState;
    m_spkScene->SetGlobalState(m_spkWireframe);

    // initial update of objects
    m_spkScene->UpdateGS(0.0f);
    m_spkScene->UpdateRS();
    PolyhedronDistance::UpdateLine();

    InitializeCameraMotion(0.01f,0.01f);
    return true;
}
//----------------------------------------------------------------------------
void PolyhedronDistance::OnTerminate()
{
    m_spkScene = 0;
    m_spkWireframe = 0;
    m_aspkLine[0] = 0;
    m_aspkLine[1] = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
bool PolyhedronDistance::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if ( WindowApplication3::OnKeyDown(ucKey,iX,iY) )
        return true;

    if ( Transform(ucKey) )
        return true;

    switch ( ucKey )
    {
    case '0':  // restart
        InitialState();
        m_spkScene->UpdateGS(0.0f);
        UpdateLine();
        return true;
    case 'w':  // toggle wireframe
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void PolyhedronDistance::OnIdle ()
{
    MoveCamera();

    m_pkRenderer->ClearBuffers();
    if ( m_pkRenderer->BeginScene() )
    {
        m_pkRenderer->DrawScene(m_spkScene);

        char acMsg[256];
        sprintf(acMsg,"separation = %3.2f",
            m_fSeparation/(Mathf::Sqrt(2.0)*m_fEdgeLength));
        m_pkRenderer->Draw(8,GetHeight()-8,ColorRGBA::WHITE,acMsg);
        sprintf(acMsg," small tetrahedron sides." );
        m_pkRenderer->Draw(140,GetHeight()-8,ColorRGBA::WHITE,acMsg);
        
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();
}
//----------------------------------------------------------------------------
TriMesh* PolyhedronDistance::CreateTetra (float fSize, bool bBlack)
{
    Vector3f* akVertex = new Vector3f[4];
    akVertex[0] = -(fSize/3.0f)*Vector3f(1.0f,1.0f,1.0f);
    akVertex[1] = Vector3f(+fSize,0.0f,0.0f);
    akVertex[2] = Vector3f(0.0f,+fSize,0.0f);
    akVertex[3] = Vector3f(0.0f,0.0f,+fSize);
    Vector3fArray* pkVertices = new Vector3fArray(4,akVertex);

    int* aiIndex = new int[12];
    aiIndex[ 0] = 0; aiIndex[ 1] = 2; aiIndex[ 2] = 1;
    aiIndex[ 3] = 0; aiIndex[ 4] = 3; aiIndex[ 5] = 2;
    aiIndex[ 6] = 0; aiIndex[ 7] = 1; aiIndex[ 8] = 3;
    aiIndex[ 9] = 1; aiIndex[10] = 2; aiIndex[11] = 3;
    IntArray* pkIndices = new IntArray(12,aiIndex);

    TriMesh* pkMesh = new TriMesh(pkVertices,pkIndices,false);

    ColorRGB* akColor = new ColorRGB[4];
    if ( bBlack )
    {
        // black tetrahedra for the small ones used as points.
        akColor[0] = ColorRGB(0.0f,0.0f,0.0f);
        akColor[1] = ColorRGB(0.0f,0.0f,0.0f);
        akColor[2] = ColorRGB(0.0f,0.0f,0.0f);
        akColor[3] = ColorRGB(0.0f,0.0f,0.0f);
    }
    else
    {
        // colorful colors for the tetrahedra under study
        akColor[0] = ColorRGB(0.0f,0.0f,1.0f);
        akColor[1] = ColorRGB(0.0f,1.0f,0.0f);
        akColor[2] = ColorRGB(1.0f,0.0f,0.0f);
        akColor[3] = ColorRGB(1.0f,1.0f,1.0f);
    }

    ColorRGBArray* pkColors = new ColorRGBArray(4,akColor);
    VertexColorEffect* pkEffect = new VertexColorEffect(pkColors);
    pkMesh->SetEffect(pkEffect);

    return pkMesh;
}
//----------------------------------------------------------------------------
Polyline* PolyhedronDistance::CreateLine ()
{
    Vector3f* akVertex = new Vector3f[2];
    akVertex[0] = Vector3f::ZERO;
    akVertex[1] = Vector3f::UNIT_X;
    Vector3fArray* pkVertices = new Vector3fArray(2,akVertex);

    Polyline* pkPLine = new Polyline(pkVertices,false,true);

    ColorRGB* akColor = new ColorRGB[2];
    akColor[0] = ColorRGB::WHITE;
    akColor[1] = ColorRGB::WHITE;
    ColorRGBArray* pkColors = new ColorRGBArray(2,akColor);
    VertexColorEffect* pkEffect = new VertexColorEffect(pkColors);
    pkPLine->SetEffect(pkEffect);

    return pkPLine;
}
//----------------------------------------------------------------------------
void PolyhedronDistance::UpdateLine ()
{
    // two lines make the line easier to see
    Vector3f aakU[2][4];

    // Offset the polyhedra so far into the first octant that we are unlikely
    // to translate them out of that octant during a run.
    m_fOffset = 20.0f;
    Vector3f kTOffset = m_fOffset*Vector3f(1.0f,1.0f,1.0f);
    
    int i;
    for (i = 0; i < 2; i++)
    {
        m_akTV[i] = m_apkTetra[i]->World.GetTranslate();
        m_akTM[i] = m_apkTetra[i]->World.GetRotate();
        Vector3f* akVertex = m_apkTetra[i]->Vertices->GetData();
        for (int j = 0; j < 4; j++)
            aakU[i][j] = m_akTV[i] + kTOffset +  m_akTM[i]*akVertex[j];
    }

    int iStatusCode;

    LCPPolyDist3(4,aakU[0],4,m_akFace,4,aakU[1],4,m_akFace,iStatusCode,
        m_fSeparation,m_apkVertex[0]);

    if ( iStatusCode != LCPPolyDist3::SC_FOUND_SOLUTION &&
         iStatusCode != LCPPolyDist3::SC_TEST_POINTS_TEST_FAILED &&
         iStatusCode != LCPPolyDist3::SC_FOUND_TRIVIAL_SOLUTION ||
         m_fSeparation < 0.0f )
    {
        // Don't draw the line joining nearest points if returns from
        // LCPPolyDist are not appropriate
        for (i = 0; i < 2; i++)
            m_apkVertex[0][i] = -kTOffset;
    }

    // correct for the offset and set up end "points" for the line
    for (i = 0; i < 2; i++)
    {
        m_apkVertex[0][i] = m_apkVertex[0][i]-kTOffset;

        // the adjustment with m_fSmall "centers" the end point tetra
        // on the solution points
        m_apkTetra[i+2]->Local.SetTranslate(m_apkVertex[0][i]
            -(m_fSmall/3.0f)*Vector3f(1.0f,1.0f,1.0f));
    }
    const float fEpsilon = 0.002f; // double-up the line for better visibility
    for (i = 0; i < 2; i++)
    {
        m_apkVertex[1][i] = m_apkVertex[0][i] +
            fEpsilon*Vector3f(1.0f,1.0f,1.0f);
    }

    for (i = 0; i < 2; i++)
    {
        m_aspkLine[i]->UpdateMS(false);
        m_aspkLine[i]->UpdateGS(0.0f);
    }
    m_spkScene->UpdateGS(0.0f);
}
//----------------------------------------------------------------------------
TriMesh* PolyhedronDistance::CreatePlane ()
{
    Vector3f* akVertex = new Vector3f[4];
    float fSize = 16.0f;
    akVertex[0] = Vector3f(-fSize,-fSize,-0.1f);
    akVertex[1] = Vector3f(+fSize,-fSize,-0.1f);
    akVertex[2] = Vector3f(+fSize,+fSize,-0.1f);
    akVertex[3] = Vector3f(-fSize,+fSize,-0.1f);
    Vector3fArray* pkVertices = new Vector3fArray(4,akVertex);

    int* aiIndex = new int[6];
    aiIndex[0] = 0; aiIndex[1] = 1; aiIndex[2] = 2;
    aiIndex[3] = 0; aiIndex[4] = 2; aiIndex[5] = 3;
    IntArray* pkIndices = new IntArray(6,aiIndex);

    TriMesh* pkMesh = new TriMesh(pkVertices,pkIndices,false);

    ColorRGB* akColor = new ColorRGB[4];
    akColor[0] = ColorRGB(0.0f,0.50f,0.00f);
    akColor[1] = ColorRGB(0.0f,0.25f,0.00f);
    akColor[2] = ColorRGB(0.0f,0.75f,0.00f);
    akColor[3] = ColorRGB(0.0f,1.0f,0.00f);
    ColorRGBArray* pkColors = new ColorRGBArray(4,akColor);
    VertexColorEffect* pkEffect = new VertexColorEffect(pkColors);
    pkMesh->SetEffect(pkEffect);

    return pkMesh;
}
//----------------------------------------------------------------------------
void PolyhedronDistance::InitialState ()
{
    // set up camera
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1000.0f);
    Vector3f kCLoc(0.0f,-2.0f,0.5f);
    Vector3f kCDir(0.0f,1.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // transform objects
    m_apkTetra[0]->Local.SetRotate(
        Matrix3f().FromAxisAngle(Vector3f::UNIT_Z,1.1f));
    m_apkTetra[0]->Local.SetTranslate(Vector3f(-0.25f,0.1f,0.3f));
    m_apkTetra[1]->Local.SetRotate(
        Matrix3f().FromAxisAngle(Vector3f::UNIT_Z,0.3f));
    m_apkTetra[1]->Local.SetTranslate(Vector3f(0.25f,0.4f,0.5f));
}
//----------------------------------------------------------------------------
bool PolyhedronDistance::Transform (unsigned char ucKey)
{
    Vector3f kTrn;
    Matrix3f kRot, kIncr;
    float fTrnSpeed = 0.1f;
    float fRotSpeed = 0.1f;

    switch ( ucKey )
    {
        case 'x':
            kTrn = m_apkTetra[0]->Local.GetTranslate();
            kTrn.X() -= fTrnSpeed;
            m_apkTetra[0]->Local.SetTranslate(kTrn);
            break;
        case 'X':
            kTrn = m_apkTetra[0]->Local.GetTranslate();
            kTrn.X() += fTrnSpeed;
            m_apkTetra[0]->Local.SetTranslate(kTrn);
            break;
        case 'y':
            kTrn = m_apkTetra[0]->Local.GetTranslate();
            kTrn.Y() -= fTrnSpeed;
            m_apkTetra[0]->Local.SetTranslate(kTrn);
            break;
        case 'Y':
            kTrn = m_apkTetra[0]->Local.GetTranslate();
            kTrn.Y() += fTrnSpeed;
            m_apkTetra[0]->Local.SetTranslate(kTrn);
            break;
        case 'z':
            kTrn = m_apkTetra[0]->Local.GetTranslate();
            kTrn.Z() -= fTrnSpeed;
            m_apkTetra[0]->Local.SetTranslate(kTrn);
            break;
        case 'Z':
            kTrn = m_apkTetra[0]->Local.GetTranslate();
            kTrn.Z() += fTrnSpeed;
            m_apkTetra[0]->Local.SetTranslate(kTrn);
            break;
        case 's':
            kTrn = m_apkTetra[1]->Local.GetTranslate();
            kTrn.X() -= fTrnSpeed;
            m_apkTetra[0]->Local.SetTranslate(kTrn);
            break;
        case 'S':
            kTrn = m_apkTetra[1]->Local.GetTranslate();
            kTrn.X() += fTrnSpeed;
            m_apkTetra[0]->Local.SetTranslate(kTrn);
            break;
        case 't':
            kTrn = m_apkTetra[1]->Local.GetTranslate();
            kTrn.Y() -= fTrnSpeed;
            m_apkTetra[0]->Local.SetTranslate(kTrn);
            break;
        case 'T':
            kTrn = m_apkTetra[1]->Local.GetTranslate();
            kTrn.Y() += fTrnSpeed;
            m_apkTetra[0]->Local.SetTranslate(kTrn);
            break;
        case 'u':
            kTrn = m_apkTetra[1]->Local.GetTranslate();
            kTrn.Z() -= fTrnSpeed;
            m_apkTetra[0]->Local.SetTranslate(kTrn);
            break;
        case 'U':
            kTrn = m_apkTetra[1]->Local.GetTranslate();
            kTrn.Z() += fTrnSpeed;
            m_apkTetra[0]->Local.SetTranslate(kTrn);
            break;
        case 'a':
            kRot = m_apkTetra[0]->Local.GetRotate();
            kIncr.FromAxisAngle(Vector3f::UNIT_Y,fRotSpeed);
            m_apkTetra[0]->Local.SetRotate(kIncr*kRot);
            break;
        case 'A':
            kRot = m_apkTetra[0]->Local.GetRotate();
            kIncr.FromAxisAngle(Vector3f::UNIT_Y,-fRotSpeed);
            m_apkTetra[0]->Local.SetRotate(kIncr*kRot);
            break;
        case 'b':
            kRot = m_apkTetra[1]->Local.GetRotate();
            kIncr.FromAxisAngle(Vector3f::UNIT_Z,fRotSpeed);
            m_apkTetra[1]->Local.SetRotate(kIncr*kRot);
            break;
        case 'B':
            kRot = m_apkTetra[1]->Local.GetRotate();
            kIncr.FromAxisAngle(Vector3f::UNIT_X,-fRotSpeed);
            m_apkTetra[1]->Local.SetRotate(kIncr*kRot);
            break;
        case 'c':
            kRot = m_apkTetra[0]->Local.GetRotate();
            kIncr.FromAxisAngle(Vector3f::UNIT_X,1.3f*fRotSpeed);
            m_apkTetra[0]->Local.SetRotate(kIncr*kRot);
            kRot = m_apkTetra[1]->Local.GetRotate();
            kIncr.FromAxisAngle(Vector3f::UNIT_Z,-fRotSpeed);
            m_apkTetra[1]->Local.SetRotate(kIncr*kRot);
            break;
        case 'C':
            kRot = m_apkTetra[0]->Local.GetRotate();
            kIncr.FromAxisAngle(Vector3f::UNIT_X,-1.3f*fRotSpeed);
            m_apkTetra[0]->Local.SetRotate(kIncr*kRot);
            kRot = m_apkTetra[1]->Local.GetRotate();
            kIncr.FromAxisAngle(Vector3f::UNIT_Z,fRotSpeed);
            m_apkTetra[1]->Local.SetRotate(kIncr*kRot);
            break;
        default:
            return false;
    }

    // Prevent solution point coordinates from being < 0.
    // The polyhedron distance calculator expects solution points
    // to be in the first octant. Vertices are offset by
    // m_fOffset*Vector3f(1,1,1) in UpdateLine() before the call to the 
    // distance routine.
    // Here we make sure that no translation of a polyhedron takes it so
    // far into one of the other 7 octants that the offset will not be
    // sufficient to guarantee that the solution points lie in the first 
    // octant.
    float fThreshold = -m_fOffset+fTrnSpeed;
    for (int j = 0; j < 2; j++)
    {
        Vector3f kTV = m_apkTetra[j]->World.GetTranslate();
        Matrix3f kTM = m_apkTetra[j]->World.GetRotate();
        Vector3f* akVertex = m_apkTetra[j]->Vertices->GetData();
        for (int i = 0; i < 4; i++)
        {
            Vector3f kTemp = kTV+kTM*akVertex[i], kTrn;
            for (int k = 0; k < 3; k++)
            {
                if ( kTemp[k] < fThreshold )
                {
                    kTrn = m_apkTetra[j]->Local.GetTranslate();
                    kTrn[k] += fTrnSpeed;
                    m_apkTetra[j]->Local.SetTranslate(kTrn);
                }
            }
        }
    }

    m_spkScene->UpdateGS(0.0f);
    UpdateLine();
    return true;
}
//----------------------------------------------------------------------------
