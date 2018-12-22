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
#include <string.h>

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
// Creates an individual boid with randomized position, 
// velocity, and orientation

CBoid::CBoid (char * id_v)
{

#ifdef BOID_DEBUG
   myprintf("\nCBoid constructor #1 called for boid %s.\n",id_v);
#endif

   strcpy(m_id, id_v);

   m_updates   = 0;

   m_alive     = TRUE;

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

   // Set the boid individuality parameters.  We'll just 
   // use the base value and add some random amount to that.

   // generate this boid's "personal maximum" upper speed

   m_max_speed = MaxSpeed + (RAND() * 0.5f);

   // feeding distance

   m_feeding_distance = MaxFeedingDistance + (RAND() * 0.5f);

   // personal range of sight

   m_perception_range = DefaultPerceptionRange + (RAND() * 0.5f);

   // preferred separation distance

   m_preferred_separation = SeparationDist + (RAND() * 0.5f);

   // preferred keepaway distance

   m_keepaway_distance = KeepAwayDist + (RAND() * 0.5f);

   // initial hunger rating

   m_hunger = m_starting_hunger = MaxHunger + (SIGN(m_vel.x) * RAND() * 0.5f);

   // other values

   m_type                      = None;
   m_my_prey                   = None;
   m_my_predator               = None;

   m_num_flockmates_seen       = 0;
   m_nearest_flockmate         = NULL;
   m_dist_to_nearest_flockmate = INFINITY;

   m_num_predators_seen        = 0;
   m_nearest_predator          = NULL;
   m_dist_to_nearest_predator  = INFINITY;

   m_num_prey_seen             = 0;
   m_nearest_prey              = NULL;
   m_dist_to_nearest_prey      = INFINITY;
   m_preys_flock               = NULL;

   m_num_obstacles_seen        = 0;
   m_nearest_obstacle          = NULL;
   m_dist_to_nearest_obstacle  = INFINITY;

   m_reproduction_flag         = FALSE;

   m_next = m_prev = NULL;

#ifdef BOID_DEBUG
   PrintData();
#endif

}  

// Constructor #2.
// Creates an individual boid with specific position, velocity, 
// orientation, and type

CBoid::CBoid (char * id_v, BoidTypes type_v,
              vector * pos_v, vector * vel_v, vector * ang_v)
{

#ifdef BOID_DEBUG
   myprintf("\nCBoid constructor #2 called for boid %s.\n",id_v);
#endif


   strcpy(m_id, id_v);

   m_updates   = 0;
   m_alive     = TRUE;
   m_pos       = pos_v;
   m_vel       = vel_v;
   m_ang       = ang_v;

   m_speed = vel_v->length();

   // Set the boid individuality parameters.  We'll just 
   // use the base value and add some random amount to that.

   // generate this boid's "personal maximum" upper speed

   m_max_speed = MaxSpeed + (RAND() * 0.5f);

   // feeding distance

   m_feeding_distance = MaxFeedingDistance + (RAND() * 0.5f);

   // personal range of sight

   m_perception_range = DefaultPerceptionRange + (RAND() * 0.5f);

   // preferred separation distance

   m_preferred_separation = SeparationDist + (RAND() * 0.5f);

   // preferred keepaway distance

   m_keepaway_distance = KeepAwayDist  + (RAND() * 0.5f);

   // initial hunger rating

   m_hunger = m_starting_hunger = MaxHunger + (SIGN(m_vel.x) * RAND() * 0.5f);

   // predator/prey values

   m_type                      = type_v;
   m_my_prey                   = None;
   m_my_predator               = None;

   // other values

   m_num_flockmates_seen       = 0;
   m_nearest_flockmate         = NULL;
   m_dist_to_nearest_flockmate = INFINITY;

   m_num_predators_seen        = 0;
   m_nearest_predator          = NULL;
   m_dist_to_nearest_predator  = INFINITY;

   m_num_prey_seen             = 0;
   m_nearest_prey              = NULL;
   m_dist_to_nearest_prey      = INFINITY;
   m_preys_flock               = NULL;

   m_num_obstacles_seen        = 0;
   m_nearest_obstacle          = NULL;
   m_dist_to_nearest_obstacle  = INFINITY;

   m_reproduction_flag         = FALSE;

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
   myprintf("\n=====\nUpdate %d for %s\n", m_updates, m_id);
   myprintf("pos before = %f %f %f\n",m_pos.x,m_pos.y,m_pos.z);
#endif

   // test:  is this boid alive or dead?

   if (!m_alive) return;               // he's dead--get outta here

   // increment counter

   m_updates++;

   // Step 1:  Update our position.
   // Update our position based on the velocity
   // vector we computed last time around.

   m_oldpos = m_pos;     // save off our previous position

   m_pos += m_vel;       // apply velocities.

   // Step 2:  Decrement our hunger rating.
   // Decrement our hunger rating so that we might become 
   // hungry--and search for our prey.  Don't bother if we
   // don't actually *have* any kind of prey to hunt.

   if (GetPreyOf()) SetHunger(HungerPerCycle);  // nothing magic about the number, it just works well

   // Step 3:  SeeFriends.
   // Determine if we can see any of our flockmates.

   SeeFriends (first_boid);

   // Step 4:  Flocking behavior.
   // Do we see any of our flockmates?  If yes, it's time to implement
   // the first Three Rules (they don't matter if we can't see anybody)

   if (m_num_flockmates_seen) {

      // Step 5:  Implement Rule #1 (Separation).
      // Try to maintain our desired separation distance from our nearest flockmate.

      AccumulateChanges (acc, KeepDistance());

      // Step 6:  Implement Rule #2 (Alignment).
      // Try to move the same way our nearest flockmate does.

      AccumulateChanges (acc, MatchHeading());

      // Step 7:  Implement Rule #3 (Cohesion).
      // Try to move towards the center of the flock.

      AccumulateChanges (acc, SteerToCenter());
   
   }

   // Step 8a:  Implement Rule #4 (Avoidance of obstacles).
   // If we see anything blocking or potentially blocking our
   // path, try to steer so as to not run into it.

   if (SeeObstacles()) AccumulateChanges (acc, AvoidObstacles());

   // Step 8b:  Implement Rules #4 (Avoidance of enemies)
   // We don't like to get eaten, so if we see any predators
   // then we want to avoid them if we can.

   if (SeePredators (flock_id)) AccumulateChanges (acc, FleePredators());

   // Step 9:  Hunt prey.
   // If we're hungry then we'll want to locate and eat some prey.

   if (m_hunger <= HungerTrigger) {

      // we're hungry...do we see anything to eat?

      if (SeePrey (flock_id)) AccumulateChanges (acc, HuntPrey());

   }

   // Step 10:  Cruising.
   // Flockmates or not, enemies or not, figure out
   // how fast we'd move if it were all up to us.

   AccumulateChanges (acc, Cruising());

   // Step 11:  Constrain acceleration
   // If our acceleration change is more than we allow, constrain it

   if (acc.length() > MaxChange) {

#ifdef BOID_DEBUG
      myprintf("WARNING:  constraining acceleration for boid %x!\n",this);
#endif

      // definitely too much...constrain to maximum change

      acc.SetMagnitudeOfVector (MaxChange);

   }

   // Step 12:  Implementation.
   // Here's where we apply our newly computed acceleration vector
   // to create a new velocity vector to use next update cycle.

   m_oldvel = m_vel;     // save off our previous velocity

   // now add in the acceleration

   m_vel += acc;

   // Step 13:  constraint Y velocity changes.
   // Attempt to restrict flight straight up/down by damping out Y axis velocity.
   // This isn't strictly necessary, but does lead to more realistic looking flight.

   m_vel.y *= MaxPitch;

   // Step 14:  Constrain our speed.
   // If we're moving faster than we're allowed to move, constrain our velocity.

   m_speed = m_vel.length();

   if (m_speed > m_max_speed) {

      // definitely too fast...constrain to maximum speed

#ifdef BOID_DEBUG
      myprintf("WARNING:  constraining speed for boid %s!\n",m_id);
      myprintf("          current speed = %f new speed = %f\n",m_speed, m_max_speed);
#endif

      m_vel.SetMagnitudeOfVector (m_max_speed);

      m_speed = m_max_speed;

   }

   // Step 15:  Compute roll/pitch/yaw.
   // Compute our orientation after all this speed adjustment nonsense.

   ComputeRPY();

   // Step 16:  World boundaries.
   // If we've wandered outside the world bounds put us back in.  You might
   // not need to do this for your world, but that depends on your implementation.

   WorldBound();

#ifdef BOID_DEBUG
   myprintf("final position     = %f %f %f\n",m_pos.x, m_pos.y, m_pos.z);
   myprintf("final velocity     = %f %f %f\n",m_vel.x, m_vel.y, m_vel.z);
   myprintf("final acceleration = %f %f %f\n",acc.x, acc.y, acc.z);
#endif

   // Step 17:  If we're hunting something at present, let's see if we can eat it

   if (m_nearest_prey) EatPrey();

   // Step 18:  Test to see if we've gone without food too long; if so, we die

   if (m_hunger < HungerDeath) KillBoid(flock_id);

   // Step 19:  Can we reproduce?

   if (CanReproduce()) MakeBaby(flock_id);

}

///////////////////////////
// private flocking methods
///////////////////////////

// AvoidObstacles.
// Generates a vector to make a flock boid avoid crashing
// into any obstacles that might be nearby.

vector CBoid::AvoidObstacles (void)
{

   vector change;
   vector obs_pos;

#ifdef BOID_DEBUG
   myprintf("\nInside AvoidObstacles\n");
   myprintf("Boid id = %s, urgency = %f.\n",m_id,urgency);
#endif

   obs_pos = m_nearest_obstacle->GetPos();

   // replace obstacle's Y value (at base) with the
   // boid's own Y value (since that's where we computed
   // the nearest point on its surface)

   obs_pos.y = m_pos.y;

   // compute vector away from the obstacle

   change = m_pos - obs_pos;

   // scale the desired change--we really hate running into things

   change.SetMagnitudeOfVector(MaxUrgency);

#ifdef BOID_DEBUG
   myprintf("   final change vector from AvoidObstacles = %f %f %f\n",
      change.x, change.y, change.z);
#endif

   // return change vector

   return (change);

}

// Cruising.
// Generates a vector indicating how a flock boid would
// like to move, if it were all up to him and he was under
// no other influences of any kind.

vector CBoid::Cruising (void)
{
   vector  change = m_vel;

   float  diff = (m_speed - DesiredSpeed)/ m_max_speed;
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

// FleePredators.
// Generates a vector for a flock boid 
// to avoid the nearest predator it sees.

vector CBoid::FleePredators (void)
{

   vector change;

#ifdef BOID_DEBUG
   myprintf("\nInside FleePredators\n");
#endif

   // test:  Are we too close to our nearest predator?

   if (m_dist_to_nearest_predator < m_keepaway_distance) {

#ifdef BOID_DEBUG
      myprintf("   too close to %x\n",m_nearest_predator);
#endif

      // yep...compute vector away from predator

      change = m_pos - m_nearest_predator->m_pos;

   }

#ifdef BOID_DEBUG
   myprintf("   final change vector from FleePredators = %f %f %f\n",change.x, change.y, change.z);
#endif

   // return change vector

   return (change);

}

// HuntPrey.
// Generates a vector to make a flock boid try to
// close position with any prey boid it may be hunting.

vector CBoid::HuntPrey (void)
{

   vector change;

#ifdef BOID_DEBUG
   myprintf("\nInside HuntPrey\n");
   myprintf("   Boid %s is hunting boid %s\n",m_id,m_nearest_prey->GetID());
#endif

   // compute a vector towards the prey--we want to get
   // as close as we possibly can so we can eat him!

   change = m_nearest_prey->m_pos - m_pos;

  // store vector for drawing use (if needed)

   m_prey_vector = change;

   // make it urgent--we're hungry, after all

   change.SetMagnitudeOfVector (m_max_speed);

#ifdef BOID_DEBUG
   myprintf("   final change vector from HuntPrey = %f %f %f\n",change.x, change.y, change.z);
#endif

   // return change vector

   return (change);

}

// KeepDistance.
// Generates a vector for a flock boid to maintain his
// desired separation distance from the nearest flockmate he sees.

vector CBoid::KeepDistance (void)
{

   float ratio = m_dist_to_nearest_flockmate/m_preferred_separation;

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
      m_dist_to_nearest_flockmate,m_preferred_separation,ratio);
#endif

   if (m_dist_to_nearest_flockmate < m_preferred_separation) {

#ifdef BOID_DEBUG
      myprintf("   too close!\n");
#endif
      
      // too close...move away from our neighbor

      change.SetMagnitudeOfVector (-ratio);

   } else if (m_dist_to_nearest_flockmate > m_preferred_separation) {

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

// SeePredators.
// Determines which predator boids a given flock boid can see.

int CBoid::SeePredators (int flock_id)
{

   float dist;

   CBoid *predator;

#ifdef BOID_DEBUG
   myprintf("\nInside SeePredators\n");
#endif

   // initialize predator data

   m_num_predators_seen       = 0;
   m_nearest_predator         = NULL;
   m_dist_to_nearest_predator = INFINITY;

   // loop over each flock and determine the closest one we can see

   for (int i = 0; i < CFlock::FlockCount; i++) {

      // test:  if this is OUR flock, skip it

      if (i == flock_id) continue;

#ifdef VISIBILITY_DEBUG
   myprintf("   \nTesting to see if %s can see anybody in flock %d\n",m_id,i);
#endif

      // not our flock, so check it out

      predator = CFlock::ListOfFlocks[i]->GetFirstMember();

      // is this flock a predator of our boid?

#ifdef VISIBILITY_DEBUG
      myprintf("   my type = %d  his type = %d\n",m_type,predator->GetType());
#endif

      if (predator != NULL) {

         if (IsPredator(predator)) {

            // it sure is....find the closest one

            while (predator != NULL) {
      
   #ifdef VISIBILITY_DEBUG
         myprintf("   looking at %x\n",predator);
   #endif

               // if this predator is visible...

               if ((dist = CanISee(predator)) != INFINITY) {

   #ifdef VISIBILITY_DEBUG
                  myprintf("   Yep,  I see him!\n");
   #endif

                  // I can see it..increment counter

                  m_num_predators_seen++;

                  // Test:  Closest predator?

                  if (dist < m_dist_to_nearest_predator) {
               
                     // yes...save it off

                     m_dist_to_nearest_predator = dist;
                     m_nearest_predator = predator;
                  }
               }

               // get next predator in flock

               predator = predator->GetNext();
            }
         }
      }
   }

#ifdef VISIBILITY_DEBUG
   myprintf("\n");
   myprintf("   total predators seen = %d\n",m_num_predators_seen);
   myprintf("   nearest predator is %x at %f\n",
      m_nearest_predator,m_dist_to_nearest_predator);
#endif

   return (m_num_predators_seen);

}

// 3.
// Determines which prey boids a given flock boid can see.

int CBoid::SeePrey (int flock_id)
{

   float dist;

   CBoid *prey;

#ifdef BOID_DEBUG
   myprintf("\nInside SeePrey\n");
#endif

   // initialize prey data

   m_num_prey_seen        = 0;
   m_nearest_prey         = NULL;
   m_dist_to_nearest_prey = INFINITY;

   // loop over each flock and determine the closest one we can see

   for (int i = 0; i < CFlock::FlockCount; i++) {

      // test:  if this is OUR flock, skip it

      if (i == flock_id) continue;

#ifdef VISIBILITY_DEBUG
   myprintf("   \nTesting to see if %s can see anybody in flock %d\n",m_id,i);
#endif

      // not our flock, so check it out

      prey = CFlock::ListOfFlocks[i]->GetFirstMember();

      // is this flock prey for our boid?

#ifdef VISIBILITY_DEBUG
      myprintf("   my type = %d  his type = %d\n",m_type,prey->GetType());
#endif

      if (prey != NULL) {

         if (IsPrey(prey)) {

            // it sure is....find the closest one

            while (prey != NULL) {
      
   #ifdef VISIBILITY_DEBUG
         myprintf("   looking at %s\n",prey->GetID());
   #endif

               // if this prey is visible...

               if ((dist = CanISee(prey)) != INFINITY) {

   #ifdef VISIBILITY_DEBUG
                  myprintf("   Yep,  I see him!\n");
   #endif

                  // I can see it..increment counter

                  m_num_prey_seen++;

                  // Test:  Closest prey?

                  if (dist < m_dist_to_nearest_prey) {
               
                     // yes...save it off

                     m_dist_to_nearest_prey = dist;
                     m_nearest_prey         = prey;
                     m_preys_flock          = CFlock::ListOfFlocks[i];
                  }

               }

               // get next prey in flock

               prey = prey->GetNext();

            }
         }
      }
   }

#ifdef VISIBILITY_DEBUG
   myprintf("\n");
   myprintf("   total prey seen = %d\n",m_num_prey_seen);
   myprintf("   nearest prey is %x at %f\n",m_nearest_prey,m_dist_to_nearest_prey);
#endif

   return (m_num_prey_seen);

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

// SeeObstacles.
// Determines if a given boid sees any obstacles near him.
// Returns the number of obstacles seen if it does.

int CBoid::SeeObstacles (void)
{

   float dist;

   CObstacle *obstacle;

#ifdef VISIBILITY_DEBUG
   myprintf("\nInside SeeObstacles with boid %s\n",m_id);
#endif

   // initialize obstacle data

   m_num_obstacles_seen       = 0;
   m_nearest_obstacle         = NULL;
   m_dist_to_nearest_obstacle = INFINITY;

   // loop over every obstacle and determine the closest one we can see

   for (int i = 0; i < CObstacle::ObstacleCount; i++) {

#ifdef VISIBILITY_DEBUG
      myprintf("   Testing to see if boid %x can see obstacle %d\n",
         this,CObstacle::ListOfObstacles[i]->GetID());
#endif

      // not our flock, so check it out

      obstacle = CObstacle::ListOfObstacles[i];

      // if this obstacle is visible...

      if ((dist = CanISeeObstacle(obstacle)) != INFINITY) {

         // I can see it..increment counter

         m_num_obstacles_seen++;

         // Test:  Closest enemy?

         if (dist < m_dist_to_nearest_obstacle) {
            
            // yes...save it off

            m_dist_to_nearest_obstacle = dist;
            m_nearest_obstacle = obstacle;
         }
      }
   }

#ifdef VISIBILITY_DEBUG
   myprintf("\n");
   myprintf("   total obstacles seen = %d\n",m_num_obstacles_seen);
   myprintf("   nearest obstacle is %x at a distance of %f\n",
      m_nearest_obstacle,m_dist_to_nearest_obstacle);
#endif

   return (m_num_obstacles_seen);

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
// predator/prey methods
////////////////////////

// SetAlive.
// Sets the alive/dead flag on this boid.

void CBoid::SetAlive (bool flag_v)
{

   m_alive = flag_v;

}

// IsPredator.
// Determines whether or not the boid in question is
// a predator of the boid making the inquiry.

bool CBoid::IsPredator (CBoid *ptr)
{

   return (ptr->GetType() == m_my_predator);

}

// IsPrey.
// Determines whether or not the boid in question is prey
// for the boid making the inquiry.

bool CBoid::IsPrey (CBoid *ptr)
{

   return (ptr->GetType() == m_my_prey);

}

// GetType.
// Gets the type of the boid in question.

BoidTypes CBoid::GetType (void)
{

   return (m_type);

}

// SetType.
// Sets the type of the boid in question.

void CBoid::SetType (BoidTypes type_v)
{

   m_type = type_v;

   // adjust personal max speed based on type (so predators are faster than prey)

   m_max_speed *= m_type;

}

// GetHunger.
// Returns the hunger level of the boid in question.

float CBoid::GetHunger (void)
{

   return (m_hunger);

}

// SetHunger.
// Changes the hunger level of the boid in question by the value supplied.

void CBoid::SetHunger (float inc_v)
{

   m_hunger += inc_v;

}

// GetPreyOf.
// Returns the type of boid (if any) which is the prey of the
// boid making the inquiry.

BoidTypes CBoid::GetPreyOf (void)
{

	return (m_my_prey);

}

// GetPreyPtr.
// Returns a pointer to the boid (if any) which is the prey of the
// boid making the inquiry.

CBoid * CBoid::GetPreyPtr (void)
{

	return (m_nearest_prey);

}

// GetPreyVector.
// Returns a vector the the prey (if any) of the current boid.

vector * CBoid::GetPreyVector (void)
{

   return (&m_prey_vector);

}

// SetPreyOf.
// Sets the prey type of the boid making the call.

void CBoid::SetPreyOf (BoidTypes type_v)
{

	m_my_prey = type_v;

}

// GetPredatorOf.
// Returns the type of boid (if any) which is the predator of
// the boid making the inquiry.

BoidTypes CBoid::GetPredatorOf (void)
{

   return (m_my_predator);

}

// GetPredatorPtr.
// Returns the type of boid (if any) which is the predator of
// the boid making the inquiry.

CBoid * CBoid::GetPredatorPtr (void)
{

   return (m_nearest_predator);

}

// SetPredatorOf.
// Sets the predator type of the boid making the call.

void CBoid::SetPredatorOf (BoidTypes type_v)
{

	m_my_predator = type_v;

}

// EatPrey.
// Eat a prey boid if we're close enough.

bool CBoid::EatPrey (void)
{

#ifdef BOID_DEBUG
   myprintf("\nInside EatPrey\n");
#endif  

   // test:  are we within "eating distance" of our prey?

   if (m_dist_to_nearest_prey <= m_feeding_distance) {

#ifdef BOID_DEBUG
      myprintf("   Boid %s is eating boid %s!\n",m_id,m_nearest_prey->GetID());
#endif

      // sure are...eat'em up!

      // we're not as hungry now

      SetHunger(m_starting_hunger * RAND());

      // kill the unfortunate boid

      m_nearest_prey->SetAlive(FALSE);

      // let the flock know

      m_preys_flock->SetLiveCount(-1);
      m_preys_flock->SetDeadCount(1);

      // Ordinarily we'd probably delete the prey boid right here,
      // but since we have a big external list in main.cpp (this is
      // a demo, after all) we won't.  With the boid removed from
      // the Flock's list, he won't be updated or drawn.

      // clear the prey variables

      m_num_prey_seen             = 0;
      m_nearest_prey              = NULL;
      m_dist_to_nearest_prey      = INFINITY;

      // we're done--get outta here

      return (TRUE);

   }

#ifdef BOID_DEBUG
      myprintf("   Boid %s wasn't close enough to eat boid %s!\n",m_id,m_nearest_prey->GetID());
#endif

   // fell through..must not have been close enough to eat!

   return (FALSE);

}

// KillBoid.
// Kills the boid in question if he's died of hunger.

void CBoid::KillBoid (int flockid_v)
{

//#ifdef BOID_DEBUG
   myprintf("\nInside KillBoid\n");
   myprintf("   Boid %s has died of hunger and is being removed from flock %d.\n",m_id,flockid_v);
//#endif  

      // this poor boid has died of hunger--kill him

      SetAlive(FALSE);

   // Ordinarily we'd probably delete the prey boid right here,
   // but since we have a big external list in main.cpp (this is
   // a demo, after all) we won't.  With the boid removed from
   // the Flock's list, he won't be updated or drawn.

}

///////////////////////
// reproduction methods
///////////////////////

// CanReproduce.
// Indicates whether or not this boid can reproduce.

int CBoid::CanReproduce (void)
{

   return (m_reproduction_flag);

}

// MakeBaby.
// Makes a new boid for the flock in question, 
// assuming all reproduction rules have been met.

void CBoid::MakeBaby (int flockid_v)
{

   CBoid *ptr;

#ifdef REPRO_DEBUG
   myprintf("\nInside MakeBaby.\n");
   myprintf("   Flock %d is going to see if it can create a new boid!\n",flockid_v);
#endif

   // Can we SEE enough of our buddies to reproduce?

#ifdef REPRO_DEBUG
   myprintf(" checking boids we can see = %d\n",m_num_flockmates_seen);
#endif

   if (m_num_flockmates_seen >= NumBoidsForBaby) {

      // yep--any dead boids in this flock?

#ifdef REPRO_DEBUG
      myprintf("checking dead count = %d\n",CFlock::ListOfFlocks[flockid_v]->GetDeadCount());
#endif

      if ((CFlock::ListOfFlocks[flockid_v]->GetDeadCount()) > 0) {

         // Yep--that means there's room for a baby.  Go 
         // make one by "reactivating" one of the dead boids.

         // search through the list of boids in this flock until we find a dead one

         ptr = CFlock::ListOfFlocks[flockid_v]->GetFirstMember();

         while (ptr != NULL) {
      
            // test--is this guy dead?

#ifdef REPRO_DEBUG
            myprintf(" boid %s state = %d\n",ptr->GetID(),ptr->GetState());
#endif

            if (!(ptr->GetState())) {

#ifdef REPRO_DEBUG
               myprintf("  making new baby--reactivating boid %s\n",ptr->GetID());
#endif

               // yep, he's dead--bring him back to life

               ptr->SetAlive(TRUE);

               // let the flock know

               CFlock::ListOfFlocks[flockid_v]->SetLiveCount(1);
               CFlock::ListOfFlocks[flockid_v]->SetDeadCount(-1);

#ifdef REPRO_DEBUG
               myprintf("Flock live/dead count = %d %d\n",
                  CFlock::ListOfFlocks[flockid_v]->GetLiveCount(),
                  CFlock::ListOfFlocks[flockid_v]->GetDeadCount());
#endif

               // we're done--get outta here

               return;

            }

         // get next boid

         ptr = ptr->GetNext();

         }

      }
   }
}

// SetReproduction.
// Sets the reproduction flag for this boid.

void CBoid::SetReproduction (bool flag_v)
{

   m_reproduction_flag = flag_v;

}

//////////////////////
// personality methods
//////////////////////

// GetPerceptionRange.
// Returns the perception range of the boid in question.

float CBoid::GetPerceptionRange (void)
{

   return (m_perception_range);

}

// SetPerceptionRange.
// Sets the perception range of the boid in question.

void CBoid::SetPerceptionRange (float range_v)
{

   m_perception_range = range_v;

}

// GetKeepawayDist.
// Returns the preferred keepaway distance of the boid in question.

float CBoid::GetKeepawayDistance (void)
{

   return (m_keepaway_distance);

}

// GetPreferredSep.
// Retuns the preferred separation distance of the boid in question.

float CBoid::GetPreferredSep (void)
{

   return (m_preferred_separation);

}

// GetState.
// Returns the current state (alive/dead) of the boid in question.

bool CBoid::GetState (void)
{

   return (m_alive);

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

   if (m_num_flockmates_seen < MaxFriendsVisible) {

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

   for (int i = 0; i < MaxFriendsVisible; i++) {
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

   // not using truth, so check sighting

   if (m_perception_range > dist) return (dist);

   // fell through; can't see it

   return (INFINITY);

}

// CanISeeObstacle.
// Determine whether a given invoking boid can see the obstacle in question.
// Returns the distance to the obstacle if it can be seen, INFINITY if not.

float CBoid::CanISeeObstacle (CObstacle *ptr)

{

   float dist, radius;

   vector temp1, temp2;

#ifdef VISIBILITY_DEBUG
   myprintf("\n   Inside CanISeeObstacle.\n");
#endif

   // If we're already higher than the obstacle, don't bother--we're not
   // going to run into it anyway, so no reason to worry about it.

   if (!(ptr->IsHeigher(m_pos.y))) {

      // Determine the distance to the obstacle in question.  Since the
      // obstacles are cones and have some diameter to them (unlike boids,
      // which we treat as point masses), we have to find the diameter of
      // the obstacle at our height and consider that as part of the sight
      // determination calculations.

      // what's the distance from my position to the centroid of the obstacle?

      temp1 = m_pos;                // position of boid
      temp2 = ptr->GetPos();        // position of obstacle

      // zero out Y values

      temp1.y = temp2.y = 0.0f;

      // compute the distance between the centroids (no altitude considered)

      dist = (temp1.GetDist(temp1, temp2));

   #ifdef VISIBILITY_DEBUG
      myprintf("   dist between %x and %x = %f\n",this,ptr,dist);
   #endif

      // Okay, we've got the basic distance between our boid and the centroid
      // of the obstacle.  However, the obstacle has a given size at this altitude,
      // so we have to determine what that is and factor that into the equation.

      radius = ptr->GetRelativeRadius(m_pos.y);

   #ifdef VISIBILITY_DEBUG
      myprintf("   relative radius of Obstacle %x is %f\n",ptr,radius);
      myprintf("   perception range = %f\n",m_perception_range);
   #endif

      // Okay, now we can check distance vs. our range of sight

      if (m_perception_range > (dist - radius)) return (dist-radius);

   }

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

// GetID.
// Returns the ID of the boid in question.

char * CBoid::GetID (void)
{

   return (m_id);

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
   myprintf("Data for boid = %s @ %x\n", m_id,this);
   myprintf(" type = %d\n",m_type);
   myprintf(" prey = %d  predator = %d\n",m_my_prey, m_my_predator);

   myprintf(" starting hunger level = %f\n",m_starting_hunger);
   myprintf(" current hunger level = %f\n",m_hunger);
   if (m_nearest_prey) myprintf(" currently hunting %s\n",m_nearest_prey->GetID());

   myprintf(" perception range = %f\n", m_perception_range);
   myprintf(" preferred separation distance = %f\n",m_preferred_separation);

   myprintf(" pos x, y, z = %f %f %f\n", m_pos.x, m_pos.y, m_pos.z);
   myprintf(" vel x, y, z = %f %f %f\n", m_vel.x, m_vel.y, m_vel.z);
   myprintf(" roll, pitch, yaw = %f %f %f\n", m_ang.z, m_ang.x, m_ang.y);

   myprintf(" speed                     = %f\n", m_speed);
   myprintf(" personal max speed        = %f\n", m_max_speed);

   myprintf(" num_flockmates_seen       = %d\n", m_num_flockmates_seen);
   myprintf(" nearest_flockmate         = %x\n", m_nearest_flockmate);
   myprintf(" dist_to_nearest_flockmate = %f\n", m_dist_to_nearest_flockmate);

   myprintf(" num_predators_seen        = %d\n", m_num_predators_seen);
   myprintf(" nearest_predator          = %x\n", m_nearest_predator);
   myprintf(" dist_to_nearest_predator  = %f\n", m_dist_to_nearest_predator);

   myprintf(" num_prey_seen             = %d\n", m_num_prey_seen);
   myprintf(" nearest_prey              = %x\n", m_nearest_prey);
   myprintf(" dist_to_nearest_prey      = %f\n", m_dist_to_nearest_prey);

   myprintf(" num_obstacles_seen        = %d\n", m_num_obstacles_seen);
   myprintf(" nearest_obstacle          = %x\n", m_nearest_obstacle);
   myprintf(" dist_to_nearest_obstacle  = %f\n", m_dist_to_nearest_obstacle);

}
