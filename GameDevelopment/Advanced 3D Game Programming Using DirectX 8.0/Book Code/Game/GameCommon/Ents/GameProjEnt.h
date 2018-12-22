/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: GameProjEnt.h
 *    Desc: Represents a projectile in the world
 *
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _GAMEPROJENT_H
#define _GAMEPROJENT_H

#include "GameEnt.h"
#include "..\ResourceMgr.h"

objID GetNextProjEntID();

class cGameProjEnt : public cGameEnt
{
	// constructor is private to force usage of CreateProjEnt
	cGameProjEnt();

	objID m_launcher; // We shouldn't kill the guy that launched us.
public:

	//==========--------------------------   cGameEnt routines

	virtual void Tick( float tickLength );

	virtual bool RespondToCollision( cGameEnt* pOther );

	/**
	 * We extend the base set of message handling abilities
	 */
	virtual uint ProcMsg( const sMsg& msg );

	virtual bool HitsObjects(){ return true; }

	/**
	 * Create a projectile entity.  All creation goes through
	 * one of these static ctors.  The server should
	 * use the server ctor, client client.
	 */
	static cGameProjEnt* CreateOnServer( objID parent, float yaw, float pitch );
	static cGameProjEnt* CreateOnClient( objID id, sEntState& state );

};

#endif //_GAMEPROJENT_H