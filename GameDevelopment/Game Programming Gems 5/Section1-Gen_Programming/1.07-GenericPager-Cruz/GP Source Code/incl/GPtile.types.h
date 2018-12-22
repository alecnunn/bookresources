// +-----------------------------------------------------------------------
// |
// |                         File Name: GPtile.types.h
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the types and definitions used in GP
// |							to manage the GPtiles.
// | 
// +-----------------------------------------------------------------------

#ifndef GPTILETYPES_H
#define GPTILETYPES_H
#pragma once

// +---------------------------
// |	Disable Warnings
// +---------------------------
#pragma warning (disable: 4251)
#pragma warning (disable: 4786)

// +---------------------------
// |	Includes
// +---------------------------
#include <GPIndex.h>
#include <map>

// +---------------------------
// |	Forward Declarations
// +---------------------------
class GPtile;

// +-----------------------------------------------------------------------
// |
// |  Enum        : GPtileState
// |  Description : 
// |								This enum defines the different states of each GPtile.
// |								The GPtile only can be in one at the same time.
// |
// |
// +-----------------------------------------------------------------------
enum GPtileState
{
	GP_LOADING,
	GP_UNLOADING,
	GP_LOADED,
	GP_UNLOADED
};

// +-----------------------------------------------------------------------
// |
// |  Enum				: GRPtileAction
// |  Description : 
// |								This enum defines the different actions to be performed
// |								with a GPtile by the GPworld in the load/unload process
// |								loop.
// |
// +-----------------------------------------------------------------------
enum GRPtileAction
{
	GP_FOR_LOAD,
	GP_FOR_UNLOAD
};

// +-----------------------------------------------------------------------
// |
// |  Struct      : GPtileListData
// |  Description : 
// |								This struct defines the data which be used by the GPworld
// |								in the load/unload process loop.
// |
// +-----------------------------------------------------------------------
struct GPtileListData
{
	GRPtileAction state_;
	GPtile* pTile_;
};

// +-----------------------------------------------------------------------
// |
// |  Class       : GPtile
// |  Description : 
// |								This class is a map of GPtiles used by the GPwindows and 
// |								GPworld to manage the GPtiles.
// |
// +-----------------------------------------------------------------------
typedef std::map<GPindex,GPtile*> GPtileMap;

#endif //GPTILETYPES_H
