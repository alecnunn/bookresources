Source: Octree Construction (Game Programming Gems) Support Code
Author: Dan Ginsburg
================================================================

The following source files are included to supplment my "Octree Construction"
Gem.  The purpose of the code is to provide an example of how an octree
can be constructed.  The Octree class takes an array of triangles as input and
internally generates octree data.  The first step is to recursively subdivide
the tree into octants until each node has a threshold number of polygons.
Each node is given array indices into an index table that points to the triangles
within the node.  

After building the tree, a second step is run that determines the neighbors to
each node.  The Octree class has a "Save()" method which demonstrates how the Octree 
can easily be saved to a file.  Additionally, an EXTREMELY simple sample program 
is provided which demonstrates construction of the octree with random triangles.  

	Octree.cpp/.h:

		Provides a class for taking a list of triangles and
		partitioning them into an Octree.  

	Support files:

	BoxSide.cpp/.h:

		Class for computing the side of a box and testing whether two
		box sides are neighbors.  This class is used by the Octree class
		for finding neighbors

	List.cpp/.h

		General list class - used to store Octree node list and list
		of triangle indices per node

	TriBox.cpp/.h
	
		Adapted from Grahics Gems III source code 'triangleCube.c'.
		Used by the Octree class to determine whether a triangle
		intersects a node's bounding cube.
	
	Test.cpp
	
		Simple test program

Sample App (requires GLUT32.DLL)
==========

Keyboard commands:

'T':	Display entire tree and all polys
'U':	Move up a node, showing only its neighbors and children
'D':	Moe down a node, showing only its neighbors and children

