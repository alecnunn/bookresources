// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "RipplingOcean.h"

WM3_WINDOW_APPLICATION(RipplingOcean);

//----------------------------------------------------------------------------
RipplingOcean::RipplingOcean ()
    :
    WindowApplication3("RipplingOcean",0,0,640,480,ColorRGB(0.5f,0.0f,1.0f))
{

}
//----------------------------------------------------------------------------
bool RipplingOcean::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    // set up camera
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,10000.0f);
    Vector3f kCLoc(0.0f, -600.0f, -100.0f);
    Vector3f kCDir(0.0f,1.0f,0.5f);
    kCDir.Normalize();
    Vector3f kCUp(0.0f,1.0f,0.0f); 
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // set up effect parameters
    m_fAmbient = 0.3f;
    m_fTexRepeat = 6.0f;
    m_fWaveSpeedFactor = 1.0f;
    m_fWaveHeightFactor = 1.0f;
    m_fRippleSpeedFactor = 1.0f;

    CreateScene();

    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    InitializeCameraMotion(0.1f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void RipplingOcean::OnTerminate ()
{
    m_spkScene = 0;
    m_spkTrnNode = 0;
    m_spkModel = 0;
    m_spkEffect = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void RipplingOcean::OnIdle ()
{
    MeasureTime();

    MoveCamera();
    if ( MoveObject() )
        m_spkScene->UpdateGS();

    SetConstants();

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
bool RipplingOcean::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if ( WindowApplication3::OnKeyDown(ucKey,iX,iY) )
        return true;

    switch ( ucKey )
    {
    case 'w':
        m_fWaveHeightFactor -= 0.1f;
        if ( m_fWaveHeightFactor < 0 )
            m_fWaveHeightFactor = 0;
        return true;
    case 'W':
        m_fWaveHeightFactor += 0.1f;
        return true;
    case 'v':
        m_fWaveSpeedFactor -= 0.1f;
        if ( m_fWaveSpeedFactor < 0 )
            m_fWaveSpeedFactor = 0;
        return true;
    case 'V':
        m_fWaveSpeedFactor += 0.1f;
        return true;
    case 'a':
        m_fAmbient -= 0.05f;
        if ( m_fAmbient < 0 )
            m_fAmbient = 0;
        return true;
    case 'A':
        m_fAmbient += 0.05f;
        if ( m_fAmbient > 1 )
            m_fAmbient = 1;
        return true;
    case 'r':
        m_fRippleSpeedFactor -= 0.1f;
        if ( m_fRippleSpeedFactor < 0 )
            m_fRippleSpeedFactor = 0;
        return true;
    case 'R':
        m_fRippleSpeedFactor += 0.1f;
        return true;
    case 'T':
        m_fTexRepeat += 0.1f;
        return true;
    case 't':
        m_fTexRepeat -= 0.1f;
        if ( m_fTexRepeat < 0 )
            m_fTexRepeat = 0;
        return true;
    case ' ':
        m_bStopped = !m_bStopped;
        m_fStopTime = (float)System::GetTime();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void RipplingOcean::HeightToNormalMap (Image*& pkImage)
{
    // Simple 4 sample nearest neighbor kernel filter

    // It only uses the red component to determine the height (which is fine
    // for all monochrome height maps).
 
    // It also assumes that the image will wrap, and so when it goes off one
    // edge it will reuse the data on the other side.

    // It also assumes that positive Z is the direction of the height map.

    int iWidth = pkImage->GetWidth();
    int iHeight = pkImage->GetHeight();
    int iFac = pkImage->GetBytesPerPixel();

    float fScale = 100.0f;

    fScale *= 1.0f/255.0f;

    unsigned char* aucNormal = new unsigned char[iWidth*iHeight*4];

    for (int iX = 0; iX < iWidth; iX++)
    {
        for (int iY = 0; iY < iHeight; iY++)
        {
            int iNegX = (int)(pkImage->GetData()[iFac*((iX-1+iWidth)%iWidth +
                iY*iWidth)]);
            int iPosX = (int)(pkImage->GetData()[iFac*((iX+1)%iWidth +
                iY*iWidth)]);;

            int iNegY = (int)(pkImage->GetData()[iFac*(iX+((iY-1+iHeight)%
                iHeight)*iWidth)]);
            int iPosY = (int)(pkImage->GetData()[iFac*(iX+((iY+1)%iHeight)*
                iWidth)]);

            Vector3f kCross(fScale*(iNegX-iPosX)/2.0f, 
                fScale*(iNegY-iPosY)/2.0f, 1.0f);
            kCross.Normalize();

            kCross = 0.5f*kCross + Vector3f(0.5f,0.5f,0.5f);

            unsigned char* aucColor = &aucNormal[4*(iX+iY*iWidth)];
            aucColor[0] = (unsigned char)((int)(kCross.X()*255));
            aucColor[1] = (unsigned char)((int)(kCross.Y()*255));
            aucColor[2] = (unsigned char)((int)(kCross.Z()*255));
            aucColor[3] = (unsigned char)255;
        }
    }

    delete pkImage;
    pkImage = new Image(Image::IT_RGBA8888,iWidth,iHeight,aucNormal);
}
//----------------------------------------------------------------------------
TriMesh* RipplingOcean::CreateRectangleMesh (
    const Vector3f& rkCenter, const Vector3f& rkU, const Vector3f& rkV,
    const Vector3f& rkAxis, float fUExtent, float fVExtent,
    int iUSamples, int iVSamples, ColorRGBArray*& rpkOutputColors,
    Vector2fArray*& rpkOutputUVs)
{
    assert( iUSamples >=2 );
    assert( iVSamples >=2 );

    // allocate vertices
    int iVQuantity = iUSamples*iVSamples;
    Vector3f* akVertex = new Vector3f[iVQuantity];
    Vector3f* akNormal = new Vector3f[iVQuantity];
    ColorRGB* akColor = new ColorRGB[iVQuantity];

    // We want tangent data here.  rkV better be normalized!
    ColorRGB kColor( rkV.X()*0.5f+0.5f, rkV.Y()*0.5f+0.5f, 
        rkV.Z()*0.5f+0.5f );        

    int i;
    for (i = 0; i < iVQuantity; i++)
        akColor[i] = kColor;

    Vector2f* akUV = new Vector2f[iVQuantity];

    // allocate connectivity
    int iTQuantity = (iUSamples-1)*(iVSamples-1)*2;
    int* aiIndex = new int[3*iTQuantity];

    int iVNum = 0;

    // generate geometry
    float fInvU = 1.0f/(float)(iUSamples-1);
    float fInvV = 1.0f/(float)(iVSamples-1);

    float fCurU, fCurV;
    int iU, iV;
    for (iU = 0, fCurU = 0.0f; iU < iUSamples; iU++)
    {
        for (iV = 0, fCurV = 0.0f; iV < iVSamples; iV++)
        {
            akVertex[iVNum] = rkCenter + fUExtent*rkU*(fCurU-0.5f)
                + fVExtent*rkV*(fCurV-0.5f);

            akUV[iVNum].X() = fCurU;
            akUV[iVNum].Y() = fCurV;

            iVNum++;
            fCurV += fInvV;
        }
        fCurU += fInvU;
    }

    for (i = 0; i < iVQuantity; i++)
        akNormal[i] = rkAxis;

    assert( iVNum == iVQuantity );

    int iC = 0;

    // generate connectivity
    for (iU = 0; iU < iUSamples-1; iU++)
    {
        for (iV = 0; iV < iVSamples-1; iV++)
        {
            int iV0, iV1, iV2, iV3;
            iV0 = iV + iVSamples*iU;
            iV1 = iV0 + 1;
            iV2 = iV0 + iVSamples;
            iV3 = iV2 + 1;

            aiIndex[iC++] = iV0;
            aiIndex[iC++] = iV1;
            aiIndex[iC++] = iV2;

            aiIndex[iC++] = iV2;
            aiIndex[iC++] = iV1;
            aiIndex[iC++] = iV3;
        }
    }

    assert( iC == iTQuantity*3 );

    Vector3fArray* pkVerts = new Vector3fArray(iVQuantity,akVertex);
    Vector3fArray* pkNorms = new Vector3fArray(iVQuantity,akNormal);
    rpkOutputColors = new ColorRGBArray(iVQuantity,akColor);
    rpkOutputUVs = new Vector2fArray(iVQuantity,akUV);
    IntArray* pkIndices = new IntArray(iC,aiIndex);

    TriMesh* pkMesh = new TriMesh(pkVerts,pkIndices,false);
    pkMesh->Normals = pkNorms;

    return pkMesh;
}
//----------------------------------------------------------------------------
void RipplingOcean::SetConstants ()
{
    // Re-set all the constants
    float fTime;

    if ( m_bStopped )
        fTime = m_fStopTime;
    else
        fTime = (float)System::GetTime();

    // arbitrary constants to make particular looking waves
    const Vector4f kWaveDirX(0.25f,0.0f,-0.7f,-0.8f);
    const Vector4f kWaveDirY(0.0f,0.15f,-0.7f,0.1f);
    const Vector4f kWaveSpeed(0.2f,0.15f,0.4f,0.4f);
    const Vector4f kWaveOffset(0.0f,0.2f,0.3f,-0.2f);
    const Vector4f kWaveHeight(-16.0f,10.0f,5.8f,8.5f);
    const Vector4f kBumpSpeed(0.031f,0.04f,-0.03f,0.02f);

    m_spkEffect->SetConstants(
        Vector4f(1/24.0f,m_fAmbient,m_fTexRepeat,fTime));

    Vector3f kLightDir(0.0f,1.0f,1.0f);
    kLightDir.Normalize();

    m_spkEffect->SetLightDir(kLightDir);
    m_spkEffect->SetWaveDirX(kWaveDirX);
    m_spkEffect->SetWaveDirY(kWaveDirY);
    m_spkEffect->SetWaveSpeed(m_fWaveSpeedFactor*kWaveSpeed);
    m_spkEffect->SetWaveOffset(kWaveOffset);
    m_spkEffect->SetWaveHeight(kWaveHeight*m_fWaveHeightFactor);
    m_spkEffect->SetBumpSpeed(kBumpSpeed*m_fRippleSpeedFactor);
}
//----------------------------------------------------------------------------
void RipplingOcean::CreateScene ()
{
    m_bStopped = false;
    m_fStopTime = (float)System::GetTime();

    m_spkScene = new Node;
    m_spkTrnNode = new Node;
    m_spkScene->AttachChild(m_spkTrnNode);

    // Root node for a scene graph that contains a bump-mapped triangle mesh
    // square.
    m_spkModel = new Node;

    // create the triangle mesh surface
    Vector2fArray* pkUVs;
    ColorRGBArray* pkRGBs;
    TriMesh* pkMesh = CreateRectangleMesh(Vector3f::ZERO,Vector3f::UNIT_X,
        Vector3f::UNIT_Y,-Vector3f::UNIT_Z,1400.0f,1200.0f,50,50,pkRGBs,
        pkUVs);

    Image* pkNormal = Image::Load(
        System::GetPath("../../Data/Wmif/Plasma.wmif"));
    assert( pkNormal );
    HeightToNormalMap( pkNormal );

    Image* pkWater = Image::Load(
        System::GetPath("../../Data/Wmif/WaterGradient.wmif"));
    assert( pkWater );

    Image* pkSkySphere = Image::Load(
        System::GetPath("../../Data/Wmif/RippleSky.wmif"));
    assert( pkSkySphere );

    m_spkEffect = new RipplingOceanEffect(pkNormal,pkWater,pkSkySphere,
        pkUVs, pkRGBs);
    AttachShader(pkMesh);

    m_spkModel->AttachChild(pkMesh);
    m_spkTrnNode->AttachChild(m_spkModel);

    // I'll admit that this is kind of a hack, but it puts the sun
    // a smidge higher in the sky.  It makes it look nicest to start.  =)
    Matrix3f kIncr;
    kIncr.FromAxisAngle(Vector3f::UNIT_X, -0.08f);
    m_spkTrnNode->World.SetRotate(kIncr);
}
//----------------------------------------------------------------------------
void RipplingOcean::AttachShader (Spatial* pkObject)
{
    Geometry* pkGeom = DynamicCast<Geometry>(pkObject);
    if ( pkGeom )
    {
        RipplingOceanEffect* pkClone = 
            (RipplingOceanEffect*)m_spkEffect->Clone();
        pkGeom->SetEffect(pkClone);
        return;
    }

    Node* pkNode = DynamicCast<Node>(pkObject);
    assert( pkNode );
    for (int i = 0; i < pkNode->GetQuantity(); i++)
    {
        Spatial* pkChild = pkNode->GetChild(i);
        if ( pkChild )
            AttachShader(pkChild);
    }
}
//----------------------------------------------------------------------------
