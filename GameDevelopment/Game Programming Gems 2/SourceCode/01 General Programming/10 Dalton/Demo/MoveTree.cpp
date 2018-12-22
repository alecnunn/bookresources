/***
 * File:   MoveTree.cpp - Implements MoveTree.h
 *         -----------------------------------------------------
 * Author: Peter Dalton
 * Date:   3/12/2001 1:49:48 PM
 *
 * Description:
			This class is responsible for creating a list of all possible moves given any 
			particular board configuration.
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
#include "Path.h"    
#include "MoveTree.h"


/*******************************************************************************************/
/*******************************************************************************************/
// ***** Implementation of the MoveTree Class

/**
 * MoveTree::MoveTree():
 *  Default Constructor, save all initialization for the Initialize() method.
 * 
 *  Return Type : 
 *  Arguments   : NONE
 */
MoveTree::MoveTree( void ) {}

/*******************************************************************************************/

/**
 * MoveTree::MoveTree():
 *  Constructor.
 * 
 *  Return Type : 
 *  Arguments   : 
 *  	Board *bd	 : The game board.
 *  	int p	     : Your position on the board.
 *  	piece ch	 : Your type.
 *  	piece team : Your team member type.
 *  	int bg	   : The beginning possible move coordinate.
 *  	int end	   : The ending possible move coordinate.
 */
MoveTree::MoveTree( Board *bd, int p, piece ch, piece team, int beg, int end )
{
	m_board = bd;
  m_ch    = ch;
  m_team  = team;
  m_beg   = beg;
  m_end   = end;
}

/*******************************************************************************************/

/**
 * MoveTree::~MoveTree():
 *  Destructor, free dynamically allocated memory.
 * 
 *  Return Type : 
 *  Arguments   : NONE
 */
MoveTree::~MoveTree( void ) {}

/*******************************************************************************************/

/**
 * MoveTree::Initialize():
 *  Initializes the MoveTree, this allows for explicit control over the class.
 * 
 *  Return Type : 
 *  Arguments   : 
 *  	Board *bd	 : The game board.
 *  	int p	     : Your position on the board.
 *  	piece ch	 : Your type.
 *  	piece team : Your team member type.
 *  	int bg	   : The beginning possible move coordinate.
 *  	int end	   : The ending possible move coordinate.
 */
MoveTree::Initialize( Board *bd, int p, piece ch, piece team, int beg, int end )
{
	m_board = bd;
  m_ch    = ch;
  m_team  = team;
  m_beg   = beg;
  m_end   = end;
}

/*******************************************************************************************/

/**
 * MoveTree::generateMoves():
 *  This method generates all of the possible moves for any given board position.
 * 
 *  Return Type : void 
 *  Arguments   : 
 *  	PathList& endStates	: An empty list used to hold all of the possible move paths.
 *  	int position	      : The starting position on the board to generate moves for.
 *  	bool canJump	      : Whether or not it is possible for the player to jump.
 */
void MoveTree::generateMoves( PathList& endStates, int position, bool canJump )
{
	piece ptemp = empty;
	pathValue path[MAX_MOVES] = {2, position, 0};

	if (canJump) {
		keepJumping( endStates, *m_board, path, 1, position );
	}
	else {
		for (int dir = m_beg; dir < m_end; ++dir) {
			int dest = Board::SLIDE[position][dir]; // 0 if no moves > 0 otherwise

			if (dest != 0) {
				ptemp = m_board->get( dest );
				if (ptemp == empty) {
					path[2] = dest;
					endStates.appendPath( path );
				}
			}
    } 
  } 
}

/*******************************************************************************************/
// ***** Private Member Methods

/**
 * MoveTree::keepJumping():
 *  This method recursively generates all possible jumps.
 * 
 *  Return Type : void 
 *  Arguments   : 
 *  	PathList& endStates	: The list of possible jumps.
 *  	Board board	        : The current game board.
 *  	pathValue path[]	  : The path being taken.
 *  	int moveNum	        : The move number.
 *  	int position	      : The board position.
 */
void MoveTree::keepJumping( PathList& endStates, Board board, pathValue path[], int moveNum, int position )
{
	piece ptemp;
	bool didJump = false;
	Board tempBoard = board;

	path[moveNum] = position;

	for (int dir = m_beg; dir < m_end; ++dir) {
		int dest = Board::SLIDE[position][dir];
		if (dest != 0) {
			ptemp = board.get( dest );
			if (ptemp != m_ch && ptemp != m_team && ptemp != empty) {
				dest = Board::SLIDE[dest][dir];
				if (dest != 0 && board.get( dest ) == empty) {
					didJump = true;
					board.updateJump( position, dest );
					keepJumping( endStates, board, path, moveNum+1, dest );
					board = tempBoard;
				}
			}
		}
	}

	if (!didJump && moveNum > 1) {
		path[0] = moveNum;
		endStates.appendPath( path );
	}
}

// ***** End of MoveTree.cpp
/*******************************************************************************************/
/*******************************************************************************************/