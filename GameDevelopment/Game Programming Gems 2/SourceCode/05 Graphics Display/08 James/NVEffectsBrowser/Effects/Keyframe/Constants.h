/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
//vertex shader constant declarations

//world-view-projection matrix
#define CV_WORLDVIEWPROJ_0 0
#define CV_WORLDVIEWPROJ_1 1
#define CV_WORLDVIEWPROJ_2 2
#define CV_WORLDVIEWPROJ_3 3

//light direction
#define CV_LIGHT_DIRECTION 4

//world matrix
#define CV_WORLD_0 5
#define CV_WORLD_1 6
#define CV_WORLD_2 7
#define CV_WORLD_3 8

//blend factors between keyframes
#define CV_BLEND_FACTORS 9

// {0.0, 0.5, 1.0, -1.0}
#define CV_CONSTANTS 10
