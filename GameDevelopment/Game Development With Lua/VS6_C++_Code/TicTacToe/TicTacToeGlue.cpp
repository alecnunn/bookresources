#include "stdafx.h"
#include "Settings.h"
#include <WinConsole.h>
#include "Base.h"

extern CBase *g_pBase;

extern "C" {
typedef struct 
{
	const char *name;
	int (*func)(lua_State *);
}luaDef;
}

// these are in indexes into the local table for the winning combos
const int winningPositions[8][3] = 
{
	{0, 1, 2},
	{3, 4, 5},
	{6, 7, 8},
	{0, 3, 6},
	{1, 4, 7},
	{2, 5, 8},
	{0, 4, 8},
	{2, 4, 6}
};

extern "C" int TTT_GetMove(lua_State *L)
{
	int move = -1;

	int argNum = 1;
	cLua *lua = g_pBase->GetLua();

	int board[9];
	for(int i=0; i<9; i++)
	{
		board[i] = (int) lua->GetNumberArgument(argNum++, 0);
	}

	int sidetomove = (int) lua->GetNumberArgument(argNum++, 0);

	// generate list of legal moves
	std::list<int> legalMoves;
	for(i=0; i<9; i++)
	{
		if(board[i] == 0)
			legalMoves.push_back(i);
	}

	// now go through the legal moves and select the best one
	int bestVal = 0;
	std::list<int>::iterator it = legalMoves.begin();
	while(it != legalMoves.end())
	{
		// look at each winning move and count the 
		// spaces (with this move made) that are in it
		for(i=0; i<8; i++)
		{
			int val = 0;
			if(((*it) == winningPositions[i][0]) || (board[winningPositions[i][0]] == sidetomove))
				++val;
			if(((*it) == winningPositions[i][1]) || (board[winningPositions[i][1]] == sidetomove))
				++val;
			if(((*it) == winningPositions[i][2]) || (board[winningPositions[i][2]] == sidetomove))
				++val;

			// now check that this one is not already blocked
			if((board[winningPositions[i][0]] != sidetomove) && (board[winningPositions[i][0]] != 0))
				val = -1;
			if((board[winningPositions[i][1]] != sidetomove) && (board[winningPositions[i][1]] != 0))
				val = -1;
			if((board[winningPositions[i][2]] != sidetomove) && (board[winningPositions[i][2]] != 0))
				val = -1;

			if(val > bestVal)
			{
				move = (*it);
				bestVal = val;
			}
		}
		
		++it;
	}

	// check for no good move...
	if(move == -1)
	{
		// just choose the first legal move
		move = (*legalMoves.begin());
	}

	lua->PushNumber(move+1); // adjust for 1 base in Lua code
	return 1;
}

// -1 means still in play
// 0 means draw
// other means that side wins

extern "C" int TTT_WinCheck(lua_State *L)
{
	int winner = -1;

	int argNum = 1;
	cLua *lua = g_pBase->GetLua();

	int board[9];
	for(int i=0; i<9; i++)
	{
		board[i] = (int) lua->GetNumberArgument(argNum++, 0);
	}

	// check for winner
	for(i=0; i<8; i++)
	{
		if(board[winningPositions[i][0]] == board[winningPositions[i][1]] && board[winningPositions[i][0]] == board[winningPositions[i][2]])
		{
			winner = board[winningPositions[i][0]];
		}
	}

	if(winner == -1)
	{
		//didn't find a winner, check for draw
		bool bDraw = true;
		for(i=0; i<9; i++)
		{
			if(board[i])
				bDraw = false;
		}
		if(bDraw)
			winner = 0;
	}

	lua->PushNumber(winner);
	return 1;
}

luaDef TTTGlue[] = 
{
	{"GetMove",				TTT_GetMove},
	{"WinCheck",			TTT_WinCheck},
	{NULL,					NULL},
};


extern void InitTicTacToeGlue(cLua *pLua)
{
	// init the glue functions required for the GUI
	for(int i=0; TTTGlue[i].name; i++)
	{
		pLua->AddFunction(TTTGlue[i].name, TTTGlue[i].func);
	}
}


