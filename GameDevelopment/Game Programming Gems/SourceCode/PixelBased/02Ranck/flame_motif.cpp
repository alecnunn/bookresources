/* Copyright (C) Steven Ranck, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steven Ranck, 2000"
 */
// Example code that generates a flickering orange flame motif.

#include <math.h>
#include "mtxlib.h"
#include "mcolor.h"



#define FLAME_SPEED	6.0f	// Constants determined via experimentation:
#define FLAME_K1	(0.093f * FLAME_SPEED)
#define FLAME_K2	(0.137f * FLAME_SPEED)
#define FLAME_K3	(0.195f * FLAME_SPEED)
#define FLAME_K4	(0.106f * FLAME_SPEED)
#define FLAME_K5	(0.170f * FLAME_SPEED)
#define FLAME_K6	(0.287f * FLAME_SPEED)


// Generates the RGB color for a particular frame of the flame motif
// and stores it in *pColor. nGameFrameCounter is simply the frame number
// of the game's current frame and is incremented once per frame.
void GenerateFlameMotif( unsigned int nGameFrameCounter, Color_t *pColor ) {
	double dSinSum;
	float fIntensity, fAngle;

	fAngle = (float)nGameFrameCounter;

	dSinSum = 
		  sin( fAngle * FLAME_K1 )
		+ sin( fAngle * FLAME_K2 )
		+ sin( fAngle * FLAME_K3 )
		+ sin( fAngle * FLAME_K4 )
		+ sin( fAngle * FLAME_K5 )
		+ sin( fAngle * FLAME_K6 );

	fIntensity = (float)dSinSum * 0.1f;
	fIntensity += 0.7f;

	if( fIntensity > 1.0f ) {
		fIntensity = 1.0f;
	}

	pColor->fR = fIntensity;
	pColor->fG = fIntensity * 0.4f;
	pColor->fB = 0.0f;
}

