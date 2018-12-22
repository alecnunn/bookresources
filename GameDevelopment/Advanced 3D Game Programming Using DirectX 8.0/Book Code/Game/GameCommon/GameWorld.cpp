/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: GameWorld.cpp
 *    Desc: Code to handle the scene, including the cells and
 *          all of the game entities.
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#include "stdafx.h"
#include "GameWorld.h"
#include "GameCell.h"
#include "Ents\GameEnt.h"
#include "Ents\GamePlayerEnt.h"
#include "Ents\GameSpawnEnt.h"

#include <algorithm>


cGameWorld::cGameWorld( cFile& file )
{
	MsgDaemon()->RegObject( g_gameWorldID, this );

	/**
	 * The first line of the file says how many cells we need to load
	 */
	char buff[1024];
	file.ReadNonCommentedLine( buff, '#' );

	int nCells;
	sscanf( buff, "NUM_CELLS %i", &nCells );

	/**
	 * Read as many cells as we were told to
	 */
	m_nCells = nCells;
	m_pCellList = NULL;
	m_pCellList = new cGameCell* [ m_nCells ];
	int i;
	for( i=0; i<nCells; i++ )
	{
		// defense in case something fails midway and we have to delete the list
		m_pCellList[i] = NULL;
	}

	for( i=0; i<nCells; i++ )
	{
		m_pCellList[i] = new cGameCell( file, i );
	}

	/**
	 * The next line gives us our # of spawn locations
	 * only do this if we're the server.
	 */
	if( !IsClient() )
	{
		int nSpawnLocs;
		file.ReadNonCommentedLine( buff, '#' );
		sscanf( buff, "NUM_SPAWN_LOCS %d", &nSpawnLocs );

		for( i=0; i<nSpawnLocs; i++ )
		{
			point3 spawnLoc;
			file.ReadNonCommentedLine( buff, '#' );
			sscanf( 
				buff, 
				"SPAWN_LOC %f %f %f", 
				&spawnLoc.x,
				&spawnLoc.y,
				&spawnLoc.z );

			LogPrint( "Creating Spawn point at %f %f %f",
				spawnLoc.x, spawnLoc.y, spawnLoc.z );

			sEntState state;
			state.m_status = gesGhost;
			state.m_loc = spawnLoc;

			int spawnID = GetNextSpawnEntID();

			cGameSpawnEnt* pEnt = cGameSpawnEnt::CreateSpawnEnt( 
				spawnID, state );
			m_spawnEnts.push_back( spawnID );
		}
	}

	// Iter is initially the tail of the list
	m_entIter = m_entList.end();
}


cGameWorld::~cGameWorld()
{
	/**
	 * Delete each of the cells in the list
	 */
	if( !m_pCellList )
		return;
	for( int i=0; i<m_nCells; i++ )
	{
		delete m_pCellList[i];
	}
	delete m_pCellList;
}



void cGameWorld::TraverseAll( vector< cGameCell* >* pList )
{
	for( int i=0; i<m_nCells; i++ )
	{
		pList->push_back( m_pCellList[i] );
	}
}



void cGameWorld::Tick( float tickLength )
{
	/**
	 * process each of them.  they'll all take care
	 * of their own physics, take care of notifying
	 * their cell when they die or move elsewhere.
	 * each of them know about the world they live in
	 * and can thusly get info (like objects they should
	 * test against.)
	 */ 
	EntIterStart();
	while( !EntIterDone() )
	{
		EntIterCurr()->Tick( tickLength );
		EntIterNext();
	}
}


objID cGameWorld::FindContainingCell( const point3& loc )
{
	objID parent = -1;
	int i;
	for( i=0; i<m_nCells; i++ )
	{
		if( m_pCellList[i]->ContainsPoint( loc ) )
		{
			parent = m_pCellList[i]->GetID();
			break;
		}
	}
	return parent;
}



cGameCell* cGameWorld::GetCell( int cellNum )
{
	assert( cellNum>=0 && cellNum<m_nCells );
	return m_pCellList[cellNum];
}


void cGameWorld::AddPlayer( objID id )
{
	/**
	 * Note: this adds a player that has already
	 * been created.  the id should be valid.
	 */
	m_players.push_back( (cGamePlayerEnt*)MsgDaemon()->Get( id ) );
}



void cGameWorld::RemovePlayer( objID id )
{
	list< cGamePlayerEnt* >::iterator iter;
	cGamePlayerEnt* pTarget = (cGamePlayerEnt*)MsgDaemon()->Get( id );

	if( !pTarget )
	{
		LogPrint("cGameWorld::RemovePlayer: bad ID");
		return;
	}


	iter = find( m_players.begin(), m_players.end(), pTarget );

	if( iter == m_players.end() )
	{
		DP( "ERROR: Tried to remove non-existent player\n" );
	}
	else
	{
		m_players.erase( iter );
	}

}



void cGameWorld::ValidateObjectMove( objID id )
{
/*	iGameObject* pObj = MsgDaemon()->Get( id );

	// This is the cell before the last physics step
	cGameCell* pOldCell = (cGameCell*)
		MsgDaemon()->Get( 
			pObj->ProcMsg( sMsg( msgGetParent, 0, 0 ) )
		);

	bSphere3 sphere = *(bSphere3*)pObj->ProcMsg( sMsg( msgGetSphere, 0, 0 ) );
	point3 loc = *(point3*)pObj->ProcMsg( sMsg( msgGetLoc, 0, 0 ) );

	cGameCell* pNewCell = NULL;

	bool bStillInOldCell = pOldCell->ContainsPoint( loc );

	if( bStillInOldCell )
	{
		pNewCell = pOldCell;
		// three possible cases:
		//  1: far enough away from the walls, do nothing
		//  2: close to walls and not in a doorway, snap
		//  3: close to walls but in a doorway, do nothing.

		if( pOldCell->ContainsSphere( sphere, true ) )
		{
			// case 1 (do nothing)
		}
		else
		{
			// case 2 or 3
			if( pOldCell->ContainedInPortal( sphere ) )
			{
				// case 3 (do nothing)
			}
			else
			{
				// case 2 (snap)
				pOldCell->SnapInside( (cGameEnt*)pObj );
			}
		}
	}
	else
	{
		// two possible cases:
		//  1: in empty space, snap
		//  2: in a nearby cell, let it deal with it.

		list< cGameCell* > cellList;
		pOldCell->Traverse( &cellList, 2 ); // 2 depth
		cGameCell* pCell;

		// The first cell is pOldCell, don't bother checking it.
		cellList.pop_front();

		while( !pNewCell && !cellList.empty() )
		{
			pCell = *cellList.begin();
			cellList.pop_front();

			if( pCell->ContainsPoint( loc ) )
			{
				pNewCell = pCell;
			}
		}
		
		if( pNewCell )
		{
			// case 2, let the other cell deal with it.
			pObj->m_pCell = pNewCell;
			ValidateObjectMove( id );
			return;
		}
		else
		{
			// case 1, snap it to the old cell
			pNewCell = pOldCell;
			pOldCell->SnapInside( (cGameEnt*)pObj );
		}
	}
*/
	/**
	 * Now we have the cell the object is currently in.
	 * do object collisions.
	 */
/*	sMsg msg;
	msg.m_type = msgSetLoc;
	msg.m_pt = loc;
	pObj->ProcMsg( msg );

	msg.m_type = msgSetParent;
	msg.m_i[0] = pNewCell->GetID();
	pObj->ProcMsg( msg );*/
}


objID cGameWorld::GetID()
{
	return g_gameWorldID;
}



void cGameWorld::SetID( objID id )
{
}



uint cGameWorld::ProcMsg( const sMsg& msg )
{
	switch( msg.m_type )
	{

	case msgFindContainingCell:
		return FindContainingCell( msg.m_pt );

	case msgAddPlayer:
		AddPlayer( msg.m_i[0] );
		return 0;

	case msgRemovePlayer:
		RemovePlayer( msg.m_i[0] );
		return 0;

	case msgAddEnt:
		NotifyAdd(
			(cGameEnt*)MsgDaemon()->Get( msg.m_i[0] ));
		return 0;

	case msgRemoveEnt:
		NotifyRemove(
			(cGameEnt*)MsgDaemon()->Get( msg.m_i[0] ));
		return 0;

	default:
		DP0("Bad Message got to cGameWorld\n");
		return -1;
	}
}




//==========--------------------------  Iteration code

void cGameWorld::EntIterStart()
{
	m_entIter = m_entList.begin();
}

bool cGameWorld::EntIterDone()
{
	return ( m_entIter == m_entList.end() );
}

void cGameWorld::EntIterNext()
{
	m_entIter++;
}

cGameEnt* cGameWorld::EntIterCurr()
{
	return (*m_entIter);
}

void cGameWorld::NotifyAdd( cGameEnt* pEnt )
{
	// Add to the end.
	m_entList.push_back( pEnt );
}

void cGameWorld::NotifyRemove( cGameEnt* pEnt )
{
	LogPrint("[cGameWorld::NotifyRemove] Removing (%d:%d)", 
		GetIDSegment(pEnt->GetID()), GetIDOffset(pEnt->GetID()) );

	tEntList::iterator iter = find(
		m_entList.begin(),
		m_entList.end(),
		pEnt );

	if( iter == m_entList.end() )
	{
		LogPrint( "[cGameWorld::NotifyRemove] object doesn't exist! can't remove." );
	}

	if( iter == m_entIter )
	{
		// erase and adjust the iterator
		m_entIter = m_entList.erase( iter );
	}
	else
	{
		// blindly erase
		m_entList.erase( iter );
	}
}


objID cGameWorld::GetGoodSpawnEnt() 
{
	/**
	 * Step through the list of spawn ents.
	 * choose the one that is farthest from
	 * the rest of the players.
	 */
	list< cGamePlayerEnt* > players;
	ListPlayers( &players );

	DebPrint( "found %d players", players.size() );

	int nSpawnEnts = m_spawnEnts.size();
	int best=0;
	float bestDist = 0.f;

	for( int i=0; i<nSpawnEnts; i++ )
	{
		cGameEnt* pEnt = (cGameEnt*)MsgDaemon()->Get( m_spawnEnts[i] );

		if( !pEnt )
		{
			LogPrint("cGameWorld::GetGoodSpawnEnt: bad ID");
			return -1;
		}


		list< cGamePlayerEnt* >::iterator iter;
		float bestCurrDist = 10000.f;
		for( iter = players.begin(); iter != players.end(); iter++ )
		{
			float currDist = point3::Dist( (*iter)->GetLoc(), pEnt->GetLoc() );
			if( currDist < bestCurrDist )
			{
				bestCurrDist = currDist;
			}
		}
		// If this is the new best one, take it.
		if( bestCurrDist > bestDist )
		{
			best = i;
			bestDist = bestCurrDist;
		}

	}
	// return the best one.
	return m_spawnEnts[ best ];
}


void cGameWorld::RunCollisionTests()
{
	list< cGameEnt* >::iterator iter1, iter2;
	list< cGameEnt* >::iterator temp;
	for( iter1 = m_entList.begin();	iter1 != m_entList.end(); ++iter1 )
	{
		/**
		 * test each object against all of the objects above it.
		 * This is a sucky way to do things ( O(n^2) ).  A better
		 * way would be to test each object only against the list
		 * objects of the nearby cells.  This wouldn't be too hard
		 * to do, but is left as an exercise to the reader.  Anyway,
		 * since n will typically be < 10, we can waste a little time.
		 */

		/**
		 * Early-out if iter1 doesn't hit objects.
		 */
		if( !(*iter1)->HitsObjects() )
		{
			continue;
		}
		
		iter2 = iter1;
		iter2++;
		for( ; iter2 != m_entList.end(); ++iter2 )
		{
			/**
			 * Early-out if iter2 doesn't hit objects.
			 */
			if( !(*iter2)->HitsObjects() )
			{
				continue;
			}

			/**
			 * Do the objects collide?
			 */
			if( bSphere3::Intersect( 
				(bSphere3&)(*iter1)->GetSphere(), 
				(bSphere3&)(*iter2)->GetSphere() ) )
			{
				// handle the collision.
				DebPrint("We had a collision!");
				temp = iter1;
				temp--;
				if( 0 == (*iter1)->RespondToCollision( (*iter2) ) )
				{
					// if we get in here, iter1 destroyed itself as a result
					// of the collision. (a rocket that hit a player, for
					// example).

					// fix iter1 and jump to the next iter1 step.
					iter1 = temp;
					goto iter1Died;
				}

				temp = iter2;
				temp--;
				if( 0 == (*iter2)->RespondToCollision( (*iter1) ) )
				{
					// if we get in here, iter2 destroyed itself as a result
					// of the collision. (a rocket that hit a player, for
					// example).

					// fix iter2 and continue on.
					iter2 = temp;
				}
			}
		}

// We jump to this label when iter1 kills itself in a collision.
iter1Died:

		; // sigh...

	}
}