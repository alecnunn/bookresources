/* Copyright (C) William van der Sterren, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) William van der Sterren, 2001"
 */
float GetCoverFromThreatsInDirection(waypointid w) {
  float        property [kMaxDirections];
  unsigned int entry[kMaxDirections];

  // set all property and entry values to 0
  ...

  // pass one: collect and interpret relations
  for (waypointid w_to = 0; w_to < kMaxWaypointId; w_to++ ) {
    direction dir = GetDirectionForWaypoints( w, w_to );

    // check for line-of-fire from w_to to w
    if ( (w_to != w ) && (HasLineOfFire(w_to, w)) ) {
       entry[dir]++;

       // get value for relation (value in [0..1])
       float value = GetCoverFromThreatsAt( w, w_to );
       property [dir} += value;
    } 
  } 

  // pass two: level result into [0 .. 1] range
  for ( direction dir = 0; dir < kMaxSectors; dir++ ) {
    if ( entry [dir] > 0 ) {
      property [dir] /= entry [dir];
    } 
  } 
} 

float GetCoverFromThreatsAt(waypointid w, waypointid w_at) {
  for (waypointid w_n = 0; w_n < kMaxWaypointId; w_n++ ) {	
    // check for lack of line-of-fire to neighbor of w
      if ( IsNeighborOf(w, w_n) && (!HasLineOfFire(w_at, w_n)) )
        return 1.0;
  } 
  return 0; // no cover found
}
