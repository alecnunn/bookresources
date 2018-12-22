Vertex Noise Demo
=================

Keyboard controls.
+/-      :  increase/decrease the noise frequency
1/2      :  increase/decrease the noise amplitude
w,W	   :  wireframe mode

Explanation
-----------
This shader permutes the vertex positions based on three dimensional Perlin
noise.  The table permutations and gradients is precomputed at start up time
and is passed as a uniform constant to the vertex shader.  This same sort of
noise could equally be used to create procedural textures.

(Used directly with very minimal changes from NVIDIA's Vertex Noise shader
on the cgshaders.org website)

Changes from CgToWm3
--------------------
No changes were made to the output files from CgToWm3.  Only TestVertexNoise.h and TestVertex.cpp were added.