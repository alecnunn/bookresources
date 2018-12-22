// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef BSPTREE2_H
#define BSPTREE2_H

#include "Types2.h"
#include <fstream>
class Polygon2;


class BspTree2
{
public:
    BspTree2 (Polygon2& rkPoly, const EArray& rkInput);
    ~BspTree2 ();

    BspTree2* GetCopy () const;

    // polygon Boolean operation support
    void Negate ();
    void GetPartition (const Polygon2& rkPoly, const Vector2d& rkV0,
        const Vector2d& rkV1, Polygon2& rkPos, Polygon2& rkNeg,
        Polygon2& rkCoSame, Polygon2& rkCoDiff) const;

    // point-in-polygon support (-1 outside, 0 on polygon, +1 inside)
    int PointLocation (const Polygon2& rkPoly, const Vector2d& rkV) const;

    void Indent (ofstream& rkOStr, int iQuantity);
    void Print (ofstream& rkOStr, int iLevel, char cType);

private:
    BspTree2 ()
    {
        // support for get copy
    }

    BspTree2 (const BspTree2&)
    {
        // not supported
    }

    BspTree2& operator= (const BspTree2&)
    {
        // not supported
        return *this;
    }

    enum
    {
        TRANSVERSE_POSITIVE,
        TRANSVERSE_NEGATIVE,
        ALL_POSITIVE,
        ALL_NEGATIVE,
        COINCIDENT
    };

    int Classify (const Vector2d& rkL0, const Vector2d& rkL1,
        const Vector2d& rkV0, const Vector2d& rkV1, Vector2d& rkI) const;

    void GetPosPartition (const Polygon2& rkPoly, const Vector2d& rkV0,
        const Vector2d& rkV1, Polygon2& rkPos, Polygon2& rkNeg,
        Polygon2& rkCoSame, Polygon2& rkCoDiff) const;

    void GetNegPartition (const Polygon2& rkPoly, const Vector2d& rkV0,
        const Vector2d& rkV1, Polygon2& rkPos, Polygon2& rkNeg,
        Polygon2& rkCoSame, Polygon2& rkCoDiff) const;

    class Interval
    {
    public:
        Interval (double dT0, double dT1, bool bSameDir, bool bTouching)
        {
            m_dT0 = dT0;
            m_dT1 = dT1;
            m_bSameDir = bSameDir;
            m_bTouching = bTouching;
        }

        double m_dT0, m_dT1;
        bool m_bSameDir, m_bTouching;
    };

    void GetCoPartition (const Polygon2& rkPoly, const Vector2d& rkV0,
        const Vector2d& rkV1, Polygon2& rkPos, Polygon2& rkNeg,
        Polygon2& rkCoSame, Polygon2& rkCoDiff) const;

    // point-in-polygon support
    int Classify (const Vector2d& rkL0, const Vector2d& rkL1,
        const Vector2d& rkV) const;
    int CoPointLocation (const Polygon2& rkPoly, const Vector2d& rkV) const;

    EArray m_kCoincident;
    BspTree2* m_pkPosChild;
    BspTree2* m_pkNegChild;
};

#endif
