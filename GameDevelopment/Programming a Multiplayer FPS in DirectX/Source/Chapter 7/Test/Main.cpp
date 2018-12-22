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
		m_font = new Font;
		m_text[0] = 0;
		m_connected = false;
	};

	//-------------------------------------------------------------------------
	// Allows the test state to preform any post-processing destruction.
	//-------------------------------------------------------------------------
	virtual void Close()
	{
		SAFE_DELETE( m_font );
	};

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
		// Check if the user wants to host a session.
		if( g_engine->GetInput()->GetKeyPress( DIK_H ) == true )
			m_connected = g_engine->GetNetwork()->Host( "host player", "session", 8 );

		// Check if the user wants to enumerate the sessions on the network.
		if( g_engine->GetInput()->GetKeyPress( DIK_E ) == true )
			g_engine->GetNetwork()->EnumerateSessions();

		// Check if the user wants to join the first session.
		if( g_engine->GetInput()->GetKeyPress( DIK_1 ) == true )
			m_connected = g_engine->GetNetwork()->Join( "join player", 0 );

		// Check if the user wants to join the second session.
		if( g_engine->GetInput()->GetKeyPress( DIK_2 ) == true )
			m_connected = g_engine->GetNetwork()->Join( "join player", 1 );

		// Check if the user wants to join the third session.
		if( g_engine->GetInput()->GetKeyPress( DIK_2 ) == true )
			m_connected = g_engine->GetNetwork()->Join( "join player", 2 );

		// Check if the user wants to quit.
		if( g_engine->GetInput()->GetKeyPress( DIK_Q ) == true )
			PostQuitMessage( 0 );

		// Create the menu text.
		strcpy( m_text, "\n\n\nH - Host Session\n\nE - Enumerate Sessions" );

		// Go through the list of sessions found on the local network.
		char count = 0;
		char name[MAX_PATH];
		SessionInfo *session = g_engine->GetNetwork()->GetNextSession( true );
		while( session != NULL )
		{
			// Add the session count.
			sprintf( name, "\n   %d - ", ++count );
			strcat( m_text, name );

			// Add the session name.
			wcstombs( name, session->description.pwszSessionName, MAX_PATH );
			strcat( m_text, name );

			// Go to the next session.
			session = g_engine->GetNetwork()->GetNextSession();
		}

		// Add the quit option to the menu text.
		strcat( m_text, "\n\nQ - Quit" );

		// Add the connection status text.
		if( m_connected == true )
		{
			if( g_engine->GetNetwork()->IsHost() == true )
				strcat( m_text, "\n\nCONNECTED - HOST" );
			else
				strcat( m_text, "\n\nCONNECTED - CLIENT" );
		}
		else
			strcat( m_text, "\n\nNOT CONNECTED" );
	};

	//-------------------------------------------------------------------------
	// Renders the state.
	//-------------------------------------------------------------------------
	virtual void Render()
	{
		m_font->Render( m_text, 0.0f, 0.0f );
	};

private:
	Font *m_font; // Font to display the menu options.
	char m_text[MAX_PATH]; // Text for the menu options.
	bool m_connected; // Indicates if the network object is connected.
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
	// A unique id for the test so it can be identified on a network.
	GUID guid = { 0x9ca7996f, 0xebb4, 0x4e9b, { 0xba, 0x6b, 0xe6, 0x4b, 0x45, 0x7f, 0x59, 0x98 } };

	// Create the engine setup structure.
	EngineSetup setup;
	setup.instance = instance;
	setup.guid = guid;
	setup.name = "Network Test";
	setup.StateSetup = StateSetup;

	// Create the engine (using the setup structure), then run it.
	new Engine( &setup );
	g_engine->Run();

	return true;
}