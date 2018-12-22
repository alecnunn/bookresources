// +-----------------------------------------------------------------------
// |
// |                         File Name: GPpch.h
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This is the Precompiled Headers File. It is used for 
// |							acelerating the compilation. The files included here
// |							are compiled only one time.
// | 
// +-----------------------------------------------------------------------

#ifndef GPPCH_H
#define GPPCH_H
#pragma once

// +---------------------------
// |	Disable Warnings
// +---------------------------
#pragma warning (disable: 4251)
#pragma warning (disable: 4786)

// +---------------------------
// |	Windows Includes
// +---------------------------
#include <windows.h>
#include <iostream>
#include <assert.h>

// +---------------------------
// |	STL Includes
// +---------------------------
#include <map>
#include <string>
#include <list>
#include <algorithm>

#endif //GPPCH_H