//-----------------------------------------------------------------------------
// Engine.h implementation.
// Refer to the Engine.h interface for more details.
//
// Programming a Multiplayer First Person Shooter in DirectX
// Copyright (c) 2004 Vaughan Young
//-----------------------------------------------------------------------------
#include "Engine.h"

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------
Engine *g_engine = NULL;

//-----------------------------------------------------------------------------
// Handles Windows messages.
//-----------------------------------------------------------------------------
LRESULT CALLBACK WindowProc( HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
	switch( msg )
	{
		case WM_ACTIVATEAPP:
			g_engine->SetDeactiveFlag( !wparam );
			return 0;

		case WM_DESTROY:
			PostQuitMessage( 0 );
			return 0;

		default:
			return DefWindowProc( wnd, msg, wparam, lparam );
	}
}

//-----------------------------------------------------------------------------
// The engine class constructor.
//-----------------------------------------------------------------------------
Engine::Engine( EngineSetup *setup )
{
	// Indicate that the engine is not yet loaded.
	m_loaded = false;

	// If no setup structure was passed in, then create a default one.
	// Otehrwise, make a copy of the passed in structure.
	m_setup = new EngineSetup;
	if( setup != NULL )
		memcpy( m_setup, setup, sizeof( EngineSetup ) );

	// Store a pointer to the engine in a global variable for easy access.
	g_engine = this;

	// Prepare and register the window class.
	WNDCLASSEX wcex;
	wcex.cbSize        = sizeof( WNDCLASSEX );
	wcex.style         = CS_CLASSDC;
	wcex.lpfnWndProc   = WindowProc;
	wcex.cbClsExtra    = 0;
	wcex.cbWndExtra    = 0;
	wcex.hInstance     = m_setup->instance;
	wcex.hIcon         = LoadIcon( NULL, IDI_APPLICATION );
	wcex.hCursor       = LoadCursor( NULL, IDC_ARROW );
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName  = NULL;
	wcex.lpszClassName = "WindowClass";
	wcex.hIconSm       = LoadIcon( NULL, IDI_APPLICATION );
	RegisterClassEx( &wcex );

	// Initialise the COM using multithreaded concurrency.
	CoInitializeEx( NULL, COINIT_MULTITHREADED );

	// Create the window and retrieve a handle to it.
	// Note: Later the window will be created using a windowed/fullscreen flag.
	m_window = CreateWindow( "WindowClass", m_setup->name, WS_OVERLAPPED, 0, 0, 800, 600, NULL, NULL, m_setup->instance, NULL );

	// Create the linked lists of states.
	m_states = new LinkedList< State >;
	m_currentState = NULL;

	// Create the resource managers.
	m_scriptManager = new ResourceManager< Script >;

	// Create the input object.
	m_input = new Input( m_window );

	// Seed the random number generator with the current time.
	srand( timeGetTime() );

	// Allow the application to perform any state setup now.
	if( m_setup->StateSetup != NULL )
		m_setup->StateSetup();

	// The engine is fully loaded and ready to go.
	m_loaded = true;
}

//-----------------------------------------------------------------------------
// The engine class destructor.
//-----------------------------------------------------------------------------
Engine::~Engine()
{
	// Ensure the engine is loaded.
	if( m_loaded == true )
	{
		// Destroy the states linked lists.
		if( m_currentState != NULL )
			m_currentState->Close();
		SAFE_DELETE( m_states );

		// Destroy everything.
		SAFE_DELETE( m_input );
		SAFE_DELETE( m_scriptManager );
	}

	// Uninitialise the COM.
	CoUninitialize();

	// Unregister the window class.
	UnregisterClass( "WindowClass", m_setup->instance );

	// Destroy the engine setup structure.
	SAFE_DELETE( m_setup );
}

//-----------------------------------------------------------------------------
// Enters the engine into the main processing loop.
//-----------------------------------------------------------------------------
void Engine::Run()
{
	// Ensure the engine is loaded.
	if( m_loaded == true )
	{
		// Show the window.
		ShowWindow( m_window, SW_NORMAL );

		// Used to retrieve details about the viewer from the application.
		ViewerSetup viewer;

		// Enter the message loop.
		MSG msg;
		ZeroMemory( &msg, sizeof( MSG ) );
		while( msg.message != WM_QUIT )
		{
			if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			else if( !m_deactive )
			{
				// Calculate the elapsed time.
				unsigned long currentTime = timeGetTime();
				static unsigned long lastTime = currentTime;
				float elapsed = ( currentTime - lastTime ) / 1000.0f;
				lastTime = currentTime;

				// Update the input object, reading the keyboard and mouse.
				m_input->Update();

				// Check if the user wants to make a forced exit.
				if( m_input->GetKeyPress( DIK_F1 ) )
					PostQuitMessage( 0 );

				// Request the viewer from the current state, if there is one.
				if( m_currentState != NULL )
					m_currentState->RequestViewer( &viewer );

				// Update the current state (if there is one), taking state
				// changes into account.
				m_stateChanged = false;
				if( m_currentState != NULL )
					m_currentState->Update( elapsed );
				if( m_stateChanged == true )
					continue;
			}
		}
	}

	// Destroy the engine.
	SAFE_DELETE( g_engine );
}

//-----------------------------------------------------------------------------
// Returns the window handle.
//-----------------------------------------------------------------------------
HWND Engine::GetWindow()
{
	return m_window;
}

//-----------------------------------------------------------------------------
// Sets the deactive flag.
//-----------------------------------------------------------------------------
void Engine::SetDeactiveFlag( bool deactive )
{
	m_deactive = deactive;
}

//-----------------------------------------------------------------------------
// Adds a state to the engine.
//-----------------------------------------------------------------------------
void Engine::AddState( State *state, bool change )
{
	m_states->Add( state );

	if( change == false )
		return;

	if( m_currentState != NULL )
		m_currentState->Close();

	m_currentState = m_states->GetLast();
	m_currentState->Load();
}

//-----------------------------------------------------------------------------
// Removes a state from the engine
//-----------------------------------------------------------------------------
void Engine::RemoveState( State *state )
{
	m_states->Remove( &state );
}

//-----------------------------------------------------------------------------
// Changes processing to the state with the specified ID.
//-----------------------------------------------------------------------------
void Engine::ChangeState( unsigned long id )
{
	// Iterate through the list of states and find the new state to change to.
	m_states->Iterate( true );
	while( m_states->Iterate() != NULL )
	{
		if( m_states->GetCurrent()->GetID() == id )
		{
			// Close the old state.
			if( m_currentState != NULL )
				m_currentState->Close();

			// Set the new current state and load it.
			m_currentState = m_states->GetCurrent();
			m_currentState->Load();

			// Indicate that the state has changed.
			m_stateChanged = true;

			break;
		}
	}
}

//-----------------------------------------------------------------------------
// Returns a pointer to the current state.
//-----------------------------------------------------------------------------
State *Engine::GetCurrentState()
{
	return m_currentState;
}

//-----------------------------------------------------------------------------
// Returns a pointer to the script manager.
//-----------------------------------------------------------------------------
ResourceManager< Script > *Engine::GetScriptManager()
{
	return m_scriptManager;
}

//-----------------------------------------------------------------------------
// Returns a pointer to the input object.
//-----------------------------------------------------------------------------
Input *Engine::GetInput()
{
	return m_input;
}