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

#ifndef _ASE_SEARCHBOARD_H_
#define _ASE_SEARCHBOARD_H_

#if _MSC_VER > 1000
#pragma once
#endif


#include "ase_board.h"

class ASE_TerrainBoard;


class ASE_SearchBoard : public ASE_Board
{
  public:
    ASE_SearchBoard(unsigned int            aNumberOfRows, 
                    unsigned int            aNumberOfColumns,
                    const ASE_TerrainBoard* theTerrain
                   );
    virtual  ~ASE_SearchBoard();

    void     MarkAsOpen(int x, int y);
    void     MarkAsClosed(int x, int y);

    COLORREF GetCellColorForOffset(unsigned int anIndex, CellValue anOffset) const;

  protected:
    const ASE_TerrainBoard* m_kTerrainBoard;        //!< terrain object

  private:
    const  CellValue        m_kInOpenList;
    const  CellValue        m_kInClosedList;

    static COLORREF         m_SearchBrushes[];
};

#endif // _ASE_SEARCHBOARD_H_