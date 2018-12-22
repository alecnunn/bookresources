// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3TRVECTOR_H
#define WM3TRVECTOR_H

#include "Wm3TRational.h"

namespace Wm3
{

template <int VSIZE, int ISIZE>
class TRVector
{
public:
    // construction
    TRVector ();
    TRVector (const TRVector& rkV);

    // coordinate access
    operator const TRational<ISIZE>* () const;
    operator TRational<ISIZE>* ();
    TRational<ISIZE> operator[] (int i) const;
    TRational<ISIZE>& operator[] (int i);

    // assignment
    TRVector& operator= (const TRVector& rkV);

    // comparison
    bool operator== (const TRVector& rkV) const;
    bool operator!= (const TRVector& rkV) const;
    bool operator<  (const TRVector& rkV) const;
    bool operator<= (const TRVector& rkV) const;
    bool operator>  (const TRVector& rkV) const;
    bool operator>= (const TRVector& rkV) const;

    // arithmetic operations
    TRVector operator+ (const TRVector& rkV) const;
    TRVector operator- (const TRVector& rkV) const;
    TRVector operator* (const TRational<ISIZE>& rkR) const;
    TRVector operator/ (const TRational<ISIZE>& rkR) const;
    TRVector operator- () const;

    // arithmetic updates
    TRVector& operator+= (const TRVector& rkV);
    TRVector& operator-= (const TRVector& rkV);
    TRVector& operator*= (const TRational<ISIZE>& rkR);
    TRVector& operator/= (const TRational<ISIZE>& rkR);

    // vector operations
    TRational<ISIZE> SquaredLength () const;
    TRational<ISIZE> Dot (const TRVector& rkV) const;

protected:
    // support for comparisons
    int CompareArrays (const TRVector& rkV) const;

    TRational<ISIZE> m_akTuple[VSIZE];
};

template <int VSIZE, int ISIZE>
TRVector<VSIZE,ISIZE> operator* (const TRational<ISIZE>& rkR,
    const TRVector<VSIZE,ISIZE>& rkV);

#include "Wm3TRVector.inl"

}

#endif
