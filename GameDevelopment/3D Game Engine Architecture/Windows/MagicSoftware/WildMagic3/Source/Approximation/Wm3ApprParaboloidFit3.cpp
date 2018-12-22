// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3ApprParaboloidFit3.h"
#include "Wm3LinearSystem.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
bool ParaboloidFit3 (int iQuantity, const Vector3<Real>* akPoint,
    Real afCoeff[6], const bool* abValid)
{
    // allocate linear system (matrix is zeroed initially)
    GMatrix<Real> kMat(6,6);
    Real afRHS[6];
    memset(afRHS,0,6*sizeof(Real));

    Real fX2, fXY, fY2, fZX, fZY, fX3, fX2Y, fXY2, fY3, fZX2, fZXY, fZY2, fX4;
    Real fX3Y, fX2Y2, fXY3, fY4;
    int i;

    if ( !abValid )
    {
        for (i = 0; i < iQuantity; i++)
        {
            fX2 = akPoint[i].X()*akPoint[i].X();
            fXY = akPoint[i].X()*akPoint[i].Y();
            fY2 = akPoint[i].Y()*akPoint[i].Y();
            fZX = akPoint[i].Z()*akPoint[i].X();
            fZY = akPoint[i].Z()*akPoint[i].Y();
            fX3 = akPoint[i].X()*fX2;
            fX2Y = fX2*akPoint[i].Y();
            fXY2 = akPoint[i].X()*fY2;
            fY3 = akPoint[i].Y()*fY2;
            fZX2 = akPoint[i].Z()*fX2;
            fZXY = akPoint[i].Z()*fXY;
            fZY2 = akPoint[i].Z()*fY2;
            fX4 = fX2*fX2;
            fX3Y = fX3*akPoint[i].Y();
            fX2Y2 = fX2*fY2;
            fXY3 = akPoint[i].X()*fY3;
            fY4 = fY2*fY2;

            kMat[0][0] += fX4;
            kMat[0][1] += fX3Y;
            kMat[0][2] += fX2Y2;
            kMat[0][3] += fX3;
            kMat[0][4] += fX2Y;
            kMat[0][5] += fX2;
            kMat[1][2] += fXY3;
            kMat[1][4] += fXY2;
            kMat[1][5] += fXY;
            kMat[2][2] += fY4;
            kMat[2][4] += fY3;
            kMat[2][5] += fY2;
            kMat[3][3] += fX2;
            kMat[3][5] += akPoint[i].X();
            kMat[4][5] += akPoint[i].Y();

            afRHS[0] += fZX2;
            afRHS[1] += fZXY;
            afRHS[2] += fZY2;
            afRHS[3] += fZX;
            afRHS[4] += fZY;
            afRHS[5] += akPoint[i].Z();
        }
    }
    else
    {
        int iValidQuantity = 0;
        for (i = 0; i < iQuantity; i++)
        {
            if ( abValid[i] )
            {
                fX2 = akPoint[i].X()*akPoint[i].X();
                fXY = akPoint[i].X()*akPoint[i].Y();
                fY2 = akPoint[i].Y()*akPoint[i].Y();
                fZX = akPoint[i].Z()*akPoint[i].X();
                fZY = akPoint[i].Z()*akPoint[i].Y();
                fX3 = akPoint[i].X()*fX2;
                fX2Y = fX2*akPoint[i].Y();
                fXY2 = akPoint[i].X()*fY2;
                fY3 = akPoint[i].Y()*fY2;
                fZX2 = akPoint[i].Z()*fX2;
                fZXY = akPoint[i].Z()*fXY;
                fZY2 = akPoint[i].Z()*fY2;
                fX4 = fX2*fX2;
                fX3Y = fX3*akPoint[i].Y();
                fX2Y2 = fX2*fY2;
                fXY3 = akPoint[i].X()*fY3;
                fY4 = fY2*fY2;

                kMat[0][0] += fX4;
                kMat[0][1] += fX3Y;
                kMat[0][2] += fX2Y2;
                kMat[0][3] += fX3;
                kMat[0][4] += fX2Y;
                kMat[0][5] += fX2;
                kMat[1][2] += fXY3;
                kMat[1][4] += fXY2;
                kMat[1][5] += fXY;
                kMat[2][2] += fY4;
                kMat[2][4] += fY3;
                kMat[2][5] += fY2;
                kMat[3][3] += fX2;
                kMat[3][5] += akPoint[i].X();
                kMat[4][5] += akPoint[i].Y();

                afRHS[0] += fZX2;
                afRHS[1] += fZXY;
                afRHS[2] += fZY2;
                afRHS[3] += fZX;
                afRHS[4] += fZY;
                afRHS[5] += akPoint[i].Z();

                iValidQuantity++;
            }
        }
        if ( iValidQuantity == 0 )
        {
            for (i = 0; i < 6; i++)
                afCoeff[i] = Math<Real>::MAX_REAL;

            return false;
        }
        iQuantity = iValidQuantity;
    }

    kMat[1][0] = kMat[0][1];
    kMat[1][1] = kMat[0][2];
    kMat[1][3] = kMat[0][4];
    kMat[2][0] = kMat[0][2];
    kMat[2][1] = kMat[1][2];
    kMat[2][3] = kMat[1][4];
    kMat[3][0] = kMat[0][3];
    kMat[3][1] = kMat[1][3];
    kMat[3][2] = kMat[2][3];
    kMat[3][4] = kMat[1][5];
    kMat[4][0] = kMat[0][4];
    kMat[4][1] = kMat[1][4];
    kMat[4][2] = kMat[2][4];
    kMat[4][3] = kMat[3][4];
    kMat[4][4] = kMat[2][5];
    kMat[5][0] = kMat[0][5];
    kMat[5][1] = kMat[1][5];
    kMat[5][2] = kMat[2][5];
    kMat[5][3] = kMat[3][5];
    kMat[5][4] = kMat[4][5];
    kMat[5][5] = (Real)iQuantity;

    return LinearSystem<Real>().SolveSymmetric(kMat,afRHS,afCoeff);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
bool ParaboloidFit3<float> (int, const Vector3<float>*, float[6],
    const bool*);

// double
template WM3_ITEM
bool ParaboloidFit3<double> (int, const Vector3<double>*, double[6],
    const bool*);
//----------------------------------------------------------------------------
}
