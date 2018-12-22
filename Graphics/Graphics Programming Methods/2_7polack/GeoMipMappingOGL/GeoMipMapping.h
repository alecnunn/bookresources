//==============================================================
// GeoMipMapping.h
// Coded by: Trent Polack (trent@codershq.com)
//==============================================================
// Implementation of a GeoMipMapping terrain engine
//==============================================================
#ifndef __Terrain_GeoMipMapping_H__
#define __Terrain_GeoMipMapping_H__

#include "Framework\\Camera.h"
#include "Framework\\Texture.h"

#include "HeightMap.h"

namespace Terrain {
	#define PATCH_SIZE 33
	#define NUM_PATCHES 32

	#define NORTH_NEIGHBOR 0
	#define EAST_NEIGHBOR 1
	#define SOUTH_NEIGHBOR 2
	#define WEST_NEIGHBOR 3

	class GeoMipMappingPatch;
	class GeoMipMapping;

	//geomipmapping patch
	class GeoMipMappingPatch {
		GeoMipMapping *m_pParent;

		int m_patchX, m_patchZ;

		unsigned int **m_indices;
		int m_LOD, m_maxLOD;

		bool m_isVisible;
		bool m_badNeighbor[4];

		int *m_numVertices, *m_numTriangles;

	void FixCracks( void );
	void BuildIndices( int LOD );

	public:

	void Create( void );
	void Release( void );

	//render the patch
	void Render( void );

	//set the parent that this patch belongs to
	void SetParent( GeoMipMapping *pParent ) {
		m_pParent= pParent;
	}

	//set the patch's location in the field
	void SetPatchXZ( int x, int z ) {
		m_patchX= x;
		m_patchZ= z;
	}

	int GetNumVertices( int LOD ) { return m_numVertices[LOD]; }
	int GetNumTriangles( int LOD ) { return m_numTriangles[LOD]; }

	int GetLOD( void ) { return m_LOD; }
	void SetLOD( int LOD ) { m_LOD= LOD; }

	//set neighbor flag (true= needs to be adjusted to fix cracks)
	void SetNeighborFlag( int neighbor, bool flag ) {
		m_badNeighbor[neighbor]= flag;
	}

	bool IsVisible( void ) { return m_isVisible; }
	bool SetVisibility( bool visible ) { m_isVisible= visible; }

	GeoMipMappingPatch( void ) : m_LOD( 0 ), m_patchX( 0 ), m_patchZ( 0 ) { }
	~GeoMipMappingPatch( void ) { }
	};

	//geomipmapping implementation
	class GeoMipMapping {
		HeightMap m_heightMap;
		int m_size;

		int *m_pVertices;
		float m_heightScale;

		float *m_pTexCoords;

		GeoMipMappingPatch m_patches[NUM_PATCHES][NUM_PATCHES];
		int m_maxLOD;

		DemoFramework::Texture m_texture;

		int m_numVertices;
		int m_numTriangles;

	public:
	
	bool Create( float heightScale );
	void Release( void );

	void Update( DemoFramework::Camera *pCamera );
	void Render( void );

	//load a RAW height map in
	bool LoadDataSet( const char *filename, int size ) {
		m_size= size;
		return( m_heightMap.Load( filename, size ) );
	}

	//release the loaded height map
	void ReleaseDataSet( void ) {
		m_heightMap.Release( );
	}

	//get some data from the height map
	unsigned char GetData( int x, int z ) { 
		return( ( unsigned char )( m_heightMap.GetData( x, z )*m_heightScale ) );
	}

	//load a texture map for the terrain to use
	bool LoadTextureMap( char *filename ) {
		m_texture.Load( filename, GL_LINEAR, GL_LINEAR, false );
	}

	//get the size of the terrain
	int GetSize( void ) { return m_size; }

	//get the number of LOD levels for the terrain
	int GetMaxLOD( void ) { return m_maxLOD; }

	//get the number of vertices/triangles
	int GetNumVertices( void ) { return m_numVertices; }
	int GetNumTriangles( void ) { return m_numTriangles; }

	GeoMipMapping( void ) : m_size( 0 ), m_numVertices( 0 ), m_numTriangles( 0 ) {
	}
	~GeoMipMapping( void ) { }
	};
}


#endif	//__Terrain_GeoMipMapping_H__