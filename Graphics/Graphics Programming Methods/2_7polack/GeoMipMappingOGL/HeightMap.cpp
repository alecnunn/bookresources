//==============================================================
//= HeightMap.cpp
//= Original coder: Trent Polack (trent@codershq.com)
//==============================================================
//= Height Map routines (loading, releasing, saving, etc.)
//==============================================================
#include <cstdio>

#include "Framework\\Log.h"

#include "HeightMap.h"

//load in a RAW height map
bool Terrain::HeightMap::Load( const char *filename, int size ) {
	FILE *pFile;

	if( m_pData )	Release( );

	//open the RAW height map dataset
	pFile= fopen( filename, "rb" );
	if( !pFile ) {
		g_log.Write( DemoFramework::LOG_SUCCESS, "%s does not exist", filename );
		return false;
	}

	m_pData= new unsigned char [size*size];

	//read the heightmap into context
	fread( m_pData, 1, size*size, pFile );

	fclose( pFile );

	m_size= size;

	//w00t w00t! The heightmap has been successfully loaded
	g_log.Write( DemoFramework::LOG_SUCCESS, "Successfully loaded %s", filename );
	return true;
}

//release the height map
void Terrain::HeightMap::Release( void ) {
	if( m_pData ) {
		delete[] m_pData;

		m_size= 0;
	}
}