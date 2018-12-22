/* Copyright (C) Steven Woodcock, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steven Woodcock, 2000"
 */
//*********************************************************************
// Name:     vector.cpp: 
// Purpose:  Class methods for the vector class (derived from vector3).
//*********************************************************************

//
// includes
//

#include <math.h>

#include "vector.h"

///////////////////////////
// Construction/Destruction
///////////////////////////

// Constructor #1.
// Creates a vector with predefined values.

vector::vector (void)
{
   x = y = z = 0.0f;
}

// Constructor #2.
// Creates a vector with predefined values.

vector::vector (float inX, float inY, float inZ)
{

   x = inX;
   y = inY;
   z = inZ;

}

vector::~vector (void)
{
}

///////////////////////
// additional operators
///////////////////////

// Assign.
// Put contents of v into vector.

vector &vector::operator = (const vector *v) 
{

  x = v->x;
  y = v->y;
  z = v->z;

  return *this;

}

// Add.
// Add two vectors

vector operator + (const vector &a, const vector &b) 
{

  vector ret(a);

  ret += b;

  return (ret);

}

// Subtract.
// Subtract one vector from another

vector operator - (const vector &a, const vector &b) 
{

  vector ret(a);

  ret -= b;

  return (ret);

}

// Cross Product.
// Compute cross product of the two vectors.

vector operator % (const vector &a, const vector &b)
{

  vector c(a.y*b.z - a.z*b.y,   // x component
           a.z*b.x - a.x*b.z,   // y component
           a.x*b.y - a.y*b.x);  // z component
  
  return (c);

}

// Dot Product.
// Compute dot product of the two vectors.  (Yes, this duplicates
// something already in mtxlib.h, but it's more compact syntatically.)

float operator * (const vector &a, const vector &b) 
{

   return (a.x*b.x + a.y*b.y + a.z*b.z);

}

/////////////////////
// additional methods
/////////////////////

// get direction of vector

void vector::GetDirection (void)
{

   this->normalize();

}

// ste the magnitude of the vector (without modifying
// direction, apportioned across the existing vector's magnitudes)

void vector::SetMagnitudeOfVector (float velocity)
{

   this->normalize();

   x *= velocity;
   y *= velocity;
   z *= velocity;

}

// get distance between two points

float vector::GetDist (vector pos1, vector pos2)
{

   vector3  temp = pos1 - pos2;

   return (temp.length());

}

// get angle between two vectors

float vector::GetAngleBetween (vector vec1, vector vec2) 
{

   float mag1 = vec1.length();
   float mag2 = vec2.length();
   float dotproduct = DotProduct(vec1, vec2);

   return (float) (acos (dotproduct/(mag1 * mag2)));
}
