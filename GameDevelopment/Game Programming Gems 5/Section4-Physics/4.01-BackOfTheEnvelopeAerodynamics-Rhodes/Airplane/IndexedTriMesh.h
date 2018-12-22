/*****************************************************************************
|
| IndexedTriMesh.h
|
| by Graham S. Rhodes
| September 2004
|
| Header declaring a *very* simple class to support drawing indexed triangle
| meshes via OpenGL.
|
| This class supports companion examples to the chapter titled "Back of the
| Envelope Aerodynamics for Game Physics" in the book "Game
| Programming Gems 5" published by Charles River Media
|
*******************************************************************************/

#ifndef _indexedtrimesh_h
#define _indexedtrimesh_h

class IndexedTriMesh
{
	public:
		IndexedTriMesh();
		~IndexedTriMesh();

		void Draw();

		bool Load(FILE *pIn);

	protected:
		void Reset();

	public:
		unsigned int m_uiNumVerts;					// Number of vertices
		unsigned int m_uiNumIndices;				// Number of indices
		float *m_pfVertCoords;						// array of vertex coordinates, 3 entries for each vertex
		float *m_pfVertNormals;						// array of vertex normals, 3 entries for each vertex
		float *m_pfVertTexCoords;					// array of texture coordinates, 2 entries for each vertex
		unsigned int *m_puiIndices;					// array of indices defining the mesh, 3 entries for each triangle
		float m_pfColor[4];							// Color for the mesh.
		float m_pfLocation[3];						// Location in parent space
		float m_fYRotation;							// Rotation about local y axis, radians;
};

#endif // _indexedtrimesh_h
