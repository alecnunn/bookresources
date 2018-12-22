/* Copyright (C) François Dominic Laramée, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) François Dominic Laramée, 2001"
 */
#include "Entity.h"

// This is the entity factory.  Note that the exported
// class is passed along to the new entity so that it can
// establish itself properly.
//
// MEDIA REGISTRY:
// In the article, I mention that the entity factory should
// know which media files have already been loaded so that
// new entities sharing them don't have to duplicate the data.
// My personal favorite way to do this is actually located in
// the level editor instead of the factory method: my tools
// put clones (i.e., entities with the same graphics and
// sounds) together in the level definition file, and the 
// loader checks to see if a new entity is a clone of the
// previous one or the first instance of a new "clone
// family"; if it is a first instance, the loader calls
// MakeNewEntity; if it is a clone, it calls Clone() on
// the family's first entity directly and does not involve
// the factory method.  
//
// Read the preceding paragraph a few times and it will
// eventually start to make sense. ;-)

Entity * Entity::MakeNewEntity( ExportedClass whichType, istream & infile )
{	
	Entity * ptr;
	switch( whichType )
	{
		case SCARED_GHOST:
		case DEAD_GHOST:
		case ACTIVE_GHOST:
			ptr = new Ghost( whichType, infile );
			break;

    case SAFE_PLAYER_CHARACTER:
		case REGULAR_PLAYER_CHARACTER:
		  ptr = new PlayerCharacter( whichType, infile );
			break;

		default:
			ptr = 0;
			break;
	}

	return ptr;
}


/***************************************************************
 *
 * GHOST implementation
 *
 **************************************************************/

// Ghost::HandleInteractions
bool Ghost::HandleInteractions( Entity * target )
{
  // With the exported class mechanism, it is easy to ignore 
	// interactions with objects the Ghost never has anything to do with.
	// In this case, we ignore interactions with other Ghosts and with
	// "safe" player characters (i.e., characters who have just died and
	// receive a moment of grace before becoming vulnerable again); dead
	// ghost interact with nothing at all except their home base, which 
	// we don't define here, whose job is to "resurrect" them

  if ( GetExportedClass() == DEAD_GHOST )
	{
    // Normally, we would look for a collision with the Ghost's home
		// base
		return true;
	}

  if ( target->GetExportedClass() == REGULAR_PLAYER_CHARACTER )
	{
    // Look for a collision
	  int collTest = Entity::CollisionDetection( this, target );
	  if ( collTest == NO_COLLISION ) 
		  return true;

    // If there is a collision, handle it according to our
		// own exported class.  First, if the ghost is scared, 
		// it dies
    if ( GetExportedClass() == SCARED_GHOST )
		{
	    PlayerCharacter * plyr = (PlayerCharacter *) target;
		  SetExportedClass( DEAD_GHOST );
		  SetNewState( DEAD_GHOST );
			plyr->AddToScore( 100 );
		}

    // And if the ghost is active and not scared, the player
		// dies
		if ( GetExportedClass() == ACTIVE_GHOST )
		{
	    PlayerCharacter * plyr = (PlayerCharacter *) target;
		  plyr->SignalDeath();
		}
	}


  // That's it!  Clean and simple, huh?  ;-)
	return true;

}


/******************************************************************
 *
 * PlayerCharacter implementation
 *
 ******************************************************************/

// Self-update would require a query to the input device, etc.
bool PlayerCharacter::UpdateYourself()
{
  if ( GetExportedClass() == SAFE_PLAYER_CHARACTER )
	{
	  // add a test to determine if enough time has passed since
		// the character became safe...

		// If so, make him vulnerable again
		SetNewState( REGULAR_PLAYER_CHARACTER );
		SetExportedClass( REGULAR_PLAYER_CHARACTER );
	}

  return true;
}


// No need to look for interactions with ghosts, as they have
// already been handled above.  In a complete game, we would
// look for interactions with pills (which add to the score
// and kill the pill), and with Power Pills, which kill the 
// power pill and send a SignalGetScared to each ghost.  The
// easiest way to do it would be to set a flag in the Player
// Character object. Then, during the next frame of 
// animation, PlayerCharacter::HandleInteractions *would* look
// for ACTIVE_GHOST targets and send them the signal.
bool PlayerCharacter::HandleInteractions( Entity * target )
{
  return true;
}

// Handle a death signal: reduce lives counter, reset position,
// etc.  Here, we merely make the character invulnerable for
// a while
bool PlayerCharacter::SignalDeath()
{
  SetNewState( SAFE_PLAYER_CHARACTER );
	SetExportedClass( SAFE_PLAYER_CHARACTER );
  return true;
}