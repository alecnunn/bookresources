/* Copyright (C) Steve Rabin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steve Rabin, 2001"
 */
int count = 0;
Clear( currentGroup );
for( element in list )
{	
  if( element is an "open bracket" ) {
     count++;
     Add entity to currentGroup;
  }
  else { //element is a "closed bracket"
     count--;
     if( count == 0 ) { //entire group found
        Store( currentGroup );
        Clear( currentGroup );
     }
  }
}
assert( count == 0 );
