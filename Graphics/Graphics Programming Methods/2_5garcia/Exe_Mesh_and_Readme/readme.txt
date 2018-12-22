/****************************************/
/* An easy-to-code smoothing algorithm  */
/* for 3D reconstructed surfaces        */
/*                                      */
/* Author: Oscar Garcia-Panyella        */
/* Date: January 2003                   */
/* Contact: oscarg@salleurl.edu         */
/****************************************/

Requirements
------------

The demo program is OpenGL-based. The interface is GLUT-based.
Then GLUT should be conveniently installed before the execution of the demo.

The input mesh must follow a required format. In a plain text file:

number of vertexs (int)
number of triangles (int)

list of vertexs coordinates (3 floats XYZ)
list of triangles (3 integers with the 3 associated vertex indexes)

Take a look at the provided mesh for more detail (Thanks to Eloi Castells for the mesh).

Usage
-----

From the Windows Explorer, take the mouse and throw the "mesh.txt" file over the smooth.exe file

or

Use the command line. Three ways are considered:

smooth input_mesh_file (where default tolerance and alfa values are provided)

or

smooth input_mesh_file tolerance (where default alfa value is provided)

or

smooth input_mesh_file tolerance alfa

Tolerance and alfa are expected to be float values.

Interaction
-----------
The mesh can be moved with the mouse by using its left button (virtual trackball effect).
Press 's or S' repeatedly for executing the algorithm
Press 'a or A' for resetting the mesh
Press 'e or E' for exitting the demo