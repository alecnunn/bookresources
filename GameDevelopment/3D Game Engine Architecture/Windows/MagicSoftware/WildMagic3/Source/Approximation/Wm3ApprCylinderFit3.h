// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3APPRCYLINDERFIT3_H
#define WM3APPRCYLINDERFIT3_H

// Least-squares fit of a finite cylinder to (x,y,z) data.  The cylinder has
// center C, unit-length axis direction U, radius r, and height h.  The end
// disks of the cylinder are at C+(h/2)*U and C-(h/2)*U.  A point X is on the
// cylinder wall if (X-C)^T*(I-U*U^T)*(X-C) = r^2.  A point X is on the end
// disk at C+(h/2)*U if Dot(U,X-C) = h/2 and (X-C)^T*(I-U*U^T)*(X-C) <= r^2.
// A point X is on the end disk at C-(h/2)*U if Dot(U,X-C) = -h/2 and
// (X-C)^T*(I-U*U^T)*(X-C) <= r^2.
  
// The inputs are the quantity of points and the point array.  The outputs
// are the center C, unit-length axis direction U, radius R, and height H.
// You can supply initial guesses for C and U.  In this case you need to set
// bInputsAreInitialGuess to 'true'.  Otherwise set it to 'false' and the
// function will select C and U by first fitting the data with a least-squares
// line.  The return function value is the error for the least-squares fit,
// e >= 0.  If all the points lie exactly on a cylinder, then e = 0.
//
// You can examine the error e and iterate the calls yourself.  The outputs
// C, U, R, and H can be fed back into the function call as initial guesses.
//
// Real fError0 = CylinderFit3<Real>(iQuantity,akPoint,kC,kU,fR,fH,false);
// for (i = 1; i <= iMax; i++)
// {
//     Real fError1 = CylinderFit3<Real>(iQuantity,akPoint,kC,kU,fR,fH,true);
//     if ( fError1 not changed much from fError0 )
//         break;
// }

#include "Wm3Vector3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM CylinderFit3
{
public:
    CylinderFit3 (int iQuantity, const Vector3<Real>* akPoint,
        Vector3<Real>& rkC, Vector3<Real>& rkU, Real& rfR, Real& rfH,
        bool bInputsAreInitialGuess);

    // return the error value
    operator Real ();

private:
    static Real UpdateInvRSqr (int iQuantity, const Vector3<Real>* akPoint,
        const Vector3<Real>& rkC, const Vector3<Real>& rkU, Real& rfInvRSqr);

    static Real UpdateDirection (int iQuantity, const Vector3<Real>* akPoint,
        const Vector3<Real>& rkC, Vector3<Real>& rkU, Real& rfInvRSqr);

    static Real UpdateCenter (int iQuantity, const Vector3<Real>* akPoint,
        Vector3<Real>& rkC, const Vector3<Real>& rkU, const Real& rfInvRSqr);

    Real m_fError;
};

typedef CylinderFit3<float> CylinderFit3f;
typedef CylinderFit3<double> CylinderFit3d;

}

#endif
