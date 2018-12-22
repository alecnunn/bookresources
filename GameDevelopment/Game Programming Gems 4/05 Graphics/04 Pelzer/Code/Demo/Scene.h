
//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Project:			IGNITION
//*
//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Subproject:			SimpleShaderDemo
//*				File:				Scene.h
//*				Description:		Scene for shader testing applications
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

#ifndef			SCENE_H_INCLUDED
#define			SCENE_H_INCLUDED

#if				_MSC_VER > 1000
#pragma once
#ifdef			IGN_SWITCH_SHOW_HEADER
#pragma message ("---> Scene.h\n")
#endif
#endif 




//------------- DEPENDENCIES -----------------------------------------------------------------------------------------

#include "ShadowTechniques/ReducedCombinedBufferTec/ReducedCombinedBufferTec.h"
#include "ShadowTechniques/CompleteCombinedBufferTec/CompleteCombinedBufferTec.h"
#include "ShadowTechniques/ShadowTecBase.h"
#include "Shared.h"
#include "Camera.h"




//------------- FORWARD DECLARATIONS ---------------------------------------------------------------------------------
//------------- DEFINITIONS ------------------------------------------------------------------------------------------




//------------- DECLARATIONS -----------------------------------------------------------------------------------------

class SCENE
{
public:

	//
	// Constructors/Destructors
	//
	SCENE		(	IGN_ENUM_GFXINSTANCETYPE a_InstanceType,
					IGN_OBJIF_GfxContext*	a_pGfxContextInterface,
					IGN_OBJIF_GfxMixer*		a_pMixer );
	~SCENE		();

protected:
	
	//
	// Handles of used dynamic link libraries
	//
	HMODULE						m_hTmrLibrary;

	//
	// Objects in IGN_graphic_sub.dll
	//
	IGN_OBJIF_GfxLight*			m_pAmbientLight;
	IGN_OBJIF_GfxLight*			m_pLight[constLightCount];

	//
	// Objects in IGN_timer_sub.dll
	//
	IGN_OBJIF_TmrInstance*		m_pTmrInstanceInterface;
	IGN_OBJIF_TmrContext*		m_pTmrContextInterface;
	IGN_OBJIF_TmrTimer*			m_pTimerInterface;

	//
	// Internal objects
	//
	IGN_OBJIF_GfxMixer*			m_pMixer;
	structVisualObject			m_aryVisualObject[constObjectCount];
	IGN_STRUCT_StencilData		m_StencilData;
	IGNU32						m_u32VisualAniStep;
	IGNU32						m_u32LightAniStep;
	IGNBOOL						m_bEnabledAdditionalLights;
	IGNBOOL						m_bEnabledShadows;
	IGNBOOL						m_bEnabledSecondLight;
	IGNBOOL						m_bEnabledColoredLight;
	ENUM_TECHNIQUE				m_ShadowTechnique;
	SHADOWTECBASE*				m_pReducedCombinedBufferTec;
	SHADOWTECBASE*				m_pCompleteCombinedBufferTec;
	
	//
	// Protected methods
	//
	void						Init								(	IGN_ENUM_GFXINSTANCETYPE a_InstanceType,
																		IGN_OBJIF_GfxContext*	a_pGfxContextInterface	);
	void						Finit								(	void											);
	void						ActivateStencilTest					(	void											);
	void						DeactivateStencilTest				(	void											);
	void						ProceedVisualsWithAmbientLight		(	void											);
	void						ProceedVisualsWithAdditionalLights	(	CAMERA*					a_pCamera				);
	void						AnimateVisuals						(	void											);
	void						AnimateLights						(	void											);
	IGNBOOL						ProceedScene						(	CAMERA*					a_pCamera				);


public:

	//
	// Public interface
	//
	IGNBOOL						Proceed								(	CAMERA*					a_pCamera,
																		ENUM_TECHNIQUE			a_ShadowTechnique,
																		IGNU32					a_u32LightCount,
																		IGNBOOL					a_bColoredLight,
																		IGNBOOL					a_bEnabledShadows		);
};




//------------- PREPROCESSOR END -------------------------------------------------------------------------------------

#endif // SCENE_H_INCLUDED