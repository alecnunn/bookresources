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
 *     purpose: threat and line-of-fire lookup table and overlay
*/

#include "stdafx.h"
#include "ase_threatboard.h"

#include "ase_terrain.h"

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


ASE_ThreatBoard::ASE_ThreatBoard(unsigned int            aNumberOfRows, 
                                 unsigned int            aNumberOfColumns,
                                 const ASE_TerrainBoard* theTerrain
                                )
  : ASE_Board(aNumberOfRows, aNumberOfColumns, 16, m_ThreatBrushes),
    m_RiskModus(eAnyLineOfFire),
    m_kTerrainBoard(theTerrain),
    m_kThreatLineOfFire(1),
    m_kThreatPosition(15),
    m_RiskFactor(4),
    m_LineOfFireReach(32),
    m_ThreatMovement(0),
    m_ThreatBrushes(0),
    m_kBaseRisk(0.15f),
    m_kMaxLOFCount(6)
  {
    assert( m_kTerrainBoard );
    assert( GetNumberOfColumns() == m_kTerrainBoard->GetNumberOfColumns() );
    assert( GetNumberOfRows()    == m_kTerrainBoard->GetNumberOfRows() );

    FillOutPalette();

    // prepare the line-of-fire template
    m_PieTemplate.PrepareScan(m_LineOfFireReach);
  }


ASE_ThreatBoard::~ASE_ThreatBoard()
  {
    delete[] m_ThreatBrushes;

    RemoveAllThreats();
  }


void ASE_ThreatBoard::FillOutPalette()
  {
    assert( m_ThreatBrushes     == 0 );
    assert( GetNumberOfValues() >  2 );
    /*!
        fill out brush table as follows:
        part1: line-of-fire
               3 x GetNumberOfValues()
                 clear terrain, all values
                 terrain 1, all values
                 terrain 2, all values
    */

    unsigned int totalnumber;
    unsigned int steps;
    steps       = GetNumberOfValues();
    totalnumber = 3 * steps;

    m_ThreatBrushes = new COLORREF[totalnumber];

    COLORREF startrgb;
    COLORREF endrgb;
    float step_r, step_g, step_b;
    float r, g, b;

    int idx;
    idx = 0;

    // fill out first steps entries
    startrgb = RGB(255,255,224); 
    endrgb   = RGB(255,255,128); 
    step_r   = static_cast<float>(GetRValue(endrgb) - GetRValue(startrgb));
    step_g   = static_cast<float>(GetGValue(endrgb) - GetGValue(startrgb));
    step_b   = static_cast<float>(GetBValue(endrgb) - GetBValue(startrgb));
    step_r  /= (steps - 1);
    step_g  /= (steps - 1);
    step_b  /= (steps - 1);

    r = GetRValue(startrgb);
    g = GetGValue(startrgb);
    b = GetBValue(startrgb);

    unsigned int step;
    for ( step = 0; step < steps; ++step, ++idx )
      {
        m_ThreatBrushes[idx] = RGB(r,g,b);
        r += step_r;
        g += step_g;
        b += step_b;
        r = min(255.0f, max(0.0f, r));
        g = min(255.0f, max(0.0f, g));
        b = min(255.0f, max(0.0f, b));
      }

    // fill out second steps entries
    startrgb = RGB(192,192,168);
    endrgb   = RGB(192,192, 96);
    step_r   = static_cast<float>(GetRValue(endrgb) - GetRValue(startrgb));
    step_g   = static_cast<float>(GetGValue(endrgb) - GetGValue(startrgb));
    step_b   = static_cast<float>(GetBValue(endrgb) - GetBValue(startrgb));
    step_r  /= (steps - 1);
    step_g  /= (steps - 1);
    step_b  /= (steps - 1);

    r = GetRValue(startrgb);
    g = GetGValue(startrgb);
    b = GetBValue(startrgb);
    for ( step = 0; step < steps; ++step, ++idx )
      {
        m_ThreatBrushes[idx] = RGB(r,g,b);
        r += step_r;
        g += step_g;
        b += step_b;
        r = min(255.0f, max(0.0f, r));
        g = min(255.0f, max(0.0f, g));
        b = min(255.0f, max(0.0f, b));
      }

    // fill out third steps entries
    startrgb = RGB(128,128,112);
    endrgb   = RGB(128,128, 64);
    step_r   = static_cast<float>(GetRValue(endrgb) - GetRValue(startrgb));
    step_g   = static_cast<float>(GetGValue(endrgb) - GetGValue(startrgb));
    step_b   = static_cast<float>(GetBValue(endrgb) - GetBValue(startrgb));
    step_r  /= (steps - 1);
    step_g  /= (steps - 1);
    step_b  /= (steps - 1);

    r = GetRValue(startrgb);
    g = GetGValue(startrgb);
    b = GetBValue(startrgb);
    for ( step = 0; step < steps; ++step, ++idx )
      {
        m_ThreatBrushes[idx] = RGB(r,g,b);
        r += step_r;
        g += step_g;
        b += step_b;
        r = min(255.0f, max(0.0f, r));
        g = min(255.0f, max(0.0f, g));
        b = min(255.0f, max(0.0f, b));
      }
  }


int   ASE_ThreatBoard::GetMaxThreatReach() const
  {
    return static_cast<int>(m_LineOfFireReach);
  }


void  ASE_ThreatBoard::MarkThreatPosition(int aRow, int aCol)
  {
    unsigned int idx;
    idx = GetIndexForRowColumn(aRow, aCol);

    // mark cell on the board
    if ( GetCellValue(idx) == m_kThreatPosition )
      return;

    SetCellValue(idx, m_kThreatPosition);

    // create threat
    ASE_Threat* threat;
    threat = new ASE_Threat(idx);
    AddThreat(threat);
 
    ComputeLinesOfFire();
  }


void  ASE_ThreatBoard::ClearThreatPosition(int aRow, int aCol)
  {
    unsigned int idx;
    idx = GetIndexForRowColumn(aRow, aCol);

    // mark cell on the board
    if ( GetCellValue(idx) == 0 )
      return;

    // remove threat
    if ( GetCellValue(idx) == m_kThreatPosition )
      RemoveThreat(idx);

    // mark cell on the board
    SetCellValue(idx, 0);

    ComputeLinesOfFire();
  }


bool  ASE_ThreatBoard::IsThreatPosition(int aRow, int aCol) const
  {
    return ( m_kThreatPosition == GetCellValue(aRow, aCol) );
  }


bool  ASE_ThreatBoard::IsThreatPosition(unsigned int anIndex) const
  {
    return ( m_kThreatPosition == GetCellValue(anIndex) );
  }


void      ASE_ThreatBoard::SetRiskModus(RiskModus aRiskModus)
  {
    assert(   ( aRiskModus == eAnyLineOfFire                 )
           || ( aRiskModus == eMinLineOfFireDistance         )
           || ( aRiskModus == eMinLineOfFireCount            )
           || ( aRiskModus == eMinLineOfFireCountAndDistance )
          );
    m_RiskModus = aRiskModus;
  }


ASE_ThreatBoard::RiskModus ASE_ThreatBoard::GetRiskModus() const
  {
    return m_RiskModus;
  }


COLORREF ASE_ThreatBoard::GetCellColorForOffset(unsigned int anIndex, CellValue anOffset) const
  {
    assert( !IsEmptyCell(anIndex) );

    CellValue value;
    value = GetCellValue(anIndex);

    unsigned int idx;
    idx  = anOffset * GetNumberOfValues();
    idx += static_cast<unsigned int>(value);

    return m_ThreatBrushes[idx];
  }


COLORREF ASE_ThreatBoard::GetLineOfFireColor() const
  {
    return m_ThreatBrushes[GetNumberOfValues() * (m_kThreatLineOfFire - 1)];
  }


unsigned int ASE_ThreatBoard::GetAssumedThreatMovement() const
  {
    return m_ThreatMovement;
  }


void         ASE_ThreatBoard::SetAssumedThreatMovement(unsigned int theAssumedThreatMovement)
  {
    m_ThreatMovement = theAssumedThreatMovement;
  }


void ASE_ThreatBoard::ClearLinesOfFire()
  {
    const unsigned int maxidx = GetNumberOfColumns() * GetNumberOfRows();

    for ( unsigned int idx = 0; idx < maxidx; ++idx )
      {
        if ( GetCellValue(idx) != m_kThreatPosition )
          {
            SetCellValue(idx, 0);
          } 
      }
  }


void ASE_ThreatBoard::InitializeLinesOfFire()
  {
    // clear old threats
    RemoveAllThreats();

    // locate new threats
	  for ( unsigned int idx = 0; 
          idx < (GetNumberOfRows() * GetNumberOfColumns()); 
          ++idx
        )
      {
        if ( GetCellValue(idx) == m_kThreatPosition )
          {
            ASE_Threat* threat;
            threat = new ASE_Threat(idx);
            AddThreat(threat);
          }
		  }
    ComputeLinesOfFire();
  }


void ASE_ThreatBoard::ComputeLinesOfFire()
  {
    ClearLinesOfFire();

	  for ( unsigned int r = 0; r < GetNumberOfRows(); ++r ) 
      {
    	  for ( unsigned int c = 0; c < GetNumberOfColumns(); ++c ) 
          {
            if ( GetCellValue(r, c) == m_kThreatPosition )
              {
                ComputeLinesOfFireForMovingThreat(r, c, m_ThreatMovement);
              }
          }
		  }

    // select appropriate cell value function
    typedef CellValue (ASE_ThreatBoard::*MemberFnDetermineRisk)(unsigned int anIndex) const;
    MemberFnDetermineRisk pMemberFunction;

    if ( m_RiskModus == eAnyLineOfFire )
      pMemberFunction = DetermineLineOfFireRiskAnyLOF;
    else
    if ( m_RiskModus == eMinLineOfFireDistance )
      pMemberFunction = DetermineLineOfFireRiskMinDistance;
    else
    if ( m_RiskModus == eMinLineOfFireCount )
      pMemberFunction = DetermineLineOfFireRiskLOFCount;
    else
    if ( m_RiskModus == eMinLineOfFireCountAndDistance )
      pMemberFunction = DetermineLineOfFireRiskDistanceLOFCount;
    else
      {
        assert( !"unsupported risk modus" );
      }

    // now start marking lines of fire
    ThreatList::const_iterator threat;
    for ( threat = m_Threats.begin(); threat != m_Threats.end(); ++threat )
      {
        const ASE_Threat::PositionsUnderFire* positionsunderfire;
        positionsunderfire = (*threat)->GetPositionsUnderFire();
        assert( !positionsunderfire->empty() );

        ASE_Threat::PositionsUnderFire::const_iterator position;
        for ( position =  positionsunderfire->begin(); 
              position != positionsunderfire->end();
              ++position
            )
          {
            if ( IsEmptyCell(*position) )
              {
                // call member function via pointer to member function
                SetCellValue(*position, (this->*pMemberFunction)(*position));
              }
          }
      }
  }


void ASE_ThreatBoard::ComputeLinesOfFireForMovingThreat
                   (unsigned int   aRow, 
                    unsigned int   aCol, 
                    unsigned int   aMaximumMovement
                   )
  {
    CoordPairList theReachablePositions;
    theReachablePositions.reserve( (2 + aMaximumMovement) * (2 + aMaximumMovement) );
    ConstructPossibleThreatPositions(aRow, aCol, aMaximumMovement, theReachablePositions);
    assert( !theReachablePositions.empty() );

    CoordPairList theCoveredPositions;
    theCoveredPositions.reserve((m_LineOfFireReach+aMaximumMovement) * (m_LineOfFireReach+aMaximumMovement) * 4);

    CoordPairList::const_iterator position;
    for ( position =  theReachablePositions.begin();
          position != theReachablePositions.end();
          ++position
        )
      {
        ComputeLinesOfFireFromCell(position->x, position->y, theCoveredPositions);
      }
    assert( !theCoveredPositions.empty() );

    // now set these locations as under fire by this threat
    ASE_Threat::PositionsUnderFire positionsUnderFire;
    positionsUnderFire.reserve(theCoveredPositions.size());

    CoordPairList::const_iterator coveredposition;
    for ( coveredposition =  theCoveredPositions.begin();
          coveredposition != theCoveredPositions.end();
          ++coveredposition
        )
      {
        unsigned int idx;
        idx = GetIndexForRowColumn(coveredposition->x, coveredposition->y);
        positionsUnderFire.push_back(idx);
      }

    ASE_Threat* threat;
    unsigned int idx;
    idx    = GetIndexForRowColumn(aRow, aCol);
    threat = GetThreatForLocation(idx);
    threat->SetPositionsUnderFire(positionsUnderFire);
    assert( threat->GetPositionsUnderFire()->size() == positionsUnderFire.size() );
  }


void ASE_ThreatBoard::ConstructPossibleThreatPositions(unsigned int   aRow, 
                                                       unsigned int   aCol, 
                                                       unsigned int   aMaximumMovement,
                                                       CoordPairList& thePositions
                                                     ) const
  {
    //! recursive implementation
    if ( aMaximumMovement == 0 )
      {
        ASE_CoordPair pair(aRow, aCol);
        thePositions.push_back(pair);
      }
    else
      {
        // move one step from aRow, aCol
        for ( int nx = -1; nx < 2; ++nx )
          {
            for ( int ny = -1; ny < 2; ++ny )
              {
                int r, c;
                r = nx + aRow;
                c = ny + aCol;
                if ( !m_kTerrainBoard->IsAccessibleBoardLocation(r, c) )
                  continue;

                ASE_CoordPair pair(r, c);
                if ( thePositions.end() != find(thePositions.begin(), thePositions.end(), pair) )
                  continue;
                // add pair and move from there as well
                ConstructPossibleThreatPositions(r, c, aMaximumMovement - 1, thePositions);
              }
          }
      }
  }


void ASE_ThreatBoard::ComputeLinesOfFireFromCell
                       (unsigned int   aRow, 
                        unsigned int   aCol,
                        CoordPairList& theCoveredPositions /*! in+out */
                       ) const
  {
    // obtain rays from template
    // for each ray
    // - test until
    //   - invalid location
    //   - blocked

    assert( !m_kTerrainBoard->IsLocationImpassable(aRow, aCol) );
    ASE_CoordPair here(aRow, aCol);
    theCoveredPositions.push_back(here);

    unsigned int nr_of_rays;
    nr_of_rays = m_PieTemplate.GetNumberOfRays();

    CoordPairList raytrace;

    for ( unsigned int ray = 0; ray < nr_of_rays; ++ray )
      {
        raytrace.clear();
        m_PieTemplate.GetRayForLocation(ray, here, raytrace);

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

            // location pos can be seen, so add it to the list of coveredpositions
            // unless it is already part of it
            if (   theCoveredPositions.end() 
                == find(theCoveredPositions.begin(), theCoveredPositions.end(), *pos)
               )
              {
                theCoveredPositions.push_back(*pos);
              } 
          }
      }
  }


ASE_Board::CellValue ASE_ThreatBoard::DetermineLineOfFireRiskAnyLOF(unsigned int anIndex) const
  {
    // set 1 if under fire from any threat, 0 otherwise
    CellValue result;
    result = 0;

    ThreatList::const_iterator threat;
    for ( threat =  m_Threats.begin();
          threat != m_Threats.end();
          ++threat
        )
      {
        if ( (*threat)->IsAbleToFireAtLocation(anIndex) )
          {
            result = 1;
            break;
          }
      }

    return result;
  }


ASE_Board::CellValue ASE_ThreatBoard::DetermineLineOfFireRiskMinDistance(unsigned int anIndex) const
  {
    // set minimum distance to threat as value
    unsigned int x;
    unsigned int y;
    GetRowAndColumnForIndex(anIndex, x, y);

    int minDistance;
    minDistance = ( m_LineOfFireReach + m_ThreatMovement ) * ( m_LineOfFireReach + m_ThreatMovement );

    // iterate over all threats, and figure out which one is the closest able to see x(,y) 
    ThreatList::const_iterator threat;
    for ( threat =  m_Threats.begin();
          threat != m_Threats.end();
          ++threat
        )
      {
        if ( (*threat)->IsAbleToFireAtLocation(anIndex) )
          {
            unsigned int x2, y2;
            GetRowAndColumnForIndex((*threat)->GetLocation(), x2, y2);

            int dx, dy;
            dx = x - x2;
            dy = y - y2;

            int dist;
            dist = dx * dx + dy * dy;
            minDistance= min(dist, minDistance);
          }
      }
    
    float value;
    value  = sqrt(static_cast<float>(minDistance));
    value /= m_LineOfFireReach;
    value  = 1 - value;
    value *= (GetNumberOfValues() - 2);
    value += 1;
    
    CellValue result;
    result = static_cast<CellValue>(value);
    assert( result < (GetNumberOfValues() - 1) );

    return result;
  }


ASE_ThreatBoard::CellValue ASE_ThreatBoard::DetermineLineOfFireRiskLOFCount(unsigned int anIndex) const
  {
    // set lof count from threat as value
    unsigned int x;
    unsigned int y;
    GetRowAndColumnForIndex(anIndex, x, y);

    CellValue lofcount;
    lofcount = 0;

    // iterate over all threats, and count the number of threats able to see position
    ThreatList::const_iterator threat;
    for ( threat =  m_Threats.begin();
          threat != m_Threats.end();
          ++threat
        )
      {
        if ( (*threat)->IsAbleToFireAtLocation(anIndex) )
          {
            lofcount++;
          }
      }
 
    lofcount = min(lofcount, m_kMaxLOFCount);

    float value;
    value  = static_cast<float>(lofcount);
    value /= m_kMaxLOFCount;
    value *= (GetNumberOfValues() - 2);

    CellValue result;
    result = static_cast<CellValue>(value);

    return result;
  }


ASE_Board::CellValue ASE_ThreatBoard::DetermineLineOfFireRiskDistanceLOFCount(unsigned int anIndex) const
  {
    // set a weighted combination of threat count and distance to threat as value
    unsigned int x;
    unsigned int y;
    GetRowAndColumnForIndex(anIndex, x, y);

    const float kMaxDistance = 1.0f + static_cast<float>( m_LineOfFireReach + m_ThreatMovement );
    const float kMaxValue    = 4.0f /* threats */ * (kMaxDistance / 3.0f);

    float threatlofs;
    threatlofs = 0;

    // iterate over all threats, and figure out which one is the closest able to see x(,y) 
    ThreatList::const_iterator threat;
    for ( threat =  m_Threats.begin();
          threat != m_Threats.end();
          ++threat
        )
      {
        if ( (*threat)->IsAbleToFireAtLocation(anIndex) )
          {
            unsigned int x2, y2;
            GetRowAndColumnForIndex((*threat)->GetLocation(), x2, y2);

            int dx, dy;
            dx = x - x2;
            dy = y - y2;

            int dist;
            dist = dx * dx + dy * dy;

            float fdist;
            fdist = sqrt(static_cast<float>(dist));
 
            assert( kMaxDistance > fdist );
            threatlofs += (kMaxDistance - fdist);
          }
      }

    
    float value;
    value  = min(1.0f, threatlofs / kMaxValue );
    value *= (GetNumberOfValues() - 3);
    value += 1;
    
    CellValue result;
    result = static_cast<CellValue>(value);
    assert( result < (GetNumberOfValues() - 1) );

    return result;
  }


float ASE_ThreatBoard::GetRiskValue(int aRow, int aCol) const
  {
    unsigned int idx;
    idx = GetIndexForRowColumn(aRow, aCol);
    return GetRiskValue(idx);
  }


float ASE_ThreatBoard::GetRiskValue(unsigned int anIndex) const
  {
    if ( m_RiskModus == eAnyLineOfFire )
      {
        return IsEmptyCell(anIndex) ? 0.0f : 1.0f;
      }
    else
      {
        CellValue value;
        value = GetCellValue(anIndex);
        if ( value == 0 )
          return 0;

        float result;
        result  = static_cast<float>(value) / GetMaxRiskValue();
        result *= (1.0f - m_kBaseRisk);
        result += m_kBaseRisk;
        return result;
      }
  }


float ASE_ThreatBoard::GetMaxRiskValue() const
  {
    if ( m_RiskModus == eAnyLineOfFire )
      return 1.0f;
    else
      {
        assert(   ( m_RiskModus == eMinLineOfFireDistance )
               || ( m_RiskModus == eMinLineOfFireCount )
               || ( m_RiskModus == eMinLineOfFireCountAndDistance )
              );
        return static_cast<float>(m_kThreatPosition);
      }
  }


void ASE_ThreatBoard::GetThreatPositions(CoordPairList& thePositions /*! out */) const
  {
    ThreatList::const_iterator threat;
    for ( threat =  m_Threats.begin();
          threat != m_Threats.end();
          ++threat
        )
      {
        int idx;
        idx = (*threat)->GetLocation();
 
        unsigned int row;
        unsigned int col;
        GetRowAndColumnForIndex(idx, row, col);
        thePositions.push_back(ASE_CoordPair(row, col));
      }
  }


const ASE_PieScan* ASE_ThreatBoard::GetLineOfSightTemplate() const
  {
    return &m_PieTemplate;
  }


void ASE_ThreatBoard::AddThreat(ASE_Threat* theThreat)
  {
    unsigned int idx;
    idx = theThreat->GetLocation();

    assert( !DoesSomeThreatOccupyLocation(idx) );
    m_Threats.push_back(theThreat);
  }


void ASE_ThreatBoard::RemoveThreat(unsigned int aLocation)
  {
    assert( DoesSomeThreatOccupyLocation(aLocation) );

    ThreatList::iterator threat;
    for ( threat =  m_Threats.begin();
          threat != m_Threats.end();
          ++threat
        )
      {
        if ( aLocation == (*threat)->GetLocation() )
          {
            delete (*threat);
            m_Threats.erase(threat);
            return;
          }
      }
    assert( !"failed to find threat for aLocation" );
  }


bool ASE_ThreatBoard::DoesSomeThreatOccupyLocation(unsigned int aLocation) const
  {
    ThreatList::const_iterator threat;
    for ( threat =  m_Threats.begin();
          threat != m_Threats.end();
          ++threat
        )
      {
        if ( aLocation == (*threat)->GetLocation() )
          return true;
      }
    return false;
  }


ASE_Threat* ASE_ThreatBoard::GetThreatForLocation(unsigned int aLocation) const
  {
    assert( DoesSomeThreatOccupyLocation(aLocation) );

    ThreatList::const_iterator threat;
    for ( threat =  m_Threats.begin();
          threat != m_Threats.end();
          ++threat
        )
      {
        if ( aLocation == (*threat)->GetLocation() )
          return (*threat);
      }
    return 0;
  }


void ASE_ThreatBoard::RemoveAllThreats()
  {
    ThreatList::iterator threat;
    for ( threat =  m_Threats.begin();
          threat != m_Threats.end();
          ++threat
        )
      {
        delete (*threat);
      }
    m_Threats.clear();
  }
