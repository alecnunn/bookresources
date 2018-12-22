/*____________________________________________________________________
|
| File: maze.h
|
| (C) Copyright 2004-2005 Timothy E. Roden.
|___________________________________________________________________*/

#define _MAZE_H_

typedef int int_plusone;  // an (unsigned) integer whose value is always one greater than it needs to be (0=invalid number)

enum MazeGenAlgorithm {
  maze_ALGORITHM_TYPE1, // original
  maze_ALGORITHM_TYPE2  // doesn't allow nodes underneath nodes with down connections
};

enum Direction6 {
  NORTH = 0,  
  SOUTH = 1,
  EAST  = 2,
  WEST  = 3,
  UP    = 4,
  DOWN  = 5
};

struct MazeNode {
  int x, y, z;          // entry/exit is 0,0,0, next down is 0,1,0
  int_plusone dir[6];   // n,s,e,w,u,d connections to other nodes (0=none, else connect to dir[x]-1 node)
};

struct Maze {
  int        num_nodes;
  int        max_level;   // deepest level in maze (entry/exit is level 0, next down is level 1)
  // Parameters sent to generate function
  int        adjaceny;    // 4 or 6
  int        min_nodes;
  int        max_nodes;
  int        percent_vertical_connects;           // % of total connects (0=none)
  int        percent_sloping_horizontal_connects; // % of horizontal connects (0=none)
  // Random number type, starting and ending seeds
  RandomType random_type;            
  unsigned   random_start_seed;
  unsigned   random_end_seed;
  MazeNode *nodes;
};

/*___________________
|
| Functions
|__________________*/

// Creates a 4-adjacent maze
Maze *maze_Init_4Adjacent (
  MazeGenAlgorithm algorithm,                   // type1 or type2
  int     min_nodes, 
  int     max_nodes,
  int     percent_sloping_horizontal_connects,  // % of total connects (0=none)
  Random  rnum,
  char   *opt_text_filename,                    // optional
  char   *opt_bin_filename );                   // optional

// Loads a maze from a binary file
Maze *maze_Init_BinaryFile (char *filename);
// Frees all memory for a maze
void maze_Free (Maze *maze);
// Writes maze to a binary file
void maze_Write_BinaryFile (Maze *maze, char *filename);
// Writes maze to a text file
void maze_Write_TextFile (Maze *maze, char *filename);
