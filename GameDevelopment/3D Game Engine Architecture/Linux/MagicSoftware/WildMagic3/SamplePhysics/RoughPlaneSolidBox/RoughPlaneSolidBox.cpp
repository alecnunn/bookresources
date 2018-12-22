// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "RoughPlaneSolidBox.h"
using namespace std;

WM3_WINDOW_APPLICATION(RoughPlaneSolidBox);

//#define SINGLE_STEP

//----------------------------------------------------------------------------
RoughPlaneSolidBox::RoughPlaneSolidBox ()
    :
    WindowApplication3("RoughPlaneSolidBox",0,0,640,480,
        ColorRGB(0.839215f,0.894117f,0.972549f))
{
    m_bDoUpdate = true;
}
//----------------------------------------------------------------------------
bool RoughPlaneSolidBox::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    InitializeModule();

    // set up the scene graph
    m_spkScene = new Node(3);

    // wireframe mode
    m_spkWireframe = new WireframeState;
    m_spkScene->SetGlobalState(m_spkWireframe);

    m_spkScene->AttachChild(CreateGround());
    m_spkScene->AttachChild(CreateRamp());
    m_spkScene->AttachChild(CreateBox());

    m_spkScene->Local.SetRotate(
        Matrix3f().FromAxisAngle(Vector3f::UNIT_Z,0.661917f));

    // set up camera
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,100.0f);
    float fAngle = 0.1f*Mathf::PI;
    float fCos = Mathf::Cos(fAngle), fSin = Mathf::Sin(fAngle);
    Vector3f kCLoc(17.695415f,0.0f,6.4494629f);
    Vector3f kCDir(-fCos,0.0f,-fSin);
    Vector3f kCUp(-fSin,0.0f,fCos);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // initial update of objects
    m_spkScene->UpdateGS(0.0f);
    m_spkScene->UpdateRS();

    InitializeCameraMotion(0.001f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void RoughPlaneSolidBox::OnTerminate ()
{
    m_spkScene = 0;
    m_spkBox = 0;
    m_spkWireframe = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void RoughPlaneSolidBox::OnIdle ()
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
        DrawFrameRate(8,GetHeight()-8,ColorRGBA::WHITE);
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool RoughPlaneSolidBox::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if ( WindowApplication3::OnKeyDown(ucKey,iX,iY) )
        return true;

    switch ( ucKey )
    {
    case 'w':  // toggle wireframe
    case 'W':
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;

    case 'r':  // restart
    case 'R':
        InitializeModule();
        MoveBox();
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
Node* RoughPlaneSolidBox::CreateBox ()
{
    Node* pkBox = new Node(6);
    float fXExtent = (float)m_kModule.XLocExt;
    float fYExtent = (float)m_kModule.YLocExt;
    float fZExtent = (float)m_kModule.ZLocExt;

    Transformation kXFrm;
    StandardMesh kSM;
    TriMesh* pkFace;
    ColorRGB* akColor;
    ColorRGBArray* pkColors;
    VertexColorEffect* pkEffect;

    // +z face
    kXFrm.SetTranslate(Vector3f(0.0f,0.0f,fZExtent));
    kSM.Transform(kXFrm);
    pkFace = kSM.Rectangle(2,2,fXExtent,fYExtent);
    akColor = new ColorRGB[4];
    akColor[0] = ColorRGB(1.0f,0.0f,0.0f);
    akColor[1] = ColorRGB(1.0f,0.0f,0.0f);
    akColor[2] = ColorRGB(1.0f,0.0f,0.0f);
    akColor[3] = ColorRGB(1.0f,0.0f,0.0f);
    pkColors = new ColorRGBArray(4,akColor);
    pkEffect = new VertexColorEffect(pkColors);
    pkFace->SetEffect(pkEffect);
    pkBox->AttachChild(pkFace);

    // -z face
    kXFrm.SetTranslate(Vector3f(0.0f,0.0f,-fZExtent));
    kXFrm.SetRotate(Matrix3f(Vector3f::UNIT_Y,Vector3f::UNIT_X,
        -Vector3f::UNIT_Z,true));
    kSM.Transform(kXFrm);
    pkFace = kSM.Rectangle(2,2,fYExtent,fXExtent);
    akColor = new ColorRGB[4];
    akColor[0] = ColorRGB(0.5f,0.0f,0.0f);
    akColor[1] = ColorRGB(0.5f,0.0f,0.0f);
    akColor[2] = ColorRGB(0.5f,0.0f,0.0f);
    akColor[3] = ColorRGB(0.5f,0.0f,0.0f);
    pkColors = new ColorRGBArray(4,akColor);
    pkEffect = new VertexColorEffect(pkColors);
    pkFace->SetEffect(pkEffect);
    pkBox->AttachChild(pkFace);

    // +y face
    kXFrm.SetTranslate(Vector3f(0.0f,fYExtent,0.0f));
    kXFrm.SetRotate(Matrix3f(Vector3f::UNIT_Z,Vector3f::UNIT_X,
        Vector3f::UNIT_Y,true));
    kSM.Transform(kXFrm);
    pkFace = kSM.Rectangle(2,2,fZExtent,fXExtent);
    akColor = new ColorRGB[4];
    akColor[0] = ColorRGB(0.0f,1.0f,0.0f);
    akColor[1] = ColorRGB(0.0f,1.0f,0.0f);
    akColor[2] = ColorRGB(0.0f,1.0f,0.0f);
    akColor[3] = ColorRGB(0.0f,1.0f,0.0f);
    pkColors = new ColorRGBArray(4,akColor);
    pkEffect = new VertexColorEffect(pkColors);
    pkFace->SetEffect(pkEffect);
    pkBox->AttachChild(pkFace);

    // -y face
    kXFrm.SetTranslate(Vector3f(0.0f,-fYExtent,0.0f));
    kXFrm.SetRotate(Matrix3f(Vector3f::UNIT_X,Vector3f::UNIT_Z,
        -Vector3f::UNIT_Y,true));
    kSM.Transform(kXFrm);
    pkFace = kSM.Rectangle(2,2,fXExtent,fZExtent);
    akColor = new ColorRGB[4];
    akColor[0] = ColorRGB(0.0f,0.5f,0.0f);
    akColor[1] = ColorRGB(0.0f,0.5f,0.0f);
    akColor[2] = ColorRGB(0.0f,0.5f,0.0f);
    akColor[3] = ColorRGB(0.0f,0.5f,0.0f);
    pkColors = new ColorRGBArray(4,akColor);
    pkEffect = new VertexColorEffect(pkColors);
    pkFace->SetEffect(pkEffect);
    pkBox->AttachChild(pkFace);

    // +x face
    kXFrm.SetTranslate(Vector3f(fXExtent,0.0f,0.0f));
    kXFrm.SetRotate(Matrix3f(Vector3f::UNIT_Y,Vector3f::UNIT_Z,
        Vector3f::UNIT_X,true));
    kSM.Transform(kXFrm);
    pkFace = kSM.Rectangle(2,2,fYExtent,fZExtent);
    akColor = new ColorRGB[4];
    akColor[0] = ColorRGB(0.0f,0.0f,1.0f);
    akColor[1] = ColorRGB(0.0f,0.0f,1.0f);
    akColor[2] = ColorRGB(0.0f,0.0f,1.0f);
    akColor[3] = ColorRGB(0.0f,0.0f,1.0f);
    pkColors = new ColorRGBArray(4,akColor);
    pkEffect = new VertexColorEffect(pkColors);
    pkFace->SetEffect(pkEffect);
    pkBox->AttachChild(pkFace);

    // -x face
    kXFrm.SetTranslate(Vector3f(-fXExtent,0.0f,0.0f));
    kXFrm.SetRotate(Matrix3f(Vector3f::UNIT_Z,Vector3f::UNIT_Y,
        -Vector3f::UNIT_X,true));
    kSM.Transform(kXFrm);
    pkFace = kSM.Rectangle(2,2,fZExtent,fYExtent);
    akColor = new ColorRGB[4];
    akColor[0] = ColorRGB(0.0f,0.0f,0.5f);
    akColor[1] = ColorRGB(0.0f,0.0f,0.5f);
    akColor[2] = ColorRGB(0.0f,0.0f,0.5f);
    akColor[3] = ColorRGB(0.0f,0.0f,0.5f);
    pkColors = new ColorRGBArray(4,akColor);
    pkEffect = new VertexColorEffect(pkColors);
    pkFace->SetEffect(pkEffect);
    pkBox->AttachChild(pkFace);

    m_spkBox = pkBox;
    MoveBox();

    return pkBox;
}
//----------------------------------------------------------------------------
TriMesh* RoughPlaneSolidBox::CreateRamp ()
{
    float fX = 8.0f, fY = 8.0f, fZ = fY*Mathf::Tan((float)m_kModule.Angle);

    Vector3f* akVertex = new Vector3f[6];
    akVertex[0] = Vector3f(-fX,0.0f,0.0f);
    akVertex[1] = Vector3f(+fX,0.0f,0.0f);
    akVertex[2] = Vector3f(-fX,fY,0.0f);
    akVertex[3] = Vector3f(+fX,fY,0.0f);
    akVertex[4] = Vector3f(-fX,fY,fZ);
    akVertex[5] = Vector3f(+fX,fY,fZ);
    Vector3fArray* pkVertices = new Vector3fArray(6,akVertex);

    int* aiIndex = new int[18];
    aiIndex[ 0] = 0;  aiIndex[ 1] = 1;  aiIndex[ 2] = 4;
    aiIndex[ 3] = 1;  aiIndex[ 4] = 5;  aiIndex[ 5] = 4;
    aiIndex[ 6] = 0;  aiIndex[ 7] = 4;  aiIndex[ 8] = 2;
    aiIndex[ 9] = 1;  aiIndex[10] = 3;  aiIndex[11] = 5;
    aiIndex[12] = 3;  aiIndex[13] = 2;  aiIndex[14] = 4;
    aiIndex[15] = 3;  aiIndex[16] = 4;  aiIndex[17] = 5;
    IntArray* pkIndices = new IntArray(18,aiIndex);

    TriMesh* pkRamp = new TriMesh(pkVertices,pkIndices,false);

    Vector2f* akUV = new Vector2f[6];
    akUV[0] = Vector2f(0.25f,0.0f);
    akUV[1] = Vector2f(0.75f,0.0f);
    akUV[2] = Vector2f(0.0f,1.0f);
    akUV[3] = Vector2f(1.0f,1.0f);
    akUV[4] = Vector2f(0.25f,1.0f);
    akUV[5] = Vector2f(0.75f,1.0f);
    Vector2fArray* pkUVs = new Vector2fArray(6,akUV);

    Texture* pkTexture = new Texture(
        Image::Load(System::GetPath("../../Data/Wmif/Metal.wmif")));
    pkTexture->CoordU = Texture::WM_REPEAT;
    pkTexture->CoordV = Texture::WM_REPEAT;

    TextureEffect* pkEffect = new TextureEffect(pkTexture,pkUVs);
    pkRamp->SetEffect(pkEffect);

    return pkRamp;
}
//----------------------------------------------------------------------------
TriMesh* RoughPlaneSolidBox::CreateGround ()
{
    StandardMesh kSM;
    kSM.UVs(true,false);
    TriMesh* pkGround = kSM.Rectangle(2,2,32.0f,32.0f);

    // change the texture repeat
    int iVQuantity = pkGround->Vertices->GetQuantity();
    Vector2f* akUV = kSM.GetUVs()->GetData();
    for (int i = 0; i < iVQuantity; i++)
        akUV[i] *= 8.0f;

    Texture* pkTexture = new Texture(
        Image::Load(System::GetPath("../../Data/Wmif/Gravel.wmif")));
    pkTexture->CoordU = Texture::WM_REPEAT;
    pkTexture->CoordV = Texture::WM_REPEAT;

    TextureEffect* pkEffect = new TextureEffect(pkTexture,kSM.GetUVs());
    pkGround->SetEffect(pkEffect);

    return pkGround;
}
//----------------------------------------------------------------------------
void RoughPlaneSolidBox::InitializeModule ()
{
    // set up the physics module
    m_kModule.Mu = 0.01;
    m_kModule.Gravity = 10.0;
    m_kModule.Angle = 0.125*Mathd::PI;
    m_kModule.XLocExt = 0.8;
    m_kModule.YLocExt = 0.4;
    m_kModule.ZLocExt = 0.2;

    // initialize the differential equations
    double dTime = 0.0;
    double dDeltaTime = 0.0005;
    double dX = -6.0;
    double dW = 1.0;
    double dXDot = 4.0;
    double dWDot = 6.0;
    double dTheta = 0.25*Mathd::PI;
    double dThetaDot = 4.0;
    m_kModule.Initialize(dTime,dDeltaTime,dX,dW,dTheta,dXDot,dWDot,dThetaDot);

    m_bDoUpdate = true;
}
//----------------------------------------------------------------------------
void RoughPlaneSolidBox::MoveBox ()
{
    float fX = (float)m_kModule.GetX();
    float fW = (float)m_kModule.GetW();
    float fXExt = (float)m_kModule.XLocExt;
    float fYExt = (float)m_kModule.YLocExt;
    float fZExt = (float)m_kModule.ZLocExt;
    float fSinPhi = (float)m_kModule.SinAngle;
    float fCosPhi = (float)m_kModule.CosAngle;
    float fTheta = (float)m_kModule.GetTheta();
    float fSinTheta = Mathf::Sin(fTheta);
    float fCosTheta = Mathf::Cos(fTheta);

    // compute the box center
    Vector3f kCenter(fX,fW*fCosPhi-fZExt*fSinPhi,fW*fSinPhi+fZExt*fCosPhi);

    // compute the box orientation
    Vector3f kAxis1(fCosTheta,-fSinTheta*fCosPhi,-fSinTheta*fSinPhi);
    Vector3f kAxis2(fSinTheta,+fCosTheta*fCosPhi,+fCosTheta*fSinPhi);
    Vector3f kAxis3(0.0f,-fSinPhi,fCosPhi);

    // keep the box from sliding below the ground
    float fZRadius = fXExt*Mathf::FAbs(kAxis1.Z()) +
        fYExt*Mathf::FAbs(kAxis2.Z()) + fZExt*Mathf::FAbs(kAxis3.Z());
    if ( kCenter.Z() >= fZRadius )
    {
        // update the box
        m_spkBox->Local.SetTranslate(kCenter);
        m_spkBox->Local.SetRotate(Matrix3f(kAxis1,kAxis2,kAxis3,true));
        m_spkBox->UpdateGS(0.0f);
    }
    else
    {
        m_bDoUpdate = false;
    }
}
//----------------------------------------------------------------------------
void RoughPlaneSolidBox::DoPhysical ()
{
    if ( m_bDoUpdate )
    {
        m_kModule.Update();
        MoveBox();
    }
}
//----------------------------------------------------------------------------
