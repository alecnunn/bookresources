/* Copyright (C) Gari Biasillo, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Gari Biasillo, 2001"
 */

// The velocity of a point relative to the center of gravity can 
// be calculated with the function PointVel().

vector3 PointVel(const vector3& linearVel, const vector3& angularVel,
                 const vector3& point)
{
    // linearVel = Linear velocity
    // angularVel = Axis of rotation * spin rate
    // point = Relative position to body's center of gravity

    return (linearVel + CrossProduct(angularVel, point));
}
