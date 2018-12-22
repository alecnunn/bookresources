// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3StandardMesh.h"
using namespace Wm3;

//----------------------------------------------------------------------------
StandardMesh::StandardMesh ()
{
    m_akVertex = 0;
    m_aiIndex = 0;
    m_akNormal = 0;
    m_akUV = 0;
    m_bCachedVertices = false;
    m_bCachedIndices = false;
    m_bCachedNormals = false;
    m_bCachedUVs = false;
    m_bNormals = false;
    m_bUVs = false;
    m_bInside = false;
}
//----------------------------------------------------------------------------
StandardMesh::~StandardMesh ()
{
    // The native arrays are always managed by shared arrays, so no need to
    // delete them here.
}
//----------------------------------------------------------------------------
StandardMesh& StandardMesh::Transform (const Transformation& rkXFrm)
{
    m_kXFrm = rkXFrm;
    return *this;
}
//----------------------------------------------------------------------------
void StandardMesh::CreateData (int iVQuantity, int iTQuantity)
{
    // create vertices
    m_akVertex = new Vector3f[iVQuantity];
    if ( m_bCachedVertices )
        m_spkVertices = new CachedVector3fArray(iVQuantity,m_akVertex);
    else
        m_spkVertices = new Vector3fArray(iVQuantity,m_akVertex);

    // create indices
    int iIQuantity = 3*iTQuantity;
    m_aiIndex = new int[iIQuantity];
    if ( m_bCachedIndices )
        m_spkIndices = new CachedIntArray(iIQuantity,m_aiIndex);
    else
        m_spkIndices = new IntArray(iIQuantity,m_aiIndex);

    // create normals if requested
    if ( m_bNormals )
    {
        m_akNormal = new Vector3f[iVQuantity];
        if ( m_bCachedNormals )
            m_spkNormals = new CachedVector3fArray(iVQuantity,m_akNormal);
        else
            m_spkNormals = new Vector3fArray(iVQuantity,m_akNormal);
    }
    else
    {
        m_akNormal = 0;
        m_spkNormals = 0;
    }

    // create texture coordinates if requested
    if ( m_bUVs )
    {
        m_akUV = new Vector2f[iVQuantity];
        if ( m_bCachedUVs )
            m_spkUVs = new CachedVector2fArray(iVQuantity,m_akUV);
        else
            m_spkUVs = new Vector2fArray(iVQuantity,m_akUV);
    }
    else
    {
        m_akUV = 0;
        m_spkUVs = 0;
    }
}
//----------------------------------------------------------------------------
void StandardMesh::CreatePlatonicNormals (int iVQuantity)
{
    if ( m_bNormals )
    {
        for (int i = 0; i < iVQuantity; i++)
            m_akNormal[i] = m_akVertex[i];
    }
}
//----------------------------------------------------------------------------
void StandardMesh::CreatePlatonicUVs (int iVQuantity)
{
    if ( m_bUVs )
    {
        for (int i = 0; i < iVQuantity; i++)
        {
            if ( Mathf::FAbs(m_akVertex[i].Z()) < 1.0f )
            {
                m_akUV[i].X() = 0.5f*(1.0f + Mathf::ATan2(m_akVertex[i].Y(),
                    m_akVertex[i].X())*Mathf::INV_PI);
            }
            else
            {
                m_akUV[i].X() = 0.5f;
            }
            m_akUV[i].Y() = Mathf::ACos(m_akVertex[i].Z())*Mathf::INV_PI;
        }
    }
}
//----------------------------------------------------------------------------
void StandardMesh::ReverseTriangleOrder (int iTQuantity)
{
    for (int i = 0; i < iTQuantity; i++)
    {
        int j1 = 3*i+1, j2 = j1+1;
        int iSave = m_aiIndex[j1];
        m_aiIndex[j1] = m_aiIndex[j2];
        m_aiIndex[j2] = iSave;
    }
}
//----------------------------------------------------------------------------
void StandardMesh::TransformData (int iVQuantity)
{
    if ( m_kXFrm.IsIdentity() )
        return;

    m_kXFrm.ApplyForward(iVQuantity,m_akVertex,m_akVertex);
    if ( m_akNormal )
    {
        Vector3f kSave = m_kXFrm.GetTranslate();
        m_kXFrm.ApplyForward(iVQuantity,m_akNormal,m_akNormal);
        m_kXFrm.SetTranslate(kSave);
        for (int i = 0; i < iVQuantity; i++)
            m_akNormal[i].Normalize();
    }
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Rectangle (int iXSamples, int iYSamples,
    float fXExtent, float fYExtent)
{
    int iVQuantity = iXSamples*iYSamples;
    int iTQuantity = 2*(iXSamples-1)*(iYSamples-1);
    CreateData(iVQuantity,iTQuantity);

    // generate geometry
    float fInv0 = 1.0f/(iXSamples - 1.0f);
    float fInv1 = 1.0f/(iYSamples - 1.0f);
    float fU, fV;
    int i, i0, i1;
    for (i1 = 0, i = 0; i1 < iYSamples; i1++)
    {
        fV = i1*fInv1;
        Vector3f kTmp = ((2.0f*fV-1.0f)*fYExtent)*Vector3f::UNIT_Y;
        for (i0 = 0; i0 < iXSamples; i0++)
        {
            fU = i0*fInv0;
            m_akVertex[i++] = kTmp +
                ((2.0f*fU-1.0f)*fXExtent)*Vector3f::UNIT_X;
        }
    }

    if ( m_bNormals )
    {
        for (i = 0; i < iVQuantity; i++)
            m_akNormal[i] = Vector3f::UNIT_Z;
    }

    if ( m_bUVs )
    {
        for (i1 = 0, i = 0; i1 < iYSamples; i1++)
        {
            fV = i1*fInv1;
            for (i0 = 0; i0 < iXSamples; i0++)
            {
                fU = i0*fInv0;
                m_akUV[i++] = Vector2f(fU,fV);
            }
        }
    }

    // generate connectivity
    for (i1 = 0, i = 0; i1 < iYSamples - 1; i1++)
    {
        for (i0 = 0; i0 < iXSamples - 1; i0++)
        {
            int iV0 = i0 + iXSamples * i1;
            int iV1 = iV0 + 1;
            int iV2 = iV1 + iXSamples;
            int iV3 = iV0 + iXSamples;
            m_aiIndex[i++] = iV0;
            m_aiIndex[i++] = iV1;
            m_aiIndex[i++] = iV2;
            m_aiIndex[i++] = iV0;
            m_aiIndex[i++] = iV2;
            m_aiIndex[i++] = iV3;
        }
    }

    TransformData(iVQuantity);
    TriMesh* pkMesh = new TriMesh(m_spkVertices,m_spkIndices,false);
    pkMesh->Normals = m_spkNormals;
    return pkMesh;
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Disk (int iShellSamples, int iRadialSamples,
    float fRadius)
{
    int iRSm1 = iRadialSamples - 1, iSSm1 = iShellSamples - 1;
    int iVQuantity = 1 + iRadialSamples*iSSm1;
    int iTQuantity = iRadialSamples*(2*iSSm1-1);
    CreateData(iVQuantity,iTQuantity);

    // generate geometry
    int iR, iS, i;

    // center of disk
    m_akVertex[0] = Vector3f::ZERO;
    if ( m_bNormals )
    {
        m_akNormal[0] = Vector3f::UNIT_Z;
    }
    if ( m_bUVs )
    {
        m_akUV[0].X() = 0.5f;
        m_akUV[0].Y() = 0.5f;
    }

    float fInvSSm1 = 1.0f/(float)iSSm1;
    float fInvRS = 1.0f/(float)iRadialSamples;
    for (iR = 0; iR < iRadialSamples; iR++)
    {
        float fAngle = Mathf::TWO_PI*fInvRS*iR;
        float fCos = Mathf::Cos(fAngle);
        float fSin = Mathf::Sin(fAngle);
        Vector3f kRadial(fCos,fSin,0.0f);

        for (iS = 1; iS < iShellSamples; iS++)
        {
            float fFraction = fInvSSm1*iS;  // in (0,R]
            Vector3f kFracRadial = fFraction*kRadial;
            i = iS+iSSm1*iR;
            m_akVertex[i] = fRadius*kFracRadial;
            if ( m_bNormals )
            {
                m_akNormal[i] = Vector3f::UNIT_Z;
            }
            if ( m_bUVs )
            {
                m_akUV[i].X() = 0.5f*(1.0f+kFracRadial.X());
                m_akUV[i].Y() = 0.5f*(1.0f+kFracRadial.Y());
            }
        }
    }

    // generate connectivity
    int* aiLocalIndex = m_aiIndex;
    int iT = 0;
    for (int iR0 = iRSm1, iR1 = 0; iR1 < iRadialSamples; iR0 = iR1++)
    {
        aiLocalIndex[0] = 0;
        aiLocalIndex[1] = 1+iSSm1*iR0;
        aiLocalIndex[2] = 1+iSSm1*iR1;
        aiLocalIndex += 3;
        iT++;
        for (int iS = 1; iS < iSSm1; iS++, aiLocalIndex += 6)
        {
            int i00 = iS+iSSm1*iR0;
            int i01 = iS+iSSm1*iR1;
            int i10 = i00+1;
            int i11 = i01+1;
            aiLocalIndex[0] = i00;
            aiLocalIndex[1] = i10;
            aiLocalIndex[2] = i11;
            aiLocalIndex[3] = i00;
            aiLocalIndex[4] = i11;
            aiLocalIndex[5] = i01;
            iT += 2;
        }
    }
    assert( iT == iTQuantity );

    TransformData(iVQuantity);
    TriMesh* pkMesh = new TriMesh(m_spkVertices,m_spkIndices,false);
    pkMesh->Normals = m_spkNormals;
    return pkMesh;
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Box (float fXExtent, float fYExtent, float fZExtent)
{
    int iVQuantity = 8;
    int iTQuantity = 12;
    CreateData(iVQuantity,iTQuantity);

    // generate geometry
    m_akVertex[0] = Vector3f(-fXExtent,-fYExtent,-fZExtent);
    m_akVertex[1] = Vector3f(+fXExtent,-fYExtent,-fZExtent);
    m_akVertex[2] = Vector3f(+fXExtent,+fYExtent,-fZExtent);
    m_akVertex[3] = Vector3f(-fXExtent,+fYExtent,-fZExtent);
    m_akVertex[4] = Vector3f(-fXExtent,-fYExtent,+fZExtent);
    m_akVertex[5] = Vector3f(+fXExtent,-fYExtent,+fZExtent);
    m_akVertex[6] = Vector3f(+fXExtent,+fYExtent,+fZExtent);
    m_akVertex[7] = Vector3f(-fXExtent,+fYExtent,+fZExtent);

    if ( m_bUVs )
    {
        m_akUV[0] = Vector2f(0.25f,0.75f);
        m_akUV[1] = Vector2f(0.75f,0.75f);
        m_akUV[2] = Vector2f(0.75f,0.25f);
        m_akUV[3] = Vector2f(0.25f,0.25f);
        m_akUV[4] = Vector2f(0.0f,1.0f);
        m_akUV[5] = Vector2f(1.0f,1.0f);
        m_akUV[6] = Vector2f(1.0f,0.0f);
        m_akUV[7] = Vector2f(0.0f,0.0f);
    }

    // generate connectivity (outside view)
    m_aiIndex[ 0] = 0;  m_aiIndex[ 1] = 2;  m_aiIndex[ 2] = 1;
    m_aiIndex[ 3] = 0;  m_aiIndex[ 4] = 3;  m_aiIndex[ 5] = 2;
    m_aiIndex[ 6] = 0;  m_aiIndex[ 7] = 1;  m_aiIndex[ 8] = 5;
    m_aiIndex[ 9] = 0;  m_aiIndex[10] = 5;  m_aiIndex[11] = 4;
    m_aiIndex[12] = 0;  m_aiIndex[13] = 4;  m_aiIndex[14] = 7;
    m_aiIndex[15] = 0;  m_aiIndex[16] = 7;  m_aiIndex[17] = 3;
    m_aiIndex[18] = 6;  m_aiIndex[19] = 4;  m_aiIndex[20] = 5;
    m_aiIndex[21] = 6;  m_aiIndex[22] = 7;  m_aiIndex[23] = 4;
    m_aiIndex[24] = 6;  m_aiIndex[25] = 5;  m_aiIndex[26] = 1;
    m_aiIndex[27] = 6;  m_aiIndex[28] = 1;  m_aiIndex[29] = 2;
    m_aiIndex[30] = 6;  m_aiIndex[31] = 2;  m_aiIndex[32] = 3;
    m_aiIndex[33] = 6;  m_aiIndex[34] = 3;  m_aiIndex[35] = 7;

    if ( m_bInside )
        ReverseTriangleOrder(iTQuantity);

    TransformData(iVQuantity);
    TriMesh* pkMesh = new TriMesh(m_spkVertices,m_spkIndices,false);
    pkMesh->Normals = m_spkNormals;
    return pkMesh;
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Cylinder (int iAxisSamples, int iRadialSamples,
    float fRadius, float fHeight, bool bOpen)
{
    TriMesh* pkMesh;

    if ( bOpen )
    {
        int iVQuantity = iAxisSamples*(iRadialSamples+1);
        int iTQuantity = 2*(iAxisSamples-1)*iRadialSamples;
        CreateData(iVQuantity,iTQuantity);

        // generate geometry
        float fInvRS = 1.0f/(float)iRadialSamples;
        float fInvASm1 = 1.0f/(float)(iAxisSamples-1);
        float fHalfHeight = 0.5f*fHeight;
        int iR, iA, iAStart, i;

        // Generate points on the unit circle to be used in computing the
        // mesh points on a cylinder slice.
        float* afSin = new float[iRadialSamples+1];
        float* afCos = new float[iRadialSamples+1];
        for (iR = 0; iR < iRadialSamples; iR++)
        {
            float fAngle = Mathf::TWO_PI*fInvRS*iR;
            afCos[iR] = Mathf::Cos(fAngle);
            afSin[iR] = Mathf::Sin(fAngle);
        }
        afSin[iRadialSamples] = afSin[0];
        afCos[iRadialSamples] = afCos[0];

        // generate the cylinder itself
        for (iA = 0, i = 0; iA < iAxisSamples; iA++)
        {
            float fAxisFraction = iA*fInvASm1;  // in [0,1]
            float fZ = -fHalfHeight + fHeight*fAxisFraction;

            // compute center of slice
            Vector3f kSliceCenter(0.0f,0.0f,fZ);

            // compute slice vertices with duplication at end point
            int iSave = i;
            for (iR = 0; iR < iRadialSamples; iR++)
            {
                float fRadialFraction = iR*fInvRS;  // in [0,1)
                Vector3f kNormal(afCos[iR],afSin[iR],0.0f);
                m_akVertex[i] = kSliceCenter + fRadius*kNormal;
                if ( m_bNormals )
                {
                    if ( m_bInside )
                        m_akNormal[i] = -kNormal;
                    else
                        m_akNormal[i] = kNormal;
                }
                if ( m_bUVs )
                {
                    m_akUV[i].X() = fRadialFraction;
                    m_akUV[i].Y() = fAxisFraction;
                }
                i++;
            }

            m_akVertex[i] = m_akVertex[iSave];
            if ( m_bNormals )
            {
                m_akNormal[i] = m_akNormal[iSave];
            }
            if ( m_bUVs )
            {
                m_akUV[i].X() = 1.0f;
                m_akUV[i].Y() = fAxisFraction;
            }
            i++;
        }

        // generate connectivity
        int* aiLocalIndex = m_aiIndex;
        for (iA = 0, iAStart = 0; iA < iAxisSamples-1; iA++)
        {
            int i0 = iAStart;
            int i1 = i0 + 1;
            iAStart += iRadialSamples + 1;
            int i2 = iAStart;
            int i3 = i2 + 1;
            for (i = 0; i < iRadialSamples; i++, aiLocalIndex += 6)
            {
                if ( m_bInside )
                {
                    aiLocalIndex[0] = i0++;
                    aiLocalIndex[1] = i2;
                    aiLocalIndex[2] = i1;
                    aiLocalIndex[3] = i1++;
                    aiLocalIndex[4] = i2++;
                    aiLocalIndex[5] = i3++;
                }
                else // outside view
                {
                    aiLocalIndex[0] = i0++;
                    aiLocalIndex[1] = i1;
                    aiLocalIndex[2] = i2;
                    aiLocalIndex[3] = i1++;
                    aiLocalIndex[4] = i3++;
                    aiLocalIndex[5] = i2++;
                }
            }
        }

        delete[] afCos;
        delete[] afSin;

        TransformData(iVQuantity);
        pkMesh = new TriMesh(m_spkVertices,m_spkIndices,false);
        pkMesh->Normals = m_spkNormals;
    }
    else
    {
        pkMesh = Sphere(iAxisSamples,iRadialSamples,fRadius);
        int iVQuantity = pkMesh->Vertices->GetQuantity();
        Vector3f* akVertex = pkMesh->Vertices->GetData();

        // flatten sphere at poles
        float fHDiv2 = 0.5f*fHeight;
        akVertex[iVQuantity-2].Z() = -fHDiv2;  // south pole
        akVertex[iVQuantity-1].Z() = fHDiv2;  // north pole

        // remap z-values to [-h/2,h/2]
        float fZFactor = 2.0f/(iAxisSamples-1);
        float fTmp0 = fRadius*(-1.0f + fZFactor);
        float fTmp1 = 1.0f/(fRadius*(+1.0f - fZFactor));
        for (int i = 0; i < iVQuantity-2; i++)
        {
            akVertex[i].Z() = fHDiv2*(-1.0f+fTmp1*(akVertex[i].Z()-fTmp0));
            float fX = akVertex[i].X(), fY = akVertex[i].Y();
            float fAdjust = fRadius*Mathf::InvSqrt(fX*fX + fY*fY);
            akVertex[i].X() *= fAdjust;
            akVertex[i].Y() *= fAdjust;
        }
        TransformData(iVQuantity);
        pkMesh->UpdateMS();
    }

    return pkMesh;
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Sphere (int iZSamples, int iRadialSamples,
    float fRadius)
{
    int iZSm1 = iZSamples-1, iZSm2 = iZSamples-2, iZSm3 = iZSamples-3;
    int iRSp1 = iRadialSamples+1;
    int iVQuantity = iZSm2*iRSp1 + 2;
    int iTQuantity = 2*iZSm2*iRadialSamples;
    CreateData(iVQuantity,iTQuantity);

    // generate geometry
    float fInvRS = 1.0f/(float)iRadialSamples;
    float fZFactor = 2.0f/(float)iZSm1;
    int iR, iZ, iZStart, i;

    // Generate points on the unit circle to be used in computing the mesh
    // points on a cylinder slice.
    float* afSin = new float[iRSp1];
    float* afCos = new float[iRSp1];
    for (iR = 0; iR < iRadialSamples; iR++)
    {
        float fAngle = Mathf::TWO_PI*fInvRS*iR;
        afCos[iR] = Mathf::Cos(fAngle);
        afSin[iR] = Mathf::Sin(fAngle);
    }
    afSin[iRadialSamples] = afSin[0];
    afCos[iRadialSamples] = afCos[0];

    // generate the cylinder itself
    for (iZ = 1, i = 0; iZ < iZSm1; iZ++)
    {
        float fZFraction = -1.0f + fZFactor*iZ;  // in (-1,1)
        float fZ = fRadius*fZFraction;

        // compute center of slice
        Vector3f kSliceCenter(0.0f,0.0f,fZ);

        // compute radius of slice
        float fSliceRadius = Mathf::Sqrt(Mathf::FAbs(fRadius*fRadius-fZ*fZ));

        // compute slice vertices with duplication at end point
        Vector3f kNormal;
        int iSave = i;
        for (iR = 0; iR < iRadialSamples; iR++)
        {
            float fRadialFraction = iR*fInvRS;  // in [0,1)
            Vector3f kRadial(afCos[iR],afSin[iR],0.0f);
            m_akVertex[i] = kSliceCenter + fSliceRadius*kRadial;
            if ( m_bNormals )
            {
                kNormal = m_akVertex[i];
                kNormal.Normalize();
                if ( m_bInside )
                    m_akNormal[i] = -kNormal;
                else
                    m_akNormal[i] = kNormal;
            }
            if ( m_bUVs )
            {
                m_akUV[i].X() = fRadialFraction;
                m_akUV[i].Y() = 0.5f*(fZFraction+1.0f);
            }
            i++;
        }

        m_akVertex[i] = m_akVertex[iSave];
        if ( m_bNormals )
        {
            m_akNormal[i] = m_akNormal[iSave];
        }
        if ( m_bUVs )
        {
            m_akUV[i].X() = 1.0f;
            m_akUV[i].Y() = 0.5f*(fZFraction+1.0f);
        }
        i++;
    }

    // south pole
    m_akVertex[i] = -fRadius*Vector3f::UNIT_Z;
    if ( m_bNormals )
    {
        if ( m_bInside )
            m_akNormal[i] = Vector3f::UNIT_Z;
        else
            m_akNormal[i] = -Vector3f::UNIT_Z;
    }
    if ( m_bUVs )
    {
        m_akUV[i].X() = 0.5f;
        m_akUV[i].Y() = 0.0f;
    }
    i++;

    // north pole
    m_akVertex[i] = fRadius*Vector3f::UNIT_Z;
    if ( m_bNormals )
    {
        if ( m_bInside )
            m_akNormal[i] = -Vector3f::UNIT_Z;
        else
            m_akNormal[i] = Vector3f::UNIT_Z;
    }
    if ( m_bUVs )
    {
        m_akUV[i].X() = 0.5f;
        m_akUV[i].Y() = 1.0f;
    }
    i++;
    assert( i == iVQuantity );

    // generate connectivity
    int* aiLocalIndex = m_aiIndex;
    for (iZ = 0, iZStart = 0; iZ < iZSm3; iZ++)
    {
        int i0 = iZStart;
        int i1 = i0 + 1;
        iZStart += iRSp1;
        int i2 = iZStart;
        int i3 = i2 + 1;
        for (i = 0; i < iRadialSamples; i++, aiLocalIndex += 6)
        {
            if ( m_bInside )
            {
                aiLocalIndex[0] = i0++;
                aiLocalIndex[1] = i2;
                aiLocalIndex[2] = i1;
                aiLocalIndex[3] = i1++;
                aiLocalIndex[4] = i2++;
                aiLocalIndex[5] = i3++;
            }
            else  // inside view
            {
                aiLocalIndex[0] = i0++;
                aiLocalIndex[1] = i1;
                aiLocalIndex[2] = i2;
                aiLocalIndex[3] = i1++;
                aiLocalIndex[4] = i3++;
                aiLocalIndex[5] = i2++;
            }
        }
    }

    // south pole triangles
    int iVQm2 = iVQuantity-2;
    for (i = 0; i < iRadialSamples; i++, aiLocalIndex += 3)
    {
        if ( m_bInside )
        {
            aiLocalIndex[0] = i;
            aiLocalIndex[1] = i+1;
            aiLocalIndex[2] = iVQm2;
        }
        else  // inside view
        {
            aiLocalIndex[0] = i;
            aiLocalIndex[1] = iVQm2;
            aiLocalIndex[2] = i+1;
        }
    }

    // north pole triangles
    int iVQm1 = iVQuantity-1, iOffset = iZSm3*iRSp1;
    for (i = 0; i < iRadialSamples; i++, aiLocalIndex += 3)
    {
        if ( m_bInside )
        {
            aiLocalIndex[0] = i+iOffset;
            aiLocalIndex[1] = iVQm1;
            aiLocalIndex[2] = i+1+iOffset;
        }
        else  // inside view
        {
            aiLocalIndex[0] = i+iOffset;
            aiLocalIndex[1] = i+1+iOffset;
            aiLocalIndex[2] = iVQm1;
        }
    }

    assert( aiLocalIndex == m_aiIndex + 3*iTQuantity );

    delete[] afCos;
    delete[] afSin;

    TransformData(iVQuantity);
    TriMesh* pkMesh = new TriMesh(m_spkVertices,m_spkIndices,false);
    pkMesh->Normals = m_spkNormals;
    return pkMesh;
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Torus (int iCircleSamples, int iRadialSamples,
    float fOuterRadius, float fInnerRadius)
{
    int iVQuantity = (iCircleSamples+1)*(iRadialSamples+1);
    int iTQuantity = 2*iCircleSamples*iRadialSamples;
    CreateData(iVQuantity,iTQuantity);

    // generate geometry
    float fInvCS = 1.0f/(float)iCircleSamples;
    float fInvRS = 1.0f/(float)iRadialSamples;
    int iC, iR, i;

    // generate the cylinder itself
    for (iC = 0, i = 0; iC < iCircleSamples; iC++)
    {
        // compute center point on torus circle at specified angle
        float fCircleFraction = iC*fInvCS;  // in [0,1)
        float fTheta = Mathf::TWO_PI*fCircleFraction;
        float fCosTheta = Mathf::Cos(fTheta);
        float fSinTheta = Mathf::Sin(fTheta);
        Vector3f kRadial(fCosTheta,fSinTheta,0.0f);
        Vector3f kTorusMiddle = fOuterRadius*kRadial;

        // compute slice vertices with duplication at end point
        int iSave = i;
        for (iR = 0; iR < iRadialSamples; iR++)
        {
            float fRadialFraction = iR*fInvRS;  // in [0,1)
            float fPhi = Mathf::TWO_PI*fRadialFraction;
            float fCosPhi = Mathf::Cos(fPhi);
            float fSinPhi = Mathf::Sin(fPhi);
            Vector3f kNormal = fCosPhi*kRadial + fSinPhi*Vector3f::UNIT_Z;
            m_akVertex[i] = kTorusMiddle + fInnerRadius*kNormal;
            if ( m_bNormals )
            {
                if ( m_bInside )
                    m_akNormal[i] = -kNormal;
                else
                    m_akNormal[i] = kNormal;
            }
            if ( m_bUVs )
            {
                m_akUV[i].X() = fRadialFraction;
                m_akUV[i].Y() = fCircleFraction;
            }
            i++;
        }

        m_akVertex[i] = m_akVertex[iSave];
        if ( m_bNormals )
        {
            m_akNormal[i] = m_akNormal[iSave];
        }
        if ( m_bUVs )
        {
            m_akUV[i].X() = 1.0f;
            m_akUV[i].Y() = fCircleFraction;
        }
        i++;
    }

    // duplicate the cylinder ends to form a torus
    for (iR = 0; iR <= iRadialSamples; iR++, i++)
    {
        m_akVertex[i] = m_akVertex[iR];
        if ( m_bNormals )
        {
            m_akNormal[i] = m_akNormal[iR];
        }
        if ( m_bUVs )
        {
            m_akUV[i].X() = m_akUV[iR].X();
            m_akUV[i].Y() = 1.0f;
        }
    }
    assert( i == iVQuantity );

    // generate connectivity
    int* aiLocalIndex = m_aiIndex;
    int iCStart = 0;
    for (iC = 0; iC < iCircleSamples; iC++)
    {
        int i0 = iCStart;
        int i1 = i0 + 1;
        iCStart += iRadialSamples + 1;
        int i2 = iCStart;
        int i3 = i2 + 1;
        for (i = 0; i < iRadialSamples; i++, aiLocalIndex += 6)
        {
            if ( m_bInside )
            {
                aiLocalIndex[0] = i0++;
                aiLocalIndex[1] = i1;
                aiLocalIndex[2] = i2;
                aiLocalIndex[3] = i1++;
                aiLocalIndex[4] = i3++;
                aiLocalIndex[5] = i2++;
            }
            else  // inside view
            {
                aiLocalIndex[0] = i0++;
                aiLocalIndex[1] = i2;
                aiLocalIndex[2] = i1;
                aiLocalIndex[3] = i1++;
                aiLocalIndex[4] = i2++;
                aiLocalIndex[5] = i3++;
            }
        }
    }

    TransformData(iVQuantity);
    TriMesh* pkMesh = new TriMesh(m_spkVertices,m_spkIndices,false);
    pkMesh->Normals = m_spkNormals;
    return pkMesh;
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Tetrahedron ()
{
    float fSqrt2Div3 = Mathf::Sqrt(2.0f)/3.0f;
    float fSqrt6Div3 = Mathf::Sqrt(6.0f)/3.0f;
    float fOneThird = 1.0f/3.0f;

    int iVQuantity = 4;
    int iTQuantity = 4;
    CreateData(iVQuantity,iTQuantity);

    m_akVertex[0] = Vector3f(0.0f,0.0f,1.0f);
    m_akVertex[1] = Vector3f(2.0f*fSqrt2Div3,0.0f,-fOneThird);
    m_akVertex[2] = Vector3f(-fSqrt2Div3,fSqrt6Div3,-fOneThird);
    m_akVertex[3] = Vector3f(-fSqrt2Div3,-fSqrt6Div3,-fOneThird);

    m_aiIndex[ 0] = 0;  m_aiIndex[ 1] = 1;  m_aiIndex[ 2] = 2;
    m_aiIndex[ 3] = 0;  m_aiIndex[ 4] = 2;  m_aiIndex[ 5] = 3;
    m_aiIndex[ 6] = 0;  m_aiIndex[ 7] = 3;  m_aiIndex[ 8] = 1;
    m_aiIndex[ 9] = 1;  m_aiIndex[10] = 3;  m_aiIndex[11] = 2;

    CreatePlatonicNormals(iVQuantity);
    CreatePlatonicUVs(iVQuantity);
    if ( m_bInside )
        ReverseTriangleOrder(iTQuantity);

    TransformData(iVQuantity);
    TriMesh* pkMesh = new TriMesh(m_spkVertices,m_spkIndices,false);
    pkMesh->Normals = m_spkNormals;
    return pkMesh;
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Hexahedron ()
{
    float fSqrtThird = Mathf::Sqrt(1.0f/3.0f);

    int iVQuantity = 8;
    int iTQuantity = 12;
    CreateData(iVQuantity,iTQuantity);

    m_akVertex[0] = Vector3f(-fSqrtThird,-fSqrtThird,-fSqrtThird);
    m_akVertex[1] = Vector3f( fSqrtThird,-fSqrtThird,-fSqrtThird);
    m_akVertex[2] = Vector3f( fSqrtThird, fSqrtThird,-fSqrtThird);
    m_akVertex[3] = Vector3f(-fSqrtThird, fSqrtThird,-fSqrtThird);
    m_akVertex[4] = Vector3f(-fSqrtThird,-fSqrtThird, fSqrtThird);
    m_akVertex[5] = Vector3f( fSqrtThird,-fSqrtThird, fSqrtThird);
    m_akVertex[6] = Vector3f( fSqrtThird, fSqrtThird, fSqrtThird);
    m_akVertex[7] = Vector3f(-fSqrtThird, fSqrtThird, fSqrtThird);

    m_aiIndex[ 0] = 0;  m_aiIndex[ 1] = 3;  m_aiIndex[ 2] = 2;
    m_aiIndex[ 3] = 0;  m_aiIndex[ 4] = 2;  m_aiIndex[ 5] = 1;
    m_aiIndex[ 6] = 0;  m_aiIndex[ 7] = 1;  m_aiIndex[ 8] = 5;
    m_aiIndex[ 9] = 0;  m_aiIndex[10] = 5;  m_aiIndex[11] = 4;
    m_aiIndex[12] = 0;  m_aiIndex[13] = 4;  m_aiIndex[14] = 7;
    m_aiIndex[15] = 0;  m_aiIndex[16] = 7;  m_aiIndex[17] = 3;
    m_aiIndex[18] = 6;  m_aiIndex[19] = 5;  m_aiIndex[20] = 1;
    m_aiIndex[21] = 6;  m_aiIndex[22] = 1;  m_aiIndex[23] = 2;
    m_aiIndex[24] = 6;  m_aiIndex[25] = 2;  m_aiIndex[26] = 3;
    m_aiIndex[27] = 6;  m_aiIndex[28] = 3;  m_aiIndex[29] = 7;
    m_aiIndex[30] = 6;  m_aiIndex[31] = 7;  m_aiIndex[32] = 4;
    m_aiIndex[33] = 6;  m_aiIndex[34] = 4;  m_aiIndex[35] = 5;

    CreatePlatonicNormals(iVQuantity);
    CreatePlatonicUVs(iVQuantity);
    if ( m_bInside )
        ReverseTriangleOrder(iTQuantity);

    TransformData(iVQuantity);
    TriMesh* pkMesh = new TriMesh(m_spkVertices,m_spkIndices,false);
    pkMesh->Normals = m_spkNormals;
    return pkMesh;
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Octahedron ()
{
    int iVQuantity = 6;
    int iTQuantity = 8;
    CreateData(iVQuantity,iTQuantity);

    m_akVertex[0] = Vector3f( 1.0f, 0.0f, 0.0f);
    m_akVertex[1] = Vector3f(-1.0f, 0.0f, 0.0f);
    m_akVertex[2] = Vector3f( 0.0f, 1.0f, 0.0f);
    m_akVertex[3] = Vector3f( 0.0f,-1.0f, 0.0f);
    m_akVertex[4] = Vector3f( 0.0f, 0.0f, 1.0f);
    m_akVertex[5] = Vector3f( 0.0f, 0.0f,-1.0f);

    m_aiIndex[ 0] = 4;  m_aiIndex[ 1] = 0;  m_aiIndex[ 2] = 2;
    m_aiIndex[ 3] = 4;  m_aiIndex[ 4] = 2;  m_aiIndex[ 5] = 1;
    m_aiIndex[ 6] = 4;  m_aiIndex[ 7] = 1;  m_aiIndex[ 8] = 3;
    m_aiIndex[ 9] = 4;  m_aiIndex[10] = 3;  m_aiIndex[11] = 0;
    m_aiIndex[12] = 5;  m_aiIndex[13] = 2;  m_aiIndex[14] = 0;
    m_aiIndex[15] = 5;  m_aiIndex[16] = 1;  m_aiIndex[17] = 2;
    m_aiIndex[18] = 5;  m_aiIndex[19] = 3;  m_aiIndex[20] = 1;
    m_aiIndex[21] = 5;  m_aiIndex[22] = 0;  m_aiIndex[23] = 3;

    CreatePlatonicNormals(iVQuantity);
    CreatePlatonicUVs(iVQuantity);
    if ( m_bInside )
        ReverseTriangleOrder(iTQuantity);

    TransformData(iVQuantity);
    TriMesh* pkMesh = new TriMesh(m_spkVertices,m_spkIndices,false);
    pkMesh->Normals = m_spkNormals;
    return pkMesh;
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Dodecahedron ()
{
    float fA = 1.0f/Mathf::Sqrt(3.0f);
    float fB = Mathf::Sqrt((3.0f-Mathf::Sqrt(5.0f))/6.0f);
    float fC = Mathf::Sqrt((3.0f+Mathf::Sqrt(5.0f))/6.0f);

    int iVQuantity = 20;
    int iTQuantity = 36;
    CreateData(iVQuantity,iTQuantity);

    m_akVertex[ 0] = Vector3f( fA, fA, fA);
    m_akVertex[ 1] = Vector3f( fA, fA,-fA);
    m_akVertex[ 2] = Vector3f( fA,-fA, fA);
    m_akVertex[ 3] = Vector3f( fA,-fA,-fA);
    m_akVertex[ 4] = Vector3f(-fA, fA, fA);
    m_akVertex[ 5] = Vector3f(-fA, fA,-fA);
    m_akVertex[ 6] = Vector3f(-fA,-fA, fA);
    m_akVertex[ 7] = Vector3f(-fA,-fA,-fA);
    m_akVertex[ 8] = Vector3f(  fB,  fC, 0.0f);
    m_akVertex[ 9] = Vector3f( -fB,  fC, 0.0f);
    m_akVertex[10] = Vector3f(  fB, -fC, 0.0f);
    m_akVertex[11] = Vector3f( -fB, -fC, 0.0f);
    m_akVertex[12] = Vector3f(  fC, 0.0f,  fB);
    m_akVertex[13] = Vector3f(  fC, 0.0f, -fB);
    m_akVertex[14] = Vector3f( -fC, 0.0f,  fB);
    m_akVertex[15] = Vector3f( -fC, 0.0f, -fB);
    m_akVertex[16] = Vector3f(0.0f,   fB,  fC);
    m_akVertex[17] = Vector3f(0.0f,  -fB,  fC);
    m_akVertex[18] = Vector3f(0.0f,   fB, -fC);
    m_akVertex[19] = Vector3f(0.0f,  -fB, -fC);

    m_aiIndex[  0] =  0;  m_aiIndex[  1] =  8;  m_aiIndex[  2] =  9;
    m_aiIndex[  3] =  0;  m_aiIndex[  4] =  9;  m_aiIndex[  5] =  4;
    m_aiIndex[  6] =  0;  m_aiIndex[  7] =  4;  m_aiIndex[  8] = 16;
    m_aiIndex[  9] =  0;  m_aiIndex[ 10] = 12;  m_aiIndex[ 11] = 13;
    m_aiIndex[ 12] =  0;  m_aiIndex[ 13] = 13;  m_aiIndex[ 14] =  1;
    m_aiIndex[ 15] =  0;  m_aiIndex[ 16] =  1;  m_aiIndex[ 17] =  8;
    m_aiIndex[ 18] =  0;  m_aiIndex[ 19] = 16;  m_aiIndex[ 20] = 17;
    m_aiIndex[ 21] =  0;  m_aiIndex[ 22] = 17;  m_aiIndex[ 23] =  2;
    m_aiIndex[ 24] =  0;  m_aiIndex[ 25] =  2;  m_aiIndex[ 26] = 12;
    m_aiIndex[ 27] =  8;  m_aiIndex[ 28] =  1;  m_aiIndex[ 29] = 18;
    m_aiIndex[ 30] =  8;  m_aiIndex[ 31] = 18;  m_aiIndex[ 32] =  5;
    m_aiIndex[ 33] =  8;  m_aiIndex[ 34] =  5;  m_aiIndex[ 35] =  9;
    m_aiIndex[ 36] = 12;  m_aiIndex[ 37] =  2;  m_aiIndex[ 38] = 10;
    m_aiIndex[ 39] = 12;  m_aiIndex[ 40] = 10;  m_aiIndex[ 41] =  3;
    m_aiIndex[ 42] = 12;  m_aiIndex[ 43] =  3;  m_aiIndex[ 44] = 13;
    m_aiIndex[ 45] = 16;  m_aiIndex[ 46] =  4;  m_aiIndex[ 47] = 14;
    m_aiIndex[ 48] = 16;  m_aiIndex[ 49] = 14;  m_aiIndex[ 50] =  6;
    m_aiIndex[ 51] = 16;  m_aiIndex[ 52] =  6;  m_aiIndex[ 53] = 17;
    m_aiIndex[ 54] =  9;  m_aiIndex[ 55] =  5;  m_aiIndex[ 56] = 15;
    m_aiIndex[ 57] =  9;  m_aiIndex[ 58] = 15;  m_aiIndex[ 59] = 14;
    m_aiIndex[ 60] =  9;  m_aiIndex[ 61] = 14;  m_aiIndex[ 62] =  4;
    m_aiIndex[ 63] =  6;  m_aiIndex[ 64] = 11;  m_aiIndex[ 65] = 10;
    m_aiIndex[ 66] =  6;  m_aiIndex[ 67] = 10;  m_aiIndex[ 68] =  2;
    m_aiIndex[ 69] =  6;  m_aiIndex[ 70] =  2;  m_aiIndex[ 71] = 17;
    m_aiIndex[ 72] =  3;  m_aiIndex[ 73] = 19;  m_aiIndex[ 74] = 18;
    m_aiIndex[ 75] =  3;  m_aiIndex[ 76] = 18;  m_aiIndex[ 77] =  1;
    m_aiIndex[ 78] =  3;  m_aiIndex[ 79] =  1;  m_aiIndex[ 80] = 13;
    m_aiIndex[ 81] =  7;  m_aiIndex[ 82] = 15;  m_aiIndex[ 83] =  5;
    m_aiIndex[ 84] =  7;  m_aiIndex[ 85] =  5;  m_aiIndex[ 86] = 18;
    m_aiIndex[ 87] =  7;  m_aiIndex[ 88] = 18;  m_aiIndex[ 89] = 19;
    m_aiIndex[ 90] =  7;  m_aiIndex[ 91] = 11;  m_aiIndex[ 92] =  6;
    m_aiIndex[ 93] =  7;  m_aiIndex[ 94] =  6;  m_aiIndex[ 95] = 14;
    m_aiIndex[ 96] =  7;  m_aiIndex[ 97] = 14;  m_aiIndex[ 98] = 15;
    m_aiIndex[ 99] =  7;  m_aiIndex[100] = 19;  m_aiIndex[101] =  3;
    m_aiIndex[102] =  7;  m_aiIndex[103] =  3;  m_aiIndex[104] = 10;
    m_aiIndex[105] =  7;  m_aiIndex[106] = 10;  m_aiIndex[107] = 11;

    CreatePlatonicNormals(iVQuantity);
    CreatePlatonicUVs(iVQuantity);
    if ( m_bInside )
        ReverseTriangleOrder(iTQuantity);

    TransformData(iVQuantity);
    TriMesh* pkMesh = new TriMesh(m_spkVertices,m_spkIndices,false);
    pkMesh->Normals = m_spkNormals;
    return pkMesh;
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Icosahedron ()
{
    float fGoldenRatio = 0.5f*(1.0f+Mathf::Sqrt(5.0f));
    float fInvRoot = 1.0f/Mathf::Sqrt(1.0f+fGoldenRatio*fGoldenRatio);
    float fU = fGoldenRatio*fInvRoot;
    float fV = fInvRoot;

    int iVQuantity = 12;
    int iTQuantity = 20;
    CreateData(iVQuantity,iTQuantity);

    m_akVertex[ 0] = Vector3f(  fU,  fV,0.0f);
    m_akVertex[ 1] = Vector3f( -fU,  fV,0.0f);
    m_akVertex[ 2] = Vector3f(  fU, -fV,0.0f);
    m_akVertex[ 3] = Vector3f( -fU, -fV,0.0f);
    m_akVertex[ 4] = Vector3f(  fV,0.0f,  fU);
    m_akVertex[ 5] = Vector3f(  fV,0.0f, -fU);
    m_akVertex[ 6] = Vector3f( -fV,0.0f,  fU);
    m_akVertex[ 7] = Vector3f( -fV,0.0f, -fU);
    m_akVertex[ 8] = Vector3f(0.0f,  fU,  fV);
    m_akVertex[ 9] = Vector3f(0.0f, -fU,  fV);
    m_akVertex[10] = Vector3f(0.0f,  fU, -fV);
    m_akVertex[11] = Vector3f(0.0f, -fU, -fV);

    m_aiIndex[ 0] =  0;  m_aiIndex[ 1] =  8;  m_aiIndex[ 2] =  4;
    m_aiIndex[ 3] =  0;  m_aiIndex[ 4] =  5;  m_aiIndex[ 5] = 10;
    m_aiIndex[ 6] =  2;  m_aiIndex[ 7] =  4;  m_aiIndex[ 8] =  9;
    m_aiIndex[ 9] =  2;  m_aiIndex[10] = 11;  m_aiIndex[11] =  5;
    m_aiIndex[12] =  1;  m_aiIndex[13] =  6;  m_aiIndex[14] =  8;
    m_aiIndex[15] =  1;  m_aiIndex[16] = 10;  m_aiIndex[17] =  7;
    m_aiIndex[18] =  3;  m_aiIndex[19] =  9;  m_aiIndex[20] =  6;
    m_aiIndex[21] =  3;  m_aiIndex[22] =  7;  m_aiIndex[23] = 11;
    m_aiIndex[24] =  0;  m_aiIndex[25] = 10;  m_aiIndex[26] =  8;
    m_aiIndex[27] =  1;  m_aiIndex[28] =  8;  m_aiIndex[29] = 10;
    m_aiIndex[30] =  2;  m_aiIndex[31] =  9;  m_aiIndex[32] = 11;
    m_aiIndex[33] =  3;  m_aiIndex[34] = 11;  m_aiIndex[35] =  9;
    m_aiIndex[36] =  4;  m_aiIndex[37] =  2;  m_aiIndex[38] =  0;
    m_aiIndex[39] =  5;  m_aiIndex[40] =  0;  m_aiIndex[41] =  2;
    m_aiIndex[42] =  6;  m_aiIndex[43] =  1;  m_aiIndex[44] =  3;
    m_aiIndex[45] =  7;  m_aiIndex[46] =  3;  m_aiIndex[47] =  1;
    m_aiIndex[48] =  8;  m_aiIndex[49] =  6;  m_aiIndex[50] =  4;
    m_aiIndex[51] =  9;  m_aiIndex[52] =  4;  m_aiIndex[53] =  6;
    m_aiIndex[54] = 10;  m_aiIndex[55] =  5;  m_aiIndex[56] =  7;
    m_aiIndex[57] = 11;  m_aiIndex[58] =  7;  m_aiIndex[59] =  5;

    CreatePlatonicNormals(iVQuantity);
    CreatePlatonicUVs(iVQuantity);
    if ( m_bInside )
        ReverseTriangleOrder(iTQuantity);

    TransformData(iVQuantity);
    TriMesh* pkMesh = new TriMesh(m_spkVertices,m_spkIndices,false);
    pkMesh->Normals = m_spkNormals;
    return pkMesh;
}
//----------------------------------------------------------------------------
