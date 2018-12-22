    Dstar Demo README
---------------------------

The window represents a graph, circles are the vertices, while lines are the edges between them.
For explaining purpose, this is a 4-connected grid, so no diagonal movement are allowed.

First of all the user should go to Operation->Setup Map command, and enter the side
of the map.

-Start is green vertex at upper left corner
-Goal is red vertex at lower right corner
-Agent position (initially at 0,0) is depicted in yellow

The presence of an edge means that the agent can pass through it, while its absence
shows that there is an edge with an high cost.

Note that agent will cross even over high cost edges if there's no path, but the are
methods to query for the total cost path. If this is higher than OBSTACLE value, it means 
that there's no path.

The command Operations->Set Obstacle allows the user to modify in any moment the environment
by simply clicking over an edge to delete it.

Operations->Set Clear is the exact inverse of the above command.

Operations->Step moves the agent of one position.

Operations->Play simply runs one Step each second, a right click of the mouse, pause the agent.

After the agent has moved, any modification in the environment forced it to re-plan the
path, re-examined vertices are coloured in blue. So at each modification the user can 
quickly look at the amount of work done by the algorithm.

Operations->Clear Map, simply cleans the window.