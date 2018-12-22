/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
        
******************************************************************************/

#include "eb_effect.h"
#include "nvmesh.h"
#include "nvdevice.h"
#include "shader_MotionBlur.h"
#include "MotionBlur.h"

using namespace nv_objects;
using namespace std;

DECLARE_EFFECT_MAIN()

extern "C"
{

__declspec(dllexport) unsigned int GetNumEffects() { return 1; }

__declspec(dllexport) EBEffect* CreateEffect(unsigned int EffectNum)
{
	return new CShaderMotionBlur();
}

}

void CShaderMotionBlur::UpdateProperties()
{
	EBEffect::UpdateProperties();
    AddProperty(new EBProperty("Wireframe", OBJECT_MEMBER(mbWireFrame), EBTYPE_BOOL_PROP));

    EBEnumProperty* pEnumProp = new EBEnumProperty("Blur Length Options", OBJECT_MEMBER(meBlurLengthOption), EBTYPE_DWORD_PROP);
	
	pEnumProp->AddEnumerant(new EBEnumValue(pEnumProp, "0.5x Blur Length", (DWORD)HALFxBLURLENGTH,   EBTYPE_DWORD_PROP));
	pEnumProp->AddEnumerant(new EBEnumValue(pEnumProp, "1.0x Blur Length", (DWORD)ONExBLURLENGTH,    EBTYPE_DWORD_PROP));
	pEnumProp->AddEnumerant(new EBEnumValue(pEnumProp, "1.5x Blur Length", (DWORD)ONEHALFxBLURLENGTH,EBTYPE_DWORD_PROP));
	AddProperty(pEnumProp);

	AddProperty(new EBProperty("Enable Motion Blur", OBJECT_MEMBER(m_bEnableMotionBlur), EBTYPE_BOOL_PROP));
    AddProperty(new EBProperty("Speed  2x", OBJECT_MEMBER(m_bEnableSpeed2x), EBTYPE_BOOL_PROP));
    AddProperty(new EBProperty("Pause", OBJECT_MEMBER(m_bPause), EBTYPE_BOOL_PROP));

	m_pVertexShaderEnum->AddEnumerant(new EBEnumValue(m_pVertexShaderEnum, "Motion Blur", GetFilePath("MotionBlur.nvv"), EBTYPE_STRING_PROP));
}

CShaderMotionBlur::CShaderMotionBlur()
    : m_pNVDevice(NULL)
	, m_pFloorMesh(NULL)
	, m_pBlurredObjectMesh(NULL)
	, m_pBlurredObjectMap(NULL)
	, m_pBlurredObjectVB(NULL)
    , m_bEnableMotionBlur( true )
    , m_bEnableSpeed2x(   false )
    , m_bPause(           false )
    , meBlurLengthOption( ONExBLURLENGTH )
    , mbWireFrame       ( false )
{
    for (int i = 6; --i >= 0; )
	{
	    m_pBlurredObjectIB[i] = NULL;
	}

	m_strEffectName = "Motion Blur";
	m_strEffectLocation = "Vertex Shaders\\Deformation";
	m_strEffectPixelShader = "";
	m_strEffectVertexShader = GetFilePath("MotionBlur.nvv");
}

CShaderMotionBlur::~CShaderMotionBlur()
{
	Free();	
}

#pragma optimize ("", off)
HRESULT CShaderMotionBlur::Initialize(IDirect3DDevice8* pDev)
{
	HRESULT     hr;

	m_pD3DDev = pDev;
	pDev->AddRef();

	m_pNVDevice = new NVMotionBlurDevice(pDev, this);

	DWORD        dwVBFlags = D3DUSAGE_WRITEONLY;
	LPD3DXBUFFER pCode = NULL;

    DWORD declaration[] = 
    {
        D3DVSD_STREAM(0),
        D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3),
        D3DVSD_REG(D3DVSDE_NORMAL,   D3DVSDT_FLOAT3),
        D3DVSD_REG(D3DVSDE_TEXCOORD0,D3DVSDT_FLOAT2),
        D3DVSD_END()
    };

	m_dwMotionBlurShader = 0;
	hr = LoadAndCreateShader(GetFilePath("MotionBlur.vso"), &declaration[0], 0, SHADERTYPE_VERTEX, &m_dwMotionBlurShader);
	if (FAILED(hr))
		return hr;


	std::string const objectNameToLoad("tiger");

    // Load the blurred object x file and extract its mesh
    m_pBlurredObjectMesh = new NVMesh();
    hr = m_pBlurredObjectMesh->Create(m_pNVDevice, GetFilePath(objectNameToLoad + ".x"));
	if (FAILED(hr))
	{
		m_strLastError = "Could not load " + GetFilePath(objectNameToLoad + ".x");
		return hr;
	}
	m_pBlurredObjectMesh->SetFVF(m_pNVDevice, MOTIONBLURVERTEX_FVF);
	m_pBlurredObjectMesh->SetVertexShader(m_dwMotionBlurShader);
    m_pBlurredObjectMesh->RestoreDeviceObjects(m_pNVDevice);

	hr = D3DXCreateTextureFromFileEx(m_pD3DDev, 
		GetFilePath(objectNameToLoad + ".dds").c_str(),
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
		&m_pBlurredObjectMap);
	if (FAILED(hr))
	{
		m_strLastError = "Could not load texture " + GetFilePath(objectNameToLoad + ".dds");
		return hr;
	}

    // Compute a (somewhat)-minimum radius sphere around the object 
    // and store that diameter in the vertex shader constant area.
    FLOAT            radius;
    FLOAT            maxRadius = 0.0f;
	int   const      kStride   = D3DXGetFVFVertexSize(m_pBlurredObjectMesh->GetSysMemMesh()->GetFVF());

	D3DXVECTOR3     *pVertices;
    
    m_numVertices = m_pBlurredObjectMesh->GetSysMemMesh()->GetNumVertices();

    m_pBlurredObjectMesh->GetSysMemMesh()->GetVertexBuffer( &m_pBlurredObjectVB );   
	m_pBlurredObjectVB->Lock( 0, 0, (BYTE**)&pVertices, 0 );

    int i;
    for (i = m_numVertices; --i >= 0; )
    {
	    FLOAT x = pVertices->x;
		FLOAT y = pVertices->y;
		FLOAT z = pVertices->z;
		radius  = sqrtf( x*x + y*y + z*z );
        if (maxRadius < radius)
            maxRadius = radius;
		pVertices = (D3DXVECTOR3*) (((BYTE*)pVertices) + kStride);
	}
	m_pBlurredObjectVB->Unlock();

    FLOAT const       kFudge = 2.0f;
    m_OneOverExtent = kFudge/maxRadius;

    IDirect3DIndexBuffer8*  pIB;
    D3DINDEXBUFFER_DESC     ddsdDescIB;
	
    m_pBlurredObjectMesh->GetSysMemMesh()->GetIndexBuffer( &pIB );
	pIB->GetDesc(&ddsdDescIB);

    switch (ddsdDescIB.Format)
	{
		case D3DFMT_INDEX16:
			m_numIndices = ddsdDescIB.Size / 2;
			break;
		case D3DFMT_INDEX32:
			m_numIndices = ddsdDescIB.Size / 4; 
            break;
        default:
            m_numIndices = 0;
            assert(false);
            break;
	}
    
    // copy the indices into my private index buffers
    BYTE*           pSrc;
	BYTE*           pDest;
	D3DXVECTOR3    *pVector[6];
    WORD            index[6];
    FLOAT           curAvgX,  curAvgY,  curAvgZ;
    FLOAT           nextAvgX, nextAvgY, nextAvgZ;
    int             j, k;

    pIB->Lock(0, ddsdDescIB.Size, &pSrc, D3DLOCK_READONLY | D3DLOCK_NOSYSLOCK );
	m_pBlurredObjectVB->Lock( 0, 0, (BYTE**)&pVertices, 0 );

    for (i = 6; --i >= 0; )
    {
        hr = pDev->CreateIndexBuffer(ddsdDescIB.Size, 0, ddsdDescIB.Format, 
                                     D3DPOOL_DEFAULT, &(m_pBlurredObjectIB[i]) );
        assert( ! FAILED(hr) );
        m_pBlurredObjectIB[i]->Lock(0, ddsdDescIB.Size, &pDest, D3DLOCK_NOSYSLOCK );
		memcpy(pDest, pSrc, ddsdDescIB.Size);

        // sort the indices so that triangles are ordered in in/decreasing x/y/z
        // I could potentially also pre-backface cull all triangles with 
        // Normal dot fromCam > 1/sqrt(2.0) for each of the IndexBuffers.
        // Some other time maybe.        
        bool bSorted = false;
        while (! bSorted)
        {
            bSorted = true;
            for (j = 0; j < m_numIndices-3; j += 3 )
            {
                for (k = 6; --k >= 0; )
                {
                    index[k]   = *((WORD *) (pDest + 2*(j+k)));
                    pVector[k] = (D3DXVECTOR3*) (((BYTE*)pVertices) + index[k] * kStride);
                }    

                curAvgX  = (pVector[0]->x + pVector[1]->x + pVector[2]->x);
                nextAvgX = (pVector[3]->x + pVector[4]->x + pVector[5]->x);
                curAvgY  = (pVector[0]->y + pVector[1]->y + pVector[2]->y);
                nextAvgY = (pVector[3]->y + pVector[4]->y + pVector[5]->y);
                curAvgZ  = (pVector[0]->z + pVector[1]->z + pVector[2]->z);
                nextAvgZ = (pVector[3]->z + pVector[4]->z + pVector[5]->z);

                bool bSwap;

                switch (i)
                {
                    case 0:
                        bSwap = (curAvgX > nextAvgX);   // fromCam = -x
                        break;
                    case 1:
                        bSwap = (curAvgX < nextAvgX);   // fromCam = +x 
                        break;
                    case 2:
                        bSwap = (curAvgY > nextAvgY);   // fromCam = -y
                        break;
                    case 3:
                        bSwap = (curAvgY < nextAvgY);   // fromCam = +y
                        break;
                    case 4:
                        bSwap = (curAvgZ > nextAvgZ);   // fromCam = -z
                        break;
                    case 5:
                        bSwap = (curAvgZ < nextAvgZ);   // fromCam = +z
                        break;
                }

                if (bSwap)
                {
                    bSorted = false;

                    // swap triangles by swapping all three indices
                    for (k = 3; --k >= 0; )
                    {
                        *((WORD *) (pDest + 2*(j+k  ))) = index[k+3];
                        *((WORD *) (pDest + 2*(j+k+3))) = index[k]; 
                    }    
                }
            }
        }
        m_pBlurredObjectIB[i]->Unlock();
    }

	m_pBlurredObjectVB->Unlock();
    pIB->Unlock();
	SAFE_RELEASE( pIB );

	// Load the seafloor x file and extract its mesh
    m_pFloorMesh = new NVMesh();
    hr = m_pFloorMesh->Create(m_pNVDevice, GetFilePath("seafloor.x"));
	if (FAILED(hr))
	{
		m_strLastError = "Could not create seafloor.x";
		return hr;
	}
	m_pFloorMesh->SetFVF(m_pNVDevice, MOTIONBLURVERTEX_FVF);
	m_pFloorMesh->SetVertexShader(m_dwMotionBlurShader);

    IDirect3DVertexBuffer8* pVB;

    // Add some "hilliness" to the terrain
    if( SUCCEEDED( m_pFloorMesh->GetSysMemMesh()->GetVertexBuffer( &pVB ) ) )
    {
        MotionBlurVertex* pVertices;
        DWORD   dwNumVertices = m_pFloorMesh->GetSysMemMesh()->GetNumVertices();

        pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 );

        for( DWORD i=0; i<dwNumVertices; i++ )
		{
			pVertices[i].Position.y  += (rand()/(FLOAT)RAND_MAX);
			pVertices[i].Position.y  += (rand()/(FLOAT)RAND_MAX);
			pVertices[i].Position.y  += (rand()/(FLOAT)RAND_MAX);
			pVertices[i].Texture.x *= 10;
			pVertices[i].Texture.y *= 10;
		}

        pVB->Unlock();
        pVB->Release();
    }
    m_pFloorMesh->RestoreDeviceObjects(m_pNVDevice);

    // set up render state
    m_pD3DDev->SetRenderState( D3DRS_LIGHTING,	     FALSE );
    m_pD3DDev->SetRenderState( D3DRS_CULLMODE,       D3DCULL_CCW ); 
    m_pD3DDev->SetRenderState( D3DRS_FOGENABLE,      FALSE);

    m_pD3DDev->SetRenderState( D3DRS_DITHERENABLE,   FALSE );
    m_pD3DDev->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
    m_pD3DDev->SetRenderState( D3DRS_ZENABLE,        TRUE );
    
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

    m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
    m_pD3DDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    m_pD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    // Setup render states
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2);
	m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	m_pD3DDev->SetVertexShaderConstant(CV_CONSTANTS, D3DXVECTOR4(0.0f,1.0f,0.0f,0.0f), 1);
	
    // Projection set up
    D3DXMATRIX        matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(60.0f), 1.0f, 1.0f, 1000.0f);
	m_pNVDevice->SetProjectionTransform(&matProj);
	return S_OK;
}
#pragma optimize ("", on)

HRESULT CShaderMotionBlur::Free()
{
    for (int i = 6; --i >= 0; )
	{
	    SAFE_RELEASE( m_pBlurredObjectIB[i] );
	}
	SAFE_RELEASE(m_pBlurredObjectVB);

	SAFE_DELETE(m_pFloorMesh);
	SAFE_DELETE(m_pBlurredObjectMesh);
    SAFE_RELEASE(m_pBlurredObjectMap);
	SAFE_DELETE(m_pNVDevice);

	if (m_pD3DDev)
	{
		m_pD3DDev->DeleteVertexShader(m_dwMotionBlurShader);
		SAFE_RELEASE(m_pD3DDev);
	}

	return S_OK;
}

HRESULT CShaderMotionBlur::Start()
{
	return S_OK;
}

HRESULT CShaderMotionBlur::SetTransform(bool  bWithBlur)
{
	D3DXMATRIX matWorld, matView, matProj;
	D3DXMATRIX matWorldView, matWorldViewIT;
	float      unused;

	matWorld = m_pNVDevice->GetWorldTransform();
	matView  = m_pNVDevice->GetViewTransform();
	matProj  = m_pNVDevice->GetProjectionTransform();

	D3DXMatrixMultiply(&matWorldView,   &matWorld, &matView);
    D3DXMatrixInverse( &matWorldViewIT, &unused, &matWorldView);
	
	// Projection to clip space
	D3DXMatrixTranspose(&matProj, &matProj);
	m_pD3DDev->SetVertexShaderConstant(CV_PROJ_TXF_0, &matProj(0, 0), 4);

    // Projection to view-space
	D3DXMatrixTranspose(&matWorldView, &matWorldView);
    m_pD3DDev->SetVertexShaderConstant(CV_CURR_WORLDVIEW_TXF_0, &matWorldView(0, 0), 3);
    if (bWithBlur)
    {
        m_pD3DDev->SetVertexShaderConstant(CV_PREV_WORLDVIEW_TXF_0, &m_LastWorldTxf(0, 0), 3);
    	m_LastWorldTxf = matWorldView;
    }
    else
    {
    	m_pD3DDev->SetVertexShaderConstant(CV_PREV_WORLDVIEW_TXF_0, &matWorldView(0, 0), 3);
    }

    // projection of normals to view-space
    m_pD3DDev->SetVertexShaderConstant(CV_CURR_WORLDVIEW_IT_0, &matWorldViewIT(0, 0), 3);

    return S_OK;
}


HRESULT CShaderMotionBlur::Tick(EBTimer* pTimer)
{
	HRESULT hr = S_OK;
	D3DXMATRIX matWorld;
	D3DXMATRIX matView;

    m_pD3DDev->SetRenderState(D3DRS_FILLMODE, (mbWireFrame) ? D3DFILL_WIREFRAME : D3DFILL_SOLID);

    if (m_bPause)
        return hr;

    // Camera setup
	D3DXVECTOR3 const vEyePt    = D3DXVECTOR3( 0.0f,  2.0f, -30.0f);
	D3DXVECTOR3 const vLookatPt = D3DXVECTOR3( 0.0f, -5.0f, 0.0f);
	D3DXVECTOR3 const vUp       = D3DXVECTOR3( 0.0f,  1.0f, 0.0f );

	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUp);
	m_pNVDevice->SetViewTransform(&matView);

	hr = m_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, BACKGROUND_COLOR, 1.0, 0);

    // Sea floor
	D3DXMatrixScaling( &matWorld, 1.0f, 1.0f, 1.0f);
	m_pNVDevice->SetWorldTransform(&matWorld);
	SetTransform( false );
	m_pFloorMesh->Render(m_pNVDevice);

    // now lets draw the blurred object
    float const kSpeed = (m_bEnableSpeed2x) ? 15.0f : 7.5f;
	float const fKickFreq = kSpeed * pTimer->GetDuration(); 
	float const fPhase    = kSpeed * pTimer->GetDuration() / 3.0f;


    m_pD3DDev->SetStreamSource(0, m_pBlurredObjectVB, sizeof(MotionBlurVertex));
	m_pD3DDev->SetVertexShader(m_dwMotionBlurShader);

    m_pD3DDev->SetTexture(0, m_pBlurredObjectMap);

    // Move the blurred object in a circle
    D3DXMATRIX      matScale, matTrans1, matRotate1, matRotate2;
    D3DXMATRIX      matBank,  matLocalRotate;

    D3DXMatrixRotationY(   &matLocalRotate, D3DX_PI/2.0f);
    D3DXMatrixRotationX(   &matBank,        D3DX_PI/8.0f);
    D3DXMatrixRotationZ(   &matRotate1, 2*D3DX_PI); 
    D3DXMatrixRotationY(   &matRotate2, fPhase );
    D3DXMatrixScaling(     &matScale, 2.0f, 2.0f, 2.0f );
    D3DXMatrixTranslation( &matTrans1, -12*sinf(fPhase), sinf(fKickFreq), 10-30*cosf(fPhase) );

	D3DXMatrixIdentity(&matWorld);
    D3DXMatrixMultiply( &matWorld, &matTrans1,      &matWorld);
	D3DXMatrixMultiply( &matWorld, &matScale,       &matWorld);
	D3DXMatrixMultiply( &matWorld, &matRotate2,     &matWorld);
	D3DXMatrixMultiply( &matWorld, &matRotate1,     &matWorld);	
    D3DXMatrixMultiply( &matWorld, &matBank,        &matWorld);
    D3DXMatrixMultiply( &matWorld, &matLocalRotate, &matWorld);

    m_pNVDevice->SetWorldTransform(&matWorld);

    // set the proper alpha colors
    // do not really have to write these all the time...
    FLOAT const kBlurAlpha = 0.1f;

    // This is not correct for vertices with actual alpha!
    // we should make sure we have the right alpha-constants in the registers...
    // Would have to modify the Render() routine...
    D3DXVECTOR4 vColorPrevious(1.0f, 1.0f, 1.0f, kBlurAlpha );
	D3DXVECTOR4 vColorCurrent (1.0f, 1.0f, 1.0f, 1.0f );

    m_pD3DDev->SetVertexShaderConstant( CV_PREV_COLOR, &vColorCurrent,  1 );
    m_pD3DDev->SetVertexShaderConstant( CV_CURR_COLOR, &vColorCurrent,  1 );

    float blurFraction(1.0f);

    switch (meBlurLengthOption)
    {
        case HALFxBLURLENGTH:
            blurFraction = 0.5f;
            break;
        case ONEHALFxBLURLENGTH:
            blurFraction = 1.5f;
            break;
        default:
            break;
    }

    D3DXVECTOR4 vOneOverExtent(m_OneOverExtent, 1.0f, blurFraction, 1.0f);
    m_pD3DDev->SetVertexShaderConstant( CV_OBJECT_EXTEND, &vOneOverExtent, 1 );

    // draw object once w/o blur
    SetTransform( false );     

    // no need to write z the first time: we will write when drawing the second time!
    m_pD3DDev->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

    float   fBias = (m_bEnableMotionBlur ) ? kSpeed/10.0f : 0.0f;
    m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, *((LPDWORD) (&fBias)));

    m_pBlurredObjectMesh->Render(m_pNVDevice);

    // now draw it again with blur and alpha sorted
    m_pD3DDev->SetVertexShaderConstant( CV_PREV_COLOR, &vColorPrevious,   1 );
	SetTransform( m_bEnableMotionBlur );


    // invert model-matrix and transform cam to model-space
    D3DXMATRIX  MatWorldInverse;    
    FLOAT       det;
    D3DXMatrixInverse(&MatWorldInverse, &det, &matWorld); 

    D3DXVECTOR4     vLookAtModel, vEyeModel;

    D3DXVec3Transform(&vLookAtModel, &vLookatPt, &MatWorldInverse);
    D3DXVec3Transform(&vEyeModel,    &vEyePt,    &MatWorldInverse);

    D3DXVECTOR3 const vFromCam = vLookAtModel - vEyeModel;
    
    // find the biggest component 
    if (   (abs(vFromCam.x) > abs(vFromCam.y))
        && (abs(vFromCam.x) > abs(vFromCam.z)) ) 
    {
        // x is largest
        m_pD3DDev->SetIndices(m_pBlurredObjectIB[(vFromCam.x < 0) ? 0 : 1], 0);
    }
    else if (   (abs(vFromCam.y) > abs(vFromCam.x)) 
             && (abs(vFromCam.y) > abs(vFromCam.z)) )
    {
        // y is largest 
        m_pD3DDev->SetIndices(m_pBlurredObjectIB[(vFromCam.y < 0) ? 2 : 3], 0);
    }
    else
    {
        // z is largest
        m_pD3DDev->SetIndices(m_pBlurredObjectIB[(vFromCam.z < 0) ? 4 : 5], 0);
    }

    // now lets write z 
    m_pD3DDev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

    // render the blurred object!
    hr = m_pD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_numVertices, 0, m_numIndices/3);

    // reset MIPMAP bias
    fBias = 0.0f;
    m_pD3DDev->SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, *((LPDWORD) (&fBias)));

    Sleep(50);      // artificially kill frame rate to make blur apparent!

	return hr;
}

HRESULT CShaderMotionBlur::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT Format)
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

    // check simultaneous texture support (only need 2...)
    if(pCaps->MaxSimultaneousTextures < 2)
	{
		m_strLastError = "Device does not support 4 simultaneous textures!";
		return E_FAIL;
	}

    // only need very few, simple register combiners...
	if(pCaps->MaxTextureBlendStages < 2)
	{
		m_strLastError = "Device does not support 8 register combiners!";
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
     
	return S_OK;
}

void CShaderMotionBlur::Keyboard(DWORD dwKey, UINT nFlags, bool bDown)
{
    unsigned int option = static_cast<unsigned int>(meBlurLengthOption); 
	eEBKeyAction Action = TranslateEffectKey(dwKey, nFlags, bDown);
	
    switch ( Action )
    {
		case EB_HELP:
		{
			::MessageBoxEx( NULL, " Help : F1 - Help \n\n Home - Reset To Defaults \n\n W - Wireframe Toggle \n\n Pause/Space - Toggle pausing the animation \n\n +/- - Lengthen/Shorten Blur Length",
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
            m_bPause = !m_bPause;
            m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
        }
        break;

		case EB_RESET:
        {
            meBlurLengthOption = ONExBLURLENGTH;
            m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
        }
		break;

        case EB_ADD:
            if (option < NUM_BLURLENGTHS-1)
                ++option;
            meBlurLengthOption = static_cast<eBlurLengths>(option);
            m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
            break;

        case EB_SUBTRACT:
            if (option > FIRST_BLURLENGTH)
                --option;
            meBlurLengthOption = static_cast<eBlurLengths>(option);
            m_dwEffectDirtyFlags |= EBEFFECT_DIRTY_PUBLICSTATE;
            break;

        default:
            break;
    }
}
