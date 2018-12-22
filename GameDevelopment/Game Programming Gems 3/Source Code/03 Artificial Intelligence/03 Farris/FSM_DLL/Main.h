#ifndef _fsm_main_h
#define _fsm_main_h

// Windows
#ifdef WIN32

	// Defines
	#define WIN32_LEAN_AND_MEAN

	// System Includes
	#include <windows.h>

	// Defines
	#ifdef FSM_EXPORTS
		#define FSM_API __declspec(dllexport)
	#else
		#define FSM_API __declspec(dllimport)
	#endif

#else

	// Defines
	#define FSM_API 

#endif

#endif
