// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef TRIANGULATEINT_H
#define TRIANGULATEINT_H

#include "Wm3System.h"

class TriangulateInt
{
public:
    // The polygon has iQuantity vertices which are stored in the array
    // aaiPosition[iQuantity][2].  The triangulation has iQuantity-2
    // triangles, each triangle stored as a triple of indices, so the output
    // array is aiTriangles[3*(iQuantity-2)].  Triangle i has vertices
    // aaiPosition[aiTriangles[3*i]], aaiPosition[aiTriangles[3*i+1]], and
    // aaiPosition[aiTriangles[3*i+2]].
    TriangulateInt (int iQuantity, int** aaiPosition, int* aiTriangles);
    ~TriangulateInt ();

private:
    bool IsConvex (int i);
    bool IsEar (int i);

    void InsertAfterC (int i);   // insert convex vertex
    void InsertAfterR (int i);   // insert reflex vertesx
    void InsertEndE (int i);     // insert ear at end of list
    void InsertAfterE (int i);   // insert ear after efirst
    void InsertBeforeE (int i);  // insert ear before efirst
    void RemoveV (int i);        // remove vertex
    void RemoveE ();             // remove ear at efirst
    void RemoveR (int i);        // remove reflex vertex

    // test if P is strictly to the left of the edge <V0,V1>
    bool IsLeftOf (const int aiP[2], const int aiV0[2], const int aiV1[2])
        const;

    // test is if P is in the triangle <V0,V1,V2>
    bool IsInTriangle (const int aiP[2], const int aiV0[2], const int aiV1[2],
        const int aiV2[2]) const;

    class Vertex
    {
    public:
        Vertex ()
        {
            m_bIsConvex = false;
            m_bIsEar = false;
            m_iVPrev = -1;
            m_iVNext = -1;
            m_iSPrev = -1;
            m_iSNext = -1;
            m_iEPrev = -1;
            m_iENext = -1;
        }

        bool m_bIsConvex, m_bIsEar;
        int m_iVPrev, m_iVNext; // vertex links for polygon
        int m_iSPrev, m_iSNext; // convex/reflex vertex links (disjoint lists)
        int m_iEPrev, m_iENext; // ear links
    };

    Vertex& V (int i);

    int m_iQuantity;  // for debugging
    int** m_aaiPosition;
    Vertex* m_akVertex;
    int m_iCFirst, m_iCLast;  // linear list of convex vertices
    int m_iRFirst, m_iRLast;  // linear list of reflex vertices
    int m_iEFirst, m_iELast;  // cyclical list of ears
};

#endif
