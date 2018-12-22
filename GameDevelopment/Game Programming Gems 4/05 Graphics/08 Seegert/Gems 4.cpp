//-----------------------------------------------------------------------------
// File: Gems 4.cpp
//
// Desc: DirectX window application created by the DirectX AppWizard
//-----------------------------------------------------------------------------
#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <commdlg.h>
#include <basetsd.h>
#include <math.h>
#include <stdio.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include <tchar.h>
#include "DXUtil.h"
#include "D3DEnumeration.h"
#include "D3DSettings.h"
#include "D3DApp.h"
#include "D3DFont.h"
#include "D3DUtil.h"
#include "resource.h"
#include "Gems 4.h"




//-----------------------------------------------------------------------------
// Global access to the app (needed for the global WndProc())
//-----------------------------------------------------------------------------
CMyD3DApplication* g_pApp  = NULL;
HINSTANCE          g_hInst = NULL;




//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    CMyD3DApplication d3dApp;

    g_pApp  = &d3dApp;
    g_hInst = hInst;

    InitCommonControls();
    if( FAILED( d3dApp.Create( hInst ) ) )
        return 0;

    return d3dApp.Run();
}


/*
 * SLSMachine::SLSMachine()
 *
 * Inits the stupid little state machine.
 */
SLSMachine::SLSMachine(float inPauseTime)
{
	/* init state durations */
	this->mStateDurations[kslsmPause]					= inPauseTime;
	this->mStateDurations[kslsmChooseNewColor]			= 0.0F;
	this->mStateDurations[kslsmInterpolateToNewColor]	= 0.5F;

	/* init colors and seed random number */
	srand((uint32)DXUtil_Timer(TIMER_GETABSOLUTETIME));
	this->SetRandomColor(this->mEndingColor);
	this->mCurrentColor.r = this->mStartingColor.r = this->mEndingColor.r;
	this->mCurrentColor.g = this->mStartingColor.g = this->mEndingColor.g;
	this->mCurrentColor.b = this->mStartingColor.b = this->mEndingColor.b;

	/* initialize our starting state */
	this->SetNewState(kslsmPause);
}


/*
 * SLSMachine::Update()
 *
 * Updates the stupid little state machine.
 */
void
SLSMachine::Update(float inTimePassed)
{
	float	theRatio;
	bool	theStateDone;

	/* update our state timer */
	this->mCurrentStateTimer -= inTimePassed;

	/* are we done with this state? */
	theStateDone = this->mCurrentStateTimer <= 0.0F;

	/* compute our completion ratio */
	theRatio = 1.0F - (this->mCurrentStateTimer / this->mStateDurations[this->mCurrentState]);
	
	/* do state stuff */
	switch (this->mCurrentState)
	{
	case kslsmPause:

		if (theStateDone)
		{
			/* set a new state */
			this->SetNewState(kslsmChooseNewColor);
		}
		break;

	case kslsmChooseNewColor:

		if (theStateDone)
		{
			/* pick a new color */
			this->SetRandomColor(this->mEndingColor);
			
			/* set a new state */
			this->SetNewState(kslsmInterpolateToNewColor);
		}
		break;

	case kslsmInterpolateToNewColor:

		if (theStateDone)
		{
			/* save off the new color */
			this->mStartingColor.r = this->mEndingColor.r;
			this->mStartingColor.g = this->mEndingColor.g;
			this->mStartingColor.b = this->mEndingColor.b;

			/* set a new state */
			this->SetNewState(kslsmPause);
		}
		else
		{
			/* compute our current color */
			float theInterpolationRatio = 1.0F - theRatio;
			theInterpolationRatio = 1.0F - (theInterpolationRatio * theInterpolationRatio);
			this->mCurrentColor.r = (this->mStartingColor.r * (1.0F - theInterpolationRatio)) + (this->mEndingColor.r * theInterpolationRatio);
			this->mCurrentColor.g = (this->mStartingColor.g * (1.0F - theInterpolationRatio)) + (this->mEndingColor.g * theInterpolationRatio);
			this->mCurrentColor.b = (this->mStartingColor.b * (1.0F - theInterpolationRatio)) + (this->mEndingColor.b * theInterpolationRatio);
		}
		break;

	default:
		break;
	}
}


//-----------------------------------------------------------------------------
// Name: CMyD3DApplication()
// Desc: Application constructor.   Paired with ~CMyD3DApplication()
//       Member variables should be initialized to a known state here.  
//       The application window has not yet been created and no Direct3D device 
//       has been created, so any initialization that depends on a window or 
//       Direct3D should be deferred to a later stage. 
//-----------------------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication() : mMajorColorStateMachine(3.0F), mMinorColorStateMachine(0.5F)
{
	for (uint32 theLoop = 0; theLoop < kgg4tctMax; theLoop++)
		this->mTestModels[theLoop] = NULL;
	this->mTeamColorTechnique	= kgg4tctTextureMaskingMultiTexture;

    m_dwCreationWidth           = 500;
    m_dwCreationHeight          = 375;
    m_strWindowTitle            = TEXT( "Game Programming Gems 4 - Team Color" );
    m_d3dEnumeration.AppUsesDepthBuffer   = TRUE;
	m_bStartFullscreen			= true;
	m_bShowCursorWhenFullscreen	= false;

    // Create a D3D font using d3dfont.cpp
    m_pFont                     = new CD3DFont( _T("Tahoma"), 10, D3DFONT_BOLD );
    m_bLoadingApp               = TRUE;

    ZeroMemory( &m_UserInput, sizeof(m_UserInput) );
    m_fWorldRotX                = -(D3DX_PI / 2);
    m_fWorldRotY                =  (D3DX_PI / 4);
	m_fCameraRotX				= 0.0F;
	m_fCameraRotY				= 0.0F;
}




//-----------------------------------------------------------------------------
// Name: ~CMyD3DApplication()
// Desc: Application destructor.  Paired with CMyD3DApplication()
//-----------------------------------------------------------------------------
CMyD3DApplication::~CMyD3DApplication()
{
}




//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Paired with FinalCleanup().
//       The window has been created and the IDirect3D9 interface has been
//       created, but the device has not been created yet.  Here you can
//       perform application-related initialization and cleanup that does
//       not depend on a device.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::OneTimeSceneInit()
{
    // TODO: perform one time initialization

    // Drawing loading status message until app finishes loading
    SendMessage( m_hWnd, WM_PAINT, 0, 0 );

    m_bLoadingApp = FALSE;

    return S_OK;
}









//-----------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: Called during device initialization, this code checks the display device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior,
                                          D3DFORMAT Format )
{
    UNREFERENCED_PARAMETER( Format );
    UNREFERENCED_PARAMETER( dwBehavior );
    UNREFERENCED_PARAMETER( pCaps );
    
    BOOL bCapsAcceptable;

    // TODO: Perform checks to see if these display caps are acceptable.
	if (dwBehavior == D3DCREATE_SOFTWARE_VERTEXPROCESSING)
	{
		bCapsAcceptable = TRUE;
	}
	else
	{
		bCapsAcceptable = (pCaps->VertexShaderVersion >= D3DVS_VERSION(1,1));
	}

    if( bCapsAcceptable )         
        return S_OK;
    else
        return E_FAIL;
}




//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Paired with DeleteDeviceObjects()
//       The device has been created.  Resources that are not lost on
//       Reset() can be created here -- resources in D3DPOOL_MANAGED,
//       D3DPOOL_SCRATCH, or D3DPOOL_SYSTEMMEM.  Image surfaces created via
//       CreateImageSurface are never lost and can be created here.  Vertex
//       shaders and pixel shaders can also be created here as they are not
//       lost on Reset().
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects()
{
	for (uint32 theLoop = 0; theLoop < kgg4tctMax; theLoop++)
	{
		/* create a new model */
		this->mTestModels[theLoop] = new GG4Model();

		/* the poly tinting technique requires a different texture */
		if (theLoop == kgg4tctPolygonTinting)
			this->mTestModels[theLoop]->Load(this->m_pd3dDevice, "Data\\crusader.model", "Data\\crusader_noalpha.tga", "Data\\colorblend.tga");
		else
			this->mTestModels[theLoop]->Load(this->m_pd3dDevice, "Data\\crusader.model", "Data\\crusader_alpha.tga", "Data\\colorblend.tga");

		/* load shaders */
		this->mTestModels[theLoop]->LoadShaders(this->m_pd3dDevice, "Data\\teamcolor.vsh", "Data\\teamcolor.psh");
	}
    
	// Init the font
    m_pFont->InitDeviceObjects( m_pd3dDevice );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Paired with InvalidateDeviceObjects()
//       The device exists, but may have just been Reset().  Resources in
//       D3DPOOL_DEFAULT and any other device state that persists during
//       rendering should be set here.  Render states, matrices, textures,
//       etc., that don't change during rendering can be set once here to
//       avoid redundant state setting during Render() or FrameMove().
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
    // Setup a material
    D3DMATERIAL9 mtrl;
    D3DUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f );
    m_pd3dDevice->SetMaterial( &mtrl );

    // Set up the textures
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

    // Set miscellaneous render states
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,   FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,        TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,        0);

	// Set up lighting states
    D3DLIGHT9 light;
    D3DUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, -1.0f, -1.0f, 2.0f );
    m_pd3dDevice->SetLight( 0, &light );
    m_pd3dDevice->LightEnable( 0, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

    // Set the world matrix
    D3DXMATRIX matIdentity;
	D3DXMATRIX theTransposeWorld;
    D3DXMatrixIdentity( &matIdentity );
    m_pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

	/* set app specific shader constants */
	float		theZero[] = { 0.0F, 0.0F, 0.0F, 0.0F };
	float		theOne[]  = { 1.0F, 1.0F, 1.0F, 1.0F };
	float		theLightColor[4];
	float		theLightDirection[4];
	
	theLightColor[0] = light.Diffuse.r;
	theLightColor[1] = light.Diffuse.g;
	theLightColor[2] = light.Diffuse.b;
	theLightColor[3] = 1.0F;
	theLightDirection[0] = -light.Direction.x;
	theLightDirection[1] = -light.Direction.y;
	theLightDirection[2] = -light.Direction.z;
	theLightDirection[3] = 1.0F;
	D3DXMatrixTranspose(&theTransposeWorld,	&matIdentity);
	m_pd3dDevice->SetPixelShaderConstantF (kgg4pscZero,					theZero,							1);
	m_pd3dDevice->SetPixelShaderConstantF (kgg4pscOne,					theOne,								1);
	m_pd3dDevice->SetVertexShaderConstantF(kgg4vscZero,					theZero,							1);
	m_pd3dDevice->SetVertexShaderConstantF(kgg4vscOne,					theOne,								1);
	m_pd3dDevice->SetVertexShaderConstantF(kgg4vscLight1Direction,		theLightDirection,					1);
	m_pd3dDevice->SetVertexShaderConstantF(kgg4vscLight1Color,			theLightColor,						1);
	m_pd3dDevice->SetVertexShaderConstantF(kgg4vscTransformWorld,		(float*)&theTransposeWorld,			4);

    // Restore the font
    m_pFont->RestoreDeviceObjects();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove()
{
	static float	sLastUpdateTime = this->m_fTime;

	/* set the current team color technique */
	this->mTestModels[this->mTeamColorTechnique]->SetTeamColorTechnique(this->mTeamColorTechnique);

	/* update the color state machines */
	this->mMajorColorStateMachine.Update(this->m_fElapsedTime);
	this->mMinorColorStateMachine.Update(this->m_fElapsedTime);

	/* set the major team color */
	this->mTestModels[this->mTeamColorTechnique]->SetTeamColorMajor(this->mMajorColorStateMachine.GetCurrentColor().r, 
										this->mMajorColorStateMachine.GetCurrentColor().g, 
										this->mMajorColorStateMachine.GetCurrentColor().b);
	/* set the minor team color */
	this->mTestModels[this->mTeamColorTechnique]->SetTeamColorMinor(this->mMinorColorStateMachine.GetCurrentColor().r, 
										this->mMinorColorStateMachine.GetCurrentColor().g, 
										this->mMinorColorStateMachine.GetCurrentColor().b);

    // Update user input state
    UpdateKeyboardInput( &m_UserInput );

    // Update the world state according to user input
    D3DXMATRIX matWorld;
    D3DXMATRIX matRotY;
    D3DXMATRIX matRotX;

	if (this->m_UserInput.bHadInput)
	{
		if (this->m_UserInput.bObjectRotateLeft && !this->m_UserInput.bObjectRotateRight)
			m_fWorldRotY += m_fElapsedTime;
		else
		if (this->m_UserInput.bObjectRotateRight && !this->m_UserInput.bObjectRotateLeft)
			m_fWorldRotY -= m_fElapsedTime;

		if (this->m_UserInput.bObjectRotateUp && !this->m_UserInput.bObjectRotateDown)
			m_fWorldRotX += m_fElapsedTime;
		else 
		if (this->m_UserInput.bObjectRotateDown && !this->m_UserInput.bObjectRotateUp)
			m_fWorldRotX -= m_fElapsedTime;

		sLastUpdateTime = this->m_fTime;
	}
	else
	{
		float theAmount = min((this->m_fTime - sLastUpdateTime) / 3.0F, 1.0F);
		m_fWorldRotY -= (CUBE(theAmount) * m_fElapsedTime * 0.25F);		
	}

	//this->m_fCameraRotX += (this->m_UserInput.fCameraRotX * this->m_fElapsedTime);
	//this->m_fCameraRotY += (this->m_UserInput.fCameraRotY * this->m_fElapsedTime);
	
    D3DXMatrixRotationX( &matRotX, m_fWorldRotX );
    D3DXMatrixRotationY( &matRotY, m_fWorldRotY );
    D3DXMatrixMultiply( &matWorld, &matRotX, &matRotY );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	/* update the model position */
	this->mTestModels[this->mTeamColorTechnique]->SetTransform(matWorld);


	// Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. Here, we set the
    // eye five units back along the z-axis and up three units, look at the
    // origin, and define "up" to be in the y-direction.
    D3DXMATRIX matView;
	D3DXMATRIX matCameraX;
	D3DXMATRIX matCameraY;
	D3DXMATRIX matCameraRotation;
	D3DXMatrixRotationX(&matCameraX, this->m_fCameraRotX);
    D3DXMatrixRotationY(&matCameraY, this->m_fCameraRotY);
    D3DXMatrixMultiply(&matCameraRotation, &matCameraX, &matCameraY);

    D3DXVECTOR3 vFromPt   = D3DXVECTOR3( 0.0f, 0.0f, -2.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.5f, 0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 vFromPtTransformed;
    D3DXVECTOR3 vLookatPtTransformed;
    D3DXVECTOR3 vUpVecTransformed;
	D3DXVECTOR4 vTempTransformed;
	D3DXVec3Transform(&vTempTransformed,	&vFromPt,	&matCameraRotation);
	vFromPtTransformed = D3DXVECTOR3(vTempTransformed.x, vTempTransformed.y, vTempTransformed.z);
	D3DXVec3Transform(&vTempTransformed,	&vLookatPt, &matCameraRotation);
	vLookatPtTransformed = D3DXVECTOR3(vTempTransformed.x, vTempTransformed.y, vTempTransformed.z);
	D3DXVec3Transform(&vTempTransformed,	&vUpVec,	&matCameraRotation);
	vUpVecTransformed = D3DXVECTOR3(vTempTransformed.x, vTempTransformed.y, vTempTransformed.z);
    D3DXMatrixLookAtLH(&matView, &vFromPtTransformed, &vLookatPtTransformed, &vUpVecTransformed);
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // Set the projection matrix
    D3DXMATRIX matProj;
    FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width) / m_d3dsdBackBuffer.Height;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspect, 1.0f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	float		theEye[4];
	D3DXMATRIX	theTransposeProjection;
	D3DXMATRIX	theTransposeView;

	theEye[0] = vFromPtTransformed.x;
	theEye[1] = vFromPtTransformed.y;
	theEye[2] = vFromPtTransformed.z;
	theEye[3] = 1.0F;
	
	D3DXMatrixTranspose(&theTransposeProjection,	&matProj);
	D3DXMatrixTranspose(&theTransposeView,			&matView);
	m_pd3dDevice->SetVertexShaderConstantF(kgg4vscEyePosition,			theEye,								1);
	m_pd3dDevice->SetVertexShaderConstantF(kgg4vscTransformProjection,	(float*)&theTransposeProjection,	4);
	m_pd3dDevice->SetVertexShaderConstantF(kgg4vscTransformView,		(float*)&theTransposeView,			4);

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: UpdateInput()
// Desc: Update the user input.  Called once per frame 
//-----------------------------------------------------------------------------
void CMyD3DApplication::UpdateKeyboardInput( UserInput* pUserInput )
{
    pUserInput->bObjectRotateUp		= ( m_bActive && (GetAsyncKeyState( VK_UP )    & 0x8000) == 0x8000 );
    pUserInput->bObjectRotateDown	= ( m_bActive && (GetAsyncKeyState( VK_DOWN )  & 0x8000) == 0x8000 );
    pUserInput->bObjectRotateLeft	= ( m_bActive && (GetAsyncKeyState( VK_LEFT )  & 0x8000) == 0x8000 );
    pUserInput->bObjectRotateRight	= ( m_bActive && (GetAsyncKeyState( VK_RIGHT ) & 0x8000) == 0x8000 );
	pUserInput->bHadInput			= (pUserInput->bObjectRotateUp || pUserInput->bObjectRotateDown || pUserInput->bObjectRotateLeft || pUserInput->bObjectRotateRight) ||
									  ((pUserInput->fCameraRotX != 0.0F) || (pUserInput->fCameraRotY != 0.0F));
}

void CMyD3DApplication::UpdateMouseInput(UserInput* pUserInput, HWND /*hWnd*/, UINT uMsg, WPARAM /*wParam*/, LPARAM lParam)
{
	static int	sOldMouseX;
	static int	sOldMouseY;

	int iMouseX = GET_X_LPARAM(lParam);
    int iMouseY = GET_Y_LPARAM(lParam);

	pUserInput->fCameraRotX = 0.0F;
	pUserInput->fCameraRotY = 0.0F;

	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		pUserInput->bLeftDrag = TRUE;
		sOldMouseX = iMouseX;
		sOldMouseY = iMouseY;
		break;

	case WM_LBUTTONUP:
		pUserInput->bLeftDrag = FALSE;
		break;

	case WM_MOUSEMOVE:
		if (pUserInput->bLeftDrag)
		{
			pUserInput->fCameraRotY = ((float)(iMouseX - sOldMouseX)) / 50.0F;
			pUserInput->fCameraRotX = ((float)(iMouseY - sOldMouseY)) / 50.0F;
		}
		break;

	default:
		break;
	}
}



//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
	bool theSuccess;

    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         0x00332222, 1.0f, 0L );

    // Begin the scene
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
		/* render the current model */
		theSuccess = this->mTestModels[this->mTeamColorTechnique]->Render(this->m_pd3dDevice);

        // Render stats and help text  
        RenderText(theSuccess);

        // End the scene.
        m_pd3dDevice->EndScene();
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderText()
// Desc: Renders stats and help text to the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RenderText(bool inSuccessfulRender)
{
    D3DCOLOR fontColor    = D3DCOLOR_ARGB(255,255,255,255);
    TCHAR szMsg[MAX_PATH] = TEXT("");
	TCHAR*	theTeamColorStyle;

	/* call the appropriate render */
	switch (this->mTeamColorTechnique)
	{
	case kgg4tctPolygonTinting:
		theTeamColorStyle = "Polygon Tinting";
		break;

	case kgg4tctTextureMaskingMultiTexture:
		theTeamColorStyle = "Texture Mask - Multi Texture";
		break;

	case kgg4tctTextureMaskingMultiPass:
		theTeamColorStyle = "Texture Mask - Multi Pass";
		break;

	case kgg4tctVertexAndPixelShader:
		theTeamColorStyle = "Advanced Vertex and Pixel Shaders";
		break;

	default:
		theTeamColorStyle = "Unknown";
		break;
	}

    // Output display stats
    FLOAT fNextLine = 40.0f; 

    lstrcpy( szMsg, m_strDeviceStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, m_strFrameStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

	if (inSuccessfulRender == false)
	{
		sprintf(szMsg, "***** Unable to render current team color style *****");
		m_pFont->DrawText( 10, 60.0F, fontColor, szMsg );
	}

    // Output statistics & help
    fNextLine = (FLOAT) m_d3dsdBackBuffer.Height; 

    lstrcpy( szMsg, TEXT("Use arrow keys to rotate object") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, TEXT("Press 'F2' to configure display") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

	sprintf(szMsg, "Press 'F3' to change Team Color Style");
	fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

	sprintf(szMsg, "Team Color Style '%s'", theTeamColorStyle);
	fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Overrrides the main WndProc, so the sample can do custom message
//       handling (e.g. processing mouse, keyboard, or menu commands).
//-----------------------------------------------------------------------------
LRESULT CMyD3DApplication::MsgProc( HWND hWnd, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
{
    switch( msg )
    {
        case WM_PAINT:
        {
            if( m_bLoadingApp )
            {
                // Draw on the window tell the user that the app is loading
                // TODO: change as needed
                HDC hDC = GetDC( hWnd );
                TCHAR strMsg[MAX_PATH];
                wsprintf( strMsg, TEXT("Loading... Please wait") );
                RECT rct;
                GetClientRect( hWnd, &rct );
                DrawText( hDC, strMsg, -1, &rct, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
                ReleaseDC( hWnd, hDC );
            }
            break;
        }

		case WM_COMMAND:
		{
            switch( LOWORD(wParam) )
            {
                case IDM_CHANGETEAMCOLOR:
					this->mTeamColorTechnique = (GG4TeamColorTechnique)((this->mTeamColorTechnique + 1) % kgg4tctMax);
					break;
			}
			break;
		}
    }

	/* update mouse input */
	UpdateMouseInput(&this->m_UserInput, hWnd, msg, wParam, lParam);

    return CD3DApplication::MsgProc( hWnd, msg, wParam, lParam );
}




//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Invalidates device objects.  Paired with RestoreDeviceObjects()
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
    // TODO: Cleanup any objects created in RestoreDeviceObjects()
    m_pFont->InvalidateDeviceObjects();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Paired with InitDeviceObjects()
//       Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.  
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
	for (uint32 theLoop = 0; theLoop < kgg4tctMax; theLoop++)
		delete this->mTestModels[theLoop];

    m_pFont->DeleteDeviceObjects();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Paired with OneTimeSceneInit()
//       Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup()
{
    // TODO: Perform any final cleanup needed
    // Cleanup D3D font
    SAFE_DELETE( m_pFont );

    return S_OK;
}




