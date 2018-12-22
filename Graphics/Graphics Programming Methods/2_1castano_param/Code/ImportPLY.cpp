/*============================================================================
	Title: ImportPly.cpp
	Module: Pi/MathLib
	Author: Ignacio Castaño
	Description: A PLY mesh importer.
	Changes:
		07/10/2002 - Ignacio Castaño
			File added.

============================================================================*/

/*----------------------------------------------------------------------------
	Doc:
----------------------------------------------------------------------------*/

/** @file ImportPLY.cpp
 * A PLY mesh importer.
**/


/*----------------------------------------------------------------------------
	Includes:
----------------------------------------------------------------------------*/

#include "ImportPLY.h"
#include "PAR.h"



/*----------------------------------------------------------------------------
	Structs:
----------------------------------------------------------------------------*/

/** @internal File formats. */
enum PlyFormat {
	PLY_ASCII     = 1,		///< ascii PLY file
	PLY_BINARY_BE = 2,		///< binary PLY file, big endian
	PLY_BINARY_LE = 3		///< binary PLY file, little endian
};


/** @internal Ply number types. */
enum PlyType {
	PLY_INT8	= 1,
	PLY_INT16	= 2,
	PLY_INT32	= 3,
	PLY_UINT8	= 4,
	PLY_UINT16	= 5,
	PLY_UINT32	= 6,
	PLY_FLOAT32	= 7,
	PLY_FLOAT64	= 8
};


/** @internal Ply property type. */
enum PlyElementType {
	PLET_VERTEX	= 0,
	PLET_FACE	= 1,
	PLET_OTHER	= 2
};


/** @internal Ply property type. */
enum PlyPropertyMode {
	PLPM_SCALAR	= 0,
	PLPM_LIST	= 1,
	PLPM_STRING	= 2
};


/** @internal Ply element. */
struct PlyProperty {
	char *			name;
	PlyPropertyMode	mode;
	PlyType			type;
	PlyType			ctype;		// Type of the count.
	PlyProperty *	next;
	PlyProperty *	prev;
};


/** @internal Ply element. */
struct PlyElement {
	char *			name;
	PlyElementType	type;
	uint32			num;
	PlyProperty		props;
	PlyElement *	next;
	PlyElement *	prev;
};


/** @internal Ply file header. */
struct PlyHeader {
	PlyFormat	format;
	uint32		version;
	PlyElement  elems;
};



/*----------------------------------------------------------------------------
	Functions:
----------------------------------------------------------------------------*/

uint16 Swap16( uint16 u ) { 
	return (((u & 0x00FF) << 8) |
		((u & 0xFF00) >> 8));
}
uint32 Swap32( uint32 u ) { 
	return (((u & 0x000000FF) << 24) | 
		((u & 0x0000FF00) << 8)  |
		((u & 0x00FF0000) >> 8)  |
		((u & 0xFF000000) >> 24));
}
uint32 Swap64( uint32 u ) { 
	return u;
}



#if BIG_ENDIAN
uint16 SwapBE16( uint16 u ) { return u; }
uint32 SwapBE32( uint32 u ) { return u; }
uint32 SwapBE64( uint32 u ) { return u; }
uint16 SwapLE16( uint16 u ) { return Swap16(u); }
uint32 SwapLE32( uint32 u ) { return Swap32(u); }
uint32 SwapLE64( uint32 u ) { return Swap64(u); }
#else
uint16 SwapBE16( uint16 u ) { return Swap16(u); }
uint32 SwapBE32( uint32 u ) { return Swap32(u); }
uint32 SwapBE64( uint32 u ) { return Swap64(u); }
uint16 SwapLE16( uint16 u ) { return u; }
uint32 SwapLE32( uint32 u ) { return u; }
uint32 SwapLE64( uint32 u ) { return u; }
#endif




/*----------------------------------------------------------------------------
	Classes:
----------------------------------------------------------------------------*/

/** Default ctor. */
PiImportPly::PiImportPly( const char * name ) {
	this->name = strdup( name );
}

/** Replacement for the private ctor. */
IImportMesh * PiImportPly::Create( const char * name ) {
	return new PiImportPly( name );
}

/** Dtor. */
PiImportPly::~PiImportPly( void ) {
	free( name ); name = NULL;
}


/** Load the PLY mesh. */
bool PiImportPly::Import( void ) {

	vertex_has = 0;

	// @todo Memory mapping would be desiderable...
	FILE * fp = fopen( name, "rb" );
	if( fp==NULL ) {
		piDebug( "*** Cannot open PLY file: '%s'\n", name );
		return false;
	}


	fscanf( fp, "%s", token );
	if( stricmp(token, "ply") ) {
		piDebug( "*** File '%s' does not appear to be a ply file.\n", name );
		fclose( fp );
		return false;
	}
	
	piDebug("--- Loading PLY file %s.\n", name );

	PlyHeader head;
	PlyElement * current;

	LL_Reset( &head.elems, next, prev );


	bool result = false;

	while( true ) {
	
		//if( !parser.GetToken( true ) ) {
		//	piDebug( "*** Unexpected 'end of file' found while reading ply file '%s'.\n", name );
		//	break;
		//}
		fscanf( fp, "%s", token );

		if( !stricmp( token, "format" ) ) {
			// 'format' <type> <version>
			//if( !parser.GetToken( false ) ) {
			//	piDebug( "*** Type expected while reading ply file '%s'.\n", name );
			//	break;
			//}
			fscanf( fp, "%s", token );

			if( !stricmp( token, "ascii" ) ) head.format = PLY_ASCII;
			else if( !stricmp( token, "binary_big_endian" ) ) head.format = PLY_BINARY_BE;
			else if( !stricmp( token, "binary_little_endian" ) ) head.format = PLY_BINARY_LE;

			//if( !parser.GetToken( false ) ) {
			//	piDebug( "*** Version expected while reading ply file '%s'.\n", name );
			//	break;
			//}
			fscanf( fp, "%s", token );

			head.version = 0;
		}
		else if( !stricmp( token, "element" ) ) {
			// 'element' <type> <num>

			current = new PlyElement;
			LL_Reset( current, next, prev );
			LL_AddLast( &head.elems, current, next, prev );
			LL_Reset( &current->props, next, prev );

			//if( !parser.GetToken( false ) ) {
			//	piDebug( "*** Element type expected while reading ply file '%s'.\n", name );
			//	break;
			//}
			fscanf( fp, "%s", token );

			current->name = strdup( token );
			if( !stricmp( token, "vertex" ) ) current->type = PLET_VERTEX;
			else if( !stricmp( token, "face" ) ) current->type = PLET_FACE;
			else current->type = PLET_OTHER;
			
			//if( !parser.GetToken( false ) ) {
			//	piDebug( "*** Number of elements expected while reading ply file '%s'.\n", name );
			//	break;
			//}
			//current->num = atoi( token );
			fscanf( fp, "%i", &current->num );

		}
		else if( !stricmp( token, "property" ) ) {
			// 'property' <type> <name>

			PlyProperty * prop = new PlyProperty;
			LL_Reset( prop, next, prev );
			LL_AddLast( &current->props, prop, next, prev );

			//if( !parser.GetToken( false ) ) {
			//	piDebug( "*** Property type expected while reading ply file '%s'.\n", name );
			//	break;
			//}
			fscanf( fp, "%s", token );

			if( !stricmp( token, "list" ) ) {
				prop->mode = PLPM_LIST;
				//if( !parser.GetToken( false ) ) {
				//	piDebug( "*** Property type expected while reading ply file '%s'.\n", name );
				//	break;
				//}
				fscanf( fp, "%s", token );
				if( !ReadType( fp, prop->ctype ) ) break;

				//if( !parser.GetToken( false ) ) {
				//	piDebug( "*** Property type expected while reading ply file '%s'.\n", name );
				//	break;
				//}
				fscanf( fp, "%s", token );
			}
			else {
				prop->mode = PLPM_SCALAR;
			}

			if( !ReadType( fp, prop->type ) ) break;

			// read name
			//if( !parser.GetToken( false ) ) {
			//	piDebug( "*** Property name expected while reading ply file '%s'.\n", name );
			//	break;
			//}
			fscanf( fp, "%s", token );
			prop->name = strdup( token );
		}
		else if( !stricmp( token, "comment" ) ) {
			piDebug( "---   Comment:" );
			//while( parser.GetToken( false ) ) {
			//	piDebug( " %s", parser.token );
			//};
			char c;
			do {
				fread( &c, 1, 1, fp );
				fputc( c, stdout );
			} while( c!='\n' );
		}
		else if( !stricmp( token, "end_header" ) ) {
			//if( parser.NextLine() )
			//	result = true;
			char c;
			do {
				fread( &c, 1, 1, fp );
				fputc( c, stdout );
			} while( c!='\n' );
			result = true;
			break;
		}
	};


	if( result == true ) {

		// Analize what we have:
		PlyElement * e;
		for( e=head.elems.next; e!=&head.elems; e=e->next ) {
			if( !stricmp( e->name, "vertex" ) ) {
				pos_array.Resize( e->num );
				col_array.Resize( e->num );
				vertex_has |= IMF_HAS_POSITION;
			}
			else if( !stricmp( e->name, "face" ) ) {
			//	face_array.Resize( e->num );
			}
		}

		int face_num = 0;
		int pos_num = 0;

		Vec3 pos;
		Vec3 col;

		// Extract data.
		for( e=head.elems.next; e!=&head.elems; e=e->next ) {

			for( uint32 i=0; i<e->num; i++ ) {

				PlyProperty * p;
				for( p=e->props.next; p!=&e->props; p=p->next ) {

					if( p->mode == PLPM_SCALAR ) {
						float value;
						ReadFloatValue( fp, head.format, p->type, value );

						if( !stricmp( p->name, "x" ) ) pos.x = value;
						else if( !stricmp( p->name, "y" ) ) pos.y = value;
						else if( !stricmp( p->name, "z" ) ) {
							pos.z = value;
							//pos_array.PushBack( pos );
							pos_array[pos_num] = pos;
							pos_num++;
						}

					//	piDebug( "--- %s = %f\n", p->name, value );
					}
					else if( p->mode == PLPM_LIST ) {
						int count;
						ReadIntValue( fp, head.format, p->ctype, count );

						Face face;
						ReadIntValue( fp, head.format, p->type, face.v0 );
						ReadIntValue( fp, head.format, p->type, face.v1 );

						// conver poly to fan
						for( int f=0; f<count-2; f++ ) {
							ReadIntValue( fp, head.format, p->type, face.v2 );
							face_array.PushBack( face );
							//face_array[face_num] = face;
							face_num++;
							face.v1 = face.v2;
						}
					}
				}
			}
		}

		piDebug("---   Load succeed.\n");
	}
	else {
		piDebug("---   Load failed.\n");
	}
	
	// clean header
	PlyElement * e, * enext;
	for( e=head.elems.next; e!=&head.elems; e=enext ) {
		enext = e->next;
		LL_Reset( e, next, prev );

		PlyProperty * p, * pnext;
		for( p=e->props.next; p!=&e->props; p=pnext ) {
			pnext = p->next;
			LL_Reset( p, next, prev );

			free( p->name );
			delete p;
		}

		free( e->name );
		delete e;
	}

	fclose( fp );
	return result;
}


/** Read and parse a type name. */
bool PiImportPly::ReadType( FILE * fp, PlyType &type ) {
	if( !stricmp( token, "Int8" ) )			type = PLY_INT8;
	else if( !stricmp( token, "char" ) )	type = PLY_INT8;
	else if( !stricmp( token, "Int16" ) )	type = PLY_INT16;
	else if( !stricmp( token, "short" ) )	type = PLY_INT16;
	else if( !stricmp( token, "Int32" ) )	type = PLY_INT32;
	else if( !stricmp( token, "int" ) )		type = PLY_INT32;
	else if( !stricmp( token, "Uint8" ) )	type = PLY_UINT8;
	else if( !stricmp( token, "uchar" ) )	type = PLY_UINT8;
	else if( !stricmp( token, "Uint16" ) )	type = PLY_UINT16;
	else if( !stricmp( token, "ushort" ) )	type = PLY_UINT16;
	else if( !stricmp( token, "Uint32" ) )	type = PLY_UINT32;
	else if( !stricmp( token, "uint" ) )	type = PLY_UINT32;
	else if( !stricmp( token, "Float32" ) )	type = PLY_FLOAT32;
	else if( !stricmp( token, "float" ) )	type = PLY_FLOAT32;
	else if( !stricmp( token, "Float64" ) )	type = PLY_FLOAT64;
	else if( !stricmp( token, "double" ) )	type = PLY_FLOAT64;
	else {
		// may be a string?
		piDebug( "*** Bad property type, while reading ply file '%s'.\n", name );
		return false;
	}

	return true;
}


/** Read an element of the body of the PLY file. */
bool PiImportPly::ReadFloatValue( FILE * fp, int format, int type, float &val ) {

	if( format == PLY_ASCII ) {
		//if( !parser.GetToken( true ) ) return false;
		//sscanf( parser.token, "%f", &val );
		fscanf( fp, "%f", &val );
	}
	else {
		switch( type ) {
			case PLY_FLOAT32: {
				uint32 u;
				//if( !parser.SkipBytes( &u, 4 ) ) return false;
				fread( &u, 4, 1, fp );
				if( format==PLY_BINARY_BE ) u = SwapBE32( u );
				if( format==PLY_BINARY_LE ) u = SwapLE32( u );
				val = (float &)u;
			} break;
			case PLY_FLOAT64: {
				uint64 u;
				//if( !parser.SkipBytes( &u, 8 ) ) return false;
				fread( &u, 8, 1, fp );
				if( format==PLY_BINARY_BE ) u = SwapBE64( u );
				if( format==PLY_BINARY_LE ) u = SwapLE64( u );
				val = (float) (double &)u;
			} break;
		}
	}

	return true;
}


/** Read an element of the body of the PLY file. */
bool PiImportPly::ReadIntValue( FILE * fp, int format, int type, int &val ) {

	if( format == PLY_ASCII ) {
		//if( !parser.GetToken( true ) ) return false;
		//sscanf( parser.token, "%i", &val );
		fscanf( fp, "%i", &val );
	}
	else {
		switch( type ) {
			case PLY_INT8: {
				sint8 i;
				//if( !parser.SkipBytes( &i, 1 ) ) return false;
				fread( &i, 1, 1, fp );
				val = (int)i;
			} break;
			case PLY_INT16: {
				uint16 u;
				//if( !parser.SkipBytes( &u, 2 ) ) return false;
				fread( &u, 2, 1, fp );
				if( format==PLY_BINARY_BE ) u = SwapBE16( u );
				if( format==PLY_BINARY_LE ) u = SwapLE16( u );
				val = (int) (sint16 &)u;
			} break;
			case PLY_INT32: {
				uint32 u;
				//if( !parser.SkipBytes( &u, 4 ) ) return false;
				fread( &u, 4, 1, fp );
				if( format==PLY_BINARY_BE ) u = SwapBE32( u );
				if( format==PLY_BINARY_LE ) u = SwapLE32( u );
				val = (int) (sint32 &)u;
			} break;
			case PLY_UINT8: {
				uint8 i;
				//if( !parser.SkipBytes( &i, 1 ) ) return false;
				fread( &i, 1, 1, fp );
				val = (int)i;
			} break;
			case PLY_UINT16: {
				uint16 u;
				//if( !parser.SkipBytes( &u, 2 ) ) return false;
				fread( &u, 2, 1, fp );
				if( format==PLY_BINARY_BE ) u = SwapBE16( u );
				if( format==PLY_BINARY_LE ) u = SwapLE16( u );
				val = (int) (uint16 &)u;
			} break;
			case PLY_UINT32: {
				uint32 u;
				//if( !parser.SkipBytes( &u, 4 ) ) return false;
				fread( &u, 4, 1, fp );
				if( format==PLY_BINARY_BE ) u = SwapBE32( u );
				if( format==PLY_BINARY_LE ) u = SwapLE32( u );
				val = (int) (uint32 &)u;	// Warning!!
			} break;
			default:
				val = 0;
		}
	}

	return true;
}

