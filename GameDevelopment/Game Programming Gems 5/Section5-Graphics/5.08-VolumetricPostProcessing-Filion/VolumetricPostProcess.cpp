//-----------------------------------------------------------------------------
// File: VolumetricPostProcess.cpp
//
// Desc: Example code showing how to apply post process environement mapping.
//-----------------------------------------------------------------------------
#include "dxstdafx.h"
#include "D3DFile.h"
#include "D3DFont.h"


//#define DEBUG_VS   // Uncomment this line to debug vertex shaders 
//#define DEBUG_PS   // Uncomment this line to debug pixel shaders 


// Change this value to increase or decrease the number of planes in the scene.
const unsigned long c_uAirplaneNumber = 10;

// Change this value to increase or decrease the initial speed of airplanes in the scene.
const float c_fInitialAirplaneSpeed = 0.5f;


// Defintion of the vertices using in the Vertex Buffers of the sample.
struct sVertex
{
    D3DXVECTOR3 m_Position; // Position
	D3DXVECTOR3 m_Normal; // normal
	D3DXVECTOR2 m_TexCoords; // texture coordinate

    static const DWORD FVF;
};
const DWORD sVertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;


// Screen quad vertex format
struct sScreenVertex
{
    D3DXVECTOR3 m_Position; // position
    D3DXVECTOR2 m_TexCoords; // texture coordinate

    static const DWORD FVF;
};
const DWORD sScreenVertex::FVF = D3DFVF_XYZ | D3DFVF_TEX1;


//-----------------------------------------------------------------------------
// Class: cVisualObject
//
// Desc: This class owns the World Matrix of Visual Objects in the scene.
//-----------------------------------------------------------------------------
class cVisualObject
{
public:
	cVisualObject()
	{
	    D3DXMatrixIdentity( &m_mWorld );
	}

	virtual ~cVisualObject()
	{
	}

    D3DXMATRIXA16 m_mWorld;
};



//-----------------------------------------------------------------------------
// Class: cSkyBox
//
// Desc: This class defines the members and methods for rendering a SkyBox.
//-----------------------------------------------------------------------------
class cSkyBox : public cVisualObject
{
public:
	cSkyBox()
	{
		D3DXMatrixScaling( &m_mWorld, 10.0f, 10.0f, 10.0f );
	}

	virtual ~cSkyBox()
	{
	}

	void Render( LPDIRECT3DDEVICE9 pd3dDevice )
	{
		// Since the SkyBox is rendered on all the pixels in the Target, there is
		// no need for clearing the buffers, or using Zbuffers.
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS );

		// Sets the texture states for rendering the skybox
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU,  D3DTADDRESS_MIRROR );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV,  D3DTADDRESS_MIRROR );
		
		// Render the skybox
		ms_pMesh->Render( pd3dDevice );
	}

    static CD3DMesh* ms_pMesh;
};

CD3DMesh* cSkyBox::ms_pMesh = NULL;



//-----------------------------------------------------------------------------
// Class: cAirplane
//
// Desc: This class defines the members and methods for rendering an Airplane.
//-----------------------------------------------------------------------------
class cAirplane : public cVisualObject
{
public:
	cAirplane()
	{
	}

	virtual ~cAirplane()
	{
	}

	// This function updates the position of the airplane in function of time.
	void Update( float fTime )
	{
		// Animate file object
		D3DXMATRIXA16 mat;

		D3DXMatrixScaling( &m_mWorld, 0.2f, 0.2f, 0.2f );

		D3DXMatrixTranslation( &mat, 0.0f, 2.0f, 0.0f );
		D3DXMatrixMultiply( &m_mWorld, &m_mWorld, &mat );

		D3DXMatrixRotationX( &mat,	-2.9f * fTime * ms_fSpeed );
		D3DXMatrixMultiply( &m_mWorld, &m_mWorld, &mat );

		D3DXMatrixRotationY( &mat,	1.055f * fTime * ms_fSpeed );
		D3DXMatrixMultiply( &m_mWorld, &m_mWorld, &mat );
	}

	// This function renders the Airplane. The bool parameter specifies if the
	// airplane material should be set before rendering or not. 
	void Render( LPDIRECT3DDEVICE9 pd3dDevice )
	{
		LPDIRECT3DVERTEXBUFFER9 pVB;
		LPDIRECT3DINDEXBUFFER9 pIB;

		ms_pMesh->m_pLocalMesh->GetVertexBuffer(&pVB);
		ms_pMesh->m_pLocalMesh->GetIndexBuffer(&pIB);

		pd3dDevice->SetStreamSource(0, pVB, 0, sizeof(sVertex));
		pd3dDevice->SetFVF( sVertex::FVF );
		pd3dDevice->SetIndices( pIB );
		pd3dDevice->DrawIndexedPrimitive(	D3DPT_TRIANGLELIST, 0,
											0, ms_pMesh->m_pLocalMesh->GetNumVertices(),
											0, ms_pMesh->m_pLocalMesh->GetNumFaces());

		pVB->Release();
		pIB->Release();
	}

    static CD3DMesh* ms_pMesh;

	static float ms_fSpeed;
};

CD3DMesh* cAirplane::ms_pMesh = NULL;
float cAirplane::ms_fSpeed = c_fInitialAirplaneSpeed;


//-----------------------------------------------------------------------------
// Class: cSphere
//
// Desc: This class defines the members and methods for rendering a Sphere.
//-----------------------------------------------------------------------------
class cSphere : public cVisualObject
{
public:
	cSphere()
	{
		D3DXMatrixScaling( &m_mWorld, 1.3f, 1.3f, 1.3f );
	}

	virtual ~cSphere()
	{
	}

	void Render( LPDIRECT3DDEVICE9 pd3dDevice )
	{
		LPDIRECT3DVERTEXBUFFER9 pVB;
		LPDIRECT3DINDEXBUFFER9 pIB;

		ms_pMesh->m_pLocalMesh->GetVertexBuffer( &pVB );
		ms_pMesh->m_pLocalMesh->GetIndexBuffer( &pIB );

		pd3dDevice->SetStreamSource( 0, pVB, 0, sizeof(sVertex) );
		pd3dDevice->SetFVF(sVertex::FVF);
		pd3dDevice->SetIndices( pIB );
		pd3dDevice->DrawIndexedPrimitive(	D3DPT_TRIANGLELIST, 0,
											0, ms_pMesh->m_pLocalMesh->GetNumVertices(),
											0, ms_pMesh->m_pLocalMesh->GetNumFaces());

		pVB->Release();
		pIB->Release();
	}

    static CD3DMesh* ms_pMesh;
};

CD3DMesh* cSphere::ms_pMesh = NULL;



//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class (CD3DApplication) provides the 
//       generic functionality needed in all Direct3D samples. CMyD3DApplication 
//       adds functionality specific to this sample program.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
    BOOL m_bCapture;

    D3DXMATRIXA16	m_mProject;
    D3DXMATRIXA16	m_mView;
    D3DXMATRIXA16	m_mTrackBall;
	D3DXMATRIXA16	m_mTextureProject;

    PDIRECT3DTEXTURE9 m_pRenderTarget;
	PDIRECT3DTEXTURE9 m_pDepthBuffer;
	PDIRECT3DTEXTURE9 m_pBumpMap;
    LPD3DXEFFECT m_pEffect;
    CD3DFont* m_pFont;

	cSphere m_Sphere;
	cSkyBox m_SkyBox;
	cAirplane m_Airplane[c_uAirplaneNumber];

protected:
	void PreRender( cVisualObject* pObject );
   	void WriteRenderTarget();
    void RenderDepthBuffer();
	void RenderColorBuffer();
	void RenderNotVolumetric();
	void RenderPostProcess();
	void OutputStatistics();
	void DrawFullScreenQuad();

    HRESULT ConfirmDevice( D3DCAPS9*, DWORD, D3DFORMAT, D3DFORMAT );
    HRESULT OneTimeSceneInit();
    HRESULT InitDeviceObjects();
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
    HRESULT DeleteDeviceObjects();
    HRESULT Render();
    HRESULT FrameMove();
    HRESULT FinalCleanup();

public:
    LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

    CMyD3DApplication();

	bool m_bApplyPostProccess;
	bool m_bSelectPostProcess;
};



//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    CMyD3DApplication d3dApp;

    InitCommonControls();
    if( FAILED( d3dApp.Create( hInst ) ) )
        return 0;

    return d3dApp.Run();
}



//-----------------------------------------------------------------------------
// Name: CMyD3DApplication()
// Desc: Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication()
{
    m_strWindowTitle = _T("Volumetric Post Process FX");
    m_d3dEnumeration.AppUsesDepthBuffer = TRUE;
    m_bCapture = FALSE;

	m_pDepthBuffer = NULL;  
	m_pRenderTarget = NULL;
	m_pBumpMap = NULL;
    m_pEffect = NULL;
	m_bApplyPostProccess = true;
	m_bSelectPostProcess = true;
}



//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::OneTimeSceneInit()
{
    m_pFont				= new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
	cSphere::ms_pMesh	= new CD3DMesh();
	cSkyBox::ms_pMesh	= new CD3DMesh();
	cAirplane::ms_pMesh = new CD3DMesh();

	if( !m_pFont || !cSphere::ms_pMesh ||
		!cSkyBox::ms_pMesh || !cAirplane::ms_pMesh )
        return E_OUTOFMEMORY;

    D3DXMatrixIdentity( &m_mTrackBall );
    D3DXMatrixTranslation( &m_mView, 0.0f, 0.0f, 3.0f );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove()
{
	for( unsigned long i = 0; i < c_uAirplaneNumber; i++ )
		m_Airplane[i].Update(	( m_fTime + ( float) i * 5 / c_uAirplaneNumber ) *
								( .5f + (float) i / c_uAirplaneNumber ) );

	// When the window has focus, let the mouse adjust the camera view
    if( m_bCapture )
    {
        D3DXMATRIXA16 mCursor;
        D3DXQUATERNION qCursor = D3DUtil_GetRotationFromCursor( m_hWnd );
        D3DXMatrixRotationQuaternion( &mCursor, &qCursor );
        D3DXMatrixMultiply( &m_mView, &m_mTrackBall, &mCursor );

        D3DXMATRIXA16 mTrans;
        D3DXMatrixTranslation( &mTrans, 0.0f, 0.0f, 3.0f );
        D3DXMatrixMultiply( &m_mView, &m_mView, &mTrans );
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects()
{
    // Load the file objects
	if( FAILED( cSphere::ms_pMesh->Create( m_pd3dDevice, _T("Sphere.x") ) ) )
        return D3DAPPERR_MEDIANOTFOUND;
	if( FAILED( cSkyBox::ms_pMesh->Create( m_pd3dDevice, _T("lobby_skybox.x") ) ) )
        return D3DAPPERR_MEDIANOTFOUND;
	if( FAILED( cAirplane::ms_pMesh->Create( m_pd3dDevice, _T("airplane.x") ) ) )
        return D3DAPPERR_MEDIANOTFOUND;
	if( FAILED( D3DXCreateTextureFromFile( m_pd3dDevice, "bumpmap.tga", &m_pBumpMap ) ) )
        return D3DAPPERR_MEDIANOTFOUND;
	
    // Set mesh properties
    cAirplane::ms_pMesh->SetFVF( m_pd3dDevice, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 );
    cSphere::ms_pMesh->SetFVF( m_pd3dDevice, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 );

    // Restore the device-dependent objects
    m_pFont->InitDeviceObjects( m_pd3dDevice );
	
	// Assemble and set the pixel shader 
	LPD3DXBUFFER pCode = NULL;
	LPD3DXBUFFER pErrorMsgs = NULL;

	// This forces using no optimization for shader compiling
    DWORD dwShaderFlags = 0;
    
#ifdef DEBUG_VS
	dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
#endif

#ifdef DEBUG_PS
    dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
#endif

    // Load the effect file
    TCHAR Path[MAX_PATH];
    DXUtil_FindMediaFileCch(Path, sizeof(Path), TEXT("VolumetricPostProcess.fx"));
	if( FAILED( D3DXCreateEffectFromFile(	m_pd3dDevice, Path, NULL, NULL,
											dwShaderFlags, NULL, &m_pEffect, NULL) ) )
        return D3DAPPERR_MEDIANOTFOUND;

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Restore device-memory objects and state after a device is created or
//       resized.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
    // InitDeviceObjects for file objects (build textures and vertex buffers)
	cSphere::ms_pMesh->RestoreDeviceObjects( m_pd3dDevice );
	cSkyBox::ms_pMesh->RestoreDeviceObjects( m_pd3dDevice );
	cAirplane::ms_pMesh->RestoreDeviceObjects( m_pd3dDevice );
    m_pFont->RestoreDeviceObjects();

	SAFE_RELEASE( m_pBumpMap );
	m_pBumpMap = NULL;
	D3DXCreateTextureFromFile( m_pd3dDevice, "bumpmap.tga", &m_pBumpMap );

    // Restore the effect
    if( m_pEffect )
        m_pEffect->OnResetDevice();
	
    // Create the HDR scene texture
    m_pd3dDevice->CreateTexture( m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height, 
                                 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F,
                                 D3DPOOL_DEFAULT, &m_pDepthBuffer, NULL );

    m_pd3dDevice->CreateTexture( m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height, 
                                 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
                                 D3DPOOL_DEFAULT, &m_pRenderTarget, NULL );

    // Set the transform matrices
    FLOAT fAspect = (FLOAT) m_d3dsdBackBuffer.Width / (FLOAT) m_d3dsdBackBuffer.Height;
    D3DXMatrixPerspectiveFovLH( &m_mProject, D3DX_PI / 2, fAspect, .5f, 100.f );

	D3DXMATRIXA16 mBias;
	mBias._11 = 0.5f;
	mBias._22 = -0.5f;
	mBias._33 = 0.5f;
	mBias._41 = 0.5f;
	mBias._42 = 0.5f;
	mBias._43 = 0.5f;
	mBias._44 = 1.0f;

	D3DXMatrixMultiply( &m_mTextureProject, &m_mProject, &mBias );

	return S_OK;
}



//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Called when the device-dependent objects are about to be lost.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
	cSphere::ms_pMesh->InvalidateDeviceObjects();
	cSkyBox::ms_pMesh->InvalidateDeviceObjects();
	cAirplane::ms_pMesh->InvalidateDeviceObjects();
    m_pFont->InvalidateDeviceObjects();
	
    if( m_pEffect )
        m_pEffect->OnLostDevice();

    SAFE_RELEASE( m_pDepthBuffer );
	SAFE_RELEASE( m_pRenderTarget );
	SAFE_RELEASE( m_pBumpMap );
	m_pBumpMap = NULL;

	return S_OK;
}



//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
    m_pFont->DeleteDeviceObjects();
	cSphere::ms_pMesh->Destroy();
	cSkyBox::ms_pMesh->Destroy();
	cAirplane::ms_pMesh->Destroy();
	
    m_pEffect->Release();

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup()
{
    SAFE_DELETE( m_pFont );
	SAFE_DELETE( cSphere::ms_pMesh );
	SAFE_DELETE( cSkyBox::ms_pMesh );
	SAFE_DELETE( cAirplane::ms_pMesh );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: Called during device initialization, this code checks the device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior,
                                          D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat )
{
    // Debugging vertex shaders requires either REF or software vertex processing 
    // and debugging pixel shaders requires REF.  
    #ifdef DEBUG_VS
        if( pCaps->DeviceType != D3DDEVTYPE_REF && 
            (dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING) == 0 )
            return E_FAIL;
    #endif
    #ifdef DEBUG_PS
        if( pCaps->DeviceType != D3DDEVTYPE_REF )
            return E_FAIL;
    #endif

    if( dwBehavior & D3DCREATE_PUREDEVICE )
        return E_FAIL;

    if( !(pCaps->TextureCaps & D3DPTEXTURECAPS_CUBEMAP) &&
        !(dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING) &&
        !(pCaps->VertexShaderVersion >= D3DVS_VERSION(1, 0)) )
    {
        return E_FAIL;
    }

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Message proc function to handle key and menu input
//-----------------------------------------------------------------------------
LRESULT CMyD3DApplication::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam,
                                    LPARAM lParam )
{
    // Capture mouse when clicked
    if( WM_LBUTTONDOWN == uMsg )
    {
        D3DXMATRIXA16 mCursor;
        D3DXQUATERNION qCursor = D3DUtil_GetRotationFromCursor( m_hWnd );
        D3DXMatrixRotationQuaternion( &mCursor, &qCursor );
        D3DXMatrixTranspose( &mCursor, &mCursor );
        D3DXMatrixMultiply( &m_mTrackBall, &m_mTrackBall, &mCursor );

        SetCapture( m_hWnd );
        m_bCapture = TRUE;
        return 0;
    }

    if( WM_LBUTTONUP == uMsg )
    {
        D3DXMATRIXA16 mCursor;
        D3DXQUATERNION qCursor = D3DUtil_GetRotationFromCursor( m_hWnd );
        D3DXMatrixRotationQuaternion( &mCursor, &qCursor );
        D3DXMatrixMultiply( &m_mTrackBall, &m_mTrackBall, &mCursor );

        ReleaseCapture();
        m_bCapture = FALSE;
        return 0;
    }

	if( WM_KEYDOWN == uMsg )
	{
        switch( toupper( LOWORD( wParam ) ) )
        {
		case 'A':
			// Increase speed of airplanes.
			cAirplane::ms_fSpeed += .1f;
			break;

		case 'S':
			// Decrease speed of airplanes.
			cAirplane::ms_fSpeed -= .1f;
			cAirplane::ms_fSpeed = ( cAirplane::ms_fSpeed < 0.f ) ? 0.f : cAirplane::ms_fSpeed;
			break;

		case 'P':
			m_bApplyPostProccess = !m_bApplyPostProccess;
			break;

		case 'O':
			m_bSelectPostProcess = !m_bSelectPostProcess;
			break;
		}

        return TRUE; // Discard unused keystrokes
    }


    // Pass remaining messages to default handler
    return CD3DApplication::MsgProc( hWnd, uMsg, wParam, lParam );
}


//-----------------------------------------------------------------------------
// Name: DrawFullScreenQuad
// Desc: Draw a properly aligned quad covering the entire render target
//-----------------------------------------------------------------------------
void CMyD3DApplication::DrawFullScreenQuad()
{
    D3DSURFACE_DESC dtdsdRT;
    PDIRECT3DSURFACE9 pSurfRT;

    // Acquire render target width and height
    m_pd3dDevice->GetRenderTarget(0, &pSurfRT);
    pSurfRT->GetDesc(&dtdsdRT);
    pSurfRT->Release();

    // Ensure that we're directly mapping texels to pixels by offset by 0.5
    // For more info see the doc page titled "Directly Mapping Texels to Pixels"
    float fWidth = (float) dtdsdRT.Width;
    float fHeight = (float) dtdsdRT.Height;

    // Draw the quad
    sScreenVertex svQuad[4];

    svQuad[0].m_Position = D3DXVECTOR3( -( fWidth + .5f ) / fWidth, ( fHeight + .5f ) / fHeight, 0.f );
    svQuad[0].m_TexCoords = D3DXVECTOR2( 0.f, 0.f );

    svQuad[2].m_Position = D3DXVECTOR3( -( fWidth + .5f ) / fWidth, -( fHeight - 1.5f ) / fHeight, 0.f );
    svQuad[2].m_TexCoords = D3DXVECTOR2( 0.f, 1.f );

    svQuad[1].m_Position = D3DXVECTOR3(  ( fWidth - 1.5f ) / fWidth, ( fHeight + .5f ) / fHeight, 0.f );
    svQuad[1].m_TexCoords = D3DXVECTOR2( 1.f, 0.f );

    svQuad[3].m_Position = D3DXVECTOR3(  ( fWidth - 1.5f ) / fWidth, -( fHeight - 1.5f ) / fHeight, 0.f );
    svQuad[3].m_TexCoords = D3DXVECTOR2( 1.f, 1.f );

	D3DXMATRIXA16 mIdentity;
	D3DXMatrixIdentity( &mIdentity );

	// Sets the transforms for the SkyBox
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &mIdentity );
	m_pd3dDevice->SetTransform( D3DTS_VIEW, &mIdentity );
	m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &mIdentity );

    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
    m_pd3dDevice->SetFVF( sScreenVertex::FVF );
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, svQuad, sizeof( sScreenVertex ) );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
}


//-----------------------------------------------------------------------------
// Name: PreRender()
// Desc: Does the PreRendering of a Visual Object by setting tranformation
//		 matrices in the device and shaders.
//-----------------------------------------------------------------------------
void CMyD3DApplication::PreRender( cVisualObject* pObject )
{
	D3DXMATRIXA16 mWorldView;
	D3DXMATRIXA16 mWorldViewProject;
	D3DXMATRIXA16 mRenderTargetProject;

	D3DXMatrixMultiply( &mWorldView, &pObject->m_mWorld, &m_mView );
	D3DXMatrixMultiply( &mWorldViewProject, &mWorldView, &m_mProject );
	D3DXMatrixMultiply( &mRenderTargetProject, &mWorldView, &m_mTextureProject );

	// Sets the transforms for the SkyBox
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &pObject->m_mWorld );
	m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_mView );
	m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_mProject );

	m_pEffect->SetMatrix( "g_mWorldViewProj", &mWorldViewProject );
	m_pEffect->SetMatrix( "g_mTexture", &mRenderTargetProject);
	m_pEffect->SetFloat( "g_fScale", ( 2.f + sin( m_fTime * 2 ) ) * 256 );
}


//-----------------------------------------------------------------------------
// Name: WriteRenderTarget()
// Desc: Does the Rendering of the Render Target serving as a Depth buffer.
//		 Only the PostProcess primitives are rendered in the buffer. 
//-----------------------------------------------------------------------------
void CMyD3DApplication::WriteRenderTarget()
{
	// Keeps track of the back buffer surface to restore it later
	IDirect3DSurface9* m_pBackBufferSurface = NULL;
	m_pd3dDevice->GetRenderTarget( 0, &m_pBackBufferSurface );

	// Set render target to float buffer to prepare depth buffer containing
	// the pixel depth of the Sphere
	IDirect3DSurface9* m_pDepthBufferSurface = NULL;
	m_pDepthBuffer->GetSurfaceLevel( 0, &m_pDepthBufferSurface );
	m_pd3dDevice->SetRenderTarget( 0, m_pDepthBufferSurface );
	m_pDepthBufferSurface->Release();
	m_pDepthBufferSurface = NULL;

	// Fill float buffer with Near Plane values ( which is 0.f ).
	// Turn of depth testing/writing as the pixel shader will replace this process.
	m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.f, 0 );

	// Begin the scene
    m_pd3dDevice->BeginScene();

	// We want to use Z Test/Write for this part as we are computing
	// our own depth buffer using the shaders
	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );

	// Computes Depth Buffer of the Sphere in RenderTarget using
	// ComputeDepth vertex shader and WriteDepth Pixel Shader.
	m_pEffect->SetTechnique( "WriteDepthBuffer" );

	PreRender( &m_Sphere );

	UINT uPassCount;
	m_pEffect->Begin( &uPassCount, 0 );
	m_pEffect->Pass( 0 );
	m_Sphere.Render( m_pd3dDevice );
    m_pEffect->End();

    // End the scene.
    m_pd3dDevice->EndScene();

	// Done rendering Sphere in float buffer, switch back to backbuffer
	m_pd3dDevice->SetRenderTarget( 0, m_pBackBufferSurface );
	m_pBackBufferSurface->Release();
	m_pBackBufferSurface = NULL;
}


//-----------------------------------------------------------------------------
// Name: RenderDepthBuffer()
// Desc: This uses the previoulsy rendered Depth Buffer to render the
//		 conventional Depth buffer of the sceme.
//-----------------------------------------------------------------------------
void CMyD3DApplication::RenderDepthBuffer()
{
// Begin the scene
    m_pd3dDevice->BeginScene();
	
	m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_ZBUFFER, 0, 1.f, 0 );
	
	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );

	// The alpha test is enabled for all pixels having alpha = 0 from the testdepth pixelshader
	// must not update the depthbuffer. ModelViewProjMatrix must be recomputed for the airplane.
	m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0 );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATER );

	// Now we render the whole scene by using the floatbuffer and the testdepth pixel shader
	// to update all the pixels of the conventional depth buffer that are behind the Sphere.
	m_pEffect->SetTechnique( "TestDepthBuffer" );
	
	// We are done using float buffer, remove it form stage 1.
	m_pd3dDevice->SetTexture( 0, m_pDepthBuffer );

	// These are for removing artifacts on edges of the Sphere.
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );

	// Render the airplanes using the shaders to update the conventional Z Buffer, which will then
	// be used for rendering only whats behind the PostPRocess region.
	for( unsigned long i = 0; i < c_uAirplaneNumber; i++ )
	{
		UINT uPassCount;
		m_pEffect->Begin( &uPassCount, 0 );
		m_pEffect->Pass( 0 );
		PreRender( &m_Airplane[i] );
		m_Airplane[i].Render( m_pd3dDevice );
		m_pEffect->End();
	}

	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );

	// We are done using float buffer, remove it form stage 1.
	m_pd3dDevice->SetTexture( 0, NULL );

	// Done rendering depth buffer, aplha test can be turned off now.
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );

	// Begin the scene
    m_pd3dDevice->EndScene();
}


//-----------------------------------------------------------------------------
// Name: RenderColorBuffer()
// Desc: This function is the final step of The PostProcess Volumetric Process
//		 Once the conventional depth buffer has been updated by
//		 RenderDepthBuffer() function, we render the whole scene using the
//		 objects respective materials, but using ZTest fct = EQUAL. This makes
//		 all parts of the Scene's objects being in front of the Post-Process
//		 region not to be rendered. The Post-Process Effect can then be applied
//		 to the Target. Once done, a final render pass of the whole scene is
//		 made using Zfct = LESS to render all objects in front of the post-process
//		 Effect.
//-----------------------------------------------------------------------------
void CMyD3DApplication::RenderColorBuffer()
{
	// Keeps track of the back buffer surface to restore it later
	IDirect3DSurface9* m_pBackBufferSurface = NULL;
	m_pd3dDevice->GetRenderTarget( 0, &m_pBackBufferSurface );

	// Set render target so we can use the back buffer for texturing.
	IDirect3DSurface9* m_pRenderTargetSurface = NULL;
	m_pRenderTarget->GetSurfaceLevel( 0, &m_pRenderTargetSurface );
	m_pd3dDevice->SetRenderTarget( 0, m_pRenderTargetSurface );
	m_pRenderTargetSurface->Release();
	m_pRenderTargetSurface = NULL;

	// Begin the scene
    m_pd3dDevice->BeginScene();

	// First render the Skybox, since the skybox shall always be behind everything in scene.
	PreRender( &m_SkyBox );
	m_SkyBox.Render( m_pd3dDevice );

	// Now that we got the depth buffer of all the objects behind the Sphere, we redraw them
	// with their respective material using Zfct = equal, so any object in front of the Sphere
	// will still not be drawn.
	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_EQUAL );

	// Render the airplanes with their own material. Parts in front of PostProcess Region are
	// not drawn.
	m_pEffect->SetTechnique( "AirplaneMaterial" );
	m_pd3dDevice->SetTexture( 0, cAirplane::ms_pMesh->m_pTextures[0] );

	// Render all scene using objects' respective materials
	// making all objects in front of Sphere to be rendered.
	for( unsigned long i = 0; i < c_uAirplaneNumber; i++ )
	{
		UINT uPassCount;
		m_pEffect->Begin( &uPassCount, 0 );
		m_pEffect->Pass( 0 );
		PreRender( &m_Airplane[i] );
		m_Airplane[i].Render( m_pd3dDevice );
		m_pEffect->End();
	}

	m_pd3dDevice->SetTexture( 0, NULL );

	// Done rendering Sphere in float buffer, switch back to backbuffer
	m_pd3dDevice->SetRenderTarget( 0, m_pBackBufferSurface );
	m_pBackBufferSurface->Release();
	m_pBackBufferSurface = NULL;

	// Applies the PostProcess Effect
	// Render a quad filling up the backbuffer with the content of the RenderTarget
	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS );

	m_pd3dDevice->SetTexture( 0, m_pRenderTarget );

	// Draws the RenerTarget on the backbuffer before applying the PostProcess Effect
	DrawFullScreenQuad();

	// Set the PostProcess Technique before rendering the Sphere
	m_pEffect->SetTechnique( m_bSelectPostProcess ? "PostProcess2" : "PostProcess1" );
	PreRender( &m_Sphere );

	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );

	m_pd3dDevice->SetTexture( 0, m_pRenderTarget );
	m_pd3dDevice->SetTexture( 1, m_pBumpMap );

	UINT uPassCount;
	m_pEffect->Begin( &uPassCount, 0 );
    m_pEffect->Pass( 0 );
	m_Sphere.Render( m_pd3dDevice );
    m_pEffect->End();

	m_pd3dDevice->SetTexture( 1, NULL );

	m_pEffect->SetTechnique( "AirplaneMaterial" );

	m_pd3dDevice->SetTexture( 0, cAirplane::ms_pMesh->m_pTextures[0] );

	// Render all scene using objects' respective materials
	// making all objects in front of Sphere to be rendered.
	for( unsigned long i = 0; i < c_uAirplaneNumber; i++ )
	{
		UINT uPassCount;
		m_pEffect->Begin( &uPassCount, 0 );
		m_pEffect->Pass( 0 );
		PreRender( &m_Airplane[i] );
		m_Airplane[i].Render( m_pd3dDevice );
		m_pEffect->End();
	}

	m_pd3dDevice->SetTexture( 0, NULL );

    // Output statistics
	OutputStatistics();

	// End the scene.
    m_pd3dDevice->EndScene();
}


void CMyD3DApplication::RenderNotVolumetric()
{
	// Begin the scene
	m_pd3dDevice->BeginScene();

	// Keeps track of the back buffer surface to restore it later
	IDirect3DSurface9* m_pBackBufferSurface = NULL;
	m_pd3dDevice->GetRenderTarget( 0, &m_pBackBufferSurface );

	// Set render target so we can use the back buffer for texturing.
	IDirect3DSurface9* m_pRenderTargetSurface = NULL;
	m_pRenderTarget->GetSurfaceLevel( 0, &m_pRenderTargetSurface );
	m_pd3dDevice->SetRenderTarget( 0, m_pRenderTargetSurface );
	m_pRenderTargetSurface->Release();
	m_pRenderTargetSurface = NULL;

	m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_ZBUFFER, 0, 1.f, 0 );
	
	// First render the Skybox, since the skybox shall always be behind everything in scene.
	PreRender( &m_SkyBox );
	m_SkyBox.Render( m_pd3dDevice );

	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );

	m_pEffect->SetTechnique( "AirplaneMaterial" );

	m_pd3dDevice->SetTexture( 0, cAirplane::ms_pMesh->m_pTextures[0] );

	// Render all scene objects
	for( unsigned long i = 0; i < c_uAirplaneNumber; i++ )
	{
		UINT uPassCount;
		m_pEffect->Begin( &uPassCount, 0 );
		m_pEffect->Pass( 0 );
		PreRender( &m_Airplane[i] );
		m_Airplane[i].Render( m_pd3dDevice );
		m_pEffect->End();
	}
	
	// Done rendering Sphere in float buffer, switch back to backbuffer
	m_pd3dDevice->SetRenderTarget( 0, m_pBackBufferSurface );
	m_pBackBufferSurface->Release();
	m_pBackBufferSurface = NULL;

	// Render a quad filling up the backbuffer with the content of the RenderTarget
	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS );

	m_pd3dDevice->SetTexture( 0, m_pRenderTarget );
	DrawFullScreenQuad();
	m_pd3dDevice->SetTexture( 0, NULL );

	m_pEffect->SetTechnique( m_bSelectPostProcess ? "PostProcess2" : "PostProcess1" );
	PreRender( &m_Sphere );

	m_pd3dDevice->SetTexture( 0, m_pRenderTarget );
	m_pd3dDevice->SetTexture( 1, m_pBumpMap );

	UINT uPassCount;
	m_pEffect->Begin( &uPassCount, 0 );
    m_pEffect->Pass( 0 );
	m_Sphere.Render( m_pd3dDevice );
    m_pEffect->End();

	m_pd3dDevice->SetTexture( 0, NULL );
	m_pd3dDevice->SetTexture( 1, NULL );

    // Output statistics
	OutputStatistics();

	// End the scene.
    m_pd3dDevice->EndScene();
}


//-----------------------------------------------------------------------------
// Name: OutputStatistics()
// Desc: This renders the statistics on the top left corner of the Target.
//-----------------------------------------------------------------------------
void CMyD3DApplication::OutputStatistics()
{
	TCHAR strVolPostProcess[35] = "(P) Vol. PostProcess : ";
	strcat( strVolPostProcess, ( m_bApplyPostProccess ) ? "On" : "Off" );
/*
    m_pFont->DrawText( 2,  0, D3DCOLOR_ARGB(255,255,255,0), m_strFrameStats );
    m_pFont->DrawText( 2, 20, D3DCOLOR_ARGB(255,255,255,0), m_strDeviceStats );
    m_pFont->DrawText( 2, 40, D3DCOLOR_ARGB(255,255,255,255), strVolPostProcess );
	m_pFont->DrawText( 2, 60, D3DCOLOR_ARGB(255,255,255,255), "(Enter) : Play/Pause" );
	m_pFont->DrawText( 2, 80, D3DCOLOR_ARGB(255,255,255,255), "(A/S) : Increase/Decrease Plane Speed" );
	m_pFont->DrawText( 2, 100, D3DCOLOR_ARGB(255,255,255,255), "(O) : Toggle PostProcess" );
	*/
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function does the overall rendering of the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
	if( m_bApplyPostProccess )
	{
		WriteRenderTarget();
		RenderDepthBuffer();
		RenderColorBuffer();
	}
	else
		RenderNotVolumetric();

    return S_OK;
}