/* Copyright (C) William van der Sterren, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) William van der Sterren, 2002"
 */

/* Copyright (C) James Matthews, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) James Matthews, 2001"
 */

/***************************************************************************  
 *                                                                            
 *     purpose: terrain lookup table and overlay
*/

#ifndef _ASE_TERRAINBOARD_H_
#define _ASE_TERRAINBOARD_H_

#if _MSC_VER > 1000
#pragma once
#endif


#include "ase_board.h"


class ASE_TerrainBoard : public ASE_Board
{
  public:
    ASE_TerrainBoard(unsigned int    aNumberOfRows, 
                     unsigned int    aNumberOfColumns
                    );
    virtual ~ASE_TerrainBoard();

    bool  IsLocationImpassable(int aRow, int aCol) const;

    //! movement check
    bool  IsValidTerrainMove(int aSourceRow,      int aSourceCol,
                             int aDestinationRow, int aDestinationCol
                            ) const;

    //! movement costs
    float GetMovementCostsCellBased(int aSourceRow,      int aSourceCol,
                                    int aDestinationRow, int aDestinationCol
                                   ) const;
    float GetMovementCostsVectorBased(int aSourceRow,      int aSourceCol,
                                      int aDestinationRow, int aDestinationCol
                                     ) const;

    //! interpret terrain and distance to result in traveltime
    float GetTravelTime(int aSourceRow,      int aSourceCol,
                        int aDestinationRow, int aDestinationCol
                       ) const;

    //! file i/o
    CArchive& Read(CArchive& ar);
    CArchive& Write(CArchive& ar) const;

  private:
    const  CellValue   m_kImpassableTerrain;

    static COLORREF    m_BoardBrushes[];
};


#endif // _ASE_TERRAINBOARD_H_