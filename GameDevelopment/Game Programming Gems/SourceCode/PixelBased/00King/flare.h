/* Copyright (C) Yossarian King, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Yossarian King, 2000"
 */
/************************************************************

    Game Programming Gems
    Lens Flare Demo header file.

    Y. King / May 2000
    Electronic Arts Canada, Inc.

 ************************************************************/

#ifndef __FLARE_H
#define __FLARE_H

/* --- Defines --- */

#define FLARE_MAXELEMENTSPERFLARE         15

#define FLARE_RANGE(A,B)    ( (rand()%((B)-(A)+1)) + (A) )
#define FLARE_FRANGE(A,B)   ( ((float)(rand()&0xffffff)/(float)0xfffffe)*((B)-(A)) + (A) )

#define MAKEID(a,b,c,d)     (((a)<<24) | ((b)<<16) | ((c)<<8) | ((d)<<0))

/* --- Types --- */

typedef struct FLARE_ELEMENT_DEF
{
    struct TEXTURE_DEF  *texture;

    float           fDistance;        // Distance along ray from source (0.0-1.0)
    float           fSize;            // Size relative to flare envelope (0.0-1.0)
    unsigned int    argb;            // ARGB for intensity mapping
}
    FLARE_ELEMENT_DEF;

typedef struct FLARE_DEF
{
    float           fScale;     // Scale factor for adjusting overall size of flare elements.
    float           fMaxSize;   // Max size of largest element, as proportion of screen width (0.0-1.0)
    int             nPieces;    // Number of elements in use


    FLARE_ELEMENT_DEF    element[FLARE_MAXELEMENTSPERFLARE];
}
    FLARE_DEF;

/* --- Prototypes --- */

void    FLARE_randomize(FLARE_DEF *flare,
                        int nTextures,
                        int nPieces,
                        float fMaxSize,
                        unsigned int minColour,
                        unsigned int maxColour);
void    FLARE_render(FLARE_DEF *flare, int lx, int ly, int cx, int cy);

#endif
