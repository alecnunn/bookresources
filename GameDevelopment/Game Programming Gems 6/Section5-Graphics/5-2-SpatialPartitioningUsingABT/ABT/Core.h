/*******************************************************************\
| Core.h
|
|--------------------------------------------------------------------
| Copyright 2004. Martin Fleisz.
| All rights reserved.
|
|--------------------------------------------------------------------
| CREATED:		2004/10/30
| AUTHOR:		Martin Fleisz
| 
|--------------------------------------------------------------------
| DESCRIPTION:
| 
| Defines common data types, identifier strings and common purpose macros 
\********************************************************************/

#ifndef CORE_H
#define CORE_H

#pragma once


#include <Windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <tchar.h>
#include <stdio.h>
#include <string>
#include <vector>

using namespace std;


// link opengl libraries
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")


//---------------------------------------------------------
// data types

// signed base types
typedef signed char			int8;
typedef signed short		int16;
typedef signed int			int32;	
typedef signed __int64		int64;

// unsigned base types
typedef unsigned char		byte;
typedef byte				uint8;
typedef unsigned short		uint16;
typedef unsigned int		uint32;
typedef unsigned __int64	uint64;

// other types
typedef float				float32;
typedef double				float64;

// unicode string definition
#ifdef _UNICODE
typedef std::wstring			String;
#else
typedef std::string				String;
#endif

// NULL
#ifndef NULL
# define NULL (0)
#endif

//---------------------------------------------------------
// MACROS

#define SAFE_DELETE(p)		{	if((p)) { delete (p);		(p)=NULL;	}	}
#define SAFE_ARRAYDELETE(p)	{	if((p)) { delete [](p);		(p)=NULL;	}	}
#define SAFE_RELEASE(p)		{	if((p)) { (p)->Release();	(p)=NULL;	}	}


#endif // CORE_H