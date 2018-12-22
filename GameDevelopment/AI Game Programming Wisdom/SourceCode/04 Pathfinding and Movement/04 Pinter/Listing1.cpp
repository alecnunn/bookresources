/* Copyright (C) Marco Pinter, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Marco Pinter, 2001"
 */

// Determining the fastest route between two waypoints.

// CalculateSegment:  Determine the fastest route between two waypoints
// Input parameters:  origin, orientStart, bTurnRight, radius
// Output parameters: P, Q, curveLength, d, angleStart, orientFinal

// Calculate location of P (distance r from Origin, and
//   center of the turning circle)
// (Turning right from the origin means that P is located
//   90 degrees clockwise from the initial orientation; 
//   Turning left means the reverse.)

if (bTurnRight)
	angleToP = orientStart - PI / 2.0;
else
	angleToP = orientStart + PI / 2.0;

P.x = origin.x + radius * cos(angleToP);
P.y = origin.y + radius * sin(angleToP);

// Calculate distance from P to Destination.
dx = dest.x - P.x;
dy = dest.y - P.y;
h = sqrt(dx*dx + dy*dy);

// If the destination is inside the circle,
//   we can't reach it.
if (h < r)
   return false;

// Calculate the length of 'd' based on the other two sides
//   of the right triangle, and theta as well.
d = sqrt(h*h - radius*radius);
theta = acos(radius / h);

// Calculate angle phi from arctangent relationship
phi = atan2(dy, dx);

// Determine point Q from position P and angle
angleFinal = (bTurnRight ? phi + theta : phi - theta);
Q.x = P.x + radius * cos(angleFinal);
Q.y = P.y + radius * sin(angleFinal);

// Calculate final values needed:
//  Total distance of curve; and final orientation
angleStart = angleToP + PI_VAL / 2;
totalCurve = bTurnRight ? angleStart - angleFinal
                        : angleFinal - angleStart;
curveLength = totalCurve * radius;
orientFinal = bTurnRight ? angleFinal - PI_VAL/2 
                         : angleFinal + PI_VAL/2;

return true;
