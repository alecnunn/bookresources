
//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Project:			IGNITION
//*
//********************************************************************************************************************
//********************************************************************************************************************
//*
//*				Subproject:			ShadowsDemo
//*				File:				Shared.h
//*				Description:		Shared data for demo applications
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

#ifndef			SHARED_H_INCLUDED
#define			SHARED_H_INCLUDED

#if				_MSC_VER > 1000
#pragma once
#ifdef			IGN_SWITCH_SHOW_HEADER
#pragma message ("---> Shared.h\n")
#endif
#endif 




//------------- DEPENDENCIES -----------------------------------------------------------------------------------------

#include "windows.h"
#include <math.h>

#include "../IGN_shared/Modules/Subsystems/ign_graphic_sub_shared.h"
#include "../IGN_shared/Modules/Subsystems/ign_input_sub_shared.h"
#include "../IGN_shared/Modules/Subsystems/ign_timer_sub_shared.h"
#include "../IGN_shared/Modules/Systems/ign_camera_sys_shared.h"




//------------- FORWARD DECLARATIONS ---------------------------------------------------------------------------------
//------------- DEFINITIONS ------------------------------------------------------------------------------------------

#define		constObjectCount	4
#define		constLightCount		2

enum	ENUM_TECHNIQUE
{
	ENUM_TECHNIQUE_REDUCEDCOMBINEDBUFFER	= 1,
	ENUM_TECHNIQUE_COMPLETECOMBINEDBUFFER	= 2,
	ENUM_TECHNIQUE_FORCEDWORD				= 0x7fffffff
};




//------------- DECLARATIONS -----------------------------------------------------------------------------------------

struct structVertexFormat
{
	IGNFLOAT	X, Y, Z;
	IGNFLOAT	NX, NY, NZ;
	IGNDWORD	COLOR;
	IGNFLOAT	TU, TV, TS;
};

struct structObjectSpere
{
	IGN_STRUCT_3DVector			m_ObjectStandardCenterVector;
	IGN_STRUCT_3DVector			m_ObjectCurrentCenterVector;
	IGNFLOAT					m_fObjectRadius;
	IGNFLOAT					m_fBiasZ;
};

struct structVisualObject
{
	IGN_OBJIF_GfxVisual*		m_pVisual;
	IGN_OBJIF_GfxVertexData*	m_pVertexData;
	IGN_OBJIF_GfxIndexData*		m_pIndexData;
	IGN_OBJIF_GfxShader*		m_pShader;
	IGN_OBJIF_GfxMaterial*		m_pMaterial;
	IGN_OBJIF_GfxTextureBlock*	m_pTextureBlock0;
	IGN_OBJIF_GfxTextureBlock*	m_pTextureBlock1;
	IGN_OBJIF_GfxTextureBlock*	m_pTextureBlock2;
	IGN_OBJIF_GfxTextureBlock*	m_pTextureBlock3;
	structObjectSpere			m_ObjectSphere;
	IGNU32						m_u32IndexCount;
	IGNU32*						m_pIndexBufferForSilhouetteDetection;
};




//------------- PREPROCESSOR END -------------------------------------------------------------------------------------

#endif // SHARED_H_INCLUDED