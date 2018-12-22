// Program: Genmaze
//
// Included on CD for book: Game Programming Gems 5
//
// Author: Timothy Roden  
//
// Copyright 2004-2005 Timothy E. Roden
//
// Permissions: The author grants the purchaser of the book Game Programming Gems 5 can freely
// use this software in any commericial or non-commercial applications the purchaser creates.
//
// Description: This file contains a driver that calls the maze generation code which creates a
// graph-based maze and writes it to a disk file.
//
// Compiler: Tested with Microsoft Visual C++ 2003 (Version 7.1.3088)

#include <iostream>
using namespace std;

#include "list.h"
#include "random.h"
#include "maze.h"

#pragma comment (lib, "winmm.lib")  // link in winmm library when bulid an EXE

void main ()
{
  Maze *maze;
  Random rnum;

  // Initialize a random number generator
  rnum = random_Init (random_TYPE_FAST_1);

  // Generate a maze, saving in a text file
  cout << "Starting maze generation ... ";
  maze = maze_Init_4Adjacent (maze_ALGORITHM_TYPE2, 50, 100, 25, rnum, "THE_MAZE.TXT", 0);
  cout << " Done!\n";

  // Free the maze
  maze_Free (maze);
  // Free the random number generator
  random_Free (rnum);
}
