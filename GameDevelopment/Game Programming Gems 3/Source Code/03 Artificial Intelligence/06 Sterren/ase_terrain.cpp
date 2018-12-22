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
 *     purpose: terrain lookup table and overlay
*/

#include "stdafx.h"
#include "ase_terrain.h"

#include <cmath>
#include <cassert>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


COLORREF ASE_TerrainBoard::m_BoardBrushes[] =
  {
    RGB(255,255,255),  //!< cost 0 - white
    RGB(192,192,192),  //!< cost 1 - light grey
    RGB(128,128,128),  //!< cost 2 - grey
	  RGB(000,000,000)   //!< impassable, not a valid move
  };


ASE_TerrainBoard::ASE_TerrainBoard(unsigned int    aNumberOfRows, 
                                   unsigned int    aNumberOfColumns
                                  )
  : ASE_Board(aNumberOfRows, aNumberOfColumns, 4, m_BoardBrushes),
    m_kImpassableTerrain(3)
  {
  }


ASE_TerrainBoard::~ASE_TerrainBoard()
  {
  }


bool ASE_TerrainBoard::IsLocationImpassable(int aRow, int aCol) const
  {
    assert( IsValidBoardLocation(aRow, aCol) );

    return ( m_kImpassableTerrain == GetCellValue(aRow, aCol) );
  }


bool ASE_TerrainBoard::IsValidTerrainMove(int aSourceRow,      int aSourceCol,
                                     int aDestinationRow, int aDestinationCol
                                    ) const
  {
    if ( !IsValidBoardLocation(aDestinationRow, aDestinationCol) )
      return false;

    if ( GetCellValue(aDestinationRow, aDestinationCol) == m_kImpassableTerrain )
      return false;

    // check for moves from impassable terrain
    assert( GetCellValue(aSourceRow, aSourceCol) != m_kImpassableTerrain );

    return true;    
  }


float ASE_TerrainBoard::GetMovementCostsCellBased(int aSourceRow,      int aSourceCol,
                                             int aDestinationRow, int aDestinationCol
                                            ) const
  {
    float costs;
    // no need to split movement out in half the travel in source cell,
    // and half the travel in destination cell, since both distances
    // will always be equal, even for diagonal movements
    costs = 1.0f + GetCellValue(aDestinationRow, aDestinationCol);

    return costs;
  }


float ASE_TerrainBoard::GetMovementCostsVectorBased(int aSourceRow,      int aSourceCol,
                                               int aDestinationRow, int aDestinationCol
                                              ) const
  {
    float distance;
    distance = static_cast<float>
                 (  abs(aSourceRow - aDestinationRow)
                  + abs(aSourceCol - aDestinationCol)
                 );
    distance = static_cast<float>(sqrt(distance));

    int   terrain_from;
    int   terrain_to;
    terrain_from = GetCellValue(aSourceRow, aSourceCol);
    terrain_to   = GetCellValue(aDestinationRow, aDestinationCol);
    
    float terrain;
    terrain = static_cast<float>(terrain_from + terrain_to + 2); 

    float costs;
    costs = terrain / 2.0f * distance;

    return costs;
  }


float ASE_TerrainBoard::GetTravelTime(int aSourceRow,      int aSourceCol,
                                 int aDestinationRow, int aDestinationCol
                                ) const
  {
    return GetMovementCostsVectorBased(aSourceRow, aSourceCol, aDestinationRow, aDestinationCol);
  }

