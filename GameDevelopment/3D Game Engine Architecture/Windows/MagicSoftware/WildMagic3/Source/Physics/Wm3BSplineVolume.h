// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3BSPLINEVOLUME_H
#define WM3BSPLINEVOLUME_H

#include "Wm3BSplineBasis.h"
#include "Wm3Vector3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM BSplineVolume
{
public:
    // Construction and destruction of an open uniform B-spline volume.  The
    // class will allocate space for the control points.  The caller is
    // responsible for setting the values with the member function
    // ControlPoint.

    BSplineVolume (int iNumUCtrlPoints, int iNumVCtrlPoints,
        int iNumWCtrlPoints, int iUDegree, int iVDegree, int iWDegree);

    ~BSplineVolume ();

    int GetNumCtrlPoints (int iDim) const;
    int GetDegree (int iDim) const;

    // Control points may be changed at any time.  If any input index is
    // invalid, the returned point is a vector whose components are all
    // MAX_REAL.
    void SetControlPoint (int iUIndex, int iVIndex, int iWIndex,
        const Vector3<Real>& rkCtrlPoint);
    Vector3<Real> GetControlPoint (int iUIndex, int iVIndex, int iWIndex)
        const;

    // The spline is defined for 0 <= u <= 1, 0 <= v <= 1, and 0 <= w <= 1.
    // The input values should be in this domain.  Any inputs smaller than 0
    // are clamped to 0.  Any inputs larger than 1 are clamped to 1.
    Vector3<Real> GetPosition (Real fU, Real fV, Real fW) const;
    Vector3<Real> GetDerivativeU (Real fU, Real fV, Real fW) const;
    Vector3<Real> GetDerivativeV (Real fU, Real fV, Real fW) const;
    Vector3<Real> GetDerivativeW (Real fU, Real fV, Real fW) const;

    // for array indexing:  i = 0 for u, i = 1 for v, i = 2 for w
    Vector3<Real> GetPosition (Real afP[3]) const;
    Vector3<Real> GetDerivative (int i, Real afP[3]) const;

private:
    Vector3<Real>*** m_aaakCtrlPoint;  // ctrl[unum][vnum][wnum]
    BSplineBasis<Real> m_akBasis[3];
};

typedef BSplineVolume<float> BSplineVolumef;
typedef BSplineVolume<double> BSplineVolumed;

}

#endif
