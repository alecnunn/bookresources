//-----------------------------------------------------------------------------
// System Includes
//-----------------------------------------------------------------------------
#include <windows.h>

//-----------------------------------------------------------------------------
// Engine Includes
//-----------------------------------------------------------------------------
#include "..\Engine\Engine.h"

//-----------------------------------------------------------------------------
// Test State Class
//-----------------------------------------------------------------------------
class TestState : public State
{
public:
	//-------------------------------------------------------------------------
	// Allows the state to preform any pre-processing construction.
	//-------------------------------------------------------------------------
	virtual void Load()
	{
		m_font = new Font;
	}

	//-------------------------------------------------------------------------
	// Allows the state to preform any post-processing destruction.
	//-------------------------------------------------------------------------
	virtual void Close()
	{
		SAFE_DELETE( m_font );
	}

	//-------------------------------------------------------------------------
	// Returns the view setup details for the given frame.
	//-------------------------------------------------------------------------
	virtual void RequestViewer( ViewerSetup *viewer )
	{
		viewer->viewClearFlags = D3DCLEAR_TARGET;
	}

	//-------------------------------------------------------------------------
	// Updates the state.
	//-------------------------------------------------------------------------
	virtual void Update( float elapsed )
	{
		// Calculate the frame rate.
		static float frameTime = 1.0f;
		static int frameCount = 0;
		frameTime += elapsed;
		frameCount++;

		// Update the fps every second.
		if( frameTime > 1.0f )
		{
			sprintf( m_fps, "%d fps", frameCount );

			frameTime = 0.0f;
			frameCount = 0;
		}
	}

	//-------------------------------------------------------------------------
	// Renders the state.
	//-------------------------------------------------------------------------
	virtual void Render()
	{
		m_font->Render( "This text is rendered using the new Direct3D device.", 10, 10 );
		m_font->Render( m_fps, 10, 50, D3DCOLOR_COLORVALUE( 1.0f, 1.0f, 0.0f, 1.0f ) );
	}

private:
	Font *m_font; // A font used to render text.
	char m_fps[16]; // Stores the frame rate as text to be render.
};

//-----------------------------------------------------------------------------
// Application specific state setup.
//-----------------------------------------------------------------------------
void StateSetup()
{
	g_engine->AddState( new TestState, true );
}

//-----------------------------------------------------------------------------
// Entry point for the application.
//-----------------------------------------------------------------------------
int WINAPI WinMain( HINSTANCE instance, HINSTANCE prev, LPSTR cmdLine, int cmdShow )
{
	// Create the engine setup structure.
	EngineSetup setup;
	setup.instance = instance;
	setup.name = "Rendering Test";
	setup.totalBackBuffers = 2;
	setup.StateSetup = StateSetup;

	// Create the engine (using the setup structure), then run it.
	new Engine( &setup );
	g_engine->Run();

	return true;
}