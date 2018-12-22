
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************
//*
//*				Project:			IGNITION
//*
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************
//*
//*				Subproject:			Timer-Subsystem	(Ign_timer_sub)
//*				File:				ign_timer_sub_shared.h
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
//*				11/27/02| 0.0.1		## Initial release															(kp) ##
//*				11/28/02| 0.1.1		## Added the 'clock & timer interfaces'										(kp) ##
//*				12/10/02| 0.2.1		## Changed interface parameter types (-> references)						(kp) ##
//*
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************




//------------- PREPROCESSOR -----------------------------------------------------------------------------------------------------------------------------------------

#ifndef			IGN_TIMER_SUB_SHARED_H_INCLUDED
#define			IGN_TIMER_SUB_SHARED_H_INCLUDED

#if				_MSC_VER > 1000
#pragma once
#ifdef			IGN_SWITCH_SHOW_HEADER
#pragma message ("---> ign_timer_sub_shared.h\n")
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

struct			IGN_STRUCT_IFSubTimer;
struct			IGN_STRUCT_TmrVersion;
struct			IGN_STRUCT_TmrContextDesc;
struct			IGN_STRUCT_SysTimeAndDate;
struct			IGN_STRUCT_TimeAndDate;
struct			IGN_STRUCT_ClockAdjustment;
struct			IGN_OBJIF_TmrInstance;
struct			IGN_OBJIF_TmrContext;
struct			IGN_OBJIF_TmrClock;
struct			IGN_OBJIF_TmrTimer;




//------------- DEFINITIONS ------------------------------------------------------------------------------------------------------------------------------------------

enum			IGN_ENUM_TMRINSTANCETYPE
{
	IGN_ENUM_TMRINSTANCETYPE_NONE				= 0,												// Not any instance
	IGN_ENUM_TMRINSTANCETYPE_WINDOWSMULTIMEDIA	= 1	,												// Windows multimedia timer
	IGN_ENUM_TMRINSTANCETYPE_FORCEDWORD			= 0x7fffffff
};




typedef		IGN_OBJIF_TmrInstance*			IGN_LP_OBJIF_TmrInstance;
typedef		IGN_OBJIF_TmrContext*			IGN_LP_OBJIF_TmrContext;
typedef		IGN_OBJIF_TmrClock*				IGN_LP_OBJIF_TmrClock;
typedef		IGN_OBJIF_TmrTimer*				IGN_LP_OBJIF_TmrTimer;




//********************************************************************************************************************************************************************
//				Typedefs for all interface methods
//********************************************************************************************************************************************************************

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// DLL export methods
//
typedef			IGNRESULT	IGN_DLLCALL	IGN_METHOD_GetSubHookTimer							(			IGN_STRUCT_IFSubTimer**			a_ppInterface				);

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// DLL interface methods
//
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_TmrGetVersion							(			IGN_STRUCT_TmrVersion&			a_Version					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_TmrCreateInstance						(			IGN_ENUM_TMRINSTANCETYPE		a_InstanceType,
																										IGN_LP_OBJIF_TmrInstance&		a_pInstanceInterface		);

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Instance methods	
//
typedef			IGN_ENUM_TMRINSTANCETYPE
							IGN_OBJCALL	IGN_METHOD_TmrInstance_GetInstanceType				(			void														);														
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_TmrInstance_EnumContext					(			IGNU32							a_u32ContextNumber,
																										IGN_STRUCT_TmrContextDesc&		a_ContextDescription		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_TmrInstance_GetContextCount				(			IGNU32&							a_u32ContextCount			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_TmrInstance_CreateContext				(			IGNU32							a_u32ContextNumber,
																										IGN_LP_OBJIF_TmrContext&		a_pContextInterface			);	

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Context methods																				
//
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_TmrContext_CreateClock					(			IGN_LP_OBJIF_TmrClock&			a_pClockInterface			);																				
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_TmrContext_CreateTimer					(			IGN_LP_OBJIF_TmrTimer&			a_pTimerInterface			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_TmrContext_GetSystemTimeAndDate			(			IGN_STRUCT_SysTimeAndDate&		a_SystemTime				);

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Clock methods
//
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_TmrClock_Adjust							(	const	IGN_STRUCT_ClockAdjustment&		a_ClockADjustment			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_TmrClock_Set								(	const	IGN_STRUCT_TimeAndDate&			a_TimeAndDate				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_TmrClock_Pause							(			void														);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_TmrClock_Resume							(			void														);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_TmrClock_GetTimeAndDate					(			IGN_STRUCT_TimeAndDate&			a_TimeAndDate				);

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Timer methods
//
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_TmrTimer_Adjust							(			IGNFLOAT						a_fSecondFactor				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_TmrTimer_Reset							(			void														);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_TmrTimer_Set								(			IGNFLOAT						a_fSeconds					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_TmrTimer_Pause							(			void														);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_TmrTimer_Resume							(			void														);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_TmrTimer_GetLastTime						(			IGNFLOAT&						a_fSeconds					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_TmrTimer_GetTime							(			IGNFLOAT&						a_fSeconds					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_TmrTimer_GetTimeDifference				(			IGNFLOAT&						a_fSeconds					);




//------------- DECLARATIONS -----------------------------------------------------------------------------------------------------------------------------------------

struct	IGN_STRUCT_IFSubTimer
{
	IGN_METHOD_TmrGetVersion*							m_pGetVersion;								// Pointer to GetVersion(..) method
	IGN_METHOD_TmrCreateInstance*						m_pCreateInstance;							// Pointer to CreateInstance(..) method
};

struct	IGN_STRUCT_TmrVersion
{
	IGNCHAR												m_pcharIdentifier[256];						// Module identifier string
	IGNCHAR												m_pcharVendor[256];							// Vendor string
	IGNU32												m_u32MainVersionNumber;						// Main version number of module
	IGNU32												m_u32SubVersionNumber;						// Sub version number of module
	IGNCHAR												m_pcharReserved[256];						// Reserved (not in use)
};

struct	IGN_STRUCT_TmrContextDesc
{
	IGNU32												m_u32PartsPerSecond;						// Timer resolution
};

struct	IGN_STRUCT_SysTimeAndDate
{
	IGNU32												m_u32Year;									// Number describing the year
	IGNU32												m_u32Month;									// Number describing the month
	IGNU32												m_u32Day;									// Number describing the day
	IGNU32												m_u32DayOfWeek;								// Number describing the day of the week (0 -> Sunday)
	IGNU32												m_u32Hour;									// Number describing the hour
	IGNU32												m_u32Minute;								// Number describing the minute
	IGNU32												m_u32Second;								// Number describing the second
	IGNU32												m_u32Millisecond;							// Number describing the millisecond
};

struct	IGN_STRUCT_TimeAndDate
{
	IGNU32												m_u32Year;									// Number describing the year
	IGNU32												m_u32Day;									// Number describing the day
	IGNFLOAT											m_fSecond;									// Number describing the second
};

struct	IGN_STRUCT_ClockAdjustment
{
	IGNFLOAT											m_fSecondFactor;							// Scaling factor for virtual seconds
	IGNU32												m_u32NumberOfSecondsPerDay;					// Number of seconds per day (virtual)
	IGNU32												m_u32NumberOfDaysPerYear;					// Number of days per year (virtual)
};




//********************************************************************************************************************************************************************
//				Concrete object interfaces
//				See separate manual for detailed documentation
//********************************************************************************************************************************************************************
															
//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Instance interface - SINGLETON	
//
IGN_OBJ_INTERFACE_DECL_BaseObject( IGN_OBJIF_TmrInstance )
{
	//
	// Version 1.0.0
	//

	//
	// Retrieves a copy of the instance type
	//
	IGN_OBJ_METHOD( IGN_METHOD_TmrInstance_GetInstanceType,					GetInstanceType					);

	//
	// Retrieves a copy of the adapter and mode description of specified context
	//
	IGN_OBJ_METHOD(	IGN_METHOD_TmrInstance_EnumContext,						EnumContext						);

	//
	// Retrieves a copy of the count of possible contexts
	//
	IGN_OBJ_METHOD(	IGN_METHOD_TmrInstance_GetContextCount,					GetContextCount					);

	//
	// Creates a new IGN_OBJIF_TmrContext object
	//
	IGN_OBJ_METHOD(	IGN_METHOD_TmrInstance_CreateContext,					CreateContext					);
};											
																
//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Context interface - SINGLETON	
//
IGN_OBJ_INTERFACE_DECL_BaseObject( IGN_OBJIF_TmrContext )
{
	//
	// Version 1.0.0
	//

	//
	// Creates a new IGN_OBJIF_TmrClock object
	//
	IGN_OBJ_METHOD( IGN_METHOD_TmrContext_CreateClock,						CreateClock						);

	//
	// Creates a new IGN_OBJIF_TmrTimer object
	//
	IGN_OBJ_METHOD(	IGN_METHOD_TmrContext_CreateTimer,						CreateTimer						);

	//
	// Retrieves system time and date
	//
	IGN_OBJ_METHOD(	IGN_METHOD_TmrContext_GetSystemTimeAndDate,				GetSystemTimeAndDate			);
};															
																
//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Clock interface
//
IGN_OBJ_INTERFACE_DECL_BaseObject( IGN_OBJIF_TmrClock )
{
	//
	// Version 1.0.0
	//

	//
	// Adjusts the clock speed and meaning
	//
	IGN_OBJ_METHOD(	IGN_METHOD_TmrClock_Adjust,								Adjust							);

	//
	// Sets the clock to specified time and date
	//
	IGN_OBJ_METHOD(	IGN_METHOD_TmrClock_Set,								Set								);

	//
	// Pauses the clock processing
	//
	IGN_OBJ_METHOD(	IGN_METHOD_TmrClock_Pause,								Pause							);

	//
	// Resumes the clock processing
	//
	IGN_OBJ_METHOD(	IGN_METHOD_TmrClock_Resume,								Resume							);

	//
	// Retrieves the clock's time and date
	//
	IGN_OBJ_METHOD(	IGN_METHOD_TmrClock_GetTimeAndDate,						GetTimeAndDate					);
};															
																
//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Timer interface
//
IGN_OBJ_INTERFACE_DECL_BaseObject( IGN_OBJIF_TmrTimer )
{
	//
	// Version 1.0.0
	//

	//
	// Adjusts the timer speed
	//
	IGN_OBJ_METHOD(	IGN_METHOD_TmrTimer_Adjust,								Adjust							);

	//
	// Resets the timer to 0
	//
	IGN_OBJ_METHOD(	IGN_METHOD_TmrTimer_Reset,								Reset							);

	//
	// Sets the timer to specified value
	//
	IGN_OBJ_METHOD(	IGN_METHOD_TmrTimer_Set,								Set								);

	//
	// Pauses the timer processing
	//
	IGN_OBJ_METHOD(	IGN_METHOD_TmrTimer_Pause,								Pause							);

	//
	// Resumes the timer processing
	//
	IGN_OBJ_METHOD(	IGN_METHOD_TmrTimer_Resume,								Resume							);

	//
	// Retrieves the time stamp that belongs to last request (without reset)
	//
	IGN_OBJ_METHOD(	IGN_METHOD_TmrTimer_GetLastTime,						GetLastTime						);

	//
	// Retrieves the current time stamp (without reset)
	//
	IGN_OBJ_METHOD(	IGN_METHOD_TmrTimer_GetTime,							GetTime							);

	//
	// Retrieves the time difference to last request (without reset)
	//
	IGN_OBJ_METHOD(	IGN_METHOD_TmrTimer_GetTimeDifference,					GetTimeDifference				);
};




//------------- PREPROCESSOR END -------------------------------------------------------------------------------------------------------------------------------------

#endif // IGN_TIMER_SUB_SHARED_H_INCLUDED





