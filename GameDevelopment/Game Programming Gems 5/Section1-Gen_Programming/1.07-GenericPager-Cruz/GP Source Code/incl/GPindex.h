// +-----------------------------------------------------------------------
// |
// |                         File Name: GPindex.h
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPindex Class Header.
// | 
// +-----------------------------------------------------------------------

#ifndef GPINDEX_H
#define GPINDEX_H
#pragma once

// +---------------------------
// |	Includes
// +---------------------------
#include <GPdefs.h>
#include <GPindex.types.h>

// +-----------------------------------------------------------------------
// |
// |  Class       : GPindex
// |  Description : 
// |								This class is the main class of GP that allows us to navigate
// |								through the world without having to do a "REAL" division of it
// |								in blocks, just defining it.
// |
// +-----------------------------------------------------------------------
class _GP GPindex
{
	public:
		// +---------------------------
		// |	Constructors & Destructor
		// +---------------------------
		GPindex();
		GPindex(const GPunit& x, const GPunit& y, const GPunit& width, const GPunit& height);
		GPindex(const GPindex& ref);
		virtual ~GPindex();

		// +---------------------------
		// |	Position
		// +---------------------------
		void SetX(const GPunit& x);
		const GPunit& GetX() const;
		void SetY(const GPunit& y);
		const GPunit& GetY() const;

		// +---------------------------
		// |	Size
		// +---------------------------
		void SetWidth(const GPunit& width);
		const GPunit& GetWidth() const;
		void SetHeight(const GPunit& height);
		const GPunit& GetHeight() const;

		// +---------------------------
		// |	Index Control
		// +---------------------------
		virtual GPunit PreviousX(int numPrevious= 1);
		virtual GPunit PreviousY(int numPrevious = 1);
		virtual GPunit NextX(int numNext = 1);
		virtual GPunit NextY(int numNext = 1);
		virtual GPindex GetIndex(const GPunit& x, const GPunit& y) const;

		// +---------------------------
		// |	Dimensions
		// +---------------------------
		int GetDimensions();

		// +---------------------------
		// |	Index Operators
		// +---------------------------
	  bool operator <  (const GPindex& indexRH) const;
	  bool operator == (const GPindex& indexRH) const;
	  bool operator <= (const GPindex& indexRH) const;
		
	protected:
		// +---------------------------
		// |	Position
		// +---------------------------
		GPunit x_;
		GPunit y_;

		// +---------------------------
		// |	Size
		// +---------------------------
		GPunit width_;
		GPunit height_;

	private:

};

#endif //GPINDEX_H
