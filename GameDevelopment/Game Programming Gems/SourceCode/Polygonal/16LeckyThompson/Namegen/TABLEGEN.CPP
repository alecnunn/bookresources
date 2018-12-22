/* Copyright (C) Guy W. Lecky-Thompson, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Guy W. Lecky-Thompson, 2000"
 */
// Building a letter frequency table
// (c) 1999 Guy W. Lecky-Thompson
// All Rights Reserved

#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <ctype.h>
#endif

void AddLetters(char * szWord, unsigned long ulTable[28][28])
{
  int nWordLength, nFirstLetter, nLastLetter, nLetter;

  // Decapitalise the word
  for (nLetter = 0; nLetter < (int)strlen(szWord)-1;nLetter++)
	  tolower(szWord[nLetter]);
  
  // Add the first, and last to the table
  nWordLength = (int)strlen(szWord);
  nFirstLetter = (szWord[0] - 'a') + 1;
  nLastLetter  = (szWord[nWordLength-1] - 'a') + 1;

  ulTable[0][nFirstLetter]++;  // Space followed by letter
  ulTable[nLastLetter][27]++;  // Letter followed by space
  
  for (nLetter = 0; nLetter < nWordLength-2; nLetter++)
  {
    nFirstLetter = (szWord[nLetter]   - 'a') + 1;
    nLastLetter  = (szWord[nLetter+1] - 'a') + 1;
    
    ulTable[nFirstLetter][nLastLetter]++;
  }
}
