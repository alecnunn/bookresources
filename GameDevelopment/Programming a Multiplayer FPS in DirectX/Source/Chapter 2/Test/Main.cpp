//-----------------------------------------------------------------------------
// System Includes
//-----------------------------------------------------------------------------
#include <windows.h>

//-----------------------------------------------------------------------------
// Engine Includes
//-----------------------------------------------------------------------------
#include "..\Engine\Engine.h"

//-----------------------------------------------------------------------------
// Entry point for the application.
//-----------------------------------------------------------------------------
int WINAPI WinMain( HINSTANCE instance, HINSTANCE prev, LPSTR cmdLine, int cmdShow )
{
	// Create the engine setup structure.
	EngineSetup setup;
	setup.instance = instance;
	setup.name = "Framework Test";

	// Create the engine (using the setup structure), then run it.
	new Engine( &setup );
	g_engine->Run();

	return true;
}