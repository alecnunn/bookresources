// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef PLATFORMSUPPORT_H
#define PLATFORMSUPPORT_H

// Cross-platform:
// Standard libraries.

#include "Wm3System.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>

// Namespaces:
// Mac OS X quirks for CodeWarrior.

#if !__MACH__
using namespace std;
#endif

// System types:
// Generic but non-standard.

#if (macintosh || __MACOS__ || __APPLECC__)

// Mac OS X and 9.

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#if !__MACH__
#include <unistd.h>
#define FS_DIR_SEPARATOR    ':'
#define FS_DIR_CURTOKEN     ":"
#else
#define FS_DIR_SEPARATOR    '/'
#define FS_DIR_CURTOKEN     "./"
#endif

#else

// Windows and UNIX.

#ifdef WIN32

#include <windows.h>
#define FS_DIR_SEPARATOR    '\\'
#define FS_DIR_CURTOKEN     ".\\"

#else   // UNIX

#include <unistd.h>
#define FS_DIR_SEPARATOR    '/'
#define FS_DIR_CURTOKEN     "./"

#endif

// The popularity of these types on Windows and UNIX is definitely smaller
// than on the Mac OS.

#if !__MACH__
#ifndef SInt8
typedef char SInt8;
#endif
#ifndef UInt8
typedef unsigned char UInt8;
#endif

#ifndef SInt16
typedef short SInt16;
#endif
#ifndef UInt16
typedef unsigned short UInt16;
#endif

#ifndef SInt32
typedef int SInt32;
#endif
#ifndef UInt32
typedef unsigned int UInt32;
#endif
#endif

#endif

// Cross-platform:
// Endianness and safe deletion support.

bool IsHostBigEndian ();
UInt32 UInt32ToBigEndian (UInt32 uiTheNumber);
UInt32 UInt32ToHost (UInt32 uiTheNumber, bool bBigEndian);
UInt16 UInt16ToHost (UInt16 uiTheNumber, bool bBigEndian);
SInt32 SInt32ToHost (SInt32 iTheNumber, bool bBigEndian);
SInt16 SInt16ToHost (SInt16 iTheNumber, bool bBigEndian);
float FloatToHost (float fTheNumber, bool bBigEndian);
template <class T> void Delete (T*& rpkPointer);
template <class T> void DeleteArray (T*& rpkPointer);

#include "PlatformSupport.inl"
using namespace std;

#endif

