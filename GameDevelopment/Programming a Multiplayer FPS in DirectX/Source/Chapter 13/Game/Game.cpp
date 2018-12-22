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

//-----------------------------------------------------------------------------
// Game class constructor.
//-----------------------------------------------------------------------------
Game::Game() : State( STATE_GAME )
{
	// Store a pointer to the game state in a global variable for easy access.
	g_game = this;

	// Invalidate the score board font.
	m_scoreBoardFont = NULL;

	// Invalidate both the bullet manager and the player manager.
	m_bulletManager = NULL;
	m_playerManager = NULL;

	// Invalidate the in-game music sound.
	m_music = NULL;
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

	// Create both the bullet manager and the player manager.
	m_bulletManager = new BulletManager;
	m_playerManager = new PlayerManager;

	// Load and play the in-game music.
	m_music = new Sound( "./Assets/Sounds/music_loop.wav" );
	m_music->Play( true );
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

	// Destroy both the bullet manager and the player manager.
	SAFE_DELETE( m_bulletManager );
	SAFE_DELETE( m_playerManager );

	// Destroy the score board font.
	SAFE_DELETE( m_scoreBoardFont );

	// Destroy the crosshair material.
	g_engine->GetMaterialManager()->Remove( &m_crosshair );

	// Destroy the in-game music.
	SAFE_DELETE( m_music );
}

//-----------------------------------------------------------------------------
// Returns the view setup details for the given frame.
//-----------------------------------------------------------------------------
void Game::RequestViewer( ViewerSetup *viewer )
{
	viewer->viewer = m_playerManager->GetViewingPlayer();
	viewer->viewClearFlags = D3DCLEAR_ZBUFFER;
}

//-----------------------------------------------------------------------------
// Update the game state.
//-----------------------------------------------------------------------------
void Game::Update( float elapsed )
{
	// Allow the player to respawn by pressing the R key.
	if( g_engine->GetInput()->GetKeyPress( DIK_R ) == true )
	{
		m_playerManager->GetLocalPlayer()->SetEnabled( false );
		m_playerManager->SpawnLocalPlayer();
	}

	// Update the bullet manager before the player manager. This will prevent
	// bullets that are fired this frame from being processed this frame.
	m_bulletManager->Update( elapsed );

	// Update the player manager.
	m_playerManager->Update( elapsed );

	// Check if the user is holding down the tab key.
	if( g_engine->GetInput()->GetKeyPress( DIK_TAB, true ) == true )
	{
		// Build the score board text.
		sprintf( m_scoreBoardNames, "PLAYER\n" );
		sprintf( m_scoreBoardFrags, "FRAGS\n" );
		sprintf( m_scoreBoardDeaths, "DEATHS\n" );

		// Add each player's details to the score board.
		PlayerObject *player = m_playerManager->GetNextPlayer( true );
		while( player != NULL )
		{
			strcat( m_scoreBoardNames, player->GetName() );
			strcat( m_scoreBoardNames, "\n" );

			sprintf( m_scoreBoardFrags, "%s%d", m_scoreBoardFrags, player->GetFrags() );
			strcat( m_scoreBoardFrags, "\n" );

			sprintf( m_scoreBoardDeaths, "%s%d", m_scoreBoardDeaths, player->GetDeaths() );
			strcat( m_scoreBoardDeaths, "\n" );

			player = m_playerManager->GetNextPlayer();
		}
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
// Returns a pointer to the bullet manager.
//-----------------------------------------------------------------------------
BulletManager *Game::GetBulletManager()
{
	return m_bulletManager;
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
			// Add the new player to the player manager and the scene.
			PlayerObject *object = m_playerManager->AddPlayer( g_engine->GetNetwork()->GetPlayer( msg->dpnid ) );
			g_engine->GetSceneManager()->AddObject( object );

			// Check if the new player is the host player.
			if( object->GetID() == g_engine->GetNetwork()->GetHostID() )
			{
				// Load the scene from the host player's selection.
				g_engine->GetSceneManager()->LoadScene( ( (PlayerData*)g_engine->GetNetwork()->GetPlayer( msg->dpnid )->data )->map, "./Assets/Scenes/" );

				// Allow the network to receive game specific messages.
				g_engine->GetNetwork()->SetReceiveAllowed( true );
			}

			break;
		}

		case MSGID_DESTROY_PLAYER:
		{
			// Remove the player from the player manager and the scene.
			SceneObject *object = m_playerManager->GetPlayer( msg->dpnid );
			g_engine->GetSceneManager()->RemoveObject( &object );
			m_playerManager->RemovePlayer( msg->dpnid );

			break;
		}

		case MSGID_PLAYER_HEALTH:
		{
			// Get a pointer to the game specific network message.
			PlayerHealthMsg *phm = (PlayerHealthMsg*)msg;

			// Set the player's health.
			m_playerManager->GetPlayer( phm->dpnid )->SetHealth( phm->health );

			// Check if the player has been killed.
			if( phm->health <= 0.0f )
				m_playerManager->GetPlayer( phm->dpnid )->Kill();

			break;
		}

		case MSGID_PLAYER_MOVE_UPDATE:
		{
			// Get a pointer to the game specific network message.
			PlayerMoveUpdateMsg *pmum = (PlayerMoveUpdateMsg*)msg;

			// Get a pointer to the player in question.
			PlayerObject *player = m_playerManager->GetPlayer( pmum->dpnid );

			// If the player is disabled then chances are the local player has
			// just joined the game, and is receiving messages about a player
			// that it does not realise has already spawned. So just enable
			// that player and start updating it.
			if( player->GetEnabled() == false )
			{
				player->SetEnabled( true );
				player->SetVisible( true );
			}

			// Update all the player's details.
			player->SetTranslation( pmum->translation.x, pmum->translation.y, pmum->translation.z );
			player->SetDrive( pmum->drive );
			player->SetStrafe( pmum->strafe );
			player->SetFire( pmum->fire );

			break;
		}

		case MSGID_PLAYER_LOOK_UPDATE:
		{
			// Get a pointer to the game specific network message.
			PlayerLookUpdateMsg *plum = (PlayerLookUpdateMsg*)msg;

			// Update the player's rotation and view tilt.
			m_playerManager->GetPlayer( plum->dpnid )->SetRotation( 0.0f, plum->rotationY, 0.0f );
			m_playerManager->GetPlayer( plum->dpnid )->SetViewTilt( plum->viewTilt );

			break;
		}

		case MSGID_PLAYER_SCORE:
		{
			// Get a pointer to the game specific network message.
			PlayerScoreMsg *psm = (PlayerScoreMsg*)msg;

			// Update the player's score.
			m_playerManager->GetPlayer( psm->dpnid )->SetFrags( psm->frags );
			m_playerManager->GetPlayer( psm->dpnid )->SetDeaths( psm->deaths );

			break;
		}

		case MSGID_PLAYER_WEAPON_CHANGE:
		{
			// Get a pointer to the game specific network message.
			PlayerWeaponChangeMsg *pwcm = (PlayerWeaponChangeMsg*)msg;

			// Change the player's weapon.
			m_playerManager->GetPlayer( pwcm->dpnid )->WeaponChanged( pwcm->weapon );

			break;
		}

		case MSGID_PLAYER_WEAPON_CHANGING:
		{
			// Get a pointer to the game specific network message.
			PlayerWeaponChangeMsg *pwcm = (PlayerWeaponChangeMsg*)msg;

			// Indicate that this player is changing weapons.
			m_playerManager->GetPlayer( pwcm->dpnid )->WeaponChanging();

			break;
		}

		case MSGID_SPAWN_POINT_REQUEST:
		{
			// Send a spawn point message back to the requesting player.
			SpawnPointMsg spm;
			spm.msgid = MSGID_SPAWN_POINT;
			spm.dpnid = msg->dpnid;
			spm.spawnPoint = g_engine->GetSceneManager()->GetSpawnPointID( g_engine->GetSceneManager()->GetRandomPlayerSpawnPoint() );
			g_engine->GetNetwork()->Send( &spm, sizeof( SpawnPointMsg ), msg->dpnid );

			break;
		}

		case MSGID_SPAWN_POINT:
		{
			// Get a pointer to the game specific network message.
			SpawnPointMsg *spm = (SpawnPointMsg*)msg;

			// Ensure this message is for the local player.
			if( spm->dpnid != g_engine->GetNetwork()->GetLocalID() )
				break;

			// Spawn the local player using this spawn point.
			m_playerManager->SpawnLocalPlayer( spm->spawnPoint );

			break;
		}

		case MSGID_SPAWN_PLAYER:
		{
			// Get a pointer to the game specific network message.
			SpawnPlayerMsg *spm = (SpawnPlayerMsg*)msg;

			// Spawn the player.
			m_playerManager->SpawnPlayer( spm->dpnid, spm->translation );

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