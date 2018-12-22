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
// Name:     CVector.h. 
// Purpose:  Class definitions and prototypes for the vector class.
//           vector is derived from the mtxlib.h "vector3" class;
//           I needed some additional methods not provided by that class.
//*********************************************************************

#ifndef _CVECTOR_H
#define _CVECTOR_H

#include "mtxlib.h"

class vector : public vector3
{

public:

   ///////////////////////////////
   // constructors and destructors
   ///////////////////////////////
   
   // Constructor #1.
   // Creates a vector with predefined values.

   vector (void);

   // Constructor #2.
   // Creates a vector with predefined values.

	vector (float inX, float inY, float inZ);

   // Destructor

   virtual ~vector();

   ///////////////////////
   // additional operators
   ///////////////////////

   vector       &operator = (const vector *a);
   
   friend vector operator + (const vector &a, const vector &b);  // add a and b (a + b)
   friend vector operator - (const vector &a, const vector &b);  // subtract b from a ( a - b)
   friend vector operator % (const vector &a, const vector &b);  // cross product of a and b (a % b)
   friend float  operator * (const vector &a, const vector &b);  // dot product of a and b (a * b)
  
   /////////////////////
   // additional methods
   /////////////////////

   // GetDirection.
   // get direction of vector

   void GetDirection (void);

   // SetMagnitudeOfVector.
   // set the magnitude of the vector (without modifying
   // direction, apportioned across the existing vector's magnitudes)

   void SetMagnitudeOfVector (float velocity);

   // GetDist.
   // get distance between two points.
   
   float GetDist (vector pos1, vector pos2);

   // GetAngleBetween.
   // get angle between two vectors.

   float GetAngleBetween (vector vec1, vector vec2);

};

#endif