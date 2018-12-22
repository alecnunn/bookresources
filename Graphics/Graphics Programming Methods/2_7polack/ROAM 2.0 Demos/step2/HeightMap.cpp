//==============================================================
//= HeightMap.cpp
//= Original coder: Trent Polack (trent@codershq.com)
//==============================================================
//= Height Map routines (loading, releasing, saving, etc.)
//==============================================================
#include <cstdio>
#include <windows.h>

#include "HeightMap.h"
#include "..\\Framework\\Log.h"

//load in a RAW height map
bool Terrain::HeightMap::Load( const char *filename, int size ) {
	FILE *pFile;

	if( m_pData )	Release( );

	//open the RAW height map dataset
	pFile= fopen( filename, "rb" );
	if( !pFile ) {
		g_log.Write( DemoFramework::LOG_RENDERER, "%s does not exist", filename );
		return false;
	}

	m_pData= new unsigned char [size*size];

	if( !m_pData ) {
		g_log.Write( DemoFramework::LOG_RENDERER, "Could not allocate memory for %s", filename );
		return false;
	}

	//read the heightmap into context
	fread( m_pData, 1, size*size, pFile );

	fclose( pFile );

	m_size= size;

	//w00t w00t! The heightmap has been successfully loaded
	g_log.Write( DemoFramework::LOG_RENDERER, "%s has been successfully loaded", filename );
	return true;
}

//release the height map
void Terrain::HeightMap::Release( void ) {
	if( m_pData ) {
		delete[] m_pData;

		m_size= 0;
	}

	g_log.Write( DemoFramework::LOG_RENDERER, "The height map has been successfully released" );
}