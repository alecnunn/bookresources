/*============================================================================
	Title: ImportMesh.h
	Module: Pi/MathLib
	Author: Ignacio Castaño
	Description: A base mesh importer.
	Changes:
		07/10/2002 - Ignacio Castaño
			File added.

============================================================================*/

#ifndef _PI_IMPORTMESH_H_
#define _PI_IMPORTMESH_H_

/*----------------------------------------------------------------------------
	Doc:
----------------------------------------------------------------------------*/

/** @file ImportMesh.h
 * A base mesh importer.
**/


/*----------------------------------------------------------------------------
	Headers:
----------------------------------------------------------------------------*/

//#include "MeshLib.h"
#include "Containers.h"
#include "Vector.h"

// From MeshLib
#define MESH_API


/*----------------------------------------------------------------------------
	Template instantiation:
----------------------------------------------------------------------------*/

// template class MESH_API PiArray<Vec3>;
// template class MESH_API PiArray<PiImportMesh::Face>;



/*----------------------------------------------------------------------------
	Classes:
----------------------------------------------------------------------------*/


class MESH_API IImportMesh {
public:

	virtual ~IImportMesh() { };

	/** Virtual Import function to be implemented by child classes. */
	virtual bool Import( void )=0;

	/** Get number of vertices. */
	virtual int GetVertexNum( void ) const =0;

	/** Get number of faces. */
	virtual int GetFaceNum( void ) const =0;

	/** Get vertex position. */
	virtual const Vec3 & GetVertexPosition( int i ) const =0;

	/** Get vertex normal. */
	virtual const Vec3 & GetVertexNormal( int i ) const =0;

	/** Get vertex texture coordinate. */
	virtual const Vec3 & GetVertexTexCoord( int i ) const =0;

	/** Get vertex color. */	
	virtual const Vec3 & GetVertexColor( int i ) const =0;

	/** Get face index. */
	virtual int GetFaceIndex( int i, int v ) const =0;
};


/**
 * Base mesh importer.
**/
class PiImportMesh : public IImportMesh {
	PI_DISABLE_COPY(PiImportMesh);
	friend class PiTriMesh;
public:

	/** Default ctor. */
	PiImportMesh() { vertex_has = 0; };

	/** Virtual dtor. */
	virtual ~PiImportMesh() { };

	/** Virtual Import function to be implemented by child classes. */
	virtual bool Import( void )=0;

	/** Get number of vertices. */
	int GetVertexNum( void ) const { return pos_array.Size(); }

	/** Get number of faces. */
	int GetFaceNum( void ) const { return face_array.Size(); }

	/** Get vertex position. */
	const Vec3 & GetVertexPosition( int i ) const {
		if( vertex_has & IMF_HAS_POSITION ) return pos_array[i];
		else return Vec3Origin;
	}

	/** Get vertex normal. */
	const Vec3 & GetVertexNormal( int i ) const {
		if( vertex_has & IMF_HAS_NORMAL ) return nor_array[i];
		else return Vec3Origin;
	}

	/** Get vertex texture coordinate. */
	const Vec3 & GetVertexTexCoord( int i ) const {
		if( vertex_has & IMF_HAS_TEX ) return tex_array[i];
		else return Vec3Origin;
	}

	/** Get vertex color. */	
	const Vec3 & GetVertexColor( int i ) const {
		if( vertex_has & IMF_HAS_COLOR ) return col_array[i];
		else return Vec3Origin;
	}

	/** Get face index. */
	int GetFaceIndex( int i, int v ) const {
		return ((int *)&face_array[i])[v];
	}



protected:
	
	/** @internal Face structure. */
	struct Face {
		int v0, v1, v2;
	};

	/** @internal Import Mesh Flags. */
	enum {
		IMF_HAS_POSITION	= 0x01,
		IMF_HAS_NORMAL		= 0x02,
		IMF_HAS_TEX			= 0x04,
		IMF_HAS_COLOR		= 0x08
	};

	uint vertex_has;


	PiArray<Vec3>	pos_array;		///< Vertex position array.
	PiArray<Vec3>	nor_array;		///< Vertex normal array.
	PiArray<Vec3>	tex_array;		///< Vertex texcoord array.
	PiArray<Vec3>	col_array;		///< Vertex color array.

	PiArray<Face>	face_array;		///< Array of faces.

};



#endif // _PI_IMPORTMESH_H_
