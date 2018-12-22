/* Copyright (C) Matt Pritchard, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Matt Pritchard, 2001"
 */
void GameUnit::TurnUpdateProcess(. . .)
{
   // Game specific unit processing code…
   . . .
   // Now we check to see if we've gone in or out of anyone's LOS
   DWORD CurrentlyVisibleTo = CombinedVisibilityMap[Yposition][Xposition];
   
   if ( CurrentlyVisibleTo != LastVisibileToValue) {
      // Get only the bits that have changed
      DWORD VisibilityChanges = CurrentlyVisibleTo ^ LastVisibileToValue;
      LastVisibileToValue = CurrentlyVisibleTo;   // Save new value

      for (int playerNo = 0; playerNo < theGame->numOfPlayers; playerNo++)
      {
         DWORD PlayerMask = 0x0001 << playerNo;  // bit mask for player

         // Check to see if our visibility for this player has changed
         if ((VisibilityChanges & PlayerMask) != 0) {
            if ((CurrentlyVisibleTo & PlayerMask) != 0)
               AddUnitToPlayersVisibileList(playerNo, self);
            else
               RemoveUnitFromPlayersVisibileList(playerNo, self);   
         }
      }
   }
   // Continue with game processing
}   
