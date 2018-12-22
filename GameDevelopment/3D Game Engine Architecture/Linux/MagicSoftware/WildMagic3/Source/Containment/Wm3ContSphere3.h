// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3CONTSPHERE3_H
#define WM3CONTSPHERE3_H

#include "Wm3Sphere3.h"

namespace Wm3
{

// Compute the smallest axis-aligned bounding box of the points, then
// compute a sphere containing the box.  If you want only a subset of the
// input points, provide an array of Boolean values, where abValid[i] is set
// to 'true' if you want akPoint[i] to be included in the calculations.  If
// you provide a Boolean array with all 'false' values, the returned sphere
// has center (0,0,0) and radius -1.
template <class Real> WM3_ITEM
Sphere3<Real> ContSphereOfAABB (int iQuantity, const Vector3<Real>* akPoint,
    const bool* abValid = 0);

// Compute the smallest sphere whose center is the average of the input
// points.  If you want only a subset of the input points, provide an array
// of Boolean values, where abValid[i] is set to 'true' if you want akPoint[i]
// to be included in the calculations.  If you provide a Boolean array with
// all 'false' values, the returned sphere has center (0,0,0) and radius -1.
template <class Real> WM3_ITEM
Sphere3<Real> ContSphereAverage (int iQuantity, const Vector3<Real>* akPoint,
    const bool* abValid = 0);

// Test for containment of a point inside a sphere.
template <class Real> WM3_ITEM
bool InSphere (const Vector3<Real>& rkPoint, const Sphere3<Real>& rkSphere);

// Compute the smallest sphere that contains the input spheres.
template <class Real> WM3_ITEM
Sphere3<Real> MergeSpheres (const Sphere3<Real>& rkSphere0,
    const Sphere3<Real>& rkSphere1);

// Compute the minimum volume sphere containing the input set of points.  The
// algorithm randomly permutes the input points so that the construction
// occurs in 'expected' O(N) time.
template <class Real>
class WM3_ITEM MinSphere3
{
public:
    // The epsilon value is a floating-point tolerance used for various
    // computations.
    MinSphere3 (int iQuantity, const Vector3<Real>* akPoint,
        Sphere3<Real>& rkMinimal, Real fEpsilon = (Real)1.0e-03);

private:
    // indices of points that support current minimum volume sphere
    class WM3_ITEM Support
    {
    public:
        bool Contains (int iIndex, Vector3<Real>** apkPoint, Real fEpsilon)
        {
            for (int i = 0; i < Quantity; i++)
            {
                Vector3<Real> kDiff = *apkPoint[iIndex] - *apkPoint[Index[i]];
                if ( kDiff.SquaredLength() < fEpsilon )
                    return true;
            }
            return false;
        }

        int Quantity;
        int Index[4];
    };

    // test if point P is inside sphere S
    bool Contains (const Vector3<Real>& rkP, const Sphere3<Real>& rkS,
        Real& rfDistDiff);

    Sphere3<Real> ExactSphere1 (const Vector3<Real>& rkP);
    Sphere3<Real> ExactSphere2 (const Vector3<Real>& rkP0,
        const Vector3<Real>& rkP1);
    Sphere3<Real> ExactSphere3 (const Vector3<Real>& rkP0,
        const Vector3<Real>& rkP1, const Vector3<Real>& rkP2);
    Sphere3<Real> ExactSphere4 (const Vector3<Real>& rkP0,
        const Vector3<Real>& rkP1, const Vector3<Real>& rkP2,
        const Vector3<Real>& rkP3);

    Sphere3<Real> UpdateSupport1 (int i, Vector3<Real>** apkPerm,
        Support& rkSupp);
    Sphere3<Real> UpdateSupport2 (int i, Vector3<Real>** apkPerm,
        Support& rkSupp);
    Sphere3<Real> UpdateSupport3 (int i, Vector3<Real>** apkPerm,
        Support& rkSupp);
    Sphere3<Real> UpdateSupport4 (int i, Vector3<Real>** apkPerm,
        Support& rkSupp);

    typedef Sphere3<Real> (MinSphere3<Real>::*UpdateFunction)(
        int,Vector3<Real>**,Support&);

    Real m_fEpsilon, m_fOnePlusEpsilon;
    UpdateFunction m_aoUpdate[5];
};

typedef MinSphere3<float> MinSphere3f;
typedef MinSphere3<double> MinSphere3d;

}

#endif
