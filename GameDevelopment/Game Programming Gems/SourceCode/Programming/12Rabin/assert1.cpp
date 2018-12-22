/* Copyright (C) Steve Rabin, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steve Rabin, 2000"
 */
#include <assert.h>
void VectorNormalize( Vec* src, Vec* dst )
{
   float length;

   assert( src != 0 && "VectorNormalize: src vector pointer is NULL" );
   assert( dst != 0 && "VectorNormalize: dst vector pointer is NULL" );

   length = sqrt( (src->x*src->x) + (src->y*src->y) + (src->z*src->z) );

   assert( length != 0 && "VectorNormalize: src vector is zero length" );

   dst->x = src->x / length;
   dst->y = src->y / length;
   dst->z = src->z / length;
}
