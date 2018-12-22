/***
 * File:   Board.cpp - Implements Board.h
 *         -----------------------------------------------------
 * Author: Peter Dalton
 * Date:   3/12/2001 11:12:56 AM
 *
 * Description:
			A Board object contains the ability to store one of five possible values 
			(RED, red, empty, white, WHITE) in each of 32 possible locations. The class is to 
			be used to represent an 8x8 checker board organized as follows:

                         { 1}    { 2)    { 3}    { 4}
                     { 5}    { 6}    { 7}    { 8}
                         { 9}    {10}    {11}    {12}
                     {13}    {14}    {15}    {16}
                         {17}    {18}    {19}    {20}
                     {21}    {22}    {23}    {24}
                         {25}    {26}    {27}    {28}
                     {29}    {30}    {31}    {32}

     As in standard rules a piece at a given position can move in one of 4 possible 
		 directions: NorthWest(NW), NorthEast(NE), SoutWest(SW), SouthEast(SE).  
 *
 * Copyright (C) Peter Dalton, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied warranties. You may freely copy 
 * and compile this source into applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 *                  "Portions Copyright (C) Peter Dalton, 2001"
 */

#include <iomanip.h>
#include <string.h>

#include "MemoryManager.h"
#include "MovePlayer.h"
#include "Path.h"
#include "Board.h"

const int Board::JUMP_OVER[BOARD_SIZE][4] =
{{0, 0, 0, 0},
 {0, 0, 0, 6},   {0, 0, 6, 7},     {0, 0, 7, 8},     {0, 0, 8, 0},
 {0, 0, 0, 9},   {0, 0, 9, 10},    {0, 0, 10, 11},   {0, 0, 11, 0},
 {0, 6, 0, 14},  {6, 7, 14, 15},   {7, 8, 15, 16},   {8, 0, 16, 0},
 {0, 9, 0, 17},  {9, 10, 17, 18},  {10, 11, 18, 19}, {11, 0, 19, 0},
 {0, 14, 0, 22}, {14, 15, 22, 23}, {15, 16, 23, 24}, {16, 0, 24, 0},
 {0, 17, 0, 25}, {17, 18, 25, 26}, {18, 19, 26, 27}, {19, 0, 27, 0},
 {0, 22, 0, 0},  {22, 23, 0, 0},   {23, 24, 0, 0},   {24, 0, 0, 0},
 {0, 25, 0, 0},  {25, 26, 0, 0},   {26, 27, 0, 0},   {27, 0, 0, 0}};

const int Board::SLIDE[BOARD_SIZE][4] =
{{0, 0, 0, 0},
 {0, 0, 5, 6},     {0, 0, 6, 7},     {0, 0, 7, 8},     {0, 0, 8, 0}, 
 {0, 1, 0, 9},     {1, 2, 9, 10},    {2, 3, 10, 11},   {3, 4, 11, 12}, 
 {5, 6, 13, 14},   {6, 7, 14, 15},   {7, 8, 15, 16},   {8, 0, 16, 0}, 
 {0, 9, 0, 17},    {9, 10, 17, 18},  {10, 11, 18, 19}, {11, 12, 19, 20}, 
 {13, 14, 21, 22}, {14, 15, 22, 23}, {15, 16, 23, 24}, {16, 0, 24, 0}, 
 {0, 17, 0, 25},   {17, 18, 25, 26}, {18, 19, 26, 27}, {19, 20, 27, 28}, 
 {21, 22, 29, 30}, {22, 23, 30, 31}, {23, 24, 31, 32}, {24, 0, 32, 0}, 
 {0, 25, 0, 0},    {25, 26, 0, 0},   {26, 27, 0, 0},   {27, 28, 0, 0}}; 

/*******************************************************************************************/
/*******************************************************************************************/
// ***** Implementation of the Board Class

/**
 * Board::Board():
 *  Constructor.
 * 
 *  Return Type : 
 *  Arguments   : 
 *  	bool emptyBoard : Whether to create an empty board or create a predefined board.
 */
Board::Board( bool emptyBoard /* = true */ )
{
	emptyBoard ? init() : init2();
}

/*******************************************************************************************/

/**
 * Board::Board():
 *  Copy Constructor.
 * 
 *  Return Type : 
 *  Arguments   : 
 *  	const Board& b	: The board to be copied.
 */
Board::Board( const Board& b )
{
	m_numWhitePieces = b.m_numWhitePieces;
	m_numRedPieces   = b.m_numRedPieces;
	m_numRedKings    = b.m_numRedKings;
	m_numWhiteKings  = b.m_numWhiteKings;
	memcpy( m_Board, b.m_Board, sizeof(m_Board) );
}

/*******************************************************************************************/

/**
 * Board::~Board():
 *  Destructor, there is no dynamic memory, so there is nothing to do here.
 * 
 *  Return Type : 
 *  Arguments   : 
 */
Board::~Board( void )
{}

/*******************************************************************************************/

/**
 * Board::generateMoves():
 *  Given the current board this method will generate all posible moves for the specific
 *	color turn (red or white).  All of the states are stored within the provided PathList
 *  variable.  This method returns the number of possible moves that are returned within the 
 *  PathList.
 * 
 *  Return Type : int -> The number of possible moves. 
 *  Arguments   : 
 *  	bool RedsTurn	  : Whether it is red's or white's turn to move.
 *    PathList &paths : The list to in which all of the available moves will be stored.
 */
int Board::generateMoves( bool RedsTurn, PathList& paths )
{
	MovePlayer player( this );
  player.generateMoves( RedsTurn, paths );
  return paths.getLength();
}

/*******************************************************************************************/

/**
 * Board::makeMove():
 *  Given a path defining a possible move this method will make the requested move.  Return
 *  values:
 *             -2 -> Must make a jump when available.
 *             -1 -> Ambiguous move.
 *              0 -> Tryed to make an invalid move.
 *              1 -> Valid move.
 * 
 *  Return Type : int 
 *  Arguments   : 
 *  	int movePath[]	:
 *  	bool RedsTurn	:
 */
int Board::makeMove( pathValue movePath[], bool RedsTurn )
{
	MovePlayer player( this );
  return player.makeMove( movePath, RedsTurn );
}

/*******************************************************************************************/

/**
 * Board::CenterControl():
 *  Returns the number of pieces currently controlling the center region of the board.
 * 
 *  Return Type : int -> The number of pieces occupying the center region of the board.
 *  Arguments   : 
 *  	bool RedPieces : Whether to query the red pieces or white pieces.
 */
int Board::CenterControl( bool RedPieces )
{
	int counter = 0;
  if (RedPieces) {
    if (m_Board[14] == RED || m_Board[14] == red) ++counter;
		if (m_Board[15] == RED || m_Board[15] == red) ++counter;
    if (m_Board[18] == RED || m_Board[18] == red) ++counter;
    if (m_Board[19] == RED || m_Board[19] == red) ++counter;
  }
  else {
    if (m_Board[14] == WHITE || m_Board[14] == white) ++counter;
		if (m_Board[15] == WHITE || m_Board[15] == white) ++counter;
    if (m_Board[18] == WHITE || m_Board[18] == white) ++counter;
    if (m_Board[19] == WHITE || m_Board[19] == white) ++counter;
  }
  return counter;
}

/*******************************************************************************************/

/**
 * Board::KingsRow():
 *  Return the number of pieces still on home row.
 * 
 *  Return Type : int -> The number of pieces still on home row.
 *  Arguments   : 
 *  	bool RedPieces : Whether to query the red pieces or white pieces.
 */
int Board::KingsRow( bool RedPieces )
{
	int pieces = 0;
  if (RedPieces) {
		if (m_Board[1] == red) ++pieces;
		if (m_Board[2] == red) ++pieces;
		if (m_Board[3] == red) ++pieces;
		if (m_Board[4] == red) ++pieces;
  }
  else {
		if (m_Board[29] == white) ++pieces;
		if (m_Board[30] == white) ++pieces;
		if (m_Board[31] == white) ++pieces;
		if (m_Board[32] == white) ++pieces;
	}
  return pieces;
}

/*******************************************************************************************/

/**
 * Board::killedPawn():
 *  Returns the number of pawns that can be killed.
 * 
 *  Return Type : int -> The number of pawns that can be killed.
 *  Arguments   : 
 *  	bool RedPieces : Whether to query the red pieces or white pieces.
 */
int Board::killedPawn( bool RedPieces )
{
	int tempCnt = 0, dead = 0;
	bool evenRow = true;

  if (RedPieces) {
    for (int ii = 6; ii <= 27; ++ii) {
			if (tempCnt == 4) evenRow = !evenRow;
			else              ++tempCnt;

      if(m_Board[ii] == red) {
				// Upper right hand corner
				if (ii != 12 && ii != 20) {
					if (evenRow) {
						if (m_Board[ii-4] == WHITE && m_Board[ii+3] == empty)    ++dead;
					}
					else if (m_Board[ii-3] == WHITE && m_Board[ii+4] == empty) ++dead;
				}

				// Upper left hand corner
				if (ii != 13 && ii != 21) {
					if (evenRow) {
						if (m_Board[ii-5] == WHITE && m_Board[ii+4] == empty)    ++dead;
					}
					else if (m_Board[ii-4] == WHITE && m_Board[ii+5] == empty) ++dead;
				}

				// Lower right hand corner
				if (ii != 12 && ii != 20) {
					if (evenRow) {
						if ((m_Board[ii+4] == WHITE || m_Board[ii+4] == white) && m_Board[ii-5] == empty)    ++dead;
					}
					else if ((m_Board[ii+5] == WHITE || m_Board[ii+5] == white) && m_Board[ii-4] == empty) ++dead;
				}
			
				// Lower left hand corner
				if (ii != 13 && ii != 21) {
					if (evenRow) {
						if ((m_Board[ii+3] == WHITE || m_Board[ii+3] == white) && m_Board[ii-4] == empty)    ++dead;
					}
					else if ((m_Board[ii+4] == WHITE || m_Board[ii+4] == white) && m_Board[ii-3] == empty) ++dead;
				}
      } 
    }  
  } 
  else {
    for (int ii = 6; ii <= 27; ++ii) {
			if (tempCnt == 4) evenRow = !evenRow;
			else              ++tempCnt;

      if (m_Board[ii] == white) {	
				// Upper right hand corner
				if (ii != 12 && ii != 20) {
					if (evenRow) {
						if ((m_Board[ii-4] == RED || m_Board[ii-4] == red) && m_Board[ii+3] == empty)    ++dead;
					}
					else if ((m_Board[ii-3] == RED || m_Board[ii-3] == red) && m_Board[ii+4] == empty) ++dead;
				}

				// Upper left hand corner
				if (ii != 13 && ii != 21) {
					if (evenRow) {
						if ((m_Board[ii-5] == RED || m_Board[ii-5] == red) && m_Board[ii+4] == empty)    ++dead;
					}
					else if ((m_Board[ii-4] == RED || m_Board[ii-4] == red) && m_Board[ii+5] == empty) ++dead;
				}

				// Lower right hand corner
				if (ii != 12 && ii != 20) {
					if (evenRow) {
						if (m_Board[ii+4] == RED && m_Board[ii-5] == empty)    ++dead;
					}
					else if (m_Board[ii+5] == RED && m_Board[ii-4] == empty) ++dead;
				}

				// Lower left hand corner
				if (ii != 13 && ii != 21) {
					if (evenRow) {
						if (m_Board[ii+3] == RED && m_Board[ii-4] == empty)    ++dead;
					}
					else if (m_Board[ii+4] == RED && m_Board[ii-3] == empty) ++dead;
				}
      } 
    }
  } 	
  return dead;
}

/*******************************************************************************************/

/**
 * Board::killedKing():
 *  Returns the number of kings that can be killed.
 * 
 *  Return Type : int -> The number of kings that can be killed.
 *  Arguments   : 
 *  	bool RedPieces : Whether to query the red pieces or white pieces.
 */
int Board::killedKing( bool RedPieces )
{
	int tempCnt = 0, dead = 0;
	bool evenRow = true;

  if (RedPieces) {
    for (int ii = 6; ii <= 27; ++ii) {
			if (tempCnt == 4) evenRow = !evenRow;
			else              ++tempCnt;

      if(m_Board[ii] == RED) {
				// Upper right hand corner
				if (ii != 12 && ii != 20) {
					if (evenRow) {
						if (m_Board[ii-4] == WHITE && m_Board[ii+3] == empty)    ++dead;
					}
					else if (m_Board[ii-3] == WHITE && m_Board[ii+4] == empty) ++dead;
				}

				// Upper left hand corner
				if (ii != 13 && ii != 21) {
					if (evenRow) {
						if (m_Board[ii-5] == WHITE && m_Board[ii+4] == empty)    ++dead;
					}
					else if (m_Board[ii-4] == WHITE && m_Board[ii+5] == empty) ++dead;
				}

				// Lower right hand corner
				if (ii != 12 && ii != 20) {
					if (evenRow) {
						if ((m_Board[ii+4] == WHITE || m_Board[ii+4] == white) && m_Board[ii-5] == empty)    ++dead;
					}
					else if ((m_Board[ii+5] == WHITE || m_Board[ii+5] == white) && m_Board[ii-4] == empty) ++dead;
				}
			
				// Lower left hand corner
				if (ii != 13 && ii != 21) {
					if (evenRow) {
						if ((m_Board[ii+3] == WHITE || m_Board[ii+3] == white) && m_Board[ii-4] == empty)    ++dead;
					}
					else if ((m_Board[ii+4] == WHITE || m_Board[ii+4] == white) && m_Board[ii-3] == empty) ++dead;
				}
      } 
    }  
  } 
  else {
    for (int ii = 6; ii <= 27; ++ii) {
			if (tempCnt == 4) evenRow = !evenRow;
			else              ++tempCnt;

      if (m_Board[ii] == WHITE) {	
				// Upper right hand corner
				if (ii != 12 && ii != 20) {
					if (evenRow) {
						if ((m_Board[ii-4] == RED || m_Board[ii-4] == red) && m_Board[ii+3] == empty)    ++dead;
					}
					else if ((m_Board[ii-3] == RED || m_Board[ii-3] == red) && m_Board[ii+4] == empty) ++dead;
				}

				// Upper left hand corner
				if (ii != 13 && ii != 21) {
					if (evenRow) {
						if ((m_Board[ii-5] == RED || m_Board[ii-5] == red) && m_Board[ii+4] == empty)    ++dead;
					}
					else if ((m_Board[ii-4] == RED || m_Board[ii-4] == red) && m_Board[ii+5] == empty) ++dead;
				}

				// Lower right hand corner
				if (ii != 12 && ii != 20) {
					if (evenRow) {
						if (m_Board[ii+4] == RED && m_Board[ii-5] == empty)    ++dead;
					}
					else if (m_Board[ii+5] == RED && m_Board[ii-4] == empty) ++dead;
				}

				// Lower left hand corner
				if (ii != 13 && ii != 21) {
					if (evenRow) {
						if (m_Board[ii+3] == RED && m_Board[ii-4] == empty)    ++dead;
					}
					else if (m_Board[ii+4] == RED && m_Board[ii-3] == empty) ++dead;
				}
      } 
    }
  } 	
  return dead;
}

/*******************************************************************************************/

/**
 * Board::operator=():
 *  Overloaded Equal Operator.
 * 
 *  Return Type : Board& -> A reference to the current board
 *  Arguments   : 
 *  	const Board &rhs : The board to be copied.
 */
Board& Board::operator=( const Board &rhs )
{
	if (this != &rhs) {
		memcpy( m_Board, rhs.m_Board, sizeof(m_Board) );
		m_numRedPieces   = rhs.m_numRedPieces;
		m_numWhitePieces = rhs.m_numWhitePieces;
		m_numRedKings    = rhs.m_numRedKings;
		m_numWhiteKings  = rhs.m_numWhiteKings;
	}
  return *this;
}

/*******************************************************************************************/

/**
 * Board::operator==():
 *  Overloaded Equivalency Operator.
 * 
 *  Return Type : bool -> Whether or not the two boards are the same.
 *  Arguments   : 
 *  	const Board &rhs : The board to be compared to 'this' board.
 */
bool Board::operator==( const Board &rhs )
{
  for (int ii = 1; ii < BOARD_SIZE; ++ii) {
     if (m_Board[ii] != rhs.m_Board[ii] ) return false;
	}

  return true;
}   

/*******************************************************************************************/

/**
 *  operator<<():
 *  Overloaded Extraction Operator.  This method is used for writting the board information
 *  to the screen.
 * 
 *  Return Type : ostream& -> A reference to the provided ostream.
 *  Arguments   : 
 *  	ostream &os	: A reference to the stream.
 *  	Board &bd	  : The board to display.
 */
ostream& operator<<( ostream &os, Board &bd )
{
  char c[5] = {'R', 'r', ' ', 'w', 'W'};
  os << endl;
  for(int ii = 1; ii < BOARD_SIZE; ++ii) {
    os << "   " << '{' << c[bd.m_Board[ii]]   << '}';
	  os << "   " << '{' << c[bd.m_Board[ii+1]] << '}';
	  os << "   " << '{' << c[bd.m_Board[ii+2]] << '}';
	  os << "   " << '{' << c[bd.m_Board[ii+3]] << '}';
	  os << "     |     ";
	  os << "    " << '{' << setw(2) << ii++ << '}';
	  os << "    " << '{' << setw(2) << ii++ << '}';
	  os << "    " << '{' << setw(2) << ii++ << '}';
	  os << "    " << '{' << setw(2) << ii++ << '}' << endl;
			
    os << '{' << c[bd.m_Board[ii]]   << '}' << "   ";
	  os << '{' << c[bd.m_Board[ii+1]] << '}' << "   ";
	  os << '{' << c[bd.m_Board[ii+2]] << '}' << "   ";
	  os << '{' << c[bd.m_Board[ii+3]] << '}' << "   ";
	  os << "     |     ";
	  os << '{' << setw(2) << ii++ << '}' << "    ";
	  os << '{' << setw(2) << ii++ << '}' << "    ";
	  os << '{' << setw(2) << ii++ << '}' << "    ";
	  os << '{' << setw(2) << ii   << '}' << endl;
  }
  os << endl;
  return os;
}

/*******************************************************************************************/

/**
 *  operator<<():
 *  Overloaded Extraction Operator.  This method is used for writting the board information
 *  to a file.
 * 
 *  Return Type : ofstream& -> A reference to the provided ofstream.
 *  Arguments   : 
 *  	ostream &ofs : A reference to the output stream.
 *  	Board &bd	   : The board to saved.
 */
ofstream& operator<<( ofstream &ofs, Board &bd )
{
	return ofs;
}

/*******************************************************************************************/

/**
 * Board::updateSlide():
 *  Updates the board to reflect a slide move.
 * 
 *  Return Type : void  
 *  Arguments   : 
 *  	int from  : The starting position of the slide move. 
 *  	int to	  : The ending position of the slide move.
 */
void  Board::updateSlide( int from, int to )
{
	piece ch = m_Board[from];

	m_Board[from] = empty;
  if (ch == red) {
		if (to == 29 || to == 30 || to == 31 || to == 32) {
			++m_numRedKings;
			m_Board[to] = RED;
		}
    else {
			m_Board[to] = red;
		}
  }
  else if (ch == white) {
    if (to == 1 || to == 2 || to == 3 || to == 4) {
			++m_numWhiteKings;
			m_Board[to] = WHITE;
		}
    else {
			m_Board[to] = white;
		}
  }
  else {
    m_Board[to] = ch;
	}
}

/*******************************************************************************************/

/**
 * Board::updateJump():
 *  Updates the board to reflect a jump move.
 * 
 *  Return Type : void  
 *  Arguments   : 
 *  	int from	: The starting position to jump from.
 *  	int to	  : The ending position of the jump.
 */
void  Board::updateJump( int from, int to )
{
	piece ch = m_Board[from];

  m_Board[from] = empty; // The position we jump from is now empty.

  if (ch == red) {       // Update our current position.
		if (to == 29 || to == 30 || to == 31 || to == 32) {
			++m_numRedKings;
			m_Board[to] = RED;
		}
    else {
			m_Board[to] = red;
		}
  }
  else if (ch == white) {
    if (to == 1 || to == 2 || to == 3 || to == 4) {
			++m_numWhiteKings;
			m_Board[to] = WHITE;
		}
    else {
			m_Board[to] = white;
		}
  }
	else {
		m_Board[to] = ch;
	}

	if (ch == red || ch == RED) {
		--m_numWhitePieces;
		if (ch == RED) --m_numRedKings;
	}
	else {
		--m_numRedPieces;
		if (ch == WHITE) --m_numWhiteKings;
	}

	if      (to - from == 9) m_Board[JUMP_OVER[to][0]] = empty;
	else if (to - from == 7) m_Board[JUMP_OVER[to][1]] = empty;
	else if (from - to == 9) m_Board[JUMP_OVER[to][3]] = empty;
	else                     m_Board[JUMP_OVER[to][2]] = empty;
}

/*******************************************************************************************/

/**
 * Board::init():
 *  Creates an empty board.
 * 
 *  Return Type : void 
 *  Arguments   : NONE
 */
void Board::init( void )
{
	m_numWhitePieces = 0;
	m_numRedPieces   = 0;
	m_numRedKings    = 0;
	m_numWhiteKings  = 0;
	for (int ii = 0; ii < BOARD_SIZE; ++ii) {
		m_Board[ii] = empty;
	}
}

/*******************************************************************************************/

/**
 * Board::init2():
 *  Creates a board with the standard starting board configuration
 * 
 *  Return Type : void 
 *  Arguments   : NONE
 */
void Board::init2( void )
{
	int ii;
  m_Board[0] = empty;
  for (ii = 1; ii < 13; ++ii) {
    m_Board[ii] = red;
	}
  for (; ii < 21; ii++) {
    m_Board[ii] = empty;
	}
  for (; ii < BOARD_SIZE; ++ii) {
    m_Board[ii] = white;
	}
	m_numWhitePieces = 12;
	m_numRedPieces   = 12;
	m_numRedKings    = 0;
	m_numWhiteKings  = 0;
}
   
// ***** End of Board.cpp
/*******************************************************************************************/
/*******************************************************************************************/