// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "FreeFormDeformation.h"

WM3_WINDOW_APPLICATION(FreeFormDeformation);

//----------------------------------------------------------------------------
FreeFormDeformation::FreeFormDeformation ()
    :
    WindowApplication3("FreeFormDeformation",0,0,640,480,
        ColorRGB(0.5f,0.0f,1.0f))
{
    m_iQuantity = 4;
    m_iDegree = 3;
    m_pkVolume = 0;
    m_akParameter = 0;
    m_bDoRandom = false;
    m_fAmplitude = 0.01f;
    m_fRadius = 0.25f;
    m_fLastUpdateTime = (float)System::GetTime();
    m_bMouseDown = false;
}
//----------------------------------------------------------------------------
bool FreeFormDeformation::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    // Set up the scene graph.  The TrnNode objects allows the application
    // to rotate the scene about its center.  Global wireframe and depth
    // buffering is used.
    m_spkScene = new Node(1);
    m_spkTrnNode = new Node(2);
    m_spkScene->AttachChild(m_spkTrnNode);
    m_spkWireframe = new WireframeState;
    m_spkScene->SetGlobalState(m_spkWireframe);

    Setup();

    // center-and-fit mesh for viewing by camera
    m_spkMesh->UpdateGS(0.0f);
    m_spkTrnNode->Local.SetTranslate(-m_spkScene->WorldBound->GetCenter());

    // set up camera
    m_spkCamera->SetFrustum(-0.055f,0.055f,-0.04125f,0.04125f,0.1f,100.0f);
    Vector3f kCDir(0.0f,0.0f,1.0f);
    Vector3f kCUp(0.0f,1.0f,0.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    Vector3f kCLoc = -4.0f*m_spkScene->WorldBound->GetRadius()*kCDir;
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS(0.0f);
    m_spkScene->UpdateRS();

    InitializeCameraMotion(0.01f,0.02f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void FreeFormDeformation::OnTerminate ()
{
    delete m_pkVolume;
    delete[] m_akParameter;
    m_spkScene = 0;
    m_spkTrnNode = 0;
    m_spkWireframe = 0;
    m_spkMesh = 0;
    m_spkPolylineRoot = 0;
    m_spkControlRoot = 0;
    m_spkSelected = 0;
    m_spkControlActive = 0;
    m_spkControlInactive = 0;
    m_spkControlLight = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void FreeFormDeformation::OnIdle ()
{
    MeasureTime();

    MoveCamera();
    if ( m_bDoRandom )
    {
        // deform the mesh no faster than 30 frames per second
        float fTime = (float)System::GetTime();
        if ( fTime - m_fLastUpdateTime >= 0.0333333f )
        {
            m_fLastUpdateTime = fTime;
            DoRandomControlPoints();
            m_spkScene->UpdateGS(0.0f);
        }
    }

    if ( MoveObject() )
        m_spkScene->UpdateGS(0.0f);

    // draw the scene as fast as possible (not limited to 30 fps)
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
bool FreeFormDeformation::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if ( WindowApplication3::OnKeyDown(ucKey,iX,iY) )
        return true;

    switch ( ucKey )
    {
    case 'w':  // toggle wireframe
    case 'W':
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;
    case 'r':  // toggle automated random versus user-adjusted controls
    case 'R':
        m_bDoRandom = !m_bDoRandom;
        return true;
    case 'c':  // toggle whether or not the polylines/boxes are drawn
    case 'C':
        m_spkPolylineRoot->ForceCull = !m_spkPolylineRoot->ForceCull;
        m_spkControlRoot->ForceCull = !m_spkControlRoot->ForceCull;
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
bool FreeFormDeformation::OnMouseClick (int iButton, int iState, int iX,
    int iY, unsigned int)
{
    if ( m_bDoRandom || iButton != MOUSE_LEFT_BUTTON )
        return false;

    if ( iState == MOUSE_DOWN )
    {
        m_bMouseDown = true;
        OnMouseDown(iX,iY);
    }
    else
    {
        m_bMouseDown = false;
    }

    return true;
}
//----------------------------------------------------------------------------
bool FreeFormDeformation::OnMotion (int, int iX, int iY)
{
    if ( m_bDoRandom || !m_bMouseDown || !m_spkSelected )
        return false;

    OnMouseMove(iX,iY);
    return true;
}
//----------------------------------------------------------------------------
void FreeFormDeformation::OnMouseDown (int iX, int iY)
{
    // the current selected control point is deactivated
    if ( m_spkSelected )
    {
        m_spkSelected->SetGlobalState(m_spkControlInactive);
        m_spkSelected->UpdateRS();
        m_spkSelected = 0;
    }

    // construct the pick ray
    Ray3f kRay;
    m_spkCamera->GetPickRay(iX,iY,m_iWidth,m_iHeight,kRay);

    // determine which control point has been selected (if any)
    Spatial::PickArray kResults;
    m_spkControlRoot->DoPick(kRay,kResults);
    if ( kResults.GetQuantity() > 0 )
    {
        // find the closest selected object
        float fTMin = kResults[0]->T;
        SpatialPtr spkCtrl = kResults[0]->IObject;
        delete kResults[0];
        if ( kResults.GetQuantity() > 1 )
        {
            for (int i = 1; i < kResults.GetQuantity(); i++)
            {
                if ( kResults[i]->T < fTMin )
                {
                    fTMin = kResults[i]->T;
                    spkCtrl = kResults[i]->IObject;
                }
                delete kResults[i];
            }
        }
        m_spkSelected = StaticCast<TriMesh>(spkCtrl);
        m_spkSelected->SetGlobalState(m_spkControlActive);
        m_spkSelected->UpdateRS();
    }
}
//----------------------------------------------------------------------------
void FreeFormDeformation::OnMouseMove (int iX, int iY)
{
    // Construct a pick ray, but this time we want to move the control
    // point from its current location to that ray.  The motion is in the
    // plane spanned by camera left/up vectors.  The new location is the
    // intersection of the pick ray and the plane Dot(D,X-P) = 0 where P
    // is the old control point location and where D is the camera direction
    // vector.
    Ray3f kRay;
    m_spkCamera->GetPickRay(iX,iY,m_iWidth,m_iHeight,kRay);

    // The new world position is at a distance along the pick ray that is
    // equal to the distance from the camera location to the old world
    // position.
    Vector3f kOldWorldPos = m_spkSelected->World.GetTranslate();
    Vector3f kDiff = kOldWorldPos - m_spkCamera->GetWorldLocation();
    Vector3f kNewWorldPos = kRay.Origin + kDiff.Length()*kRay.Direction;

    // Move the control point to the new world location.  The technical
    // problem is that we need to modify the world coordinates for the
    // selected control point.  Thus, we need to determine how to change the
    // local translation in order to cause the correct world translation.
    Node* pkPar = (Node*)m_spkSelected->GetParent();
    kDiff = kNewWorldPos - kOldWorldPos;
    Vector3f kLocalDiff = pkPar->World.InvertVector(kDiff);
    m_spkSelected->Local.SetTranslate(m_spkSelected->Local.GetTranslate() +
        kLocalDiff);
    m_spkSelected->UpdateGS(0.0f);

    // modify the control point itself
    int i0, i1, i2;
    sscanf(m_spkSelected->GetName(),"%d %d %d",&i0,&i1,&i2);
    m_pkVolume->SetControlPoint(i0,i1,i2,m_spkSelected->Local.GetTranslate());

    // TO DO.  Only need to update mesh vertices that are affected by the
    // change in one control point.  This requires working with the B-spline
    // basis function and knowing which (u,v,w) to evaluate at (i.e. determine
    // the local control region).
    UpdateMesh();

    // TO DO.  Only need to update neighboring lines.
    UpdatePolylines();
}
//----------------------------------------------------------------------------
void FreeFormDeformation::Setup ()
{
    Stream kInfile;
    kInfile.Load(System::GetPath("../../Data/Wmof/SmallBrain.wmof"));
    m_spkMesh = (TriMesh*)kInfile.GetObjectAt(0);
    m_spkTrnNode->AttachChild(m_spkMesh);

    CreateBSplineVolume();
    CreatePolylines();
    CreateControlBoxes();
}
//----------------------------------------------------------------------------
void FreeFormDeformation::CreateBSplineVolume ()
{
    // generate the B-spline volume function
    m_pkVolume = new BSplineVolumef(m_iQuantity,m_iQuantity,m_iQuantity,
        m_iDegree,m_iDegree,m_iDegree);

    // get AABB of form [xmin,xmax]x[ymin,ymax]x[zmin,zmax]
    int iVQuantity = m_spkMesh->Vertices->GetQuantity();
    Vector3f* akVertex = m_spkMesh->Vertices->GetData();
    m_fXMin = akVertex[0].X();
    m_fYMin = akVertex[0].Y();
    m_fZMin = akVertex[0].Z();
    float fXMax = m_fXMin, fYMax = m_fYMin, fZMax = m_fZMin;
    int i;
    for (i = 0; i < iVQuantity; i++)
    {
        if ( akVertex[i].X() < m_fXMin )
            m_fXMin = akVertex[i].X();
        else if ( akVertex[i].X() > fXMax )
            fXMax = akVertex[i].X();

        if ( akVertex[i].Y() < m_fYMin )
            m_fYMin = akVertex[i].Y();
        else if ( akVertex[i].Y() > fYMax )
            fYMax = akVertex[i].Y();

        if ( akVertex[i].Z() < m_fZMin )
            m_fZMin = akVertex[i].Z();
        else if ( akVertex[i].Z() > fZMax )
            fZMax = akVertex[i].Z();
    }

    // generate the control points
    float fXRange = fXMax - m_fXMin;
    float fYRange = fYMax - m_fYMin;
    float fZRange = fZMax - m_fZMin;
    m_fDX = fXRange/(float)(m_iQuantity-1);
    m_fDY = fYRange/(float)(m_iQuantity-1);
    m_fDZ = fZRange/(float)(m_iQuantity-1);
    Vector3f kCtrl;
    int i0, i1, i2;
    for (i0 = 0; i0 < m_iQuantity; i0++)
    {
        kCtrl.X() = m_fXMin + m_fDX*i0;
        for (i1 = 0; i1 < m_iQuantity; i1++)
        {
            kCtrl.Y() = m_fYMin + m_fDY*i1;
            for (i2 = 0; i2 < m_iQuantity; i2++)
            {
                kCtrl.Z() = m_fZMin + m_fDZ*i2;
                m_pkVolume->SetControlPoint(i0,i1,i2,kCtrl);
            }
        }
    }

    // compute the (u,v,w) values of the mesh relative to the B-spline volume
    float fInvXRange = 1.0f/fXRange;
    float fInvYRange = 1.0f/fYRange;
    float fInvZRange = 1.0f/fZRange;
    m_akParameter = new Vector3f[iVQuantity];
    for (i = 0; i < iVQuantity; i++)
    {
        m_akParameter[i].X() = (akVertex[i].X() - m_fXMin)*fInvXRange;
        m_akParameter[i].Y() = (akVertex[i].Y() - m_fYMin)*fInvYRange;
        m_akParameter[i].Z() = (akVertex[i].Z() - m_fZMin)*fInvZRange;
    }
}
//----------------------------------------------------------------------------
void FreeFormDeformation::CreatePolylines ()
{
    // generate the polylines that connect adjacent control points
    m_spkPolylineRoot = new Node(3*m_iQuantity*m_iQuantity*(m_iQuantity-1));
    m_spkTrnNode->AttachChild(m_spkPolylineRoot);

    Vector3f* akPVertex;
    Vector3fArray* pkVertices;
    ColorRGB* akPColor;
    ColorRGBArray* pkColors;
    VertexColorEffect* pkEffect;
    Polyline* pkPoly;
    int i0, i1, i2;

    for (i0 = 0; i0 < m_iQuantity; i0++)
    {
        for (i1 = 0; i1 < m_iQuantity; i1++)
        {
            for (i2 = 0; i2 < m_iQuantity-1; i2++)
            {
                akPVertex = new Vector3f[2];
                akPVertex[0] = m_pkVolume->GetControlPoint(i0,i1,i2);
                akPVertex[1] = m_pkVolume->GetControlPoint(i0,i1,i2+1);
                pkVertices = new Vector3fArray(2,akPVertex);
                pkPoly = new Polyline(pkVertices,false,true);

                akPColor = new ColorRGB[2];
                akPColor[0] = ColorRGB(0.0f,0.0f,0.75f);
                akPColor[1] = ColorRGB(0.0f,0.0f,0.75f);
                pkColors = new ColorRGBArray(2,akPColor);
                pkEffect = new VertexColorEffect(pkColors);
                pkPoly->SetEffect(pkEffect);

                m_spkPolylineRoot->AttachChild(pkPoly);
            }
        }

        for (i2 = 0; i2 < m_iQuantity; i2++)
        {
            for (i1 = 0; i1 < m_iQuantity-1; i1++)
            {
                akPVertex = new Vector3f[2];
                akPVertex[0] = m_pkVolume->GetControlPoint(i0,i1,i2);
                akPVertex[1] = m_pkVolume->GetControlPoint(i0,i1+1,i2);
                pkVertices = new Vector3fArray(2,akPVertex);
                pkPoly = new Polyline(pkVertices,false,true);

                akPColor = new ColorRGB[2];
                akPColor[0] = ColorRGB(0.0f,0.75f,0.0f);
                akPColor[1] = ColorRGB(0.0f,0.75f,0.0f);
                pkColors = new ColorRGBArray(2,akPColor);
                pkEffect = new VertexColorEffect(pkColors);
                pkPoly->SetEffect(pkEffect);

                m_spkPolylineRoot->AttachChild(pkPoly);
            }
        }
    }

    for (i0 = 0; i0 < m_iQuantity-1; i0++)
    {
        for (i1 = 0; i1 < m_iQuantity; i1++)
        {
            for (i2 = 0; i2 < m_iQuantity; i2++)
            {
                akPVertex = new Vector3f[2];
                akPVertex[0] = m_pkVolume->GetControlPoint(i0,i1,i2);
                akPVertex[1] = m_pkVolume->GetControlPoint(i0+1,i1,i2);
                pkVertices = new Vector3fArray(2,akPVertex);
                pkPoly = new Polyline(pkVertices,false,true);

                akPColor = new ColorRGB[2];
                akPColor[0] = ColorRGB(0.75f,0.0f,0.0f);
                akPColor[1] = ColorRGB(0.75f,0.0f,0.0f);
                pkColors = new ColorRGBArray(2,akPColor);
                pkEffect = new VertexColorEffect(pkColors);
                pkPoly->SetEffect(pkEffect);

                m_spkPolylineRoot->AttachChild(pkPoly);
            }
        }
    }
}
//----------------------------------------------------------------------------
void FreeFormDeformation::CreateControlBoxes ()
{
    // generate small boxes to represent the control points
    m_spkControlRoot = new Node(m_iQuantity*m_iQuantity*m_iQuantity);
    m_spkTrnNode->AttachChild(m_spkControlRoot);

    // create a single box to be copied by each control point box
    const float fHalfWidth = 0.02f;
    Vector3f* akVertex = new Vector3f[8];
    akVertex[0] = Vector3f(-fHalfWidth,-fHalfWidth,-fHalfWidth);
    akVertex[1] = Vector3f(+fHalfWidth,-fHalfWidth,-fHalfWidth);
    akVertex[2] = Vector3f(+fHalfWidth,+fHalfWidth,-fHalfWidth);
    akVertex[3] = Vector3f(-fHalfWidth,+fHalfWidth,-fHalfWidth);
    akVertex[4] = Vector3f(-fHalfWidth,-fHalfWidth,+fHalfWidth);
    akVertex[5] = Vector3f(+fHalfWidth,-fHalfWidth,+fHalfWidth);
    akVertex[6] = Vector3f(+fHalfWidth,+fHalfWidth,+fHalfWidth);
    akVertex[7] = Vector3f(-fHalfWidth,+fHalfWidth,+fHalfWidth);
    Vector3fArrayPtr spkVertices = new Vector3fArray(8,akVertex);

    int* aiIndex = new int[36];
    aiIndex[ 0] = 0;  aiIndex[ 1] = 2;  aiIndex[ 2] = 1;
    aiIndex[ 3] = 0;  aiIndex[ 4] = 3;  aiIndex[ 5] = 2;
    aiIndex[ 6] = 4;  aiIndex[ 7] = 5;  aiIndex[ 8] = 6;
    aiIndex[ 9] = 4;  aiIndex[10] = 6;  aiIndex[11] = 7;
    aiIndex[12] = 0;  aiIndex[13] = 5;  aiIndex[14] = 4;
    aiIndex[15] = 0;  aiIndex[16] = 1;  aiIndex[17] = 5;
    aiIndex[18] = 3;  aiIndex[19] = 7;  aiIndex[20] = 6;
    aiIndex[21] = 3;  aiIndex[22] = 6;  aiIndex[23] = 2;
    aiIndex[24] = 1;  aiIndex[25] = 2;  aiIndex[26] = 6;
    aiIndex[27] = 1;  aiIndex[28] = 6;  aiIndex[29] = 5;
    aiIndex[30] = 0;  aiIndex[31] = 4;  aiIndex[32] = 7;
    aiIndex[33] = 0;  aiIndex[34] = 7;  aiIndex[35] = 3;
    IntArrayPtr spkIndices = new IntArray(36,aiIndex);

    // create the materials and light to be attached to each box
    m_spkControlActive = new MaterialState;
    m_spkControlActive->Emissive = ColorRGBA::BLACK;
    m_spkControlActive->Ambient = ColorRGBA(1.0f,0.0f,0.0f,1.0f);
    m_spkControlActive->Diffuse = ColorRGBA(1.0f,0.0f,0.0f,1.0f);
    m_spkControlActive->Specular = ColorRGBA::BLACK;
    m_spkControlActive->Shininess = 1.0f;

    m_spkControlInactive = new MaterialState;
    m_spkControlInactive->Emissive = ColorRGBA::BLACK;
    m_spkControlInactive->Ambient = ColorRGBA(0.75f,0.75f,0.75f,1.0f);
    m_spkControlInactive->Diffuse = ColorRGBA(0.75f,0.75f,0.75f,1.0f);
    m_spkControlInactive->Specular = ColorRGBA::BLACK;
    m_spkControlInactive->Shininess = 1.0f;

    m_spkControlLight = new Light(Light::LT_AMBIENT);
    m_spkControlLight->Ambient = ColorRGBA::WHITE;
    m_spkControlLight->Diffuse = ColorRGBA::WHITE;
    m_spkControlLight->Specular = ColorRGBA::BLACK;

    int i0, i1, i2;
    for (i0 = 0; i0 < m_iQuantity; i0++)
    {
        for (i1 = 0; i1 < m_iQuantity; i1++)
        {
            for (i2 = 0; i2 < m_iQuantity; i2++)
            {
                TriMesh* pkBox = new TriMesh(spkVertices,spkIndices,false);
                pkBox->Local.SetTranslate(
                    m_pkVolume->GetControlPoint(i0,i1,i2));

                // Encode the indices in the name for later use.  This will
                // allow fast lookup of volume control points.
                char acName[32];
                sprintf(acName,"%d %d %d",i0,i1,i2);
                pkBox->SetName(acName);

                pkBox->SetGlobalState(m_spkControlInactive);
                pkBox->SetLight(m_spkControlLight);

                m_spkControlRoot->AttachChild(pkBox);
            }
        }
    }
}
//----------------------------------------------------------------------------
void FreeFormDeformation::UpdateMesh ()
{
    // Update the mesh points.  Typically you would update the model bound
    // after modifying the vertices, but in this application the mesh is
    // always visible, so an accurate model bound for culling purposes is
    // not necessary--the model bound is never updated.

    int iVQuantity = m_spkMesh->Vertices->GetQuantity();
    Vector3f* akVertex = m_spkMesh->Vertices->GetData();
    for (int i = 0; i < iVQuantity; i++)
    {
        akVertex[i] = m_pkVolume->GetPosition(m_akParameter[i].X(),
            m_akParameter[i].Y(),m_akParameter[i].Z());
    }

    // Typical calls after modification, but not in this application.
    // m_spkMesh->UpdateModelBound();
    // m_spkMesh->UpdateModelNormals();  // if lighting affects mesh
}
//----------------------------------------------------------------------------
void FreeFormDeformation::UpdatePolylines ()
{
    // update the polyline mesh
    Vector3f* akPVertex;
    Polyline* pkPoly;
    int i0, i1, i2, i = 0;

    for (i0 = 0; i0 < m_iQuantity; i0++)
    {
        for (i1 = 0; i1 < m_iQuantity; i1++)
        {
            for (i2 = 0; i2 < m_iQuantity-1; i2++)
            {
                pkPoly = StaticCast<Polyline>(m_spkPolylineRoot->GetChild(i));
                akPVertex = pkPoly->Vertices->GetData();
                akPVertex[0] = m_pkVolume->GetControlPoint(i0,i1,i2);
                akPVertex[1] = m_pkVolume->GetControlPoint(i0,i1,i2+1);
                i++;
            }
        }

        for (i2 = 0; i2 < m_iQuantity; i2++)
        {
            for (i1 = 0; i1 < m_iQuantity-1; i1++)
            {
                pkPoly = StaticCast<Polyline>(m_spkPolylineRoot->GetChild(i));
                akPVertex = pkPoly->Vertices->GetData();
                akPVertex[0] = m_pkVolume->GetControlPoint(i0,i1,i2);
                akPVertex[1] = m_pkVolume->GetControlPoint(i0,i1+1,i2);
                i++;
            }
        }
    }

    for (i0 = 0; i0 < m_iQuantity-1; i0++)
    {
        for (i1 = 0; i1 < m_iQuantity; i1++)
        {
            for (i2 = 0; i2 < m_iQuantity; i2++)
            {
                pkPoly = StaticCast<Polyline>(m_spkPolylineRoot->GetChild(i));
                akPVertex = pkPoly->Vertices->GetData();
                akPVertex[0] = m_pkVolume->GetControlPoint(i0,i1,i2);
                akPVertex[1] = m_pkVolume->GetControlPoint(i0+1,i1,i2);
                i++;
            }
        }
    }
}
//----------------------------------------------------------------------------
void FreeFormDeformation::UpdateControlBoxes ()
{
    int i0, i1, i2, i = 0;
    for (i0 = 0; i0 < m_iQuantity; i0++)
    {
        for (i1 = 0; i1 < m_iQuantity; i1++)
        {
            for (i2 = 0; i2 < m_iQuantity; i2++)
            {
                TriMesh* pkBox = StaticCast<TriMesh>(
                    m_spkControlRoot->GetChild(i));
                pkBox->Local.SetTranslate(
                    m_pkVolume->GetControlPoint(i0,i1,i2));
                i++;
            }
        }
    }
}
//----------------------------------------------------------------------------
void FreeFormDeformation::DoRandomControlPoints ()
{
    // Randomly perturb the control points, but stay near the original
    // control points.
    int i0, i1, i2;
    Vector3f kCtrl;
    for (i0 = 0; i0 < m_iQuantity; i0++)
    {
        kCtrl.X() = m_fXMin + m_fDX*i0;
        for (i1 = 0; i1 < m_iQuantity; i1++)
        {
            kCtrl.Y() = m_fYMin + m_fDY*i1;
            for (i2 = 0; i2 < m_iQuantity; i2++)
            {
                kCtrl.Z() = m_fZMin + m_fDZ*i2;

                Vector3f kNewCtrl = m_pkVolume->GetControlPoint(i0,i1,i2) +
                    m_fAmplitude*Vector3f(Mathf::SymmetricRandom(),
                    Mathf::SymmetricRandom(),Mathf::SymmetricRandom());

                Vector3f kDiff = kNewCtrl - kCtrl;
                float fLength = kDiff.Length();
                if ( fLength > m_fRadius )
                    kDiff *= m_fRadius/fLength;

                m_pkVolume->SetControlPoint(i0,i1,i2,kCtrl+kDiff);
            }
        }
    }

    UpdateMesh();
    UpdatePolylines();
    UpdateControlBoxes();
}
//----------------------------------------------------------------------------
