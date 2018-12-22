
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************
//*
//*				Project:			IGNITION
//*
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************
//*
//*				Subproject:			Input-Subsystem	(Ign_input_sub)
//*				File:				ign_input_sub_shared.h
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
//*				08/28/03| 0.0.2		## Added buffered data request (keyboard and mouse device)					(kp) ##
//*
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************




//------------- PREPROCESSOR -----------------------------------------------------------------------------------------------------------------------------------------

#ifndef			IGN_INPUT_SUB_SHARED_H_INCLUDED
#define			IGN_INPUT_SUB_SHARED_H_INCLUDED

#if				_MSC_VER > 1000
#pragma once
#ifdef			IGN_SWITCH_SHOW_HEADER
#pragma message ("---> ign_input_sub_shared.h\n")
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

struct			IGN_STRUCT_IFSubInput;
struct			IGN_STRUCT_InpVersion;
struct			IGN_STRUCT_InpContextDesc;
struct			IGN_STRUCT_KeyboardState;
struct			IGN_STRUCT_MouseState;
struct			IGN_STRUCT_BufferedInpData;
struct			IGN_OBJIF_InpInstance;
struct			IGN_OBJIF_InpContext;
struct			IGN_OBJIF_InpKeyboard;
struct			IGN_OBJIF_InpMouse;




//------------- DEFINITIONS ------------------------------------------------------------------------------------------------------------------------------------------

enum			IGN_ENUM_INPINSTANCETYPE
{
	IGN_ENUM_INPINSTANCETYPE_NONE				= 0,												// Not any instance
	IGN_ENUM_INPINSTANCETYPE_STANDARD			= 1,												// Standard instance type
	IGN_ENUM_INPINSTANCETYPE_FORCEDWORD			= 0x7fffffff
};

enum			IGN_ENUM_INPKEY
{
	IGN_ENUM_INPKEY_ESCAPE						= 0x01,												// DIK_ESCAPE
	IGN_ENUM_INPKEY_1							,													// '1' on main keyboard
	IGN_ENUM_INPKEY_2							,													// '2' on main keyboard
	IGN_ENUM_INPKEY_3							,													// '3' on main keyboard
	IGN_ENUM_INPKEY_4							,													// '4' on main keyboard
	IGN_ENUM_INPKEY_5							,													// '5' on main keyboard
	IGN_ENUM_INPKEY_6							,													// '6' on main keyboard
	IGN_ENUM_INPKEY_7							,													// '7' on main keyboard
	IGN_ENUM_INPKEY_8							,													// '8' on main keyboard
	IGN_ENUM_INPKEY_9							,													// '9' on main keyboard 
	IGN_ENUM_INPKEY_0							,													// '0' on main keyboard
	IGN_ENUM_INPKEY_MINUS						,													// On main keyboard 
	IGN_ENUM_INPKEY_EQUALS						,													// On main keyboard 
	IGN_ENUM_INPKEY_BACK						,													// 
	IGN_ENUM_INPKEY_TAB							,													// 
	IGN_ENUM_INPKEY_Q							,													// 
	IGN_ENUM_INPKEY_W							,													// 
	IGN_ENUM_INPKEY_E							,													// 
	IGN_ENUM_INPKEY_R							,													// 
	IGN_ENUM_INPKEY_T							,													// 
	IGN_ENUM_INPKEY_Y							,													// 
	IGN_ENUM_INPKEY_U							,													// 
	IGN_ENUM_INPKEY_I							,													// 
	IGN_ENUM_INPKEY_O							,													// 
	IGN_ENUM_INPKEY_P							,													// 
	IGN_ENUM_INPKEY_LBRACKET					,													// Left square bracket [ 
	IGN_ENUM_INPKEY_RBRACKET					,													// Right square bracket ] 
	IGN_ENUM_INPKEY_RETURN						,													// ENTER on main keyboard 
	IGN_ENUM_INPKEY_LCONTROL					,													// Left CTRL 
	IGN_ENUM_INPKEY_A							,													// 
	IGN_ENUM_INPKEY_S							,													// 
	IGN_ENUM_INPKEY_D							,													// 
	IGN_ENUM_INPKEY_F							,													// 
	IGN_ENUM_INPKEY_G							,													// 
	IGN_ENUM_INPKEY_H							,													// 
	IGN_ENUM_INPKEY_J							,													// 
	IGN_ENUM_INPKEY_K							,													// 
	IGN_ENUM_INPKEY_L							,													// 
	IGN_ENUM_INPKEY_SEMICOLON					,													// 
	IGN_ENUM_INPKEY_APOSTROPHE					,													// 
	IGN_ENUM_INPKEY_GRAVE						,													// Grave accent (`) 
	IGN_ENUM_INPKEY_LSHIFT						,													// Left SHIFT 
	IGN_ENUM_INPKEY_BACKSLASH					,													// 
    IGN_ENUM_INPKEY_Z							,													// 
    IGN_ENUM_INPKEY_X							,													// 
	IGN_ENUM_INPKEY_C							,													// 
	IGN_ENUM_INPKEY_V							,													// 
	IGN_ENUM_INPKEY_B							,													// 
	IGN_ENUM_INPKEY_N							,													// 
	IGN_ENUM_INPKEY_M							,													// 
	IGN_ENUM_INPKEY_COMMA						,													// 
	IGN_ENUM_INPKEY_PERIOD						,													// On main keyboard 
	IGN_ENUM_INPKEY_SLASH						,													// Forward slash (/) on main keyboard 
	IGN_ENUM_INPKEY_RSHIFT						,													// Right SHIFT 
	IGN_ENUM_INPKEY_MULTIPLY					,													// Asterisk (*) on numeric keypad 
	IGN_ENUM_INPKEY_LMENU						,													// Left Windows logo key 
	IGN_ENUM_INPKEY_SPACE						,													// SPACEBAR 
	IGN_ENUM_INPKEY_CAPITAL						,													// CAPS LOCK 
	IGN_ENUM_INPKEY_F1							,													// 
	IGN_ENUM_INPKEY_F2							,													// 
	IGN_ENUM_INPKEY_F3							,													// 
	IGN_ENUM_INPKEY_F4							,													// 
	IGN_ENUM_INPKEY_F5							,													// 
	IGN_ENUM_INPKEY_F6							,													// 
	IGN_ENUM_INPKEY_F7							,													// 
	IGN_ENUM_INPKEY_F8							,													// 
	IGN_ENUM_INPKEY_F9							,													// 
	IGN_ENUM_INPKEY_F10							,													// 
	IGN_ENUM_INPKEY_NUMLOCK						,													// 
	IGN_ENUM_INPKEY_SCROLL						,													// SCROLL LOCK 
	IGN_ENUM_INPKEY_NUMPAD7						,													// 
	IGN_ENUM_INPKEY_NUMPAD8						,													// 
	IGN_ENUM_INPKEY_NUMPAD9						,													// 
	IGN_ENUM_INPKEY_SUBTRACT					,													// MINUS SIGN (-) on numeric keypad 
	IGN_ENUM_INPKEY_NUMPAD4						,													// 
	IGN_ENUM_INPKEY_NUMPAD5						,													// 
	IGN_ENUM_INPKEY_NUMPAD6						,													// 
	IGN_ENUM_INPKEY_ADD							,													// PLUS SIGN (+) on numeric keypad 
	IGN_ENUM_INPKEY_NUMPAD1						,													// 
	IGN_ENUM_INPKEY_NUMPAD2						,													// 
	IGN_ENUM_INPKEY_NUMPAD3						,													// 
	IGN_ENUM_INPKEY_NUMPAD0						,													// 
	IGN_ENUM_INPKEY_DECIMAL						,													// PERIOD (decimal point) on numeric keypad 
	IGN_ENUM_INPKEY_OEM_102						= 0x56,												// On British and German keyboards 
	IGN_ENUM_INPKEY_F11							,													// 
	IGN_ENUM_INPKEY_F12							,													// 
	IGN_ENUM_INPKEY_F13							= 0x64,												// On NEC PC-98 Japanese keyboard 
	IGN_ENUM_INPKEY_F14							,													// On NEC PC-98 Japanese keyboard 
	IGN_ENUM_INPKEY_F15							,													// On NEC PC-98 Japanese keyboard 
	IGN_ENUM_INPKEY_KANA						= 0x70,												// On Japanese keyboard 
	IGN_ENUM_INPKEY_ABNT_C1						= 0x73,												// On numeric pad of Brazilian keyboards 
	IGN_ENUM_INPKEY_CONVERT						= 0x79,												// On Japanese keyboard 
	IGN_ENUM_INPKEY_NOCONVERT					= 0x7B,												// On Japanese keyboard 
	IGN_ENUM_INPKEY_YEN							= 0x7D,												// 
	IGN_ENUM_INPKEY_ABNT_C2						,													// On numeric pad of Brazilian keyboards 
	IGN_ENUM_INPKEY_NUMPADEQUALS				= 0x8D,												// On numeric keypad of NEC PC-98 Japanese keyboard 
	IGN_ENUM_INPKEY_PREVTRACK					= 0x90,												// Previous track; circumflex on Japanese keyboard 
	IGN_ENUM_INPKEY_AT							,													// On Japanese keyboard 
	IGN_ENUM_INPKEY_COLON						,													// On Japanese keyboard 
	IGN_ENUM_INPKEY_UNDERLINE					,													// On NEC PC-98 Japanese keyboard 
	IGN_ENUM_INPKEY_KANJI						,													// On Japanese keyboard 
	IGN_ENUM_INPKEY_STOP						,													// On NEC PC-98 Japanese keyboard 
	IGN_ENUM_INPKEY_AX							,													// On Japanese keyboard 
	IGN_ENUM_INPKEY_UNLABELED					,													// On Japanese keyboard 
	IGN_ENUM_INPKEY_NEXTTRACK					= 0x99,												// Next track 
	IGN_ENUM_INPKEY_NUMPADENTER					= 0x9C,												// 
	IGN_ENUM_INPKEY_RCONTROL					,													// Right CTRL 
	IGN_ENUM_INPKEY_MUTE						= 0xA0,												// 
	IGN_ENUM_INPKEY_CALCULATOR					,													// 
	IGN_ENUM_INPKEY_PLAYPAUSE					,													// 
	IGN_ENUM_INPKEY_MEDIASTOP					= 0xA4,												// 
	IGN_ENUM_INPKEY_VOLUMEDOWN					= 0xAE,												// 
	IGN_ENUM_INPKEY_VOLUMEUP					,													// 
	IGN_ENUM_INPKEY_WEBHOME						= 0xB2,												// 
	IGN_ENUM_INPKEY_NUMPADCOMMA					,													// On numeric keypad of NEC PC-98 Japanese keyboard 
	IGN_ENUM_INPKEY_DIVIDE						= 0xB5,												// Forward slash (/) on numeric keypad 
	IGN_ENUM_INPKEY_SYSRQ						= 0xB7,												// 
	IGN_ENUM_INPKEY_RMENU						,													// Right ALT 
	IGN_ENUM_INPKEY_PAUSE						= 0xC5,												// 
	IGN_ENUM_INPKEY_HOME						= 0xC7,												// 
	IGN_ENUM_INPKEY_UP							,													// UP ARROW 
	IGN_ENUM_INPKEY_PRIOR						,													// PAGE UP 
	IGN_ENUM_INPKEY_LEFT						= 0xCB,												// LEFT ARROW 
	IGN_ENUM_INPKEY_RIGHT						= 0xCD,												// RIGHT ARROW 
	IGN_ENUM_INPKEY_END							= 0xCF,												// 
	IGN_ENUM_INPKEY_DOWN						,													// DOWN ARROW 
	IGN_ENUM_INPKEY_NEXT						,													// PAGE DOWN 
	IGN_ENUM_INPKEY_INSERT						,													// 
	IGN_ENUM_INPKEY_DELETE						,													// 
	IGN_ENUM_INPKEY_LWIN						= 0xDB,												// Left Microsoft® Windows® logo key 
	IGN_ENUM_INPKEY_RWIN						,													// Right Windows logo key 
	IGN_ENUM_INPKEY_APPS						,													// Application key 
	IGN_ENUM_INPKEY_POWER						,													// 
	IGN_ENUM_INPKEY_SLEEP						,													// 
	IGN_ENUM_INPKEY_WAKE						= 0xE3,												// 
	IGN_ENUM_INPKEY_WEBSEARCH					= 0xE5,												// 
	IGN_ENUM_INPKEY_WEBFAVORITES				,													// Displays the Microsoft Internet Explorer Favorites list ...
	IGN_ENUM_INPKEY_WEBREFRESH					,													// 
	IGN_ENUM_INPKEY_WEBSTOP						,													// 
	IGN_ENUM_INPKEY_WEBFORWARD					,													// 
	IGN_ENUM_INPKEY_WEBBACK						,													// 
	IGN_ENUM_INPKEY_MYCOMPUTER					,													// 
	IGN_ENUM_INPKEY_MAIL						,													// 
	IGN_ENUM_INPKEY_MEDIASELECT					,													// Media Select key
	IGN_ENUM_INPKEY_COUNT						,													// Don't use this !!!
	IGN_ENUM_INPKEY_FORCEDWORD					= 0x7fffffff
};

enum			IGN_ENUM_COOPLEVEL
{
	IGN_ENUM_COOPLEVEL_EXCLUSIVE				= 0x00000001,										// The application requires exclusive access
	IGN_ENUM_COOPLEVEL_NONEXCLUSIVE				= 0x00000002,										// The application requires nonexclusive access
	IGN_ENUM_COOPLEVEL_FOREGROUND				= 0x00000004,										// The application requires foreground access
	IGN_ENUM_COOPLEVEL_BACKGROUND				= 0x00000008,										// The application requires background access
	IGN_ENUM_COOPLEVEL_NOWINKEY					= 0x00000010,										// Disable the Microsoft Windows® logo key
	IGN_ENUM_COOPLEVEL_FORCEDWORD				= 0x7fffffff
};

enum			IGN_ENUM_INPMOUSEMODE
{
	IGN_ENUM_INPMOUSEMODE_RELATIVEAXISCOORDS	= 0,												// Show the amount by which position changed since last get
	IGN_ENUM_INPMOUSEMODE_ABSOLUTEAXISCOORDS	= 1,												// Show the position in relation to a fixed point
	IGN_ENUM_INPMOUSEMODE_FORCEDWORD			= 0x7fffffff
};

enum			IGN_ENUM_MOUSEDATAOFFSET
{
	IGN_ENUM_MOUSEDATAOFFSET_X					= 0,
	IGN_ENUM_MOUSEDATAOFFSET_Y					= 1,
	IGN_ENUM_MOUSEDATAOFFSET_Z					= 2,
	IGN_ENUM_MOUSEDATAOFFSET_BUTTON0			= 3,
	IGN_ENUM_MOUSEDATAOFFSET_BUTTON1			= 4,
	IGN_ENUM_MOUSEDATAOFFSET_BUTTON2			= 5,
	IGN_ENUM_MOUSEDATAOFFSET_BUTTON3			= 6,
	IGN_ENUM_MOUSEDATAOFFSET_FORCEDWORD			= 0x7fffffff
};




typedef		IGN_OBJIF_InpInstance*			IGN_LP_OBJIF_InpInstance;
typedef		IGN_OBJIF_InpContext*			IGN_LP_OBJIF_InpContext;
typedef		IGN_OBJIF_InpKeyboard*			IGN_LP_OBJIF_InpKeyboard;
typedef		IGN_OBJIF_InpMouse*				IGN_LP_OBJIF_InpMouse;




//********************************************************************************************************************************************************************
//				Typedefs for all interface methods
//********************************************************************************************************************************************************************

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// DLL export methods
//
typedef			IGNRESULT	IGN_DLLCALL	IGN_METHOD_GetSubHookInput							(			IGN_STRUCT_IFSubInput**			a_ppInterface				);

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// DLL interface methods
//
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_InpGetVersion							(			IGN_STRUCT_InpVersion&			a_Version					);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_InpCreateInstance						(			IGN_ENUM_INPINSTANCETYPE		a_InstanceType,
																										IGN_LP_OBJIF_InpInstance&		a_pInstanceInterface		);

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Instance methods	
//
typedef			IGN_ENUM_INPINSTANCETYPE
							IGN_OBJCALL	IGN_METHOD_InpInstance_GetInstanceType				(			void														);														
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_InpInstance_EnumContext					(			IGNU32							a_u32ContextNumber,
																										IGN_STRUCT_InpContextDesc&		a_ContextDescription		);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_InpInstance_GetContextCount				(			IGNU32&							a_u32ContextCount			);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_InpInstance_CreateContext				(			IGNU32							a_u32ContextNumber,
																										HWND							a_hWindow,
																										IGN_LP_OBJIF_InpContext&		a_pContextInterface			);	

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Context methods																				
//
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_InpContext_CreateKeyboard				(			IGN_ENUM_COOPLEVEL				a_CoOpLevel,
																										IGN_LP_OBJIF_InpKeyboard&		a_pKeyboardInterface		);																				
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_InpContext_CreateMouse					(			IGN_ENUM_COOPLEVEL				a_CoOpLevel,
																										IGN_LP_OBJIF_InpMouse&			a_pMouseInterface			);

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Keyboard methods																				
//
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_InpKeyboard_GetState						(			IGN_STRUCT_KeyboardState&		a_State						);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_InpKeyboard_SetBufferSize				(			IGNU32							a_u32BufferSize				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_InpKeyboard_GetBufferSize				(			IGNU32&							a_u32BufferSize				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_InpKeyboard_GetBufferedData				(			IGNU32&							a_u32ItemsInOut,
																										IGN_STRUCT_BufferedInpData*		a_KeyboardBufData			);

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Mouse methods																				
//
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_InpMouse_SetMode							(			IGN_ENUM_INPMOUSEMODE			a_Mode						);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_InpMouse_GetMode							(			IGN_ENUM_INPMOUSEMODE&			a_Mode						);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_InpMouse_GetState						(			IGN_STRUCT_MouseState&			a_State						);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_InpMouse_SetBufferSize					(			IGNU32							a_u32BufferSize				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_InpMouse_GetBufferSize					(			IGNU32&							a_u32BufferSize				);
typedef			IGNRESULT	IGN_OBJCALL	IGN_METHOD_InpMouse_GetBufferedData					(			IGNU32&							a_u32ItemsInOut,
																										IGN_STRUCT_BufferedInpData*		a_MouseBufData				);




//------------- DECLARATIONS -----------------------------------------------------------------------------------------------------------------------------------------

struct	IGN_STRUCT_IFSubInput
{
	IGN_METHOD_InpGetVersion*							m_pGetVersion;								// Pointer to GetVersion(..) method
	IGN_METHOD_InpCreateInstance*						m_pCreateInstance;							// Pointer to CreateInstance(..) method
};

struct	IGN_STRUCT_InpVersion
{
	IGNCHAR												m_pcharIdentifier[256];						// Module identifier string
	IGNCHAR												m_pcharVendor[256];							// Vendor string
	IGNU32												m_u32MainVersionNumber;						// Main version number of module
	IGNU32												m_u32SubVersionNumber;						// Sub version number of module
	IGNCHAR												m_pcharReserved[256];						// Reserved (not in use)
};

struct	IGN_STRUCT_InpContextDesc
{
	IGNU32												m_u32Reserved;								// Reserved (not in use)
};

struct	IGN_STRUCT_KeyboardState
{
	IGNBOOL												m_bKeyDown[IGN_ENUM_INPKEY_COUNT];			// Current state of the keyboard 
};

struct	IGN_STRUCT_MouseState
{
	IGNINT												m_iX;										// X-axis
	IGNINT												m_iY;										// Y-axis
	IGNINT												m_iZ;										// Z-axis
	IGNBOOL												m_bButtonDown[4];							// Current state of the first four mouse buttons
};

struct	IGN_STRUCT_BufferedInpData
{
	IGNU32												m_u32Offset;								// The offset into the keyboard data format 
	IGNU32												m_u32Data;									// Data obtained from the keyboard
	IGNU32												m_u32TimeStamp;								// System time at which the input event was generated, in milliseconds
	IGNU32												m_u32Sequence;								// Sequence number for this event
};




//********************************************************************************************************************************************************************
//				Concrete object interfaces
//				See separate manual for detailed documentation
//********************************************************************************************************************************************************************
															
//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Instance interface - SINGLETON	
//
IGN_OBJ_INTERFACE_DECL_BaseObject( IGN_OBJIF_InpInstance )
{
	//
	// Version 1.0.0
	//

	//
	// Retrieves a copy of the instance type
	//
	IGN_OBJ_METHOD( IGN_METHOD_InpInstance_GetInstanceType,					GetInstanceType					);

	//
	// Retrieves a copy of the adapter and mode description of specified context
	//
	IGN_OBJ_METHOD(	IGN_METHOD_InpInstance_EnumContext,						EnumContext						);

	//
	// Retrieves a copy of the count of possible contexts
	//
	IGN_OBJ_METHOD(	IGN_METHOD_InpInstance_GetContextCount,					GetContextCount					);

	//
	// Creates a new IGN_OBJIF_InpContext object
	//
	IGN_OBJ_METHOD(	IGN_METHOD_InpInstance_CreateContext,					CreateContext					);
};											
																
//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Context interface - SINGLETON	
//
IGN_OBJ_INTERFACE_DECL_BaseObject( IGN_OBJIF_InpContext )
{
	//
	// Version 1.0.0
	//

	//
	// Creates a new IGN_OBJIF_InpKeyboard object
	//
	IGN_OBJ_METHOD(	IGN_METHOD_InpContext_CreateKeyboard,					CreateKeyboard					);

	//
	// Creates a new IGN_OBJIF_InpMouse object
	//
	IGN_OBJ_METHOD(	IGN_METHOD_InpContext_CreateMouse,						CreateMouse						);
};														
									
//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Keyboard interface	
//
IGN_OBJ_INTERFACE_DECL_ExecutableObject( IGN_OBJIF_InpKeyboard )
{
	//
	// Version 1.0.0
	//

	//
	// Retrieves the current input snapshot (generated on latest Execute() call)
	//
	IGN_OBJ_METHOD(	IGN_METHOD_InpKeyboard_GetState,						GetState						);

	//
	// Sets the current buffer size
	//
	IGN_OBJ_METHOD(	IGN_METHOD_InpKeyboard_SetBufferSize,					SetBufferSize					);

	//
	// Retrieves the current buffer size
	//
	IGN_OBJ_METHOD(	IGN_METHOD_InpKeyboard_GetBufferSize,					GetBufferSize					);

	//
	// Retrieves the buffered data since the latest Execute() call
	//
	IGN_OBJ_METHOD(	IGN_METHOD_InpKeyboard_GetBufferedData,					GetBufferedData					);
};														
									
//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Mouse interface	
//
IGN_OBJ_INTERFACE_DECL_ExecutableObject( IGN_OBJIF_InpMouse )
{
	//
	// Version 1.0.0
	//

	//
	// Sets the coordinates reporting mode
	//
	IGN_OBJ_METHOD(	IGN_METHOD_InpMouse_SetMode,							SetMode							);

	//
	// Gets the coordinates reporting mode
	//
	IGN_OBJ_METHOD( IGN_METHOD_InpMouse_GetMode,							GetMode							);

	//
	// Retrieves the current input snapshot (generated on latest Execute() call)
	//
	IGN_OBJ_METHOD(	IGN_METHOD_InpMouse_GetState,							GetState						);

	//
	// Sets the current buffer size
	//
	IGN_OBJ_METHOD(	IGN_METHOD_InpMouse_SetBufferSize,						SetBufferSize					);

	//
	// Retrieves the current buffer size
	//
	IGN_OBJ_METHOD(	IGN_METHOD_InpMouse_GetBufferSize,						GetBufferSize					);

	//
	// Retrieves the buffered data since the latest Execute() call
	//
	IGN_OBJ_METHOD(	IGN_METHOD_InpMouse_GetBufferedData,					GetBufferedData					);
};



																
//------------- PREPROCESSOR END -------------------------------------------------------------------------------------------------------------------------------------

#endif // IGN_INPUT_SUB_SHARED_H_INCLUDED





