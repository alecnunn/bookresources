/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: GamePlayerEnt.h
 *    Desc: this class represents a human-Ctrl player
 *          all players have the same model
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _GAMEPLAYERENT_H
#define _GAMEPLAYERENT_H

#include "..\..\NetLib\MTUDP.h"

#include "GameEnt.h"
#include "..\ResourceMgr.h"

#include <string>
using std::string;

objID GetNextPlayerEntID();

class cGamePlayerEnt : public cGameEnt
{
protected:

	string		m_name;
	HOSTHANDLE  m_host;
	int			m_score;

	cGamePlayerEnt();

public:

	virtual ~cGamePlayerEnt();

	//==========--------------------------   cGameEnt routines

	virtual void Tick( float tickLength );

	virtual bool RespondToCollision( cGameEnt* pOther );

	/**
	 * We extend the base set of message handling abilities
	 */
	virtual uint ProcMsg( const sMsg& msg );

	virtual bool HitsObjects(){ return true; }

	/**
	 * Create a player entity.  All creation goes through
	 * one of these static ctors.  The server should
	 * use the server ctor, client client.
	 *
	 * Both of these functions create an unspawned player in
	 * the world.  CreateOnServer sends notification messages
	 */
	static cGamePlayerEnt* CreateOnServer( HOSTHANDLE host, string name );
	static cGamePlayerEnt* CreateOnClient( objID id, string name );

	HOSTHANDLE  GetHost()                       { return m_host; }
	void        SetHost( HOSTHANDLE host )      { m_host = host; }
	string&		GetName()                       { return m_name; }
	void        SetName( const string& name )   { m_name = name; }

	int			GetScore()						{ return m_score; }
	void		SetScore( int score );

};



#endif //_GAMEPLAYERENT_H