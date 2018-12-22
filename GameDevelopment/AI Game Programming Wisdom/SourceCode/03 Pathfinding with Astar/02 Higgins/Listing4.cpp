/* Copyright (C) Dan Higgins, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Higgins, 2001"
 */
long GetTileCost( U2Dpoint<long>& inFrom, U2Dpoint<long>& inTo )
{
    // Modifier goals keep us from having more pathfinder  
    // classes when all we need is a 2 method modifier
    if (this->mModifierGoal == NULL)
        return this->InternalGetTileCost(inFrom,inTo);
    else
        return this->mModiferGoal->GetTileCost(inFrom,inTo);
}
