/***
 * File:   MovePlayer.h - Header File
 *         -----------------------------------------------------
 * Author: Peter Dalton
 * Date:   3/13/2001 9:53:13 AM
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

#ifndef _MOVEPLAYER_H__
#define _MOVEPLAYER_H__

class Board;
class MoveTree;
class PathList;

#include "Path.h"

/*******************************************************************************************/
/*******************************************************************************************/
// ***** Declaration of the MovePlayer Class

class MovePlayer
{
public:
	// ***** Public Member Methods

	MovePlayer( Board* bd );
  ~MovePlayer( void );
   
	int makeMove( pathValue path[], bool redsTurn );
  bool canJump( bool redsTurn );
  void generateMoves( bool redsTurn, PathList& endStates );
   
private:

	bool validateJump( int startDir, int endDir, pathValue path[] ); 
	bool validateSlide( int startDir, int endDir, pathValue path[] );

	// ***** Private Member Variables
  Board *m_board;       // The board that contains state information
};

#endif /* _MOVEPLAYER_H__ */

// ***** End of MovePlayer.h
/*******************************************************************************************/
/*******************************************************************************************/