//-----------------------------------------------------------------------------
// Game.h implementation.
// Refer to the Game.h interface for more details.
//
// Programming a Multiplayer First Person Shooter in DirectX
// Copyright (c) 2004 Vaughan Young
//-----------------------------------------------------------------------------
#include "Main.h"

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------
Game *g_game = NULL;

SceneObject *object = NULL;

//-----------------------------------------------------------------------------
// Game class constructor.
//-----------------------------------------------------------------------------
Game::Game() : State( STATE_GAME )
{
	// Store a pointer to the game state in a global variable for easy access.
	g_game = this;

	// Invalidate the score board font.
	m_scoreBoardFont = NULL;
}

//-----------------------------------------------------------------------------
// Allows the game state to preform any pre-processing construction.
//-----------------------------------------------------------------------------
void Game::Load()
{
	// Hide the mouse cursor.
	ShowCursor( false );

	// Load the crosshair material.
	m_crosshair = g_engine->GetMaterialManager()->Add( "Crosshair.dds.txt", "./Assets/" );

	// Create the score board font.
	m_scoreBoardFont = new Font( "Arial", 14, FW_BOLD );
	m_scoreBoardNames[0] = 0;
	m_scoreBoardFrags[0] = 0;
	m_scoreBoardDeaths[0] = 0;
}

//-----------------------------------------------------------------------------
// Allows the game state to preform any post-processing destruction.
//-----------------------------------------------------------------------------
void Game::Close()
{
	// Show the mouse cursor.
	ShowCursor( true );

	// Terminate the session.
	g_engine->GetNetwork()->Terminate();

	// Destroy the scene.
	g_engine->GetSceneManager()->DestroyScene();

	// Destroy the score board font.
	SAFE_DELETE( m_scoreBoardFont );

	// Destroy the crosshair material.
	g_engine->GetMaterialManager()->Remove( &m_crosshair );
}

//-----------------------------------------------------------------------------
// Returns the view setup details for the given frame.
//-----------------------------------------------------------------------------
void Game::RequestViewer( ViewerSetup *viewer )
{
	viewer->viewClearFlags = D3DCLEAR_ZBUFFER;
}

//-----------------------------------------------------------------------------
// Update the game state.
//-----------------------------------------------------------------------------
void Game::Update( float elapsed )
{
	// Check if the user is holding down the tab key.
	if( g_engine->GetInput()->GetKeyPress( DIK_TAB, true ) == true )
	{
		// Build the score board text.
		sprintf( m_scoreBoardNames, "PLAYER\n" );
		sprintf( m_scoreBoardFrags, "FRAGS\n" );
		sprintf( m_scoreBoardDeaths, "DEATHS\n" );
	}

	// Check if the user wants to exit back to the menu.
	if( g_engine->GetInput()->GetKeyPress( DIK_ESCAPE ) )
		g_engine->ChangeState( STATE_MENU );
}

//-----------------------------------------------------------------------------
// Render the game state.
//-----------------------------------------------------------------------------
void Game::Render()
{
	// Ensure the scene is loaded.
	if( g_engine->GetSceneManager()->IsLoaded() == false )
		return;

	// If the user is holding down the tab key, then render the score board.
	if( g_engine->GetInput()->GetKeyPress( DIK_TAB, true ) == true )
	{
		m_scoreBoardFont->Render( m_scoreBoardNames, 20, 100, 0xFFFF7700 );
		m_scoreBoardFont->Render( m_scoreBoardFrags, 180, 100, 0xFFFF7700 );
		m_scoreBoardFont->Render( m_scoreBoardDeaths, 260, 100, 0xFFFF7700 );
	}

	// Draw the local player's crosshair in the centre of the screen.
	g_engine->GetSprite()->Begin( D3DXSPRITE_ALPHABLEND );
	g_engine->GetSprite()->Draw( m_crosshair->GetTexture(), NULL, NULL, &D3DXVECTOR3( g_engine->GetDisplayMode()->Width / 2.0f - 15.0f, g_engine->GetDisplayMode()->Height / 2.0f - 15.0f, 0.0f ), 0xFFFFFFFF );
	g_engine->GetSprite()->End();
}

//-----------------------------------------------------------------------------
// Handles the game specific network messages. Called by the network object.
//-----------------------------------------------------------------------------
void Game::HandleNetworkMessage( ReceivedMessage *msg )
{
	// Process the received messaged based on its type.
	switch( msg->msgid )
	{
		case MSGID_CREATE_PLAYER:
		{
			break;
		}

		case MSGID_DESTROY_PLAYER:
		{
			break;
		}

		case MSGID_TERMINATE_SESSION:
		{
			// Switch to the menu state.
			g_engine->ChangeState( STATE_MENU );

			break;
		}
	}
}