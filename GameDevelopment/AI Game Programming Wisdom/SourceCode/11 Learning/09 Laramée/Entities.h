/****************************************************************
 * ENTITIES.H
 * Definitions of the classes of objects found in the world,
 * except for the troll himself
 ***************************************************************/

#include "Globals.h"
#include "WorldGrid.h"

#ifndef ENTITIES_H
#define ENTITIES_H

class Troll;

/***************************************************************
 * CLASS Entity
 * A virtual base class for the hierarchy of entities.  Its
 * structure is based on my article in Game Programming Gems 2,
 * which you should have read by now ;-)
 **************************************************************/

class Entity
{
	// Storage common to all classes of entities: Positional data and
	// an ID to register their actions with the WorldGrid
	int PosX, PosY;
	int WorldID;

protected:
	// A static pointer to the troll, shared by all entities which
	// may need to send it messages
	static Troll * ptrTroll;

	// A static pointer to the World Grid
	static WorldGrid * ptrGrid;

public:
	// Construction and access
	Entity( int x, int y, int id ) : PosX( x ), PosY( y ), WorldID( id ) {}
	virtual Entity * Clone() = 0;

	int GetX() { return PosX; }
	int GetY() { return PosY; }
	int GetWorldID() { return WorldID; }
	void SetX( int x ) { PosX = x; }
	void SetY( int y ) { PosY = y; }
	static void AttachTroll( Troll & theTroll ) { ptrTroll = &theTroll; }
	static void AttachGrid( WorldGrid & theGrid ) { ptrGrid = &theGrid; }

	// Movement on the grid
	void MoveEntity( int dx, int dy ); 

	// The class this object advertises itself as.  Can change
	// over time, so we can't get by with typeid
	virtual int ExportedClass() = 0;

	// Interface call for whatever the entity does in each
	// time step
	virtual bool Update() = 0;
};


/*************************************************************
 * CLASS Sheep
 * The bottom of the food chain in this demo, the Sheep are
 * there to be eaten by the troll.  All they ever do is wander
 * and try to run away if they see a troll coming too close.
 *************************************************************/

class Sheep : public Entity
{
	// Sheep that get eaten die
	bool IsDead;

public:
	// Construction and access
	Sheep( int x, int y, int id ) : Entity( x, y, id ), IsDead( false ) 
		{ ptrGrid->Register( id, ExportedClass(), x, y ); }
	Entity * Clone() { return( new Sheep( GetX(), GetY(), GetWorldID() ) ); }

	int ExportedClass() { return ENTITY_SHEEP; }

	bool Update();
};


/*************************************************************
 * CLASS Knight
 * The bad guys in this simulation.  Knights wander aimlessly
 * until they detect a troll, then track it mercilessly until
 * a fight to the death can ensue.
 ************************************************************/

class Knight : public Entity
{
	// Knights wander around aimlessly until they smell a troll,
	// and then they pursue it to the death
	bool IsBerserk;
	bool IsDead;

public:
	Knight( int x, int y, int id ) : Entity( x, y, id ), IsBerserk( false ), IsDead( false )
			{ ptrGrid->Register( id, ExportedClass(), x, y ); }
	Entity * Clone() { return( new Knight( GetX(), GetY(), GetWorldID() ) ); }

	int ExportedClass() { return ENTITY_KNIGHT; }

	bool Update();
};


/************************************************************
 * CLASS Tower
 * This is an immobile, indestructible guard tower manned by
 * archers whofire at the troll from a distance
 ***********************************************************/

class Tower : public Entity
{
public:
	Tower( int x, int y, int id ) : Entity( x, y, id )
			{ ptrGrid->Register( id, ExportedClass(), x, y ); }
	Entity * Clone() { return( new Tower( GetX(), GetY(), GetWorldID() ) ); }

	int ExportedClass() { return ENTITY_TOWER; }
	bool Update();
	
};


/***********************************************************
 * CLASS Haven
 * A safe place for the troll to rest (i.e., a dark and
 * foreboding monster-riddern bog).
 **********************************************************/

class Haven : public Entity
{
public:
	Haven( int x, int y, int id ) : Entity( x, y, id )
			{ ptrGrid->Register( id, ExportedClass(), x, y ); }
	Entity * Clone() { return( new Haven( GetX(), GetY(), GetWorldID() ) ); }

	int ExportedClass() { return ENTITY_HAVEN; }
	bool Update() { return true; }
};


/**********************************************************
 * CLASS Trap
 * A troll-capture device, disguised as a Haven until the 
 * troll detects the trap or falls into it.
 *********************************************************/

class Trap : public Entity
{
	// A trap looks just like a safe haven until it is unveiled
	bool Unveiled;

public:
	Trap( int x, int y, int id ) : Entity( x, y, id ), Unveiled( false )
			{ ptrGrid->Register( id, ExportedClass(), x, y ); }
	Entity * Clone() { return( new Trap( GetX(), GetY(), GetWorldID() ) ); }

	int ExportedClass() { return( Unveiled ? ENTITY_TRAP : ENTITY_HAVEN ); }

	bool Update();
};

#endif