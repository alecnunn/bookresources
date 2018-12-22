/* Copyright (C) Andrew Kirmse, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Andrew Kirmse, 2000"
 */
#include "random.h"

/***************************************************************/
Random::Random()
{
   state = 0;
   GenerateSeeds();
}
/***************************************************************/
Random::Random(uint32 seed)
{
   state = seed;
   GenerateSeeds();
}
/***************************************************************/
Random::~Random()
{
}
/***************************************************************/
uint32 Random::Generate()
{
    uint32 retval = (table[index1] ^= table[index2]); 

    index1++;
    if (index1 == TABLE_SIZE)
       index1 = 0;

    index2++;
    if (index2 == TABLE_SIZE)
       index2 = 0;

    return retval;
}
/***************************************************************/
void Random::GenerateSeeds()
{
   int32 n;
   uint32 msk, bit;
   
   index1 = 0;
   index2 = 103;
   for (n = TABLE_SIZE - 1; n >= 0; n--)
      table[n] = GenerateSimple();
   for (n = 3, msk = 0xffffffff, bit = 0x80000000; bit; n += 7) 
   {
      table[n] = (table[n] & msk) | bit;
      msk >>= 1;
      bit >>= 1;
   }
}
/***************************************************************/
uint32 Random::GenerateSimple()
{
   // IL_Shift32Random from Inner Loops book
   uint32 n, bit, temp;
   temp = state;
   for (n=0; n < 32; n++)
   {
      bit = ((temp >> 0) ^ (temp >> 1) ^ (temp >> 2) ^
	     (temp >> 3) ^ (temp >> 5) ^ (temp >> 7)) & 1;
      temp = (((temp >> 1) | (temp << 31)) & ~1) | bit;
   }
   state = temp;
   return state;
}
