/*===========================================================================
	Title: EDT.h
	Module: GPM/EDT
	Author: Ignacio Castaño
	Description: Main header file.
	Changes:
		11/01/2003 - Ignacio Castaño
			File added.

===========================================================================*/

#ifndef _GPM_EDT_H_
#define _GPM_EDT_H_


/*---------------------------------------------------------------------------
	Doc:
---------------------------------------------------------------------------*/

/** @file EDT.h
 * Main header file.
**/


/*---------------------------------------------------------------------------
	Headers:
---------------------------------------------------------------------------*/

//#include "Core.h"
//#include "Debug.h"

// From Core
#include <windows.h>
#include <stdio.h>


typedef unsigned char		uint8;
typedef unsigned short		uint16;
typedef unsigned int		uint32;
typedef unsigned __int64	uint64;


// From Debug
#include <assert.h>

#define piAssert(exp)		assert(exp)
#define piCheck(exp)		assert(exp)

inline void piDebug( const char *msg, ... ) {
	va_list arg;
	va_start(arg,msg);
	vprintf( msg, arg );
	va_end(arg);	
}



/*---------------------------------------------------------------------------
	Types:
---------------------------------------------------------------------------*/

enum {
	RT_DN,
	RT_UP,
	LF_DN,
	LF_UP
};

enum {
	NB_0 = 0x01,
	NB_1 = 0x02,
	NB_2 = 0x04,
	NB_3 = 0x08,
	NB_4 = 0x10,
	NB_5 = 0x20,
	NB_6 = 0x40,
	NB_7 = 0x80
};


/*---------------------------------------------------------------------------
	Classes:
---------------------------------------------------------------------------*/


/** A pixel of the image. */
struct pixel {
	uint8 r;
	uint8 g;
	uint8 b;
	int x;
	int y;
	float dist;
	bool e;
};


/** An image. */
struct Image	{

	Image( void );
	~Image( void );
	Image * clone( void );


	bool open( const char * name );

	void empty( int w, int h );
	void free( void );

	void show( const char * title=NULL, int x=-1, int y=-1 );

	void clear( uint8 r, uint8 g, uint8 b );
	void setPixel( int x, int y, uint8 r, uint8 g, uint8 b ) {
		piCheck( x>=0 && y>=0 );
		piCheck( x<w && y<h );
		buffer[y*w+x].r = r;
		buffer[y*w+x].g = g;
		buffer[y*w+x].b = b;
	}
	void getPixel( int x, int y, uint8 *r, uint8 *g, uint8 *b ) {
		piCheck( x>=0 && y>=0 );
		piCheck( x<w && y<h );
		*r = buffer[y*w+x].r;
		*g = buffer[y*w+x].g;
		*b = buffer[y*w+x].b;
	}


	/** Raster-Scan. */
	void scan( int dir, int neighbour );
	void sed4( void );
	void sed8( void );

	/** Cuisenaire's postprocess. */
	void cuisenaire( void );

	/** Perform EDT.*/
	void edt( void );


protected:

	void checkErrorQ1( int x, int y, pixel &a, pixel &b, pixel &c );
	void checkErrorQ2( int x, int y, pixel &a, pixel &b, pixel &c );
	void checkErrorQ3( int x, int y, pixel &a, pixel &b, pixel &c );
	void checkErrorQ4( int x, int y, pixel &a, pixel &b, pixel &c );

	void checkDiagonal( int x, int y, pixel &a, int xinc, int xmax, int yinc, int ymax );

public:

	int w;
	int h;
	pixel * buffer;

};



/*---------------------------------------------------------------------------
	MiniUI:
---------------------------------------------------------------------------*/

namespace MiniUI {

	void Init( void );
	void Shut( void );

	void AddWindow( const Image * , const char *, int x=-1, int y=-1 );
	void RemoveWindow( const Image * );

	bool Update( void );

};







#endif // _GPM_EDT_H_