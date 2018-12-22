//==============================================================
//= HeightMap.cpp
//= Original coder: Trent Polack (trent@codershq.com)
//==============================================================
//= Height Map routines (loading, releasing, saving, etc.)
//==============================================================
#include <cstdio>
#include <windows.h>
#include "HeightMap.h"

//load in a RAW height map
bool Terrain::HeightMap::Load( const char *filename, int size ) {
	FILE *pFile;

	if( m_pData )	Release( );

	//open the RAW height map dataset
	pFile= fopen( filename, "rb" );
	if( !pFile )	return false;

	m_pData= new unsigned char [size*size];

	if( m_pData == NULL )	return false;

	//read the heightmap into context
	fread( m_pData, 1, size*size, pFile );

	fclose( pFile );

	m_size= size;

	//w00t w00t! The heightmap has been successfully loaded
	return true;
}

//release the height map
void Terrain::HeightMap::Release( void ) {
	if( m_pData ) {
		delete[] m_pData;

		m_size= 0;
	}
}