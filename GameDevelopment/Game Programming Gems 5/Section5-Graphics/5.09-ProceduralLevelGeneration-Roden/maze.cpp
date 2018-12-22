/*____________________________________________________________________
|
| File: maze.cpp
|
| Description: Routines to create a random maze, saving it in a file/s.
|
| Functions: maze_Init_4Adjacent
|             Make_Maze_Type1
|             Make_Maze_Type2
|		           Init_Node
|         		 Node_In_Maze
|         		 Remove_From_List
|             maze_Init_BinaryFile
|             maze_Free
|             maze_Write_BinaryFile
|             maze_Write_TextFile
|
| (C) Copyright 2004-2005 Timothy E. Roden.
|___________________________________________________________________*/

/*___________________
|
| Include Files
|__________________*/

#include <windows.h>
#include <winbase.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "list.h"
#include "random.h"
#include "maze.h"

/*___________________
|
| Type defintions
|__________________*/

struct NodeInfo {
  int x, y, z;
};

/*___________________
|
| Function Prototypes
|__________________*/

static bool Make_Maze_Type1 (Maze *maze, Random rnum, int *deepest);
static bool Make_Maze_Type2 (Maze *maze, Random rnum, int *deepest);
static void Init_Node (
  MazeNode *node );
static int Node_In_Maze (
  MazeNode *nodes,
  int       num_nodes,
  int       x,          // coords of node to search for
  int       y,
  int       z );
static void Remove_From_List (
  int *avail_list,
  int *num_avail,
  int node );

/*___________________
|
| Constants
|__________________*/

#undef PERCENT
#define PERCENT(_x_) (random_GetPercent(rnum) <= _x_)

#define HORIZONTAL  1
#define VERTICAL    2

/*____________________________________________________________________
|
| Function: maze_Init_4Adjacent
|
| Input: Called from ___
| Output: Builds a randomly-generated maze.  Returns pointer to the maze
|   or 0 on any error. Optionally, writes maze data to files.
|
| Description: Data written to binary file is in the following format:
|	  Maze struct
|	  MazeNode array (size = maze->num_nodes * sizeof(MazeNode))
|___________________________________________________________________*/

Maze *maze_Init_4Adjacent (
  MazeGenAlgorithm algorithm,                   // type1 or type2
  int     min_nodes, 
  int     max_nodes,
  int     percent_sloping_horizontal_connects,  // % of total connects (0=none)
  Random  rnum,
  char   *opt_text_filename,                    // optional
  char   *opt_bin_filename )                    // optional
{
  bool error, created;
  Maze *maze = 0;

/*____________________________________________________________________
|
| Verify input params
|___________________________________________________________________*/

  assert (min_nodes >= 1);
  assert (max_nodes >= min_nodes);
  assert ((percent_sloping_horizontal_connects >= 0) && (percent_sloping_horizontal_connects <= 100));
  assert (rnum);

/*____________________________________________________________________
|
| Init maze header
|___________________________________________________________________*/

  error = false;

  // Allocate memory for maze header
  maze = (Maze *) malloc (sizeof(Maze));
  if (maze == 0) 
    error = true;
  else {
    // Save input params
    maze->adjaceny  = 4;
    maze->min_nodes = min_nodes;
    maze->max_nodes = max_nodes;
    maze->percent_vertical_connects = 0;
    maze->percent_sloping_horizontal_connects = percent_sloping_horizontal_connects;
    maze->random_type       = random_GetType (rnum);
    maze->random_start_seed = random_GetSeed (rnum);
  }

/*____________________________________________________________________
|
| Create the maze
|___________________________________________________________________*/

  // Compute number of nodes in the maze
  maze->num_nodes = random_GetInt (rnum, min_nodes, max_nodes);
  // Allocate memory for maze node array
  maze->nodes = (MazeNode *) calloc (maze->num_nodes, sizeof(MazeNode));;
  if (maze->nodes == 0) 
    error = true;
  else {
    // Generate the maze
    switch (algorithm) {
      case maze_ALGORITHM_TYPE1: 
        created = Make_Maze_Type1 (maze, rnum, &(maze->max_level));
        break;
      case maze_ALGORITHM_TYPE2:
        created = Make_Maze_Type2 (maze, rnum, &(maze->max_level));
        break;
    }
    if (created)
      maze->random_end_seed = random_GetSeed (rnum);
    else 
      error = true;
  }

/*____________________________________________________________________
|
| Write maze data to files?
|___________________________________________________________________*/

  // Write to text file
  if ((! error) && opt_text_filename) 
    maze_Write_TextFile (maze, opt_text_filename);

  // Write to binary file
  if ((! error) && opt_bin_filename)
    maze_Write_BinaryFile (maze, opt_bin_filename);

/*____________________________________________________________________
|
| On any error, delete the maze
|___________________________________________________________________*/

  if (error) {
    maze_Free (maze);
    maze = 0;
  }

/*____________________________________________________________________
|
| Verify output params
|___________________________________________________________________*/

  assert (maze);

  return (maze);
}

/*____________________________________________________________________
|
| Function: Make_Maze_Type1
|
| Input: Called from maze_Init_4Adjacent()
|	Output: Fills in maze nodes array with values, creating the maze.
|   Returns true if created, else false on any error.
|___________________________________________________________________*/

static bool Make_Maze_Type1 (Maze *maze, Random rnum, int *deepest)
{
  int i, node, num_avail, avail_index, max_connects, connect_node;
  int x, y, z, ydiff, dir, exist_node, connection, *avail_list;
  int xdiff[6] = { 0, 0, 1, -1, 0, 0 }; // n,s,e,w,u,d
  int zdiff[6] = { 1, -1, 0, 0, 0, 0 }; // n,s,e,w,u,d
  Direction6 opposite_dir [6] = {       // opposites from n,s,e,w,u,d
    SOUTH, NORTH, WEST, EAST, DOWN, UP
  };
  bool created = false;

/*____________________________________________________________________
|
| Verify input params
|___________________________________________________________________*/

  assert (maze);
  assert (rnum);
  assert (deepest);

/*____________________________________________________________________
|
| Init first two nodes and avail list
|___________________________________________________________________*/

  if (maze->percent_vertical_connects)
    max_connects = 4;
  else
    max_connects = 3;
  
  // Allocate memory for available nodes list
  avail_list = (int *) malloc (maze->num_nodes * 4 * sizeof(int));
  if (avail_list) {
    // Init topmost node - not used
    Init_Node (&maze->nodes[0]);

    // Connect to node 1 down diagonally?
    if (maze->percent_sloping_horizontal_connects) 
      dir = random_GetInt (rnum, 0, 3);
    // Connect to node 1 straight down
    else 
      dir = DOWN;
    maze->nodes[0].dir[dir] = 2; // 2 is node 1(+1)
    Init_Node (&maze->nodes[1]);
    maze->nodes[1].x = xdiff[dir];
    maze->nodes[1].y = 1;
    maze->nodes[1].z = zdiff[dir];
    maze->nodes[1].dir[opposite_dir[dir]] = 1;  // 1 is node 0(+1)
    // Put node 1 in avail list
    for (num_avail=0; num_avail<max_connects; num_avail++)
      avail_list[num_avail] = 1;

/*____________________________________________________________________
|
| Main loop
|___________________________________________________________________*/

    *deepest = 1;

    for (node=2; node < maze->num_nodes; node++) {
      // Initialize node to connect to maze
      Init_Node (&maze->nodes[node]);
                                                
      // Pick a node to connect to
      avail_index = random_GetInt (rnum, 0, num_avail-1);
      avail_index = random_GetInt (rnum, avail_index/2, num_avail-1); // prefer more recently created nodes
      connect_node = avail_list[avail_index];

      connection = 0;          

      // Connect vertically?
      if (maze->percent_vertical_connects)
        if (PERCENT(maze->percent_vertical_connects))  
          // Make sure a vertical connection is possible from connect_node (doesn't already have a vert connection)
          if ((maze->nodes[connect_node].dir[UP]   == 0) && 
              (maze->nodes[connect_node].dir[DOWN] == 0))
            connection = VERTICAL;
          // If not possible, then connect horizontally
          else
            connection = HORIZONTAL;

      // Connect horizontally?
      if (connection == 0)
        // Make sure a horizontal connection is possible from connect_node
        if ((maze->nodes[connect_node].dir[NORTH] == 0) || 
            (maze->nodes[connect_node].dir[SOUTH] == 0) ||
            (maze->nodes[connect_node].dir[EAST]  == 0) ||
            (maze->nodes[connect_node].dir[WEST]  == 0))
          connection = HORIZONTAL;
        // If not possible, then connect vertically
        else if (maze->percent_vertical_connects)
          connection = VERTICAL;

      assert (connection);

/*____________________________________________________________________
|
| Connect new node to connect_node horizontally.
|___________________________________________________________________*/

      if (connection == HORIZONTAL) {
	      // Pick a direction (n,s,e,w) without a connection already
	      do {
	        dir = random_GetInt (rnum, 0, 3);
        } while (maze->nodes[connect_node].dir[dir]);

        ydiff = 0;

        // Make this horizontal connection sloping?
        if (maze->percent_sloping_horizontal_connects)
          if (PERCENT(maze->percent_sloping_horizontal_connects))
            // 10% of the time, they slope upward, else down  
	          if (PERCENT(10) && (maze->nodes[connect_node].y > 1))
		          ydiff = -1;
	          else
		          ydiff = 1;
      } 

/*____________________________________________________________________
|
| Connect new node to connect_node vertically, if not already a vertical
| connection to connect_node.
|___________________________________________________________________*/

      else {  // connection == VERTICAL
        // 10% of the time, connect upward, else down
	      if (PERCENT(10) && (maze->nodes[connect_node].y > 1)) {
	        dir = UP;
	        ydiff = -1;
        }
	      else {
	        dir = DOWN;
	        ydiff = 1;
        }
      }

/*____________________________________________________________________
|
| Place node at new position, if not already a node at that position.
| If an existing node is found at new position, possibly connect the
| existing node with the connect_node.  In that case, reuse new node.
|___________________________________________________________________*/

      // Set coords of newnode
      x = maze->nodes[connect_node].x + xdiff[dir];
      y = maze->nodes[connect_node].y + ydiff;
      z = maze->nodes[connect_node].z + zdiff[dir];

      // Does a node already exist in the new node position?
      exist_node = Node_In_Maze (maze->nodes, node, x, y, z);
      if (exist_node) {
	      // Connect two existing nodes (sometimes) if possible
	      if (PERCENT(20) && (! maze->nodes[exist_node-1].dir[opposite_dir[dir]])) {
	        maze->nodes[connect_node].dir[dir] 		           = exist_node;
	        maze->nodes[exist_node-1].dir[opposite_dir[dir]] = connect_node + 1;
	        Remove_From_List (avail_list, &num_avail, exist_node-1);
	        Remove_From_List (avail_list, &num_avail, connect_node);
	      }
	      node--; // Reuse new node
      }
      else { // connect new node to connect_node
	      maze->nodes[connect_node].dir[dir]       = node + 1;
	      maze->nodes[node].dir[opposite_dir[dir]] = connect_node + 1;
	      maze->nodes[node].x                      = x;
	      maze->nodes[node].y                      = y;
	      maze->nodes[node].z                      = z;
	      Remove_From_List (avail_list, &num_avail, connect_node);
	      for (i=0; i<max_connects; i++)
	        avail_list[num_avail++] = node;
	      if (y > *deepest)
	        *deepest = y;
      }
    } // main loop

    created = true;

/*____________________________________________________________________
|
| Free memory for avail list and exit
|___________________________________________________________________*/
  
    if (avail_list)
      free (avail_list);
  } // if (avail_list)

/*____________________________________________________________________
|
| Verify output params
|___________________________________________________________________*/

  assert (created);

  return (created);
}

/*____________________________________________________________________
|
| Function: Make_Maze_Type2
|
| Input: Called from maze_Init_4Adjacent()
|	Output: Fills in maze nodes array with values, creating the maze.
|   Returns true if created, else false on any error.
|
|   This version of the algorithm doesn't allow nodes to be created
|   undernearth nodes who have a connection to a node that is lower.
|
|   Don't use any vertical_connects with this algorithm - should be
|   set to 0.  Always use sloping_horizontal connects - should be
|   nonzero.
|___________________________________________________________________*/

#define ADD_OFF_LIMITS(_x_,_y_,_z_)               \
{                                                 \
  ninfo.x = _x_;                                  \
  ninfo.y = _y_;                                  \
  ninfo.z = _z_;                                  \
  list_AddEnd (off_limits_nodes, (void *)&ninfo); \
}

static bool Make_Maze_Type2 (Maze *maze, Random rnum, int *deepest)
{
  int i, node, num_avail, avail_index, max_connects, connect_node;
  int x, y, z, ydiff, dir, exist_node, connection, *avail_list;
  int xdiff[6] = { 0, 0, 1, -1, 0, 0 }; // n,s,e,w,u,d
  int zdiff[6] = { 1, -1, 0, 0, 0, 0 }; // n,s,e,w,u,d
  NodeInfo ninfo; 
  Direction6 opposite_dir [6] = {       // opposites from n,s,e,w,u,d
    SOUTH, NORTH, WEST, EAST, DOWN, UP
  };
  List off_limits_nodes;                // list of nodes can't build in
  bool created = false;

/*____________________________________________________________________
|
| Verify input params
|___________________________________________________________________*/

  assert (maze);
  assert (rnum);
  assert (deepest);

/*____________________________________________________________________
|
| Init first two nodes and avail list and off_limits list
|___________________________________________________________________*/

  max_connects = 3;
  
  // Init off_limits list
  off_limits_nodes = list_Init (sizeof(NodeInfo));
  // Allocate memory for available nodes list
  avail_list = (int *) malloc (maze->num_nodes * 4 * sizeof(int));
  if (avail_list && off_limits_nodes) {
    // Init topmost node - not used
    Init_Node (&maze->nodes[0]);
    ADD_OFF_LIMITS (0, 1, 0)

    // Connect to node 1 down diagonally
    dir = random_GetInt (rnum, 0, 3);

    maze->nodes[0].dir[dir] = 2; // 2 is node 1(+1)
    Init_Node (&maze->nodes[1]);
    x = xdiff[dir];
    y = 1;
    z = zdiff[dir];
    maze->nodes[1].x = x;
    maze->nodes[1].y = y;
    maze->nodes[1].z = z;
    maze->nodes[1].dir[opposite_dir[dir]] = 1;  // 1 is node 0(+1)
    // Put node 1 in avail list
    for (num_avail=0; num_avail<max_connects; num_avail++)
      avail_list[num_avail] = 1;
    // Set node under node 1 to off limits
    ADD_OFF_LIMITS (x, y+1, z);

/*____________________________________________________________________
|
| Main loop
|___________________________________________________________________*/

    *deepest = 1;

    for (node=2; node < maze->num_nodes; node++) {
      // Initialize node to connect to maze
      Init_Node (&maze->nodes[node]);
                                                
      // Pick a node to connect to
      avail_index = random_GetInt (rnum, 0, num_avail-1);
      avail_index = random_GetInt (rnum, avail_index/2, num_avail-1); // prefer more recently created nodes
      connect_node = avail_list[avail_index];

      connection = 0;          

      // Connect horizontally?
      if (connection == 0)
        // Make sure a horizontal connection is possible from connect_node
        if ((maze->nodes[connect_node].dir[NORTH] == 0) || 
            (maze->nodes[connect_node].dir[SOUTH] == 0) ||
            (maze->nodes[connect_node].dir[EAST]  == 0) ||
            (maze->nodes[connect_node].dir[WEST]  == 0))
          connection = HORIZONTAL;

      assert (connection);

/*____________________________________________________________________
|
| Connect new node to connect_node horizontally.
|___________________________________________________________________*/

      if (connection == HORIZONTAL) {
	      // Pick a direction (n,s,e,w) without a connection already
	      do {
	        dir = random_GetInt (rnum, 0, 3);
        } while (maze->nodes[connect_node].dir[dir]);

        ydiff = 0;

        // Make this horizontal connection sloping?
        if (maze->percent_sloping_horizontal_connects)
          if (PERCENT(maze->percent_sloping_horizontal_connects))
            // 10% of the time, they slope upward, else down  
	          if (PERCENT(10) && (maze->nodes[connect_node].y > 1))
		          ydiff = -1;
	          else
		          ydiff = 1;
      } 

/*____________________________________________________________________
|
| Place node at new position, if not already a node at that position.
| If an existing node is found at new position, possibly connect the
| existing node with the connect_node.  In that case, reuse new node.
|___________________________________________________________________*/

      // Set coords of newnode
      x = maze->nodes[connect_node].x + xdiff[dir];
      y = maze->nodes[connect_node].y + ydiff;
      z = maze->nodes[connect_node].z + zdiff[dir];

      // Is this node off limits?
      ninfo.x = x;
      ninfo.y = y;
      ninfo.z = z;
      if (list_ContainsItem (off_limits_nodes, (void *)&ninfo))
        node--; // Reuse new node
      // Is there already a node below the new intended position?
      else if (Node_In_Maze (maze->nodes, node, x, y+1, z))
        node--; // Reuse new node
      else {
        // Does a node already exist in the new node position?
        exist_node = Node_In_Maze (maze->nodes, node, x, y, z);
        if (exist_node) {
	        // Connect two existing nodes (sometimes) if possible
	        if (PERCENT(20) && (! maze->nodes[exist_node-1].dir[opposite_dir[dir]])) {
	          maze->nodes[connect_node].dir[dir] 		           = exist_node;
	          maze->nodes[exist_node-1].dir[opposite_dir[dir]] = connect_node + 1;
	          Remove_From_List (avail_list, &num_avail, exist_node-1);
	          Remove_From_List (avail_list, &num_avail, connect_node);
	        }
	        node--; // Reuse new node
        }
        else { // connect new node to connect_node
	        maze->nodes[connect_node].dir[dir]       = node + 1;
	        maze->nodes[node].dir[opposite_dir[dir]] = connect_node + 1;
	        maze->nodes[node].x                      = x;
	        maze->nodes[node].y                      = y;
	        maze->nodes[node].z                      = z;
          ADD_OFF_LIMITS (x, y+1, z)
	        Remove_From_List (avail_list, &num_avail, connect_node);
	        for (i=0; i<max_connects; i++)
	          avail_list[num_avail++] = node;
	        if (y > *deepest)
	          *deepest = y;
        }
      }
    } // main loop

    created = true;

/*____________________________________________________________________
|
| Free memory
|___________________________________________________________________*/
  
    // Free available list
    if (avail_list)
      free (avail_list);
    // Free off_limits list
    list_Free (off_limits_nodes);

  } // if (avail_list AND off_limits_nodes)

/*____________________________________________________________________
|
| Verify output params
|___________________________________________________________________*/

  assert (created);

  return (created);
}

#undef ADD_OFF_LIMITS

/*____________________________________________________________________
|
| Function: Init_Node
|
| Input: Called from Make_Maze()
| Output: Initializes a MazeNode struct.
|___________________________________________________________________*/

static void Init_Node (MazeNode *node)
{
  int i;

/*____________________________________________________________________
|
| Verify input params
|___________________________________________________________________*/

  assert (node);

/*____________________________________________________________________
|
| Zero out all elements of the struct
|___________________________________________________________________*/

  for (i=0; i<6; i++)
    node->dir[i] = 0;
  node->x = 0;
  node->y = 0;
  node->z = 0;
}

/*____________________________________________________________________
|
| Function: Node_In_Maze
|
| Input: Called from Make_Maze()
| Output: Returns index+1 if node is in maze.
|___________________________________________________________________*/

static int Node_In_Maze (
  MazeNode *nodes,
  int       num_nodes,
  int       x,          // coords of node to search for
  int       y,
  int       z )
{
  int i, result = 0;

/*____________________________________________________________________
|
| Verify input params
|___________________________________________________________________*/

  assert (nodes);

/*____________________________________________________________________
|
| Main procedure
|___________________________________________________________________*/

  for (i=1; i<num_nodes; i++)
    if ((nodes[i].x == x) && (nodes[i].y == y) && (nodes[i].z == z)) {
  	  result = i + 1;
   	  break;
   	}

  return (result);
}

/*____________________________________________________________________
|
| Function: Remove_From_List
|
| Input: Called from Make_Maze()
| Output: Removes a node index from avail list.
|___________________________________________________________________*/

static void Remove_From_List (
  int *avail_list,  // array of indexes
  int *num_avail,   // # in array
  int  node )       // index to remove
{
  int i;

/*____________________________________________________________________
|
| Verify input params
|___________________________________________________________________*/

  assert (avail_list);
  assert (num_avail);

/*____________________________________________________________________
|
| Main procedure
|___________________________________________________________________*/

  // Find index to remove
  for (i=0; i<*num_avail; i++)
    if (avail_list[i] == node) {
      avail_list[i] = avail_list[*num_avail-1];
      *num_avail = *num_avail - 1;
      break;
    }
}

/*____________________________________________________________________
|
| Function: maze_Init_BinaryFile
|
| Output: Loads a maze from a binary file.  Returns pointer to maze or
|   0 on any error.
|___________________________________________________________________*/

// Loads a maze from a binary file
Maze *maze_Init_BinaryFile (char *filename)
{
  int error;
  FILE *infile;
  Maze *maze = 0;

/*____________________________________________________________________
|
| Verify input params
|___________________________________________________________________*/

  assert (filename);

/*____________________________________________________________________
|
| Init maze header
|___________________________________________________________________*/

  error = false;

  // Allocate memory for maze header
  maze = (Maze *) malloc (sizeof(Maze));
  if (maze == 0) 
    error = true;

/*____________________________________________________________________
|
| Main procedure
|___________________________________________________________________*/

  if (! error) {
    // Open infile file
    infile = fopen (filename, "rb");
    if (infile == 0) 
      error = true;
    else {
      // Read data from file
      fread (maze, sizeof(Maze), 1, infile);
      // Allocate memory for maze node array
      maze->nodes = (MazeNode *) malloc (maze->num_nodes * sizeof(MazeNode));
      if (maze->nodes == 0) 
        error = true;
      else 
        fread (maze->nodes, sizeof (MazeNode), maze->num_nodes, infile);
      fclose (infile);
    }
  }

/*____________________________________________________________________
|
| On any error, delete the maze
|___________________________________________________________________*/

  if (error) {
    maze_Free (maze);
    maze = 0;
  }

/*____________________________________________________________________
|
| Verify output params
|___________________________________________________________________*/

  assert (maze);

  return (maze);
}

/*____________________________________________________________________
|
| Function: maze_Free
|
| Output: Deletes all memory for maze.
|___________________________________________________________________*/

void maze_Free (Maze *maze)
{

/*____________________________________________________________________
|
| Verify input params
|___________________________________________________________________*/

  assert (maze);

/*____________________________________________________________________
|
| Main procedure
|___________________________________________________________________*/

  if (maze) {
    if (maze->nodes)
      free (maze->nodes);
    free (maze);
  }
}

/*____________________________________________________________________
|
| Function: maze_Write_BinaryFile
|
| Output: Writes maze to a binary file.  Writes Maze struct followed by
|   MazeNode array.
|___________________________________________________________________*/

void maze_Write_BinaryFile (Maze *maze, char *filename)
{
  FILE *outfile;

/*____________________________________________________________________
|
| Verify input params
|___________________________________________________________________*/

  assert (maze);
  assert (filename);

/*____________________________________________________________________
|
| Main procedure
|___________________________________________________________________*/

  // Open outfile file
  outfile = fopen (filename, "wb");
  if (outfile) {
    // Write data to file
    fwrite (maze, sizeof(Maze), 1, outfile);
    fwrite (maze->nodes, sizeof (MazeNode), maze->num_nodes, outfile);
    fclose (outfile);
  }
}

/*____________________________________________________________________
|
| Function: maze_Write_TextFile
|
| Output: Writes maze to a text file.
|___________________________________________________________________*/

// Writes maze to a text file
void maze_Write_TextFile (Maze *maze, char *filename)
{
  int i, j;
  FILE *outfile;
  char *directions[6] = { "(N)", "(S)", "(E)", "(W)", "(U)", "(D)" };

/*____________________________________________________________________
|
| Verify input params
|___________________________________________________________________*/

  assert (maze);
  assert (filename);

/*____________________________________________________________________
|
| Main procedure
|___________________________________________________________________*/

  // Open outfile file
  outfile = fopen (filename, "wt");
  if (outfile) {
    // Write maze header struct
    fprintf (outfile, "num_nodes: %d\n", maze->num_nodes);
    fprintf (outfile, "max_level: %d\n", maze->max_level);
    fprintf (outfile, "adjacency: %d\n", maze->adjaceny);
    fprintf (outfile, "min_nodes: %d\n", maze->min_nodes);
    fprintf (outfile, "max_nodes: %d\n", maze->max_nodes);
    fprintf (outfile, "percent_vertical_connects: %d\n", maze->percent_vertical_connects);
    fprintf (outfile, "percent_sloping_horizontal_connects: %d\n", maze->percent_sloping_horizontal_connects);
    fprintf (outfile, "random_type: %d\n", maze->random_type);
    fprintf (outfile, "random_start_seed: %u\n", maze->random_start_seed);
    fprintf (outfile, "random_end_seed: %u\n", maze->random_end_seed);
    fprintf (outfile, "\n");
    // Write maze nodes array
    for (i=0; i<maze->num_nodes; i++) {
      fprintf (outfile, "Node: %d\n", i);
      fprintf (outfile, "  location (x,y,z): %d,%d,%d\n", maze->nodes[i].x, maze->nodes[i].y, maze->nodes[i].z);
      fprintf (outfile, "  connected to nodes: ");
      for (j=0; j<6; j++) {
        if (maze->nodes[i].dir[j])
          fprintf (outfile, "%d%s ", maze->nodes[i].dir[j]-1, directions[j]);
      }
      fprintf (outfile, "\n");
    }
    fclose (outfile);
  }
}
