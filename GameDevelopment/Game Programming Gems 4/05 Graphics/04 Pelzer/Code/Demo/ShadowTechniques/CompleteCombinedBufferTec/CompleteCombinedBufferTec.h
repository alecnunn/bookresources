
//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Project:			IGNITION
//*
//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Subproject:			ShadowsDemo
//*				File:				CompleteCombinedBufferTec.h
//*				Description:		Shadowing based on the 'complete combined buffer' technique
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

#ifndef			COMPLETECOMBINEDBUFFERTEC_H_INCLUDED
#define			COMPLETECOMBINEDBUFFERTEC_H_INCLUDED

#if				_MSC_VER > 1000
#pragma once
#ifdef			IGN_SWITCH_SHOW_HEADER
#pragma message ("---> CompleteCombinedBufferTec.h\n")
#endif
#endif 




//------------- DEPENDENCIES -----------------------------------------------------------------------------------------

#include "../ShadowTecBase.h"




//------------- FORWARD DECLARATIONS ---------------------------------------------------------------------------------
//------------- DEFINITIONS ------------------------------------------------------------------------------------------

#define		constIDColorShiftVector		0x00080808
#define		constImageWidth				512
#define		constImageHeight			512




//------------- DECLARATIONS -----------------------------------------------------------------------------------------

struct structIDAndDepthShader
{
	IGN_OBJIF_GfxShader*			m_pShader;
	IGN_OBJIF_GfxMaterial*			m_pMaterial;
	IGN_OBJIF_GfxTextureBlock*		m_pTextureBlock0;
	IGN_OBJIF_GfxTextureBlock*		m_pTextureBlock1;
	IGN_OBJIF_GfxImageData*		m_pDepthAlphaImage;
};

struct structCptComBufTecShader
{
	IGN_OBJIF_GfxShader*		m_pShader;
	IGN_OBJIF_GfxMaterial*		m_pMaterial;
	IGN_OBJIF_GfxTextureBlock*	m_pTextureBlock0;
	IGN_OBJIF_GfxTextureBlock*	m_pTextureBlock1;
	IGN_OBJIF_GfxTextureBlock*	m_pTextureBlock2;
	IGN_OBJIF_GfxTextureBlock*	m_pTextureBlock3;
	IGN_OBJIF_GfxImageData*		m_pIDAndObjectDepthImage;
	IGN_OBJIF_GfxImageData*		m_pDepthAlphaImage;
	IGNHANDLE					m_hDepthBuffer;
};

class COMPLETECOMBINEDBUFFERTEC : public SHADOWTECBASE
{
public:

	//
	// Constructors/Destructors
	//
	COMPLETECOMBINEDBUFFERTEC	(	IGN_OBJIF_GfxContext*	a_pContextInterface,
									IGN_OBJIF_GfxMixer*		a_pMixer );
	~COMPLETECOMBINEDBUFFERTEC	();

protected:

	//
	// Internal objects
	//
	IGN_OBJIF_GfxCamera*		m_pLightCamera;
	structIDAndDepthShader		m_IDAndDepthShader;
	structCptComBufTecShader	m_CptComBufTecShader;

public:

	//
	// Public interface
	//
	IGNBOOL						PreProcess							(	CAMERA*					a_pCamera,
																		IGN_OBJIF_GfxLight*		a_pLight, 
																		structVisualObject*		a_pVisualObject,
																		IGNU32					a_u32ObjectCount );
	IGNBOOL						Proceed								(	IGN_OBJIF_GfxLight*		a_pLight, 
																		structVisualObject*		a_pVisualObject,
																		IGNU32					a_u32ObjectCount );
};




//------------- PREPROCESSOR END -------------------------------------------------------------------------------------

#endif // COMPLETECOMBINEDBUFFERTEC_H_INCLUDED