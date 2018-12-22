/*============================================================================
	Title: ImportPLY.h
	Module: Pi/MeshLib
	Author: Ignacio Castaño
	Description: A PLY mesh importer.
	Changes:
		07/10/2002 - Ignacio Castaño
			File added.

============================================================================*/

#ifndef _PI_IMPORTPLY_H_
#define _PI_IMPORTPLY_H_

/*----------------------------------------------------------------------------
	Doc:
----------------------------------------------------------------------------*/

/** @file ImportPLY.h
 * A PLY mesh importer.
**/


/*----------------------------------------------------------------------------
	Headers:
----------------------------------------------------------------------------*/

#include "ImportMesh.h"



/*----------------------------------------------------------------------------
	Functions:
----------------------------------------------------------------------------*/




/*----------------------------------------------------------------------------
	Classes:
----------------------------------------------------------------------------*/

/**
 * A PLY mesh importer.
**/
class PiImportPly : public PiImportMesh {
	PI_DISABLE_COPY(PiImportPly);
	PiImportPly( const char * name );
public:

	MESH_API static IImportMesh * Create( const char * name );
	
	virtual ~PiImportPly( void );

	virtual bool Import( void );

protected:

	bool ReadType( FILE * fp, enum PlyType &type );
	bool ReadFloatValue( FILE * fp, int format, int type, float &val );
	bool ReadIntValue( FILE * fp, int format, int type, int &val );


public:
	
	char * name;

	char token[1024];

};




#endif // _PI_IMPORTPLY_H_
