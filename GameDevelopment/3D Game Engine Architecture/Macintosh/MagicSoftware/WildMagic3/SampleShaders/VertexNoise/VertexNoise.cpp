// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "VertexNoise.h"

WM3_WINDOW_APPLICATION(VertexNoise);

//----------------------------------------------------------------------------
VertexNoise::VertexNoise ()
    :
    WindowApplication3("VertexNoise",0,0,640,480,ColorRGB(0.5f,0.0f,1.0f))
{

}
//----------------------------------------------------------------------------
bool VertexNoise::OnInitialize ()
{
    if ( !WindowApplication3::OnInitialize() )
        return false;

    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1000.0f);
    Vector3f kCLoc(0.0f,-200.0f,0.0f);
    Vector3f kCDir(0.0f,1.0f,0.0f);
    Vector3f kCUp(0.0f,0.0f,1.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    CreateScene();

    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    InitializeCameraMotion(0.1f,0.001f);
    InitializeObjectMotion(m_spkScene);
    return true;
}
//----------------------------------------------------------------------------
void VertexNoise::OnTerminate ()
{
    m_spkScene = 0;
    m_spkEffect = 0;
    m_spkWireframe = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void VertexNoise::OnIdle ()
{
    MeasureTime();

    m_kNoiseTrans[1] = (float)System::GetTime()*2.0f;
    UpdateConstants();

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
bool VertexNoise::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if ( WindowApplication3::OnKeyDown(ucKey,iX,iY) )
        return true;

    switch ( ucKey )
    {
    case '+':
        m_fNoiseScale += 0.2f;
        return true;
    case '-':
        m_fNoiseScale -= 0.2f;
        return true;
    case '1':
        m_fDisplacement += 0.5f;
        return true;
    case '2':
        m_fDisplacement -= 0.5f;
        return true;
    case 'R':
    case 'r':
        m_fNoiseScale = 1.0f;
        m_kNoiseTrans[0] = 0.0f;
        m_kNoiseTrans[1] = 0.0f;
        m_kNoiseTrans[2] = 0.0f;
        m_kNoiseTrans[3] = 0.0f;
        m_fDisplacement = 8.0f;
        return true;
    case 'W':
    case 'w':
        m_spkWireframe->Enabled = !m_spkWireframe->Enabled;
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void VertexNoise::CreateScene ()
{
    Stream kStream;
    bool bLoaded = kStream.Load(System::GetPath("../../Data/Wmof/Face.wmof"));
    assert( bLoaded );
    m_spkScene = (Node*)kStream.GetObjectAt(0);

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

    m_spkEffect = new VertexNoiseEffect();
    InitVertexNoiseConstants();

    AttachShader(m_spkScene);

    m_spkWireframe = new WireframeState;
    m_spkScene->SetGlobalState(m_spkWireframe);
}
//----------------------------------------------------------------------------
void VertexNoise::InitVertexNoiseConstants()
{
    const int TABLE_SIZE = 32;

	int* aiP = new int[TABLE_SIZE*2+2];  // permutation table
    Vector4f* akG = new Vector4f[TABLE_SIZE*2+2];
	int i;

    // initalize random gradients
	for (i = 0; i < TABLE_SIZE; i++)
    {
		aiP[i] = i;
        akG[i][0] = Mathf::SymmetricRandom();
		akG[i][1] = Mathf::SymmetricRandom();
		akG[i][2] = Mathf::SymmetricRandom();
        akG[i][3] = 0.0f;
        akG[i].Normalize();
	}

	// initialize permutation table (random shuffle)
	for (i = 0; i < TABLE_SIZE; i++)
    {
		int j, t;
		j = (rand() >> 4) % TABLE_SIZE;
		t = aiP[i];
		aiP[i] = aiP[j];
		aiP[j] = t;
        akG[i][3] = (float)aiP[i];
    }

    for (i = 0; i < TABLE_SIZE+2; i++)
    {
        // mirror first half of table into second half (+2)
        akG[i+TABLE_SIZE][0] = akG[i%TABLE_SIZE][0];
        akG[i+TABLE_SIZE][1] = akG[i%TABLE_SIZE][1];
        akG[i+TABLE_SIZE][2] = akG[i%TABLE_SIZE][2];
        akG[i+TABLE_SIZE][3] = akG[i%TABLE_SIZE][3];
	}

    for (i = 0; i < TABLE_SIZE*2+2; i++)
    {
        m_spkEffect->SetPg(i,Vector4f(akG[i][0],akG[i][1],akG[i][2],
            akG[i][3]));
    }

    m_fDisplacement = 8.0f;
    m_kNoiseTrans = Vector4f::ZERO;
    m_fNoiseScale = 1.0f;
    UpdateConstants();

    m_spkEffect->SetBaseColor(Vector4f(0.99607f,0.8392f,0.67059f,1.0f));

    delete[] aiP;
    delete[] akG;
}
//----------------------------------------------------------------------------
void VertexNoise::UpdateConstants ()
{
    m_spkEffect->SetNoiseTranslate(m_kNoiseTrans);
    m_spkEffect->SetNoiseScale(m_fNoiseScale);
    m_spkEffect->SetDisplacement(m_fDisplacement);
}
//----------------------------------------------------------------------------
void VertexNoise::AttachShader (Spatial* pkObject)
{
    Geometry* pkGeom = DynamicCast<Geometry>(pkObject);
    if ( pkGeom )
    {
        VertexNoiseEffect* pkClone = (VertexNoiseEffect*)m_spkEffect->Clone();
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
