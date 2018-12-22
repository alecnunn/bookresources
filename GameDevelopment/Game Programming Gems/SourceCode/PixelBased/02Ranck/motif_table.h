/* Copyright (C) Steven Ranck, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steven Ranck, 2000"
 */
// Application-specific motif table.

#ifndef _MOTIF_TABLE_H_
#define _MOTIF_TABLE_H_ 1

#include "mcolor.h"


typedef enum {
	MOTIF_FLAME0,
	MOTIF_FLAME1,
	MOTIF_FLAME2,
	MOTIF_FLAME3,
	MOTIF_SWITCH0,
	MOTIF_SWITCH1,
	MOTIF_LIGHTNING,
	MOTIF_DARK_RED,
	MOTIF_BRIGHT_BLUE,

	MOTIF_COUNT
} Motif_e;


extern Color_t aMotifTable[MOTIF_COUNT];


#endif
