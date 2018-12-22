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
// Name:     CObstacle.h: 
// Purpose:  Class definitions and prototypes for the CObstacle class.
//*********************************************************************

#ifndef _COBSTACLE_H
#define _COBSTACLE_H

//
// includes
//

#include <stdio.h>
#include <math.h>

#include "defaults.h"
#include "util.h"
#include "vector.h"

class CObstacle
{

public:

   ///////////////////
   // static variables
   ///////////////////

   // number of obstacles

   static int ObstacleCount;

   // list of flocks

   static CObstacle * ListOfObstacles[MaxObstacles];

   ///////////////////////////////
   // constructors and destructors
   ///////////////////////////////
   
   // Constructor #1.
   // Creates an Obstacle with default values of 25 meters tall and 5 meters radius.
   // Position will be randomized.  Obstacles are always placed on the "ground" of
   // the Box they exist in, so the "y" value is automatically constrained.

   CObstacle (float hv=25.0, float rv=5.0);
   
   // Constructor #2.
   // Creates an Obstacle with all values specified by the user..
   // No checks will be made if the position lies outside the CBox (world) extents.
   // Obstacles are always placed on the "ground" of the Box they exist in, so
   // no "y" value is supplied.

   CObstacle (float hv, float rv, float xposv, float zposv);

   // Destructor

   virtual ~CObstacle();

   ////////////////////
   // rendering methods
   ////////////////////

   void Draw (void);

   //////////////////////////
   // miscellaneous functions
   //////////////////////////

   // GetID.
   // Returns the ID of the Obstacle in question.

   int GetID (void);

   // GetBaseRadius.
   // Returns the radius of the Obstacle's base, in meters.

   float GetBaseRadius (void);

   // GetHeight.
   // Returns the height of the Obstacle, in meters.

   float GetHeight (void);

   // GetPos.
   // Returns the postion of the Obstacle, in meters, as class "vector". 

   vector * GetPos (void);

   // GetRelativeRadius.
   // Returns the relative radius of the Obstacle at the height 
   // specified, in meters.

   float GetRelativeRadius (float refheightv);

   // IsHeigher.
   // Returns a true/false whether or not the reference height
   // is higher than the height of the obstacle.

   bool IsHeigher (float refheightv);

private:

   int    m_id;            // id of this Obstacle

   float  m_Radius;        // radius of Obstacle
   float  m_Diameter;      // diameter of Obstacle (computed for speed)
   float  m_Height;        // height of Obstacle

   vector m_Position;      // position of Obstacle

   float  m_Ratio;         // ratio of height to radius

};

#endif