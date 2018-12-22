// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "IntersectBoxes.h"

WM3_WINDOW_APPLICATION(IntersectBoxes);

float IntersectBoxes::ms_fSize = 256.0f;

// #define SINGLE_STEP

//----------------------------------------------------------------------------
IntersectBoxes::IntersectBoxes ()
    :
    WindowApplication3("IntersectingBoxes",0,0,640,480,
        ColorRGB(0.75f,0.75f,0.75f))
{
    m_pkIB = 0;
    m_bDoSimulation = true;
    m_fLastIdle = 0.0f;
}
//----------------------------------------------------------------------------
bool IntersectBoxes::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    // create some axis-aligned boxes for intersection testing
    const int iMax = 16;
    int i;
    for (i = 0; i < iMax; i++)
    {
        float fXMin = 0.5f*ms_fSize*Mathf::SymmetricRandom();
        float fXMax = fXMin + Mathf::IntervalRandom(8.0f,32.0f);
        float fYMin = 0.5f*ms_fSize*Mathf::SymmetricRandom();
        float fYMax = fYMin + Mathf::IntervalRandom(8.0f,32.0f);
        float fZMin = 0.5f*ms_fSize*Mathf::SymmetricRandom();
        float fZMax = fZMin + Mathf::IntervalRandom(8.0f,32.0f);
        m_kBoxes.push_back(
            AxisAlignedBox3f(fXMin,fXMax,fYMin,fYMax,fZMin,fZMax));
    }
    m_pkIB = new IntersectingBoxesf(m_kBoxes);

    // scene graph for the visual representation of the boxes
    m_spkScene = new Node(iMax);
    m_spkWireframe = new WireframeState;
    m_spkScene->SetGlobalState(m_spkWireframe);

    // materials for boxes, blue for nonintersecting and red for intersecting
    m_spkBlue = new MaterialState;
    m_spkBlue->Emissive = ColorRGBA::BLACK;
    m_spkBlue->Ambient = ColorRGBA(0.25f,0.25f,0.25f,1.0f);
    m_spkBlue->Diffuse = ColorRGBA(0.0f,0.0f,0.5f,1.0f);
    m_spkBlue->Specular = ColorRGBA::BLACK;
    m_spkBlue->Shininess = 1.0f;

    m_spkRed = new MaterialState;
    m_spkRed->Emissive = ColorRGBA::BLACK;
    m_spkRed->Ambient = ColorRGBA(0.25f,0.25f,0.25f,1.0f);
    m_spkRed->Diffuse = ColorRGBA(0.5f,0.0f,0.0f,1.0f);
    m_spkRed->Specular = ColorRGBA::BLACK;
    m_spkRed->Shininess = 1.0f;

    // a light for the scene
    Light* pkLight = new Light(Light::LT_DIRECTIONAL);
    pkLight->Ambient = ColorRGBA::WHITE;
    pkLight->Diffuse = ColorRGBA::WHITE;
    pkLight->Specular = ColorRGBA::BLACK;
    pkLight->SetDirection(Vector3f::UNIT_Z);
    m_spkScene->SetLight(pkLight);

    // create visual representations of the boxes
    for (i = 0; i < iMax; i++)
    {
        Vector3f kCenter(
            0.5f*(m_kBoxes[i].Min[0]+m_kBoxes[i].Max[0]),
            0.5f*(m_kBoxes[i].Min[1]+m_kBoxes[i].Max[1]),
            0.5f*(m_kBoxes[i].Min[2]+m_kBoxes[i].Max[2]));

        Transformation kXFrm;
        kXFrm.SetTranslate(kCenter);

        float fXExtent = 0.5f*(m_kBoxes[i].Max[0]-m_kBoxes[i].Min[0]);
        float fYExtent = 0.5f*(m_kBoxes[i].Max[1]-m_kBoxes[i].Min[1]);
        float fZExtent = 0.5f*(m_kBoxes[i].Max[2]-m_kBoxes[i].Min[2]);

        StandardMesh kSM;
        kSM.Normals(true,false).Transform(kXFrm);
        TriMesh* pkMesh = kSM.Box(fXExtent,fYExtent,fZExtent);

        pkMesh->SetGlobalState(m_spkBlue);
        m_spkScene->AttachChild(pkMesh);
    }

    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1000.0f);
    Vector3f kCLoc(0.0f,0.0f,-ms_fSize);
    Vector3f kCDir(0.0f,0.0f,1.0f);
    Vector3f kCUp(0.0f,1.0f,0.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS(0.0f);
    m_spkScene->UpdateRS();

    InitializeCameraMotion(0.5f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void IntersectBoxes::OnTerminate ()
{
    delete m_pkIB;
    m_spkScene = 0;
    m_spkWireframe = 0;
    m_spkBlue = 0;
    m_spkRed = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void IntersectBoxes::OnIdle ()
{
    MeasureTime();

    MoveCamera();
    if ( MoveObject() )
        m_spkScene->UpdateGS(0.0f);

#ifndef SINGLE_STEP
    if ( m_bDoSimulation )
    {
        float fCurrIdle = (float)System::GetTime();
        float fDiff = fCurrIdle - m_fLastIdle;
        if ( fDiff >= 1.0f/30.0f )
        {
            ModifyBoxes();
            m_fLastIdle = fCurrIdle;
        }
    }
#endif

    m_pkRenderer->ClearBuffers();
    if ( m_pkRenderer->BeginScene() )
    {
        m_pkRenderer->DrawScene(m_spkScene);
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool IntersectBoxes::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if ( WindowApplication3::OnKeyDown(ucKey,iX,iY) )
        return true;

    switch ( ucKey )
    {
    case 'w':
    case 'W':
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;
    case 's':
    case 'S':
        m_bDoSimulation = !m_bDoSimulation;
        return true;
#ifdef SINGLE_STEP
    case 'g':
    case 'G':
        ModifyBoxes();
        return true;
#endif
    }

    return false;
}
//----------------------------------------------------------------------------
void IntersectBoxes::ModifyBoxes ()
{
    int i;
    for (i = 0; i < (int)m_kBoxes.size(); i++)
    {
        AxisAlignedBox3f kBox = m_kBoxes[i];

        float fDX = Mathf::IntervalRandom(-4.0f,4.0f);
        if ( kBox.Min[0]+fDX >= -ms_fSize && kBox.Max[0]+fDX <= ms_fSize )
        {
            kBox.Min[0] += fDX;
            kBox.Max[0] += fDX;
        }

        float fDY = Mathf::IntervalRandom(-4.0f,4.0f);
        if ( kBox.Min[1]+fDY >= -ms_fSize && kBox.Max[1]+fDY <= ms_fSize )
        {
            kBox.Min[1] += fDY;
            kBox.Max[1] += fDY;
        }

        float fDZ = Mathf::IntervalRandom(-4.0f,4.0f);
        if ( kBox.Min[2]+fDZ >= -ms_fSize && kBox.Max[2]+fDZ <= ms_fSize )
        {
            kBox.Min[2] += fDZ;
            kBox.Max[2] += fDZ;
        }

        m_pkIB->SetBox(i,kBox);
        ModifyMesh(i);
    }

    m_pkIB->Update();
    m_spkScene->UpdateGS(0.0f);

    // switch material to red for any box that overlaps another
    TriMesh* pkMesh;
    for (i = 0; i < (int)m_kBoxes.size(); i++)
    {
        // reset all boxes to blue
        pkMesh = StaticCast<TriMesh>(m_spkScene->GetChild(i));
        pkMesh->SetGlobalState(m_spkBlue);
    }

    const std::set<EdgeKey>& rkOverlap = m_pkIB->GetOverlap();
    std::set<EdgeKey>::const_iterator pkIter = rkOverlap.begin();
    for (/**/; pkIter != rkOverlap.end(); pkIter++)
    {
        // set intersecting boxes to red
        i = pkIter->V[0];
        pkMesh = StaticCast<TriMesh>(m_spkScene->GetChild(i));
        pkMesh->SetGlobalState(m_spkRed);
        i = pkIter->V[1];
        pkMesh = StaticCast<TriMesh>(m_spkScene->GetChild(i));
        pkMesh->SetGlobalState(m_spkRed);
    }

    m_spkScene->UpdateRS();
}
//----------------------------------------------------------------------------
void IntersectBoxes::ModifyMesh (int i)
{
    Vector3f kCenter(
        0.5f*(m_kBoxes[i].Min[0]+m_kBoxes[i].Max[0]),
        0.5f*(m_kBoxes[i].Min[1]+m_kBoxes[i].Max[1]),
        0.5f*(m_kBoxes[i].Min[2]+m_kBoxes[i].Max[2]));

    float fXExtent = 0.5f*(m_kBoxes[i].Max[0]-m_kBoxes[i].Min[0]);
    float fYExtent = 0.5f*(m_kBoxes[i].Max[1]-m_kBoxes[i].Min[1]);
    float fZExtent = 0.5f*(m_kBoxes[i].Max[2]-m_kBoxes[i].Min[2]);

    Vector3f kXTerm = fXExtent*Vector3f::UNIT_X;
    Vector3f kYTerm = fYExtent*Vector3f::UNIT_Y;
    Vector3f kZTerm = fZExtent*Vector3f::UNIT_Z;

    TriMesh* pkMesh = StaticCast<TriMesh>(m_spkScene->GetChild(i));
    Vector3f* akVertex = pkMesh->Vertices->GetData();

    akVertex[0] = kCenter - kXTerm - kYTerm - kZTerm;
    akVertex[1] = kCenter + kXTerm - kYTerm - kZTerm;
    akVertex[2] = kCenter + kXTerm + kYTerm - kZTerm;
    akVertex[3] = kCenter - kXTerm + kYTerm - kZTerm;
    akVertex[4] = kCenter - kXTerm - kYTerm + kZTerm;
    akVertex[5] = kCenter + kXTerm - kYTerm + kZTerm;
    akVertex[6] = kCenter + kXTerm + kYTerm + kZTerm;
    akVertex[7] = kCenter - kXTerm + kYTerm + kZTerm;

    pkMesh->UpdateMS(true);
}
//----------------------------------------------------------------------------
