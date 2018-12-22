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
 * File:   driver.cpp - Implements driver.h
 *         -----------------------------------------------------
 * Author: Peter Dalton
 * Date:   4/10/2001 10:16:32 AM
 *
 * Description:
       This file is the interface for the Checkers class. It provides a menu,
		   that allows the user to either play checkers with another opponent or the computer. 
			 If the computer is choosen a number of options can be choosen.
		
			 Algorithms: 
          1. MiniMax Search
          2. MiniMax Search with Alpha-Beta pruning
          3. MiniMax Search with Alpha-Beta pruning and plausable node ordering
          4. Iterative Deepening with MiniMax search (1 minute)

       Evaluation Functions:
          1. Basic  (number of pieces I have - number of pieces you have)
          2. Personal  - Explaned in detail below in the function
          3. Change the depth of search using either evaluation function.

 *
 */

#include "new_off.h"      // This here shows how to avoid compiler errors while using the 
#include <ctype.h>        //   Memory Manager.  This example is for illustration, these
#include <string.h>       //   headers do not redefine the new/delete operators, thus 
#include <time.h>         //   the new_off and new_on headers are not required.
#include "new_on.h"

#define USE_MEMORY_CACHE 1

#include "MemoryManager.h"
#include "Checkers.h"

// ***** Function Prototypes
void eventHandler( void );
void getMove( void );
void mainMenu( void );           
void algorithms( void );     
void evalFunctions( void );     
void displayMoves( PathList *moves );
int  PersonalEvaluation( Board *board, bool red );

// ***** Global Variables
Checkers *g_checkers = NULL;


/*******************************************************************************************/

/**
 * main():
 *  Main entry point.
 * 
 *  Return Type : int 
 *  Arguments   : NONE
 */
int main( void ) 
{
	g_checkers = new Checkers;
	eventHandler();
	delete g_checkers;
  return 0;
}  

/*******************************************************************************************/

/**
 * eventHandler():
 *  Controls the game flow.
 * 
 *  Return Type : void 
 *  Arguments   : NONE
 */
void eventHandler( void ) 
{
   char cmd;
   double timer;
   bool first = true;

   int Num_turns = 1;

   mainMenu();  // Display menu initally

   do {
     cout << ( g_checkers->redsTurn() ? "Red's   turn  (?):> " : "White's turn  (?):> " ); 
	 
     cin >> cmd;
     cin.ignore();
     cmd = tolower( cmd );
      
     switch (cmd) {             // Implement the following menu selections
			case 'a':                // Select between different AI algorithms
				algorithms();
				break;
				
			case 'c':                  // Requests the computer to make the move using current AI Algor.
				timer = clock();
				g_checkers->computerMove();
				cout << *g_checkers->getBoard();
				timer = ( clock() - timer )/CLOCKS_PER_SEC;
				cout << "\t\tComputer's turn took " << timer << " seconds\n\n";
				break;
				
			case 'r':                // Put the program into automatic run mode.   
				Num_turns = 1;
				while (Num_turns < 1000 && !g_checkers->gameOver()) {
					if (g_checkers->redsTurn()) {
						cout << "\nRed's turn: Move number: " << (Num_turns+1)/2 << endl;
					}
					else {
						cout << "\nWhite's turn: Move number: " << (Num_turns+1)/2 << endl;
					}
					Num_turns++;
 					
					timer = clock();
					if (!g_checkers->computerMove()) {
						break;
					}
					cout << *g_checkers->getBoard();
					timer = ( clock() - timer )/CLOCKS_PER_SEC;
					cout << "\t\tComputer's turn took " << timer << " seconds\n\n";
				}
				break;
					
			case 'e':   // Select between possible evaluation algorithms
				evalFunctions();
				break;
			case 'd':   // Display board
				cout << *g_checkers->getBoard();
				break;
			case 'g':   // Get possible moves for current player
				{
					PathList *moves = new PathList;
					g_checkers->getBoard()->generateMoves( g_checkers->redsTurn(), *moves );
					displayMoves( moves );
					delete moves;
				}
				break;
			case 'm':   // Enter a move
				getMove();
				break;
			case '?':   // HELP menu
				mainMenu();
				break;
			case 'q':   // Quit
				cout << *g_checkers->getBoard();
				cout << endl << "\t\tGOODBYE...." << endl << endl;
				break;
			default:    // Invalid Menu selection
				cout << "\nInvalid selection ... Please re-enter." << endl;
				break;
		}
      
		bool endFound = false;

		if (g_checkers->gameOver()) {
			cout << "\t\tGAME OVER....";
			if (g_checkers->getBoard()->NumRedPieces() > 0) {
				cout << "RED WINS !!!!!" << endl << endl;
			}
			else {
				cout << "WHITE WINS !!!!!" << endl << endl;
			}
			endFound = true;
		}
		else if (g_checkers->staleMate()) {
			cout << "\t\tSTALE MATE....";
			if (g_checkers->getBoard()->NumRedPieces() > g_checkers->getBoard()->NumWhitePieces()) {
				cout << "RED WINS !!!!!" << endl << endl;
			}
			else {
				cout << "WHITE WINS !!!!!" << endl << endl;
			}
			endFound = true;
		}
				
		if (endFound) {
			do {
				cout << "Play again (Y/N)? ";
				cin >> cmd;
				cmd = tolower( cmd );
			} while (cmd != 'n' && cmd != 'y');

			if (cmd == 'n') {
				cout << "\t\tGOODBYE...." << endl << endl;
				cmd = 'q';
			}
			else {
				g_checkers->getBoard()->init2();
			}
     }
  } while (cmd != 'q');
}  

/*******************************************************************************************/

/**
 * getMove():
 *  Lets the user make a move.
 * 
 *  Return Type : void 
 *  Arguments   : NONE
 */
void getMove( void ) 
{
	pathValue path[MAX_PATHLEN];
  char move[80];
  int  length, num, end=0;
   
  // Read move sequence from keyboard
  cout << "Enter move :> ";
  cin.getline( move, 70 );
  length = strlen( move );
  num = 0;

  // Only grabs integers from input stream. Ignores any characters
  // that are not integers between (0, BOARD_SIZE)
  for (int ii = 0; ii < length; ++ii) {
		if (isdigit( move[ii] )) {
 			num = num*10 + move[ii] - 48;         // Convert the string to a number
		}
    else {
			if (num > 0 && num < BOARD_SIZE) {
				path[++end] = num;                 // Add the move to the path list
			}
			num = 0;
		}
	}
  if (num != 0) {
    path[++end] = num;
	}
  path[0] = end;        // Add the length to the beginning of the path.
   
  // This function checks the move specified by path.  It will return 1 if
  // the move is valid, 0 if the move is invalid, or -1 if the move is ambiguous.
  int done = g_checkers->makeMove( path );
   
  if (done == 1) { //we made a move
    cout << *g_checkers->getBoard();
  }
  else if (done == -1) {
     cout << "You have entered an ambiguous move." << endl;
	}
  else if (done == -2) {
     cout << "You must make a jump when available." << endl;
	}
  else {
     cout << "You have entered an invalid move." << endl;
	}
} 

/*******************************************************************************************/

/**
 * displayMoves():
 *  Given a list of moves this method will display the list to the user.
 * 
 *  Return Type : void 
 *  Arguments   : 
 *  	PathList *moves	: The list of moves to be displayed.
 */
void displayMoves( PathList *moves ) 
{
  int length;
  pathValue *currPath;        //pointer to current path move array
   
  if (moves == NULL ) return;

  length = moves->getLength();
  if (length == 0) {
		cout << endl
	       << "There are currently no moves for you to make!. GAME OVER!." << endl
	       << endl;
    return;
  }

  // Loop through every move
  for (int ii = 0; ii < length; ++ii) {
		currPath = moves->getPath( ii );
    cout << "Move[" << ii << "]  =  {";

    // Step through the sequence of legal moves
    for (int jj = 1; jj <= currPath[0]; ++jj) {
			cout << (int)currPath[jj];
			if (jj != currPath[0]) {
				cout << '-';
			}
		}
		cout << '}' << endl;
	}
} 

/*******************************************************************************************/

/**
 * mainMenu():
 *  The main menu.
 * 
 *  Return Type : void 
 *  Arguments   : NONE
 */
void mainMenu( void ) 
{
	cout << endl << endl
			 << "***********************************************************************" << endl
	     << endl
	     << "\tC --> Computer. Computer makes a move." << endl
			 << "\tR --> Run.      Computer vs Computer." << endl
			 << "\tM --> Move.     User makes a move." << endl
			 << "\tG --> Get.      Gets a list of legal moves for current player." << endl
			 << endl
			 << "\tE --> Eval.     Select evaluation function to use." << endl
			 << "\tA --> Algo.     Select between AI Algorithms used." << endl
			 << endl
			 << "\tD --> Display.  Displays the game board." << endl
			 << endl
			 << "\t? --> Help.     Displays command menu." << endl
			 << "\tQ --> Quit.     Terminate." << endl
			 << endl
			 << "***********************************************************************" << endl
			 << endl;
}

/*******************************************************************************************/

/**
 * algorithms():
 *  Allows for the search algorithm to be changed.
 * 
 *  Return Type : void 
 *  Arguments   : NONE
 */
void algorithms( void ) 
{
	cout << endl << endl
			 << "***********************************************************************" << endl
			 << endl
			 << "\tM --> Minimax Search by itself (Default)." << endl
			 << "\tP --> Minimax Search with Alpha-Beta Pruning." << endl
			 << "\tA --> Add Plausible Move Ordering for Alpha-Beta Pruning." << endl
			 << "\tI --> Iterative Deepening with Alpha-Beta Pruning (1 minute)." << endl
			 << endl
			 << "***********************************************************************" << endl
			 << endl << endl << "Enter choice :> ";

	char cmd;
	do {
		cin >> cmd;
		cin.ignore();
		cmd = tolower( cmd );
		switch (cmd) {
			case 'm':
				if (g_checkers->redsTurn()) g_checkers->setRedsAlgorithm( Checkers::MINMAX );
				else                        g_checkers->setWhitesAlgorithm( Checkers::MINMAX );
				break;
			case 'p':
				if (g_checkers->redsTurn()) g_checkers->setRedsAlgorithm( Checkers::PRUNNING );
				else                        g_checkers->setWhitesAlgorithm( Checkers::PRUNNING );
				break;
			case 'a':
				if (g_checkers->redsTurn()) g_checkers->setRedsAlgorithm( Checkers::PLAUSIBLE );
				else                        g_checkers->setWhitesAlgorithm( Checkers::PLAUSIBLE );
				break;
			case 'i':
				if (g_checkers->redsTurn()) g_checkers->setRedsAlgorithm( Checkers::ITERATIVE );
				else                        g_checkers->setWhitesAlgorithm( Checkers::ITERATIVE );
				break;
			default:    // invalid selection
				cout << "\nInvalid selection ... Please re-enter." << endl << "--> ";
				break;
		}
	} while( cmd != 'm' && cmd != 'p' && cmd != 'a' && cmd != 'i' );
}

/*******************************************************************************************/

/**
 * evalFunctions():
 *  Allows for a new evaluation function to be used as well as the search depth to be changed.
 * 
 *  Return Type : void 
 *  Arguments   : NONE
 */
void evalFunctions( void ) 
{
	cout << endl << endl
			 << "***********************************************************************" << endl
			 << endl
			 << "\tB --> Basic Evaluation Function (Default)." << endl
			 << "\tP --> Personal Evaluation Fucntion." << endl
			 << "\tD --> Change the depth of the search algorithm (Default = 4, "
			 <<          "max = 30)." << endl
			 << endl
			 << "***********************************************************************" << endl
			 << endl << "Enter choice :> ";

	char cmd;
	int depth;
	do {
		cin >> cmd;
		cin.ignore();
		cmd = tolower( cmd );
		switch (cmd) {
			case 'b':
				if (g_checkers->redsTurn()) g_checkers->setRedsEvalFunc( NULL );
				else                        g_checkers->setWhitesEvalFunc( NULL );
				break;
			case 'p':
				if (g_checkers->redsTurn()) g_checkers->setRedsEvalFunc( PersonalEvaluation );
				else                        g_checkers->setWhitesEvalFunc( PersonalEvaluation );
				break;
			case 'd':
				cout << "\nEnter the new desired depth for the search algorithm :> ";
				cin >> depth;
				cout << endl;
				while( depth < 1 || depth > 20 ) {  // Invalid depth level
					cout << "\nYou have entered an invalid depth.  Please re-enter\n"
							 << "the desired depth: Range = 1 - 20 :> :";
					cin >> depth;
					cout << endl;
				}
				if (g_checkers->redsTurn()) g_checkers->setRedsSearchDepth( depth );
				else                        g_checkers->setWhitesSearchDepth( depth );
				break;
			default:   // invalid selection
				cout << "\nInvalid selection ... Please re-enter."
						 << endl;
				break;
		}
	} while( cmd != 'b' && cmd != 'p' && cmd != 'd' );
}

/*******************************************************************************************/

/**
 * PersonalEvaluation():
 *  Personal Evaluation: The value of a node is determined by a variety of 
 *  heuristics that are described directly below in the variable declarations.
 * 
 *  Return Type : int -> The value of the board.
 *  Arguments   : 
 *  	Board *board : The board to be evaluated.
 *  	bool red	   : Whether or not it is reds turn.
 */
int PersonalEvaluation( Board *board, bool red )
{
  int Red_vs_White;
	int MyCenterControl = board->CenterControl( red );
	int OpCenterControl = board->CenterControl( !red );
	int MyKingsRow      = board->KingsRow( red );
	int OpKingsRow      = board->KingsRow( !red );
	int MyPawnsExposed  = board->killedPawn( red );
	int MyKingsExposed  = board->killedKing( red );

	int MyNumKings, OpNumKings, Victory, MyNumPieces;
	if (red) {
		MyNumKings = board->NumRedKings();
		OpNumKings = board->NumWhiteKings();

		Red_vs_White = board->NumRedPieces() - board->NumWhitePieces();

		MyNumPieces = board->NumRedPieces();
		if (board->NumWhitePieces() == 0) Victory = 1;
		else                              Victory = 0;
	}
	else {
		MyNumKings = board->NumWhiteKings();
		OpNumKings = board->NumRedKings();

		Red_vs_White = board->NumWhitePieces() - board->NumRedPieces(); 

		MyNumPieces = board->NumWhitePieces();
		if (board->NumRedPieces() == 0) Victory = 1;
		else                            Victory = 0;
	}

	if (red) {
		return ((Red_vs_White*4)    + (MyNumKings*2)       + (OpNumKings*-1)    + 
						(MyCenterControl*0) + (OpCenterControl*-0) + 
						(MyKingsRow*1)      + (OpKingsRow*-1)      + 
						(MyPawnsExposed*-1) + (MyKingsExposed*-1)  + 
						(MyNumPieces*0)     + (Victory*100));
	}
	else {
		return ((Red_vs_White*4)    + (MyNumKings*2)       + (OpNumKings*-1)    + 
						(MyCenterControl*0) + (OpCenterControl*-0) + 
						(MyKingsRow*0)      + (OpKingsRow*-0)      + 
						(MyPawnsExposed*-1) + (MyKingsExposed*-1)  + 
						(MyNumPieces*0)     + (Victory*100));
	}
} 
