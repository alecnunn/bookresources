//-----------------------------------------------------------------------------
// The primary engine header file. This file links the entire engine together
// and is the only header file that needs to be included in any project using
// the engine.
//
// Programming a Multiplayer First Person Shooter in DirectX
// Copyright (c) 2004 Vaughan Young
//-----------------------------------------------------------------------------
#ifndef ENGINE_H
#define ENGINE_H

//-----------------------------------------------------------------------------
// DirectInput Version Define
//-----------------------------------------------------------------------------
#define DIRECTINPUT_VERSION 0x0800

//-----------------------------------------------------------------------------
// System Includes
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <tchar.h>
#include <windowsx.h>

//-----------------------------------------------------------------------------
// DirectX Includes
//-----------------------------------------------------------------------------
#include <d3dx9.h>
#include <dinput.h>
#include <dplay8.h>
#include <dmusici.h>

//-----------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------
#define SAFE_DELETE( p )       { if( p ) { delete ( p );     ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p ) { if( p ) { delete[] ( p );   ( p ) = NULL; } }
#define SAFE_RELEASE( p )      { if( p ) { ( p )->Release(); ( p ) = NULL; } }

//-----------------------------------------------------------------------------
// Engine Includes
//-----------------------------------------------------------------------------
#include "LinkedList.h"
#include "ResourceManagement.h"
#include "Geometry.h"
#include "Scripting.h"
#include "Input.h"
#include "State.h"

//-----------------------------------------------------------------------------
// Engine Setup Structure
//-----------------------------------------------------------------------------
struct EngineSetup
{
	HINSTANCE instance; // Application instance handle.
	char *name; // Name of the application.
	void (*StateSetup)(); // State setup function.

	//-------------------------------------------------------------------------
	// The engine setup structure constructor.
	//-------------------------------------------------------------------------
	EngineSetup()
	{
		instance = NULL;
		name = "Application";
		StateSetup = NULL;
	}
};

//-----------------------------------------------------------------------------
// Engine Class
//-----------------------------------------------------------------------------
class Engine
{
public:
	Engine( EngineSetup *setup = NULL );
	virtual ~Engine();

	void Run();

	HWND GetWindow();
	void SetDeactiveFlag( bool deactive );

	void AddState( State *state, bool change = true );
	void RemoveState( State *state );
	void ChangeState( unsigned long id );
	State *GetCurrentState();

	ResourceManager< Script > *GetScriptManager();

	Input *GetInput();

private:
	bool m_loaded; // Indicates if the engine is loading.
	HWND m_window; // Main window handle.
	bool m_deactive; // Indicates if the application is active or not.

	EngineSetup *m_setup; // Copy of the engine setup structure.

	LinkedList< State > *m_states; // Linked list of states.
	State *m_currentState; // Pointer to the current state.
	bool m_stateChanged; // Indicates if the state changed in the current frame.

	ResourceManager< Script > *m_scriptManager; // Script manager.

	Input *m_input; // Input object.
};

//-----------------------------------------------------------------------------
// Externals
//-----------------------------------------------------------------------------
extern Engine *g_engine;

#endif