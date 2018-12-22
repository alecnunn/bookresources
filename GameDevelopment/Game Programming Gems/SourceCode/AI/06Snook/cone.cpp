/* Copyright (C) Greg Snook, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg Snook, 2000"
 */
// cone.cpp
// Geometry Data taken from cone.lwo
 
// This file contains geometry information for the cone objects used in the navimesh sample program
// The navimesh.cpp file links to this data externally to create the demo objects

#include "mtxlib.h" // definition of vector3
 
unsigned long cone_totalpoints = 7;
 
vector3 cone_points[] = 
{
 vector3(5.0000f, 10.0000f, 0.0000f),
 vector3(2.5000f, 10.0000f, 4.3301f),
 vector3(-2.5000f, 10.0000f, 4.3301f),
 vector3(-5.0000f, 10.0000f, 0.0000f),
 vector3(-2.5000f, 10.0000f, -4.3301f),
 vector3(2.5000f, 10.0000f, -4.3301f),
 vector3(0.0000f, 0.0000f, 0.0000f),
};
 
 
 
unsigned short cone_polys[][3] = 
{
 { 5, 4, 3},
 { 0, 5, 3},
 { 0, 3, 2},
 { 0, 2, 1},
 { 6, 0, 1},
 { 6, 1, 2},
 { 6, 2, 3},
 { 6, 3, 4},
 { 6, 4, 5},
 { 6, 5, 0},
};
 
 
unsigned long cone_totalpolys = 10;
