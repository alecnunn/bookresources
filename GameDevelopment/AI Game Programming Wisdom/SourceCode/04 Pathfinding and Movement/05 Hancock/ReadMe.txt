
This is a skeleton of the code necessary to implement some of the
ideas presented in the article:

"Navigating Doors, Elevators, Ledges and Other Obstacles." 

Author: John A. Hancock


Since the entire navigation system depends on many subsystems which
I could not include here, not all of the code provided will compile
unless at least stub functions are provided for some calls. Nevertheless,
the code should provide a good starting point for your own implementations
of versatile navigation code.

NodeAndLink.h and NodeAndLink.cpp include a basic implementation
of the PathNode and PathLink classes that make up the nodemap.
If memory is tight, some of the class members used for annotation
or path-planning (such as the std::string name, the elevatorFloor,
and the mutable pathfinding members) should be separated from the
essential members.

ExtractRegions.cpp provides the functions necessary to implement
the connected component extraction outlined towards the end of the article.
NodeMap::ExtractRegions is the master function which generates 4 types of
regions for each node. GenerateRegions labels all the nodes with the
proper region type based on one-way and connectivity predicate function
objects and adds one-way links to a list. GenerateRegionConnectivity uses
the one-way links to fill in the region connectivity matrix that describes
which regions are reachable from other regions.

Goal.h and Goal.cpp include the code for the basic Goal and GoalQueue classes.
The goal queue is implemented as an Standard Template Library (STL) list.

Goal_FollowLink.h and .cpp illustrate the basics of following an individual
link. The types of links used will likely depend on the type of game, and
code for full implementations will depend on a variety of subsystems.

Goal_GotoNode.h and Goal_GotoNode.cpp are the goals that generate most of
the path-planning requests. Goal_GotoNode calls the AI function
GeneratePathingGoals() which requests an A* search of the nodemap and
GenerateGoalsFromPath() turns the A* path into a series of goals. See
GenerateGoals.cpp for these functions.

Goal_GoThroughDoor.h and .cpp, Goal_RideElevator.h and .cpp, and
Goal_HitSwitch.h and .cpp implement basic goals for navigating through
doors and elevators.


