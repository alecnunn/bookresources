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
// Name:     CObstacle.cpp: 
// Purpose:  Class methods for the CObstacle class.
//*********************************************************************

//
// includes
//

#include "CBox.h"
#include "CObstacle.h"
#include "math.h"
#include "util.h"
#include "glut.h"

//
// static variable initialization
//

int CObstacle::ObstacleCount = 0;

CObstacle * CObstacle::ListOfObstacles[] = {NULL};

///////////////////////////
// Construction/Destruction
///////////////////////////

// Constructor #1.
// Creates an Obstacle with default values of 25 meters tall and 5 meters radius.
// Position will be randomized.  Obstacles are always placed on the "ground" of
// the Box they exist in, so the "y" value is automatically constrained.

CObstacle::CObstacle (float hv, float rv)
{

#ifdef OBSTACLE_DEBUG
   myprintf("\nCObstacle constructor #1 called for Obstacle %d\n",ObstacleCount);
   myprintf("Height = %f  Radius = %f\n",hv, rv);
   myprintf("Position will be randomized...\n");
#endif

   // initialize id

   m_id = ObstacleCount;

   // set the params the user provided

   m_Height = hv;
   m_Radius = rv;

   m_Diameter = m_Radius * 2.0f;

   // compute the ratio (used for collision detection)

   m_Ratio = m_Radius/m_Height;

   // now randomly generate a position for the obstacle
   
   m_Position.x = RAND() * CBox::WorldPtr->GetBoxWidth()/2;
   m_Position.z = RAND() * CBox::WorldPtr->GetBoxLength()/2;
   
   // randomly flip positions for more variety

   if (RAND() > 0.5) m_Position.x *= -1;
   if (RAND() < 0.5) m_Position.z *= -1;

   // obstacles are always placed on the ground

   m_Position.y = -(CBox::WorldPtr->GetBoxHeight()/2);

   // store pointer to obstacle

   ListOfObstacles[ObstacleCount] = this;

   // increment counter

   ObstacleCount++;

}

// Constructor #2.
// Creates an Obstacle with all values specified by the user..
// No checks will be made if the position lies outside the CBox (world) extents.
// Obstacles are always placed on the "ground" of the Box they exist in, so
// no "y" value is supplied.

CObstacle::CObstacle (float hv, float rv, float xposv, float zposv)
{

#ifdef OBSTACLE_DEBUG
   myprintf("\nCObstacle constructor #2 called for Obstacle %d\n",ObstacleCount);
   myprintf("Height = %f  Radius = %f\n",hv, rv);
   myprintf("Postion x, y, z = %f %f %f\n",xposv, zposv);
#endif

   // initialize id

   m_id       = ObstacleCount;

   // set the params the user provided

   m_Height = hv;
   m_Radius = rv;

   m_Diameter = m_Radius * 2.0f;
   
   // compute the ratio (used for collision detection)

   m_Ratio = m_Radius/m_Height;

   // set the position the user provided

   m_Position.x = xposv;
   m_Position.z = zposv;

   // obstacles are always placed on the ground

   m_Position.y = -(CBox::WorldPtr->GetBoxHeight()/2);

   // store pointer to obstacle

   ListOfObstacles[ObstacleCount] = this;

   // increment counter

   ObstacleCount++;

}

CObstacle::~CObstacle ()
{

#ifdef OBSTACLE_DEBUG
   myprintf("\nCObstacle destructor called for Obstacle %d\n",m_id);
#endif

   // remove it from the list

   ListOfObstacles[ObstacleCount] = NULL;

   // decrement counter

   ObstacleCount--;

}

////////////////////
// rendering methods
////////////////////

void CObstacle::Draw (void)
{

   glPushMatrix();

      // translate to the obstacle's position

      glTranslatef(m_Position.x, m_Position.y, m_Position.z);

	   // rotate

      glRotatef(90.0, 0.0, 0.0, 1.0);
      glRotatef(90.0, 0.0, 1.0, 0.0);

	   // draw
	
      glColor3f(0.0f, 0.6f, 0.0f);
      glutWireCone (m_Radius, m_Height, 12.0f, 12.0f);

   glPopMatrix();

}

//////////////////////////
// miscellaneous functions
//////////////////////////

// GetID.
// Returns the ID of the Obstacle in question.

int CObstacle::GetID (void)
{
   return (m_id);

}

// GetBaseRadius.
// Returns the radius of the Obstacle's base, in meters.

float CObstacle::GetBaseRadius (void)
{

	return (m_Radius);

}

// GetHeight.
// Returns the height of the Obstacle, in meters.

float CObstacle::GetHeight (void)
{

   return (m_Height);

}

// GetPos.
// Returns the postion of the Obstacle, in meters, as class "vector". 

vector * CObstacle::GetPos (void)
{

	return (&m_Position);

}

// GetRelativeRadius.
// Returns the relative radius of the Obstacle at the height 
// specified, in meters.

float CObstacle::GetRelativeRadius (float refheightv)
{

   // Get relative height from "bottom" of world.
   // Obstacles are already on the bottom so we don't have
   // to make this calculation for them.

   float h1 = refheightv + (CBox::WorldPtr->GetBoxHeight()/2);

   // now we can compare heights to get a relative radius

   if (h1 > m_Height) {                      // we're higher than the obstacle
      return (0.0f);                                  
   } else {                            // we're alongside the obstacle
      return (m_Radius - (h1 * m_Ratio));
   }

}

// IsHeigher.
// Returns a true/false whether or not the reference height
// is higher than the height of the obstacle.

bool CObstacle::IsHeigher (float refheightv)
{

   // Get relative height from "bottom" of world.
   // Obstacles are already on the bottom so we don't have
   // to make this calculation for them.

   float h1 = refheightv + (CBox::WorldPtr->GetBoxHeight()/2);

   // now we can compare heights

   if (h1 > m_Height) {                      // we're higher than the obstacle
      return (TRUE);                                 
   } else {                                 // we're alongside the obstacle
      return (FALSE);
   }

}