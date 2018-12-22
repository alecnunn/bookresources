/************************************************************
 * Prim.cpp - Primitive Functions V2.04
 *
 * (c) Copyright 1996-2000 Sabarasa Entertainment
 * For internal use only.
 ************************************************************/

#include <dos.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <iostream.h>
#include <string.h>

#include <ddraw.h>

#include "support.hpp"
#include "blitbuf.hpp"
#include "letras.hpp"
#include "prim.hpp" 
#include "colores.hpp"

/* ------------------------ Drawing Primitives ---------------------------- */

/* Set the screen to a certain color */

void
FillScreen( pixelT color )
{
	DDBLTFX ddbltfx;

	ddbltfx.dwSize= sizeof( ddbltfx );
	ddbltfx.dwFillColor=color;

	vscreen->Blt( NULL, NULL, NULL,
		DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx );
}

/* Set an individual pixel - Don't use */

void
putPixel(int x, int y, pixelT color)
{
	RECT rect;
	DDBLTFX ddbltfx;

	rect.left=x;
	rect.top=y;
	rect.right=x+1;
	rect.bottom=y+1;

	ddbltfx.dwSize = sizeof( ddbltfx );
	ddbltfx.dwFillColor = color;

	vscreen->Blt( &rect, NULL, NULL,
		DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx );
}

/* Set an individual pixel checking boundaries */

void
putPixelC(int x, int y, pixelT color)
{
	if( x>=0 && x<PhysicalWidth && y>=0 && y<PhysicalHeight )
		putPixel(x,y,color);
}

/* Set an horizontal line on virtualscreen */

void
Hline(int x1, int x2, int y, pixelT color)
{
	RECT rect;
	DDBLTFX ddbltfx;

	rect.left=x1;
	rect.top=y;
	rect.right=x2+1;
	rect.bottom=y+1;

	ddbltfx.dwSize = sizeof( ddbltfx );
	ddbltfx.dwFillColor = color;

	vscreen->Blt( &rect, NULL, NULL,
		DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx );
}

/* Set a vertical line on virtualscreen */

void
Vline(int x, int y1, int y2, pixelT color)
{
	RECT rect;
	DDBLTFX ddbltfx;

	rect.left=x;
	rect.top=y1;
	rect.right=x+1;
	rect.bottom=y2+1;

	ddbltfx.dwSize = sizeof( ddbltfx );
	ddbltfx.dwFillColor = color;

	vscreen->Blt( &rect, NULL, NULL,
		DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx );
}

/* This function is used by Line() to determine the sign of a long */

signed int
sgn (int a) 
{
	if (a > 0) return +1;
	else if (a < 0) return -1;
	else return 0;
}

/* round() - This function is used by Line() to round a long to the        
             nearest integer.                                              */

int
round (long a)
{
  if ( (a - (int)a) < 0.5) return a;
    else return (a+1);
}

/* Draw a line */

void
Line(int x1, int y1, int x2, int y2, pixelT color)
{
  int u,s,v,d1x,d1y,d2x,d2y,m,n;
  int  i;

  u   = x2-x1;      // x2-x1
  v   = y2-y1;      // y2-y1
  d1x = sgn(u);   // d1x is the sign of u (x2-x1) (VALUE -1,0,1)
  d1y = sgn(v);   // d1y is the sign of v (y2-y1) (VALUE -1,0,1)
  d2x = sgn(u);   // d2x is the sign of u (x2-x1) (VALUE -1,0,1)
  d2y = 0;
  m   = abs(u);   // m is the distance between x1 and x2
  n   = abs(v);   // n is the distance between y1 and y2

  if (m<=n) {     // if the x distance is greater than the y distance
    d2x = 0;
    d2y = sgn(v); // d2y is the sign of v (x2-x1) (VALUE -1,0,1)
    m   = abs(v); // m is the distance between y1 and y2
    n   = abs(u); // n is the distance between x1 and x2
  }

  s = (int)(m / 2); // s is the m distance (either x or y) divided by 2

  for (i=0;i<round(m);i++)
  { // repeat this loop until it
	// is = to m (y or x distance)
    putPixelC(x1,y1,color);       // plot a pixel at the original x1, y1
    s += n;                  // add n (dis of x or y) to s (dis of x of y)
    if (s >= m) {            // if s is >= m (distance between y1 and y2)
      s -= m;
      x1 += d1x;
      y1 += d1y;
    }
    else {
      x1 += d2x;
      y1 += d2y;
    }
  }
  
}

/* Draw a Box from x1, y1 to x2, y2 */

void
Box( int x1, int y1, int x2, int y2, pixelT color)
{
	Hline(x1, x2, y1, color);
	Vline(x1, y1, y2, color);
	Hline(x1, x2, y2, color);
	Vline(x2, y1, y2, color);
}
