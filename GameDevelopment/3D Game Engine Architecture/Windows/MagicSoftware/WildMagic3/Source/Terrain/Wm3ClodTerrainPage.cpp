// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3ClodTerrainPage.h"
#include "Wm3ClodTerrainBlock.h"
#include "Wm3ClodTerrainVertex.h"
#include "Wm3Camera.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,ClodTerrainPage,TriMesh);
WM3_IMPLEMENT_ABSTRACT_STREAM(ClodTerrainPage);
WM3_IMPLEMENT_DEFAULT_NAME_ID(ClodTerrainPage,TriMesh);

//----------------------------------------------------------------------------
ClodTerrainPage::ClodTerrainPage (unsigned short usSize,
    unsigned short* ausHeight, const Vector2f& rkOrigin, float fMinElevation,
    float fMaxElevation, float fSpacing, float fUVBias)
    :
    m_kOrigin(rkOrigin)
{
    // usSize = 2^p + 1, p <= 7
    assert( usSize ==  3 || usSize ==  5 || usSize ==   9 || usSize == 17
        ||  usSize == 33 || usSize == 65 || usSize == 129 );

    // generate space for all the vertices
    m_iTotalVQuantity = (int)(usSize*usSize);
    Vector3f* akVertex = new Vector3f[m_iTotalVQuantity];
    Vertices = new Vector3fArray(m_iTotalVQuantity,akVertex);

    // generate space for the texture coordinates
    Vector2f* akUV = new Vector2f[m_iTotalVQuantity];
    m_spkUVs = new Vector2fArray(m_iTotalVQuantity,akUV);
    m_fUVBias = fUVBias;

    // generate space for the indices
    m_iTotalTQuantity = 2*(usSize-1)*(usSize-1);
    m_iTotalIQuantity = 3*m_iTotalTQuantity;
    int* aiIndex = new int[m_iTotalIQuantity];
    Indices = new IntArray(m_iTotalIQuantity,aiIndex);

    // native data
    m_usSize = usSize;
    m_ausHeight = ausHeight;
    m_fMinElevation = fMinElevation;
    m_fMaxElevation = fMaxElevation;
    m_fSpacing = fSpacing;

    InitializeDerivedData();
}
//----------------------------------------------------------------------------
ClodTerrainPage::ClodTerrainPage ()
    :
    m_kOrigin(Vector2f::ZERO)
{
    // native data
    m_iTotalVQuantity = 0;
    m_iTotalIQuantity = 0;
    m_usSize = 0;
    m_ausHeight = 0;
    m_fMinElevation = 0.0f;
    m_fMaxElevation = 0.0f;
    m_fSpacing = 0.0f;
    m_fUVBias = 0.0f;

    // derived data
    m_usSizeM1 = 0;
    m_fPixelTolerance = 0.0f;
    m_fWorldTolerance = 0.0f;
    m_fInvSpacing = 0.0f;
    m_fTextureSpacing = 0.0f;
    m_fMultiplier = 0.0f;
    m_bNeedsTessellation = false;
    m_ausLookup = 0;
    m_iConnectLength = 0;
    m_akTVertex = 0;
    m_usBlockQuantity = 0;
    m_akBlock = 0;
    m_usQueueQuantity = 0;
    m_ausQueue = 0;
    m_usFront = 0;
    m_usRear = 0;
    m_usItemsInQueue = 0;
}
//----------------------------------------------------------------------------
ClodTerrainPage::~ClodTerrainPage ()
{
    delete[] m_ausHeight;
    delete[] m_akTVertex;
    delete[] m_akBlock;
    delete[] m_ausQueue;
    delete[] m_ausLookup;
}
//----------------------------------------------------------------------------
void ClodTerrainPage::InitializeDerivedData ()
{
    m_usSizeM1 = m_usSize - 1;
    m_fPixelTolerance = 1.0f;
    m_fWorldTolerance = -1.0f;
    m_fInvSpacing = 1.0f/m_fSpacing;
    m_fTextureSpacing = 1.0f/float(m_usSizeM1);
    m_fMultiplier = (m_fMaxElevation - m_fMinElevation)/65535.0f;
    m_bNeedsTessellation = true;

    // for tessellation (mapping of indices)
    m_ausLookup = new unsigned short[m_iTotalVQuantity];

    // initialize vertex information array
    m_akTVertex = new ClodTerrainVertex[m_iTotalVQuantity];
    memset(m_akTVertex,0,m_iTotalVQuantity*sizeof(ClodTerrainVertex));

    // allocate quadtree
    m_usBlockQuantity = m_usSize*(m_usSize-2)/3;
    m_akBlock = new ClodTerrainBlock[m_usBlockQuantity];

    // initialize quadtree
    unsigned char ucStride = (unsigned char)((m_usSize-1)/2);
    m_akBlock[0].Initialize(this,m_akBlock,0,0,0,ucStride,true);
    m_akBlock[0].UpdateBoundingBox(this,m_akBlock,0,ucStride);

    // model bounding sphere contains the top level block's bounding box
    Vector3f kCenter = 0.5f*(m_akBlock[0].GetMax() + m_akBlock[0].GetMin());
    Vector3f kDiag = 0.5f*(m_akBlock[0].GetMax() - m_akBlock[0].GetMin());
    ModelBound->SetCenter(kCenter);
    ModelBound->SetRadius(kDiag.Length());

    // allocate queue
    m_usQueueQuantity = m_usBlockQuantity - m_usBlockQuantity/4;
    m_ausQueue = new unsigned short[m_usQueueQuantity];

    // root of quadtree is initially active
    m_ausQueue[0] = 0;
    m_usFront = 0;
    m_usRear = 1;
    m_usItemsInQueue = 1;
}
//----------------------------------------------------------------------------
void ClodTerrainPage::SetPixelTolerance (const Renderer* pkRenderer,
    float fTolerance)
{
    float fWidth = (float)pkRenderer->GetWidth();
    CameraPtr spkCamera = pkRenderer->GetCamera();
    float fRMax = spkCamera->GetRMax();
    float fDMin = spkCamera->GetDMin();

    m_fPixelTolerance = fTolerance;
    m_fWorldTolerance = 2.0f*fRMax*m_fPixelTolerance/(fDMin*fWidth);
    m_fWorldTolerance *= m_fWorldTolerance;
}
//----------------------------------------------------------------------------
float ClodTerrainPage::GetHeight (float fX, float fY) const
{
    float fXGrid = (fX - m_kOrigin.X())*m_fInvSpacing;
    if ( fXGrid < 0.0f || fXGrid >= float(m_usSizeM1) )
    {
        // location not in page
        return Mathf::MAX_REAL;
    }

    float fYGrid = (fY - m_kOrigin.Y())*m_fInvSpacing;
    if ( fYGrid < 0.0f || fYGrid >= float(m_usSizeM1) )
    {
        // location not in page
        return Mathf::MAX_REAL;
    }

    float fCol = Mathf::Floor(fXGrid);
    unsigned short usCol = (unsigned short) fCol;
    float fRow = Mathf::Floor(fYGrid);
    unsigned short usRow = (unsigned short) fRow;

    unsigned short usIndex = usCol + m_usSize*usRow;
    float fDx = fXGrid - fCol;
    float fDy = fYGrid - fRow;
    float fH00, fH10, fH01, fH11, fHeight;

    if ( (usCol & 1) == (usRow & 1) )
    {
        float fDiff = fDx - fDy;
        fH00 = m_fMinElevation + m_fMultiplier*m_ausHeight[usIndex];
        fH11 = m_fMinElevation + m_fMultiplier *
            m_ausHeight[usIndex+1+m_usSize];
        if ( fDiff > 0.0f )
        {
            fH10 = m_fMinElevation + m_fMultiplier*m_ausHeight[usIndex+1];
            fHeight = (1.0f-fDiff-fDy)*fH00+fDiff*fH10+fDy*fH11;
        }
        else
        {
            fH01 = m_fMinElevation + m_fMultiplier *
                m_ausHeight[usIndex+m_usSize];
            fHeight = (1.0f+fDiff-fDx)*fH00-fDiff*fH01+fDx*fH11;
        }
    }
    else
    {
        float fSum = fDx + fDy;
        fH10 = m_fMinElevation + m_fMultiplier*m_ausHeight[usIndex+1];
        fH01 = m_fMinElevation + m_fMultiplier*m_ausHeight[usIndex+m_usSize];
        if ( fSum <= 1.0f )
        {
            fH00 = m_fMinElevation + m_fMultiplier*m_ausHeight[usIndex];
            fHeight = (1.0f-fSum)*fH00+fDx*fH10+fDy*fH01;
        }
        else
        {
            fH11 = m_fMinElevation + m_fMultiplier *
                m_ausHeight[usIndex+1+m_usSize];
            fHeight = (fSum-1.0f)*fH11+(1.0f-fDy)*fH10+(1.0f-fDx)*fH01;
        }
    }

    return fHeight;
}
//----------------------------------------------------------------------------
bool ClodTerrainPage::IntersectFrustum (const Camera* pkCamera)
{
    // check if terrain page itself is inside frustum
    m_akBlock[0].TestIntersectFrustum(this,pkCamera);
    bool bIntersect = m_akBlock[0].GetVisible();
    m_akBlock[0].ClearBits();
    return bIntersect;
}
//----------------------------------------------------------------------------
void ClodTerrainPage::AddToQueue (unsigned short usBlock)
{
    m_ausQueue[m_usRear] = usBlock;
    if ( ++m_usRear == m_usQueueQuantity )
        m_usRear = 0;
    m_usItemsInQueue++;
}
//----------------------------------------------------------------------------
unsigned short ClodTerrainPage::RemoveFromQueue ()
{
    unsigned short usBlock = m_ausQueue[m_usFront];
    if ( ++m_usFront == m_usQueueQuantity )
        m_usFront = 0;
    m_usItemsInQueue--;
    return usBlock;
}
//----------------------------------------------------------------------------
unsigned short ClodTerrainPage::ReadFromQueue (unsigned short usIndex)
{
    usIndex += m_usFront;
    if ( usIndex < m_usQueueQuantity )
        return m_ausQueue[usIndex];
    else
        return m_ausQueue[usIndex - m_usQueueQuantity];
}
//----------------------------------------------------------------------------
void ClodTerrainPage::ResetBlocks ()
{
    unsigned short usQueue, usBlock;
    if ( m_usFront < m_usRear )
    {
        m_usNumUnprocessed = m_usRear - m_usFront;
        for (usQueue = m_usFront; usQueue < m_usRear; usQueue++)
        {
            usBlock = m_ausQueue[usQueue];
            if ( m_akBlock[usBlock].BitsSet() )
            {
                m_akBlock[usBlock].Disable(this);
                m_akBlock[usBlock].ClearBits();
            }
        }
    }
    else
    {
        m_usNumUnprocessed = m_usQueueQuantity - m_usFront + m_usRear;
        for (usQueue = m_usFront; usQueue < m_usQueueQuantity; usQueue++)
        {
            usBlock = m_ausQueue[usQueue];
            if ( m_akBlock[usBlock].BitsSet() )
            {
                m_akBlock[usBlock].Disable(this);
                m_akBlock[usBlock].ClearBits();
            }
        }
        for (usQueue = 0; usQueue < m_usRear; usQueue++)
        {
            usBlock = m_ausQueue[usQueue];
            if ( m_akBlock[usBlock].BitsSet() )
            {
                m_akBlock[usBlock].Disable(this);
                m_akBlock[usBlock].ClearBits();
            }
        }
    }
}
//----------------------------------------------------------------------------
void ClodTerrainPage::SimplifyBlocks (const Camera* pkCamera,
    const Vector3f& rkModelEye, const Vector3f& rkModelDir,
    bool bCloseAssumption)
{
    while ( m_usNumUnprocessed > 0 )
    {
        // process the block in the front of queue
        unsigned short usBlock = RemoveFromQueue();
        ClodTerrainBlock* pkBlock = &m_akBlock[usBlock];

        if ( !pkBlock->GetProcessed() )
        {
            m_usNumUnprocessed--;

            unsigned short usChild;
            ClodTerrainBlock* pkChild;
            Vector2f kCLoc;
            int i;

            if ( pkBlock->IsFirstChild(usBlock) )
            {
                // first child has responsibility for replacing by parent
                if ( pkBlock->IsSibling(usBlock,ReadFromQueue(2)) )
                {
                    pkChild = pkBlock;
                    if ( bCloseAssumption )
                    {
                        for (i = 0; i < 4; i++, pkChild++)
                        {
                            kCLoc.X() = pkChild->GetX()*m_fSpacing +
                                m_kOrigin.X();
                            kCLoc.Y() = pkChild->GetY()*m_fSpacing +
                                m_kOrigin.Y();
                            pkChild->ComputeInterval(rkModelEye,rkModelDir,
                                m_fWorldTolerance,kCLoc,m_fSpacing);
                            if (pkChild->GetDeltaMax() > pkChild->GetDeltaL())
                                break;
                        }
                    }
                    else // distant assumption
                    {
                        for (i = 0; i < 4; i++, pkChild++)
                        {
                            pkChild->ComputeInterval(rkModelEye,
                                m_fWorldTolerance);
                            if (pkChild->GetDeltaMax() > pkChild->GetDeltaL())
                                break;
                        }
                    }

                    if ( i == 4 )
                    {
                        // remove child blocks (first child already removed)
                        for (i = 0; i < 3; i++)
                        {
                            usChild = RemoveFromQueue();
                            if ( !m_akBlock[usChild].GetProcessed() )
                                m_usNumUnprocessed--;
                            m_akBlock[usChild].ClearBits();
                        }

                        // add parent block
                        unsigned short usParent =
                            pkBlock->GetParentIndex(usBlock);
                        AddToQueue(usParent);
                        assert( !m_akBlock[usParent].GetProcessed() );
                        m_usNumUnprocessed++;
                        continue;
                    }
                }
            }

            if ( !pkBlock->GetVisibilityTested() )
                pkBlock->TestIntersectFrustum(this,pkCamera);

            if ( pkBlock->GetStride() > 1 )
            {
                // subdivide only if bounding box of block intersects frustum
                if ( pkBlock->GetVisible() )
                {
                    usChild = pkBlock->GetChildIndex(usBlock,1);
                    pkChild = &m_akBlock[usChild];
                    if ( bCloseAssumption ) 
                    {
                        for (i = 0; i < 4; i++, pkChild++)
                        {
                            kCLoc.X() = pkChild->GetX()*m_fSpacing +
                                m_kOrigin.X();
                            kCLoc.Y() = pkChild->GetY()*m_fSpacing +
                                m_kOrigin.Y();
                            pkChild->ComputeInterval(rkModelEye,rkModelDir,
                                m_fWorldTolerance,kCLoc,m_fSpacing);
                            if (pkChild->GetDeltaMax() > pkChild->GetDeltaL())
                                break;
                        }
                    }
                    else // distant assumption
                    {
                        for (i = 0; i < 4; i++, pkChild++)
                        {
                            pkChild->ComputeInterval(rkModelEye,
                                m_fWorldTolerance);
                            if (pkChild->GetDeltaMax() > pkChild->GetDeltaL())
                                break;
                        }
                    }

                    // subdivide only if children all agree it should happen
                    if ( i < 4 )
                    {
                        // add child blocks (parent already removed)
                        for (i = 0; i < 4; i++, usChild++)
                        {
                            // add child block
                            AddToQueue(usChild);
                            assert( !m_akBlock[usChild].GetProcessed() );
                            m_usNumUnprocessed++;
                        }
                        continue;
                    }
                }
            }

            // tag block as processed
            pkBlock->SetProcessed(true);
        }

        // put processed block at rear of queue
        AddToQueue(usBlock);
    }
}
//----------------------------------------------------------------------------
void ClodTerrainPage::SimplifyVertices (const Vector3f& rkModelEye,
    const Vector3f& rkModelDir, bool bCloseAssumption)
{
    unsigned int usQueue, usBlock;

    if ( m_usFront < m_usRear )
    {
        for (usQueue = m_usFront; usQueue < m_usRear; usQueue++)
        {
            usBlock = m_ausQueue[usQueue];
            if ( m_akBlock[usBlock].GetVisible() )
            {
                m_akBlock[usBlock].SimplifyVertices(this,rkModelEye,
                    rkModelDir,m_fWorldTolerance,bCloseAssumption);
            }
        }
    }
    else
    {
        for (usQueue = m_usFront; usQueue < m_usQueueQuantity; usQueue++)
        {
            usBlock = m_ausQueue[usQueue];
            if ( m_akBlock[usBlock].GetVisible() )
            {
                m_akBlock[usBlock].SimplifyVertices(this,rkModelEye,
                    rkModelDir,m_fWorldTolerance,bCloseAssumption);
            }
        }
        for (usQueue = 0; usQueue < m_usRear; usQueue++)
        {
            usBlock = m_ausQueue[usQueue];
            if ( m_akBlock[usBlock].GetVisible() )
            {
                m_akBlock[usBlock].SimplifyVertices(this,rkModelEye,
                    rkModelDir,m_fWorldTolerance,bCloseAssumption);
            }
        }
    }
}
//----------------------------------------------------------------------------
void ClodTerrainPage::Simplify (const Renderer* pkRenderer,
    const Vector3f& rkModelEye, const Vector3f& rkModelDir,
    bool bCloseAssumption)
{
    if ( m_fWorldTolerance == -1.0f )
        SetPixelTolerance(pkRenderer,m_fPixelTolerance);

    SimplifyBlocks(pkRenderer->GetCamera(),rkModelEye,rkModelDir,
        bCloseAssumption);
    SimplifyVertices(rkModelEye,rkModelDir,bCloseAssumption);

    m_bNeedsTessellation = true;
}
//----------------------------------------------------------------------------
void ClodTerrainPage::Render (ClodTerrainBlock& rkBlock)
{
    unsigned short usOrigin = rkBlock.GetX() + m_usSize*rkBlock.GetY();
    unsigned short usTwoStride = 2*rkBlock.GetStride();
    unsigned short usTwoStrideTimesSize = usTwoStride*m_usSize;
    unsigned short usIndex[5] =
    {
        usOrigin,
        usOrigin + usTwoStride,
        usOrigin + rkBlock.GetStride()*(m_usSize + 1),
        usOrigin + usTwoStrideTimesSize,
        usOrigin + usTwoStrideTimesSize + usTwoStride
    };

    if ( rkBlock.GetEven() )
    {
        RenderTriangle(usIndex[0],usIndex[3],usIndex[1]);
        RenderTriangle(usIndex[4],usIndex[1],usIndex[3]);
    }
    else
    {
        RenderTriangle(usIndex[1],usIndex[0],usIndex[4]);
        RenderTriangle(usIndex[3],usIndex[4],usIndex[0]);
    }

}
//----------------------------------------------------------------------------
void ClodTerrainPage::RenderTriangle (unsigned short usT, unsigned short usL,
    unsigned short usR)
{
    // determine if triangle is leaf or interior
    bool bInterior;
    if ( usR > usT )
    {
        if ( usL > usT )
            bInterior = (usR - usT > 1);
        else
            bInterior = (usT - usL > 1);
    }
    else
    {
        if ( usL > usT )
            bInterior = (usL - usT > 1);
        else
            bInterior = (usT - usR > 1);
    }

    if ( bInterior )
    {
        // Triangle is part of internal block and can be subdivided.  M is
        // the midpoint of edge <L,R>.
        unsigned short usM = ((usL + usR) >> 1);
        if ( m_akTVertex[usM].GetEnabled() )
        {
            RenderTriangle(usM,usT,usL);
            RenderTriangle(usM,usR,usT);
            return;
        }
    }

    // pack the vertex data
    int iVQuantity = Vertices->GetQuantity();
    Vector3f* akVertex = Vertices->GetData();
    Vector2f* akUV = m_spkUVs->GetData();
    unsigned char ucX, ucY;
    if ( m_ausLookup[usT] == (unsigned short)(~0) )
    {
        ucX = usT % m_usSize;
        ucY = usT / m_usSize;

        akVertex[iVQuantity].X() = GetX(ucX);
        akVertex[iVQuantity].Y() = GetY(ucY);
        akVertex[iVQuantity].Z() = GetHeight(usT);
        akUV[iVQuantity].X() = GetTextureCoordinate(ucX);
        akUV[iVQuantity].Y() = GetTextureCoordinate(ucY);
        m_ausLookup[usT] = iVQuantity++;
        Vertices->SetActiveQuantity(iVQuantity);
    }

    if ( m_ausLookup[usR] == (unsigned short)(~0) )
    {
        ucX = usR % m_usSize;
        ucY = usR / m_usSize;
        
        akVertex[iVQuantity].X() = GetX(ucX);
        akVertex[iVQuantity].Y() = GetY(ucY);
        akVertex[iVQuantity].Z() = GetHeight(usR);
        akUV[iVQuantity].X() = GetTextureCoordinate(ucX);
        akUV[iVQuantity].Y() = GetTextureCoordinate(ucY);
        m_ausLookup[usR] = iVQuantity++;
        Vertices->SetActiveQuantity(iVQuantity);
    }

    if ( m_ausLookup[usL] == (unsigned short)(~0) )
    {
        ucX = usL % m_usSize;
        ucY = usL / m_usSize;

        akVertex[iVQuantity].X() = GetX(ucX);
        akVertex[iVQuantity].Y() = GetY(ucY);
        akVertex[iVQuantity].Z() = GetHeight(usL);
        akUV[iVQuantity].X() = GetTextureCoordinate(ucX);
        akUV[iVQuantity].Y() = GetTextureCoordinate(ucY);
        m_ausLookup[usL] = iVQuantity++;
        Vertices->SetActiveQuantity(iVQuantity);
    }

    assert( iVQuantity < m_iTotalVQuantity );

    // add triangle to connectivity array
    int iIQuantity = Indices->GetQuantity();
    int* aiIndex = Indices->GetData();
    aiIndex[iIQuantity++] = m_ausLookup[usT];
    aiIndex[iIQuantity++] = m_ausLookup[usR];
    aiIndex[iIQuantity++] = m_ausLookup[usL];
    Indices->SetActiveQuantity(iIQuantity);

    assert( iIQuantity < m_iTotalIQuantity );
}
//----------------------------------------------------------------------------
void ClodTerrainPage::RenderBlocks ()
{
    // reset dynamic quantities
    memset(m_ausLookup,0xFF,m_usSize*m_usSize*sizeof(unsigned short));
    Vertices->SetActiveQuantity(0);
    Indices->SetActiveQuantity(0);

    unsigned short usQueue;
    if ( m_usFront < m_usRear )
    {
        for (usQueue = m_usFront; usQueue < m_usRear; usQueue++)
        {
            ClodTerrainBlock& rkBlock = m_akBlock[m_ausQueue[usQueue]];
            if ( rkBlock.GetVisible() )
                Render(rkBlock);
        }
    }
    else
    {
        for (usQueue = m_usFront; usQueue < m_usQueueQuantity; usQueue++)
        {
            ClodTerrainBlock& rkBlock = m_akBlock[m_ausQueue[usQueue]];
            if ( rkBlock.GetVisible() )
                Render(rkBlock);
        }
        for (usQueue = 0; usQueue < m_usRear; usQueue++)
        {
            ClodTerrainBlock& rkBlock = m_akBlock[m_ausQueue[usQueue]];
            if ( rkBlock.GetVisible() )
                Render(rkBlock);
        }
    }
}
//----------------------------------------------------------------------------
void ClodTerrainPage::Draw (Renderer& rkRenderer, bool bNoCull)
{
    if ( m_bNeedsTessellation )
    {
        m_bNeedsTessellation = false;
        RenderBlocks();
    }

    // It is possible (but not likely) that blocks are not culled, but the
    // camera is positioned so that no triangles are visible.  For example,
    // this can occur if you are below the terrain mesh and looking down.
    if ( Indices->GetQuantity() > 0 )
        TriMesh::Draw(rkRenderer,bNoCull);
}
//----------------------------------------------------------------------------
void ClodTerrainPage::StitchNextCol (ClodTerrainPage* pkNextCol)
{
    // 'this' is page (r,c), 'pkNextCol' is page (r,c+1)
    assert( pkNextCol->m_usSize == m_usSize );

    int iSize = (int)m_usSize;
    int iMax = iSize - 2;
    int iXThis = iSize - 1;
    int iXNext = 0;
    for (int iY = 1; iY <= iMax; iY++)
    {
        int iIThis = iXThis + iSize*iY;
        int iINext = iXNext + iSize*iY;
        ClodTerrainVertex* pkVThis = &m_akTVertex[iIThis];
        ClodTerrainVertex* pkVNext = &pkNextCol->m_akTVertex[iINext];
        pkVNext->SetDependent(0,pkVThis);
        pkVThis->SetDependent(1,pkVNext);
    }
}
//----------------------------------------------------------------------------
void ClodTerrainPage::UnstitchNextCol (ClodTerrainPage* pkNextCol)
{
    // 'this' is page (r,c), 'pkNextCol' is page (r,c+1)
    assert( pkNextCol->m_usSize == m_usSize );

    int iSize = (int)m_usSize;
    int iMax = iSize - 2;
    int iXThis = iSize - 1;
    int iXNext = 0;
    for (int iY = 1; iY <= iMax; iY++)
    {
        int iIThis = iXThis + iSize*iY;
        int iINext = iXNext + iSize*iY;
        ClodTerrainVertex* pkVThis = &m_akTVertex[iIThis];
        ClodTerrainVertex* pkVNext = &pkNextCol->m_akTVertex[iINext];
        pkVNext->SetDependent(0,0);
        pkVThis->SetDependent(1,0);
    }
}
//----------------------------------------------------------------------------
void ClodTerrainPage::StitchNextRow (ClodTerrainPage* pkNextRow)
{
    // 'this' is page (r,c), 'pkNextRow' is page (r+1,c)
    assert( pkNextRow->m_usSize == m_usSize );

    int iSize = (int)m_usSize;
    int iMax = iSize - 2;
    int iYThis = iSize - 1;
    int iYNext = 0;
    for (int iX = 1; iX <= iMax; iX++)
    {
        int iIThis = iX + iSize*iYThis;
        int iINext = iX + iSize*iYNext;
        ClodTerrainVertex* pkVThis = &m_akTVertex[iIThis];
        ClodTerrainVertex* pkVNext = &pkNextRow->m_akTVertex[iINext];
        pkVNext->SetDependent(1,pkVThis);
        pkVThis->SetDependent(0,pkVNext);
    }
}
//----------------------------------------------------------------------------
void ClodTerrainPage::UnstitchNextRow (ClodTerrainPage* pkNextRow)
{
    // 'this' is page (r,c), 'pkNextRow' is page (r+1,c)
    assert( pkNextRow->m_usSize == m_usSize );

    int iSize = (int)m_usSize;
    int iMax = iSize - 2;
    int iYThis = iSize - 1;
    int iYNext = 0;
    for (int iX = 1; iX <= iMax; iX++)
    {
        int iIThis = iX + iSize*iYThis;
        int iINext = iX + iSize*iYNext;
        ClodTerrainVertex* pkVThis = &m_akTVertex[iIThis];
        ClodTerrainVertex* pkVNext = &pkNextRow->m_akTVertex[iINext];
        pkVNext->SetDependent(1,0);
        pkVThis->SetDependent(0,0);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void ClodTerrainPage::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    TriMesh::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_usSize);
    int iVQuantity = (int)(m_usSize*m_usSize);
    m_ausHeight = new unsigned short[iVQuantity];
    rkStream.Read(iVQuantity,m_ausHeight);
    rkStream.Read(m_kOrigin);
    rkStream.Read(m_fMinElevation);
    rkStream.Read(m_fMaxElevation);
    rkStream.Read(m_fSpacing);

    InitializeDerivedData();

    WM3_END_DEBUG_STREAM_LOAD(ClodTerrainPage);
}
//----------------------------------------------------------------------------
void ClodTerrainPage::Link (Stream& rkStream, Stream::Link* pkLink)
{
    TriMesh::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool ClodTerrainPage::Register (Stream& rkStream) const
{
    return TriMesh::Register(rkStream);
}
//----------------------------------------------------------------------------
void ClodTerrainPage::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    // The vertex and triangle quantities are dynamically varied during the
    // simplification.  Write the maximum quantities to disk, then reset to
    // the current dynamic values.
    int iSaveVQ = Vertices->GetQuantity();
    int iSaveIQ = Indices->GetQuantity();
    Vertices->SetActiveQuantity(m_iTotalVQuantity);
    Indices->SetActiveQuantity(m_iTotalIQuantity);

    TriMesh::Save(rkStream);

    Vertices->SetActiveQuantity(iSaveVQ);
    Indices->SetActiveQuantity(iSaveIQ);

    // native data
    rkStream.Write(m_usSize);
    rkStream.Write(m_iTotalVQuantity,m_ausHeight);
    rkStream.Write(m_kOrigin);
    rkStream.Write(m_fMinElevation);
    rkStream.Write(m_fMaxElevation);
    rkStream.Write(m_fSpacing);

    WM3_END_DEBUG_STREAM_SAVE(ClodTerrainPage);
}
//----------------------------------------------------------------------------
StringTree* ClodTerrainPage::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(9,1);
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("size =",m_usSize));
    pkTree->SetString(2,StringTree::Format("origin =",m_kOrigin));
    pkTree->SetString(3,StringTree::Format("min elevation =",
        m_fMinElevation));
    pkTree->SetString(4,StringTree::Format("max elevation =",
        m_fMaxElevation));
    pkTree->SetString(5,StringTree::Format("spacing =",m_fSpacing));
    pkTree->SetString(6,StringTree::Format("uv bias =",m_fUVBias));
    pkTree->SetString(7,StringTree::Format("pixel tolerance =",
        m_fPixelTolerance));
    pkTree->SetString(8,StringTree::Format("world tolerance =",
        m_fWorldTolerance));

    pkTree->SetChild(0,TriMesh::SaveStrings());
    return pkTree;
}
//----------------------------------------------------------------------------
int ClodTerrainPage::GetMemoryUsed () const
{
    // The vertex and triangle quantities are dynamically varied during the
    // simplification.  Count the maximum quantities, then reset to the
    // current dynamic values.  The typecast to integer references is to
    // circumvent the 'const' of this function.  In effect, however, the
    // function is 'const' since the quantities are restored.
    int iSaveVQ = Vertices->GetQuantity();
    int iSaveIQ = Indices->GetQuantity();
    Vertices->SetActiveQuantity(m_iTotalVQuantity);
    Indices->SetActiveQuantity(m_iTotalIQuantity);

    int iSize = sizeof(ClodTerrainPage) - sizeof(TriMesh) +
        TriMesh::GetMemoryUsed() +
        m_iTotalVQuantity*sizeof(m_ausHeight[0]) + 
        m_iTotalVQuantity*sizeof(m_ausLookup[0]) +
        m_iTotalVQuantity*sizeof(m_akTVertex[0]) +
        m_usBlockQuantity*sizeof(m_akBlock[0]) +
        m_usQueueQuantity*sizeof(m_ausQueue[0]);

    Vertices->SetActiveQuantity(iSaveVQ);
    Indices->SetActiveQuantity(iSaveIQ);

    return iSize;
}
//----------------------------------------------------------------------------
int ClodTerrainPage::GetDiskUsed () const
{
    // The vertex and triangle quantities are dynamically varied during the
    // simplification.  Count the maximum quantities, then reset to the
    // current dynamic values.  The typecast to integer references is to
    // circumvent the 'const' of this function.  In effect, however, the
    // function is 'const' since the quantities are restored.
    int iSaveVQ = Vertices->GetQuantity();
    int iSaveIQ = Indices->GetQuantity();
    Vertices->SetActiveQuantity(m_iTotalVQuantity);
    Indices->SetActiveQuantity(m_iTotalIQuantity);

    int iDiskUsed = TriMesh::GetDiskUsed() +
        sizeof(m_usSize) +
        m_iTotalVQuantity*sizeof(m_ausHeight[0]) +
        sizeof(m_kOrigin) +
        sizeof(m_fMinElevation) +
        sizeof(m_fMaxElevation) +
        sizeof(m_fSpacing);

    Vertices->SetActiveQuantity(iSaveVQ);
    Indices->SetActiveQuantity(iSaveIQ);

    return iDiskUsed;
}
//----------------------------------------------------------------------------
