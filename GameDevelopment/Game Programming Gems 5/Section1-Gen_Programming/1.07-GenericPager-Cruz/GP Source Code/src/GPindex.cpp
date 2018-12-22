// +-----------------------------------------------------------------------
// |
// |                         File Name: GPindex.cpp
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPindex Class Implementation.
// | 
// +-----------------------------------------------------------------------

// +---------------------------
// |	Includes
// +---------------------------
#include <GPpch.h>
#include <GPindex.h>

// +---------------------------
// |	Constructors & Destructor
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPindex::GPindex
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPindex::GPindex()
{
}

// +-------------------------------------------------------------
// |
// | Function        : GPindex::GPindex
// | Description     : 
// |
// | Parameters : 
// |   x             : 
// |   y             : 
// |   width         : 
// |   height        : 
// | 
// +-------------------------------------------------------------
GPindex::GPindex(const GPunit& x, const GPunit& y, const GPunit& width, const GPunit& height):
x_			( x ),
y_			( y ),
width_	( width ),
height_ ( height )
{
}


// +-------------------------------------------------------------
// |
// | Function        : GPindex::GPindex
// | Description     : 
// |
// | Parameters : 
// |   ref           : 
// | 
// +-------------------------------------------------------------
GPindex::GPindex(const GPindex& ref):
x_			( ref.x_ ),
y_			( ref.y_ ),
width_	( ref.width_ ),
height_ ( ref.height_ )
{
}

// +-------------------------------------------------------------
// |
// | Function        : GPindex::~GPindex
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPindex::~GPindex()
{
}

// +---------------------------
// |	Position
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPindex::SetX
// | Description     : 
// |
// | Parameters : 
// |   x             : 
// | 
// +-------------------------------------------------------------
void GPindex::SetX(const GPunit& x)
{
	x_ = x;
}

// +-------------------------------------------------------------
// |
// | Function        : GPindex::GetX
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
const GPunit& GPindex::GetX() const
{
	return x_;
}

// +-------------------------------------------------------------
// |
// | Function        : GPindex::SetY
// | Description     : 
// |
// | Parameters : 
// |   y             : 
// | 
// +-------------------------------------------------------------
void GPindex::SetY(const GPunit& y)
{
	y_ = y;
}

// +-------------------------------------------------------------
// |
// | Function        : GPindex::GetY
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
const GPunit& GPindex::GetY() const
{
	return y_;
}

// +---------------------------
// |	Size
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPindex::SetWidth
// | Description     : 
// |
// | Parameters : 
// |   width         : 
// | 
// +-------------------------------------------------------------
void GPindex::SetWidth(const GPunit& width)
{
	width_ = width;
}

// +-------------------------------------------------------------
// |
// | Function        : GPindex::GetWidth
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
const GPunit& GPindex::GetWidth() const
{
	return width_;
}

// +-------------------------------------------------------------
// |
// | Function        : GPindex::SetHeight
// | Description     : 
// |
// | Parameters : 
// |   height        : 
// | 
// +-------------------------------------------------------------
void GPindex::SetHeight(const GPunit& height)
{
	height_ = height;
}

// +-------------------------------------------------------------
// |
// | Function        : GPindex::GetHeight
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
const GPunit& GPindex::GetHeight() const
{
	return height_;
}

// +---------------------------
// |	Index Control
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPindex::PreviousX
// | Description     : 
// |
// | Parameters : 
// |   numPrevious   : 
// | 
// +-------------------------------------------------------------
GPunit GPindex::PreviousX(int numPrevious)
{
	GPunit previousX = x_;
	while ( numPrevious-- )
	{
		previousX -= width_;
	}
	return previousX;
}

// +-------------------------------------------------------------
// |
// | Function        : GPindex::PreviousY
// | Description     : 
// |
// | Parameters : 
// |   numPrevious   : 
// | 
// +-------------------------------------------------------------
GPunit GPindex::PreviousY(int numPrevious)
{
	GPunit previousY = y_;
	while ( numPrevious-- )
	{
		previousY -= height_;
	}
	return previousY;
}

// +-------------------------------------------------------------
// |
// | Function        : GPindex::NextX
// | Description     : 
// |
// | Parameters : 
// |   numNext       : 
// | 
// +-------------------------------------------------------------
GPunit GPindex::NextX(int numNext)
{
	GPunit nextX = x_;
	while ( numNext-- )
	{
		nextX += width_;
	}
	return nextX;
}

// +-------------------------------------------------------------
// |
// | Function        : GPindex::NextY
// | Description     : 
// |
// | Parameters : 
// |   numNext       : 
// | 
// +-------------------------------------------------------------
GPunit GPindex::NextY(int numNext)
{
	GPunit nextY = y_;
	while ( numNext-- )
	{
		nextY += height_;
	}
	return nextY;
}

// +-------------------------------------------------------------
// |
// | Function        : GPindex::GetIndex
// | Description     : 
// |
// | Parameters : 
// |   x             : 
// |   y             : 
// | 
// +-------------------------------------------------------------
GPindex GPindex::GetIndex(const GPunit& x, const GPunit& y) const
{
	GPunit indexX = ( (GPunit) ( x / width_  ) ) * width_;
	GPunit indexY = ( (GPunit) ( y / height_ ) ) * height_;
	
	GPindex indexPos(indexX, indexY, width_, height_ );
	
	return indexPos;
}

// +---------------------------
// |	Dimensions
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPindex::GetDimensions
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
int GPindex::GetDimensions()
{
	return 2;
}

// +---------------------------
// |	Index Operators
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPindex::operator < 
// | Description     : 
// |
// | Parameters : 
// |   indexRH       : 
// | 
// +-------------------------------------------------------------
bool GPindex::operator < (const GPindex& indexRH) const
{
	if ( y_ < indexRH.y_ )
	{
		return true;
	}
	else if ( y_ == indexRH.y_ )
	{
		if ( x_ < indexRH.x_ )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else //if ( y_ > indexRH.y_ )
	{
		return false;
	}
}

// +-------------------------------------------------------------
// |
// | Function        : GPindex::operator == 
// | Description     : 
// |
// | Parameters : 
// |   indexRH       : 
// | 
// +-------------------------------------------------------------
bool GPindex::operator == (const GPindex& indexRH) const
{
	if (
			( y_ == indexRH.y_ ) &&
			( x_ == indexRH.x_ )
		 )
	{
		return true;
	}
	else
	{
		return false;
	}
}

// +-------------------------------------------------------------
// |
// | Function        : GPindex::operator <= 
// | Description     : 
// |
// | Parameters : 
// |   indexRH       : 
// | 
// +-------------------------------------------------------------
bool GPindex::operator <= (const GPindex& indexRH) const
{
	if ( 
			((*this) == indexRH) ||
			((*this) < indexRH)
		 )
	{
		return true;
	}
	else
	{
		return false;
	}
}
