/* Copyright (C) François Dominic Laramée, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) François Dominic Laramée, 2001"
 */
/****************************************************************
 *
 * ENTITY.H - Sample behavioral class hierarchy
 *
 ***************************************************************/

#include <iostream>
#include "sammy.h"

/***************************************************************
 *
 * ExportedClass Enumeration
 *
 * As explained in the text, each C++ class of entity can
 * advertise itself as any number of "exported classes"; this 
 * helps clean up and simplify the code.  Here, I define
 * exported classes for a PlayerCharacter and a Ghost object
 * in a Pac-Man lookalike game.
 *
 **************************************************************/

enum ExportedClass
{
  SCARED_GHOST, ACTIVE_GHOST, DEAD_GHOST,
	SAFE_PLAYER_CHARACTER, REGULAR_PLAYER_CHARACTER
};


/***************************************************************
 *
 * CLASS ENTITY
 *
 * This is the mothen of all entity classes ;-)
 * It is a pure virtual class; while we use pointers to Entity
 * objects everywhere, we never create any actual instances of
 * this base class.
 *
 **************************************************************/

class Entity
{

  // Every Entity needs a SAMMY and an exported class
  SAMMY * sammy;
  int exportedClass;
		  
	// The entity's current state
	EntitySnapshot snapshot;

  // Add some application-specific data if you need to
  // ...


public:

  // Accessors
	int GetExportedClass() { return exportedClass; }
	void SetExportedClass( int newval ) { exportedClass = newval; }

  // The next three methods are the core of the Entity hierarchy
	// First, UpdateYourself, the method which lets each object step
	// forward in time and change its internal state independent of
	// anything and anyone else before rendering each frame of animation
	virtual bool UpdateYourself() 
	{
	  // This one can use a default implementation
	  // At the very least, you will need SAMMY to deal with
		// the animation keyframes and stuff like that.  
    sammy->UpdateEntityState( &snapshot );
		return true;
	}

  // Second, the "cloning constructor"
	virtual Entity * CloneEntity() = 0;

	// Finally, the HandleInteractions method, which lets entities
	// look at their surroundings and react to external events
	// (i.e., collisions)
	virtual bool HandleInteractions( Entity * target ) = 0;

	// Factory method; commented in detail in the .cpp file
	static Entity * MakeNewEntity( ExportedClass intCode, istream & is );


protected:

  // Now, a few helper methods, given protected access because they
	// are only intended to be used internally by HandleInteractions()
	// in the various subclasses

	// First, collision-detection and its possible results
	// TARGET_NORTH means that there is a collision between the source (the
	// object checking for collisions) and the target, and that the target
	// is located "north" of the source (i.e., higher on screen).
  enum { NO_COLLISION, TARGET_NORTH, TARGET_SOUTH, TARGET_WEST, TARGET_EAST };
	static int CollisionDetection( Entity * source, Entity * target )
	{
	  // Insert your favourite collision detection method here
		return NO_COLLISION;
	}

  // And finally, a little method which sets the internal state of the
	// entity by force and adjusts SAMMY accordingly
  virtual bool SetNewState( int i )
  {
	  snapshot.entityState = i; 
	  sammy->AlignWithNewFSMState( &snapshot ); 

    // Perform some extra bookkeeping if necessary
		return true;
	}


};


/***************************************************************
 *
 * CLASS GHOST: A behavioral class for PacMan enemies
 *
 **************************************************************/

class Ghost : public Entity
{

  // This bare-bones ghost implementation needs no instance data

public:

  // Constructor
	Ghost( int whichType, istream & infile )
	{
	  SetExportedClass( whichType );

		// Read instance parameters, for example initial position
		// and speed, from the infile
	}

	// Copy Constructor
	Ghost( Ghost & target )
	{
	  // copy everything shallowly, in flyweight fashion
		SetExportedClass( target.GetExportedClass() );
	}

	// CloneEntity will always look the exact same way, except
	// for the constructor call which depends on the class
  virtual Entity * CloneEntity()
	{
	  return( new Ghost( *this ) );
	}

	virtual bool UpdateYourself()
	{
	  // Begin by taking care of things that are common to all objects
	  Entity::UpdateYourself();

		// Now, new stuff
    cout << "This Ghost entity has exported class " << GetExportedClass() << endl;

    return true;
	}

  virtual bool HandleInteractions( Entity * target );
};


/******************************************************************
 *
 * CLASS PlayerCharacter
 *
 * A template for a player character in a PacMan game
 *
 *****************************************************************/

class PlayerCharacter : public Entity
{
  // Let's keep the score here.  You'll probably want to add
	// position, velocity and other parameters as well.
  int score;

public:

  // Construction and cloning look a lot like those of the
	// Ghost class.  This is one of the neat things about this
	// hierarchy: you can add behavioral classes in a few minutes
  PlayerCharacter( int whichType, istream & infile )
	{
	  score = 0;
	  SetExportedClass( whichType );
	}

  PlayerCharacter( PlayerCharacter & target )
	{
	  SetExportedClass( target.GetExportedClass() );
	}

	virtual Entity * CloneEntity()
	{
	  return ( new PlayerCharacter( *this ) );
	}

  // UpdateYourself and HandleInteractions implement the
	// pure virtual aspects of class Entity
	virtual bool UpdateYourself();
	virtual bool HandleInteractions( Entity * target );

  // Finally, the class-specific methods which do not
	// descend from Entity

	// SignalDeath is received when the player dies.  In this case,
	// it would only happen when a vulnerable player character collides
	// with a ghost; in other games, it would also occur when a timer
	// runs out, the player runs out of food, is killed by a lethal blow, 
	// etc.  Having the method around prevents the other classes from having
	// to know anything about how the player character handles death, which
	// is cleaner object-oriented design.
	bool SignalDeath();

  // Score-keeping
	void AddToScore( int howManyPoints )
	{
	  score += howManyPoints;
	}

};