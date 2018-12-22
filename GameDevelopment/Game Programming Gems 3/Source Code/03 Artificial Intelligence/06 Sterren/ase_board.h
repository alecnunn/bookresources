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
 *    (c) 2002: William van der Sterren
 *
 *     purpose: base board to contain data for lookup and overlay
*/


#ifndef _ASE_BOARD_H_
#define _ASE_BOARD_H_

#if _MSC_VER > 1000
#pragma once
#endif


#include <cassert>


class ASE_Board
{
  public:
    typedef unsigned char CellValue;

    ASE_Board(unsigned int    aNumberOfRows, 
              unsigned int    aNumberOfColumns,
              unsigned int    aNumberOfValues,
              const COLORREF* aValueColorPalette
             );
    virtual ~ASE_Board();

    bool            IsValidBoardLocation(int anX, int anY) const;
    bool            IsAccessibleBoardLocation(int anX, int anY) const;

    bool            IsEmptyCell(unsigned int aRow, unsigned int aCol) const;
    bool            IsEmptyCell(unsigned int anIndex) const;

    CellValue       GetCellValue(unsigned int aRow, unsigned int aCol) const;
    CellValue       GetCellValue(unsigned int anIndex) const;

    const COLORREF  GetCellColor(unsigned int aRow, unsigned int aCol) const;
    const COLORREF  GetCellColor(unsigned int anIndex) const;
    const COLORREF  GetCellColor(CellValue aCellValue) const;

    void            SetCellValue(unsigned int aRow, unsigned int aCol, CellValue aValue);
    void            SetCellValue(unsigned int anIndex, CellValue aValue);
  
    void            Clear();
                      //!< resets all values to 0

    unsigned int    GetNumberOfRows() const;
    unsigned int    GetNumberOfColumns() const;
    
    unsigned int    GetNumberOfValues() const;

    // file i/o - assume dimensions already have been read/written
    void            ReadBoard(CArchive& ar);
    void            WriteBoard(CArchive& ar) const;

    inline unsigned int GetIndexForRowColumn(unsigned int aRow, unsigned int aCol) const
      {
        assert( aRow < m_Rows );
        assert( aCol < m_Cols );
        return aRow * m_Cols + aCol;
      }

    inline void         GetRowAndColumnForIndex(unsigned int  anIndex, 
                                                unsigned int& theRow, 
                                                unsigned int& theColumn
                                               ) const
      {
        assert( anIndex < (m_Rows * m_Cols) );
        theColumn = anIndex % m_Cols;
        theRow    = anIndex / m_Cols;
        assert( anIndex == GetIndexForRowColumn(theRow, theColumn) );
      }

  protected:
    unsigned int    m_Rows;
    unsigned int    m_Cols;
    CellValue*      m_Cells;
    unsigned int    m_NumberOfValues;
    const COLORREF* m_ValueColorPalette;
    const CellValue m_kInaccessibleLocation;
};

#endif // _ASE_BOARD_H_