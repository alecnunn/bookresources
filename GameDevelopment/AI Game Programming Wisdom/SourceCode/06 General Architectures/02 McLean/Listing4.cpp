/* Copyright (C) Alex McLean, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Alex McLean, 2001"
 */

BOOL CCharacter::CanBeExcludedFromUpdate( void )
{
    if( TRUE == m_bCanNeverBeExcluded ) {
        return FALSE;
    }
    if( IsDead() ) {	
        return TRUE;
    }
    if( m_DistanceToPlayer > CUTOFF_THRESHOLD )	{
        return TRUE;
    }	
}
