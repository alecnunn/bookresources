// +-----------------------------------------------------------------------
// |
// |                         File Name: GPindex.types.h
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the basic types and definitions used in GP
// |							to manage the GPindexes.
// | 
// +-----------------------------------------------------------------------

#ifndef GPINDEXTYPES_H
#define GPINDEXTYPES_H
#pragma once

// +---------------------------
// |	Includes
// +---------------------------
#include <deque>

// +-----------------------------------------------------------------------
// |
// |  Class       : GPunit
// |  Description : 
// |								This class is the basic "unit" of the GP. This "unit" is
// |								used for defining the position and size of the GPindex.
// |								It can be be of any type; but ALWAYS has to implement at
// |								least the following operators:
// |										operator <
// |										operator ==
// |										operator <=
// |
// +-----------------------------------------------------------------------
typedef int GPunit;

// +-----------------------------------------------------------------------
// |
// |  Struct      : GPposition
// |  Description : 
// |								This class defines the position that will be used by
// |								GPwindow for navigating and telling to the GPworld which
// |								GPtiles must be loaded and which of them must be unloaded.
// |
// +-----------------------------------------------------------------------
struct GPposition
{
	GPunit x_;
	GPunit y_;
};

// +-----------------------------------------------------------------------
// |
// |  Class       : GPunitList
// |  Description : 
// |								This class is a deque (similar to std::list with the
// |								operator[] defined ) of GPunits.
// |
// +-----------------------------------------------------------------------
typedef std::deque<GPunit> GPunitList;

#endif //GPINDEXTYPES_H
