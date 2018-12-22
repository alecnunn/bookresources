Skinning Demo
=============

Explanation
-----------
This application demonstrates the use of a vertex shader to do skinning
transformations on the graphics card.  It loads four bones (i.e. matrix
transformations) as uniform constants to the vertex shader and passes
the weights in as texture coordinates.  In the case of this program, the
bone matrices are created on the fly, but they could easily be loaded from
some animation file.  The vertex shader transforms the point by each matrix
and then does a linear combination of the transformed points with the
weights that were passed in.  Now that graphics cards have an increasingly
large number of constants, a much larger number of bones could be used.
With a high number of bones, it would also be possible to pass in indices
to matrices and weights and transform each vertex with some number of bones
from a much larger cache of them.  This would be good for a model with both
a large number of vertices and a large number of bones because it would
enable the shader to use the same set of bone matrices for all of the
vertices.

Changes from CgToWm3
--------------------
A constructor was added to take the two sets of texture coordinates and the 
colors as parameters.