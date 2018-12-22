//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Subproject:			SimpleShadersDemo
//*				File:				Camera.cpp
//*
//********************************************************************************************************************
//*
//*				Author:				Kurt Pelzer
//*									Copyright (c) 2002, 2003 Kurt Pelzer
//*									All rights reserved.
//*
//********************************************************************************************************************
//********************************************************************************************************************




#include		"Camera.h"
#include		<direct.h>




//********************************************************************************************************************
//*				CAMERA
//********************************************************************************************************************

CAMERA::CAMERA													(	HWND					a_hWnd,
																	IGN_OBJIF_GfxContext*	a_pGfxContextInterface )
{
	//----------------------------------------------------------|---------------------------------------------CAMERA-|
	// public method
	// Constructor
	
	m_hCamLibrary			= NULL;
	m_pCamInstanceInterface	= NULL;
	m_pCamContextInterface	= NULL;
	m_pFreeCameraInterface	= NULL;

	//
	// Initialize the system
	//
	Init( a_hWnd, a_pGfxContextInterface );
}


CAMERA::~CAMERA											()
{
	//----------------------------------------------------------|--------------------------------------------~CAMERA-|
	// public method
	// Destructor

	//
	// Finalize the system
	//
	Finit();
}


void CAMERA::Init												(	HWND					a_hWnd,
																	IGN_OBJIF_GfxContext*	a_pGfxContextInterface )
{
	//----------------------------------------------------------|-----------------------------------------------Init-|
	// public method
	// Initializes the tester.

	_chdir( "System" );
	IGNRESULT hResult = 0;

	//
	// Load IGN_camera_sys.dll and create objects
	//
	IGN_STRUCT_IFSysCamera* pSysCamera = NULL;
	m_hCamLibrary = LoadLibrary( "Ign_camera_sys.dll" );
	IGN_METHOD_GetSysHookCamera* 
			CamGetInterface = reinterpret_cast<IGN_METHOD_GetSysHookCamera*>( GetProcAddress( m_hCamLibrary, "GetInterface" ) );
	if( NULL!=CamGetInterface )
		hResult = CamGetInterface( &pSysCamera );
	if( NULL!=pSysCamera )
		hResult = pSysCamera->m_pCreateInstance( IGN_ENUM_CAMINSTANCETYPE_STANDARD, m_pCamInstanceInterface );
	if( NULL!=m_pCamInstanceInterface )
		m_pCamInstanceInterface->CreateContext( 1, m_pCamContextInterface );
	if( NULL!=m_pCamContextInterface )
		m_pCamContextInterface->CreateFreeCamera( m_pFreeCameraInterface );
	if( NULL!=m_pFreeCameraInterface )
	{
		//
		// Initialize the free camera
		//
		RECT Rect;
		GetClientRect( a_hWnd, &Rect );
		m_pFreeCameraInterface->AttachGfxContext( a_pGfxContextInterface );
		IGNFLOAT fAspectRatio = static_cast<IGNFLOAT>(Rect.right - Rect.left)/static_cast<IGNFLOAT>(Rect.bottom- Rect.top);
		m_pFreeCameraInterface->SetFOV( 3.1416f/3.0f, fAspectRatio );
		m_pFreeCameraInterface->SetNearAndFarClipping( 1.0f, 1000.0f );
		m_pFreeCameraInterface->Execute();
	}

	_chdir( "..\\" );
}


void CAMERA::Finit												(	void )
{
	//----------------------------------------------------------|----------------------------------------------Finit-|
	// public method
	// Finalizes the tester.

	//
	// Release objects in IGN_camera_sys.dll
	//
	if( NULL!=m_pCamInstanceInterface )
		m_pCamInstanceInterface->Release();
	if( NULL!=m_pCamContextInterface )
		m_pCamContextInterface->Release();
	if( NULL!=m_pFreeCameraInterface )
		m_pFreeCameraInterface->Release();
	if( NULL!=m_hCamLibrary )
		FreeLibrary( m_hCamLibrary );
}


IGNBOOL CAMERA::ProceedCamera									(	void )
{
	//----------------------------------------------------------|--------------------------------------ProceedCamera-|
	// public method
	// Proceeds the camera.

	//
	// Set position & direction and execute the camera
	//
	if( NULL!=m_pFreeCameraInterface )
	{
		IGNFLOAT fTime = 10.0f;
		IGN_STRUCT_3DVector PositionVector;
		PositionVector.m_fX	=	400.0f*static_cast<IGNFLOAT>( sin( fTime/3.0f ) )*static_cast<IGNFLOAT>( sin( fTime ) );
		PositionVector.m_fY	=	400.0f*static_cast<IGNFLOAT>( cos( fTime ) );
		PositionVector.m_fZ	=	300.0f*static_cast<IGNFLOAT>( sin( fTime/3.0f ) );
		m_pFreeCameraInterface->SetPosition( PositionVector );
		IGN_STRUCT_3DVector ViewDirectionVector;
		ViewDirectionVector.m_fX =	-PositionVector.m_fX;
		ViewDirectionVector.m_fY =	-PositionVector.m_fY;
		ViewDirectionVector.m_fZ =	400.0f - PositionVector.m_fZ;
		m_pFreeCameraInterface->SetViewDirection( ViewDirectionVector );
		if( IGN_OK==m_pFreeCameraInterface->Execute() )
			return true;
	}

	return false;
}


IGNBOOL CAMERA::GetViewMatrix 									(	IGN_STRUCT_4x4Matrix&	a_Matrix )
{
	//----------------------------------------------------------|--------------------------------------GetViewMatrix-|
	// public method
	// Retrieves the current view matrix.

	//
	// Get the matrix
	//
	if( NULL!=m_pFreeCameraInterface )
		m_pFreeCameraInterface->GetViewMatrix( a_Matrix );
	else
		return false;

	return true;
}


IGNBOOL CAMERA::GetProjectionMatrix 							(	IGN_STRUCT_4x4Matrix&	a_Matrix )
{
	//----------------------------------------------------------|--------------------------------GetProjectionMatrix-|
	// public method
	// Proceeds the camera.

	//
	// Get the matrix
	//
	if( NULL!=GetProjectionMatrix )
		m_pFreeCameraInterface->GetProjectionMatrix( a_Matrix );
	else
		return false;

	return true;
}


IGNBOOL CAMERA::ProcessAgain									(	void )
{
	//----------------------------------------------------------|---------------------------------------ProcessAgain-|
	// public method
	// Process the camera again without any updates.

	//
	// Execute the camera
	//
	if( NULL!=m_pFreeCameraInterface )
	{
		if( IGN_OK==m_pFreeCameraInterface->ExecuteWithoutUpdate() )
			return true;
	}

	return false;
}


IGNBOOL CAMERA::Proceed 										(	void )
{
	//----------------------------------------------------------|--------------------------------------------Proceed-|
	// public method
	// Proceeds the camera.

	//
	// Proceed with the camera
	//
	return ProceedCamera();
}
