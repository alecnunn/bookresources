
Automatic Parameterizations on the Cube
---------------------------------------

This program computes an automatic parametrization of the example mesh. The mesh must be genus zero, and the parameterization domain is a cube instead of a regular texture. Using this parameterization the example shows how to precompute lighting and other shading computations. This can also be used to compute a normal map of this dense mesh, that can later be used on a lower resolution mesh.


- PAR.h, PAR.cpp
Our main program. Uses OpenGL and glut for displaying things.

- TriMesh.cpp, TriMesh.h
Class that represents our triangle mesh. Computes the spherical conformal map.

- Sparse.h, Sparse.cpp
Sparse matrix solver using the conjugate gradient method.

- Raster.h, Raster.cpp, Image.cpp
Simple affine rasterization library.

- ImportPLY.h, ImportPLY.cpp:
A ply file loader. The PLY file format is usually used for high resolution meshes produced by 3d scanners.

- Containers.h
Simple containers, based on the code of Thatcher Ulrich.

- Vector.h, Matrix.h, BBox.h
Some usefull math tools.

- RenderMan.h, RenderMan.cpp
Some math functions that emulate the functions of the renderman api.


Ignacio Castaño
castanyo@yahoo.es
