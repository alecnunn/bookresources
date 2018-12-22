/***
 * File:   Board.h - Header File
 *         -----------------------------------------------------
 * Author: Peter Dalton
 * Date:   3/12/2001 11:12:47 AM
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

#ifndef _BOARD_H__
#define _BOARD_H__

#include <fstream.h>
#include "Path.h"

// The definition of a piece. Capitalized names represent KING's
typedef char piece;
const char RED   = 0;
const char red   = 1;
const char empty = 2;
const char white = 3;
const char WHITE = 4;

// These direction abbreviations are used to index into lookup tables
const int NW  = 0;
const int NE  = 1;
const int SW  = 2;
const int SE  = 3;

const int BOARD_SIZE  = 33; // Valid positions are 1 to 32
const int NUM_DIR     = 4;  // A constant to terminate looping over directions
const int MAX_MOVES   = 12;
const int MAX_PATHLEN = 11;
class PathList;

/*******************************************************************************************/
/*******************************************************************************************/
// ***** Declaration of the Board Class

class Board
{
public:
	// ***** Public Member Methods

	Board( bool emptyBoard = true );
	Board( const Board& b );
	~Board( void );

	int generateMoves( bool RedsTurn, PathList& list );
	int makeMove( pathValue movePath[], bool RedsTurn );

  int CenterControl( bool RedPieces );
  int KingsRow( bool RedPieces );
  int killedPawn( bool RedPieces );
  int killedKing( bool RedPieces );

	inline int  NumRedPieces( void )   { return m_numRedPieces;   }
  inline int  NumWhitePieces( void ) { return m_numWhitePieces; }
	inline int  NumRedKings( void )    { return m_numRedKings;    }
	inline int  NumWhiteKings( void )  { return m_numWhiteKings;  }
	inline bool GameOver( void )       { return (m_numWhitePieces == 0 || m_numRedPieces == 0); }

	inline void  set(int pos, piece p) { m_Board[pos] = p;    }
  inline piece get(int pos)          { return m_Board[pos]; }

  Board& operator=( const Board &rhs );
  bool   operator==( const Board &rhs );  

	void updateSlide( int from, int to );
  void updateJump( int from, int to );
   
  static const int JUMP_OVER[BOARD_SIZE][4];
  static const int SLIDE[BOARD_SIZE][4];

	void init();
	void init2();

  friend ostream&  operator<<( ostream &os,   Board &bd );
  friend ofstream& operator<<( ofstream &ofs, Board &bd );

private:
	// ***** Private Member Variables

	int m_numRedPieces;         // The number of red pieces.
	int m_numWhitePieces;       // The number of white pieces.
	int m_numRedKings;          // The number of red kings.
	int m_numWhiteKings;        // The number of white kings.
  piece m_Board[BOARD_SIZE];  // The actual game board.
};

#endif /* _BOARD_H__ */

// ***** End of Board.h
/*******************************************************************************************/
/*******************************************************************************************/