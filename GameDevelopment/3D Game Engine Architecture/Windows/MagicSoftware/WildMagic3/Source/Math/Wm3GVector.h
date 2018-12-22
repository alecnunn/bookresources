// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3GVECTOR_H
#define WM3GVECTOR_H

#include "Wm3Math.h"

namespace Wm3
{

template <class Real>
class GVector
{
public:
    // construction
    GVector (int iSize = 0);
    GVector (int iSize, const Real* afTuple);
    GVector (const GVector& rkV);
    ~GVector ();

    // coordinate access
    void SetSize (int iSize);
    int GetSize () const;
    operator const Real* () const;
    operator Real* ();
    Real operator[] (int i) const;
    Real& operator[] (int i);

    // assignment
    GVector& operator= (const GVector& rkV);

    // comparison
    bool operator== (const GVector& rkV) const;
    bool operator!= (const GVector& rkV) const;
    bool operator<  (const GVector& rkV) const;
    bool operator<= (const GVector& rkV) const;
    bool operator>  (const GVector& rkV) const;
    bool operator>= (const GVector& rkV) const;

    // arithmetic operations
    GVector operator+ (const GVector& rkV) const;
    GVector operator- (const GVector& rkV) const;
    GVector operator* (Real fScalar) const;
    GVector operator/ (Real fScalar) const;
    GVector operator- () const;

    // arithmetic updates
    GVector& operator+= (const GVector& rkV);
    GVector& operator-= (const GVector& rkV);
    GVector& operator*= (Real fScalar);
    GVector& operator/= (Real fScalar);

    // vector operations
    Real Length () const;
    Real SquaredLength () const;
    Real Dot (const GVector& rkV) const;
    Real Normalize ();

protected:
    // support for comparisons
    int CompareArrays (const GVector& rkV) const;

    int m_iSize;
    Real* m_afTuple;
};

template <class Real>
GVector<Real> operator* (Real fScalar, const GVector<Real>& rkV);

#include "Wm3GVector.inl"

typedef GVector<float> GVectorf;
typedef GVector<double> GVectord;

}

#endif
