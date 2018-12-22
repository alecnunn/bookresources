/* Copyright (C) Gari Biasillo, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Gari Biasillo, 2001"
 */

// A simple structure is used to control the car, which contains 
// the steering and acceleration/braking values. Steering, dx, is 
// in the range of -1.0 to +1.0, representing full left to full 
// right. The second member, dy, has two uses depending on its 
// value which also has the range -1.0 to +1.0. Negative values 
// represent the braking amount (zero to 100%), and positive values 
// represent acceleration (zero to 100%).

struct CCarControl
{
    float    dx;	// -1.0 ... +1.0 = Left to Right steering
    float    dy;	// <0.0 = Braking, >0.0 Acceleration
};
