/* Copyright (C) Marco Pinter, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Marco Pinter, 2001"
 */

// Determining position and orientation at a given time interval.

// CalcPosition: Determine current position based on
//   pre-calculated path between waypoints.
// Input Parameters:  speed, elapsed, radius, P, Q,
//       curveLength, bTurnRight, angleStart, orientFinal
// Output parameters: orientation, curPos

// Compute the total distance covered in the path so far
dist = speed * elapsed;
// If the agent is still in the curved portion...
if (dist < curveLength)
{
	// Find the angle on the arc where the agent is
	theta = angleStart 
              + (bTurnRight ? -1.0 : 1.0) * dist / radius;

	// Determine the current position and orientation
	curPos.x = P.x + radius * cos(theta);
	curPos.y = P.y + radius * sin(theta);
	orientation = bTurnRight ? theta - PI_VAL / 2 
                                 : theta + PI_VAL / 2);
}
// If the agent is on the linear portion
else
{
	// Find the distance we've moved along the straight line
	distNow = dist - curveLength;
	// Find the current position and orientation
	curPos.x = Q.x + distNow * cos(orientFinal);
	curPos.y = Q.y + distNow * sin(orientFinal);
	orientation = orientFinal;
}
