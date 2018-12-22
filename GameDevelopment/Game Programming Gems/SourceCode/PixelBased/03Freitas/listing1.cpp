/* Copyright (C) Jorge Freitas, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Jorge Freitas, 2000"
 */
//
//------------
// defines
//------------
//
#define NUMBER_OF_ARGB_LISTS	4	/* number of vertex color lists */
#define NUMBER_OF_RADIANS	1024	/* number of radians in 360 degrees */

//
//---------------
// structures
//---------------
//
typedef struct
{
	float	alpha;
	float	red;
	float	green;
	float	blue;

}ARGB_DEF;

typedef struct
{
/* angle used for simulated lighting */
	int		angleOfRotation;				
/* number of vertices in object */
	int		nVertex;				
/* pointers to vertex color lists */
	ARGB_DEF	*pARGB[ NUMBER_OF_ARGB_LISTS ];	
}OBJECT_DEF;

//
//--------------
// variables
//--------------
//
ARGB_DEF *gpVertexColorBuffer;/* pointer to buffer used to store the calculated RGB's */
ARGB_DEF gAmbientLight;	/* global additive ambient light RGB */

int gLightOffset;		/* additive value used to offset the light "hot spot" */

//
//---------------
// functions
//---------------
//
/***************************

Function :  interpolateVertexRGBs

Linearly interpolates between two lists of vertex colors.

Input:

ARGB_DEF	*pSrcA		- pointer to first source vertex color list
ARGB_DEF	*pSrcB		- pointer to second source vertex color list
ARGB_DEF	*pDest		- pointer to storage for calculated vertex colors
int			nARGB		- number of vertex colors to interpolate
float		percentage	- amount to interpolate between two vertex tables (0.0 - 1.0)

Output:

Fills pDest with the calculated vertex colors

***************************/
void interpolateVertexRGBs(	ARGB_DEF *pSrcA,/* pointer to source vertex color data */
				ARGB_DEF *pSrcB,/* pointer to source vertex color data */
				ARGB_DEF *pDest,/* pointer to dest vertex color data */
				int nARGB,	/* number of vertex colors to interpolate */
				float percentage )	/* interpolation amount, 0 to 1 */
{
int	index;	/* index into arrays of ARGB_DEF's */
float	red,	/* temporary storage for calculated RGB's */
		green,
		blue;

	for ( index = 0; index < nARGB; index++ )
	{
//
// calculate interpolated ARGB
//
		red	= pSrcA[ index ].red + ( pSrcB[ index ].red - pSrcA[ index ].red ) * percentage;
		green	= pSrcA[ index ].green + ( pSrcB[ index ].green - pSrcA[ index ].green ) * percentage;
		blue	= pSrcA[ index ].blue + ( pSrcB[ index ].blue - pSrcA[ index ].blue ) * percentage;
//
// add ambient light
//
		red		+= gAmbientLight.red;
		green	+= gAmbientLight.green;
		blue	+= gAmbientLight.blue;
//
// clamp RGB's
//
		if ( red > 255.0 )
			red = 255.0;
		else
		{
			if ( red < 0.0 )
				red = 0.0;
		}

		if ( green > 255.0 )
			green = 255.0;
		else
		{
			if ( green < 0.0 )
				green = 0.0;
		}

		if ( blue > 255.0 )
			blue = 255.0;
		else
		{
			if ( blue < 0.0 )
				blue = 0.0;
		}
//
// store results
//
		pDest[ index ].red = red;
		pDest[ index ].green = green;
		pDest[ index ].blue = blue;
	}
}

/******************************

Function : calculateSimulatedLighting

Calculates the vertex colors used to represent the lighting at the given angle of rotation.

Input:

OBJECT_DEF	*pObject	- pointer to object structure

*******************************/
void calculateSimulatedLighting( OBJECT_DEF *pObject )
{
float	percentage;
int		quadrant;
int		angleOfRotation;

ARGB_DEF	*pSrcA,
			*pSrcB;

	angleOfRotation = ( pObject->angleOfRotation + gLightOffset ) % NUMBER_OF_RADIANS;

	percentage = (float)(angleOfRotation % NUMBER_OF_ARGB_LISTS ) / (float)( NUMBER_OF_RADIANS / NUMBER_OF_ARGB_LISTS );

	quadrant = angleOfRotation / ( NUMBER_OF_RADIANS / NUMBER_OF_ARGB_LISTS );

	pSrcA = pObject->pARGB[ quadrant ];

	if  ( quadrant == (NUMBER_OF_ARGB_LISTS - 1 ) )
		pSrcB = pObject->pARGB[ 0 ];
	else
		pSrcB = pObject->pARGB[ quadrant + 1 ];

	interpolateVertexRGBs( pSrcA, pSrcB, gpVertexColorBuffer, pObject->nVertex, percentage );
}

