// +-----------------------------------------------------------------------
// |
// |                         File Name: GPG5_gpNode.types.h
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This files contains the basic types used in the GPG5 Test Engine.
// | 
// +-----------------------------------------------------------------------

#ifndef GPG5_GPNODETYPES_H
#define GPG5_GPNODETYPES_H
#pragma once

// +---------------------------
// |	Includes
// +---------------------------
#include <deque>
#include <algorithm>
#include <string>
#include <GPG5_gpVector.h>

// +---------------------------
// |	Forward Declarations
// +---------------------------
class GPG5_gpNode;

// +-----------------------------------------------------------------------
// |
// |  Class       : GPG5_gpNodeList
// |  Description : 
// |								This class is a deque of GPG5_gpNodes
// |
// +-----------------------------------------------------------------------
typedef std::deque<GPG5_gpNode*> GPG5_gpNodeList;

#endif //GPG5_GPNODETYPES_H