/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: ReadMe.txt
 *    Desc: Info on the chapter 4 code
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 ******************************************************************/

The chapter 4 workspace has the following projects
PathPlan (Application)
neuralNet (Application)
potentialFunc (Application)
GameLib (Library)

/******************************************************************/
Application Name: PathPlan

Application Description: 
    This code uses Dijkstra's algorithm to navigate around a 
    creature around a simple world.  See the 'about' dialog for
    information as to how to operate the program during runtime.

Executable flags:
    None.

/******************************************************************/
Application Name: neuralNet

Application Description: 
    Loads a .nn file (provided as a command line argument 
    input) and then allows the user to provide inputs to 
    the network, giving the resultant output.

Executable flags:

Flag                  Desc
====                  ====
filepath              Run using the file at "filepath"
                      the file should be of type .nn, there
                      are some in the bin\media directory.

/******************************************************************/
Application Name: PotentialFunc

Application Description: 
    This application uses potential functions to guide units
    to a goal while avoiding obstacles.  See the 'about' 
    dialog for info as to how to operate the program during
    runtime.

Executable flags:
    None.
