/* Copyright (C) Alex McLean, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Alex McLean, 2001"
 */

// Now that we have our tasks split into two sets, we are 
// able to look at a newer version of our main AI loop. 

CGame::UpdateAI( void )
{
    CCharacter   *pChar;
    BOOL         bExit = FALSE;
    unsigned int CharsProcessed=0;

    pChar =GetFirstCharacter();

    /* CONSTANT */
    while( pChar )
    {
        pChar->ProcessConstantAI();
        pChar = pChar->GetNext();
    }

    /* PERIODIC */
    while( CharsProcessed < m_CharsPerFrame )
    {
        m_pPresentCharacter->ProcessPeriodicAI();
        CharsProcessed++;
        m_pPresentCharacter = m_pPresentCharacter->GetNextCyclic();
    }
}

CCharacter::ProcessConstantAI( void )
{
    TrackTarget();
    UpdateMovement();
}

CCharacter::ProcessPeriodicAI( void )
{
    ProcessVision();
    ProcessHearing();
}

