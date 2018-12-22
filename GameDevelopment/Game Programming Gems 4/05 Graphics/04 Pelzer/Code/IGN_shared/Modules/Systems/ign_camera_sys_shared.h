
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************
//*
//*				Project:			IGNITION
//*
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************
//*
//*				Subproject:			Camera-System	(Ign_camera_sys)
//*				File:				ign_camera_sys_shared.h
//*				Description:		Shared file with DLL and OBJ interfaces
//*
//********************************************************************************************************************************************************************
//*
//*				Author:				Kurt Pelzer
//*									Copyright (c) 2002 Kurt Pelzer
//*									All rights reserved.
//*
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************
//*
//*				Status:				In progress
//*
//********************************************************************************************************************************************************************
//*
//*				History:
//*
//*				01/07/03| 0.0.1		## Initial release															(kp) ##
//*
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************




//------------- PREPROCESSOR -----------------------------------------------------------------------------------------------------------------------------------------

#ifndef			IGN_CAMERA_SYS_SHARED_H_INCLUDED
#define			IGN_CAMERA_SYS_SHARED_H_INCLUDED

#if				_MSC_VER > 1000
#pragma once
#ifdef			IGN_SWITCH_SHOW_HEADER
#pragma message ("---> ign_camera_sys_shared.h\n")
#endif
#endif 




//------------- DEPENDENCIES -----------------------------------------------------------------------------------------------------------------------------------------

#include		<windows.h>

#ifndef			IGN_TYPES_SHARED_H_INCLUDED
  #include		"../../Types/ign_types_shared.h"
#endif

#ifndef			IGN_POINTERLIST_SHARED_H_INCLUDED
  #include		"../../Types/Lists/ign_pointerlist_shared.h"
#endif

#ifndef			IGN_VALUELIST_SHARED_H_INCLUDED
  #include		"../../Types/Lists/ign_valuelist_shared.h"
#endif

#ifndef			IGN_GRAPHIC_SUB_SHARED_H_INCLUDED
  #include		"../Subsystems/ign_graphic_sub_shared.h"
#endif

#ifndef			IGN_VECTOR_SHARED_H_INCLUDED
  #include		"../../Types/Math/ign_vector_shared.h"
#endif

#ifdef			IGN_EXCEPTIONHANDLING
	#ifndef			IGN_EXCEPTIONS_H_INCLUDED
		#include		"../../Exceptions/ign_exceptions.h"
	#endif
#else
	#ifndef			IGN_EXCEPTIONSDUMMY_H_INCLUDED
		#include		"../../Exceptions/ign_exceptionsdummy.h"
	#endif
#endif




//------------- FORWARD DECLARATIONS ---------------------------------------------------------------------------------------------------------------------------------

struct			IGN_STRUCT_IFSysCamera;
struct			IGN_STRUCT_CamVersion;
struct			IGN_STRUCT_CamContextDesc;
struct			IGN_OBJIF_CamInstance;
struct			IGN_OBJIF_CamContext;
struct			IGN_OBJIF_CamFreeCamera;




//------------- DEFINITIONS ------------------------------------------------------------------------------------------------------------------------------------------

enum			IGN_ENUM_CAMINSTANCETYPE
{
	IGN_ENUM_CAMINSTANCETYPE_NONE				= 0,												// Not any instance
	IGN_ENUM_CAMINSTANCETYPE_STANDARD			= 1	,												// Standard instance type
	IGN_ENUM_CAMINSTANCETYPE_FORCEDWORD			= 0x7fffffff
};




typedef		IGN_OBJIF_CamInstance*			IGN_LP_OBJIF_CamInstance;
typedef		IGN_OBJIF_CamContext*			IGN_LP_OBJIF_CamContext;
typedef		IGN_OBJIF_CamFreeCamera*		IGN_LP_OBJIF_CamFreeCamera;




//********************************************************************************************************************************************************************
//				Typedefs for all interface methods
//********************************************************************************************************************************************************************

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// DLL export methods
//
typedef			IGNRESULT	IGN_DLLCALL	IGN_METHOD_GetSysHookCamera							(			IGN_STRUCT_IFSysCamera**		a_ppInterface				);

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// DLL interface methods
//
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamGetVersion							(			IGN_STRUCT_CamVersion&			a_Version					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamCreateInstance						(			IGN_ENUM_CAMINSTANCETYPE		a_InstanceType,
																										IGN_LP_OBJIF_CamInstance&		a_pInstanceInterface		);

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Instance methods	
//
typedef			IGN_ENUM_CAMINSTANCETYPE
							IGN_OBJCALL	IGN_METHOD_CamInstance_GetInstanceType				(			void														);														
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamInstance_EnumContext					(			IGNU32							a_u32ContextNumber,
																										IGN_STRUCT_CamContextDesc&		a_ContextDescription		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamInstance_GetContextCount				(			IGNU32&							a_u32ContextCount			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamInstance_CreateContext				(			IGNU32							a_u32ContextNumber,
																										IGN_LP_OBJIF_CamContext&		a_pContextInterface			);	

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Context methods																				
//
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamContext_CreateFreeCamera				(			IGN_LP_OBJIF_CamFreeCamera&		a_pFreeCameraInterface		);

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Free-Camera methods
//
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_AttachGfxContext			(			IGN_LP_OBJIF_GfxContext			a_pContextInterface			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_GetAttachedGfxContext		(			IGN_LP_OBJIF_GfxContext&		a_pContextInterface			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_GetGfxCamera				(			IGN_LP_OBJIF_GfxCamera&			a_pCameraInterface			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_SetFOV						(			IGNFLOAT						a_fYRadiant,
																										IGNFLOAT						a_fAspect					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_GetFOV						(			IGNFLOAT&						a_fYRadiant,
																										IGNFLOAT&						a_fAspect					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_SetNearAndFarClipping		(			IGNFLOAT						a_fNearClippingDistance,
																										IGNFLOAT						a_fFarClippingDistance		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_GetNearAndFarClipping		(			IGNFLOAT&						a_fNearClippingDistance,
																										IGNFLOAT&						a_fFarClippingDistance		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_SetPosition				(	const	IGN_STRUCT_3DVector&			a_Position					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_GetPosition				(			IGN_STRUCT_3DVector&			a_Position					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_AddTranslation				(	const	IGN_STRUCT_3DVector&			a_Translation				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_MoveForward				(			IGNFLOAT						a_fRange					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_MoveSideways				(			IGNFLOAT						a_fRange					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_MoveUpwards				(			IGNFLOAT						a_fRange					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_SetViewDirection			(	const	IGN_STRUCT_3DVector&			a_Direction					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_GetViewDirection			(			IGN_STRUCT_3DVector&			a_Direction					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_SetViewUp					(	const	IGN_STRUCT_3DVector&			a_Up						);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_GetViewUp					(			IGN_STRUCT_3DVector&			a_Up						);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_FixViewUp					(			IGNBOOL							a_bFixed					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_SetPitch					(			IGNFLOAT						a_fRadiant					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_GetPitch					(			IGNFLOAT&						a_fRadiant					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_AddPitch					(			IGNFLOAT						a_fRadiant					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_SetYaw						(			IGNFLOAT						a_fRadiant					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_GetYaw						(			IGNFLOAT&						a_fRadiant					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_AddYaw						(			IGNFLOAT						a_fRadiant					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_SetRoll					(			IGNFLOAT						a_fRadiant					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_GetRoll					(			IGNFLOAT&						a_fRadiant					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_AddRoll					(			IGNFLOAT						a_fRadiant					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_SetAdditionalPitch			(			IGNFLOAT						a_fRadiant					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_GetAdditionalPitch			(			IGNFLOAT&						a_fRadiant					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_AddAdditionalPitch			(			IGNFLOAT						a_fRadiant					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_SetAdditionalYaw			(			IGNFLOAT						a_fRadiant					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_GetAdditionalYaw			(			IGNFLOAT&						a_fRadiant					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_AddAdditionalYaw			(			IGNFLOAT						a_fRadiant					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_SetAdditionalRoll			(			IGNFLOAT						a_fRadiant					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_GetAdditionalRoll			(			IGNFLOAT&						a_fRadiant					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_AddAdditionalRoll			(			IGNFLOAT						a_fRadiant					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_GetViewMatrix				(			IGN_STRUCT_4x4Matrix&			a_Matrix					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_GetProjectionMatrix		(			IGN_STRUCT_4x4Matrix&			a_Matrix					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_CamFreeCamera_ExecuteWithoutUpdate		(			void														);




//------------- DECLARATIONS -----------------------------------------------------------------------------------------------------------------------------------------

struct	IGN_STRUCT_IFSysCamera
{
	IGN_METHOD_CamGetVersion*							m_pGetVersion;								// Pointer to GetVersion(..) method
	IGN_METHOD_CamCreateInstance*						m_pCreateInstance;							// Pointer to CreateInstance(..) method
};

struct	IGN_STRUCT_CamVersion
{
	IGNCHAR												m_pcharIdentifier[256];						// Module identifier string
	IGNCHAR												m_pcharVendor[256];							// Vendor string
	IGNU32												m_u32MainVersionNumber;						// Main version number of module
	IGNU32												m_u32SubVersionNumber;						// Sub version number of module
	IGNCHAR												m_pcharReserved[256];						// Reserved (not in use)
};

struct	IGN_STRUCT_CamContextDesc
{
	IGNU32												m_u32Reserved;								// Reserved (not in use)
};




//********************************************************************************************************************************************************************
//				Concrete object interfaces
//				See separate manual for detailed documentation
//********************************************************************************************************************************************************************
															
//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Instance interface - SINGLETON	
//
IGN_OBJ_INTERFACE_DECL_BaseObject( IGN_OBJIF_CamInstance )
{
	//
	// Version 1.0.0
	//

	//
	// Retrieves a copy of the instance type
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamInstance_GetInstanceType,					GetInstanceType					);

	//
	// Retrieves a copy of the adapter and mode description of specified context
	//
	IGN_OBJ_METHOD(	IGN_METHOD_CamInstance_EnumContext,						EnumContext						);

	//
	// Retrieves a copy of the count of possible contexts
	//
	IGN_OBJ_METHOD(	IGN_METHOD_CamInstance_GetContextCount,					GetContextCount					);

	//
	// Creates a new IGN_OBJIF_CamContext object
	//
	IGN_OBJ_METHOD(	IGN_METHOD_CamInstance_CreateContext,					CreateContext					);
};											
																
//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Context interface - SINGLETON	
//
IGN_OBJ_INTERFACE_DECL_BaseObject( IGN_OBJIF_CamContext )
{
	//
	// Version 1.0.0
	//

	//
	// Creates a new IGN_OBJIF_CamFreeCamera object
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamContext_CreateFreeCamera,					CreateFreeCamera				);
};															
																
//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Free-Camera interface
//
IGN_OBJ_INTERFACE_DECL_ExecutableObject( IGN_OBJIF_CamFreeCamera )
{
	//
	// Version 1.0.0
	//

	//
	// Attachs a graphic subsystem context
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_AttachGfxContext,				AttachGfxContext				);

	//
	// Retrieves the attached a graphic subsystem context
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_GetAttachedGfxContext,			GetAttachedGfxContext			);

	//
	// Retrieves the internal graphic subsystem camera
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_GetGfxCamera,					GetGfxCamera					);

	//
	// Sets the field of view
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_SetFOV,						SetFOV							);

	//
	// Retrieves the field of view
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_GetFOV,						GetFOV							);

	//
	// Sets the the near and far clipping distances
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_SetNearAndFarClipping,			SetNearAndFarClipping			);

	//
	// Retrieves the near and far clipping distances
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_GetNearAndFarClipping,			GetNearAndFarClipping			);

	//
	// Sets the camera position
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_SetPosition,					SetPosition						);

	//
	// Retrieves the camera position
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_GetPosition,					GetPosition						);

	//
	// Moves the camera forward (or backward)
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_MoveForward,					MoveForward						);

	//
	// Moves the camera sideways
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_MoveSideways,					MoveSideways					);

	//
	// Moves the camera upwards
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_MoveUpwards,					MoveUpwards						);

	//
	// Translates the camera position
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_AddTranslation,				AddTranslation					);

	//
	// Sets the viewing direction
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_SetViewDirection,				SetViewDirection				);

	//
	// Retrieves the viewing direction
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_GetViewDirection,				GetViewDirection				);

	//
	//  Sets the viewing up vector
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_SetViewUp,						SetViewUp						);

	//
	// Retrieves the viewing up vector
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_GetViewUp,						GetViewUp						);

	//
	// Fixes the viewing up vector
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_FixViewUp,						FixViewUp						);

	//
	// Sets the pitch around the x-axis
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_SetPitch,						SetPitch						);

	//
	// Gets the pitch around the x-axis
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_GetPitch,						GetPitch						);

	//
	// Adds value to the pitch around the x-axis
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_AddPitch,						AddPitch						);

	//
	// Sets the yaw around the y-axis
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_SetYaw,						SetYaw							);

	//
	// Gets the yaw around the y-axis
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_GetYaw,						GetYaw							);

	//
	// Adds value to the yaw around the y-axis
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_AddYaw,						AddYaw							);

	//
	// Sets the  roll around the z-axis
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_SetRoll,						SetRoll							);

	//
	// Gets the roll around the z-axis
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_GetRoll,						GetRoll							);

	//
	// Adds value to the roll around the z-axis
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_AddRoll,						AddRoll							);

	//
	// Sets the additional pitch around the x-axis
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_SetAdditionalPitch,			SetAdditionalPitch				);

	//
	// Gets the additional pitch around the x-axis
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_GetAdditionalPitch,			GetAdditionalPitch				);

	//
	// Adds value to the additional pitch around the x-axis
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_AddAdditionalPitch,			AddAdditionalPitch				);

	//
	// Sets the additional yaw around the y-axis
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_SetAdditionalYaw,				SetAdditionalYaw				);

	//
	// Gets the additional yaw around the y-axis
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_GetAdditionalYaw,				GetAdditionalYaw				);

	//
	// Adds value to the additional yaw around the y-axis
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_AddAdditionalYaw,				AddAdditionalYaw				);

	//
	// Sets the additional roll around the z-axis
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_SetAdditionalRoll,				SetAdditionalRoll				);

	//
	// Gets the additional roll around the z-axis
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_GetAdditionalRoll,				GetAdditionalRoll				);

	//
	// Adds value to the additional roll around the z-axis
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_AddAdditionalRoll,				AddAdditionalRoll				);

	//
	// Retrieves the current view matrix
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_GetViewMatrix,					GetViewMatrix					);

	//
	// Retrieves the current projection matrix
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_GetProjectionMatrix,			GetProjectionMatrix				);

	//
	// Executes the camera without updating the view and projection matrix
	//
	IGN_OBJ_METHOD( IGN_METHOD_CamFreeCamera_ExecuteWithoutUpdate,			ExecuteWithoutUpdate			);
};




//------------- PREPROCESSOR END -------------------------------------------------------------------------------------------------------------------------------------

#endif // IGN_CAMERA_SYS_SHARED_H_INCLUDED





