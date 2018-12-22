/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: EntNM.cpp
 *    Desc: Entity-oriented NetMessages
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#include "stdafx.h"

#include "EntNM.h"

#include "..\MsgDaemon.h"

cNM_EntAdd::cNM_EntAdd( 
	int entID, 
	sEntState state )
: m_entID( entID )
, m_state( state )
{
}

int cNM_EntAdd::SerializeTo( uchar* pOutput )
{
	int dataSize = sizeof( sEntState ) + 4; // ent state, 1 int.
	*pOutput++ = 3;
	memcpy( pOutput, &m_entID, dataSize );
	return 1 + dataSize;
}	

void cNM_EntAdd::SerializeFrom( uchar *pFromData, int datasize )
{
	int dataSize = sizeof( sEntState ) + 4; // ent state, 1 ints.
	memcpy( &m_entID, pFromData + 1, dataSize );
}

void cNM_EntAdd::Exec()
{
	LogPrint( "[cNM_EntAdd::Exec] id is (%d:%d)",
		GetIDSegment(m_entID), GetIDOffset(m_entID) );
	DebPrint( "[cNM_EntAdd::Exec] id is (%d:%d)",
		GetIDSegment(m_entID), GetIDOffset(m_entID) );

	/**
	 * Use the segment constants to figure out what kind
	 * of object we got told to add.
	 */
	switch( GetIDSegment( m_entID ) )
	{
	case c_playerSegment:
		LogPrint( "[ERROR] shouldn't get entadd's for players!" );
		break;
	case c_spawnSegment:
		LogPrint( "  The hell do I do with spawn ents?" );
		break;
	case c_projSegment:
		{
			// Create a projection entity on the client
			cGameProjEnt* pEnt = cGameProjEnt::CreateOnClient(
				m_entID, m_state );
			LogPrint("  !! Added a projectile ent to the world");
			DebPrint("  !! Added a projectile ent to the world");
		}
		break;
	case c_paraSegment:
		LogPrint( "  The hell do I do with para ents?" );
		break;
	default:
		// some other weird type
		LogPrint( "[ERROR] Tried to create unknown object segment: %d", 
			GetIDSegment( m_entID ) );
	}
}

//==========--------------------------  

/**
 * FIXME: This message is redundant.  remove it.
 */


cNM_PlayerAdd::cNM_PlayerAdd( int playerID )
: m_playerID( playerID )
{
	cGamePlayerEnt* pPlayer = (cGamePlayerEnt*)MsgDaemon()->Get( playerID );

	if( !pPlayer )
	{
		LogPrint("PlayerAdd: bad ID");
		return;
	}
	
	m_state = pPlayer->GetState();
	assert( pPlayer->GetName().length() <= 31 );
	strcpy( m_name, pPlayer->GetName().c_str() );
}

int cNM_PlayerAdd::SerializeTo( uchar* pOutput )
{
	int dataSize = 4 + sizeof( sEntState ) + 32; // int, entstate, and name
	*pOutput++ = 12;
	memcpy( pOutput, &m_playerID, dataSize );
	return 1 + dataSize;
}

void cNM_PlayerAdd::SerializeFrom( uchar *pFromData, int datasize )
{
	int dataSize = 4 + sizeof( sEntState ) + 32; // int, entstate, and name
	memcpy( &m_playerID, pFromData + 1, dataSize );
}

void cNM_PlayerAdd::Exec()
{
	DebPrint("Player %s has joined the game!", m_name);
	LogPrint("Adding a player with name %s", m_name);

	cGamePlayerEnt* pPlayer = 
		cGamePlayerEnt::CreateOnClient( m_playerID, string( m_name ) );

	pPlayer->SetState( m_state );
	LogPrint("Finished adding player\n");
}


//==========--------------------------  

cNM_EntStateChange::cNM_EntStateChange( 
	int entID, 
	sEntState state )
: m_entID( entID )
, m_state( state )
{

}

int cNM_EntStateChange::SerializeTo( uchar* pOutput )
{
	int dataSize = sizeof( sEntState ) + 4; // ent state, one int.
	*pOutput++ = 4;
	memcpy( pOutput, &m_entID, dataSize );
	return 1 + dataSize;
}

void cNM_EntStateChange::SerializeFrom( uchar *pFromData, int datasize )
{
	int dataSize = sizeof( sEntState ) + 4; // ent state, one int.
	memcpy( &m_entID, pFromData + 1, dataSize );
}

void cNM_EntStateChange::Exec()
{
	// apply the state to the entity.
	cGameEnt* pEnt = (cGameEnt*)MsgDaemon()->Get( m_entID );
	//DebPrint("StateChange for %d", m_entID );

	if( NULL == pEnt )
	{
		LogPrint("[cNM_EntStateChange]: ERROR! Object not found: (%d:%d)",
			GetIDSegment( m_entID ), GetIDOffset( m_entID ) );
		return;
	}

	pEnt->SetState( m_state );
}


//==========--------------------------  

cNM_SpawnReq::cNM_SpawnReq( int entID )
: m_entID( entID )
{

}

int cNM_SpawnReq::SerializeTo( uchar* pOutput )
{
	// First byte is the message ID (5)
	*pOutput++ = 5;

	*((int*)pOutput) = m_entID;

	return 1 + sizeof(int);
}

void cNM_SpawnReq::SerializeFrom( uchar *pFromData, int datasize )
{
	m_entID = *(int*)(pFromData + 1);
}

void cNM_SpawnReq::Exec()
{
	LogPrint("Received a Spawn Request");
	cGamePlayerEnt* pPlayer = (cGamePlayerEnt*)MsgDaemon()->Get( m_entID );

	if( NULL == pPlayer )
	{
		LogPrint("[cNM_SpawnReq]: ERROR! Player not found: (%d:%d)",
			GetIDSegment( m_entID ), GetIDOffset( m_entID ) );
		return;
	}

	if( pPlayer->GetStatus() != gesDead &&
		pPlayer->GetStatus() != gesUnSpawned )
	{
		// Shouldn't spawn (player is alive)
		return;
	}

	// otherwise, spawn away...
	cGameWorld* pWorld = (cGameWorld*)MsgDaemon()->Get( g_gameWorldID );

	objID spawnID = pWorld->GetGoodSpawnEnt();

	cGameSpawnEnt* pSpawn = (cGameSpawnEnt*)MsgDaemon()->Get( spawnID );

	sEntState state = pPlayer->GetState();

	state.m_health = 100;
	state.m_loc = pSpawn->GetLoc();
	state.m_status = gesAlive;

	pPlayer->SetState( state );

	DebPrint("spawning player %d....", m_entID );

	cNM_EntStateChange msg( m_entID, state );
	ReliableBroadcast( msg );
}

//==========--------------------------  

cNM_EntStateChangeRequest::cNM_EntStateChangeRequest( 
	int entID, 
	sEntState state )
: m_entID( entID )
, m_state( state )
{

}

int cNM_EntStateChangeRequest::SerializeTo( uchar* pOutput )
{
	int dataSize = sizeof( sEntState ) + 4; // ID + ent state
	*pOutput++ = 6;
	memcpy( pOutput, &m_entID, dataSize );
	return 1 + dataSize;
}

void cNM_EntStateChangeRequest::SerializeFrom( uchar *pFromData, int datasize )
{
	int dataSize = sizeof( sEntState ) + 4; // ID + ent state
	memcpy( &m_entID, pFromData+1, dataSize );
}

void cNM_EntStateChangeRequest::Exec()
{
	// Apply the state to the server-side object.
	cGameEnt* pEnt = (cGameEnt*)MsgDaemon()->Get( m_entID );

	if( NULL == pEnt )
	{
		LogPrint("[cNM_EntStateChangeRequest]: ERROR! Object not found: (%d:%d)",
			GetIDSegment( m_entID ), GetIDOffset( m_entID ) );
		return;
	}
	
	pEnt->SetState( m_state );

	// Right now, just re-broadcast it out.
	cNM_EntStateChange msg( m_entID, m_state );
	UnreliableBroadcast( msg );
}


//==========--------------------------  

cNM_MiniStateChange::cNM_MiniStateChange( 
	int entID,
	sEntState& state )
: m_entID( entID )
, m_loc( state.m_loc )
, m_vel( state.m_vel )
, m_yaw( state.m_yaw )
, m_pitch( state.m_pitch )
{
	sMsg msg;
	msg.m_dest = g_gameID;
	msg.m_type = msgGetServerTime;
	m_time = SendMessage( msg );
}

int cNM_MiniStateChange::SerializeTo( uchar* pOutput )
{
	*pOutput++ = 9;
	int dataSize = 4 + 4 + 12 + 12 + 4 + 4;
	memcpy( pOutput, (void*)&m_entID, dataSize );
	return dataSize + 1;
}

void cNM_MiniStateChange::SerializeFrom( uchar *pFromData, int datasize )
{
	int dataSize = 4 + 4 + 12 + 12 + 4 + 4;
	memcpy( (void*)&m_entID, pFromData+1, dataSize );
}

void cNM_MiniStateChange::Exec()
{
	// apply the state to the entity.
	cGameEnt* pEnt = (cGameEnt*)MsgDaemon()->Get( m_entID );

	if( NULL == pEnt )
	{
		LogPrint("[cNM_MiniStateChange]: ERROR! Object not found: (%d:%d)",
			GetIDSegment( m_entID ), GetIDOffset( m_entID ) );
		return;
	}

	if( !pEnt )
	{
		LogPrint( "[cNM_MiniStateChange::Exec] got a bad ID: %d", m_entID );
		return;
	}

	if( IsClient() )
	{
		// only apply yaw and pitch if it's not the player object.
		if( GetClientID() != m_entID )
		{
			pEnt->SetYaw( m_yaw );
			pEnt->SetPitch( m_pitch );
		}
	}
	pEnt->SetLoc( m_loc );
	pEnt->SetVel( m_vel );
}


//==========--------------------------  

cNM_MiniStateChangeRequest::cNM_MiniStateChangeRequest( 
	int entID,
	sEntState& state )
: m_entID( entID )
, m_loc( state.m_loc )
, m_vel( state.m_vel )
, m_yaw( state.m_yaw )
, m_pitch( state.m_pitch )
{
	sMsg msg;
	msg.m_dest = g_gameID;
	msg.m_type = msgGetServerTime;
	m_time = SendMessage( msg );
}

int cNM_MiniStateChangeRequest::SerializeTo( uchar* pOutput )
{
	*pOutput++ = 10;
	int dataSize = 4 + 4 + 12 + 12 + 4 + 4;
	memcpy( pOutput, (void*)&m_entID, dataSize );
	return dataSize + 1;
}

void cNM_MiniStateChangeRequest::SerializeFrom( uchar *pFromData, int datasize )
{
	int dataSize = 4 + 4 + 12 + 12 + 4 + 4;
	memcpy( (void*)&m_entID, pFromData+1, dataSize );
}

void cNM_MiniStateChangeRequest::Exec()
{
	/**
	 * Apply the state change
	 */

	// FIXME: should scale for time here.
	cGameEnt* pEnt = (cGameEnt*)MsgDaemon()->Get( m_entID );

	if( NULL == pEnt )
	{
		LogPrint("[cNM_MiniStateChangeRequest]: ERROR! Object not found: (%d:%d)",
			GetIDSegment( m_entID ), GetIDOffset( m_entID ) );
		return;
	}

	pEnt->SetLoc( m_loc );
	pEnt->SetYaw( m_yaw );
	pEnt->SetPitch( m_pitch );
	pEnt->SetVel( m_vel );
}

//==========--------------------------  

cNM_ProjFireRequest::cNM_ProjFireRequest( objID id )
{
	cGameEnt* pEnt = (cGameEnt*)MsgDaemon()->Get( id );

	if( !pEnt )
	{
		LogPrint("FireRequest: bad ID");
		return;
	}

	m_yaw = pEnt->GetYaw();
	m_pitch = pEnt->GetPitch();
	m_entID = id;
}

int cNM_ProjFireRequest::SerializeTo( uchar* pOutput )
{
	*pOutput++ = 11;
	int dataSize = 4 + 4 + 4; // 1 int, 2 floats
	memcpy( pOutput, &m_entID, dataSize );

	return 1 + dataSize;
}

void cNM_ProjFireRequest::SerializeFrom( uchar *pFromData, int datasize )
{
	int dataSize = 4 + 4 + 4; // 1 int, 2 floats
	memcpy( &m_entID, pFromData+1, dataSize );
}

void cNM_ProjFireRequest::Exec()
{
	DebPrint( "Server received a request to fire a projectile" );
	LogPrint( "obj %d wants to fire.", m_entID );
	cGameEnt* pSrc = (cGameEnt*)MsgDaemon()->Get( m_entID );

	if( NULL == pSrc )
	{
		LogPrint("[cNM_ProjFireRequest]: ERROR! Object not found (%d:%d)",
			GetIDSegment( m_entID ), GetIDOffset( m_entID ) );
		return;
	}

	/**
	 * Create a projectile entity
	 */
	cGameProjEnt* pProj = cGameProjEnt::CreateOnServer(
		m_entID, m_yaw, m_pitch );

	point3 loc = pSrc->GetLoc();
	LogPrint( "launcher is at %f %f %f",
		loc.x, loc.y, loc.z );

	/**
	 * Broadcast the new entity
	 */
	cNM_EntAdd msg( 
		pProj->GetID(), 
		pProj->GetState() );
	ReliableBroadcast( msg );
	
	LogPrint( "Sucessfully added rocket to world" );
}


