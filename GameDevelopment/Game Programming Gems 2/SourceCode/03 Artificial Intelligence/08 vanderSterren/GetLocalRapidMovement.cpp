/* Copyright (C) William van der Sterren, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) William van der Sterren, 2001"
 */
float GetLocalRapidMovement( waypointid wp )
{ // result in [0 .. 1], higher values meaning higher speeds
  return ( GetActorMovementSpeedAtWaypoint( wp )
           / GetMaxActorMovementSpeed( ) );
}		
float GetOpenAreaMembership ( waypointid wp )
{ // result in [0 .. 1], higher values meaning more open
  return 1.0 - max( IsPartOfSmallRoom( wp ),
                    IsPartOfTunnel( wp ) );
}		
