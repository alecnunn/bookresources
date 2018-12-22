// +-----------------------------------------------------------------------
// |
// |                         File Name: GPdefs.h
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the needed directives for export/import
// |							the GP Dynamic Link Library (DLL).
// | 
// | 
// +-----------------------------------------------------------------------

#ifndef GPDEFS_H
#define GPDEFS_H
#pragma once

// +---------------------------
// |	EXPORT /IMPORT Defines
// +---------------------------
#ifdef C_GP
  #define _GP __declspec(dllexport)
  #define F_GP __declspec(dllexport)
#else
  #define _GP __declspec(dllimport)
  #define F_GP
#endif C_GP


#endif //GPDEFS_H