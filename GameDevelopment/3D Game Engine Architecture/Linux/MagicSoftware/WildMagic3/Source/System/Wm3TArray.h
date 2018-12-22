// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3TARRAY_H
#define WM3TARRAY_H

// The class T is either native data or is class data that has the following
// member functions:
//   T::T ()
//   T::T (const T&);
//   T& T::operator= (const T&)

namespace Wm3
{

template <class T>
class TArray
{
public:
    // construction and destruction
    TArray (int iQuantity = 1, int iGrowBy = 1);
    TArray (const TArray& rkObject);
    ~TArray ();

    // assignment
    TArray& operator= (const TArray& rkObject);

    // element access, index i must be in range
    int GetQuantity () const;
    T* GetArray ();
    const T* GetArray () const;
    T& operator[] (int i);
    const T& operator[] (int i) const;

    // add new element, array will dynamically grow if necessary
    void Append (const T& rtElement);
    void SetElement (int i, const T& rtElement);

    // Remove the element at the specified index.  The elements occurring
    // after that one are shifted so that the array remains contiguous.  After
    // the shift, but before the decrement on quantity, array[quantity-1] is
    // a duplicate of array[quantity-2].  The element at index quantity-1 is
    // reconstructed using the default constructor for class T.  Then the
    // quantity is decremented.
    void Remove (int i);

    // all elements are set to the default object of class T
    void RemoveAll ();

    // dynamic growth, new array elements are default constructed
    void SetMaxQuantity (int iNewMaxQuantity, bool bCopy);
    int GetMaxQuantity () const;
    void SetGrowBy (int iGrowBy);
    int GetGrowBy () const;

private:
    int m_iQuantity, m_iMaxQuantity, m_iGrowBy;
    T* m_atArray;
};

#include "Wm3TArray.inl"

}

#endif
