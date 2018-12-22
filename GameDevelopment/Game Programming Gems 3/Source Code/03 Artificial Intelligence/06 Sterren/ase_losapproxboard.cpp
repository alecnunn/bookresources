/* Copyright (C) William van der Sterren, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) William van der Sterren, 2002"
 */

/***************************************************************************  
 *                                                                            
 *     purpose: approximated line-of-sight lookup table and overlay
*/


#include "stdafx.h"
#include "ase_losapproxboard.h"

#include "ase_terrain.h"
#include "ase_threatboard.h"

#include <vector>
#include <algorithm> // though MSVC6 doesn't feature min/max there
using namespace std;

#include <cmath>
#include <cassert>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


COLORREF ASE_LOSApproximationBoard::m_LOSApproximationBrushes[] =
  {
    RGB(128,248, 96),  //!< safe 
    RGB(  0,180,  0),  //!< 
    RGB(  0,120,  0),  //!< 
    RGB(  0,  0,  0),  //!< not used?

    RGB(248,208, 96),  //!< deep yellow
    RGB(255,180,190),  //!< 
    RGB(255,120,120),  //!< 
    RGB(000,000,000),  //!< not used?

    RGB(248,176, 96),  //!< orange
    RGB(180,180,255),  //!< 
    RGB(120,120,255),  //!< 
    RGB(000,000,000),  //!< not used?

    RGB(248,128, 96),  //!< red
    RGB(180,255,180),  //!< 
    RGB(120,255,120),  //!< 
    RGB(000,000,000)   //!< 
  };



ASE_LOSApproximationBoard::ASE_LOSApproximationBoard(unsigned int            aNumberOfRows, 
                                                     unsigned int            aNumberOfColumns,
                                                     const ASE_TerrainBoard* theTerrainBoard,
                                                     const ASE_ThreatBoard*  theThreatBoard
                                                    )
  : ASE_Board(aNumberOfRows, aNumberOfColumns, 5, m_LOSApproximationBrushes),
    m_kTerrainBoard(theTerrainBoard),
    m_kThreatBoard(theThreatBoard),
    m_kDangerFromFarThreats(1),
    m_kDangerFromNearThreats(2),
    m_kDangerFromNearAndFarThreats(3)
  {
    assert( m_kTerrainBoard );
    assert( m_kThreatBoard );
    assert( GetNumberOfColumns() == m_kTerrainBoard->GetNumberOfColumns() );
    assert( GetNumberOfRows()    == m_kTerrainBoard->GetNumberOfRows() );
    assert( GetNumberOfColumns() == m_kThreatBoard->GetNumberOfColumns() );
    assert( GetNumberOfRows()    == m_kThreatBoard->GetNumberOfRows() );

    m_LineOfSightScanInfos.resize(GetNumberOfColumns() * GetNumberOfRows());

    Clear();
  }




ASE_LOSApproximationBoard::~ASE_LOSApproximationBoard()
  {
    m_LineOfSightScanInfos.clear();
  }


void      ASE_LOSApproximationBoard::Clear()
  {
    ASE_Board::Clear();

    // init changes
    InitChangeRange();
  }


float     ASE_LOSApproximationBoard::GetRiskValue(int aRow, int aCol) const
  {
    unsigned int idx;
    idx = GetIndexForRowColumn(aRow, aCol);
    return GetRiskValue(idx);
  }


float     ASE_LOSApproximationBoard::GetRiskValue(unsigned int anIndex) const
  {
    float result;
    result  = static_cast<float>(m_Cells[anIndex]);
    if ( result > 0 )
      result -= 1;
         
    result /= 3.0;
    assert( result <= 1.0 );
    assert( result >= 0.0 );
    return result;
  }

COLORREF  ASE_LOSApproximationBoard::GetCellColorForOffset(unsigned int anIndex, CellValue anOffset) const
  {
    assert( !IsEmptyCell(anIndex) );

    unsigned int idx;
    idx  = GetCellValue(anIndex);
    idx  = 4 * (idx - 1);
    idx += static_cast<unsigned int>(anOffset);

    assert( idx < sizeof(m_LOSApproximationBrushes) );

    return m_LOSApproximationBrushes[idx];
  }


void ASE_LOSApproximationBoard::ResetChangeRange()
  {
    m_MinThreatX  = GetNumberOfColumns();
    m_MaxThreatX  = 0;
    m_MinThreatY  = GetNumberOfRows();
    m_MaxThreatY  = 0;

    m_MinTerrainX = GetNumberOfColumns(); 
    m_MaxTerrainX = 0;                    
    m_MinTerrainY = GetNumberOfRows();    
    m_MaxTerrainY = 0;                    
  }


void ASE_LOSApproximationBoard::InitChangeRange()
  {
    m_MinThreatX  = 0; 
    m_MaxThreatX  = GetNumberOfColumns();
    m_MinThreatY  = 0;
    m_MaxThreatY  = GetNumberOfRows();

    m_MinTerrainX = 0;                     
    m_MaxTerrainX = GetNumberOfColumns();  
    m_MinTerrainY = 0;                     
    m_MaxTerrainY = GetNumberOfRows();     
  }


void ASE_LOSApproximationBoard::GetChangeRange(int& aMinX, int& aMinY, int& aMaxX, int& aMaxY)
  {
    assert( DidAnythingChange() );
    aMinX = min(m_MinThreatX, m_MinTerrainX);
    aMinY = min(m_MinThreatY, m_MinTerrainY);
    aMaxX = max(m_MaxThreatX, m_MaxTerrainX);
    aMaxY = max(m_MaxThreatY, m_MaxTerrainY);
  }


bool ASE_LOSApproximationBoard::DidAnythingChange() const
  {
    assert( ( m_MaxThreatX   >= m_MinThreatX  ) == ( m_MaxThreatY   >= m_MinThreatY  ) );
    assert( ( m_MaxTerrainX  >= m_MinTerrainX ) == ( m_MaxTerrainY  >= m_MinTerrainY ) );
    return (   ( m_MaxThreatX  >= m_MinThreatX  )
            || ( m_MaxTerrainX >= m_MinTerrainX )
           );
  }


bool ASE_LOSApproximationBoard::IsPositionInRangeOfAnyThreat(int anX, int aY, int& /*!out*/ theDistance)
  {
    const int kRange    = m_kThreatBoard->GetMaxThreatReach();
    const int kRangeSqr = kRange * kRange;

    CoordPairList kThreatPositions;
    m_kThreatBoard->GetThreatPositions(kThreatPositions);

    theDistance = 0;

    // iterate over all known threats, and update value accordingly
    CoordPairList::const_iterator threat;
    for ( threat =  kThreatPositions.begin(); 
          threat != kThreatPositions.end(); 
          ++threat
        )
      {
        int dsqr;
        dsqr = (anX - threat->x) * (anX - threat->x) + (aY - threat->y) * (aY - threat->y);
        if ( dsqr <= kRangeSqr )
          {
            int   dist;
            float d;
            d    = sqrt(dsqr);
            dist = 1 + (kRange - static_cast<int>(d));
            theDistance = max(theDistance, dist);
          }
      }

    return ( theDistance > 0 );
  }


void ASE_LOSApproximationBoard::ApplyTerrainModification(int anX, int aY)
  {
    // irrelevant if not near a threat
    int threatviewingdist;

    if ( !IsPositionInRangeOfAnyThreat(anX, aY, threatviewingdist) )
      return;

    m_MinTerrainX = min(m_MinTerrainX, anX - threatviewingdist);
    m_MinTerrainY = min(m_MinTerrainY, aY  - threatviewingdist);
    m_MaxTerrainX = max(m_MaxTerrainX, anX + threatviewingdist);
    m_MaxTerrainY = max(m_MaxTerrainY, aY  + threatviewingdist);
  }


void ASE_LOSApproximationBoard::ApplyThreatModification(int anX, int aY)
  {
    const int kMaxThreatReach = 1 + m_kThreatBoard->GetMaxThreatReach();

    // always relevant
    m_MinThreatX = min(m_MinThreatX, anX - kMaxThreatReach);
    m_MinThreatY = min(m_MinThreatY, aY  - kMaxThreatReach);
    m_MaxThreatX = max(m_MaxThreatX, anX + kMaxThreatReach);
    m_MaxThreatY = max(m_MaxThreatY, aY  + kMaxThreatReach);
  }


void ASE_LOSApproximationBoard::ComputeLinesOfFireApproximation()
  {
    // iterate over each cell, and compute its approximation
    // use a number of short-cuts to cut down on computation time

    assert( m_LineOfSightScanInfos.size() == GetNumberOfColumns() * GetNumberOfRows() );

    const int kEntries = GetNumberOfColumns() * GetNumberOfRows();
    const int kRange   = static_cast<int>(m_kThreatBoard->GetMaxThreatReach());

    ASE_LineOfSightScan::SetThreatReachDistance(m_kThreatBoard->GetMaxThreatReach());

  
    int minr;
    int minc;
    int maxr;
    int maxc;
    if ( !DidAnythingChange() )
      return;

    GetChangeRange(minr, minc, maxr, maxc);
/*
    minr = 0;
    minc = 0;
    maxr = -1 + static_cast<int>(GetNumberOfRows());
    maxc = -1 + static_cast<int>(GetNumberOfColumns());
*/
    PrepareListOfObstacles();

    for ( unsigned int idx = 0; idx < kEntries; ++idx )
      {
        unsigned int row;
        unsigned int col;
        GetRowAndColumnForIndex(idx, row, col);

        int irow;
        int icol;
        irow = static_cast<int>(row);
        icol = static_cast<int>(col);

        if (   ( irow >= minr )
            && ( irow <= maxr )
            && ( icol >= minc )
            && ( icol <= maxc )
           )
          {
            if ( !IsValidBoardLocation(row, col) )
              continue;

            // test for any nearby obstacle
            if ( !IsAnyObstacleWithinRange(row, col, kRange) )
              {
                SetLocationAsLackingAnyCover(row, col);
              }
            else
              {
                ApproximateLOFForLocationBySector(row, col);
              }
          }
      }

    UpdateBoard();

    ResetChangeRange();
  }


void ASE_LOSApproximationBoard::PrepareListOfObstacles()
  {
    m_ObstaclePositions.clear();

    const int kEntries = GetNumberOfColumns() * GetNumberOfRows();

    for ( unsigned int idx = 0; idx < kEntries; ++idx )
      {
        unsigned int row;
        unsigned int col;
        GetRowAndColumnForIndex(idx, row, col);

        if ( m_kTerrainBoard->IsLocationImpassable(row, col) )
          {
            ASE_CoordPair pos(row, col);
            m_ObstaclePositions.push_back(pos);
          }
      }
  }


bool ASE_LOSApproximationBoard::IsAnyObstacleWithinRange(int x, int y, int theRange) const
  {
    const int rangeSqr = theRange * theRange;

    CoordPairList::const_iterator pos;
    for ( pos =  m_ObstaclePositions.begin();
          pos != m_ObstaclePositions.end();
          ++pos
        )
      {
        int dx;
        int dy;
        int dsqr;
        dx   = pos->x - x;
        dy   = pos->y - y;
        dsqr = dx * dx + dy * dy;
 
        if ( dsqr <= rangeSqr )
          return true;
      }
    return false;
  }


bool ASE_LOSApproximationBoard::IsAnyObstacleWithinRangeAndSector(int x, int y, int theRange, unsigned int aSector) const
  {
    const int rangeSqr = theRange * theRange;

    CoordPairList::const_iterator pos;
    for ( pos =  m_ObstaclePositions.begin();
          pos != m_ObstaclePositions.end();
          ++pos
        )
      {
        int dx;
        int dy;
        int dsqr;
        dx   = pos->x - x;
        dy   = pos->y - y;
        dsqr = dx * dx + dy * dy;
 
        if ( dsqr > rangeSqr )
          continue;

        unsigned int sector;
        sector = ASE_LineOfSightScan::GetSectorForLine(dx, dy);
 
        if ( sector == aSector )
          return true;
      }
    return false;
  }


unsigned int ASE_LOSApproximationBoard::GetDistanceToNearestObstacleWithinRangeAndSector(int x, int y, int theRange, unsigned int aSector) const
  {
    const int rangeSqr = theRange * theRange;

    int minSqr = ( (theRange + 1) * (theRange + 1) );

    CoordPairList::const_iterator pos;
    for ( pos =  m_ObstaclePositions.begin();
          pos != m_ObstaclePositions.end();
          ++pos
        )
      {
        int dx;
        int dy;
        int dsqr;
        dx   = pos->x - x;
        dy   = pos->y - y;
        dsqr = dx * dx + dy * dy;
 
        if ( dsqr > rangeSqr )
          continue;

        unsigned int sector;
        sector = ASE_LineOfSightScan::GetSectorForLine(dx, dy);
 
        if ( sector == aSector )
          {
            minSqr = min(minSqr, dsqr);
          }
      }
    return static_cast<unsigned int>(minSqr);
  }


void ASE_LOSApproximationBoard::SetLocationAsLackingAnyCover(int x, int y)
  {
    ASE_LineOfSightScan los;
    los.SetNoCoverAtAll();

    unsigned int idx;
    idx = GetIndexForRowColumn(x, y);

    m_LineOfSightScanInfos[idx] = los;
  }


void ASE_LOSApproximationBoard::ApproximateLOFForLocationBySector(int x, int y)
  {
    unsigned int idx;
    idx = GetIndexForRowColumn(x, y);

    m_LineOfSightScanInfos[idx].Clear();

    for ( unsigned int sector = 0; sector < ASE_LineOfSightScan::GetNumberOfSectors(); ++sector )
      {
        unsigned int obstSqr;
        obstSqr = GetDistanceToNearestObstacleWithinRangeAndSector(x, y, m_kThreatBoard->GetMaxThreatReach(), sector);

        if (   ( ASE_LineOfSightScan::IsOutOfReachSqrd(obstSqr) )
            || ( ASE_LineOfSightScan::IsAFarDistanceSqrd(obstSqr) )
           )
          {
            m_LineOfSightScanInfos[idx].UpdateSector(sector, ASE_LineOfSightScan::eObservedFromFarDistance);
          }
        else
          {
            ComputeSectorLOSForLocation(x, y, sector, m_kThreatBoard->GetMaxThreatReach());
          }
      }
  } 


void ASE_LOSApproximationBoard::ComputeSectorLOSForLocation(int x, int y, unsigned int aSector, unsigned int aRange)
  {
    // try each applicable ray from the template for this sector, until getting a eObservedFromFarDistance
    const ASE_PieScan* pie = m_kThreatBoard->GetLineOfSightTemplate();

    unsigned int nr_of_rays;
    nr_of_rays = pie->GetNumberOfRays();

    ASE_CoordPair here(x, y);
    CoordPairList raytrace;

    const int minmaxdistsqr = static_cast<int>(aRange * aRange);
    int maxdistsqr          = 0;

    for ( unsigned int ray = 0; ray < nr_of_rays; ++ray )
      {
        if ( pie->IsRayInSector(ray, aSector) )
          {
            raytrace.clear();
            pie->GetRayForLocation(ray, here, raytrace);

            CoordPairList::const_iterator pos;
            for ( pos =  raytrace.begin(); 
                  pos != raytrace.end(); 
                  ++pos
                )
              {
                if ( !IsValidBoardLocation(pos->x, pos->y) )
                  break;

                if ( m_kTerrainBoard->IsLocationImpassable(pos->x, pos->y) )
                  break;
              }

            // determine distance sqr
            if (   ( pos != raytrace.end() )
                && ( pos != raytrace.begin() )
               ) 
              {
                int dx;
                int dy;
                int dsqr;
                dx   = pos->x - x;
                dy   = pos->y - y;
                dsqr = (dx * dx) + (dy * dy);

                maxdistsqr = max(maxdistsqr, dsqr); 
              }
            else
            if ( pos == raytrace.end() )
              {
                maxdistsqr = minmaxdistsqr; 
              }

            // test for bail out
            if ( maxdistsqr >= minmaxdistsqr )
              break;
          }
      }

    // check max distance, and set los accordingly
    ASE_LineOfSightScan::ScanValue value;
    if (   ( ASE_LineOfSightScan::IsOutOfReachSqrd(maxdistsqr) )
        || ( ASE_LineOfSightScan::IsAFarDistanceSqrd(maxdistsqr) )
       ) 
      value = ASE_LineOfSightScan::eObservedFromFarDistance;
    else
    if ( maxdistsqr == 0 )
      value = ASE_LineOfSightScan::eNotObserved;
    else
    if ( ASE_LineOfSightScan::IsAMediumDistanceSqrd(maxdistsqr) )
      value = ASE_LineOfSightScan::eObservedFromMediumDistance;
    else
      value = ASE_LineOfSightScan::eObservedFromNearDistance;

    unsigned int idx;
    idx = GetIndexForRowColumn(x, y);
    m_LineOfSightScanInfos[idx].UpdateSector(aSector, value);
  }


void ASE_LOSApproximationBoard::UpdateBoard()
  {
    /*! now update the board values, signaling danger positions
        given current threats

        1. set all cells to safe (0)
        2. iterate over each threat
           - then iterate over every position within reach of the threat
           - if position is visible by threat, mark as danger
    */
    const int kEntries = GetNumberOfColumns() * GetNumberOfRows();

    CoordPairList kThreatPositions;
    m_kThreatBoard->GetThreatPositions(kThreatPositions);

    for ( unsigned int idx = 0; idx < kEntries; ++idx )
      {
        unsigned int row;
        unsigned int col;
        GetRowAndColumnForIndex(idx, row, col);

        CellValue value;
        value = 0;
        
        // iterate over all known threats, and update value accordingly
        CoordPairList::const_iterator threat;
        for ( threat =  kThreatPositions.begin(); 
              threat != kThreatPositions.end(); 
              ++threat
            )
          {
            ASE_LineOfSightScan::ScanValue los1;
            ASE_LineOfSightScan::ScanValue los2;
            los1 = m_LineOfSightScanInfos[idx].GetObservationFromPosition(row, col, threat->x, threat->y);

            unsigned int jdx;
            jdx  = GetIndexForRowColumn(threat->x, threat->y);
            los2 = m_LineOfSightScanInfos[jdx].GetObservationFromPosition(threat->x, threat->y, row, col);

            if (   ( los1 != ASE_LineOfSightScan::eNotObserved )
                && ( los2 != ASE_LineOfSightScan::eNotObserved )
               )
              {  
                value = max(value, static_cast<CellValue>(los1));
              }
          }
        // skip over 'green' value
        if ( value > 0 )
          value++;

        SetCellValue(idx, value);
      }
  }


/*
void ASE_LOSApproximationBoard::ComputeLinesOfFireApproximation()
  {
     ComputeLinesOfFireApproximation2();

     if ( !DidAnythingChange() )
       return;

     ASE_LineOfSightScan::SetThreatReachDistance(m_kThreatBoard->GetMaxThreatReach());

     int minx;
     int miny;
     int maxx;
     int maxy;
     GetChangeRange(minx, miny, maxx, maxy);

     // adjust for threat range
     minx = max(0, minx - m_kThreatBoard->GetMaxThreatReach());
     miny = max(0, miny - m_kThreatBoard->GetMaxThreatReach());
     maxx = min(-1 + GetNumberOfColumns(), maxx + m_kThreatBoard->GetMaxThreatReach());
     maxy = min(-1 + GetNumberOfRows(),    maxy + m_kThreatBoard->GetMaxThreatReach());

     InitObserversLOS(minx, miny, maxx, maxy);

     // fall back on original area
     GetChangeRange(minx, miny, maxx, maxy);
     minx = max(0, minx);
     miny = max(0, miny);
     maxx = min(-1 + GetNumberOfColumns(), maxx);
     maxy = min(-1 + GetNumberOfRows(),    maxy);

     ProcessLocations(minx, miny, maxx, maxy);
     UpdateBoard(minx, miny, maxx, maxy);

     ResetChangeRange();
  }
*/

