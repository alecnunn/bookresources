// +-----------------------------------------------------------------------
// |
// |                         File Name: GPG5_gpNodeGrid.h
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPG5_gpNodeGrid Class Header.
// | 
// +-----------------------------------------------------------------------

#ifndef GPG5_GPNODEGRID_H
#define GPG5_GPNODEGRID_H
#pragma once

// +---------------------------
// |	Includes
// +---------------------------
#include<GPG5_gpNode.h>

// +-----------------------------------------------------------------------
// |
// |  Class       : GPG5_gpNodeGrid
// |  Description : 
// |								This class draws and manages a Grid
// |
// +-----------------------------------------------------------------------
class GPG5_gpNodeGrid: public GPG5_gpNode
{
	public:
		// +---------------------------
		// |	Constructor & Destructor
		// +---------------------------
		GPG5_gpNodeGrid();
		virtual ~GPG5_gpNodeGrid();

		// +---------------------------
		// |	Size
		// +---------------------------
		void SetSize(int rows, int cols, float incrRows, float incrCols);
		void GetSize(int& rows, int& cols, float& incrRows, float& incrCols) const;

		// +--------------------------------------------------
		// |	Virtual Methods of GPG5_gpNode
		// +--------------------------------------------------
		virtual void Process();

	protected:
		// +---------------------------
		// |	Size
		// +---------------------------
		int rows_;
		int cols_;

		float incrRows_;
		float incrCols_;
		
		float rowLength_;
		float colLength_;

	private:
		
};

#endif //GPG5_GPNODEGRID_H