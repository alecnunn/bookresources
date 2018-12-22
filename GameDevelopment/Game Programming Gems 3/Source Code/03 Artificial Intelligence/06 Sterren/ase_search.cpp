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
 *     purpose: search-space lookup table and overlay
*/


#include "stdafx.h"
#include "ase_search.h"

#include "ase_terrain.h"

#include <cassert>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


COLORREF ASE_SearchBoard::m_SearchBrushes[] =
  {
    RGB(255,220,220),  //!< open, clear terrain
    RGB(255,180,190),  //!< 
    RGB(255,120,120),  //!< 
    RGB(000,000,000),  //!< not used?

    RGB(220,220,255),  //!< closed, clear terrain
    RGB(180,180,255),  //!< 
    RGB(120,120,255),  //!< 
    RGB(000,000,000),  //!< not used?
  };


ASE_SearchBoard::ASE_SearchBoard(unsigned int            aNumberOfRows, 
                                 unsigned int            aNumberOfColumns,
                                 const ASE_TerrainBoard* theTerrain
                                )
  : ASE_Board(aNumberOfRows, aNumberOfColumns, 3, m_SearchBrushes),
    m_kTerrainBoard(theTerrain),
    m_kInOpenList(1),
    m_kInClosedList(2)
  {
    assert( m_kTerrainBoard );
    assert( GetNumberOfColumns() == m_kTerrainBoard->GetNumberOfColumns() );
    assert( GetNumberOfRows()    == m_kTerrainBoard->GetNumberOfRows() );
  }


ASE_SearchBoard::~ASE_SearchBoard()
  {
  }


void ASE_SearchBoard::MarkAsOpen(int x, int y)
  {
    SetCellValue(x, y, m_kInOpenList);
  }


void ASE_SearchBoard::MarkAsClosed(int x, int y)
  {
    SetCellValue(x, y, m_kInClosedList);
  }


COLORREF ASE_SearchBoard::GetCellColorForOffset(unsigned int anIndex, CellValue anOffset) const
  {
    assert( !IsEmptyCell(anIndex) );

    unsigned int idx;
    idx  = GetCellValue(anIndex);
    idx  = 4 * (idx - 1);
    idx += static_cast<unsigned int>(anOffset);

    assert( idx < sizeof(m_SearchBrushes) );

    return m_SearchBrushes[idx];
  }
