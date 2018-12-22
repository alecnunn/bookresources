/* Copyright (C) Gari Biasillo, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Gari Biasillo, 2001"
 */

// A simple approach that works extremely well in practice is 
// to compute the traveled distance parallel to the driving line 
// as shown in the following code snippet.

float DistAlongSector(const CSector& sector, const vector3& pos)
{
    vector3 delta = pos - sector.drivingLinePos;
    delta.y = 0.0f;
    float dist = DotProduct(sector.drivingLineForward, delta);
    return (dist * lengthScale);
}
