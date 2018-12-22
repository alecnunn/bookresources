/* Copyright (C) François Dominic Laramée, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) François Dominic Laramée, 2001"
 */

const int numLegalMoves = 20;
int Trigram[ numLegalMoves ][ numLegalMoves ][ numLegalMoves ];

// Act if the probabibility that A follows BA is high enough...
if( Trigram[ B ][ A ][ A ] > some threshold )
	do something;

// To find the most probable next move given that we know what the
// player has done recently...
topScore = MINUS_INFINITY;
for( int i = 0; i < numLegalMoves; i++ )
{
	if( Trigram[ slightlyOlderMove ][ mostRecentMove ][ i ] > topScore )
	{
		topScore = Trigram[ slightlyOlderMove ][ mostRecentMove ][ i ];
		probableNextMove = i;
	}
}

