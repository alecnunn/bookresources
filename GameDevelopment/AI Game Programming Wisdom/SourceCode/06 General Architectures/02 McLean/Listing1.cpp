/* Copyright (C) Alex McLean, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Alex McLean, 2001"
 */

// A simple initial AI loop might look something like the following:

CGame::UpdateAI()
{
    CCharacter *pChar = GetFirstCharacter();
    while( pChar )
    {
        pChar->ProcessAI();
        pChar = pChar->GetNext();
    }
}

CCharacter::ProcessAI( void )
{
    ProcessVision();
    ProcessHearing();
    TrackTarget();
    UpdateMovement();
}
