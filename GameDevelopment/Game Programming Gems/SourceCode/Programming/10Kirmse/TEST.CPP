/* Copyright (C) Andrew Kirmse, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Andrew Kirmse, 2000"
 */
#include "twobitarray.h"
#include "bitarray2d.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


void fail(char *fmt, ...)
{
   char buf[1000];
   va_list marker;
   va_start(marker, fmt);

   vsprintf(buf, fmt, marker);
   
   va_end(marker);
   
   puts(buf);

   assert(false);
}

//*********************************************************************************
//*
//* BitArray tests
//*
//*********************************************************************************

void TestBitArrayBasic(int size)
{
   BitArray a(size);

   a.Clear();

   // Is "a" empty?
   int i, j;
   for (i = 0; i < size; i++)
      if (a.IsBitSet(i))
         fail("BitArray clear size %d pos %d", size, i);

   // Try setting each bit
   for (i = 0; i < size; i++)
   {
      a.SetBit(i);
      if (!a.IsBitSet(i))
         fail("BitArray set size %d pos %d", size, i);
   }

   // Try clearing each bit
   for (i = 0; i < size; i++)
   {
      a.ClearBit(i);
      if (a.IsBitSet(i))
         fail("BitArray reset size %d pos %d", size, i);
   }

   // Try flipping each bit
   for (i = 0; i < size; i++)
   {
      a.FlipBit(i);
      if (!a.IsBitSet(i))
         fail("BitArray toggle size %d pos %d", size, i);
   }

   a.Clear();
   if (!a.AllBitsFalse())
      fail("BitArray AllBitsFalse size %d", size);

   // Try turning on each bit; make sure all other bits are off
   for (i=0; i < size; i++)
   {
      a.SetBit(i);
      if (!a.IsBitSet(i))
         fail("BitArray long set size %d pos %d", size, i);

      if (a.AllBitsFalse())
         fail("BitArray long all zero size %d pos %d", size, i);
      
      for (j=0; j < size; j++)
         if (i != j && a.IsBitSet(j))
            fail("BitArray long IsBitSet size %d pos %d, %d", size, i, j);

      a.ClearBit(i);
   }

   // Same as above, but clear bit with "flip" instead
   for (i=0; i < size; i++)
   {
      a.SetBit(i);
      if (!a.IsBitSet(i))
         fail("BitArray long set size %d pos %d", size, i);

      if (a.AllBitsFalse())
         fail("BitArray long all zero size %d pos %d", size, i);
      
      for (j=0; j < size; j++)
         if (i != j && a.IsBitSet(j))
            fail("BitArray long IsBitSet size %d pos %d, %d", size, i, j);

      a.FlipBit(i);
   }

   a.Clear();
   a.FlipAllBits();
   for (i=0; i < size; i++)
   {
      if (!a.IsBitSet(i))
         fail("BitArray FlipAllBits size %d pos %d", size, i);
   }

   // Set all even-numbered bits
   a.Clear();
   for (i=0; i < size; i++)
   {
      if ((i % 2) == 0)
         a.SetBit(i);
   }

   a.FlipAllBits();

   // Now all odd bits are set
   for (i=0; i < size; i++)
   {
      if ((i % 2) == 0)
      {
         if (a.IsBitSet(i))
            fail("BitArray FlipAllBits even size %d pos %d", size, i);
      }
      else
         if (!a.IsBitSet(i))
            fail("BitArray FlipAllBits odd size %d pos %d", size, i);
   }

   a.Clear();
   for (i=0; i < size; i++)
   {
      if (true != a[i].Flip())
         fail("BitArray Flip size %d pos %d", size, i);

      if (true != a[i])
         fail("BitArray Flip get size %d pos %d", size, i);
   }
   
}

void TestBitArrayOperators(unsigned size)
{
   BitArray a(size);

   unsigned i, j;

   a.Clear();

   // operator[]
   for (i=0; i < size; i++)
   {
      if (a[i] == true)
         fail("BitArray [] clear size %d pos %d", size, i);
   }

   for (i=0; i < size; i++)
   {
      a[i] = true;
      if (!a.IsBitSet(i))
         fail("BitArray [] assign size %d pos %d", size, i);
      
      for (j=0; j < size; j++)
         if (i != j && a[j] == true)
            fail("BitArray [] assign check size %d pos %d, %d", size, i, j);

      a[i] = false;
      if (a.IsBitSet(i))
         fail("BitArray [] assign size %d pos %d", size, i);
   }

   
   // Test a[x] = a[y]
   a.Clear();
   if (size > 0)
      a[0] = true;
   for (i=1; i < size; i++)
   {
      a[i] = a[i - 1];
      // Bits up to i should be true
      for (j=0; j <= i; j++)
         if (!a.IsBitSet(j))
            fail("BitArray [] assign[] true size %d pos %d", size, i);
   
      // Bits past i i should be false
      for (j= i + 1; j < size; j++)
         if (a.IsBitSet(j))
            fail("BitArray [] assign[] false size %d pos %d", size, i);
   }

   // operator==

   a.Clear();
   
   if (a != a)
      fail("BitArray !=  size %d", size, i);

   if (!(a == a))
      fail("BitArray =  size %d", size, i);

   if (size > 0)
   {
      BitArray b = a;
      a.SetBit(size - 1);

      if (b == a)
         fail("BitArray = set size %d", size, i);

      b.SetBit(size - 1);

      if (b != a)
         fail("BitArray != set size %d", size, i);
   }

   // operator~
   a.Clear();
   BitArray b = ~a;

   if (!a.AllBitsFalse())
      fail("BitArray ~ clear size %d", size);

   for (i=0; i < size; ++i)
      if (!b.IsBitSet(i))
         fail("BitArray ~ filled size %d", size);

   
   // operator &=, |=, ^=
   a.Clear();

   b = ~a;
   if (b.AllBitsFalse() && size > 0)
      fail("BitArray ~ b size %d", size);

   b &= a;
   if (!b.AllBitsFalse())
      fail("BitArray &= b size %d", size);
   if (!a.AllBitsFalse())
      fail("BitArray &= a size %d", size);

   a = ~a;
   b |= a;
   // Both have all bits set now
   if (a != b)
      fail("BitArray |= size %d", size);

   b |= a;
   if (a != b)
      fail("BitArray |= size %d", size);

   // This clears b
   b ^= a;
   if (!b.AllBitsFalse())
      fail("BitArray ^= b size %d", size);

   // This clears a
   a ^= a;
   if (!a.AllBitsFalse())
      fail("BitArray ^= a size %d", size);

   BitArray c = a | b;
   if (!c.AllBitsFalse())
      fail("BitArray ^ size %d", size);
}

void TestBitArray()
{
   const int max_size = 200;
   
   int i;
   for (i=0; i < max_size; i++)
      TestBitArrayBasic(i);

   for (i=0; i < max_size; i++)
      TestBitArrayOperators(i);   
}

//*********************************************************************************
//*
//* BitArray2D tests
//*
//*********************************************************************************

void TestBitArray2DBasic(int width, int height)
{
   BitArray2D a(height, width);

   a.Clear();

   // Is "a" empty?
   int i, j, k, l;
   for (i = 0; i < height; i++)
      for (j = 0; j < width; j++)
         if (a.IsBitSet(i, j))
            fail("BitArray2D clear size %d %d pos %d %d", height, width, i, j);

   // Try setting each bit
   for (i = 0; i < height; i++)
      for (j = 0; j < width; j++)
      {
         a.SetBit(i, j);
         if (!a.IsBitSet(i, j))
            fail("BitArray2D set size %d %d pos %d %d", height, width, i, j);
      }

   // Now clear each bit
   for (i = 0; i < height; i++)
      for (j = 0; j < width; j++)
      {
         a.ClearBit(i, j);
         if (a.IsBitSet(i, j))
            fail("BitArray2D set size %d %d pos %d %d", height, width, i, j);
      }
   
   // Flip each bit to true
   for (i = 0; i < height; i++)
      for (j = 0; j < width; j++)
      {
         a.FlipBit(i, j);
         if (!a.IsBitSet(i, j))
            fail("BitArray2D toggle size %d %d pos %d %d", height, width, i, j);
      }

   a.Clear();
   if (!a.AllBitsFalse())
      fail("BitArray2D AllBitsFalse size %d %d", height, width);

   // Try turning on each bit; make sure all other bits are off
   for (i=0; i < height; i++)
      for (j=0; j < width; j++)
      {
         a.SetBit(i, j);
         if (!a.IsBitSet(i, j))
            fail("BitArray2D long set size %d %d pos %d %d", height, width, i, j);
         
         if (a.AllBitsFalse())
            fail("BitArray2D long all zero size %d %d pos %d %d", height, width, i, j);
      
         for (k=0; k < height; k++)
            for (l=0; l < width; l++)
               if ((i != k || j != l) && a.IsBitSet(k, l))
                  fail("BitArray2D long IsBitSet size %d %d pos %d %d", height, width, k, l);
         
         a.ClearBit(i, j);
      }

   // Same as above, but turn off bit with "Flip"
   for (i=0; i < height; i++)
      for (j=0; j < width; j++)
      {
         a.SetBit(i, j);
         if (!a.IsBitSet(i, j))
            fail("BitArray2D long set size %d %d pos %d %d", height, width, i, j);
         
         if (a.AllBitsFalse())
            fail("BitArray2D long all zero size %d %d pos %d %d", height, width, i, j);
      
         for (k=0; k < height; k++)
            for (l=0; l < width; l++)
               if ((i != k || j != l) && a.IsBitSet(k, l))
                  fail("BitArray2D long IsBitSet size %d %d pos %d %d", height, width, k, l);
         
         a.FlipBit(i, j);
      }

   // Set all even-numbered bits (checkerboard pattern)
   a.Clear();
   for (i=0; i < height; i++)
      for (j=0; j < width; j++)
      {
         if (((i + j) % 2) == 0)
            a.SetBit(i, j);
      }

   a.FlipAllBits();

   // Now all odd bits are set
   for (i=0; i < height; i++)
      for (j=0; j < width; j++)
      {
         if (((i + j) % 2) == 0)
         {
            if (a.IsBitSet(i, j))
            fail("BitArray2D FlipAllBits even size %d %d pos %d %d", height, width, i, j);
         }
         else
            if (!a.IsBitSet(i, j))
               fail("BitArray2D FlipAllBits odd size %d %d pos %d %d", height, width, i, j);
      }
   
}

void TestBitArray2DOperators(int width, int height)
{
   BitArray2D a(height, width);

   unsigned i, j, k, l;

   a.Clear();

   // operator[] 
   for (i=0; i < height; i++)
      for (j=0; j < width; j++)
      {
         if (a[i][j] == true)
            fail("BitArray2D [] clear size %d %d pos %d %d", height, width, i, j);
      }

   for (i=0; i < height; i++)
      for (j=0; j < width; j++)
      {
         a[i][j] = true;
         if (!a.IsBitSet(i, j))
            fail("BitArray2D [] assign size %d %d pos %d %d", height, width, i, j);
         if (!a[i][j])
            fail("BitArray2D [] assign size %d %d pos %d %d", height, width, i, j);
      
         for (k=0; k < height; k++)
            for (l=0; l < width; l++)
               if ((i != k || j != l) && a[k][l] == true)
                  fail("BitArray2D [] assign check size %d %d pos %d %d", height, width, i, j);

         a[i][j] = false;
         if (a.IsBitSet(i, j))
            fail("BitArray2D [] assign size %d %d pos %d %d", height, width, i, j);
         if (a[i][j])
            fail("BitArray2D [] assign size %d %d pos %d %d", height, width, i, j);
      }

   // operator==

   a.Clear();
   
   if (a != a)
      fail("BitArray2D !=  size %d %d", height, width);

   if (!(a == a))
      fail("BitArray2D =  size %d %d", height, width);

   if (width * height > 0)
   {
      BitArray2D b = a;
      a.SetBit(height - 1, width - 1);

      if (b == a)
         fail("BitArray2D = set size %d %d", height, width);

      b.SetBit(height - 1, width - 1);

      if (b != a)
         fail("BitArray2D != set size %d %d", height, width);
   }

   // operator~
   a.Clear();
   BitArray2D b = ~a;

   if (!a.AllBitsFalse())
      fail("BitArray2D ~ clear size %d %d", height, width);

   for (i=0; i < height; i++)
      for (j=0; j < width; j++)
         if (!b.IsBitSet(i, j))
            fail("BitArray2D ~ filled size %d %d", height, width);

   
   // operator &=, |=, ^=
   a.Clear();

   b = ~a;
   if (b.AllBitsFalse() && width * height > 0)
      fail("BitArray2D ~ b size %d %d", height, width);

   b &= a;
   if (!b.AllBitsFalse())
      fail("BitArray2D &= b size %d %d", height, width);
   if (!a.AllBitsFalse())
      fail("BitArray2D &= a size %d %d", height, width);

   a = ~a;
   b |= a;
   // Both have all bits set now
   if (a != b)
      fail("BitArray2D |= size %d %d", height, width);

   b |= a;
   if (a != b)
      fail("BitArray2D |= size %d %d", height, width);

   b ^= a;
   if (!b.AllBitsFalse())
      fail("BitArray2D ^= b size %d %d", height, width);

   a ^= a;
   if (!a.AllBitsFalse())
      fail("BitArray2D ^= a size %d %d", height, width);
}

void TestBitArray2D()
{
   const int max_size = 20;

   int i, j;
   for (i=0; i < max_size; i++)
      for (j=0; j < max_size; j++)
         TestBitArray2DBasic(i, j);   

   TestBitArray2DBasic(1, 100);
   TestBitArray2DBasic(100, 1);
   TestBitArray2DBasic(5, 50);
   TestBitArray2DBasic(50, 5);

   for (i=0; i < max_size; i++)
      for (j=0; j < max_size; j++)
         TestBitArray2DOperators(i, j);   

   TestBitArray2DOperators(1, 100);
   TestBitArray2DOperators(100, 1);
   TestBitArray2DOperators(5, 50);
   TestBitArray2DOperators(50, 5);
}


//*********************************************************************************
//*
//* TwoBitArray tests
//*
//*********************************************************************************

void TestTwoBitArrayBasic(int size)
{
   TwoBitArray a(size);

   a.Clear();

   // Is "a" empty?
   int i, j, num;
   for (i = 0; i < size; i++)
      if (a.Get(i) != 0)
         fail("TwoBitArray clear size %d pos %d", size, i);

   // Try setting each #
   for (num = 0; num < 4; num++)
      for (i = 0; i < size; i++)
      {
         a.Set(i, num);
         if (a.Get(i) != num)
            fail("TwoBitArray set size %d pos %d", size, i);
      }

   a.Clear();
   
   // Try flipping each bit
   for (i = 0; i < size; i++)
   {
      a.FlipBits(i);
      if (a.Get(i) != 3)
         fail("TwoBitArray toggle size %d pos %d", size, i);
   }

   a.Clear();
   if (!a.AllZero())
      fail("TwoBitArray AllZero size %d", size);

   // Try turning on each bit; make sure all other bits are off
   for (i=0; i < size; i++)
   {
      a.Set(i, 3);
      if (a.Get(i) != 3)
         fail("TwoBitArray long set size %d pos %d", size, i);

      if (a.AllZero())
         fail("TwoBitArray long all zero size %d pos %d", size, i);
      
      for (j=0; j < size; j++)
         if (i != j && a.Get(j) != 0)
            fail("TwoBitArray long IsBitSet size %d pos %d, %d", size, i, j);

      a.Set(i, 0);
   }

   // Same as above, but clear bit with "flip" instead
   for (i=0; i < size; i++)
   {
      a.Set(i, 3);
      if (a.Get(i) != 3)
         fail("TwoBitArray long set size %d pos %d", size, i);

      if (a.AllZero())
         fail("TwoBitArray long all zero size %d pos %d", size, i);
      
      for (j=0; j < size; j++)
         if (i != j && a.Get(j) != 0)
            fail("TwoBitArray long IsBitSet size %d pos %d, %d", size, i, j);

      a.FlipBits(i);
   }

   a.Clear();
   a.FlipAllBits();
   for (i=0; i < size; i++)
   {
      if (a.Get(i) != 3)
         fail("TwoBitArray FlipAllBits size %d pos %d", size, i);
   }

   // Set all even-numbered bits
   a.Clear();
   for (i=0; i < size; i++)
   {
      if ((i % 2) == 0)
         a.Set(i, 1);
   }

   a.FlipAllBits();

   // Now all odd positions are 3, even ones are 2
   for (i=0; i < size; i++)
   {
      if ((i % 2) == 0)
      {
         if (a.Get(i) != 2)
            fail("TwoBitArray FlipAllBits even size %d pos %d", size, i);
      }
      else
         if (a.Get(i) != 3)
            fail("TwoBitArray FlipAllBits odd size %d pos %d", size, i);
   }
}

void TestTwoBitArrayOperators(int size)
{
   TwoBitArray a(size);

   unsigned i, j;

   a.Clear();

   // operator[]
   for (i=0; i < size; i++)
   {
      if (a[i] != 0)
         fail("TwoBitArray [] clear size %d pos %d", size, i);
   }

   for (i=0; i < size; i++)
   {
      a[i] = 2;
      if (a.Get(i) != 2)
         fail("TwoBitArray [] assign size %d pos %d", size, i);
      
      for (j=0; j < size; j++)
         if (i != j && a[j] != 0)
            fail("TwoBitArray [] assign check size %d pos %d, %d", size, i, j);

      a[i] = 0;
      if (a.Get(i) != 0)
         fail("TwoBitArray [] assign size %d pos %d", size, i);
   }

   
   // Test a[x] = a[y]
   a.Clear();
   if (size > 0)
      a[0] = 3;
   for (i=1; i < size; i++)
   {
      a[i] = a[i - 1];
      // Bits up to i should be true
      for (j=0; j <= i; j++)
         if (a.Get(j) != 3)
            fail("TwoBitArray [] assign[] true size %d pos %d", size, i);
   
      // Bits past i i should be false
      for (j= i + 1; j < size; j++)
         if (a.Get(j) != 0)
            fail("TwoBitArray [] assign[] false size %d pos %d", size, i);
   }

   // operator==

   a.Clear();
   
   if (a != a)
      fail("TwoBitArray !=  size %d", size, i);

   if (!(a == a))
      fail("TwoBitArray =  size %d", size, i);

   if (size > 0)
   {
      TwoBitArray b = a;
      a.Set(size - 1, 2);

      if (b == a)
         fail("TwoBitArray = set size %d", size, i);

      b.Set(size - 1, 2);

      if (b != a)
         fail("TwoBitArray != set size %d", size, i);
   }

   // operator~
   a.Clear();
   TwoBitArray b = ~a;

   if (!a.AllZero())
      fail("TwoBitArray ~ clear size %d", size);

   for (i=0; i < size; ++i)
      if (b.Get(i) != 3)
         fail("TwoBitArray ~ filled size %d", size);

   
   // operator &=, |=, ^=
   a.Clear();

   b = ~a;
   if (b.AllZero() && size > 0)
      fail("TwoBitArray ~ b size %d", size);

   b &= a;
   if (!b.AllZero())
      fail("TwoBitArray &= b size %d", size);
   if (!a.AllZero())
      fail("TwoBitArray &= a size %d", size);

   a = ~a;
   b |= a;
   // Both have all bits set now
   if (a != b)
      fail("TwoBitArray |= size %d", size);

   b |= a;
   if (a != b)
      fail("TwoBitArray |= size %d", size);

   // This clears b
   b ^= a;
   if (!b.AllZero())
      fail("TwoBitArray ^= b size %d", size);

   // This clears a
   a ^= a;
   if (!a.AllZero())
      fail("TwoBitArray ^= a size %d", size);

   TwoBitArray c = a | b;
   if (!c.AllZero())
      fail("TwoBitArray ^ size %d", size);
}


void TestTwoBitArray()
{
   const int max_size = 200;
   
   int i;
   for (i=0; i < max_size; i++)
      TestTwoBitArrayBasic(i);

   for (i=0; i < max_size; i++)
      TestTwoBitArrayOperators(i);   
}

//*********************************************************************************
int main(int argc, char **argv)
{
   printf("Testing bit array: ");
   TestBitArray();
   printf("PASSED\n");

   printf("Testing 2D bit array: ");
   TestBitArray2D();
   printf("PASSED\n");

   printf("Testing two-bit array: ");
   TestTwoBitArray();
   printf("PASSED\n");

   return 0;
}

