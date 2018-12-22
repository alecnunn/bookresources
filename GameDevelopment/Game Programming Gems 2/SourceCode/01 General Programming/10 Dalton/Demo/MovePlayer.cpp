/***
 * File:   MovePlayer.cpp - Implements MovePlayer.h
 *         -----------------------------------------------------
 * Author: Peter Dalton
 * Date:   3/13/2001 9:53:22 AM
 *
 * Description:
			This class is responsible for generating all posible moves and for actually making 
			moves.
 *
 * Copyright (C) Peter Dalton, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied warranties. You may freely copy 
 * and compile this source into applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 *                  "Portions Copyright (C) Peter Dalton, 2001"
 */

#include "MemoryManager.h"

#include "Board.h"
#include "MoveTree.h"
#include "Path.h"
#include "MovePlayer.h"

/*******************************************************************************************/
/*******************************************************************************************/
// ***** Implementation of the MovePlayer Class

/**
 * MovePlayer::MovePlayer():
 *  Constructor
 * 
 *  Return Type : 
 *  Arguments   : 
 *  	Board* bd	: The current game board.
 */
MovePlayer::MovePlayer( Board* bd ) : m_board(bd)
{}

/*******************************************************************************************/

/**
 * MovePlayer::~MovePlayer():
 *  Destructor.
 * 
 *  Return Type : 
 *  Arguments   : 
 */
MovePlayer::~MovePlayer( void )
{}

/*******************************************************************************************/

/**
 * MovePlayer::makeMove():
 *  This method is responsible for making the desired move and updating the game board.  
 *  The following is a list of return values:
 *       -2 -> Must make a jump when available.
 *       -1 -> Ambiguous move.
 *        0 -> Tryed to make an invalid move.
 *        1 -> Valid move.
 * 
 *  Return Type : int -> One of the return types listed above.
 *  Arguments   : 
 *  	int path[]	  : The path describing the desired move.
 *  	bool redsTurn	: Whether or not it is reds turn.
 */
int MovePlayer::makeMove( pathValue path[], bool redsTurn )
{
	piece ptemp;
	bool mustJump = canJump( redsTurn );
	bool validMove = false;

	if (path[0] <= 1) return -1;
	
	ptemp = m_board->get( path[1] );
	if (redsTurn) {
		if (ptemp != red && ptemp != RED)     return 0;
	}
	else {
		if (ptemp != white && ptemp != WHITE) return 0;
	}

	if (mustJump) {
		if (ptemp == red) {              // Red Pawn
			if (validateJump( SW, NUM_DIR, path )) validMove = true; 
		}
		else if (ptemp == white) {       // White Pawn
			if (validateJump( NW, SW, path ))      validMove = true; 
		}
		else {                           // Red or White King
			if (validateJump( NW, NUM_DIR, path )) validMove = true; 
		}

		if (!validMove) return -2;       // Invalid Move, must make a jump when available.
	}
	else {
		if (path[0] > 2) return 0;       // A slide can only be defined by 2 states.

		if (ptemp == red) {              // Red Pawn
			if (validateSlide( SW, NUM_DIR, path )) validMove = true;
		}
		else if (ptemp == white) {       // White Pawn
			if (validateSlide( NW, SW, path ))      validMove = true;
		}
		else {                           // Red or White King
			if (validateSlide( NW, NUM_DIR, path )) validMove = true;
		}

		if (!validMove) return 0;        // Invalid Move!!
	}

	// Actually make the move now that it has been validated.
	if (mustJump) {
		for (int ii = 1; ii < path[0]; ++ii) {
			m_board->updateJump( path[ii], path[ii+1] );
		}
	}
	else {
		m_board->updateSlide( path[1], path[2] );
	}
	return 1;
}

/*******************************************************************************************/

/**
 * MovePlayer::canJump():
 *  Returns TRUE if it is possible to jump, otherwise FALSE.
 * 
 *  Return Type : bool -> Whether or not it is possible to make a jump.
 *  Arguments   : 
 *  	bool redsTurn	: Whether or not it is reds turn.
 */
bool MovePlayer::canJump( bool redsTurn )
{
  piece ptemp;
  int ii, curDir, dest;
	int Direction;

  if (redsTurn) { // We are moving red
    for (ii = 1; ii < BOARD_SIZE; ++ii) {
			ptemp = m_board->get( ii );

			if (ptemp == red)      Direction = SW;
			else if (ptemp == RED) Direction = NW;
			else                   continue;
			
			for (curDir = Direction; curDir < NUM_DIR; ++curDir) {
				dest = Board::SLIDE[ii][curDir];
				if (dest != 0) {                                             // It is possible to jump from this position.
					ptemp = m_board->get( dest );
					if (ptemp != red && ptemp != RED && ptemp != empty) {      // There is an enemy piece to jump.
						dest = Board::SLIDE[dest][curDir];
						if (dest) {
							if (m_board->get( dest ) == empty) return true;        // The spot on the other side of the enemy
						}                                                        // is empty.
					}
				}
			}
    } 
  } 
  else {         // We are moving white
    for (ii = 1; ii < BOARD_SIZE; ++ii) {
			ptemp = m_board->get( ii );

			if (ptemp == white)      Direction = SW;
			else if (ptemp == WHITE) Direction = NUM_DIR;
			else                     continue;

			for (curDir = NW; curDir < Direction; ++curDir) {
				dest = Board::SLIDE[ii][curDir];

				if (dest != 0) {                                             // It is possible to jump from this position.
					ptemp = m_board->get( dest );
					if (ptemp != white && ptemp != WHITE && ptemp != empty) {  // There is an enemy piece to jump.
						dest = Board::SLIDE[dest][curDir];
						if (dest) {
							if (m_board->get( dest ) == empty) return true;        // The spot on the other side of the enemy
						}                                                        // is empty.
					}
				}
			}
    }
  }
  return false;
}

/*******************************************************************************************/

/**
 * MovePlayer::generateMoves():
 *  Given the current state of the board and which players turn it is this method determines
 *  all possible moves.  All moves are stored within the provided PathList.
 * 
 *  Return Type : void 
 *  Arguments   : 
 *  	bool redsTurn	      : Whether or not it is red's turn.
 *  	PathList& endStates	: A linked list to hold all possible moves.
 */
void MovePlayer::generateMoves( bool redsTurn, PathList& endStates )
{
  piece ptemp;
  int ii;
   
	MoveTree moves; 
  bool mustJump = canJump( redsTurn );

  if (redsTurn) {                        // We are moving Red
    for (ii = 1; ii < BOARD_SIZE; ++ii) {
			ptemp = m_board->get( ii );
			if (ptemp == red) {                // Red Pawn
				moves.Initialize( m_board, ii, red, RED, SW, NUM_DIR );
				moves.generateMoves( endStates, ii, mustJump );
			}
			else if (ptemp == RED) {           // Red King
				moves.Initialize( m_board, ii, RED, red, NW, NUM_DIR);
				moves.generateMoves( endStates, ii, mustJump );
			}
    } 
  } 
  else {                                 // We are moving White 
		for (ii = 1; ii < BOARD_SIZE; ++ii) {
			ptemp = m_board->get( ii );
			if (ptemp == white) {              // White Pawn
				moves.Initialize( m_board, ii, white, WHITE, NW, SW );
				moves.generateMoves( endStates, ii, mustJump );
			}
			else if (ptemp == WHITE) {         // White King
				moves.Initialize( m_board, ii, WHITE, white, NW, NUM_DIR );
				moves.generateMoves( endStates, ii, mustJump );
			}
    }
  }
}

/*******************************************************************************************/

/**
 * MovePlayer::validateSlide():
 *  Determines whether or given move path is a valid slide or not.
 * 
 *  Return Type : bool -> Whether or not the given move path is a valid slide.
 *  Arguments   : 
 *  	int startDir	   : The starting board coordinate for possible moves.
 *  	int endDir	     : The ending board coordinate for possible moves.
 *  	pathValue path[] : The desired move path.
 */
bool MovePlayer::validateSlide( int startDir, int endDir, pathValue path[] )
{
	for (int ii = startDir; ii < endDir; ++ii) {
		if (Board::SLIDE[path[1]][ii] == path[2] && m_board->get( path[2] ) == empty) {
			return true;
		}
	}
	return false;
}

/*******************************************************************************************/

/**
 * MovePlayer::validateJump():
 *  Determines whether or given move path is a valid jump or not.
 * 
 *  Return Type : bool -> Whether or not the given move path is a valid jump.
 *  Arguments   : 
 *  	int startDir	   : The starting board coordinate for possible moves.
 *  	int endDir	     : The ending board coordinate for possible moves.
 *  	pathValue path[] : The desired move path.
 */
bool MovePlayer::validateJump( int startDir, int endDir, pathValue path[] ) 
{
	PathList list;
	MoveTree jumpTree;

	piece ptemp = m_board->get( path[1] );

	if (ptemp == red || ptemp == RED) {
		jumpTree.Initialize( m_board, path[1], red, RED, startDir, endDir );
	}
	else {
		jumpTree.Initialize( m_board, path[1], WHITE, white, startDir, endDir );
	}

	jumpTree.generateMoves( list, path[1], true );

	for (int ii = 0; ii < list.getLength(); ++ii) {
		if (list.pathsEqual( path, ii )) return true;
	}
	return false;
}

// ***** End of MovePlayer.cpp
/*******************************************************************************************/
/*******************************************************************************************/