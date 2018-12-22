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

//
// class definition
//

class CBoid {

   public:

      ///////////////////
      // static variables
      ///////////////////

      // visible friends list (work space reused by each boid)

      static CBoid * VisibleFriendsList[Max_Friends_Visible];

      ///////////////////////////////
      // constructors and destructors
      ///////////////////////////////

      // Constructor #1.
      // Creates an individual boid with randomized position, 
      // velocity, and orientation.

      CBoid (short id_v);

      // Constructor #2.
      // Creates an individual boid with specific position, 
      // velocity, and orientation.

      CBoid (short id_v,
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
      // miscellaneous functions
      //////////////////////////

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

      short    m_id;                         // member individual ID

      float    m_perception_range;           // how far member can see

      vector   m_pos;                       // position of member (in meters)
      vector   m_vel;                       // velocity of member (meters/sec)
      vector   m_ang;                       // orientation of member 
                                             // (right-handed coordinate system)
      // computed

      float    m_speed;                      // overall speed of member

      short    m_num_flockmates_seen;        // # of flockmates this member sees
      short    m_num_enemies_seen;           // # of enemies this member sees

      CBoid    *m_nearest_flockmate;         // pointer to nearest flockmate (if any)
   
      CBoid    *m_nearest_enemy;             // pointer to nearest enemy (if any)

      float    m_dist_to_nearest_flockmate;  // distance to nearest flockmate
                                             // (if any), in meters

      float    m_dist_to_nearest_enemy;      // distance to nearest enemy
                                             // (if any), in meters

      vector   m_oldpos;                     // last position
      vector   m_oldvel;                     // last velocity

      CBoid    *m_next;                      // pointer to next flockmate
      CBoid    *m_prev;                      // pointer to previous flockmate

      ///////////////////
      // flocking methods
      ///////////////////

      // Cruising.
      // Generates a vector indicating how a flock boid would
      // like to move, if it were all up to him and he was under
      // no other influences of any kind.

      vector CBoid::Cruising (void);

      // FleeEnemies.
      // Generates a vector for a flock boid to avoid the 
      // nearest enemy (boid of a different flock) it sees.

      vector CBoid::FleeEnemies (void);

      // KeepDistance.
      // Generates a vector for a flock boid to maintain his
      // desired separation distance from the nearest flockmate he sees.

      vector CBoid::KeepDistance (void);

      // MatchHeading.
      // Generates a vector for a flock boid to try
      // to match the heading of its nearest flockmate.

      vector CBoid::MatchHeading (void);

      // SeeEnemies.
      // Determines which enemy flock boids a given flock boid can see.

      int CBoid::SeeEnemies (int flock_id);

      // SeeFriends.
      // Determines which flockmates a given flock boid can see.

      int CBoid::SeeFriends (CBoid *first_boid);

      // SteerToCenter.
      // Generates a vector to guide a flock boid towards
      // the "center of mass" of the flockmates he can see.

      vector CBoid::SteerToCenter (void);

      // WorldBound.
      // Implements a world boundary so that flocks don't fly
      // infinitely far away from the camera, instead remaining 
      // in a nice viewable area.  It does this by wrapping flock
      // boids around to the other side of the world, so (for example)
      // they move out the right and return on the left.

      void CBoid::WorldBound (void);

      //////////////////////////
      // miscellaneous functions
      //////////////////////////

      // AccumulateChanges.
      // Adds vector values in changes into the accumumlator vector.
      // Returns magnitude of accumulator vector after adding changes.

      float CBoid::AccumulateChanges (vector &accumulator, vector changes);

      // CanISee.
      // Determine whether a given invoking boid can see the boid in question.
      // Returns the distance to the boid.

      float CBoid::CanISee (CBoid *ptr);

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

      void CBoid::ComputeRPY (void);
};

#endif
