// A two-dimensional array of bits.
//
// Copyright 2000 Andrew Kirmse.  All rights reserved.
//
// Permission is granted to use this code for any purpose, as long as this
// copyright message remains intact.

#ifndef _BITARRAY2D_H
#define _BITARRAY2D_H

#include "bitarray.h"

class BitArray2D : private BitArray
{
   typedef BitArray super;
   
public:

   //
   // Constructors
   //

   BitArray2D(unsigned dim1, unsigned dim2) : super(dim1 * dim2)
   {
      mWidth = dim2;
   }

   BitArray2D(const BitArray2D &that) : super(that)
   {
      mWidth = that.mWidth;
   }

   //
   // Operators
   //

   class ArrayProxy;
   
   BitArray2D &operator=(const BitArray2D &that)
   {
      super::operator=(that);
      mWidth = that.mWidth;
      return *this;
   }

   ArrayProxy operator[](unsigned pos)
   {
      return ArrayProxy(*this, pos);
   }

   const ArrayProxy operator[](unsigned pos) const
   {
      return ArrayProxy(const_cast<BitArray2D &>(*this), pos);
   }
   
   bool operator==(const BitArray2D &that) const
   {
      return super::operator==(that);
   }

   bool operator!=(const BitArray2D &that) const
   {
      return !(*this == that);
   }

   BitArray2D &operator&=(const BitArray2D &that)
   {
      super::operator&=(that);
      return *this;
   }

   BitArray2D operator|=(const BitArray2D &that)
   {
      super::operator|=(that);
      return *this;
   }

   BitArray2D operator^=(const BitArray2D &that)
   {
      super::operator^=(that);
      return *this;
   }

   BitArray2D operator~() const
   {
      return BitArray2D(*this).FlipAllBits();
   }

   friend BitArray2D operator&(const BitArray2D &a1, const BitArray2D &a2)
   {
      return BitArray2D(a1) &= a2;
   }

   friend BitArray2D operator|(const BitArray2D &a1, const BitArray2D &a2)
   {
      return BitArray2D(a1) |= a2;
   }

   friend BitArray2D operator^(const BitArray2D &a1, const BitArray2D &a2)
   {
      return BitArray2D(a1) ^= a2;
   }

   //
   // Plain English interface
   //
   
   // Set all bits to false.
   void Clear()
   {
      super::Clear();
   }
   
   // Set the bit at position pos to true.
   void SetBit(unsigned pos1, unsigned pos2)
   {
      super::SetBit(pos1 * mWidth + pos2);
   }

   // Set the bit at position pos to false.
   void ClearBit(unsigned pos1, unsigned pos2)
   { 
      super::ClearBit(pos1 * mWidth + pos2);
   }

   // Toggle the bit at position pos.
   void FlipBit(unsigned pos1, unsigned pos2)
   { 
      super::FlipBit(pos1 * mWidth + pos2);
   }

   // Set the bit at position pos to the given value.
   void Set(unsigned pos1, unsigned pos2, bool val)
   { 
      val ? SetBit(pos1, pos2) : ClearBit(pos1, pos2);
   }

   // Returns true iff the bit at position pos is true.
   bool IsBitSet(unsigned pos1, unsigned pos2) const
   {
      return super::IsBitSet(pos1 * mWidth + pos2);
   }

   // Returns true iff all bits are false.
   bool AllBitsFalse() const
   {
      return super::AllBitsFalse();
   }

   // Change value of all bits
   BitArray2D &FlipAllBits()
   {
      super::FlipAllBits();
      return *this;
   }

   //
   // Array proxy (for operator[])
   //
   
   friend class ArrayProxy;
   
   class ArrayProxy
   {
   public:
      ArrayProxy(BitArray2D &array, unsigned pos):
            mArray(array), mPos(pos)
      {}

      BitProxy operator[](unsigned pos) const
      {
         return BitProxy(mArray, mPos * mArray.mWidth + pos);
      }

   private:
      BitArray2D &mArray;
      unsigned    mPos;  // We are a proxy for this row of the array
   };

private:
   unsigned mWidth;
};

#endif
