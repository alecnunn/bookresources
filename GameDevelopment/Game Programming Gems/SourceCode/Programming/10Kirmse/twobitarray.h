// A one-dimensional array of two-bit entries.
//
// Copyright 2000 Andrew Kirmse.  All rights reserved.
//
// Permission is granted to use this code for any purpose, as long as this
// copyright message remains intact.

#ifndef _TWOBITARRAY_H
#define _TWOBITARRAY_H

#include "bitarray.h"

class TwoBitArray : private BitArray
{
   typedef BitArray super;
   
public:

   //
   // Constructors
   //
   
   explicit TwoBitArray(unsigned size) : super(2 * size)
   {}
   
   TwoBitArray(const TwoBitArray &that) : super(that)
   {}
   
   //
   // Operators
   //

   class TwoBitProxy;
   
   TwoBitArray &operator=(const TwoBitArray &that)
   {
      super::operator=(that);
      return *this;
   }

   TwoBitProxy operator[](unsigned pos)
   {
      return TwoBitProxy(*this, pos);
   }

   const TwoBitProxy operator[](unsigned pos) const
   {
      return TwoBitProxy(const_cast<TwoBitArray &>(*this), pos);
   }
   
   bool operator==(const TwoBitArray &that) const
   {
      return super::operator==(that);
   }

   bool operator!=(const TwoBitArray &that) const
   {
      return !(*this == that);
   }

   TwoBitArray &operator&=(const TwoBitArray &that)
   {
      super::operator&=(that);
      return *this;
   }

   TwoBitArray operator|=(const TwoBitArray &that)
   {
      super::operator|=(that);
      return *this;
   }

   TwoBitArray operator^=(const TwoBitArray &that)
   {
      super::operator^=(that);
      return *this;
   }

   TwoBitArray operator~() const
   {
      return TwoBitArray(*this).FlipAllBits();
   }

   friend TwoBitArray operator&(const TwoBitArray &a1, const TwoBitArray &a2)
   {
      return TwoBitArray(a1) &= a2;
   }

   friend TwoBitArray operator|(const TwoBitArray &a1, const TwoBitArray &a2)
   {
      return TwoBitArray(a1) |= a2;
   }

   friend TwoBitArray operator^(const TwoBitArray &a1, const TwoBitArray &a2)
   {
      return TwoBitArray(a1) ^= a2;
   }

   //
   // Plain English interface
   //
   
   // Set all bits to false.
   void Clear()
   {
      super::Clear();
   }
   
   // Toggle the bits at position pos.
   void FlipBits(unsigned pos) 
   {
      Set(pos, 3 ^ Get(pos));
   }

   // Set the bit at position pos to the given value.
   void Set(unsigned pos, unsigned val)
   {
      super::Set(2 * pos, val & 1);
      super::Set(2 * pos + 1, (val >> 1) & 1);
   }

   // Returns true iff the bit at position pos is true.
   unsigned Get(unsigned pos) const
   {
      return (unsigned(super::IsBitSet(2 * pos)) |
              (unsigned(super::IsBitSet(2 * pos + 1)) << 1));
   }

   // Returns true iff all bits are false.
   bool AllZero() const
   {
      return super::AllBitsFalse();
   }

   // Change value of all bits
   TwoBitArray &FlipAllBits()
   {
      super::FlipAllBits();
      return *this;
   }

   //
   // Two bit proxy (for operator[])
   //
   
   friend class TwoBitProxy;
   
   class TwoBitProxy
   {
   public:
      TwoBitProxy(TwoBitArray &array, unsigned pos):
            mArray(array), mPos(pos)
      {}

      TwoBitProxy &operator=(const TwoBitProxy &that)
      {
         mArray.Set(mPos, that.mArray.Get(that.mPos));
         return *this;
      }

      TwoBitProxy &operator=(unsigned value)
      {
         mArray.Set(mPos, value);
         return *this;
      }

      operator unsigned() const
      {
         return mArray.Get(mPos);
      }

   private:
      TwoBitArray &mArray;
      unsigned     mPos;
   };
   
private:
};

#endif
