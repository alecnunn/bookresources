// +-----------------------------------------------------------------------
// |
// |                         File Name: GPwindow.types.h
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the types and definitions used in GP
// |							to manage the GPwindows.
// | 
// +-----------------------------------------------------------------------

#ifndef GPWINDOWTYPES_H
#define GPWINDOWTYPES_H
#pragma once

#include <list>

class GPwindow;

// +-----------------------------------------------------------------------
// |
// |  Class       : GPwindowList
// |  Description : 
// |								This class is a list of GPwindows. Each GPtile has one.
// |
// +-----------------------------------------------------------------------
typedef std::list<GPwindow*> GPwindowList;

#endif // GPWINDOWTYPES_H
