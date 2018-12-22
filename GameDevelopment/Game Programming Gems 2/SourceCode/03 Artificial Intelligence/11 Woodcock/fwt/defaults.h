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
// Name:     defaults.h
// Purpose:  Various default values used by the constructors 
//           when necessary.  Also contains all of the Boid constraints.
//*********************************************************************

#ifndef _DEFAULTS_H
#define _DEFAULTS_H

////////////////////////////////////////
// default values for various attributes
////////////////////////////////////////

#define MaxNameLength                12

#define HungerTrigger               20.0f
#define MaxHunger                   30.0f
#define MaxFeedingDistance           3.0f
#define HungerDeath                -30.0f
#define HungerPerCycle              -0.2f

#define NumBoidsForBaby              2

#define MaxPitch                     0.5f;

#define MaxSpeed                     1.0f
#define MinUrgency                   0.05f
#define MaxUrgency                   0.1f
#define MaxChange                    0.3f
#define DesiredSpeed                 (MaxSpeed/2)
#define KeepAwayDist                 6.0f
#define SeparationDist               3.0f

#define MaxFlies                     30
#define MaxSparrows                   5
#define MaxHawks                      2
#define MaxBoids                      (MaxFlies + MaxSparrows + MaxHawks)
#define MaxFlocks                     3
#define MaxObstacles                  4

#define MaxFriendsVisible             5
#define DefaultPerceptionRange       15.0f

////////////
// constants
////////////

#define TRUE   1
#define FALSE  0

#define PI        3.14159f
#define HALF_PI   (PI/2)

#define GRAVITY   9.806650f  // meters/second

#define INFINITY  999999.0f

#endif