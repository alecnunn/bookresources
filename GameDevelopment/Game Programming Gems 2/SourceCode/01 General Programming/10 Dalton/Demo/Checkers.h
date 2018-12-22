/***
 * File:   Checkers.h - Header File
 *         -----------------------------------------------------
 * Author: Peter Dalton
 * Date:   4/9/2001 3:32:48 PM
 *
 * Description:

 *
 * Copyright (C) Peter Dalton, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied warranties. You may freely copy 
 * and compile this source into applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 *                  "Portions Copyright (C) Peter Dalton, 2001"
 */

#ifndef _CHECKERS_H__
#define _CHECKERS_H__

#include "Board.h"

/*******************************************************************************************/
/*******************************************************************************************/
// ***** Declaration of the Checkers Class

class Checkers
{
public:
	// ***** Public Member Methods

	enum ALGORITHM { MINMAX, PRUNNING, PLAUSIBLE, ITERATIVE };

	Checkers( void );
	~Checkers( void );

	bool computerMove( void );
	

	inline bool gameOver( void )                    { return m_board->GameOver();         }
	inline bool redsTurn( void )                    { return m_redsTurn;                  }
	inline void setRedsTurn( bool redsTurn = true ) { m_redsTurn = redsTurn;              }
	inline void setRedsSearchDepth( int depth )     { if (depth > 0) m_redsPly   = depth; }
	inline void setWhitesSearchDepth( int depth )   { if (depth > 0) m_whitesPly = depth; }

	inline int getRedsSearchDepth( void )           { return m_redsPly;    }
	inline int getWhitesSearchDepth( void )         { return m_whitesPly;  }
	inline Board* getBoard( void )                  { return m_board;      }
	inline void setRedsAlgorithm( ALGORITHM alg )   { m_redsAlgor = alg;   }
	inline void setWhitesAlgorithm( ALGORITHM alg ) { m_whitesAlgor = alg; }
	inline int  makeMove( pathValue path[] ) 
	{ 
		int r = m_board->makeMove( path, m_redsTurn ); 
		if (r == 1) m_redsTurn = !m_redsTurn;
		return r;
	}
	inline bool staleMate( void )
	{
		PathList list;
		return (m_board->generateMoves( m_redsTurn, list ) == 0);
	}
	inline void setRedsEvalFunc( int(*eval)(Board*, bool) ) 
	{ 
		if (!eval) m_redsEvalFunc = BasicEvaluation;
		else       m_redsEvalFunc = eval;
	}
	inline void setWhitesEvalFunc( int(*eval)(Board*, bool) ) 
	{ 
		if (!eval) m_whitesEvalFunc = BasicEvaluation;
		else       m_whitesEvalFunc = eval;
	}

private:
	// ***** Private Member Methods

	int AlphaBeta( Board* board, bool myTurn, int curPly, int maxPly, int alpha, 
		             int beta, pathValue *path, double startTime = -1 );

	int Iterative( Board *board, pathValue *array );

	void Plausible_Ordering( Board *board, PathList *moves, bool MaxNode );

	static int BasicEvaluation( Board *board, bool redsTurn );

	
	// ***** Private Member Variables
	Board *m_board;
	bool   m_redsTurn;

	int(*m_redsEvalFunc)( Board *bd, bool redsTurn );
	int(*m_whitesEvalFunc)( Board *bd, bool redsTurn );

	ALGORITHM m_redsAlgor;
	ALGORITHM m_whitesAlgor;

	int m_redsPly;
	int m_whitesPly;

	pathValue  m_RedsLastMove[10];
	pathValue  m_WhitesLastMove[10];
};

#endif /* _CHECKERS_H__ */

// ***** End of Checkers.h
/*******************************************************************************************/
/*******************************************************************************************/