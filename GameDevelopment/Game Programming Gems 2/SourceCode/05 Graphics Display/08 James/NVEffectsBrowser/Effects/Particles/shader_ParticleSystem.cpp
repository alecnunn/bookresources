/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
        
        
******************************************************************************/

#include "eb_effect.h"
#include "nvinc.h"
#include "shader_ParticleSystem.h"
#include "ParticleSystem.h"

using namespace std;

DECLARE_EFFECT_MAIN()

extern "C"
{

__declspec(dllexport) unsigned int GetNumEffects() { return 1; }

__declspec(dllexport) EBEffect* CreateEffect(unsigned int EffectNum)
{
	return new CShaderPSystem();
}

}

CShaderPSystem::CShaderPSystem()
	: m_pParticleVB(NULL)
	, m_pFloorVB(NULL)
    , m_pFloorTexture(NULL)
    , m_pParticleTexture(NULL)
    , mNumParticles(kStartParticles)
    , mbFreezeFrame(false)
    , mbWireFrame(false)
    , mpMouseUI(NULL)

{
	m_strEffectName = "Particle System";
	m_strEffectLocation = "Vertex Shaders\\Physics";
	m_strEffectPixelShader = "";
	m_strEffectVertexShader = GetFilePath("particleSystem.nvv");
}

CShaderPSystem::~CShaderPSystem()
{
	Free();	
}

void CShaderPSystem::UpdateProperties()
{
	EBEffect::UpdateProperties();
    AddProperty(new EBProperty("Wireframe", OBJECT_MEMBER(mbWireFrame), EBTYPE_BOOL_PROP));
    AddProperty(new EBProperty("Pause", OBJECT_MEMBER(mbFreezeFrame), EBTYPE_BOOL_PROP));

	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Physics in Vertex Shader", GetFilePath("particleSystem.nvv"), EBTYPE_STRING_PROP));
}

HRESULT CShaderPSystem::Initialize(IDirect3DDevice8* pDev)
{
    ZeroMemory(&m_bKey[0], sizeof(bool) * kMaxVKey);

	HRESULT hr;

	m_pD3DDev = pDev;
	pDev->AddRef();

    //initialize mouse UI
	D3DVIEWPORT8    viewport;
	RECT            rect;

    m_pD3DDev->GetViewport(&viewport);
	rect.left   = rect.top = 0;
	rect.bottom = viewport.Height;
	rect.right  = viewport.Width;
	mpMouseUI   = new MouseUI(rect, true);
    mpMouseUI->SetRotationalSensitivityFactor(1.0f);

    // create the floor quadrangle
    hr = m_pD3DDev->CreateVertexBuffer( 4 * sizeof(FloorVertex), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_pFloorVB);
	if (FAILED(hr))
		return hr;

	vector<DWORD> floorDecl;
	floorDecl.push_back(D3DVSD_STREAM(0));
	floorDecl.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT4));
	floorDecl.push_back(D3DVSD_REG(1, D3DVSDT_FLOAT2));
	floorDecl.push_back(D3DVSD_END());
	hr = LoadAndCreateShader(GetFilePath("particleFloor.vso"), &floorDecl[0], 0, SHADERTYPE_VERTEX, &mFloorShader);
	if (FAILED(hr))
		return hr;
    
    FloorVertex* pFloor;

    assert(m_pFloorVB != NULL);
	hr = m_pFloorVB->Lock(0, 4 * sizeof(FloorVertex),(BYTE**)&pFloor, 0);
	if (FAILED(hr))
	{
		m_strLastError = "Couldn't lock Floor buffer!";
		return hr;
	}
    float const  kFloorDimension = 101.0f;

    pFloor->position = D3DXVECTOR4(-kFloorDimension, 0.0f, -kFloorDimension, 1.0f);
    pFloor->texture  = D3DXVECTOR2(0.0f, 0.0f);
    ++pFloor;
    pFloor->position = D3DXVECTOR4( kFloorDimension, 0.0f, -kFloorDimension, 1.0f);
    pFloor->texture  = D3DXVECTOR2(0.0f, kFloorDimension);
    ++pFloor;
    pFloor->position = D3DXVECTOR4(-kFloorDimension, 0.0f, kFloorDimension, 1.0f);
    pFloor->texture  = D3DXVECTOR2(kFloorDimension, 0.0f);
    ++pFloor;
    pFloor->position = D3DXVECTOR4( kFloorDimension, 0.0f, kFloorDimension, 1.0f);
    pFloor->texture  = D3DXVECTOR2(kFloorDimension, kFloorDimension);
    ++pFloor;

    m_pFloorVB->Unlock();

	hr = D3DXCreateTextureFromFileEx(m_pD3DDev, 
		GetFilePath("wall.dds").c_str(),
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		0,
		0,
		D3DFMT_UNKNOWN,
		D3DPOOL_MANAGED,
		D3DX_FILTER_LINEAR,
		D3DX_FILTER_LINEAR,
		0,
		NULL,
		NULL,
		&m_pFloorTexture);
	if (FAILED(hr))
	{
		m_strLastError = "Could not load texture wall.dds";
		return hr;
	}


    // create the particles
    hr = m_pD3DDev->CreateVertexBuffer( kNumParticles * sizeof(PSystemVertex), D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_pParticleVB);
	if (FAILED(hr))
		return hr;

	vector<DWORD> Declaration;
	Declaration.push_back(D3DVSD_STREAM(0));
	Declaration.push_back(D3DVSD_REG(0, D3DVSDT_FLOAT4));
	Declaration.push_back(D3DVSD_REG(1, D3DVSDT_FLOAT4));
	Declaration.push_back(D3DVSD_REG(2, D3DVSDT_FLOAT3));
	Declaration.push_back(D3DVSD_END());
	hr = LoadAndCreateShader(GetFilePath("particleSystem.vso"), &Declaration[0], 0, SHADERTYPE_VERTEX, &mParticleShader);
	if (FAILED(hr))
		return hr;

	PSystemVertex* pBuff;

    assert(m_pParticleVB != NULL);
	hr = m_pParticleVB->Lock(0, kNumParticles * sizeof(PSystemVertex),(BYTE**)&pBuff, 0);
	if (FAILED(hr))
	{
		m_strLastError = "Couldn't lock buffer!";
		return hr;
	}

    D3DXVECTOR4 timeSeeds;
    D3DXVECTOR4 physics;
    D3DXVECTOR3 color;

    _ftime(&mStartTime);
    srand( (unsigned)mStartTime.time );

    float const     kRandMax  = static_cast<float>(RAND_MAX);
    float const     kIAOver2  = 16807.0f/2.0f;

    // all following times are in seconds
    float   creationTime      =  0.0f;    // used to figure out how far along a particle is in its lifetime
    float   lifeTime          =  4.0f;    // life time of a particle.  A particle becomes invisible at the end of its life time
    float   periodicityOffset =  0.3f;    // a particle is reborn (reused) every (lifeTime+periodicityOffset)
    float   decay             = -1.0f;    // 2^decay = Eps, w/ Eps < 1, but close to 1, ie, -1 < decay < 0;
    float   velocity          =  3.0f;    // initial velocity of particle (direction vector multiplier)
    float   random;

    // generate and copy particle values
    for (int i = kNumParticles; --i >= 0; )
    {
        timeSeeds.x   = creationTime;
        creationTime += 2.0f*lifeTime/static_cast<float>(kNumParticles);

        random = lifeTime + 4.0f * static_cast<float>(rand())/kRandMax;
        timeSeeds.y = 1.0f/random;                
        timeSeeds.z = random + periodicityOffset * static_cast<float>(rand())/kRandMax;
        timeSeeds.w = 1.0f/(timeSeeds.z);
    
        physics.x   = decay;              // could be a random variation 
        physics.y   = 1.0f/(physics.x*logf(2.0f));
        random      = velocity + 2.0f * static_cast<float>(rand())/kRandMax;
        physics.z   = random;           
        physics.w   = kIAOver2 * static_cast<float>(rand())/kRandMax;   // per vertex random-seed (to be added to constant seed)

        random = static_cast<float>(rand())/kRandMax;
        color.x = 0.9f + 0.1f * random;
        random = static_cast<float>(rand())/kRandMax;
        color.y = 0.6f + 0.4f * random;
        random = static_cast<float>(rand())/kRandMax;
        color.z = 0.0f + 0.25f * random;

        pBuff->timeSeeds = timeSeeds;
	    pBuff->physics   = physics;
	    pBuff->color     = color;
	    ++pBuff;
    }
	m_pParticleVB->Unlock();

    // read a texture for the particle
	hr = D3DXCreateTextureFromFileEx(m_pD3DDev, 
		GetFilePath("flare1.dds").c_str(),
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		0,
		0,
		D3DFMT_UNKNOWN,
		D3DPOOL_MANAGED,
		D3DX_FILTER_LINEAR,
		D3DX_FILTER_LINEAR,
		0,
		NULL,
		NULL,
		&m_pParticleTexture);
	if (FAILED(hr))
	{
		m_strLastError = "Could not load texture flare1.dds";
		return hr;
	}

    m_pD3DDev->SetTexture(0, NULL);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    m_pD3DDev->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );


    m_pD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_pD3DDev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	m_pD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    // going to set real fog color in Tick()
    m_pD3DDev->SetRenderState( D3DRS_FOGCOLOR,  D3DCOLOR_RGBA(0, 0, 0, 0));
	m_pD3DDev->SetRenderState( D3DRS_FOGENABLE,      TRUE );
    m_pD3DDev->SetRenderState( D3DRS_FOGTABLEMODE,   D3DFOG_NONE );
    m_pD3DDev->SetRenderState( D3DRS_FOGVERTEXMODE,  D3DFOG_NONE );
    m_pD3DDev->SetRenderState( D3DRS_RANGEFOGENABLE, FALSE );
    
    m_pD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, true);

    // add alpha's straight to achieve a "burn-out" effect
    m_pD3DDev->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
    m_pD3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE); 

    m_pD3DDev->SetRenderState(D3DRS_ZWRITEENABLE, true);

    // we actually override point-size in the vertex shader
    float const kMinPointSize =  0.1f;
    float const kMaxPointSize = 25.0f;
    float const kPointSize    =  4.0f;

    m_pD3DDev->SetRenderState(D3DRS_POINTSIZE_MIN, *reinterpret_cast<DWORD const *>(&kMinPointSize) ); 
    m_pD3DDev->SetRenderState(D3DRS_POINTSIZE_MAX, *reinterpret_cast<DWORD const *>(&kMaxPointSize) );
    m_pD3DDev->SetRenderState(D3DRS_POINTSIZE,     *reinterpret_cast<DWORD const *>(&kPointSize) );

    m_pD3DDev->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	m_pD3DDev->SetRenderState(D3DRS_POINTSCALEENABLE,  false); 

    // write the non-changing vertex shader constants
    WriteVertexShaderConstants();

    // initialize the camera controls
    mpMouseUI->Reset();

	return S_OK;
}

void CShaderPSystem::WriteVertexShaderConstants() const
{
    // All of the following are write-once!

    D3DXVECTOR4     constants(0.0f, 2.0f, D3DX_PI, 1.0f/3.0f);
	m_pD3DDev->SetVertexShaderConstant(VSC_CONSTANTS,      &constants, 1);

    D3DXVECTOR4     constants2(1.0f/2.0f, 0.25f, 80.0f, 1.0f / (2.0f * D3DX_PI));
	m_pD3DDev->SetVertexShaderConstant(VSC_CONSTANTS2,     &constants2, 1);

    D3DXVECTOR4     cosine(1.0f, -1.0f / 2.0f, 1.0f / 24.0f, -1.0f / 720.0f);
    m_pD3DDev->SetVertexShaderConstant(VSC_COSINE,         &cosine, 1);

    D3DXVECTOR4     sine(1.0f, -1.0f / 6.0f, 1.0f / 120.0f, -1.0f / 5040.0f);
    m_pD3DDev->SetVertexShaderConstant(VSC_SINE,           &sine, 1);


    D3DXVECTOR4     rndVariance(1.0f, 1.0f, 2.0f, 0.3f);
    m_pD3DDev->SetVertexShaderConstant(VSC_RND_VARIANCE, &rndVariance, 1);

    float const     kNumRnds = static_cast<float>(kNumRndVectors);
    D3DXVECTOR4     rndNumVectors(kNumRnds, 1.0f/kNumRnds, 0.0f, 0.0f);
    m_pD3DDev->SetVertexShaderConstant(VSC_RND_NUM_VECTORS, &rndNumVectors, 1);

    float       x, y, z, w;
    float const kRandMax = static_cast<float>(RAND_MAX);
    float const kIAOver2 = 16807.0f/2.0f;

    struct _timeb   currentTime;
    _ftime(&currentTime);
    srand( (unsigned)currentTime.time );
    
    for (int i = 0; i < kNumRndVectors; ++i)
    {
        x = kIAOver2 * static_cast<float>(rand())/kRandMax;
        y = kIAOver2 * static_cast<float>(rand())/kRandMax;
        z = kIAOver2 * static_cast<float>(rand())/kRandMax;
        w = kIAOver2 * static_cast<float>(rand())/kRandMax;
        D3DXVECTOR4 rndVector(x, y, z, w);
        m_pD3DDev->SetVertexShaderConstant(VSC_RND_START + i, &rndVector, 1);
    }

    // particle start, direction, and perpendicular direction
	D3DXVECTOR4 startPosition(-0.0f, 0.0f, 0.0f, 1.0f);
    m_pD3DDev->SetVertexShaderConstant(VSC_START_POSITION, &startPosition, 1);

    D3DXVECTOR3        direction(-0.2f, 1.0f, 0.0f);
    D3DXVec3Normalize(&direction, &direction);
	m_pD3DDev->SetVertexShaderConstant(VSC_DIRECTION,      &direction, 1);

    D3DXVECTOR3        dirPerp1(0.0f, 0.0f, 1.0f);
    D3DXVec3Cross(    &dirPerp1, &dirPerp1, &direction);
    D3DXVec3Normalize(&dirPerp1, &dirPerp1);
    m_pD3DDev->SetVertexShaderConstant(VSC_DIR_PERPENDICULAR1, &dirPerp1, 1);

    D3DXVECTOR3        dirPerp2(0.0f, 0.0f, 0.0f);
    D3DXVec3Cross(    &dirPerp2, &dirPerp1, &direction);
    D3DXVec3Normalize(&dirPerp2, &dirPerp2);
    m_pD3DDev->SetVertexShaderConstant(VSC_DIR_PERPENDICULAR2, &dirPerp2, 1);

    D3DXVECTOR4 halfGravity(0.0f, -0.9f, 0.0f, 0.0f);
	m_pD3DDev->SetVertexShaderConstant(VSC_HALF_GRAVITY,   &halfGravity, 1);
}

HRESULT CShaderPSystem::Free()
{
	SAFE_RELEASE(m_pParticleVB);
	SAFE_RELEASE(m_pFloorVB);

	SAFE_RELEASE(m_pFloorTexture);
	SAFE_RELEASE(m_pParticleTexture);

	if (m_pD3DDev)
	{
		m_pD3DDev->DeleteVertexShader(mFloorShader);
		m_pD3DDev->DeleteVertexShader(mParticleShader);
		SAFE_RELEASE(m_pD3DDev);
	}
    SAFE_DELETE(mpMouseUI);

	return S_OK;
}

HRESULT CShaderPSystem::Start()
{
	return S_OK;
}

HRESULT CShaderPSystem::Tick(EBTimer* pTimer)
{
    HRESULT hr = S_OK;
   
	D3DXMATRIX matWorld;
	D3DXMATRIX matView;
	D3DXMATRIX matProj;
	D3DXMATRIX matWorldViewProj;

    DWORD   const kSmokeColor = D3DCOLOR_RGBA(125, 105, 95, 0);
    DWORD   const kSkyColor   = D3DCOLOR_RGBA( 70,  80, 65, 0);

    // clear the backbuffer
    hr = m_pD3DDev->SetRenderState(D3DRS_FILLMODE, (mbWireFrame) ? D3DFILL_WIREFRAME : D3DFILL_SOLID);
	hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, kSkyColor, 1.0, 0);

    // set up the camera/projection

	// Setup the world and projection matrices
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(60.0f), 1.0f, 0.01f, 200.0f);

    // update the view matrix from the keyboard
    SetViewFromKeyboardInput(pTimer, matView);
    
	// Setup the composite matrix to get vertices to projection space
	D3DXMatrixMultiply( &matWorldViewProj, &matWorld, &matView);
	D3DXMatrixMultiply( &matWorldViewProj, &matWorldViewProj, &matProj);	
	D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);

	// write the matrix to the vertex shader constants
    m_pD3DDev->SetVertexShaderConstant(VSC_WORLDVIEW_PROJ_0, &matWorldViewProj(0, 0), 4);

         
    // update current time
    struct _timeb   now;
    _ftime(&now);

    float kDifference(static_cast<float>(now.time - mStartTime.time));
    kDifference += static_cast<float>(0.001f * (now.millitm - mStartTime.millitm));
    kDifference += 10000.0f;    // fudge it to randoomize particles more

    D3DXVECTOR4 currentTime(kDifference, kDifference, kDifference, kDifference);

    // but only if we are not freezing the current frame
    if (! mbFreezeFrame)
        m_pD3DDev->SetVertexShaderConstant(VSC_CURRENT_TIME,   &currentTime, 1);

    // done writing constants, just draw the VBs now
    // draw the floor first!    
    
    // all color comes from texture, no alpha at all
    m_pD3DDev->SetRenderState(D3DRS_FOGENABLE, false );
    m_pD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
    m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
    m_pD3DDev->SetRenderState(D3DRS_ZWRITEENABLE, true);   

    hr = m_pD3DDev->SetStreamSource(0, m_pFloorVB, sizeof(FloorVertex));
	hr = m_pD3DDev->SetVertexShader( mFloorShader );
    hr = m_pD3DDev->SetTexture(0, m_pFloorTexture);
	
    hr = m_pD3DDev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	if (FAILED(hr))
		return hr;

    // now the particles 
    // all color comes from diffuse of vertex
    m_pD3DDev->SetRenderState(D3DRS_FOGENABLE, true );
    m_pD3DDev->SetRenderState(D3DRS_FOGCOLOR,  kSmokeColor);
    m_pD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2);
    // particles have alpha, but are not (cannot be) depth-sorted.  Thus, disable z-writing 
    // to avoid funny alpha-occlusion
    m_pD3DDev->SetRenderState(D3DRS_ZWRITEENABLE, false);   
    
    hr = m_pD3DDev->SetStreamSource(0, m_pParticleVB, sizeof(PSystemVertex));
	hr = m_pD3DDev->SetVertexShader(mParticleShader);
    hr = m_pD3DDev->SetTexture(0, m_pParticleTexture);
    
    assert(10 < kNumParticles);
    hr = m_pD3DDev->DrawPrimitive(D3DPT_POINTLIST, 0, mNumParticles);

	return hr;
}

void CShaderPSystem::Keyboard(DWORD vKey, UINT nFlags, bool bDown)
{
	eEBKeyAction Action = TranslateEffectKey(vKey, nFlags, bDown);
	
    switch ( Action )
    {
		case EB_HELP:
		{
			::MessageBoxEx( NULL, " Help : F1 - Help \n\n Home - Reset To Defaults \n\n W - Wireframe Toggle \n\n Space/Pause Toggle Pausing Animation\n\n +/- - Increase/Reduce Number of Particles\n\n Left Button & Mouse - Rotate Camera\n\n Shift Left Button & Mouse - Pan Camera \n\n Ctrl Left Button & Mouse - Move Camera In & Out\n\n",
				   "Help", MB_ICONINFORMATION | MB_TASKMODAL, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ) );
		}
		break;

		case EB_WIREFRAME:
        {
            mbWireFrame = !mbWireFrame;
            m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
        }
		break;

        case EB_PAUSE:
        {
            mbFreezeFrame = !mbFreezeFrame;
            m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
        }
        break;

		case EB_RESET:
        {
            mbFreezeFrame = false;
            mNumParticles = kStartParticles;
            m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
            mpMouseUI->Reset();
        }
		break;

        case EB_ADD:
            mNumParticles += max(5, mNumParticles/10);
            if (mNumParticles > kNumParticles)
                mNumParticles = kNumParticles;
            break;

        case EB_SUBTRACT:
            mNumParticles -= max(5, mNumParticles/10);
            if (mNumParticles < 10)
                mNumParticles = 10;
            break;

        default:
            m_bKey[vKey] = bDown;
    }
}

void CShaderPSystem::SetViewFromKeyboardInput(EBTimer* pTimer, D3DXMATRIX &matView)
{
    float   const   kSpeed              = pTimer->GetSecsPerFrame() * 10.0f;
    float   const   moveSpeed           = kSpeed * 2.0f;

    D3DXVECTOR3 const   yVector(0.0f, 1.0f, 0.0f);
    D3DXVECTOR3         lookDir(0.0f, 0.0f, 1.0f);    
    D3DXVECTOR4         position;
    D3DXMATRIX          matLookRotations;

    D3DXMatrixTranspose(&matLookRotations, &(mpMouseUI->GetRotationMatrix()));
    D3DXVec3TransformNormal(&lookDir, &lookDir, &matLookRotations);

    float       const   h = D3DXVec3Dot(&yVector, &lookDir);
    D3DXVECTOR3         xzProjectedLookDirection(lookDir - h*yVector);

    D3DXVec3Normalize(&xzProjectedLookDirection, &xzProjectedLookDirection);
    
    D3DXVECTOR3         rightVector;         
    D3DXVec3Cross(&rightVector, &yVector, &xzProjectedLookDirection);

    if(m_bKey[VK_UP]    || m_bKey[VK_NUMPAD8])
        mpMouseUI->Translate(  moveSpeed*xzProjectedLookDirection);
	if(m_bKey[VK_DOWN]  || m_bKey[VK_NUMPAD2])
        mpMouseUI->Translate(- moveSpeed*xzProjectedLookDirection);
	if(m_bKey[VK_LEFT]  || m_bKey[VK_NUMPAD4])
        mpMouseUI->Translate(- 0.5f*moveSpeed*rightVector);
	if(m_bKey[VK_RIGHT] || m_bKey[VK_NUMPAD6])              
        mpMouseUI->Translate(  0.5f*moveSpeed*rightVector);
	if(m_bKey[VK_PRIOR])
        mpMouseUI->Translate(  0.25f*moveSpeed*yVector);
	if(m_bKey[VK_NEXT])
        mpMouseUI->Translate(- 0.25f*moveSpeed*yVector);
    

    D3DXVec3Transform(&position, &D3DXVECTOR3(0.0f, 1.0f, -5.0f), &mpMouseUI->GetTranslationMatrix());

    D3DXVECTOR3 const   camPosition = D3DXVECTOR3(position.x, max(0.1f, position.y), position.z);
    D3DXVECTOR3 const   vLookatPt   = camPosition + lookDir;         

    D3DXMatrixLookAtLH(&matView, &camPosition, &vLookatPt, &yVector);
}

HRESULT CShaderPSystem::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
{
    // check vertex shading support
    if(D3DSHADER_VERSION_MAJOR(pCaps->VertexShaderVersion) < 1)
	{
		m_strLastError = "Device does not support vertex shaders!";
		return E_FAIL;
	}
    if(D3DSHADER_VERSION_MINOR(pCaps->VertexShaderVersion) < 1)
	{
		m_strLastError = "Device does not support 1.1 vertex shaders!";
		return E_FAIL;
	}

    // check simultaneous texture support (only need 1...)
    if(pCaps->MaxSimultaneousTextures < 1)
	{
		m_strLastError = "Device does not support 1 simultaneous textures!";
		return E_FAIL;
	}

    // only need very few, simple register combiners...
	if(pCaps->MaxTextureBlendStages < 2)
	{
		m_strLastError = "Device does not support 2 register combiners!";
		return E_FAIL;
	}

    // need mipmapping 
	if ((pCaps->TextureCaps & D3DPTEXTURECAPS_MIPMAP) == 0)
	{
		m_strLastError = "Device does not support mip-mapping!";
		return E_FAIL;
	}
    // and trilinear filtering between mipmaps
	if ((pCaps->TextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR) == 0)
	{
		m_strLastError = "Device does not support trilinear mip-mapping!";
		return E_FAIL;
	}

    // point primitives?!
	if (pCaps->MaxPointSize == 1.0f)
	{
		m_strLastError = "Device does not support point-sprites!";
		return E_FAIL;
	}

	return S_OK;
}

void CShaderPSystem::MouseMove(HWND hWnd, int x, int y)
{
    mpMouseUI->OnMouseMove(x, y);
}

void CShaderPSystem::MouseButton(HWND hWnd, eButtonID button, bool bDown, int x, int y)
{
	if(button == MOUSE_LEFTBUTTON)
	{
		if(bDown)
			mpMouseUI->OnLButtonDown(x, y);
		else
			mpMouseUI->OnLButtonUp(x, y);
	}
}