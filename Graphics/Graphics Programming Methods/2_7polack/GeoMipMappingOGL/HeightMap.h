//==============================================================
// HeightMap.h
// Coded by: Trent Polack (trent@codershq.com)
//==============================================================
// Height Map routines (loading, releasing, saving, etc.)
//==============================================================
#ifndef __Terrain_HeightMap_H__
#define __Terrain_HeightMap_H__

namespace Terrain {
	class HeightMap {
		unsigned char *m_pData;
		int m_size;

	public:
	
	bool Load( const char *filename, int size );
	void Release( void );

	inline unsigned char GetData( int x, int z ) {
		return( m_pData[( z*m_size ) + x ] );
	}

	inline int GetSize( void ) { return m_size; }

	HeightMap( const char *filename, int size ) {
		Load( filename, size );
	}
	HeightMap( void ) { }
	~HeightMap( void ) { }
	};
}


#endif	//__Terrain_HeightMap_H__