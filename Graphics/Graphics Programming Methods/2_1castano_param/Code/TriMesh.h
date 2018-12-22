/*============================================================================
	Title: TriMesh.h
	Module: Pi/MeshLib
	Author: Ignacio Castaño
	Description: A generic triangle mesh.
	Changes:
		07/10/2002 - Ignacio Castaño
			File added.

============================================================================*/

#ifndef _PI_TRIMESH_H_
#define _PI_TRIMESH_H_

/*----------------------------------------------------------------------------
	Doc:
----------------------------------------------------------------------------*/

/** @file TriMesh.h
 * A generic triangle mesh.
**/


/*----------------------------------------------------------------------------
	Headers:
----------------------------------------------------------------------------*/

#include "Containers.h"
#include "Vector.h"
#include "BBox.h"

#include "ImportMesh.h"




/*----------------------------------------------------------------------------
	Classes:
----------------------------------------------------------------------------*/

/**
 * A generic triangle mesh.
**/
class MESH_API PiTriMesh {
	friend class PiTriMeshObject;
public:

	/** @internal Mesh face. */
	struct Face {
		int v0, v1, v2;
		int e0, e1, e2;

		uint counter;

		REAL area;
		Vec3 normal;
	};

	/** @internal Mesh edge. */
	struct Edge {
		int v0, v1;
		int f0, f1;
	};

	/** @internal Mesh vertex. */
	struct Vertex {
		Vec3 pos;		// position
		Vec3 tex;		// texture coordinate
		Vec3 nor;		// normal
		Vec3 col;		// color
	};



public:

	/** @name Loading methods: */
	//@{
		/** Create a TriMesh from an ImportMesh. */
		bool Open( const IImportMesh * im );
	//@}



	/** @name Manipulation methods: */
	//@{
		/** Flip faces. Do this if the mesh has opposite handness. */
		void FlipFaces( void );

		/** Interchange Y and Z axis.*/
		void InterchangeYZ( void );

		/** Scale mesh to fit the given cube. */
		void FitCube( float dist );

		/** Compute the normals of the object. */
		void ComputeNormals( void );
	//@}



	/** @name Adjacency methods: */
	//@{
		/** Create the adjacency information. */
		void BuildAdjacency( void );

		/** Determine if the mesh is closed. (Each edge is shared by two faces) */
		bool IsClosed( void ) const;
	//@}


	/** @name Parameterization methods: */
	//@{
		/** Conformal parameterization in the sphere. */
		void ConformalSphericalMap( int v0, int v1, int v2 );
	//@}


	/** @name Accessors: */
	//@{
		/** Get the number of faces of this object. */
		int GetFaceNum( void ) const { return face_array.Size(); }

		/** Get the number of edges of this object. */
		int GetEdgeNum( void ) const { return edge_array.Size(); }

		/** Get the number of vertices of this object. */
		int GetVertexNum( void ) const { return vertex_array.Size(); }

		/** Get the given face. */
		const Face & GetFace( int i ) const { return face_array[i]; }

		/** Get the given edge. */
		const Edge & GetEdge( int i ) const { return edge_array[i]; }

		/** Get the given vertex. */
		const Vertex & GetVertex( int i ) const { return vertex_array[i]; }

		/** Get the index of the smallest vertex in the given axis. */
		int GetLowestVertex( int axis );

		/** Get the index of the middle vertex in the given axis. */
		int GetMediumVertex( int axis );

		/** Get the index of the greatest vertex in the given axis. */
		int GetHighestVertex( int axis );
	//@}


protected:

	/** Create edge array. */
	void BuildEdges( void );

	/** Weld edges. */
	int WeldEdges( PiArray<Edge> &p, int * xrefs, int N );

	/** The the hash value of an edge. */
	uint32 GetEdgeHashValue( const Edge &e ) {
		//return (e.v0 ^ e.v1);
		return (e.v0 + e.v1) * 1103515245 + 12345;
	}

	/** Return true if the edges are the same. */
	bool EdgeEquiv( const Edge &e0, const Edge &e1 ) {
		return (e0.v0==e1.v1) && (e0.v1==e1.v0);
	}

	/** Compute the bounding box. */
	void ComputeBox( void );




protected:

	PiArray<Face>	face_array;		///< Array of faces.
	PiArray<Edge>	edge_array;		///< Array of edges.
	PiArray<Vertex>	vertex_array;	///< Array of vertices.

	BBox bounds;					///< Bounding box.

	uint counter;					///< @internal

};




#endif // _PI_TRIMESH_H_
