
//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Project:			IGNITION
//*
//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Subproject:			ShadowsDemo
//*				File:				DemoFrame.h
//*				Description:		Entry point for shadows testing applications
//*
//********************************************************************************************************************
//*
//*				Author:				Kurt Pelzer
//*									Copyright (c) 2002 Kurt Pelzer
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

#ifndef			DEMOFRAME_H_INCLUDED
#define			DEMOFRAME_H_INCLUDED

#if				_MSC_VER > 1000
#pragma once
#ifdef			IGN_SWITCH_SHOW_HEADER
#pragma message ("---> DemoFrame.h\n")
#endif
#endif 




//------------- DEPENDENCIES -----------------------------------------------------------------------------------------

#include "windows.h"
#include <math.h>

#include "../WinMain/Frame.h"
#include "Shared.h"
#include "Camera.h"
#include "Scene.h"




//------------- FORWARD DECLARATIONS ---------------------------------------------------------------------------------
//------------- DEFINITIONS ------------------------------------------------------------------------------------------




//------------- DECLARATIONS -----------------------------------------------------------------------------------------

class DEMOFRAME : public FRAME
{
public:

	//
	// Constructors/Destructors
	//
	DEMOFRAME	(	HWND		a_hWnd,
					IGNU32		a_u32APIType );
	~DEMOFRAME	();

protected:
	
	//
	// Handles of used dynamic link libraries
	//
	HMODULE						m_hGfxLibrary;
	HMODULE						m_hInpLibrary;
	HMODULE						m_hTmrLibrary;

	//
	// Objects in IGN_graphic_sub.dll
	//
	IGN_OBJIF_GfxInstance*		m_pGfxInstanceInterface;
	IGN_OBJIF_GfxContext*		m_pGfxContextInterface;
	IGN_OBJIF_GfxMixer*			m_pMixer;
	IGN_OBJIF_GfxAssistant*		m_pAssistant;
	IGN_OBJIF_GfxGauge*			m_pGauge;

	//
	// Objects in IGN_input_sub.dll
	//
	IGN_OBJIF_InpInstance*		m_pInpInstanceInterface;
	IGN_OBJIF_InpContext*		m_pInpContextInterface;
	IGN_OBJIF_InpKeyboard*		m_pKeyboard;

	//
	// Objects in IGN_timer_sub.dll
	//
	IGN_OBJIF_TmrInstance*		m_pTmrInstanceInterface;
	IGN_OBJIF_TmrContext*		m_pTmrContextInterface;
	IGN_OBJIF_TmrTimer*			m_pTimerInterface;

	//
	// Internal objects
	//
	CAMERA*						m_pCamera;
	SCENE*						m_pScene;
	IGNU32						m_u32APIType;
	IGNFLOAT					m_fCurrentTimeStamp;
	ENUM_TECHNIQUE				m_ShadowTechnique;
	IGNU32						m_u32LightCount;
	IGNBOOL						m_bColoredLight;
	IGNBOOL						m_bDisplayInfo;
	
	//
	// Protected methods
	//
	void						Init					(	HWND					a_hWnd,
															IGNU32					a_u32APIType	);
	void						Finit					(	void									);
	IGN_OBJIF_GfxContext*		StartCompatibleContext	(	HWND					a_hWnd			);
	void						ShowInfo				(	void									);
	IGNBOOL						ProceedNextFrame		(	HWND					a_hWnd			);


public:

	//
	// Public interface
	//
	IGNBOOL						Proceed					(	HWND					a_hWnd,
															MSG*					a_pMsg );
};




//------------- PREPROCESSOR END -------------------------------------------------------------------------------------

#endif // DEMOFRAME_H_INCLUDED