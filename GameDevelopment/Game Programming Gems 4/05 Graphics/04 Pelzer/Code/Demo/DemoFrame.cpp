//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Subproject:			ShadowsDemo
//*				File:				DemoFrame.cpp
//*
//********************************************************************************************************************
//*
//*				Author:				Kurt Pelzer
//*									Copyright (c) 2002 Kurt Pelzer
//*									All rights reserved.
//*
//********************************************************************************************************************
//********************************************************************************************************************




#include		"DemoFrame.h"
#include		<direct.h>




//********************************************************************************************************************
//*				DEMOFRAME
//********************************************************************************************************************

DEMOFRAME::DEMOFRAME											(	HWND						a_hWnd,
																	IGNU32						a_u32APIType )
{
	//----------------------------------------------------------|------------------------------------------DEMOFRAME-|
	// public method
	// Constructor
	
	m_hGfxLibrary				= NULL;
	m_hInpLibrary				= NULL;
	m_hTmrLibrary				= NULL;
	m_pGfxInstanceInterface		= NULL;
	m_pGfxContextInterface		= NULL;
	m_pMixer					= NULL;
	m_pAssistant				= NULL;
	m_pGauge					= NULL;
	m_pInpInstanceInterface		= NULL;
	m_pInpContextInterface		= NULL;
	m_pKeyboard					= NULL;
	m_pTmrInstanceInterface		= NULL;
	m_pTmrContextInterface		= NULL;
	m_pTimerInterface			= NULL;
	m_pCamera					= NULL;
	m_pScene					= NULL;
	m_fCurrentTimeStamp			= 0.0f;
	m_ShadowTechnique			= ENUM_TECHNIQUE_COMPLETECOMBINEDBUFFER;
	m_bColoredLight				= false;
	m_u32LightCount				= 1;
	m_bDisplayInfo				= false;
	m_u32APIType				= a_u32APIType;

	//
	// Initialize the system
	//
	Init( a_hWnd, a_u32APIType );
}


DEMOFRAME::~DEMOFRAME											()
{
	//----------------------------------------------------------|-----------------------------------------~DEMOFRAME-|
	// public method
	// Destructor

	//
	// Finalize the system
	//
	Finit();
}


IGN_OBJIF_GfxContext* DEMOFRAME::StartCompatibleContext			(	HWND						a_hWnd )
{
	//----------------------------------------------------------|-----------------------------StartCompatibleContext-|
	// public method
	// Start the subsystem with a compatible context.

	//
	// Get device caps
	//
	HDC hdc					= GetDC( 0 );
	IGNU32 u32DesktopWidth	= GetDeviceCaps( hdc, HORZRES );
	IGNU32 u32DesktopHeight	= GetDeviceCaps( hdc, VERTRES );
	IGNU32 u32DesktopBPP	= GetDeviceCaps( hdc, BITSPIXEL );
	IGN_ENUM_PIXELFORMAT pixelFormat;
	if( 32==u32DesktopBPP )
		pixelFormat = IGN_ENUM_PIXELFORMAT_X8R8G8B8;
	else
		pixelFormat = IGN_ENUM_PIXELFORMAT_R5G6B5;
	ReleaseDC( 0, hdc );

	//
	// Search and create context
	//
	if( NULL!=m_pGfxInstanceInterface )
	{
		IGNU32 u32ContextCount;
		m_pGfxInstanceInterface->GetContextCount( u32ContextCount );
		IGN_OBJIF_GfxContext* pGfxContextInterface = NULL;
		for( IGNU32 i = 0; i<u32ContextCount; i++ )
		{
			IGN_STRUCT_GfxContextDesc contextDesc;
			m_pGfxInstanceInterface->EnumContext( i, contextDesc );
			if( ( u32DesktopWidth==contextDesc.m_ModeDesc.m_u32Width )&&
				( u32DesktopHeight==contextDesc.m_ModeDesc.m_u32Height )&&
				( pixelFormat==contextDesc.m_ModeDesc.m_Format ) )
			{
				m_pGfxInstanceInterface->CreateContext( i, 
														a_hWnd,
														IGN_ENUM_GFXCONTEXTCREATE_MIXVERTEXPROCESSING,
														true,
														IGN_ENUM_MULTISAMPLE_NONE,
														pGfxContextInterface );
				break;
			}
		}
		return pGfxContextInterface;
	}

	return NULL;
}


void DEMOFRAME::Init											(	HWND						a_hWnd,
																	IGNU32						a_u32APIType )
{
	//----------------------------------------------------------|-----------------------------------------------Init-|
	// public method
	// Initializes the tester.

	_chdir( "System" );
	IGNRESULT hResult = 0;

	//
	// Load IGN_graphic_sub.dll and create objects
	//
	IGN_ENUM_GFXINSTANCETYPE instanceType;
	switch( a_u32APIType )
	{
	case	1:
			instanceType = IGN_ENUM_GFXINSTANCETYPE_DIRECTX90;
			break;
	case	2:
			instanceType = IGN_ENUM_GFXINSTANCETYPE_DIRECTX81;
			break;
	}
	IGN_STRUCT_IFSubGraphic* pSubGraphic = NULL;
	m_hGfxLibrary = LoadLibrary( "IGN_graphic_sub.dll" );
	IGN_METHOD_GetSubHookGraphic* 
			GfxGetInterface = reinterpret_cast<IGN_METHOD_GetSubHookGraphic*>( GetProcAddress( m_hGfxLibrary, "GetInterface" ) );
	if( NULL!=GfxGetInterface )
		hResult = GfxGetInterface( &pSubGraphic );
	if( NULL!=pSubGraphic )
		hResult = pSubGraphic->m_pCreateInstance( instanceType, m_pGfxInstanceInterface );
	if( NULL!=m_pGfxInstanceInterface )
		m_pGfxContextInterface	= StartCompatibleContext( a_hWnd );
	if( NULL!=m_pGfxContextInterface )
	{
		m_pGfxContextInterface->CreateMixer( m_pMixer );
		if( NULL!=m_pMixer )
		{
			m_pMixer->SetMasterLighting( true );
			m_pMixer->SetMasterAlphaBlending( true );
		}
		m_pGfxContextInterface->CreateAssistant( m_pAssistant );
		m_pGfxContextInterface->CreateGauge( m_pGauge );
		if( NULL!=m_pGauge )
			m_pGauge->SetModus( IGN_ENUM_GAUGEMODUS_SIMPLE ); //IGN_ENUM_GAUGEMODUS_ADVANCED //IGN_ENUM_GAUGEMODUS_DISABLED
	}

	//
	// Load IGN_input_sub.dll and create objects
	//
	IGN_STRUCT_IFSubInput* pSubInput = NULL;
	m_hInpLibrary = LoadLibrary( "IGN_input_sub.dll" );
	IGN_METHOD_GetSubHookInput* 
			InpGetInterface = reinterpret_cast<IGN_METHOD_GetSubHookInput*>( GetProcAddress( m_hInpLibrary, "GetInterface" ) );
	if( NULL!=InpGetInterface )
		hResult = InpGetInterface( &pSubInput );
	if( NULL!=pSubInput )
		hResult = pSubInput->m_pCreateInstance( IGN_ENUM_INPINSTANCETYPE_STANDARD, m_pInpInstanceInterface );
	if( NULL!=m_pInpInstanceInterface )
		m_pInpInstanceInterface->CreateContext( 1, a_hWnd, m_pInpContextInterface );
	if( NULL!=m_pInpContextInterface )
	{
		m_pInpContextInterface->CreateKeyboard( static_cast<IGN_ENUM_COOPLEVEL>( IGN_ENUM_COOPLEVEL_NONEXCLUSIVE|IGN_ENUM_COOPLEVEL_FOREGROUND ), m_pKeyboard );
	}

	//
	// Load IGN_timer_sub.dll and create objects
	//
	IGN_STRUCT_IFSubTimer* pSubTimer = NULL;
	m_hTmrLibrary = LoadLibrary( "Ign_timer_sub.dll" );
	IGN_METHOD_GetSubHookTimer* 
			TmrGetInterface = reinterpret_cast<IGN_METHOD_GetSubHookTimer*>( GetProcAddress( m_hTmrLibrary, "GetInterface" ) );
	if( NULL!=TmrGetInterface )
		hResult = TmrGetInterface( &pSubTimer );
	if( NULL!=pSubTimer )
		hResult = pSubTimer->m_pCreateInstance( IGN_ENUM_TMRINSTANCETYPE_WINDOWSMULTIMEDIA, m_pTmrInstanceInterface );
	if( NULL!=m_pTmrInstanceInterface )
		m_pTmrInstanceInterface->CreateContext( 1, m_pTmrContextInterface );
	if( NULL!=m_pTmrContextInterface )
		m_pTmrContextInterface->CreateTimer( m_pTimerInterface );
	if( NULL!=m_pTimerInterface )
	{
		m_pTimerInterface->Reset();
		m_pTimerInterface->Resume();
	}

	_chdir( "..\\" );

	//
	// Create camera and skydome
	//
	m_pCamera	= new CAMERA( a_hWnd, m_pGfxContextInterface );
	m_pScene	= new SCENE( instanceType, m_pGfxContextInterface, m_pMixer );
}


void DEMOFRAME::Finit											(	void )
{
	//----------------------------------------------------------|----------------------------------------------Finit-|
	// public method
	// Finalizes the tester.

	//
	// Delete camera and skydome
	//
	if( NULL!=m_pCamera )
		delete m_pCamera;
	if( NULL!=m_pScene )
		delete m_pScene;

	//
	// Release objects in IGN_timer_sub.dll
	//
	if( NULL!=m_pTimerInterface )
		m_pTimerInterface->Release();
	if( NULL!=m_pTmrContextInterface )
		m_pTmrContextInterface->Release();
	if( NULL!=m_pTmrInstanceInterface )
		m_pTmrInstanceInterface->Release();
	if( NULL!=m_hTmrLibrary )
		FreeLibrary( m_hTmrLibrary );

	//
	// Release objects in IGN_input_sub.dll
	//
	if( NULL!=m_pKeyboard )
		m_pKeyboard->Release();
	if( NULL!=m_pInpContextInterface )
		m_pInpContextInterface->Release();
	if( NULL!=m_pInpInstanceInterface )
		m_pInpInstanceInterface->Release();
	if( NULL!=m_hInpLibrary )
		FreeLibrary( m_hInpLibrary );

	//
	// Release objects in IGN_graphic_sub.dll
	//
	if( NULL!=m_pGauge )
		m_pGauge->Release();
	if( NULL!=m_pAssistant )
		m_pAssistant->Release();
	if( NULL!=m_pMixer )
		m_pMixer->Release();
	if( NULL!=m_pGfxContextInterface )
		m_pGfxContextInterface->Release();
	if( NULL!=m_pGfxInstanceInterface )
		m_pGfxInstanceInterface->Release();
	if( NULL!=m_hGfxLibrary )
		FreeLibrary( m_hGfxLibrary );
}


void DEMOFRAME::ShowInfo										(	void )
{
	//----------------------------------------------------------|-------------------------------------------ShowInfo-|
	// public method
	// Presents the current info data.

	RECT	rect;
	rect.left = rect.top = 25;
	rect.right = rect.bottom = 0;
	char string[500];
	strcpy( string, "Shadow Mapping Technique: Combined Shadow Buffers\n" );
	switch( m_ShadowTechnique )
	{
	case	ENUM_TECHNIQUE_REDUCEDCOMBINEDBUFFER:
			strcat( string, "Version:  Reduced (fixed function)\n\n" );
			break;
	case	ENUM_TECHNIQUE_COMPLETECOMBINEDBUFFER:
			strcat( string, "Version:  Complete (fixed function)\n\n" );
			break;
	}
	if( false==m_bDisplayInfo )
		strcat( string, "Key Infos:  Press F1\n" );
	else
	{
		strcat( string, "KEY INFOS\n" );
		strcat( string, "F1  - Display Key Infos\n" );
		strcat( string, "F2  - Hide Key Infos\n" );
		strcat( string, "F3  - Reduced Version\n" );
		strcat( string, "F4  - Complete Version\n" );
		strcat( string, "F5  - One Light\n" );
		strcat( string, "F6  - Two Lights\n" );
		strcat( string, "F7  - White Lights\n" );
		strcat( string, "F8  - Colored Lights\n" );
		strcat( string, "ESC - Quit\n" );
	}
	strcat( string, "\0" );
	if( NULL!=m_pAssistant )
		m_pAssistant->DrawText( string, 0xff60ff60, rect );
}


IGNBOOL DEMOFRAME::ProceedNextFrame								(	HWND						a_hWnd )
{
	//----------------------------------------------------------|-----------------------------------ProceedNextFrame-|
	// public method
	// Proceeds the next frame.

	if( NULL!=m_pMixer )
	{
		//
		// Get the client rect of the application's window
		//
		RECT Rect;
		GetClientRect( a_hWnd, &Rect );
		Rect.right	= Rect.right - Rect.left;
		Rect.bottom	= Rect.bottom - Rect.top;
		Rect.left	= 0;
		Rect.top	= 0;

		if( (0<Rect.right)&&(0<Rect.bottom) )
		{
			//
			// Set and clear the current view port
			//
			IGN_STRUCT_Viewport Viewport;
			Viewport.m_u32X			= 0;
			Viewport.m_u32Y			= 0;
			Viewport.m_u32Width		= Rect.right;
			Viewport.m_u32Height	= Rect.bottom;
			Viewport.m_fMinZ		= 0.0f;
			Viewport.m_fMaxZ		= 1.0f;
			m_pMixer->SetViewport( Viewport );
			m_pMixer->ClearViewport( 
				static_cast<IGN_ENUM_VIEWPORTCLEAR>( IGN_ENUM_VIEWPORTCLEAR_COLOR|IGN_ENUM_VIEWPORTCLEAR_DEPTH|IGN_ENUM_VIEWPORTCLEAR_STENCIL ), 
				0x00000000, 
				1.0f, 
				0 );

			//
			// Proceed with the camera
			//
			if( NULL!=m_pCamera )
				m_pCamera->Proceed();
			else
				return false;

			//
			// Begin a new scene
			//
			m_pMixer->BeginScene();

			//
			// Proceed with the scene
			//
			if( NULL!=m_pScene )
				m_pScene->Proceed(	m_pCamera, 
									m_ShadowTechnique, 
									m_u32LightCount, 
									m_bColoredLight, 
									true );
			else 
				return false;

			//
			// Display the system info
			//
			ShowInfo();

			//
			// End the scene and swap the screen
			//
			m_pMixer->EndScene();
			m_pMixer->SwapScreen( NULL, 
								  &Rect, 
								  NULL );
		}
		return true;
	}

	return false;
}


IGNBOOL DEMOFRAME::Proceed										(	HWND						a_hWnd,
																	MSG*						a_pMsg )
{
	//----------------------------------------------------------|--------------------------------------------Proceed-|
	// public method
	// Proceeds the next frame.
	
	//
	// Execute keyboard
	//
	if( NULL!=m_pKeyboard )
	{
		m_pKeyboard->Execute();

		//
		// Get current keyboard data and check for escape ...
		//
		IGN_STRUCT_KeyboardState State;
		m_pKeyboard->GetState( State );
		if( true==State.m_bKeyDown[IGN_ENUM_INPKEY_ESCAPE] )
			return false;
		if( true==State.m_bKeyDown[IGN_ENUM_INPKEY_F1] )
			m_bDisplayInfo = true;
		if( true==State.m_bKeyDown[IGN_ENUM_INPKEY_F2] )
			m_bDisplayInfo = false;
		if( true==State.m_bKeyDown[IGN_ENUM_INPKEY_F3] )
			m_ShadowTechnique = ENUM_TECHNIQUE_REDUCEDCOMBINEDBUFFER;
		if( true==State.m_bKeyDown[IGN_ENUM_INPKEY_F4] )
			m_ShadowTechnique = ENUM_TECHNIQUE_COMPLETECOMBINEDBUFFER;
		if( true==State.m_bKeyDown[IGN_ENUM_INPKEY_F5] )
			m_u32LightCount = 1;
		if( true==State.m_bKeyDown[IGN_ENUM_INPKEY_F6] )
			m_u32LightCount = 2;
		if( true==State.m_bKeyDown[IGN_ENUM_INPKEY_F7] )
			m_bColoredLight = false;
		if( true==State.m_bKeyDown[IGN_ENUM_INPKEY_F8] )
			m_bColoredLight = true;
	}

	//
	// Proceed the complete next frame
	//
	return ProceedNextFrame( a_hWnd );
}
