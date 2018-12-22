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
// Name:     CBoid.h
// Purpose:  Class definitions and method prototypes for individual
//           boids (flock members).
//*********************************************************************

#ifndef _CBOID_H
#define _CBOID_H

//
// includes
//

#include "defaults.h"
#include "mtxlib.h"
#include "util.h"
#include "vector.h"

#include "CFlock.h"
#include "CObstacle.h"

//
// enumerated types of boids
//

enum BoidTypes { None    = 0,
                 Fly     = 1,
                 Sparrow = 2,
                 Hawk    = 3 };

//
// forward declaration
//

class CFlock;

//
// class definition
//

class CBoid {

   public:

      ///////////////////
      // static variables
      ///////////////////

      // visible friends list (work space reused by each boid)

      static CBoid * VisibleFriendsList[MaxFriendsVisible];

      ///////////////////////////////
      // constructors and destructors
      ///////////////////////////////

      // Constructor #1.
      // Creates an individual boid with randomized position, 
      // velocity, and orientation.

      CBoid (char * id_v);

      // Constructor #2.
      // Creates an individual boid with specific position, 
      // velocity, orientation, and type.

      CBoid (char * id_v, BoidTypes type_v,
             vector * pos_v, vector * vel_v, vector * ang_v);

      // Destructor

      ~CBoid (void);

      //////////////////////////
      // public flocking methods
      //////////////////////////

      // FlockIt.
      // Used for frame-by-frame updates; no time 
      // deltas on positions.

      void FlockIt (int flock_id, CBoid *first_boid);

	   //////////////////////////
	   // predator/prey methods
	   //////////////////////////

      // SetAlive.
      // Sets the alive/dead flag on this boid.

      void SetAlive (bool flag_v);

	   // IsPredator.
	   // Determines whether or not the boid in question is
	   // a predator of the boid making the inquiry.

	   bool IsPredator (CBoid *ptr);

	   // IsPrey.
	   // Determines whether or not the boid in question is prey
	   // for the boid making the inquiry.

	   bool IsPrey (CBoid *ptr);

      // GetType.
      // Gets the type of the boid in question.

      BoidTypes GetType (void);

      // SetType.
      // Sets the type of the boid in question.

      void SetType (BoidTypes type_v);

      // GetHunger.
      // Returns the hunger level of the boid in question.

      float GetHunger (void);

      // SetHunger.
      // Changes the hunger level of the boid in question by the value supplied.

      void SetHunger (float inc_v);

	   // GetPreyOf.
	   // Returns the type of boid (if any) which is the prey of the
	   // boid making the inquiry.

	   BoidTypes GetPreyOf (void);

	   // GetPreyPtr.
	   // Returns the type of boid (if any) which is the prey of the
	   // boid making the inquiry.

	   CBoid * GetPreyPtr (void);

      // GetPreyVector.
      // Returns a vector the the prey (if any) of the current boid.

      vector * GetPreyVector (void);

	   // GetPredatorOf.
	   // Returns the type of boid (if any) which is the predator of
	   // the boid making the inquiry.

      BoidTypes GetPredatorOf (void);

	   // GetPredatorPtr.
	   // Returns the type of boid (if any) which is the predator of
	   // the boid making the inquiry.

      CBoid * GetPredatorPtr (void);

	   // SetPreyOf.
	   // Sets the prey type of the boid making the call.

	   void SetPreyOf (BoidTypes type_v);

	   // SetPredatorOf.
      // Sets the predator type of the boid making the call.

	   void SetPredatorOf (BoidTypes type_v);
      
      // EatPrey.
      // Eat a prey boid if we're close enough.

      bool EatPrey (void);

      // KillBoid.
      // Kills the boid in question if he's died of hunger.

      void KillBoid (int flockid_v);

      ///////////////////////
      // reproduction methods
      ///////////////////////

      // CanReproduce.
      // Indicates whether or not this boid can reproduce.

      int CanReproduce (void);

      // MakeBaby.
      // Makes a new boid for the flock in question, 
      // assuming all reproduction rules have been met.

      void MakeBaby (int flockid_v);

      // SetReproduction.
      // Sets the reproduction flag for this boid.

      void SetReproduction (bool flag_v);

      //////////////////////
      // personality methods
      //////////////////////

      // GetPerceptionRange.
      // Returns the perception range of the boid in question.

      float GetPerceptionRange (void);

      // SetPerceptionRange.
      // Sets the perception range of the boid in question.

      void SetPerceptionRange (float range_v);

      // GetKeepawayDist.
      // Returns the preferred keepaway distance of the boid in question.

      float GetKeepawayDistance (void);

      // GetPreferredSep.
      // Retuns the preferred separation distance of the boid in question.

      float GetPreferredSep (void);

      // GetState.
      // Returns the current state (alive/dead) of the boid in question.

      bool GetState (void);

      ////////////////////////
      // miscellaneous methods
      ////////////////////////

      // AddToVisibleList.
      // This visibility list is regenerated for each member each update cycle,
      // and acts much like a push-down queue; the latest member added to the
      // list becomes the first one when the list is sequentially accessed.
      // Mostly I did this for speed reasons, as this allows for fast inserts
      // (and we don't delete from this list, we just rebuild it each update cycle).

      void AddToVisibleList (CBoid *ptr);

      // ClearVisibleList.
      // Clears the visibility list.

      void ClearVisibleList (void);

      // GetOrient.
      // Returns the orientation of the boid in question.

      vector * GetOrient (void);

      // GetID.
      // Returns the ID of the boid in question.

      char * GetID (void);

      // GetPos.
      // Returns the position of the boid in question.

      vector * GetPos (void);

      // GetNext.
      // Returns the "next" pointer of the invoking member.

      CBoid * GetNext(void);

      // LinkOut.
      // Removes a member from a list.

      void LinkOut (void);

      // PrintData.
      // Dumps all data describing a given member.

      void PrintData (void);

      // SetNext.
      // Set the "next" pointer of an individual member.

      void SetNext (CBoid *ptr);

      // SetPrev.
      // Set the "prev" pointer of an individual member.

      void SetPrev (CBoid *ptr);

   private:

      ///////////////
      // data members
      ///////////////

      // supplied with constructor(s)

      char       m_id[MaxNameLength];          // member individual ID

      bool       m_alive;                      // flag indicating whether or not
                                               // this particular boid is dead or alive

      int        m_updates;                    // running counter of the total number
                                               // of flocking updates this boid has had

      vector     m_pos;                        // position of member (in meters)
      vector     m_vel;                        // velocity of member (meters/sec)
      vector     m_ang;                        // orientation of member 
                                               // ( in right-handed coordinate system)
      // computed

      float      m_speed;                      // overall speed of member

      // flockmate data

      short      m_num_flockmates_seen;        // # of flockmates this member sees
      CBoid      *m_nearest_flockmate;         // pointer to nearest flockmate (if any)
      float      m_dist_to_nearest_flockmate;  // distance to nearest flockmate
                                               // (if any), in meters

      // predator data

      short      m_num_predators_seen;         // # of predators this member sees
      CBoid      *m_nearest_predator;          // pointer to nearest predator (if any)
      float      m_dist_to_nearest_predator;   // distance to nearest predator
                                               // (if any), in meters

      // prey data

      short      m_num_prey_seen;              // # of prey this member sees
      CBoid      *m_nearest_prey;              // pointer to nearest prey (if any)
      float      m_dist_to_nearest_prey;       // distance to nearest prey
                                               // (if any), in meters
      CFlock     *m_preys_flock;               // pointer to prey's flock
      vector     m_prey_vector;                // vector to prey

      // obstacle data

      short      m_num_obstacles_seen;         // # of obstacles this member sees
      CObstacle  *m_nearest_obstacle;          // pointer to nearest obstacle (if any)
      float      m_dist_to_nearest_obstacle;   // distance to nearest obstacle
                                               // (if any), in meters

      vector     m_oldpos;                     // last position
      vector     m_oldvel;                     // last velocity

      CBoid      *m_next;                      // pointer to next flockmate
      CBoid      *m_prev;                      // pointer to previous flockmate

      // individuality values

      float      m_hunger;                     // personal hunger level

      float      m_starting_hunger;            // starting hunger level

      float      m_max_speed;                  // personal maximum speed

      float      m_feeding_distance;           // how close this boid has to be to
                                               // its prey to eat it

      float      m_perception_range;           // how far member can see

      float      m_preferred_separation;       // personal preferred separataion distance
                                               // this boid wants to maintain from others 
                                               // in his flock

      float      m_keepaway_distance;          // personal minimum distance to stay away
                                               // from any enemies or predators

      bool       m_reproduction_flag;          // flag indicating whether or not this kind
                                               // of boid can reproduce

      // predator/prey info

      BoidTypes   m_type;                      // boid type 

	   BoidTypes   m_my_prey;                   // flag indicating this boid's type 
	                                            //  of prey (to eat)
											   
	   BoidTypes   m_my_predator;               // flag indicating this boid's type
	                                            //  of predator (to flee)

      ///////////////////
      // flocking methods
      ///////////////////

      // AvoidObstacles.
      // Generates a vector to make a flock boid avoid crashing
      // into any obstacles that might be nearby.

      vector AvoidObstacles (void);

      // Cruising.
      // Generates a vector indicating how a flock boid would
      // like to move, if it were all up to him and he was under
      // no other influences of any kind.

      vector Cruising (void);

      // FleePredators.
      // Generates a vector for a flock boid
      // to avoid the nearest predator it sees.

      vector FleePredators (void);

      // HuntPrey.
      // Generates a vector to make a flock boid try to
      // close position with any prey boid it may be hunting.

      vector HuntPrey (void);

      // KeepDistance.
      // Generates a vector for a flock boid to maintain his
      // desired separation distance from the nearest flockmate he sees.

      vector KeepDistance (void);

      // MatchHeading.
      // Generates a vector for a flock boid to try
      // to match the heading of its nearest flockmate.

      vector MatchHeading (void);

      // SeePredators.
      // Determines which predator boids a given flock boid can see.

      int SeePredators (int flock_id);

      // SeePrey.
      // Determines which prey boids a given flock boid can see.

      int SeePrey (int flock_id);

      // SeeFriends.
      // Determines which flockmates a given flock boid can see.

      int SeeFriends (CBoid *first_boid);

      // SeeObstacles.
      // Determines if a given boid sees any obstacles near him.
      // Returns the number of obstacles seen if it does.

      int SeeObstacles (void);

      // SteerToCenter.
      // Generates a vector to guide a flock boid towards
      // the "center of mass" of the flockmates he can see.

      vector SteerToCenter (void);

      // WorldBound.
      // Implements a world boundary so that flocks don't fly
      // infinitely far away from the camera, instead remaining 
      // in a nice viewable area.  It does this by wrapping flock
      // boids around to the other side of the world, so (for example)
      // they move out the right and return on the left.

      void WorldBound (void);

      //////////////////////////
      // miscellaneous functions
      //////////////////////////

      // AccumulateChanges.
      // Adds vector values in changes into the accumumlator vector.
      // Returns magnitude of accumulator vector after adding changes.

      float AccumulateChanges (vector &accumulator, vector changes);

      // CanISee.
      // Determine whether a given invoking boid can see the boid in question.
      // Returns the distance to the boid if it can be seen; INFINITY if not.

      float CanISee (CBoid *ptr);

      // CanISeeObstacle.
      // Determine whether a given invoking boid can see the obstacle in question.
      // Returns the distance to the obstacle if it can be seen, INFINITY if not.

      float CanISeeObstacle (CObstacle *ptr);

      // ComputeRPY.
      // Computes the roll/pitch/yaw of the flock boid based on its
      // latest velocity vector changes.  Roll/pitch/yaw are stored in
      // the "ang" data member as follows:
      //   pitch is about the x axis
      //   yaw is about the y axis
      //   roll is about the z axis
      // All calculations assume a right-handed coordinate system:
      //   +x = through the left side of the object
      //   +y = up
      //   +z = through the nose of the model

      void ComputeRPY (void);
};

#endif
