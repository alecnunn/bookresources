/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: GameWorld.h
 *    Desc: Code that describes and implements the world that the
 *          players move around in.
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef	_GAMEWORLD_H
#define _GAMEWORLD_H

#include "ResourceMgr.h"
#include "MsgDaemon.h"
#include "..\gamelib\file.h"

#include <list>
#include <vector>
#include <algorithm>
using namespace std;

class cGameCell;

class cGameEnt;
class cGamePlayerEnt;
enum eObjectType;

class cGameWorld 
: public iGameObject
{
protected:

	vector< objID > m_spawnEnts;

public:
	int m_nCells;
	cGameCell** m_pCellList;

	/**
	 * list of the players in the game
	 */
	list< cGamePlayerEnt* > m_players; 
	
	/**
	 * Used in the iteration calls
	 */
	typedef list< cGameEnt* > tEntList;
	tEntList::iterator m_entIter;
	tEntList m_entList;

public:

//==========--------------------------  cGameWorld

	cGameWorld( cFile& file );
	virtual ~cGameWorld();

	objID GetGoodSpawnEnt();

	void TraverseAll( vector< cGameCell* >* pList );

	objID FindContainingCell( const point3& loc );

	/**
	 * Called after the physics is run, if the object
	 * ran into anything, it resolves it.
	 */
	void ValidateObjectMove( objID id );

	cGameCell* GetCell( int cellNum );

	void Tick( float tickLength );

	//==========--------------------------  Player functions

	void AddPlayer( objID id ); // adds a **constructed** player
	void RemovePlayer( objID id );

	// Stuff the list of players into a container
	template< class container >
	void ListPlayers( container* pList )
	{
		list< cGamePlayerEnt* >::iterator iter;
		for( iter = m_players.begin(); iter != m_players.end(); iter++ )
		{
			pList->push_back( *iter );
		}
	}

	// Checks for duplicate names
	bool ValidName( char* name )
	{ 
		// FIXME: not validating names.
		return true;
	}

	//==========--------------------------  iGameObject
	virtual objID GetID();
	virtual void SetID( objID id );
	virtual uint ProcMsg( const sMsg& msg );

	//==========--------------------------  Iteration functions
	void		EntIterStart(); // Entity iteration (all objects in the world)
	bool		EntIterDone();
	void		EntIterNext();
	cGameEnt*	EntIterCurr();

	// Called when an object creates itself.
	void		NotifyAdd( cGameEnt* pEnt );
	// Called when an object destroys itself.
	void		NotifyRemove( cGameEnt* pEnt );

	// Run object-object collision tests
	void		RunCollisionTests();

};

#endif //_GAMEWORLD_H
