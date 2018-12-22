// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3Delaunay3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
Delaunay3<Real>::Delaunay3 (int iVQuantity, Vector3<Real>* akVertex,
    bool bOwner, Real fEpsilon, Real fRange, int iTSize, int iQuantityFactor)
{
    m_iVQuantity = iVQuantity;
    m_akVertex = akVertex;
    m_bOwner = bOwner;

    // output values
    m_iTQuantity = 0;
    m_aiIndex = 0;
    m_aiAdjacent = 0;

    // Make a copy of the input vertices.  These will be modified.  The
    // extra four slots are required for temporary storage.
    Vector3<Real>* akPoint = new Vector3<Real>[m_iVQuantity+4];
    memcpy(akPoint,akVertex,m_iVQuantity*sizeof(Vector3<Real>));

    // compute the axis-aligned bounding box of the vertices
    Real fXMin = akPoint[0].X();
    Real fXMax = fXMin;
    Real fYMin = akPoint[0].Y();
    Real fYMax = fYMin;
    Real fZMin = akPoint[0].Z();
    Real fZMax = fZMin;
    int i;
    for (i = 1; i < m_iVQuantity; i++)
    {
        Real fValue = akPoint[i].X();
        if ( fXMax < fValue )
            fXMax = fValue;
        if ( fXMin > fValue )
            fXMin = fValue;

        fValue = akPoint[i].Y();
        if ( fYMax < fValue )
            fYMax = fValue;
        if ( fYMin > fValue )
            fYMin = fValue;

        fValue = akPoint[i].Z();
        if ( fZMax < fValue )
            fZMax = fValue;
        if ( fZMin > fValue )
            fZMin = fValue;
    }

    Real fXRange = fXMax - fXMin;
    Real fYRange = fYMax - fYMin;
    Real fZRange = fZMax - fZMin;

    // need to scale the data later to do a correct tetrahedron count
    Real fMaxRange = fXRange;
    if ( fMaxRange < fYRange )
        fMaxRange = fYRange;
    if ( fMaxRange < fZRange )
        fMaxRange = fZRange;
    Real fMaxRangeCube = fMaxRange*fMaxRange*fMaxRange;

    // Tweak the points by very small random numbers to help avoid
    // cosphericities in the vertices.
    Real fAmplitude = ((Real)0.5)*fEpsilon*fMaxRange;
    for (i = 0; i < m_iVQuantity; i++) 
    {
        akPoint[i].X() += fAmplitude*Math<Real>::SymmetricRandom();
        akPoint[i].Y() += fAmplitude*Math<Real>::SymmetricRandom();
        akPoint[i].Z() += fAmplitude*Math<Real>::SymmetricRandom();
    }

    Real aafWork[4][3] =
    {
        { ((Real)8.0)*fRange, -fRange, -fRange },
        { -fRange, ((Real)8.0)*fRange, -fRange },
        { -fRange, -fRange, ((Real)8.0)*fRange },
        { -fRange, -fRange, -fRange }
    };

    for (i = 0; i < 4; i++)
    {
        akPoint[m_iVQuantity+i].X() = fXMin + fXRange*aafWork[i][0];
        akPoint[m_iVQuantity+i].Y() = fYMin + fYRange*aafWork[i][1];
        akPoint[m_iVQuantity+i].Z() = fZMin + fZRange*aafWork[i][2];
    }

    int i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, i11, aiII[3];
    Real fTmp;

    int iSixTSize = 6*iTSize;
    int** aaiTmp = new int*[iSixTSize+1];
    aaiTmp[0] = new int[3*(iSixTSize+1)];
    for (i0 = 1; i0 < iSixTSize+1; i0++)
        aaiTmp[i0] = aaiTmp[0] + 3*i0;

    // Estimate of how many tetrahedrons there can be.  Since theoretically
    // the number is O(N^2), this could be quite large.  You may need to
    // increase the quantity factor if a call to this function fails.
    i1 = iQuantityFactor*m_iVQuantity;

    int* aaID = new int[i1];
    for (i0 = 0; i0 < i1; i0++)
        aaID[i0] = i0;

    int** aaiA3S = new int*[i1];
    aaiA3S[0] = new int[4*i1];
    for (i0 = 1; i0 < i1; i0++)
        aaiA3S[i0] = aaiA3S[0] + 4*i0;
    aaiA3S[0][0] = m_iVQuantity;
    aaiA3S[0][1] = m_iVQuantity+1;
    aaiA3S[0][2] = m_iVQuantity+2;
    aaiA3S[0][3] = m_iVQuantity+3;

    // circumscribed centers and radii
    Real** aafCCR = new Real*[i1];
    aafCCR[0] = new Real[4*i1];
    for (i0 = 1; i0 < i1; i0++)
        aafCCR[i0] = aafCCR[0] + 4*i0;
    aafCCR[0][0] = (Real)0.0;
    aafCCR[0][1] = (Real)0.0;
    aafCCR[0][2] = (Real)0.0;
    aafCCR[0][3] = Math<Real>::MAX_REAL;

    int iTQuantity = 1;  // number of tetrahedra
    i4 = 1;

    // compute tetrahedralization
    for (i0 = 0; i0 < m_iVQuantity; i0++)
    {  
        i1 = i7 = -1;
        i9 = 0;
        for (i11 = 0; i11 < iTQuantity; i11++)
        {  
            i1++;
            while ( aaiA3S[i1][0] < 0 ) 
                i1++;
            fTmp = aafCCR[i1][3];
            for (i2 = 0; i2 < 3; i2++)
            {
                Real fZ = akPoint[i0][i2] - aafCCR[i1][i2];
                fTmp -= fZ*fZ;
                if ( fTmp < (Real)0.0 ) 
                    goto Corner3;
            }
            i9--;
            i4--;
            aaID[i4] = i1;
            for (i2 = 0; i2 < 4; i2++)
            {
                aiII[0] = 0;
                if ( aiII[0] == i2 ) 
                    aiII[0]++;
                for (i3 = 1; i3 < 3; i3++)
                {
                    aiII[i3] = aiII[i3-1] + 1;
                    if ( aiII[i3] == i2 ) 
                        aiII[i3]++;
                }
                if ( i7 > 2 )
                {
                    i8 = i7;
                    for (i3 = 0; i3 <= i8; i3++)
                    {
                        for (i5 = 0; i5 < 3; i5++)
                        {
                            if ( aaiA3S[i1][aiII[i5]] != aaiTmp[i3][i5] ) 
                                goto Corner1;
                        }
                        for (i6 = 0; i6 < 3; i6++)
                            aaiTmp[i3][i6] = aaiTmp[i8][i6];
                        i7--;
                        goto Corner2;
Corner1:;
                    }
                }
                if ( ++i7 > iSixTSize )
                {
                    // Temporary storage exceeded.  Increase iTSize and
                    // call the constructor again.
                    assert( false );
                    goto ExitDelaunay;
                }
                for (i3 = 0; i3 < 3; i3++) 
                    aaiTmp[i7][i3] = aaiA3S[i1][aiII[i3]];
Corner2:;
            }
            aaiA3S[i1][0] = -1;
Corner3:;
        }

        for (i1 = 0; i1 <= i7; i1++)
        {
            for (i2 = 0; i2 < 3; i2++)
                for (aafWork[3][i2] = 0, i3 = 0; i3 < 3; i3++)
                {
                    aafWork[i3][i2] = akPoint[aaiTmp[i1][i2]][i3] -
                        akPoint[i0][i3];
                    aafWork[3][i2] += ((Real)0.5)*aafWork[i3][i2]*(
                        akPoint[aaiTmp[i1][i2]][i3] + akPoint[i0][i3]);
                }

            fTmp = (aafWork[0][0]*(aafWork[1][1]*aafWork[2][2] -
                aafWork[1][2]*aafWork[2][1])) - (aafWork[1][0]*(
                aafWork[0][1]*aafWork[2][2] - aafWork[0][2]*aafWork[2][1])) +
                (aafWork[2][0]*(aafWork[0][1]*aafWork[1][2] - aafWork[0][2]*
                aafWork[1][1]));
            assert( fTmp != (Real)0.0 );
            fTmp = ((Real)1.0)/fTmp;
            aafCCR[aaID[i4]][0] = ((aafWork[3][0]*(aafWork[1][1]*
                aafWork[2][2] - aafWork[1][2]*aafWork[2][1])) -
                (aafWork[1][0]*(aafWork[3][1]*aafWork[2][2] - aafWork[3][2]*
                aafWork[2][1])) + (aafWork[2][0]*(aafWork[3][1]*
                aafWork[1][2] - aafWork[3][2]*aafWork[1][1])))*fTmp;
            aafCCR[aaID[i4]][1] = ((aafWork[0][0]*(aafWork[3][1]*
                aafWork[2][2] - aafWork[3][2]*aafWork[2][1])) -
                (aafWork[3][0]*(aafWork[0][1]*aafWork[2][2] - aafWork[0][2]*
                aafWork[2][1])) + (aafWork[2][0]*(aafWork[0][1]*
                aafWork[3][2] - aafWork[0][2]*aafWork[3][1])))*fTmp;
            aafCCR[aaID[i4]][2] = ((aafWork[0][0]*(aafWork[1][1]*
                aafWork[3][2] - aafWork[1][2]*aafWork[3][1])) -
                (aafWork[1][0]*(aafWork[0][1]*aafWork[3][2] - aafWork[0][2]*
                aafWork[3][1])) + (aafWork[3][0]*(aafWork[0][1]*
                aafWork[1][2] - aafWork[0][2]*aafWork[1][1])))*fTmp;

            for (aafCCR[aaID[i4]][3] = 0, i2 = 0; i2 < 3; i2++) 
            {
                Real fZ = akPoint[i0][i2] - aafCCR[aaID[i4]][i2];
                aafCCR[aaID[i4]][3] += fZ*fZ;
                aaiA3S[aaID[i4]][i2] = aaiTmp[i1][i2];
            }

            aaiA3S[aaID[i4]][3] = i0;
            i4++;
            i9++;
        }
        iTQuantity += i9;
    }

    // count the number of tetrahedra
    m_iTQuantity = 0;
    i0 = -1;
    for (i11 = 0; i11 < iTQuantity; i11++)
    {
        i0++;
        while ( aaiA3S[i0][0] < 0 )
            i0++;
        if ( aaiA3S[i0][0] < m_iVQuantity )
        {
            for (i1 = 0; i1 < 3; i1++)
            {
                for (i2 = 0; i2 < 3; i2++) 
                {
                    aafWork[i2][i1] = akPoint[aaiA3S[i0][i1]][i2] -
                        akPoint[aaiA3S[i0][3]][i2];
                }
            }

            fTmp = ((aafWork[0][0]*(aafWork[1][1]*aafWork[2][2] -
                aafWork[1][2]*aafWork[2][1])) - (aafWork[1][0]*(
                aafWork[0][1]*aafWork[2][2] - aafWork[0][2]*aafWork[2][1])) +
                (aafWork[2][0]*(aafWork[0][1]*aafWork[1][2] - aafWork[0][2]*
                aafWork[1][1])));

            if ( Math<Real>::FAbs(fTmp) > fEpsilon*fMaxRangeCube )
                m_iTQuantity++;
        }
    }

    // create the tetrahedra
    m_aiIndex = new int[4*m_iTQuantity];
    m_iTQuantity = 0;
    i0 = -1;
    for (i11 = 0; i11 < iTQuantity; i11++)
    {
        i0++;
        while ( aaiA3S[i0][0] < 0 )
            i0++;
        if ( aaiA3S[i0][0] < m_iVQuantity )
        {
            for (i1 = 0; i1 < 3; i1++)
            {
                for (i2 = 0; i2 < 3; i2++) 
                {
                    aafWork[i2][i1] = akPoint[aaiA3S[i0][i1]][i2] -
                        akPoint[aaiA3S[i0][3]][i2];
                }
            }

            fTmp = ((aafWork[0][0]*(aafWork[1][1]*aafWork[2][2] -
                aafWork[1][2]*aafWork[2][1])) - (aafWork[1][0]*(
                aafWork[0][1]*aafWork[2][2] - aafWork[0][2]*aafWork[2][1])) +
                (aafWork[2][0]*(aafWork[0][1]*aafWork[1][2]-aafWork[0][2]*
                aafWork[1][1])));

            if ( Math<Real>::FAbs(fTmp) > fEpsilon*fMaxRangeCube )
            {
                int iDelta = (fTmp < (Real)0.0 ? 1 : 0);
                int* aiTetra = &m_aiIndex[4*m_iTQuantity];
                aiTetra[0] = aaiA3S[i0][0];
                aiTetra[1] = aaiA3S[i0][1];
                aiTetra[2] = aaiA3S[i0][2+iDelta];
                aiTetra[3] = aaiA3S[i0][3-iDelta];
                m_iTQuantity++;
            }
        }
    }

ExitDelaunay:
    delete[] aaiTmp[0];
    delete[] aaiTmp;
    delete[] aaID;
    delete[] aaiA3S[0];
    delete[] aaiA3S;
    delete[] aafCCR[0];
    delete[] aafCCR;
    delete[] akPoint;

    // create the adjacency mesh
    ComputeAdjacencies();
}
//----------------------------------------------------------------------------
template <class Real>
Delaunay3<Real>::~Delaunay3 ()
{
    delete[] m_aiIndex;
    delete[] m_aiAdjacent;

    if ( m_bOwner )
        delete[] m_akVertex;
}
//----------------------------------------------------------------------------
template <class Real>
int Delaunay3<Real>::GetVertexQuantity () const
{
    return m_iVQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>* Delaunay3<Real>::GetVertices () const
{
    return m_akVertex;
}
//----------------------------------------------------------------------------
template <class Real>
int Delaunay3<Real>::GetTetrahedronQuantity () const
{
    return m_iTQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const int* Delaunay3<Real>::GetIndices () const
{
    return m_aiIndex;
}
//----------------------------------------------------------------------------
template <class Real>
const int* Delaunay3<Real>::GetAdjacencies () const
{
    return m_aiAdjacent;
}
//----------------------------------------------------------------------------
template <class Real>
void Delaunay3<Real>::ComputeAdjacencies ()
{
    const int aiT[4][3] =
    {
        { 0, 1, 2 },
        { 0, 1, 3 },
        { 0, 2, 3 },
        { 1, 2, 3 }
    };

    TMap kMap;
    TriangleKey kKey;
    TMapIterator pkIter;
    ITuple2 kPair;
    int i, j;

    m_aiAdjacent = new int[4*m_iTQuantity];

    for (i = 0; i < m_iTQuantity; i++)
    {
        // values of -1 indicate nothing adjacent to a face
        for (j = 0; j < 4; j++)
            m_aiAdjacent[4*i+j] = -1;

        const int* aiV = &m_aiIndex[4*i];
        for (j = 0; j < 4; j++)
        {
            // check for triangle with specified ordering
            kKey = TriangleKey(aiV[aiT[j][0]],aiV[aiT[j][1]],aiV[aiT[j][2]]);
            pkIter = kMap.find(kKey);
            if ( pkIter != kMap.end() )
            {
                // must be a manifold mesh (3 or more tetra cannot share face)
                assert( pkIter->second.second == -1 );
                pkIter->second.second = i;
                continue;
            }

            // check for triangle with opposite ordering
            kKey = TriangleKey(aiV[aiT[j][0]],aiV[aiT[j][2]],aiV[aiT[j][1]]);
            pkIter = kMap.find(kKey);
            if ( pkIter != kMap.end() )
            {
                // must be a manifold mesh (3 or more tetra cannot share face)
                assert( pkIter->second.second == -1 );
                pkIter->second.second = i;
                continue;
            }

            // Key does not exist, add it and the tetrahedron index to the
            // map.
            kPair.first = i;
            kPair.second = -1;
            kMap.insert(make_pair(kKey,kPair));
        }
    }

    for (pkIter = kMap.begin(); pkIter != kMap.end(); pkIter++)
    {
        TriangleKey kKey = pkIter->first;
        int iT0 = pkIter->second.first, iT1 = pkIter->second.second;
        if ( iT0 >= 0 && iT1 >= 0 )
        {
            const int* aiV0 = &m_aiIndex[4*iT0];
            for (j = 0; j < 4; j++)
            {
                if ( aiV0[j] != kKey.V[0] && aiV0[j] != kKey.V[1]
                &&   aiV0[j] != kKey.V[2] )
                {
                    m_aiAdjacent[4*iT0+j] = iT1;
                    break;
                }
            }

            const int* aiV1 = &m_aiIndex[4*iT1];
            for (j = 0; j < 4; j++)
            {
                if ( aiV1[j] != kKey.V[0] && aiV1[j] != kKey.V[1]
                &&   aiV1[j] != kKey.V[2] )
                {
                    m_aiAdjacent[4*iT1+j] = iT0;
                    break;
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
void Delaunay3<Real>::GetHull (int& riTQuantity, int*& raiIndex) const
{
    riTQuantity = 0;
    raiIndex = 0;

    int i, iAdjQuantity = 4*m_iTQuantity;
    for (i = 0; i < iAdjQuantity; i++)
    {
        if ( m_aiAdjacent[i] == -1 )
            riTQuantity++;
    }
    assert( riTQuantity > 0 );
    if ( riTQuantity == 0 )
        return;

    raiIndex = new int[3*riTQuantity];
    int* piIndex = raiIndex;

    for (i = 0; i < iAdjQuantity; i++)
    {
        if ( m_aiAdjacent[i] == -1 )
        {
            int iTetra = i/4, iFace = i%4;
            for (int j = 0; j < 4; j++)
            {
                if ( j != iFace )
                    *piIndex++ = m_aiIndex[4*iTetra+j];
            }
            if ( (iFace % 2) == 0 )
            {
                int iSave = *(piIndex-1);
                *(piIndex-1) = *(piIndex-2);
                *(piIndex-2) = iSave;
            }
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
int Delaunay3<Real>::GetContainingTetrahedron (const Vector3<Real>& rkP) const
{
    // start at first tetrahedron in mesh
    int iIndex = 0;

    // use tetrahedron faces as binary separating planes
    for (int i = 0; i < m_iTQuantity; i++)
    {
        Vector3<Real> kV0 = m_akVertex[m_aiIndex[4*iIndex]  ];
        Vector3<Real> kV1 = m_akVertex[m_aiIndex[4*iIndex+1]];
        Vector3<Real> kV2 = m_akVertex[m_aiIndex[4*iIndex+2]];
        Vector3<Real> kV3 = m_akVertex[m_aiIndex[4*iIndex+3]];

        // <V0,V1,V2> counterclockwise when viewed outside tetrahedron
        if ( PointOutsideFace(rkP,kV0,kV1,kV2) )
        {
            iIndex = m_aiAdjacent[4*iIndex+3];
            if ( iIndex == -1 )
                return -1;
            continue;
        }

        // <V0,V3,V1> counterclockwise when viewed outside tetrahedron
        if ( PointOutsideFace(rkP,kV0,kV3,kV1) )
        {
            iIndex = m_aiAdjacent[4*iIndex+2];
            if ( iIndex == -1 )
                return -1;
            continue;
        }

        // <V0,V2,V3> counterclockwise when viewed outside tetrahedron
        if ( PointOutsideFace(rkP,kV0,kV2,kV3) )
        {
            iIndex = m_aiAdjacent[4*iIndex+1];
            if ( iIndex == -1 )
                return -1;
            continue;
        }

        // <V1,V3,V2> counterclockwise when viewed outside tetrahedron
        if ( PointOutsideFace(rkP,kV1,kV3,kV2) )
        {
            iIndex = m_aiAdjacent[4*iIndex];
            if ( iIndex == -1 )
                return -1;
            continue;
        }

        return iIndex;
    }

    return -1;
}
//----------------------------------------------------------------------------
template <class Real>
bool Delaunay3<Real>::GetVertices (int i, Vector3<Real>& rkV0,
    Vector3<Real>& rkV1, Vector3<Real>& rkV2, Vector3<Real>& rkV3) const
{
    if ( 0 <= i && i < m_iTQuantity )
    {
        rkV0 = m_akVertex[m_aiIndex[4*i  ]];
        rkV1 = m_akVertex[m_aiIndex[4*i+1]];
        rkV2 = m_akVertex[m_aiIndex[4*i+2]];
        rkV3 = m_akVertex[m_aiIndex[4*i+3]];
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <class Real>
bool Delaunay3<Real>::GetIndices (int i, int aiIndex[4]) const
{
    if ( 0 <= i && i < m_iTQuantity )
    {
        aiIndex[0] = m_aiIndex[4*i  ];
        aiIndex[1] = m_aiIndex[4*i+1];
        aiIndex[2] = m_aiIndex[4*i+2];
        aiIndex[3] = m_aiIndex[4*i+3];
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <class Real>
bool Delaunay3<Real>::GetAdjacencies (int i, int aiAdjacent[4]) const
{
    if ( 0 <= i && i < m_iTQuantity )
    {
        aiAdjacent[0] = m_aiAdjacent[4*i  ];
        aiAdjacent[1] = m_aiAdjacent[4*i+1];
        aiAdjacent[2] = m_aiAdjacent[4*i+2];
        aiAdjacent[3] = m_aiAdjacent[4*i+3];
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <class Real>
bool Delaunay3<Real>::GetBarycentrics (int i, const Vector3<Real>& rkP,
    Real afBary[4]) const
{
    if ( 0 <= i && i < m_iTQuantity )
    {
        Vector3<Real> kV0 = m_akVertex[m_aiIndex[4*i  ]];
        Vector3<Real> kV1 = m_akVertex[m_aiIndex[4*i+1]];
        Vector3<Real> kV2 = m_akVertex[m_aiIndex[4*i+2]];
        Vector3<Real> kV3 = m_akVertex[m_aiIndex[4*i+3]];
        rkP.GetBarycentrics(kV0,kV1,kV2,kV3,afBary);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <class Real>
bool Delaunay3<Real>::PointOutsideFace (const Vector3<Real>& rkP,
    const Vector3<Real>& rkV0, const Vector3<Real>& rkV1,
    const Vector3<Real>& rkV2)
{
    // <V0,V1,V2> is counterclockwise when viewed from outside

    Vector3<Real> kPDiff, kVDiff1, kVDiff2;
    Real fTSP;

    if ( rkV0 <= rkV1 )
    {
        if ( rkV1 <= rkV2 )
        {
            // <V0,V1,V2> counterclockwise
            kPDiff = rkP - rkV0;
            kVDiff1 = rkV1 - rkV0;
            kVDiff2 = rkV2 - rkV0;
            fTSP = kPDiff.Dot(kVDiff1.Cross(kVDiff2));
            return fTSP > (Real)0.0;
        }
        else if ( rkV0 <= rkV2 )
        {
            // <V0,V2,V1> clockwise
            kPDiff = rkP - rkV0;
            kVDiff1 = rkV2 - rkV0;
            kVDiff2 = rkV1 - rkV0;
            fTSP = kPDiff.Dot(kVDiff1.Cross(kVDiff2));
            return fTSP < (Real)0.0;
        }
        else
        {
            // <V2,V0,V1> counterclockwise
            kPDiff = rkP - rkV2;
            kVDiff1 = rkV0 - rkV2;
            kVDiff2 = rkV1 - rkV2;
            fTSP = kPDiff.Dot(kVDiff1.Cross(kVDiff2));
            return fTSP > (Real)0.0;
        }
    }
    else
    {
        if ( rkV0 <= rkV2 )
        {
            // <V1,V0,V2> clockwise
            kPDiff = rkP - rkV1;
            kVDiff1 = rkV0 - rkV1;
            kVDiff2 = rkV2 - rkV1;
            fTSP = kPDiff.Dot(kVDiff1.Cross(kVDiff2));
            return fTSP < (Real)0.0;
        }
        else if ( rkV1 <= rkV2 )
        {
            // <V1,V2,V0> counterclockwise
            kPDiff = rkP - rkV1;
            kVDiff1 = rkV2 - rkV1;
            kVDiff2 = rkV0 - rkV1;
            fTSP = kPDiff.Dot(kVDiff1.Cross(kVDiff2));
            return fTSP > (Real)0.0;
        }
        else
        {
            // <V2,V1,V0> clockwise
            kPDiff = rkP - rkV2;
            kVDiff1 = rkV1 - rkV2;
            kVDiff2 = rkV0 - rkV2;
            fTSP = kPDiff.Dot(kVDiff1.Cross(kVDiff2));
            return fTSP < (Real)0.0;
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class Delaunay3<float>;

// double
template WM3_ITEM
class Delaunay3<double>;
//----------------------------------------------------------------------------
}
