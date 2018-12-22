/* Copyright (C) Gari Biasillo, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Gari Biasillo, 2001"
 */

// Once the AI has chosen its target, appropriate actions must 
// be taken to resolve how to reach it. The AI aims to mimic user 
// input so it must calculate the relative direction to travel and 
// apply the appropriate steering.

float CCarAI::SteerToTarget(const vector3& dest)
{
    vector3 DestForward, cp;

    DestForward = dest - m_Pos;
    DestForward.y = 0.0f;
    DestForward.normalize();

    // Compute the sine between current & destination.
    cp = CrossProduct(m_Forward, DestForward);
    float steer = cp.magnitude() * m_SteerConvert;

    // Steer left or right ?
    if (cp.y > 0.0f)  {steer = -steer;}
    steer = Clamp(steer, -1.0f, 1.0f);

    return (steer);
}
