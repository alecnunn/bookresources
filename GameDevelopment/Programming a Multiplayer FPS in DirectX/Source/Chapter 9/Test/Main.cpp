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
	// Allows the test state to preform any pre-processing construction.
	//-------------------------------------------------------------------------
	virtual void Load()
	{
		// Create an animated object, and play one of its animations.
		m_character = new AnimatedObject( "Marine.x", "./Assets/" );
		m_character->SetTranslation( -100.0f, 0.0f, 0.0f );
		m_character->SetRotation( 0.0f, 3.14f, 0.0f );
		m_character->PlayAnimation( 1, 0.0f );

		// Create a scene object that will be used as a camera.
		m_viewer = new SceneObject;
		m_viewer->SetTranslation( 0.0f, 0.0f, -400.0f );
		m_viewer->SetFriction( 5.0f );

		// Create a test spawner object.
		m_spawner = new SpawnerObject( "Spawner.txt", "./Assets/" );
		m_spawner->SetTranslation( 100.0f, -50.0f, 0.0f );
	};

	//-------------------------------------------------------------------------
	// Allows the test state to preform any post-processing destruction.
	//-------------------------------------------------------------------------
	virtual void Close()
	{
		SAFE_DELETE( m_character );
		SAFE_DELETE( m_viewer );
		SAFE_DELETE( m_spawner );
	};

	//-------------------------------------------------------------------------
	// Returns the view setup details for the given frame.
	//-------------------------------------------------------------------------
	virtual void RequestViewer( ViewerSetup *viewer )
	{
		viewer->viewer = m_viewer;
		viewer->viewClearFlags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER;
	}

	//-------------------------------------------------------------------------
	// Updates the test state.
	//-------------------------------------------------------------------------
	virtual void Update( float elapsed )
	{
		// Allow the camera (scene object) to be moved around.
		if( g_engine->GetInput()->GetKeyPress( DIK_W, true ) == true )
			m_viewer->Drive( 1000.0f * elapsed, false );
		if( g_engine->GetInput()->GetKeyPress( DIK_S, true ) == true )
			m_viewer->Drive( -1000.0f * elapsed, false );
		if( g_engine->GetInput()->GetKeyPress( DIK_A, true ) == true )
			m_viewer->Strafe( -1000.0f * elapsed, false );
		if( g_engine->GetInput()->GetKeyPress( DIK_D, true ) == true )
			m_viewer->Strafe( 1000.0f * elapsed, false );

		// Adjust the rotation of the camera based on the mouse movement.
		m_viewer->AddRotation( (float)g_engine->GetInput()->GetDeltaY() * elapsed, (float)g_engine->GetInput()->GetDeltaX() * elapsed, 0.0f );

		m_character->Update( elapsed );
		m_viewer->Update( elapsed );
		m_spawner->Update( elapsed );
	};

	//-------------------------------------------------------------------------
	// Renders the test state.
	//-------------------------------------------------------------------------
	virtual void Render()
	{
		m_character->Render();
		m_spawner->Render();
	};

private:
	AnimatedObject *m_character; // An animated character object.
	SceneObject *m_viewer; // The viewer scene object is used as a camera.
	SpawnerObject *m_spawner; // A test spawner object.
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
	setup.name = "Object Test";
	setup.scale = 0.01f;
	setup.StateSetup = StateSetup;

	// Create the engine (using the setup structure), then run it.
	new Engine( &setup );
	g_engine->Run();

	return true;
}