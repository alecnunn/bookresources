/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
//Constants for PaletteSkin

#define CV_WORLDVIEWPROJ_0 0
#define CV_WORLDVIEWPROJ_1 1
#define CV_WORLDVIEWPROJ_2 2
#define CV_WORLDVIEWPROJ_3 3

#define CV_LIGHT_DIRECTION 4

#define CV_CONSTANTS 5

//each bone is three constants long, so bone 0 starts at 6, bone 1 starts at 9, etc.
#define CV_BONESTART 6

//Vertex components
#define V_POSITION  v0
#define V_WEIGHT0   v1
#define V_WEIGHT1   v2
#define V_INDICES   v3
#define V_NORMAL    v4
#define V_TEX       v5
#define V_S         v6
#define V_T         v7
#define V_SxT       v8

