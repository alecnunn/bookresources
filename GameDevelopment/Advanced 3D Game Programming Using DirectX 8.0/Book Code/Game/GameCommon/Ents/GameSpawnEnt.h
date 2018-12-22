/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: GameSpawnEnt.h
 *    Desc: Represents a spawn location in the world
 *
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _GAMESPAWNENT_H
#define _GAMESPAWNENT_H

#include "GameEnt.h"
#include "..\ResourceMgr.h"

objID GetNextSpawnEntID();

class cGameSpawnEnt : public cGameEnt
{
	// constructor is private to force usage of CreateSpawnEnt
	cGameSpawnEnt();
public:

	//==========--------------------------   cGameEnt routines

	virtual void Tick( float tickLength ){}

	virtual bool RespondToCollision( cGameEnt* pOther ){ return true; }

	/**
	 * We extend the base set of message handling abilities
	 */
	virtual uint ProcMsg( const sMsg& msg );

	virtual bool HitsObjects(){ return false; }

	/**
	 * Create a spawn entity
	 */
	static cGameSpawnEnt* CreateSpawnEnt(
		objID id,
		sEntState state );


};

#endif //_GAMESPAWNENT_H