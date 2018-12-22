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


#ifndef _ASE_LOSAPPROXBOARD_H_
#define _ASE_LOSAPPROXBOARD_H_

#if _MSC_VER > 1000
#pragma once
#endif


#include "ase_board.h"
#include "ase_losscan.h"
#include "ase_coordpair.h"
#include <vector>

// forward decls
class ASE_TerrainBoard;
class ASE_ThreatBoard;


class ASE_LOSApproximationBoard : public ASE_Board
{
  public:
    ASE_LOSApproximationBoard(unsigned int            aNumberOfRows, 
                              unsigned int            aNumberOfColumns,
                              const ASE_TerrainBoard* theTerrainBoard,
                              const ASE_ThreatBoard*  theThreatBoard
                             );
    virtual ~ASE_LOSApproximationBoard();

    //! inits board and change area
    void         Clear();

    //! signalling of terrain/threat modification
    void         ApplyTerrainModification(int anX, int aY);
    void         ApplyThreatModification(int anX, int aY);

    //! approximation
    void         ComputeLinesOfFireApproximation();

    float        GetRiskValue(int aRow, int aCol) const;
    float        GetRiskValue(unsigned int anIndex) const;

    //! obtaining values for drawing
    COLORREF     GetCellColorForOffset(unsigned int anIndex, CellValue anOffset) const;

  protected:
    void         PrepareListOfObstacles();
    bool         IsAnyObstacleWithinRange(int x, int y, int theRange) const;
    bool         IsAnyObstacleWithinRangeAndSector(int x, int y, int theRange, unsigned int aSector) const;
    unsigned int GetDistanceToNearestObstacleWithinRangeAndSector(int x, int y, int theRange, unsigned int aSector) const;
    
    void         SetLocationAsLackingAnyCover(int x, int y);
    void         ApproximateLOFForLocationBySector(int x, int y); 

    void         ComputeSectorLOSForLocation(int x, int y, unsigned int aSector, unsigned int aRange);

    void         UpdateBoard();  

  protected:
    void         InitChangeRange();
    void         ResetChangeRange();
    void         GetChangeRange(int& aMinX, int& aMinY, int& aMaxX, int& aMaxY);
    bool         DidAnythingChange() const;

    bool         IsPositionInRangeOfAnyThreat(int anX, int aY, int& /*!out*/ theDistance);
                   //<! returns the distance that the nearest threat can see beyond (anX, aY)

  private:
    typedef std::vector<ASE_LineOfSightScan> LineOfSightScanInfos;
    LineOfSightScanInfos    m_LineOfSightScanInfos;

    const ASE_TerrainBoard* m_kTerrainBoard;                   //!< terrain object
    const ASE_ThreatBoard*  m_kThreatBoard;                    //!< threat object

    CoordPairList           m_ObstaclePositions;

    const CellValue         m_kDangerFromFarThreats;
    const CellValue         m_kDangerFromNearThreats;
    const CellValue         m_kDangerFromNearAndFarThreats;

    int                     m_MinThreatX;
    int                     m_MinThreatY;
    int                     m_MaxThreatX;
    int                     m_MaxThreatY;
    int                     m_MinTerrainX;
    int                     m_MinTerrainY;
    int                     m_MaxTerrainX;
    int                     m_MaxTerrainY;

    static COLORREF         m_LOSApproximationBrushes[];
};


#endif // _ASE_LOSAPPROXBOARD_H_