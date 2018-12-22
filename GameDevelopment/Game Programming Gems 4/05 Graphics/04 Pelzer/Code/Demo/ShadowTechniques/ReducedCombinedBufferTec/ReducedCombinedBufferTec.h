
//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Project:			IGNITION
//*
//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Subproject:			ShadowsDemo
//*				File:				ReducedCombinedBufferTec.h
//*				Description:		Shadowing based on the 'reduced combined buffer' technique
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

#ifndef			REDUCEDCOMBINEDBUFFERTEC_H_INCLUDED
#define			REDUCEDCOMBINEDBUFFERTEC_H_INCLUDED

#if				_MSC_VER > 1000
#pragma once
#ifdef			IGN_SWITCH_SHOW_HEADER
#pragma message ("---> ReducedCombinedBufferTec.h\n")
#endif
#endif 




//------------- DEPENDENCIES -----------------------------------------------------------------------------------------

#include "../ShadowTecBase.h"




//------------- FORWARD DECLARATIONS ---------------------------------------------------------------------------------
//------------- DEFINITIONS ------------------------------------------------------------------------------------------

#define		constZeroAlphaShiftVector	0x77080808
#define		constImageWidth				512
#define		constImageHeight			512




//------------- DECLARATIONS -----------------------------------------------------------------------------------------

struct structIDShader
{
	IGN_OBJIF_GfxShader*		m_pShader;
	IGN_OBJIF_GfxMaterial*		m_pMaterial;
	IGN_OBJIF_GfxTextureBlock*	m_pTextureBlock0;
	IGN_OBJIF_GfxTextureBlock*	m_pTextureBlock1;
};

struct structRedCombBufTecShader
{
	IGN_OBJIF_GfxShader*		m_pShader;
	IGN_OBJIF_GfxMaterial*		m_pMaterial;
	IGN_OBJIF_GfxTextureBlock*	m_pTextureBlock0;
	IGN_OBJIF_GfxTextureBlock*	m_pTextureBlock1;
	IGN_OBJIF_GfxTextureBlock*	m_pTextureBlock2;
	IGN_OBJIF_GfxTextureBlock*	m_pTextureBlock3;
	IGN_OBJIF_GfxImageData*		m_pIDImage;
	IGNHANDLE					m_hDepthBuffer;
};

class REDUCEDCOMBINEDBUFFERTEC : public SHADOWTECBASE
{
public:

	//
	// Constructors/Destructors
	//
	REDUCEDCOMBINEDBUFFERTEC	(	IGN_OBJIF_GfxContext*	a_pContextInterface,
									IGN_OBJIF_GfxMixer*		a_pMixer );
	~REDUCEDCOMBINEDBUFFERTEC	();

protected:

	//
	// Internal objects
	//
	IGN_OBJIF_GfxCamera*		m_pLightCamera;
	structIDShader				m_IDShader;
	structRedCombBufTecShader	m_RedCombBufTecShader;

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

#endif // REDUCEDCOMBINEDBUFFERTEC_H_INCLUDED