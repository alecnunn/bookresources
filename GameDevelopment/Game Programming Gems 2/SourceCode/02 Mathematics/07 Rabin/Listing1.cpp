/* Copyright (C) Steve Rabin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steve Rabin, 2001"
 */
// Slightly optimized O((n^2-n)/2) time complexity
for( i=0; i<num_objects; i++ ) {
  for( j=i+1; j<num_objects; j++ ) {
     if( Distance(i, j) < Radius(i) + Radius(j) ) {
        //in collision
     }
  }
}
