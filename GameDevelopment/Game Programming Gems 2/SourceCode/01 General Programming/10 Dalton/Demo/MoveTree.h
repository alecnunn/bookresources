/***
 * File:   MoveTree.h - Header File
 *         -----------------------------------------------------
 * Author: Peter Dalton
 * Date:   3/12/2001 1:49:38 PM
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

#ifndef _MOVETREE_H__
#define _MOVETREE_H__

class Jump;
class PathList;
class Board;

/*******************************************************************************************/
/*******************************************************************************************/
// ***** Declaration of the MoveTree Class

class MoveTree
{
public:
	// ***** Public Member Methods

	friend class MovePlayer;

	MoveTree();
  MoveTree( Board *bd, int p, piece ch, piece team, int beg, int end );
  ~MoveTree( void );
   
	Initialize( Board *bd, int p, piece ch, piece team, int beg, int end );
   
  void generateMoves( PathList& endStates, int position, bool canJump );
 
private:
	// ***** Private Member Methods
	void keepJumping( PathList& endStates, Board board, pathValue path[], int moveNum, int position );

	// ***** Private Member Variables
  Board *m_board;         // Game Board.   
  int    m_beg;           // Holds the direction to start looking in.
  int    m_end;           // Holds the directoin to quit looking in.
  piece  m_ch;            // Holds the color and rank of the piece.
  piece  m_team;          // Holds the other rank for this color (for speed).
};

#endif /* _MOVETREE_H__ */

// ***** End of MoveTree.h
/*******************************************************************************************/
/*******************************************************************************************/