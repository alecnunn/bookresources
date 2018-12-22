/* Copyright (C) Steven Woodcock, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steven Woodcock, 2000"
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

#define MaxSpeed                     1.0f
#define MinUrgency                   0.05f
#define MaxUrgency                   0.1f
#define MaxChange                   (MaxSpeed * MaxUrgency)
#define DesiredSpeed                (MaxSpeed/2)
#define KeepAwayDist                 6.0f
#define SeparationDist               3.0f

#define MaxBoids                    30
#define MaxFlocks                    5
#define Max_Friends_Visible         MaxBoids/2
#define Default_Perception_Range    8.0f

#define UseTruth                    FALSE
#define ReactToEnemies              TRUE

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