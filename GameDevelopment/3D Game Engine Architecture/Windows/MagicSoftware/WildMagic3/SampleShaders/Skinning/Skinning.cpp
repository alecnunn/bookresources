// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Skinning.h"

WM3_WINDOW_APPLICATION(Skinning);

//----------------------------------------------------------------------------
Skinning::Skinning ()
    :
    WindowApplication3("Skinning",0,0,640,480,ColorRGB(0.5f,0.0f,1.0f))
{

}
//----------------------------------------------------------------------------
bool Skinning::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1000.0f);
    Vector3f kCLoc(0.0f, 0.0f, 0.0f);
    Vector3f kCDir(0.0f,0.0f,1.0f);
    Vector3f kCUp(0.0f,1.0f,0.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    CreateScene();

    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    InitializeCameraMotion(0.1f,0.001f);
    InitializeObjectMotion(m_spkMesh);
    return true;
}
//----------------------------------------------------------------------------
void Skinning::OnTerminate ()
{
    m_spkScene = 0;
    m_spkEffect = 0;
    m_spkMesh = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void Skinning::OnIdle ()
{
    MeasureTime();

    UpdateConstants((float)System::GetTime());

    MoveCamera();
    if ( MoveObject() )
        m_spkScene->UpdateGS();

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
void Skinning::CreateScene ()
{
    const int iRadialSamples = 10;
    const float fRadius = 10.0f;
    const float fHeight = 80.0f;

    InitializeEffect(iRadialSamples,fRadius,fHeight);

    Light* pkLight0 = new Light(Light::LT_DIRECTIONAL);
    Vector3f kDir0 = Vector3f(-1.0f,0.3f,-0.8f);
    kDir0.Normalize();
    pkLight0->SetDirection(kDir0);
    m_spkScene->SetLight(pkLight0);

    Light* pkLight1 = new Light(Light::LT_DIRECTIONAL);
    Vector3f kDir1 = Vector3f(0.4f,-1.0f,-0.5f);
    kDir1.Normalize();
    pkLight1->SetDirection(kDir1);
    m_spkScene->SetLight(pkLight1);

    AttachShader(m_spkScene);
}
//----------------------------------------------------------------------------
void Skinning::UpdateConstants (float fTime)
{
    // create some arbitrary skinning transformations
    const float fFactor = Mathf::PI/1.25f;
    int iDiv = (int)(fTime/fFactor);

    // the angle now ranges from -factor/4 to +factor/4
    float fAngle = Mathf::FAbs(fTime-iDiv*fFactor-fFactor/2.0f)-
        fFactor/4.0f;

    for (int i = 0, iM = 0; i < 4; i++)
    {
        float fThisAngle;
        if ( (int)(fTime/fFactor+0.25f)%2 )
        {
            fThisAngle = Mathf::FAbs((float)i-1.5f)*fAngle;
        }
        else
        {
            fThisAngle = ((float)i-1.5f)*fAngle;
        }

        Matrix3f kTemp(Vector3f::UNIT_Z,fThisAngle);

        // set up shader constants for matrix i
        m_spkEffect->SetSkinningMat(iM++,Vector4f(kTemp[0][0],kTemp[0][1],
            kTemp[0][2],0.0f));

        m_spkEffect->SetSkinningMat(iM++,Vector4f(kTemp[1][0],kTemp[1][1],
            kTemp[1][2],10*Mathf::Sin(fTime+0.5f*(float)i)));

        m_spkEffect->SetSkinningMat(iM++,Vector4f(kTemp[2][0],kTemp[2][1],
            kTemp[2][2],0.0f));

        m_spkEffect->SetSkinningMat(iM++,Vector4f(0.0f,0.0f,0.0f,1.0f));
    }
}
//----------------------------------------------------------------------------
void Skinning::InitializeEffect (int iRadialSamples, float fRadius, 
    float fHeight)
{
    const int iAxisSamples = 7;

    const Vector3f kCenter(0.0f,0.0f,100.0f);
    const Vector3f kU(0.0f,0.0f,-1.0f);
    const Vector3f kV(0.0f,1.0f,0.0f);
    const Vector3f kAxis(1.0f,0.0f,0.0f);

    // allocate vertices
    int iVQuantity = iAxisSamples*(iRadialSamples+1);
    Vector3f* akVertex = new Vector3f[iVQuantity];

    // allocate normals if requested
    Vector3f* akNormal = new Vector3f[iVQuantity];

    // allocate colors if requested
    ColorRGB* akColor = new ColorRGB[iVQuantity];
    memset(akColor,0,iVQuantity*sizeof(ColorRGB));

    // allocate texture coordinates for skinning weights
    Vector2f* akUV = new Vector2f[iVQuantity];
    Vector2f* akST = new Vector2f[iVQuantity];

    // allocate connectivity
    int iTQuantity = 2*(iAxisSamples-1)*iRadialSamples;
    int iIQuantity = 3*iTQuantity;
    int* aiIndex = new int[iIQuantity];

    // generate geometry
    float fInvRS = 1.0f/(float)iRadialSamples;
    float fInvASm1 = 1.0f/(float)(iAxisSamples-1);
    float fHalfHeight = 0.5f*fHeight;
    int iR, iA, iAStart, i;

    // Generate points on the unit circle to be used in computing the mesh
    // points on a cylinder slice.
    float* afSin = new float[iRadialSamples+1];
    float* afCos = new float[iRadialSamples+1];
    for (iR = 0; iR < iRadialSamples; iR++)
    {
        float fAngle = Mathf::TWO_PI*fInvRS*iR;
        afCos[iR] = Mathf::Cos(fAngle);
        afSin[iR] = Mathf::Sin(fAngle);
    }
    afSin[iRadialSamples] = afSin[0];
    afCos[iRadialSamples] = afCos[0];

    // generate the cylinder itself
    for (iA = 0, i = 0; iA < iAxisSamples; iA++)
    {
        float fAxisFraction = iA*fInvASm1;  // in [0,1]
        float fZ = -fHalfHeight + fHeight*fAxisFraction;

        // compute center of slice
        Vector3f kSliceCenter = kCenter + fZ*kAxis;

        // compute slice vertices with duplication at end point
        int iSave = i;
        for (iR = 0; iR < iRadialSamples; iR++)
        {
            Vector3f kNormal = afCos[iR]*kU + afSin[iR]*kV;
            akVertex[i] = kSliceCenter + fRadius*kNormal;
            akNormal[i] = kNormal;

            // Set texture coordinates to correct vertex weights
            if ( iA == 0 )
                akUV[i].X() = 1.0f;
            else if ( iA == 1 )
                akUV[i].X() = 0.5f;
            else
                akUV[i].X() = 0.0f;

            if ( ( iA == 1 ) || ( iA == 3 ) )
                akUV[i].Y() = 0.5f;
            else if ( iA == 2 )
                akUV[i].Y() = 1.0f;
            else
                akUV[i].Y() = 0.0f;


            if ( ( iA == 3 ) || ( iA == 5 ) )
                akST[i].X() = 0.5f;
            else if ( iA == 4 )
                akST[i].X() = 1.0f;
            else
                akST[i].X() = 0.0f;

            if ( iA == 6 )
                akST[i].Y() = 1.0f;
            else if ( iA == 5 )
                akST[i].Y() = 0.5f;
            else
                akST[i].Y() = 0.0f;

            akColor[i] = ColorRGB(fAxisFraction,1.0f-fAxisFraction,0.3f );
            i++;
        }

        akVertex[i] = akVertex[iSave];
        akNormal[i] = akNormal[iSave];

        // Set texture coordinates to correct vertex weights
        if ( iA == 0 )
            akUV[i].X() = 1.0f;
        else if ( iA == 1 )
            akUV[i].X() = 0.5f;
        else
            akUV[i].X() = 0.0f;

        if ( ( iA == 1 ) || ( iA == 3 ) )
            akUV[i].Y() = 0.5f;
        else if ( iA == 2 )
            akUV[i].Y() = 1.0f;
        else
            akUV[i].Y() = 0.0f;


        if ( ( iA == 3 ) || ( iA == 5 ) )
            akST[i].X() = 0.5f;
        else if ( iA == 4 )
            akST[i].X() = 1.0f;
        else
            akST[i].X() = 0.0f;

        if ( iA == 6 )
            akST[i].Y() = 1.0f;
        else if ( iA == 5 )
            akST[i].Y() = 0.5f;
        else
            akST[i].Y() = 0.0f;

        akColor[i] = ColorRGB(fAxisFraction,1.0f-fAxisFraction,0.3f);
        i++;
    }

    // generate connectivity
    int* aiLocalIndex = aiIndex;
    for (iA = 0, iAStart = 0; iA < iAxisSamples-1; iA++)
    {
        int i0 = iAStart;
        int i1 = i0 + 1;
        iAStart += iRadialSamples + 1;
        int i2 = iAStart;
        int i3 = i2 + 1;
        for (i = 0; i < iRadialSamples; i++, aiLocalIndex += 6)
        {
            aiLocalIndex[0] = i0;
            aiLocalIndex[1] = i1;
            aiLocalIndex[2] = i2;
            aiLocalIndex[3] = i1;
            aiLocalIndex[4] = i3;
            aiLocalIndex[5] = i2;
            i0++;
            i1++;
            i2++;
            i3++;
        }
    }

    delete[] afCos;
    delete[] afSin;

    Vector3fArray* pkVertices = new Vector3fArray(iVQuantity,akVertex);
    IntArray* pkIndices = new IntArray(3*iTQuantity,aiIndex);
    m_spkMesh = new TriMesh(pkVertices,pkIndices,false);
    m_spkMesh->Normals = new Vector3fArray(iVQuantity,akNormal);

    m_spkScene = new Node;
    m_spkScene->AttachChild(m_spkMesh);
    m_spkScene->UpdateGS();

    Vector2fArray* pkUVs = new Vector2fArray(iVQuantity,akUV);
    Vector2fArray* pkSTs = new Vector2fArray(iVQuantity,akST);
    ColorRGBArray* pkRGBs = new ColorRGBArray(iVQuantity,akColor);

    m_spkEffect = new SkinningEffect(pkUVs,pkSTs,pkRGBs);
}
//----------------------------------------------------------------------------
void Skinning::AttachShader (Spatial* pkObject)
{
    Geometry* pkGeom = DynamicCast<Geometry>(pkObject);
    if ( pkGeom )
    {
        SkinningEffect* pkClone = (SkinningEffect*)m_spkEffect->Clone();
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
