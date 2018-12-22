////////////////////////////////////////////////////////////
//
//  GENERAL.H - General Definitions
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/12/16 - Version 1.01A release.
//              94/12/17 - Added __huge conditional
//                         directive.
//              95/02/05 - Version 1.02A release.
//              95/07/21 - Version 1.02B release.
//              95/10/10 - Changed BOOL data type from int
//                         to short.
//              96/02/14 - Version 1.02C release.
//              96/04/01 - Version 1.03A release.
//              97/04/01 - Version 1.03B release.
//
//  Compilers:  Microsoft Visual C/C++ Professional V5.0
//              Borland C++ Version 5.0
//
//  Author:     Ian Ashdown, P.Eng.
//              byHeart Consultants Limited
//              620 Ballantree Road
//              West Vancouver, B.C.
//              Canada V7S 1W3
//              Tel. (604) 922-6148
//              Fax. (604) 987-7621
//
//  Copyright 1994-2003 byHeart Consultants Limited
//
//  The following source code has been derived from:
//
//    Ashdown, I. 1994. Radiosity: A Programmer's
//    Perspective. New York, NY: John Wiley & Sons.
//
//    (Available on CD-ROM from www.helios32.com.)
//
//  It may be freely copied, redistributed, and/or modified
//  for personal use ONLY, as long as the copyright notice
//  is included with all source code files.
//
////////////////////////////////////////////////////////////

#ifndef _GENERAL_H
#define _GENERAL_H

#ifndef _NOT_WIN_APP
#include <windows.h>    // MS-Windows application
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef _NOT_WIN_APP
#define FALSE   0
#define TRUE    1

typedef short BOOL;             // Boolean flag
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
#endif

// __huge data type is undefined for Win32
#ifdef WIN32
#define __huge
#endif

#ifndef max
#define max(a,b)  (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif

#define PI              3.141592654
#define MIN_VALUE       1.0e-10         // Minimum value
#define MAX_VALUE       1.0e10          // Maximum value

inline double RadToDeg( double r )
{ return r * 180.0 / PI; }

inline double DegToRad( double d )
{ return d * PI / 180.0; }

inline double GetNormRand()
{ return (double) rand() / (double) RAND_MAX; }

#endif

