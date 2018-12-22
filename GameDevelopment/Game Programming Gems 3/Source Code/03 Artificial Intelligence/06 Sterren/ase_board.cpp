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
 *     purpose: base board to contain data for lookup and overlay
*/


#include "stdafx.h"
#include "ase_board.h"

#include <cassert>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


ASE_Board::ASE_Board(unsigned int    aNumberOfRows, 
                     unsigned int    aNumberOfColumns,
                     unsigned int    aNumberOfValues,
                     const COLORREF* aValueColorPalette
                    )
  : m_Rows(aNumberOfRows),
    m_Cols(aNumberOfColumns),
    m_NumberOfValues(aNumberOfValues),
    m_ValueColorPalette(aValueColorPalette),
    m_kInaccessibleLocation(aNumberOfValues - 1)
  {
    assert( m_Rows > 0 );
    assert( m_Cols > 0 );
    m_Cells = new CellValue[m_Rows * m_Cols];

    memset(m_Cells, m_Rows*m_Cols, sizeof(CellValue));

    assert( m_NumberOfValues );
  }


ASE_Board::~ASE_Board()
  {
    delete[] m_Cells;
  }


bool ASE_Board::IsValidBoardLocation(int anX, int aY) const
  {
    return (   ( anX >= 0 )
            && ( anX <  static_cast<int>(m_Rows) )
            && ( aY  >= 0 )
            && ( aY  <  static_cast<int>(m_Cols) )
           ); 
  }

bool ASE_Board::IsAccessibleBoardLocation(int anX, int anY) const
  {
    return (   ( IsValidBoardLocation(anX, anY) )
            && ( GetCellValue(anX, anY) != m_kInaccessibleLocation )
           );
  }

bool ASE_Board::IsEmptyCell(unsigned int aRow, unsigned int aCol) const
  {
    unsigned int idx;
    idx = GetIndexForRowColumn(aRow, aCol);
    return ( m_Cells[idx] == 0 );
  }


bool ASE_Board::IsEmptyCell(unsigned int anIndex) const
  {
    assert( anIndex < (GetNumberOfColumns() * GetNumberOfRows()) );
    return ( m_Cells[anIndex] == 0 );
  }


ASE_Board::CellValue ASE_Board::GetCellValue(unsigned int aRow, unsigned int aCol) const
  {
    unsigned int idx;
    idx = GetIndexForRowColumn(aRow, aCol);
    assert( m_Cells[idx] < m_NumberOfValues );
    return m_Cells[idx];
  }


ASE_Board::CellValue ASE_Board::GetCellValue(unsigned int anIndex) const
  {
    assert( anIndex < (GetNumberOfColumns() * GetNumberOfRows()) );
    assert( m_Cells[anIndex] < m_NumberOfValues );
    return m_Cells[anIndex];
  }


const COLORREF ASE_Board::GetCellColor(unsigned int aRow, unsigned int aCol) const
  {
    return m_ValueColorPalette[GetCellValue(aRow, aCol)];
  }


const COLORREF ASE_Board::GetCellColor(unsigned int anIndex) const
  {
    return m_ValueColorPalette[GetCellValue(anIndex)];
  }


const COLORREF ASE_Board::GetCellColor(CellValue aCellValue) const
  {
    assert( aCellValue < m_NumberOfValues );
    return m_ValueColorPalette[aCellValue];
  }


void ASE_Board::SetCellValue(unsigned int aRow, unsigned int aCol, CellValue aValue)
  {
    assert( aValue < m_NumberOfValues );

    unsigned int idx;
    idx = GetIndexForRowColumn(aRow, aCol);
    m_Cells[idx] = aValue;
  }


void ASE_Board::SetCellValue(unsigned int anIndex, CellValue aValue)
  {
    assert( anIndex < m_Rows * m_Cols );
    assert( aValue < m_NumberOfValues );
    m_Cells[anIndex] = aValue;
  }


void ASE_Board::Clear()
  {
    unsigned int maxidx;
    maxidx = m_Rows * m_Cols;
    for ( unsigned idx = 0; idx < maxidx; ++idx )
      {
        m_Cells[idx] = 0;
      }
  }


unsigned int ASE_Board::GetNumberOfRows() const
  {
    return m_Rows;
  }


unsigned int ASE_Board::GetNumberOfColumns() const
  {
    return m_Cols;
  }


unsigned int ASE_Board::GetNumberOfValues() const
  {
    return m_NumberOfValues;
  }


void ASE_Board::ReadBoard(CArchive& ar)
  {
    //! assumes board already has been created with the right dimensions
		for ( unsigned int row = 0; row < GetNumberOfRows(); ++row ) 
      {
			  for ( unsigned int col = 0; col < GetNumberOfColumns(); ++col ) 
          {
            CellValue c;
            ar >> c;
            SetCellValue(row, col, c);
          }
      } 
  }


void ASE_Board::WriteBoard(CArchive& ar) const
  {
    //! assumes the dimensions already have been written to the archive
		for ( unsigned int row = 0; row < GetNumberOfRows(); ++row ) 
      {
			  for ( unsigned int col = 0; col < GetNumberOfColumns(); ++col ) 
          {
            CellValue c;
            c = GetCellValue(row, col);
            ar << c;
          }
      } 
  }
