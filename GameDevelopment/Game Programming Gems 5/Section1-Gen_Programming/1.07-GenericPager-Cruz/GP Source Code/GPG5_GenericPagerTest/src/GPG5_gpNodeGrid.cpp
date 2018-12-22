// +-----------------------------------------------------------------------
// |
// |                         File Name: GPG5_gpNodeGrid.cpp
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPG5_gpNodeAxis Class Implementation.
// | 
// +-----------------------------------------------------------------------

// +---------------------------
// |	Includes
// +---------------------------
#include <GPG5_gpPch.h>
#include <GPG5_gpNodeGrid.h>

// +---------------------------
// |	Constructor & Destructor
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeGrid::GPG5_gpNodeGrid
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPG5_gpNodeGrid::GPG5_gpNodeGrid():
GPG5_gpNode(),

rows_ (10),
cols_ (10),

incrRows_ (1.0f),
incrCols_	(1.0f)
{
	rowLength_ = cols_*incrCols_;
	colLength_ = rows_*incrRows_;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeGrid::~GPG5_gpNodeGrid
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPG5_gpNodeGrid::~GPG5_gpNodeGrid()
{
}

// +---------------------------
// |	Size
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeGrid::SetSize
// | Description     : 
// |
// | Parameters : 
// |   rows          : 
// |   cols          : 
// |   incrRows      : 
// |   incrCols      : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeGrid::SetSize(int rows, int cols, float incrRows, float incrCols)
{
	rows_ = rows;
	cols_ = cols;
	incrRows_ = incrRows;
	incrCols_ = incrCols;

	rowLength_ = cols_*incrCols_;
	colLength_ = rows_*incrRows_;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeGrid::GetSize
// | Description     : 
// |
// | Parameters : 
// |   rows          : 
// |   cols          : 
// |   incrRows      : 
// |   incrCols      : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeGrid::GetSize(int& rows, int& cols, float& incrRows, float& incrCols) const
{
	rows = rows_;
	cols = cols_;
	incrRows = incrRows_;
	incrCols = incrCols_;
}

// +--------------------------------------------------
// |	Virtual Methods of GPG5_gpNode
// +--------------------------------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeGrid::Process
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeGrid::Process()
{
	glBegin(GL_LINES);
		for (int row=0;row<=rows_;row++)
		{
			glVertex3f(0.0f,row*incrRows_,0.0f);
			glVertex3f(rowLength_,row*incrRows_,0.0f);
		}
	glEnd();


	glBegin(GL_LINES);
		for (int col=0;col<=cols_;col++)
		{
			glVertex3f(col*incrCols_,0.0f,0.0f);
			glVertex3f(col*incrCols_,colLength_,0.0f);
		}
	glEnd();
}
