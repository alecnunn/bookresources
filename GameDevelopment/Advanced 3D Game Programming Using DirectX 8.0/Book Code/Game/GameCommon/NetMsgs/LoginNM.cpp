/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: LoginNM.cpp
 *    Desc: Login-oriented net messages.
 *          Grok NetMessages.h before you explore here.
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#include "stdafx.h"
#include "NetMessages.h"
#include "LoginNM.h"
#include "EntNM.h"

// Just to save some bandwidth... When the login gets accepted
// This string holds the name we requested.
string g_desiredName;

cNM_LoginRequest::cNM_LoginRequest( 
	string clientName )
{
	assert( clientName.size() < 32 );
	strcpy( m_name, clientName.c_str() );
	g_desiredName = clientName;
	DebPrint( "Ready to try to login!\n" );
}

int cNM_LoginRequest::SerializeTo( uchar* pOutput )
{
	// First byte is the message ID (1)
	*pOutput++ = 1;

	// Next is the client name
	strcpy( (char*)pOutput, m_name );

	return 1 + 32;
}

void cNM_LoginRequest::SerializeFrom( 
	uchar *pFromData, 
	int datasize )
{
	strcpy( m_name, (const char*)(pFromData + 1) );
}

void cNM_LoginRequest::Exec()
{
	/**
	 * Create the server-side player
	 */
	cGamePlayerEnt* pEnt = 
		cGamePlayerEnt::CreateOnServer( m_from, string(m_name) );
	int playerID = pEnt->GetID();

	/**
	 * Send a message back to the player.
	 */
	DebPrint( "Sending Login Acceptance to player [%s]", m_name );
	ReliableSendToPlayer( 
		cNM_LoginAccepted(playerID), playerID
		);

	/**
	 * Now tell our new player about all the objects.
	 */
	GetWorld()->EntIterStart();
	while( !GetWorld()->EntIterDone() )
	{
		cGameEnt* pCurr = GetWorld()->EntIterCurr();
		GetWorld()->EntIterNext();

		objID currID = pCurr->GetID();

		ushort seg = GetIDSegment( currID );

		// skip spawn objects
		if( c_spawnSegment == seg )
			continue;

		// skip player objects
		if( c_playerSegment == seg )
			continue;

		cNM_EntAdd msg( currID, pCurr->GetState() );
		ReliableSendToPlayer( msg, playerID );
	}

	/**
	 * Tell it about all of the other players.
	 */
	list< cGamePlayerEnt* > playerList;
	GetWorld()->ListPlayers( &playerList );
	list< cGamePlayerEnt* >::iterator iter;
	for(iter = playerList.begin(); iter != playerList.end(); iter++ )
	{
		cGamePlayerEnt* pCurr = (*iter);

		if( pCurr->GetID() == playerID )
			continue;

		//cNM_PlayerAdd msg( pCurr->GetID() );
		cNM_NewPlayer msg( 
			pCurr->GetID(), 
			pCurr->GetScore(), 
			pCurr->GetName(), 
			pCurr->GetState() );

		ReliableSendToPlayer( msg, playerID );
	}

	/**
	 * Tell the other players about it.
	 */
	{
		//cNM_PlayerAdd msg( playerID );
		cNM_NewPlayer msg( 
			playerID, 
			0, 
			string(m_name),
			pEnt->GetState() );
		ReliableBroadcastExcept( msg, playerID );
	}

	/**
	 * Add it from the edit box
	 */
	SendMessage( sMsg( msgAddPlayerName, g_gameServer, (void*)m_name ));
}

//==========--------------------------  

cNM_LoginAccepted::cNM_LoginAccepted( int playerID )
: m_playerID( playerID )
{
}

int cNM_LoginAccepted::SerializeTo( uchar* pOutput )
{
	// First byte is the message ID (1)
	*pOutput++ = 2;

	*((int*)pOutput) = m_playerID;

	return 1 + sizeof(int);
}

void cNM_LoginAccepted::SerializeFrom( uchar *pFromData, int datasize )
{
	m_playerID = *(int*)(pFromData + 1);
}

void cNM_LoginAccepted::Exec()
{
	DebPrint( "Login Accepted!\n" );

	/**
	 * Create the client-side player
	 */
	cGamePlayerEnt* pEnt = 
		cGamePlayerEnt::CreateOnClient( m_playerID, g_desiredName );

	/**
	 * Notify the game client
	 */
	SendMessage( sMsg( msgPlayerCreated, g_gameID, (int)pEnt->GetID() ));

	/**
	 * Request a spawn
	 */
	cNM_SpawnReq msg( m_playerID );

	SendMessage( sMsg( msgSendToServerUR, g_gameID, &msg ) );

	//cGamePlayerEnt* pEnt = cGamePlayerEnt::CreatePlayerEnt( m_playerID );

}

//==========--------------------------  

cNM_LogoutNotice::cNM_LogoutNotice( int playerID )
: m_playerID( playerID )
{
}

int cNM_LogoutNotice::SerializeTo( uchar* pOutput )
{
	*pOutput++ = 7;
	*((int*)pOutput) = m_playerID;
	return 4 + 1;
}

void cNM_LogoutNotice::SerializeFrom( uchar *pFromData, int datasize )
{
	m_playerID = *((int*)(pFromData+1));
}

void cNM_LogoutNotice::Exec()
{
	DebPrint("Got a logout request");
	LogPrint("Got a logout request");

	cGamePlayerEnt* pEnt = (cGamePlayerEnt*)MsgDaemon()->Get( m_playerID );

	if( !pEnt )
	{
		LogPrint("LogoutNotice: bad ID");
		return;
	}

	/**
	 * Close the socket to the player
	 */
	SendMessage(
		sMsg( msgKillSocket, g_gameServer, (int)pEnt->GetHost() )
		);

	/**
	 * Broadcast a notification
	 */

	/**
	 * Remove it from the edit box
	 */
	SendMessage( 
		sMsg( msgRemovePlayerName, g_gameServer, (void*)pEnt->GetName().c_str() )
		);

	/**
	 * Remove the player
	 */
	pEnt->SetStatus( gesMortis );
}

//==========--------------------------  

cNM_NewPlayer::cNM_NewPlayer( int id, int score, string clientName, sEntState state )
{
	m_entID = id;
	m_score = score;
	strcpy( m_name, clientName.c_str() );
	m_state = state;
}

int cNM_NewPlayer::SerializeTo( uchar* pOutput )
{
	int dataSize = 4 + 4 + 32 + sizeof( sEntState ); // id, score, name.
	*pOutput++ = 8;
	memcpy( pOutput, &m_entID, dataSize );
	return 1 + dataSize;
}

void cNM_NewPlayer::SerializeFrom( uchar *pFromData, int datasize )
{
	int dataSize = 4 + 4 + 32 + sizeof( sEntState ); // id, score, name.
	memcpy( &m_entID, pFromData + 1, dataSize );
}

void cNM_NewPlayer::Exec()
{
	// Create the player.
	DebPrint("Player %s has joined the game!", m_name);
	LogPrint("Adding a player with name %s", m_name);

	cGamePlayerEnt* pPlayer = 
		cGamePlayerEnt::CreateOnClient( m_entID, string( m_name ) );

	pPlayer->SetState( m_state );
	pPlayer->SetScore( m_score );
	LogPrint("Finished adding player\n");
}


