/* Copyright (C) Guy W. Lecky-Thompson, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Guy W. Lecky-Thompson, 2000"
 */
// Building a word from a letter frequency table
// (c) 2000 Guy W. Lecky-Thompson
// All Rights Reserved

#include <stdio.h>
#include "Prandgen.h"  // Pseudo-randomizer
#include <string.h>

int GetLetterPosition(unsigned long ulWordTable[28][28], pseudorandom * prGenerator, int nPrevious)
{
  int nCounter;
  unsigned long ulFrequencyTotal, ulFrequencyRunningTotal, ulRandomLetter;

  // Get the frequencies
  for (nCounter = 1; nCounter < 27; nCounter++)
  {
    ulFrequencyTotal = ulFrequencyTotal + ulWordTable[nPrevious][nCounter];
  }
   
  // Choose a 'target' frequency
  ulRandomLetter = prGenerator->Rand(ulFrequencyTotal);

  // Move through the table until we hit the 'target' frequency
  ulFrequencyRunningTotal = 0;
  nCounter = 1;
  do
  {
    ulFrequencyRunningTotal = ulFrequencyRunningTotal + ulWordTable[nPrevious][nCounter];
    nCounter++;
  } while (ulFrequencyRunningTotal < ulRandomLetter);

  return nCounter;
}

void GenerateWord(int nLetters, char * szWord, unsigned long ulWordTable[28][28], pseudorandom * prGenerator)
{
  int nLetterPosition;

  // First, find the letter that can start the word
  nLetterPosition = GetLetterPosition(ulWordTable,prGenerator,0);
  // We have the first letter
  sprintf(szWord,"%c\0",(nLetterPosition-1) + 'a');

  // Repeat until nLetters have been added
  do
  { 
    nLetterPosition = GetLetterPosition(ulWordTable,prGenerator,nLetterPosition);
    sprintf(szWord,"%c\0",(nLetterPosition-1) + 'a');    
  } while ((int)strlen(szWord) < nLetters);

  printf("Word : %s\n",szWord);  
}