/****************************************************************
 * Entity Class implementation
 ***************************************************************/

#include "Globals.h"
#include "Entities.h"
#include "Troll.h"
#include <stdlib.h>
#include <iostream.h>


/***************************************************************
 * ENTITY Implementation
 ***************************************************************/

Troll * Entity::ptrTroll = 0;
WorldGrid * Entity::ptrGrid = 0;


// The grid wraps around; if movement goes overboard, the entity
// magically warps to the other end of the world
void Entity::MoveEntity( int dx, int dy )
{
	int newX = GetX() + dx;
	if ( newX < 0 )
		newX += WORLD_GRID_SIZE;
	if ( newX >= WORLD_GRID_SIZE )
		newX -= WORLD_GRID_SIZE;
	int newY = GetY() + dy;
	if ( newY < 0 )
		newY += WORLD_GRID_SIZE;
	if ( newY >= WORLD_GRID_SIZE )
		newY -= WORLD_GRID_SIZE;

	if ( ExportedClass() == ENTITY_TROLL )
	{
		ptrGrid->MoveTroll( newX, newY );
	}
	else
	{
		ptrGrid->MoveEntity( GetWorldID(), newX, newY );
	}

	SetX( newX );
	SetY( newY );
}


/******************************************************************
 * SHEEP Implementation
 *****************************************************************/

bool Sheep::Update()
{
	if( IsDead )
		return true;

	// If the troll is in the sheep's square, the sheep is meat
	if( ptrGrid->DistanceFromTroll( GetWorldID() ) == 0 )
	{
		IsDead = true;
		ptrGrid->UnRegister( GetWorldID() );
	}

	// Two cases: either there is a troll nearby, in which
	// case the sheep runs away, or there isn't, in which
	// case the sheep wanders aimlessly
	if( ptrGrid->IsTrollWithinRange( GetWorldID() ) )
	{
		int dx, dy;
		if ( ptrGrid->GetTrollX() < GetX() )
			dx = 1;
		else
			dx = -1;
		if ( ptrGrid->GetTrollY() < GetY() )
			dy = 1;
		else
			dy = -1;
		MoveEntity( dx, dy );
	}
	else
	{
		int dx = rand() % 3 - 1;
		int dy = rand() % 3 - 1;
		MoveEntity( dx, dy );
	}
		
	return true;
}


/***************************************************************
 * KNIGHT Implementation
 ***************************************************************/

bool Knight::Update()
{
	// Dead knights don't do much of anything
	if ( IsDead )
		return true;

	// Should the knight go berserk?
	if ( !IsBerserk && ptrGrid->IsTrollWithinRange( GetWorldID() ) )
	{
		IsBerserk = true;
	}

	// If the troll is in a safe haven, the knight automatically
	// loses track
	if ( ptrGrid->HowManyCloseToTroll( ENTITY_HAVEN, 0 ) > 0 )
	{
		IsBerserk = false;
	}

	// If the knight is berserk, he pursues the troll; otherwise,
	// he wanders about aimlessly
	if ( IsBerserk )
	{
		int dx, dy;
		if ( ptrGrid->GetTrollX() < GetX() )
			dx = -1;
		else if ( ptrGrid->GetTrollX() > GetX() )
			dx = 1;
		else
			dx = 0;
		if ( ptrGrid->GetTrollY() < GetY() )
			dy = -1;
		else if ( ptrGrid->GetTrollY() > GetY() )
			dy = 1;
		else
			dy = 0;
		MoveEntity( dx, dy );
	}
	else
	{
		int dx = rand() % 3 - 1;
		int dy = rand() % 3 - 1;
		MoveEntity( dx, dy );
	}

	if ( ptrGrid->GetTrollX() == GetX() && ptrGrid->GetTrollY() == GetY() )
	{
		IsDead = ptrTroll->SendFightMessage();
		if( IsDead )
			ptrGrid->UnRegister( GetWorldID() );
	}

	return true;
}


/***************************************************************
 * TOWER Implementation
 **************************************************************/

bool Tower::Update()
{
	// If there is a troll within firing range, shoot at it!
	if ( ptrGrid->IsTrollWithinRange( GetWorldID() ) )
	{
		ptrTroll->SendDamageMessage( rand() % 3 );
	}

	return true;
}



/***************************************************************
 * TRAP Implementation
 ***************************************************************/

bool Trap::Update()
{
	// If a troll walks into the trap, capture it!
	if ( ptrGrid->IsTrollWithinRange( GetWorldID() ) )
	{
		Unveiled = true;
		ptrTroll->SendCaptureMessage();
		ptrGrid->Register( GetWorldID(), ENTITY_TRAP, GetX(), GetY() );
	}

	// Trolls are dumb, and may forget that a "haven" is actually a 
	// trap once they're gone
	if( Unveiled && ptrGrid->DistanceFromTroll( GetWorldID() ) > 0 && ( rand() % 10 == 0 ) )
	{
		Unveiled = false;
		ptrGrid->Register( GetWorldID(), ENTITY_HAVEN, GetX(), GetY() );
	}

	return true;
}