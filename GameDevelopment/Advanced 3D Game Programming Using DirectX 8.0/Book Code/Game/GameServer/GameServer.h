/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: GameServer.h
 *    Desc: This header holds the server object.  It's registered
 *          like any other iGameObject.
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _GAMESERVER_H
#define _GAMESERVER_H

class cGameServer 
: public iGameObject
{
public:
	void Tick();
	void Initialize();
	void Shutdown();
	cGameServer();
	virtual ~cGameServer();

	//==========--------------------------	iGameObject
	virtual objID GetID(){ return g_gameServer; }
	virtual void SetID( objID id ) {}
	virtual uint ProcMsg( const sMsg& msg );

	void ReliableSendToAllPlayers( cNetMessage& msg );
	void ReliableSendToAllPlayersExcept( cNetMessage& msg, objID exception );
	void UnreliableSendToAllPlayers( cNetMessage& msg );
	void ReliableSendToPlayer( cNetMessage& msg, objID player );
	void UnreliableSendToPlayer( cNetMessage& msg, objID player );

};

#endif // _GAMESERVER_H
