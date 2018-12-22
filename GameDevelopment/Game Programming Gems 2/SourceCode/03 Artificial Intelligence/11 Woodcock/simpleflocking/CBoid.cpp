/* Copyright (C) Steven Woodcock, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steven Woodcock, 2001"
 */
//*********************************************************************
// Name:     CBoid.cpp
// Purpose:  Class methods for individual boids (flock members).
//*********************************************************************

//
// includes
//

#include <stdio.h>
#include <math.h>

#include "CFlock.h"
#include "CBoid.h"
#include "CBox.h"
#include "vector.h"

//
// static variable initialization
//

// visible friends list (work space reused by each flock boid)

CBoid * CBoid::VisibleFriendsList[] = {NULL};

//
// constructor and destructor methods
//

// Constructor #1.
// Creates an individual boid with randomized position and velocity.

CBoid::CBoid (short id_v)
{

#ifdef BOID_DEBUG
   myprintf("\nCBoid constructor #1 called for boid %d.\n",id_v);
#endif

   m_id               = id_v;
   m_perception_range = Default_Perception_Range;

   // generate random position

   m_pos.x = RAND() * CBox::WorldPtr->GetBoxWidth()/3;
   m_pos.y = RAND() * CBox::WorldPtr->GetBoxHeight()/3;
   m_pos.z = RAND() * CBox::WorldPtr->GetBoxLength()/3;

   // flip positions for greater randomness

   if (RAND() > 0.5f) m_pos.x *= -1;
   if (RAND() > 0.5f) m_pos.y *= -1;
   if (RAND() > 0.5f) m_pos.z *= -1;

   // generate random velocity

   m_vel.x = RAND();
   m_vel.z = RAND();

   // flip velocities for greater randomness

   if (RAND() > 0.5f) m_vel.x *= -1;
   if (RAND() > 0.5f) m_vel.z *= -1;

   // compute speed based on velocity

   m_speed = m_vel.length();

   // zero out orientation

   m_ang.x = m_ang.y = m_ang.z = 0;

   // other values

   m_num_flockmates_seen       = 0;
   m_nearest_flockmate         = NULL;
   m_dist_to_nearest_flockmate = INFINITY;

   m_num_enemies_seen          = 0;
   m_nearest_enemy             = NULL;
   m_dist_to_nearest_enemy     = INFINITY;

   m_next = m_prev = NULL;

#ifdef BOID_DEBUG
   PrintData();
#endif

}  

// Constructor #2.
// Creates an individual boid with specific position and velocity.

CBoid::CBoid (short id_v,
              vector * pos_v, vector * vel_v, vector * ang_v)
{

#ifdef BOID_DEBUG
   myprintf("\nCBoid constructor #2 called for boid %d.\n",id_v);
#endif

   m_id               = id_v;
   m_perception_range = Default_Perception_Range;

   m_pos = pos_v;
   m_vel = vel_v;
   m_ang = ang_v;

   m_speed = vel_v->length();

   m_num_flockmates_seen       = 0;
   m_nearest_flockmate         = NULL;
   m_dist_to_nearest_flockmate = INFINITY;

   m_num_enemies_seen          = 0;
   m_nearest_enemy             = NULL;
   m_dist_to_nearest_enemy     = INFINITY;

   m_next = m_prev = NULL;

#ifdef BOID_DEBUG
   PrintData();
#endif

}  

// Destructor.
// Destroys indicated boid.

CBoid::~CBoid (void)
{

#ifdef BOID_DEBUG
   myprintf("\nCBoid destructor called for boid %x\n",this);
#endif

}

//////////////////////////
// public flocking methods
//////////////////////////

// FlockIt.
// Used for frame-by-frame updates; no time deltas on positions.

void CBoid::FlockIt (int flock_id, CBoid *first_boid)
{

   vector acc;

#ifdef BOID_DEBUG
   myprintf("\n=====\nUpdate for %d\n", m_id);
   myprintf("pos before = %f %f %f\n",m_pos.x,m_pos.y,m_pos.z);
#endif

   // Step 1:  Update our position.
   // Update our position based on the velocity
   // vector we computed last time around.

   m_oldpos = m_pos;     // save off our previous position

   m_pos += m_vel;      // apply velocities.

   // Step 2:  SeeFriends.
   // Determine if we can see any of our flockmates.

   SeeFriends (first_boid);

   // Step 3:  Flocking behavior.
   // Do we see any of our flockmates?  If yes, it's time to implement
   // the first Three Rules (they don't matter if we can't see anybody)

   if (m_num_flockmates_seen) {

      // Step 4:  Implement Rule #1 (Separation).
      // Try to maintain our desired separation distance from our nearest flockmate.

      AccumulateChanges (acc, KeepDistance());

      // Step 5:  Implement Rule #2 (Alignment).
      // Try to move the same way our nearest flockmate does.

      AccumulateChanges (acc, MatchHeading());

      // Step 6:  Implement Rule #3 (Cohesion).
      // Try to move towards the center of the flock.

      AccumulateChanges (acc, SteerToCenter());
   
   }

   // Step 6:  The Fourth Rule (enemies)
   // If we're supposed to react to enemy flocks, determine
   // if there are any then avoid them if we can.

   if (ReactToEnemies) {
      SeeEnemies (flock_id);
      AccumulateChanges (acc, FleeEnemies());
   }

   // Step 7:  Cruising.
   // Flockmates or not, enemies or not, figure out
   // how fast we'd move if it were all up to us.

   AccumulateChanges (acc, Cruising());

   // Step 8:  Constrain acceleration
   // If our acceleration change is more than we allow, constrain it

   if (acc.length() > MaxChange) {

#ifdef BOID_DEBUG
      myprintf("WARNING:  constraining acceleration for boid %x!\n",this);
#endif

      // definitely too much...constrain to maximum change

      acc.SetMagnitudeOfVector (MaxChange);

   }

   // Step 9:  Implementation.
   // Here's where we apply our newly computed acceleration vector
   // to create a new velocity vector to use next update cycle.

   m_oldvel = m_vel;     // save off our previous velocity

   // now add in the acceleration

   m_vel += acc;

   // Step 10:  constraint Y velocity changes.
   // Attempt to restrict flight straight up/down by damping out Y axis velocity.
   // This isn't strictly necessary, but does lead to more realistic looking flight.

   m_vel.y *= MaxUrgency;

   // Step 11:  Constrain our speed.
   // If we're moving faster than we're allowed to move, constrain our velocity.

   if ((m_speed = m_vel.length()) > MaxSpeed) {

      // definitely too fast...constrain to maximum speed

#ifdef BOID_DEBUG
      myprintf("WARNING:  constraining speed for boid %x!\n",this);
      myprintf("          current speed = %f new speed = %f\n",m_speed, MaxSpeed);
#endif

      m_vel.SetMagnitudeOfVector (MaxSpeed);

      m_speed = MaxSpeed;

   }

   // Step 12:  Compute roll/pitch/yaw.
   // Compute our orientation after all this speed adjustment nonsense.

   ComputeRPY();

   // Step 13:  World boundaries.
   // If we've wandered outside the world bounds put us back in.  You might
   // not need to do this for your world, but that depends on your implementation.

   WorldBound();

#ifdef BOID_DEBUG
   myprintf("final position     = %f %f %f\n",m_pos.x, m_pos.y, m_pos.z);
   myprintf("final velocity     = %f %f %f\n",m_vel.x, m_vel.y, m_vel.z);
   myprintf("final acceleration = %f %f %f\n",acc.x, acc.y, acc.z);
#endif

}

///////////////////////////
// private flocking methods
///////////////////////////

// Cruising.
// Generates a vector indicating how a flock boid would
// like to move, if it were all up to him and he was under
// no other influences of any kind.

vector CBoid::Cruising (void)
{
   vector  change = m_vel;

   float  diff = (m_speed - DesiredSpeed)/ MaxSpeed;
   float  urgency = (float) fabs(diff);

#ifdef BOID_DEBUG
   myprintf("\nInside Cruising\n");
   myprintf("   diff = %f  urgency = %f\n",diff, urgency);
   myprintf("   m_speed = %f  desired speed = %f\n",m_speed, DesiredSpeed);
   myprintf("   initial change vector from Cruising = %f %f %f\n",change.x, change.y, change.z);
   if (diff > 0) {
      myprintf("   slowing down to meet cruising speed...\n");
   } else {
      myprintf("   speeding up to meet cruising speed...\n");
   }
#endif

   // constrain the urgency level

   if (urgency < MinUrgency) urgency = MinUrgency;
   if (urgency > MaxUrgency) urgency = MaxUrgency;

   // Now add some "jitter" so that each boid has a 
   // bit of randomness just to keep things interesting.
   // This will also get us moving if we happen to start
   // things standing perfectly still (which is sorta boring).

   float jitter = RAND();

   if (jitter < 0.45f) {
      change.x += MinUrgency * SIGN(diff);
   } else if (jitter < 0.90f) {
      change.z += MinUrgency * SIGN(diff);
   } else {
      change.y += MinUrgency * SIGN(diff);   // we don't like vertical motion all that much
   }

#ifdef BOID_DEBUG
   myprintf("   intermediate change vector from Cruising = %f %f %f\n",change.x, change.y, change.z);
#endif

   // compute velocity change necessary to get to our desired cruising speed

   change.SetMagnitudeOfVector ((urgency * (diff > 0 ? -1 : 1) ));

#ifdef BOID_DEBUG
   myprintf("   final change vector from Cruising = %f %f %f\n",change.x, change.y, change.z);
#endif

   return (change);
}

// FleeEnemies.
// Generates a vector for a flock boid to avoid the 
// nearest enemy (boid of a different flock) it sees.

vector CBoid::FleeEnemies (void)
{
   vector change;

#ifdef BOID_DEBUG
   myprintf("\nInside FleeEnemies\n");
#endif

   // test:  Are we too close to our nearest enemy?

   if (m_dist_to_nearest_enemy < KeepAwayDist) {

#ifdef BOID_DEBUG
      myprintf("   too close to %x\n",m_nearest_enemy);
#endif

      // yep...compute vector away from enemy

      change = m_pos - m_nearest_enemy->m_pos;

   }

   // return change vector

   return (change);

#ifdef BOID_DEBUG
   myprintf("   final change vector from Cruising = %f %f %f\n",change.x, change.y, change.z);
#endif

}

// KeepDistance.
// Generates a vector for a flock boid to maintain his
// desired separation distance from the nearest flockmate he sees.

vector CBoid::KeepDistance (void)
{

   float ratio = m_dist_to_nearest_flockmate/SeparationDist;

   // compute vector towards our nearest buddy

   vector change = m_nearest_flockmate->m_pos - m_pos;   

#ifdef BOID_DEBUG
   myprintf("\nInside KeepDistance\n");
#endif

   // constrain computed ratio to our min/max Urgency levels

   if (ratio < MinUrgency) ratio = MinUrgency;
   if (ratio > MaxUrgency) ratio = MaxUrgency;

   // test:  are we too close to our nearest flockmate?

#ifdef BOID_DEBUG
   myprintf("   dist_to_nearest_flockmate = %f  Sep = %f  ratio = %f\n",
      m_dist_to_nearest_flockmate,SeparationDist,ratio);
#endif

   if (m_dist_to_nearest_flockmate < SeparationDist) {

#ifdef BOID_DEBUG
      myprintf("   too close!\n");
#endif
      
      // too close...move away from our neighbor

      change.SetMagnitudeOfVector (-ratio);

   } else if (m_dist_to_nearest_flockmate > SeparationDist) {

#ifdef BOID_DEBUG
      myprintf("   too far away!\n");
#endif

      // too far away...move towards our neighbor

      change.SetMagnitudeOfVector (ratio);

   } else {

#ifdef BOID_DEBUG
      myprintf("   just right!\n");
#endif

      // in the UNLIKELY event we're exactly the right distance away, do nothing

      change.SetMagnitudeOfVector (0.0);
   }

   // return change vector

#ifdef BOID_DEBUG
   myprintf("   final change vector from KeepDistance = %f %f %f\n",change.x, change.y, change.z);
#endif

   return (change);
}

// MatchHeading.
// Generates a vector for a flock boid to try
// to match the heading of its nearest flockmate.

vector CBoid::MatchHeading (void)
{

   // copy the heading of our nearest buddy

   vector change = m_nearest_flockmate->m_vel;

#ifdef BOID_DEBUG
   myprintf("\nInside MatchHeading\n");
#endif

   // normalize and then scale our vector change a bit...after
   // all we can't instantly snap to a new heading, can we?

   change.SetMagnitudeOfVector (MinUrgency);

   // return change vector

#ifdef BOID_DEBUG
   myprintf("   final change vector from MatchHeading = %f %f %f\n",change.x, change.y, change.z);
#endif

   return (change);

}

// SeeEnemies.
// Determines which enemy flock boids a given flock boid can see.

int CBoid::SeeEnemies (int flock_id)
{

   float dist;

   CBoid *enemy;

#ifdef BOID_DEBUG
   myprintf("\nInside SeeEnemies\n");
#endif

   // initialize enemy data

   m_num_enemies_seen      = 0;
   m_nearest_enemy         = NULL;
   m_dist_to_nearest_enemy = INFINITY;

   // loop over each flock and determine the closest one we can see

   for (int i = 0; i < CFlock::FlockCount; i++) {

      // test:  if this is OUR flock, skip it

      if (i == flock_id) continue;

#ifdef VISIBILITY_DEBUG
   myprintf("   Testing to see if %x can see anybody in flock %d\n",this,i);
#endif

      // not our flock, so check it out

      enemy = CFlock::ListOfFlocks[i]->GetFirstMember();

      while (enemy != NULL) {
      
#ifdef VISIBILITY_DEBUG
      myprintf("   looking at %x\n",enemy);
#endif

         // if this enemy is visible...

         if ((dist = CanISee(enemy)) != INFINITY) {

            // I can see it..increment counter

            m_num_enemies_seen++;

            // Test:  Closest enemy?

            if (dist < m_dist_to_nearest_enemy) {
               
               // yes...save it off

               m_dist_to_nearest_enemy = dist;
               m_nearest_enemy = enemy;
            }
         }

         // get next enemy in flock

         enemy = enemy->GetNext();

      }
   }

#ifdef VISIBILITY_DEBUG
   myprintf("\n");
   myprintf("   total enemies seen = %d\n",m_num_enemies_seen);
   myprintf("   nearest enemy is %x at %f\n",m_nearest_enemy,m_dist_to_nearest_enemy);
#endif

   return (m_num_enemies_seen);

}

// SeeFriends.
// Determines which flockmates a given flock boid can see.

int CBoid::SeeFriends (CBoid *first_boid)
{

   float dist;

   CBoid *flockmate = first_boid;

#ifdef BOID_DEBUG
   myprintf("\nInside SeeFriends\n");
#endif

#ifdef VISIBILITY_DEBUG
   myprintf("   Building visibilty list for %x...\n",this);
#endif

   // clear the existing visibility list of any holdover from last round

   ClearVisibleList();

   // now figure out who we can see

   while (flockmate != NULL) {

      // Test:  Within sight of this boid?

#ifdef VISIBILITY_DEBUG
      myprintf("   looking at %x\n",flockmate);
#endif

      if ((dist = CanISee(flockmate)) != INFINITY) {

         // add it to the list

         AddToVisibleList(flockmate);

         // Test:  If this guy is closer than the current
         // closest, make him the current closest

         if (dist < m_dist_to_nearest_flockmate) {
            m_dist_to_nearest_flockmate = dist;
            m_nearest_flockmate = flockmate;
         }
      }

      // next flockmate

      flockmate = flockmate->GetNext();
   }

#ifdef VISIBILITY_DEBUG
   myprintf("\n");
   myprintf("   total flockmates seen = %d\n",m_num_flockmates_seen);
   myprintf("   nearest flockmate is %x at %f\n",
      m_nearest_flockmate, m_dist_to_nearest_flockmate);
#endif

   return (m_num_flockmates_seen);
}

// SteerToCenter.
// Generates a vector to guide a flock boid towards
// the "center of mass" of the flockmates he can see.

vector CBoid::SteerToCenter (void)
{

   vector center, change;

#ifdef BOID_DEBUG
   myprintf("\nInside SteerToCenter\n");
#endif

   // walk down the visibility list and sum up their position vectors

   for (int i = 0; i < m_num_flockmates_seen; i++) {
      if (VisibleFriendsList[i] != NULL) center += VisibleFriendsList[i]->m_pos;
   }

#ifdef BOID_DEBUG
   myprintf("   perceived center before averaging = %f %f %f\n",center.x, center.y, center.z);
   myprintf("   num_flockmates_seen = %d\n",m_num_flockmates_seen);
#endif

   // average the positions to get the perceived center of the flock

   center /= m_num_flockmates_seen;

#ifdef BOID_DEBUG
   myprintf("   perceived center after averaging = %f %f %f\n",center.x, center.y, center.z);
#endif

   // now that we have the perceived center, computer a vector towards it

   change = center - m_pos;

   // Scale the change vector so we don't turn on a dime..

   change.SetMagnitudeOfVector (MinUrgency);

#ifdef BOID_DEBUG
   myprintf("   final change vector from SteerToCenter = %f %f %f\n",
      change.x, change.y, change.z);
#endif

   // return change vector

   return (change);
}

// WorldBound.
// Implements a world boundary so that flocks don't fly
// infinitely far away from the camera, instead remaining 
// in a nice viewable area.  It does this by wrapping flock
// boids around to the other side of the world, so (for example)
// they move out the right and return on the left.
// Returns:  nothing.

void CBoid::WorldBound (void)
{

   float maxX = CBox::WorldPtr->GetBoxWidth()/2;
   float maxY = CBox::WorldPtr->GetBoxHeight()/2;
   float maxZ = CBox::WorldPtr->GetBoxLength()/2;

   float minX = -maxX;
   float minY = -maxY;
   float minZ = -maxZ;

#ifdef BOID_DEBUG
   myprintf("\nInside Worldbound\n");
#endif

   // test position of flock boid and 
   // 'warp' if they've strayed out of bounds

   if (m_pos.x > maxX) {
      m_pos.x = minX;
   } else if (m_pos.x < minX) {
      m_pos.x = maxX;
   
   } else if (m_pos.y > maxY) {
      m_pos.y = minY;
   } else if (m_pos.y < minY) {
      m_pos.y = maxY;
   
   } else if (m_pos.z > maxZ) {
      m_pos.z = minZ;
   } else if (m_pos.z < minZ) {
      m_pos.z = maxZ;
   }

}

////////////////////////
// miscellaneous methods
////////////////////////

// AccumulateChanges.
// Adds vector values in changes into the accumumlator vector.
// Returns:  magnitude of accumulator vector after adding changes.

float CBoid::AccumulateChanges (vector &accumulator, vector changes)
{

#ifdef BOID_DEBUG
   myprintf("\nInside AccumulateChanges\n");
#endif  

   // apply the change

   accumulator += changes;

   //accumulator.x += changes.x;
   //accumulator.y += changes.y;
   //accumulator.z += changes.z;

   return (accumulator.length());

}

// AddToVisibleList.
// Adds a boid to the list of visible boids.
// This visibility list is regenerated for each boid each update cycle,
// and acts much like a push-down queue; the latest boid added to the
// list becomes the first one when the list is sequentially accessed.
// Mostly I did this for speed reasons, as this allows for fast inserts
// (and we don't delete from this list, we just rebuild it each update cycle).

void CBoid::AddToVisibleList (CBoid *ptr)
{

   // test:  do we see enough buddies already?

   if (m_num_flockmates_seen < Max_Friends_Visible) {

      // nope--we can add to this one to the list

      VisibleFriendsList[m_num_flockmates_seen] = ptr;

      // increment counter

      m_num_flockmates_seen++;

   }

#ifdef VISIBILITY_DEBUG
   int i;

   myprintf("visibility list for %x after adding %x:\n",this,ptr);

   for (i = 0; i < m_num_flockmates_seen; i++) {
      if (VisibleFriendsList[i] != NULL) {
         myprintf("VFL = %x\n",VisibleFriendsList[i]);
      } else {
         break;
      }
   }
#endif

}

// ClearVisibleList.
// Clears the visibility list.

void CBoid::ClearVisibleList (void)
{

   // walk down the list and clear each visible entry

   for (int i = 0; i < Max_Friends_Visible; i++) {
      VisibleFriendsList[i] = NULL;
   }

   // clear other visibility info

   m_num_flockmates_seen       = 0;
   m_nearest_flockmate         = NULL;
   m_dist_to_nearest_flockmate = INFINITY;

}

// Determine whether a given invoking boid can see the boid in question.
// Returns the distance to the boid.

float CBoid::CanISee (CBoid *ptr)
{

#ifdef VISIBILITY_DEBUG
   myprintf("\n   Inside CanISee.\n");
#endif

   // Test:  if we're looking at ourselves, never mind

   if (this == ptr) return (INFINITY);

   // figure out distance

   float dist = (m_pos.GetDist(m_pos, ptr->m_pos));

#ifdef VISIBILITY_DEBUG
   myprintf("   dist between %x and %x = %f\n",this,ptr,dist);
#endif

   // if we're using truth data, don't bother to check sighting

   if (UseTruth) return (dist);

   // not using truth, so check sighting

   if (m_perception_range > dist) return (dist);

   // fell through; can't see it

   return (INFINITY);

}

// ComputeRPY.
// Computes the roll/pitch/yaw of the flock boid based on its
// latest velocity vector changes.  Roll/pitch/yaw are stored in
// the "ang" data boid as follows:
//   pitch is about the x axis
//   yaw is about the y axis
//   roll is about the z axis
//
// All calculations assume a right-handed coordinate system:
//   +x = through the left side of the object
//   +y = up
//   +z = through the nose of the model
//
// All angles are computed in radians.
//
// NOTE:  This algorithm was generously provided by Christopher Kline, who originally
//        developed it for *his* flocking applications.  Thanks Christopher!

void CBoid::ComputeRPY (void)
{

   float  roll, pitch, yaw;

   // Determine the direction of the lateral acceleration.

   vector lateralDir = (m_vel % (m_vel - m_oldvel)) % m_vel;

   lateralDir.GetDirection();

   // Set the lateral acceleration's magnitude. The magnitude is the vector
   // projection of the appliedAcceleration vector onto the direction of the
   // lateral acceleration).
  
   float lateralMag = (m_vel - m_oldvel) * lateralDir;
  
   // compute roll

   if (lateralMag == 0) {
      roll = 0.0f;  
   } else {
      roll = (float) -atan2(GRAVITY, lateralMag) + HALF_PI;
   }

   // compute pitch

   pitch = (float) -atan(m_vel.y / sqrt((m_vel.z*m_vel.z) + (m_vel.x*m_vel.x)));

   // compute yaw

   yaw = (float) atan2(m_vel.x, m_vel.z);

   // store them

   m_ang.x = pitch;

   m_ang.y = yaw;

   m_ang.z = roll;

#ifdef BOID_DEBUG
   myprintf("   roll = %f  pitch = %f  yaw = %f\n",roll, pitch, yaw);
#endif

}

// SetNext.
// Set the "next" pointer of an individual boid.

void CBoid::SetNext (CBoid *ptr)
{

   m_next = ptr;

}

// GetNext.
// Returns the "next" pointer of the invoking boid.

CBoid * CBoid::GetNext()
{

   return (m_next);

}

// SetPrev.
// Set the "prev" pointer of an individual boid.

void CBoid::SetPrev (CBoid *ptr)
{

   m_prev = ptr;

}

// GetOrient.
// Returns the orientation of the boid in question.

vector * CBoid::GetOrient (void)
{

   return (&m_ang);

}

// GetPos.
// Returns the position of the boid in question.

vector * CBoid::GetPos (void)
{

   return (&m_pos);

}

// LinkOut.
// Removes a boid from a list.

void CBoid::LinkOut ()
{

   // test location of boid

   if ((m_next == NULL) && (m_prev == NULL)) {

      SetNext(NULL);
      SetPrev(NULL);

   } else if (m_next == NULL) {

      m_prev->SetNext(NULL);

   } else if (m_prev == NULL) {
      
      m_next->SetPrev(NULL);

   } else {

      m_prev->SetNext(m_next);
      m_next->SetPrev(m_prev);
   }
}

// PrintData
// Dumps all data describing a given boid.

void CBoid::PrintData (void)
{

   myprintf("===================\n");
   myprintf("Data for boid = %d @ %x\n", m_id,this);

   myprintf(" perception_range = %f\n", m_perception_range);

   myprintf(" pos x, y, z = %f %f %f\n", m_pos.x, m_pos.y, m_pos.z);
   myprintf(" vel x, y, z = %f %f %f\n", m_vel.x, m_vel.y, m_vel.z);
   myprintf(" roll, pitch, yaw = %f %f %f\n", m_ang.z, m_ang.x, m_ang.y);

   myprintf(" speed                     = %f\n", m_speed);

   myprintf(" num_flockmates_seen       = %d\n", m_num_flockmates_seen);
   myprintf(" nearest_flockmate         = %x\n", m_nearest_flockmate);
   myprintf(" dist_to_nearest_flockmate = %f\n", m_dist_to_nearest_flockmate);

   myprintf(" num_enemies_seen          = %d\n", m_num_enemies_seen);
   myprintf(" nearest_enemy             = %x\n", m_nearest_enemy);
   myprintf(" dist_to_nearest_enemy     = %f\n", m_dist_to_nearest_enemy);

}
