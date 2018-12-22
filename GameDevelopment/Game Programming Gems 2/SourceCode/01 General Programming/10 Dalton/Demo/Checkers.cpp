/* Copyright (C) Peter Dalton, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Peter Dalton, 2001"
 */
/***
 * File:   Checkers.cpp - Implements Checkers.h
 *         -----------------------------------------------------
 * Author: Peter Dalton
 * Date:   4/9/2001 3:33:00 PM
 *
 * Description:

 *
 */

#include <time.h>

#include "MemoryManager.h"
#include "Checkers.h"

/*******************************************************************************************/
/*******************************************************************************************/
// ***** Implementation of the Checkers Class

/**
 * Checkers::Checkers():
 *  Constructor.
 * 
 *  Return Type : 
 *  Arguments   : NONE
 */
Checkers::Checkers( void )
{
	m_board = new Board( false );
	m_redsTurn = true;

	m_redsEvalFunc   = BasicEvaluation;
	m_whitesEvalFunc = BasicEvaluation;

	m_redsAlgor   = PRUNNING;
	m_whitesAlgor = PRUNNING;

	m_redsPly   = 4;
	m_whitesPly = 4;

	for (int ii = 0; ii < 10; ++ii) {
		m_RedsLastMove[ii] = m_WhitesLastMove[ii] = 0;
	}
}

/*******************************************************************************************/

/**
 * Checkers::~Checkers():
 *  Destructor.
 * 
 *  Return Type : 
 *  Arguments   : NONE
 */
Checkers::~Checkers( void ) 
{
	delete m_board;
	PathList::releaseCacheMemory();
}

/*******************************************************************************************/

/**
 * Checkers::computerMove():
 *  This method instructs the computer to make a move.  Based upon the set heristics this
 *  method will call the appropriate routines to determine the best move to be made.  TRUE
 *  is returned if the computer made a move, otherwise FALSE is returned.  FALSE is only
 *  returned if the game is over or there is a stale mate condition.
 * 
 *  Return Type : bool 
 *  Arguments   : NONE
 */
bool Checkers::computerMove( void )
{
  if (gameOver() || staleMate()) { // generate all of the possible moves for red
		return false;      // No moves left to be made, stale mate or game over!!
	}

	pathValue path[MAX_PATHLEN];       // Used to hold the best path
	int value;                         // The value of the best path

	if (m_redsTurn) {
		if (m_redsAlgor == ITERATIVE) value = Iterative( m_board, path );
		else                          value = AlphaBeta( m_board, true, 0, m_redsPly, -99999, 99999, path );
	
		for (int ii = 0; ii <= path[0]; ++ii) {
			m_RedsLastMove[ii] = path[ii];
		}
	}
	else {
		if (m_redsAlgor == ITERATIVE) value = Iterative( m_board, path );
		else                          value = AlphaBeta( m_board, true, 0, m_whitesPly, -99999, 99999, path );

		for (int ii = 0; ii <= path[0]; ++ii) {
			m_WhitesLastMove[ii] = path[ii];
		}
	}

	int done = m_board->makeMove( path, m_redsTurn );  // Make the computer's move

	m_redsTurn = !m_redsTurn;
	return true;
} 

/*******************************************************************************************/
// ***** Private Member Methods

/**
 * Checkers::AlphaBeta():
 *  This method performs alpha beta search routines to determine the best move for the 
 *  computer to make.
 * 
 *  Return Type : int -> The value of the best path found throughout the search.
 *  Arguments   : 
 *  	Board* board  	 : The current game board.
 *  	bool myTurn	     : Whether or not it is my time, maximize the score.
 *  	int curPly	     : The current search ply.
 *  	int maxPly	     : The maximum search ply.
 *  	int alpha	       : The best move found so far.
 *  	int beta	       : The best move found so far for my oponent.
 *  	pathValue *path	 : A buffer to hold the best move found.
 *    double startTime : The time at which the process began, used for Iterative searches.
 */
int Checkers::AlphaBeta( Board* board, bool myTurn, int curPly, int maxPly, int alpha, 
												 int beta, pathValue *path, double startTime )
{
	Board tempBoard;
	PathList possibleStates;
	int value;

	ALGORITHM alg = m_redsTurn ? m_redsAlgor : m_whitesAlgor;

	if (alg == ITERATIVE) {
		if ((clock() - startTime)/CLOCKS_PER_SEC >= 59.0) {
			if (m_redsTurn) return -99999; 
			else            return -99999; 
		}
	}

	if (myTurn) {
		value = alpha;     // Can't do worse than alpha
		board->generateMoves( m_redsTurn, possibleStates );
		if (possibleStates.getLength() == 0) {
			if (m_redsTurn) return (*m_redsEvalFunc)( board, m_redsTurn ); 
			else            return (*m_whitesEvalFunc)( board, m_redsTurn ); 
		}

		if (alg == PLAUSIBLE || alg == ITERATIVE) {
//			Plausible_Ordering( board, &possibleStates, true );
		}

		for (int ii = 0; ii < possibleStates.getLength(); ++ii) {

			if (curPly == 0 && possibleStates.getLength() > 1) {
				pathValue *p = possibleStates.getPath( ii );

				// Avoid making the same move over and over again.
				if (m_redsTurn) {
					if (p[0] == 2 && p[1] == m_RedsLastMove[2] && p[2] == m_RedsLastMove[1]) continue;
				}
				else {
					if (p[0] == 2 && p[1] == m_WhitesLastMove[2] && p[2] == m_WhitesLastMove[1]) continue;
				}
			}

			tempBoard = *board;
			tempBoard.makeMove( possibleStates.getPath( ii ), m_redsTurn );
			int A;
			if (curPly == maxPly-1) {
				if (m_redsTurn) A = (*m_redsEvalFunc)( &tempBoard, m_redsTurn ); 
				else            A = (*m_whitesEvalFunc)( &tempBoard, m_redsTurn );
			}
			else {
				A = AlphaBeta( &tempBoard, false, curPly+1, maxPly, value, beta, NULL );
			}
			if (A > value) {
				value = A;
				if (path) {
					pathValue *p = possibleStates.getPath( ii );
					for (int jj = 0; jj <= p[0]; ++jj) {
						path[jj] = p[jj];
					}
				}
			}

			if (alg != MINMAX && value >= beta) {
				return value;
			}
		}
	}
	else {
		value = beta;      // Can't do better than beta
		board->generateMoves( !m_redsTurn, possibleStates );
		if (possibleStates.getLength() == 0) {
			if (m_redsTurn) return (*m_redsEvalFunc)( board, m_redsTurn ); 
			else            return (*m_whitesEvalFunc)( board, m_redsTurn ); 
		}

		if (alg == PLAUSIBLE || alg == ITERATIVE) {
//			Plausible_Ordering( board, &possibleStates, false );
		}

		for (int ii = 0; ii < possibleStates.getLength(); ++ii) {
			tempBoard = *board;
			tempBoard.makeMove( possibleStates.getPath( ii ), !m_redsTurn );
			int B;
			if (curPly == maxPly-1) {
				if (m_redsTurn) B = (*m_redsEvalFunc)( &tempBoard, m_redsTurn ); 
				else            B = (*m_whitesEvalFunc)( &tempBoard, m_redsTurn );
			}
			else {
				B = AlphaBeta( &tempBoard, true, curPly+1, maxPly, alpha, value, NULL );
			}
			if (B < value) {
				value = B;
			}

			if (alg != MINMAX && value <= alpha) {
				return value;
			}
		}
	}

	return value;
}

/*******************************************************************************************/

/**
 * Checkers::Iterative():
 *  Performs iterative deepening to determine the best move to be made.
 * 
 *  Return Type : int -> The best path value found throughout the search.
 *  Arguments   : 
 *  	Board *board	  : The current game board.
 *  	pathValue *path : Used to hold the best move path upon return.
 */
int Checkers::Iterative( Board *board, pathValue *path )
{
  int bestMove;
	pathValue path2[12];

  int depth = 5; // At least a depth of 5 can be reached.  Starting the search at
                 // a lower level is simply a waste of time and resources.

  double timer = clock();
  do 
  {
		int v = AlphaBeta( board, true, 0, depth++, -99999, 99999, path2, timer );
		if (v != -99999) {
			bestMove = v;
			for (int ii = 0; ii <= path2[0]; ++ii) {
				path[ii] = path2[ii];
			}
		}

  }while((clock() - timer)/CLOCKS_PER_SEC < 59.8 );

	cout << "Reached Depth: " << --depth << endl;
  return bestMove;
} 

/*******************************************************************************************/

/**
 * Checkers::Plausible_Ordering():
 *  Given a list of moves this method is responsible for reordering the moves by sorting 
 *  them.  If MaxNode == true then the list is sorted in decending order, otherwise they
 *  are sorted in accending order.
 * 
 *  Return Type : void 
 *  Arguments   : 
 *  	Board *board	  : The current game board.
 *  	PathList *moves	: The list of moves to be sorted.
 *  	bool MaxNode	  : Whether to sort in accending or decending order.
 */
void Checkers::Plausible_Ordering( Board *board, PathList *moves, bool MaxNode )
{
	if (moves->getLength() <= 1) return;   // No need to sort the list
  
	int value[MAX_MOVES];
  Board tempBoard;

  // Grab the value of the board if each of the possible moves where made
  for (int ii = 0; ii < moves->getLength(); ++ii) {
		tempBoard = *board;
    tempBoard.makeMove( moves->getPath( ii ), m_redsTurn );

		if (m_redsTurn) value[ii] = (*m_redsEvalFunc)( &tempBoard, m_redsTurn );
		else            value[ii] = (*m_whitesEvalFunc)( &tempBoard, m_redsTurn );
  }	

	if (MaxNode) moves->sortListDecending( value );
	else         moves->sortListAccending( value );
} 

/*******************************************************************************************/

/**
 * Checkers::BasicEvaluation():
 *  The default evaluation routine.  Direct comparison to the number of pieces each player
 *  has.
 * 
 *  Return Type : int 
 *  Arguments   : 
 *  	Board *board	: The board to be evaluated.
 *  	bool redsTurn	: Whether or not it is red's turn.
 */
int Checkers::BasicEvaluation( Board *board, bool redsTurn )
{
  if (redsTurn) return (board->NumRedPieces() - board->NumWhitePieces());
  else	        return (board->NumWhitePieces() - board->NumRedPieces());
} 

// ***** End of Checkers.cpp
/*******************************************************************************************/
/*******************************************************************************************/