/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/

//---------------------------------------------------------------------------------------
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glaux.lib")
#include "stdafx.h"

//---------------------------------------------------------------------------------------
REAL	GEpsilon	= .5f;
int     GMinDetDim  = 64;
int     GMaxDetDim  = 100;
int     GMinDetPolys= 32;
REAL    GGridSnap   = 32;

RGBA	CLR_NORMAL	= {150,150,150,255};
RGBA	CLR_DARK	= {128,128,128,255};
RGBA	CLR_CUT		= {180,0,0,255};
RGBA	CLR_WHITE	= {255,255,255,255};
RGBA	CLR_SELECT	= {240,240,255,255};
RGBA	CLR_SELBOX	= {0,0,255,255};
RGBA	CLR_CAMSTART={255,0,0,255};
RGBA	CLR_CAMEND  ={0,0,0,255};
RGBA	CLR_CAMFRUST={255,0,0,255};
RGBA	CLR_SCRSURFACE={0,0,255,75};
RGBA	CLR_CUT3DBRUSH={255,0,0,64};
RGBA	CLR_CUT3DBRUSHREV={0,255,0,54};
RGBA	CLR_PORTALS={0,0,128,200};
RGBA	CLR_PORTALS1={0,128,0,200};
RGBA	CLR_PORTALSLINE={255,0,255,255};
RGBA	CLR_DETAIL={192,192,0,255};
