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

#ifndef _ASE_THREATBOARD_H_
#define _ASE_THREATBOARD_H_

#if _MSC_VER > 1000
#pragma once
#endif


#include "ase_board.h"
#include "ase_threat.h"
#include "ase_coordpair.h"
#include "ase_piescan.h"
#include <vector>

// forward decls
class ASE_TerrainBoard;


class ASE_ThreatBoard : public ASE_Board
{
  public:
    ASE_ThreatBoard(unsigned int            aNumberOfRows, 
                    unsigned int            aNumberOfColumns,
                    const ASE_TerrainBoard* theTerrain
                   );
    virtual ~ASE_ThreatBoard();

    //! manipulation
    void  MarkThreatPosition(int aRow, int aCol);
    void  ClearThreatPosition(int aRow, int aCol);

    bool  IsThreatPosition(int aRow, int aCol) const;
    bool  IsThreatPosition(unsigned int anIndex) const;

    // threat positions
    void  GetThreatPositions(CoordPairList& thePositions /*! out */) const;

    // los template
    const ASE_PieScan* GetLineOfSightTemplate() const;

    //! risk modus
    enum RiskModus
      { 
        eAnyLineOfFire,
        eMinLineOfFireDistance,
        eMinLineOfFireCount,
        eMinLineOfFireCountAndDistance
      };
    void         SetRiskModus(RiskModus aRiskModus);
    RiskModus    GetRiskModus() const;

    float        GetRiskValue(int aRow, int aCol) const;
    float        GetRiskValue(unsigned int anIndex) const;
    float        GetMaxRiskValue() const;

    int          GetMaxThreatReach() const;

    //! initialization upon map load
    void         InitializeLinesOfFire();
    //! line-of-fire expansion
    void         ComputeLinesOfFire();

    // threat movement
    unsigned int GetAssumedThreatMovement() const;
    void         SetAssumedThreatMovement(unsigned int theAssumedThreatMovement);

    // visualization
    COLORREF     GetCellColorForOffset(unsigned int anIndex, CellValue anOffset) const;

    COLORREF     GetLineOfFireColor() const;

    // support for LOS approximation board
    void ComputeLinesOfFireFromCell(unsigned int   aRow, 
                                    unsigned int   aCol,
                                    CoordPairList& theCoveredPositions /*! in+out */
                                   ) const;

  protected:
    void         FillOutPalette();

  protected:
    void ClearLinesOfFire();
    void ConstructPossibleThreatPositions(unsigned int   aRow, 
                                          unsigned int   aCol, 
                                          unsigned int   aMaximumMovement,
                                          CoordPairList& thePositions
                                         ) const;
    void ComputeLinesOfFireForMovingThreat(unsigned int   aRow, 
                                           unsigned int   aCol, 
                                           unsigned int   aMaximumMovement
                                          );
    void ComputeLinesOfFireFromCellTowardsCell(float          aSourceXPos, 
                                               float          aSourceYPos,
                                               float          aDestinationXPos, 
                                               float          aDestinationYPos,
                                               float          aMaximumDistance,
                                               CoordPairList& theCoveredPositions /*! in+out */
                                              ) const;

    CellValue DetermineLineOfFireRiskAnyLOF(unsigned int anIndex) const;
    CellValue DetermineLineOfFireRiskMinDistance(unsigned int anIndex) const;
    CellValue DetermineLineOfFireRiskLOFCount(unsigned int anIndex) const;
    CellValue DetermineLineOfFireRiskDistanceLOFCount(unsigned int anIndex) const;

  protected: // keep track of actual locations covered by threat
    typedef std::vector<ASE_Threat*> ThreatList;
    ThreatList         m_Threats;

    void AddThreat(ASE_Threat* theThreat);
    void RemoveThreat(unsigned int aLocation);
    bool DoesSomeThreatOccupyLocation(unsigned int aLocation) const;

    ASE_Threat* GetThreatForLocation(unsigned int aLocation) const;

    void ObtainAllThreatsFromBoard();
    void RemoveAllThreats();

  protected: 
    ASE_PieScan             m_PieTemplate;     //!< object containing template rays

  protected:
    RiskModus               m_RiskModus;       //!< determines what cell value means
    const ASE_TerrainBoard* m_kTerrainBoard;   //!< terrain object
    float                   m_LineOfFireReach; //!< maximum reach of fire/sight
    float                   m_RiskFactor;      //!< multiplier for costs of being in lof

  private:
    const  CellValue        m_kThreatPosition;
    const  CellValue        m_kThreatLineOfFire;

    const  float            m_kBaseRisk;       //!< minimum risk value for being in the lof
    const  CellValue        m_kMaxLOFCount;    //!< maximum number of lof counts that is considered

    unsigned int            m_ThreatMovement;

    COLORREF*               m_ThreatBrushes;
};


#endif // _ASE_THREATBOARD_H_