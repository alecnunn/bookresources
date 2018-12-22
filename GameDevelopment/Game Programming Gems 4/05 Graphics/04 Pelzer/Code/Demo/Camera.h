
//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Project:			IGNITION
//*
//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Subproject:			SimpleShadersDemo
//*				File:				Camera.h
//*				Description:		Camera for shader testing applications
//*
//********************************************************************************************************************
//*
//*				Author:				Kurt Pelzer
//*									Copyright (c) 2002, 2003 Kurt Pelzer
//*									All rights reserved.
//*
//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Status:				In progress
//*
//********************************************************************************************************************
//*
//*				History:
//*
//*				08/29/03| 0.0.1		## Initial release	(kp) ##
//*
//********************************************************************************************************************
//********************************************************************************************************************




//------------- PREPROCESSOR -----------------------------------------------------------------------------------------

#ifndef			CAMERA_H_INCLUDED
#define			CAMERA_H_INCLUDED

#if				_MSC_VER > 1000
#pragma once
#ifdef			IGN_SWITCH_SHOW_HEADER
#pragma message ("---> Camera.h\n")
#endif
#endif 




//------------- DEPENDENCIES -----------------------------------------------------------------------------------------

#include "Shared.h"




//------------- FORWARD DECLARATIONS ---------------------------------------------------------------------------------
//------------- DEFINITIONS ------------------------------------------------------------------------------------------




//------------- DECLARATIONS -----------------------------------------------------------------------------------------

class CAMERA
{
public:

	//
	// Constructors/Destructors
	//
	CAMERA		(	HWND					a_hWnd,
					IGN_OBJIF_GfxContext*	a_pGfxContextInterface );
	~CAMERA		();

protected:
	
	//
	// Handles of used dynamic link libraries
	//
	HMODULE						m_hCamLibrary;
	HMODULE						m_hTmrLibrary;

	//
	// Objects in IGN_camera_sys.dll
	//
	IGN_OBJIF_CamInstance*		m_pCamInstanceInterface;
	IGN_OBJIF_CamContext*		m_pCamContextInterface;
	IGN_OBJIF_CamFreeCamera*	m_pFreeCameraInterface;
	
	//
	// Protected methods
	//
	void						Init					(	HWND					a_hWnd,
															IGN_OBJIF_GfxContext*	a_pGfxContextInterface	);
	void						Finit					(	void											);
	IGNBOOL						ProceedCamera			(	void											);


public:

	//
	// Public interface
	//
	IGNBOOL						GetViewMatrix			(	IGN_STRUCT_4x4Matrix&	a_Matrix				);
	IGNBOOL						GetProjectionMatrix		(	IGN_STRUCT_4x4Matrix&	a_Matrix				);
	IGNBOOL						ProcessAgain			(	void											);
	IGNBOOL						Proceed					(	void											);
};




//------------- PREPROCESSOR END -------------------------------------------------------------------------------------

#endif // CAMERA_H_INCLUDED