/*===========================================================================
	Title: Scan.cpp
	Module: GPM/EDT
	Author: Ignacio Castaño
	Description: Raster-Scan implementation.
	Changes:
		11/01/2003 - Ignacio Castaño
			File added.

===========================================================================*/

/*---------------------------------------------------------------------------
	Doc:
---------------------------------------------------------------------------*/

/** @file EDT.cpp
 * Raster-Scan implementation.
**/


/*---------------------------------------------------------------------------
	Includes:
---------------------------------------------------------------------------*/

#include "EDT.h"
//#include "MathLib.h"

// From MathLib
#include <math.h>

template <class T> 
T clamp( T x, T a, T b ) {
	return (x<a ? a : (x>b ? b : x));
}

#define EPSILON		0.000001



/*---------------------------------------------------------------------------
	Functions:
---------------------------------------------------------------------------*/


/** Compute squared euclidean distance. */
static float dist_euc( int ax, int ay, int bx, int by ) {
	float dx = float(bx-ax);
	float dy = float(by-ay);
	return dx*dx + dy*dy;
}


/** Check neighbour, this is the core of the Raster-Scan algorithm. */
static void CheckNeighbour( int x, int y, pixel * e, pixel * n ) {
	float d = dist_euc( x, y, n->x, n->y );
	if( d < e->dist ) {
		e->x = n->x;
		e->y = n->y;
		e->dist = d;
	}
}

/** Neighbour of P */
static bool EqualNearestNeighbour( const pixel &a, const pixel &b ) {
	return (a.x==b.x) && (a.y==b.y);
}


/** 
 * Test that the distance of pixel e in (x,y) is less than the distance 
 * to the neighbour of n.
**/
bool testWrite( int x, int y, pixel &e, pixel &n ) {
	float d = dist_euc( x, y, n.x, n.y );
	if( d < e.dist ) {
		e.x = n.x;
		e.y = n.y;
		e.dist = d;
		return true;
	}
	return false;
}


/*---------------------------------------------------------------------------
	Methods:
---------------------------------------------------------------------------*/

/** Scan the image using the 4sed method. */
void Image::sed4( void ) {
	int x, y;

	for( y=0; y<h; y++ ) {
		for( x=0; x<w; x++ ) {
			int x0 = clamp( x-1, 0, w-1 );
			int y0 = clamp( y-1, 0, h-1 );

			pixel * p = &buffer[y*w+x];

			CheckNeighbour( x, y, p, &buffer[y0*w+x] );
			CheckNeighbour( x, y, p, &buffer[y*w+x0] );
		}

		for( x=w-1; x>=0; x-- ) {
			int x1 = clamp( x+1, 0, w-1 );

			pixel * p = &buffer[y*w+x];

			CheckNeighbour( x, y, p, &buffer[y*w+x1] );
		}
	}

	for( y=h-1; y>=0; y-- ) {
		for( x=w-1; x>=0; x-- ) {
			int x1 = clamp( x+1, 0, w-1 );
			int y1 = clamp( y+1, 0, h-1 );

			pixel * p = &buffer[y*w+x];

			CheckNeighbour( x, y, p, &buffer[y*w+x1] );
			CheckNeighbour( x, y, p, &buffer[y1*w+x] );
		}

		for( x=0; x<w; x++ ) {
			int x0 = clamp( x-1, 0, w-1 );

			pixel * p = &buffer[y*w+x];

			CheckNeighbour( x, y, p, &buffer[y*w+x0] );
		}
	}

}


/** Scan the image using the 8sed method. */
void Image::sed8( void ) {
	int x, y;

	for( y=0; y<h; y++ ) {
		for( x=0; x<w; x++ ) {
			int x0 = clamp( x-1, 0, w-1 );
			int x1 = clamp( x+1, 0, w-1 );
			int y0 = clamp( y-1, 0, h-1 );

			pixel * p = &buffer[y*w+x];

			CheckNeighbour( x, y, p, &buffer[y0*w+x0] );
			CheckNeighbour( x, y, p, &buffer[y0*w+x] );
			CheckNeighbour( x, y, p, &buffer[y0*w+x1] );
			CheckNeighbour( x, y, p, &buffer[y*w+x0] );
		}

		for( x=w-1; x>=0; x-- ) {
			int x1 = clamp( x+1, 0, w-1 );

			pixel * p = &buffer[y*w+x];

			CheckNeighbour( x, y, p, &buffer[y*w+x1] );
		}
	}

	for( y=h-1; y>=0; y-- ) {
		for( x=w-1; x>=0; x-- ) {
			int x0 = clamp( x-1, 0, w-1 );
			int x1 = clamp( x+1, 0, w-1 );
			int y1 = clamp( y+1, 0, h-1 );

			pixel * p = &buffer[y*w+x];

			CheckNeighbour( x, y, p, &buffer[y*w+x1] );
			CheckNeighbour( x, y, p, &buffer[y1*w+x0] );
			CheckNeighbour( x, y, p, &buffer[y1*w+x] );
			CheckNeighbour( x, y, p, &buffer[y1*w+x1] );
		}

		for( x=0; x<w; x++ ) {
			int x0 = clamp( x-1, 0, w-1 );

			pixel * p = &buffer[y*w+x];

			CheckNeighbour( x, y, p, &buffer[y*w+x0] );
		}
	}

}



/** Transform the image according to the nearest neighbours. */
void Image::edt( void ) {

	int x, y;

	// Fill the empty pixels with the nearest neighbour.
	for( y=0; y<h; y++ ) {
		for( x=0; x<w; x++ ) {
			pixel * p = &buffer[y*w+x];

			int sx = p->x;
			int sy = p->y;
			if( sx>=0 && sx<w && sy>=0 && sy<h ) {
				p->r = buffer[sy*w + sx].r;
				p->g = buffer[sy*w + sx].g;
				p->b = buffer[sy*w + sx].b;
			}
			if( p->e ) {
				p->r = 255;
				p->g = 0;
				p->b = 0;
			}
		}
	}
}



/** Cuisenaire's postprocessing algorithm. */
void Image::cuisenaire( void ) {

	int x, y;

	for( y=1; y<h-1; y++ ) {
		for( x=1; x<w-1; x++ ) {

			pixel & a = buffer[y*w+x];

			// skip features.
			if( a.x == x && a.y == y ) continue;

			int x0 = clamp( x-1, 0, w-1 );
			int x1 = clamp( x+1, 0, w-1 );
			int y0 = clamp( y-1, 0, h-1 );
			int y1 = clamp( y+1, 0, h-1 );

			if( (x-a.x) > 0 ) {
				pixel & b = buffer[y*w+x1];

				if( !EqualNearestNeighbour(a, b) ) {
					if( (y-a.y) > 0 ) {
						if( !EqualNearestNeighbour(a, buffer[y1*w+x]) && !EqualNearestNeighbour(b, buffer[y1*w+x]) ) 
							checkErrorQ1( x, y, a, b, buffer[y1*w+x] );
					}
					else {
						if( !EqualNearestNeighbour(a, buffer[y0*w+x]) && !EqualNearestNeighbour(b, buffer[y0*w+x]) ) 
							checkErrorQ2( x, y, a, b, buffer[y0*w+x] );
					}
				}
			}
			else {
				pixel & b = buffer[y*w+x0];

				if( !EqualNearestNeighbour(a, b) ) {
					if( (y-a.y) > 0 ) {
						if( !EqualNearestNeighbour(a, buffer[y1*w+x]) && !EqualNearestNeighbour(b, buffer[y1*w+x]) ) 
							checkErrorQ4( x, y, a, b, buffer[y1*w+x] );
					}
					else {
						if( !EqualNearestNeighbour(a, buffer[y0*w+x]) && !EqualNearestNeighbour(b, buffer[y0*w+x]) ) 
							checkErrorQ3( x, y, a, b, buffer[y0*w+x] );
					}
				}
			}
		}
	}
}


/* Error correction in first quadrant. */
void Image::checkErrorQ1( int x, int y, pixel &a, pixel &b, pixel &c ) {

//	buffer[y*w+x].e = true;

	// if distance < 16 it's more efficient to check only the diagonals
	if( a.dist < 16*16 ) {
		checkDiagonal( x, y, a, 1, w-1, 1, h-1 );
	}
	else {
		int Ax, Ay;		// nearest neighbour of a relative to (x,y)
		int Bx, By;		// nearest neighbour of b relative to (x,y)
		int Cx, Cy;		// nearest neighbour of c relative to (x,y)

		Ax = x - a.x;
		Ay = y - a.y;
		Bx = x - b.x;
		By = y - b.y;
		Cx = x - c.x;
		Cy = y - c.y;

		float m, M, stepm, stepM;
		int i, imax, j;

		stepM = float(Bx - Ax) / float(Ay - By);
		M = 0.5f * (float(Ax + Bx) * stepM - float(Ay + By));

		stepm = float(Cx - Ax) / float(Ay - Cy);
		m = 0.5f * (float(Ax + Cx) * stepm - float(Ay + Cy));

		if( Bx==Cx ) return; // min-line is vertical -> no gray zone.

		if( fabs(stepM - stepm) < EPSILON ) {

			// TODO: check that stepM = stepm = 1!

			checkDiagonal( x, y, a, 1, w-1, 1, h-1 );
		}

		imax = int( (M - m) / (stepm - stepM) );

		for( i=1; i<=imax; i++ ) {
			if( x+i>w-1 ) break;
			m += stepm;
			M += stepM;
			for( j=(int)floor(m); j<=(int)ceil(M); j++ ) {
				if( y+j>h-1 ) break;
				testWrite( x+i, y+j, buffer[(y+j)*w+(x+i)], a );
			}
		}
	}
}

/*


get (x1,x2), (y1,y2) and (z1,z2) from the distance map;
check for singular cases;
compute m,M,stepm,stepM;
compute imax ;

for i = 1 -> imax
	m = m+stepm; M = M+stepM
	for j = m -> M
		testp = p + (i,j);
		if d( testp , NOP(p) ) < d ( testp, NOP(testp))
			NOP( testp ) = NOP ( p);

where d(p,q) is the Euclidean distance between pixels p and q.


*/


void Image::checkErrorQ2( int x, int y, pixel &a, pixel &b, pixel &c ) {

//	buffer[y*w+x].e = true;

	// if distance < 16 it's more efficient to check only the diagonals
	if( a.dist < 16*16 ) {
		checkDiagonal( x, y, a, 1, w-1, -1, 0 );
	}
	else {
		int Ax, Ay;		// nearest neighbour of a relative to (x,y)
		int Bx, By;		// nearest neighbour of b relative to (x,y)
		int Cx, Cy;		// nearest neighbour of c relative to (x,y)

		Ax = x - a.x;
		Ay = y - a.y;
		Bx = x - b.x;
		By = y - b.y;
		Cx = x - c.x;
		Cy = y - c.y;

		float m, M, stepm, stepM;
		int i, imax, j;

		stepM = float(Bx - Ax) / float(Ay - By);
		M = 0.5f * (float(Ax + Bx) * stepM - float(Ay + By));

		stepm = float(Cx - Ax) / float(Ay - Cy);
		m = 0.5f * (float(Ax + Cx) * stepm - float(Ay + Cy));

		if( Bx==Cx ) return; // min-line is vertical -> no gray zone.

		if( fabs(stepM - stepm) < EPSILON ) {
			checkDiagonal( x, y, a, 1, w-1, -1, 0 );
		}

		imax = int( (M - m) / (stepm - stepM) );

		for( i=1; i<=imax; i++ ) {
			if( x+i>w-1 ) break;
			m += stepm;
			M += stepM;
			for( j=(int)ceil(M); j>=(int)floor(m); j-- ) {
				if( y+j<0 ) break;
				testWrite( x+i, y+j, buffer[(y+j)*w+(x+i)], a );
			}
		}
	}
}


void Image::checkErrorQ3( int x, int y, pixel &a, pixel &b, pixel &c ) {

//	buffer[y*w+x].e = true;

	// if distance < 16 it's more efficient to check only the diagonals
	if( a.dist < 16*16 ) {
		checkDiagonal( x, y, a, -1, 0, -1, 0 );
	}
	else {
		int Ax, Ay;		// nearest neighbour of a relative to (x,y)
		int Bx, By;		// nearest neighbour of b relative to (x,y)
		int Cx, Cy;		// nearest neighbour of c relative to (x,y)

		Ax = x - a.x;
		Ay = y - a.y;
		Bx = x - b.x;
		By = y - b.y;
		Cx = x - c.x;
		Cy = y - c.y;

		float m, M, stepm, stepM;
		int i, imax, j;

		stepM = float(Bx - Ax) / float(Ay - By);
		M = 0.5f * (float(Ax + Bx) * stepM - float(Ay + By));

		stepm = float(Cx - Ax) / float(Ay - Cy);
		m = 0.5f * (float(Ax + Cx) * stepm - float(Ay + Cy));

		if( Bx==Cx ) return; // min-line is vertical -> no gray zone.

		if( fabs(stepM - stepm) < EPSILON ) {
			checkDiagonal( x, y, a, -1, 0, -1, 0 );
		}

		imax = int( (M - m) / (stepm - stepM) );

		for( i=-1; i>=imax; i-- ) {
			if( x+i<0 ) break;
			m -= stepm;
			M -= stepM;
			for( j=(int)ceil(M); j>=(int)floor(m); j-- ) {
				if( y+j<0 ) break;
				testWrite( x+i, y+j, buffer[(y+j)*w+(x+i)], a );
			}
		}
	}
}


void Image::checkErrorQ4( int x, int y, pixel &a, pixel &b, pixel &c ) {

//	buffer[y*w+x].e = true;

	// if distance < 16 it's more efficient to check only the diagonals
	if( a.dist < 16*16 ) {
		checkDiagonal( x, y, a, -1, 0, 1, h-1 );
	}
	else {
		int Ax, Ay;		// nearest neighbour of a relative to (x,y)
		int Bx, By;		// nearest neighbour of b relative to (x,y)
		int Cx, Cy;		// nearest neighbour of c relative to (x,y)

		Ax = x - a.x;
		Ay = y - a.y;
		Bx = x - b.x;
		By = y - b.y;
		Cx = x - c.x;
		Cy = y - c.y;

		float m, M, stepm, stepM;
		int i, imax, j;

		stepM = float(Bx - Ax) / float(Ay - By);
		M = 0.5f * (float(Ax + Bx) * stepM - float(Ay + By));

		stepm = float(Cx - Ax) / float(Ay - Cy);
		m = 0.5f * (float(Ax + Cx) * stepm - float(Ay + Cy));

		if( Bx==Cx ) return; // min-line is vertical -> no gray zone.

		if( fabs(stepM - stepm) < EPSILON ) {
			checkDiagonal( x, y, a, -1, 0, 1, h+1 );
		}

		imax = int( (M - m) / (stepm - stepM) );

		for( i=-1; i>=imax; i-- ) {
			if( x+i<0 ) break;
			m -= stepm;
			M -= stepM;
			for( j=(int)floor(m); j<=(int)ceil(M); j++ ) {
				if( y+j>w-1 ) break;
				testWrite( x+i, y+j, buffer[(y+j)*w+(x+i)], a );
			}
		}
	}
}


void Image::checkDiagonal( int x, int y, pixel &a, int xinc, int xlim, int yinc, int ylim ) {
	x += xinc;
	y += yinc;
	while( testWrite( x, y, buffer[y*w+x], a ) ) {
		if( x == xlim ) break;
		if( y == ylim ) break;
		x += xinc;
		y += yinc;
	};
}