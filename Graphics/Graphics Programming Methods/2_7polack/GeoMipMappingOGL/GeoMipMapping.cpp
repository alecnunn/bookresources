//==============================================================
// GeoMipMapping.cpp
// Coded by: Trent Polack (trent@codershq.com)
//==============================================================
// Implementation of a GeoMipMapping terrain engine
//==============================================================
#include <SDL_opengl.h>

#include "Framework\\Log.h"
#include "Framework\\GLExtensions.h"

#include "MathLib\\Vector.h"

#include "GeoMipMapping.h"

#include <cmath>

//initialize the geomipmappin system
bool Terrain::GeoMipMapping::Create( float heightScale ) {
	if( !m_size )	return false;

	int x, z;

	m_heightScale= heightScale;

	m_pVertices= new int[m_size*m_size*3];
	m_pTexCoords= new float[m_size*m_size*2];

	//create the vertex and texture arrays
	for( z= 0; z < m_size; z++ ) {
		for( x= 0; x < m_size; x++ ) {
			int vertOffset= ( ( z*m_size ) + x )*3;
			int texOffset= ( ( z*m_size ) + x )*2;

			m_pVertices[vertOffset]= x;
			m_pVertices[vertOffset + 1]= ( int )( ( float )m_heightMap.GetData( x, z )*m_heightScale );
			m_pVertices[vertOffset + 2]= z;

			m_pTexCoords[texOffset]= ( float )x/( float )m_size;
			m_pTexCoords[texOffset + 1]= ( float )z/( float )m_size;
		}
	}

	int divisor= PATCH_SIZE - 1;
	int LOD= 0;
	while( divisor > 2 ) {
		divisor= divisor >> 1;
		LOD++;
	}

	//the max number of detail levels
	m_maxLOD= LOD;

	//build patch indices
	for( z= 0; z < NUM_PATCHES; z++ ) {
		for( x= 0; x < NUM_PATCHES; x++ ) {
			m_patches[z][x].SetParent( this );
			m_patches[z][x].SetPatchXZ( x, z );
			m_patches[z][x].Create( );
		}
	}

	g_log.Write( DemoFramework::LOG_SUCCESS, "Successfully created the GeoMipMapping system" );
	return true;
}

//shutdown the geomipmapping system
void Terrain::GeoMipMapping::Release( void ) {
	for( int z= 0; z < NUM_PATCHES; z++ ) {
		for( int x= 0; x < NUM_PATCHES; x++ ) {
			m_patches[z][x].Release( );
		}
	}

	delete[] m_pTexCoords;	
	delete[] m_pVertices;

	g_log.Write( DemoFramework::LOG_SUCCESS, "Successfully released the GeoMipMapping system" );
}

//update all of the geomipmapping patches (culling and LOD)
void Terrain::GeoMipMapping::Update( DemoFramework::Camera *pCamera ) {
	int halfSize= PATCH_SIZE >> 1;
	
	for( int z= 0; z < NUM_PATCHES; z++ ) {
		for( int x= 0; x < NUM_PATCHES; x++ ) {
			float dist;

			float cX= ( float )( x*NUM_PATCHES ) + halfSize;
			float cZ= ( float )( z*NUM_PATCHES ) + halfSize;
			float cY= m_heightMap.GetData( ( int )cX, ( int )cZ )*m_heightScale;

			//do a frustum test against the patch
			if( pCamera->CubeInFrustum( cX, cY, cZ, PATCH_SIZE ) )
				m_patches[z][x].SetVisibility( true );
			else
				m_patches[z][x].SetVisibility( false );

			dist= sqrtf( SQR( ( cX - pCamera->m_eyePos[0] ) )+
						 SQR( ( cY - pCamera->m_eyePos[1] ) )+
						 SQR( ( cZ - pCamera->m_eyePos[2] ) ) );

			//set patch LOD, TODO: make a more rigorous test
			if( dist < 250 )	m_patches[z][x].SetLOD( 0 );
			else if( dist < 500 )	m_patches[z][x].SetLOD( 1 );
			else if( dist < 750 )	m_patches[z][x].SetLOD( 2 );
			else if( dist >= 750 )	m_patches[z][x].SetLOD( 3 );
		}
	}
}

//render the terrain
void Terrain::GeoMipMapping::Render( void ) {
	glEnable( GL_CULL_FACE );

	m_numVertices= 0;
	m_numTriangles= 0;

	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, m_texture.GetID( ) );

	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 3, GL_INT, 0, m_pVertices );

	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glTexCoordPointer( 2, GL_FLOAT, 0, m_pTexCoords );

	if( GLExtensions::canCVA ) GLExtensions::glLockArraysEXT( 0, m_size*m_size*3 );	

	//loop through the patches and render them individually
	for( int z= 0; z < NUM_PATCHES; z++ ) {
		for( int x= 0; x < NUM_PATCHES; x++ ) {
			if( m_patches[z][x].IsVisible( ) ) {
				int LOD= m_patches[z][x].GetLOD( );

				if( z < NUM_PATCHES ) {
					if( m_patches[z + 1][x].GetLOD( ) < LOD )
						m_patches[z][x].SetNeighborFlag( NORTH_NEIGHBOR, true );	//set north neighbor as bad
				}

				if( x < NUM_PATCHES ) {
					if( m_patches[z][x + 1].GetLOD( ) < LOD )
						m_patches[z][x].SetNeighborFlag( EAST_NEIGHBOR, true );		//set east neighbor as bad
				}

				if( z > 0 ) {
					if( m_patches[z - 1][x].GetLOD( ) < LOD )
						m_patches[z][x].SetNeighborFlag( SOUTH_NEIGHBOR, true );	//set south neighbor as bad
				}

				if( x > 0 ) {
					if( m_patches[z][x - 1].GetLOD( ) < LOD )
						m_patches[z][x].SetNeighborFlag( WEST_NEIGHBOR, true );		//set west neighbor as bad
				}

				m_patches[z][x].Render( );

				m_numVertices+= m_patches[z][x].GetNumVertices( m_patches[z][x].GetLOD( ) );
				m_numTriangles+= m_patches[z][x].GetNumTriangles( m_patches[z][x].GetLOD( ) );
			}
		}
	}

	if( GLExtensions::canCVA ) GLExtensions::glUnlockArraysEXT( );

	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_VERTEX_ARRAY );
}

//initialize the geomipmapping patch
void Terrain::GeoMipMappingPatch::Create( void ) {
	m_maxLOD= m_pParent->GetMaxLOD( );

	m_indices= new unsigned int *[m_maxLOD];
	m_numVertices= new int [m_maxLOD];
	m_numTriangles= new int [m_maxLOD];

	for( int i= 0; i < m_maxLOD; i++ ) {
		int size;

		if( i == 0 ) size= PATCH_SIZE;
		else size= ( int )( PATCH_SIZE/pow( 2, i ) ) + 1;

		m_indices[i]= new unsigned int[size*size*2];
	}

	//build the patches indices into the vertex/texture arrays
	for( i= 0; i < m_maxLOD; i++ )
		BuildIndices( i );

	m_LOD= 3;
}

//shutdown the geomipmapping patch
void Terrain::GeoMipMappingPatch::Release( void ) {
	delete[] m_numTriangles;
	delete[] m_numVertices;

	for( int i= 0; i < m_maxLOD; i++ )
		delete[] m_indices[i];
	delete[] m_indices;
	m_indices= NULL;
}

//render the patch
void Terrain::GeoMipMappingPatch::Render( void ) {
	unsigned int *pBuffer;
	int patchSize;
	int increment, spill;

	if( m_LOD == 0 ) {
		increment= 1;
		spill= 0;
	} else {
		increment= ( int )pow( 2, m_LOD );
		spill= 2;
	}

	patchSize= PATCH_SIZE/increment;

	pBuffer= m_indices[m_LOD];

	//draw the patch vertices
	for( int z= 0; z < PATCH_SIZE - 1; z+= increment ) {
		glDrawElements( GL_TRIANGLE_STRIP, ( patchSize*2 ) + spill, GL_UNSIGNED_INT, pBuffer );

		pBuffer+= ( patchSize*2 ) + spill;
	}

	//render skirts (where need be) around the patch to fix any potential cracks
	FixCracks( );
}

//fix cracks around the patch using skirts where necessary
void Terrain::GeoMipMappingPatch::FixCracks( void ) {
	int size= m_pParent->GetSize( );
	int LOD= m_LOD - 1;
	int patchSize;
	int increment;

	if( LOD == 0 ) increment= 1;
	else increment= ( int )pow( 2, LOD );

	patchSize= PATCH_SIZE/increment;

	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );

	glDisable( GL_CULL_FACE );

	//check to see if a skirt needs to be rendered on the north patch side
	if( m_badNeighbor[NORTH_NEIGHBOR] ) {
		bool calcHeight= false;

		glBegin( GL_TRIANGLE_STRIP );		//TODO: use a vertex array instead
			for( int x= 0; x < PATCH_SIZE; x+= increment ) {
				int rX= ( m_patchX*NUM_PATCHES ) + x;
				int rZ= ( m_patchZ*NUM_PATCHES ) + PATCH_SIZE - 1;
				float height;

				if( !calcHeight ) {
					height= m_pParent->GetData( rX, rZ );
					calcHeight= true;
				} else {
					height= ( m_pParent->GetData( rX - increment, rZ ) + 
							  m_pParent->GetData( rX + increment, rZ ) )/2.0f;
					calcHeight= false;
				}

				glTexCoord2f( ( float )rX/( float )size, ( float )rZ/( float )size );
				glVertex3f( ( float )rX, height, ( float )rZ );
				glVertex3f( ( float )rX, height - 5.0f, ( float )rZ );
			}
		glEnd( );

		m_badNeighbor[NORTH_NEIGHBOR]= false;
	}

	//check to see if a skirt needs to be rendered on the east patch side
	if( m_badNeighbor[EAST_NEIGHBOR] ) {
		bool calcHeight= false;

		glBegin( GL_TRIANGLE_STRIP );		//TODO: use a vertex array instead
			for( int z= 0; z < PATCH_SIZE; z+= increment ) {
				int rX= ( m_patchX*NUM_PATCHES ) + PATCH_SIZE - 1;
				int rZ= ( m_patchZ*NUM_PATCHES ) + z;
				float height;

				if( !calcHeight ) {
					height= m_pParent->GetData( rX, rZ );
					calcHeight= true;
				} else {
					height= ( m_pParent->GetData( rX, rZ - increment ) + 
							  m_pParent->GetData( rX, rZ + increment ) )/2.0f;
					calcHeight= false;
				}		

				glTexCoord2f( ( float )rX/( float )size, ( float )rZ/( float )size );
				glVertex3f( ( float )rX, height, ( float )rZ );
				glVertex3f( ( float )rX, height - 5.0f, ( float )rZ );
			}
		glEnd( );

		m_badNeighbor[EAST_NEIGHBOR]= false;
	}

	//check to see if a skirt needs to be rendered on the south patch side
	if( m_badNeighbor[SOUTH_NEIGHBOR] ) {
		bool calcHeight= false;

		glBegin( GL_TRIANGLE_STRIP );		//TODO: use a vertex array instead
			for( int x= 0; x < PATCH_SIZE; x+= increment ) {
				int rX= ( m_patchX*NUM_PATCHES ) + x;
				int rZ= m_patchZ*NUM_PATCHES;
				float height;

				if( !calcHeight ) {
					height= m_pParent->GetData( rX, rZ );
					calcHeight= true;
				} else {
					height= ( m_pParent->GetData( rX - increment, rZ ) + 
							  m_pParent->GetData( rX + increment, rZ ) )/2.0f;
					calcHeight= false;
				}		

				glTexCoord2f( ( float )rX/( float )size, ( float )rZ/( float )size );
				glVertex3f( ( float )rX, height, ( float )rZ );
				glVertex3f( ( float )rX, height - 5.0f, ( float )rZ );
			}
		glEnd( );

		m_badNeighbor[SOUTH_NEIGHBOR]= false;
	}

	//check to see if a skirt needs to be rendered on the west patch side
	if( m_badNeighbor[WEST_NEIGHBOR] ) {
		bool calcHeight= false;

		glBegin( GL_TRIANGLE_STRIP );		//TODO: use a vertex array instead
		for( int z= 0; z < PATCH_SIZE; z+= increment ) {
			int rX= m_patchX*NUM_PATCHES;
			int rZ= ( m_patchZ*NUM_PATCHES ) + z;
			float height;

			if( !calcHeight ) {
				height= m_pParent->GetData( rX, rZ );
				calcHeight= true;
			} else {
				height= ( m_pParent->GetData( rX, rZ - increment ) + 
						  m_pParent->GetData( rX, rZ + increment ) )/2.0f;
				calcHeight= false;
			}

			glTexCoord2f( ( float )rX/( float )size, ( float )rZ/( float )size );
			glVertex3f( ( float )rX, height, ( float )rZ );
			glVertex3f( ( float )rX, height - 5.0f, ( float )rZ );
		}
		glEnd( );

		m_badNeighbor[WEST_NEIGHBOR]= false;
	}

	glEnable( GL_CULL_FACE );
}

//build the patch vertex/texture array indices
void Terrain::GeoMipMappingPatch::BuildIndices( int LOD ) {
	int size= m_pParent->GetSize( );
	int index= 0;
	int patchSize;
	int increment;

	if( LOD == 0 ) increment= 1;
	else increment= ( int )pow( 2, LOD );

	patchSize= PATCH_SIZE/increment;

	memset( m_indices[LOD], 0, sizeof( patchSize*patchSize*2 ) );
	m_numVertices[LOD]= 0;
	m_numTriangles[LOD]= 0;

	for( int z= 0; z < PATCH_SIZE; z+= increment ) {
		for( int x= 0; x < PATCH_SIZE; x+= increment ) {
			int rX= ( m_patchX*NUM_PATCHES ) + x;
			int rZ= ( m_patchZ*NUM_PATCHES ) + z;
		
			m_indices[LOD][index + 0]= ( rZ*size ) + rX;
			m_indices[LOD][index + 1]= ( ( rZ + increment )*size ) + rX;

			index+= 2;

			m_numVertices[LOD]+= 2;
			m_numTriangles[LOD]+= 2;
		}

		m_numTriangles[LOD]-= 2;
	}
}