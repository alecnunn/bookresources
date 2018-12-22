//
// Installation routines for Win32
// Copyright (c) Glenn Fiedler (ptc@gaffer.org)
// This source code is in the public domain
//

#ifndef __INSTALL_ROUTINES_H
#define __INSTALL_ROUTINES_H

// include files
#include <windows.h>



// dll comparison enum
enum DLL_COMPARE
{
    OLDER,
    SAME,
    NEWER
};

// dll routines
BOOL InstallDLL(WORD resource,const char name[],BOOL force);
BOOL ReplaceDLL(const char source[],const char destination[]);
BOOL ScheduleDLL(const char source[],const char destination[]);
DLL_COMPARE CompareDLL(const char source[],const char destination[]);

// save a resource as a file
BOOL SaveResourceAsFile(WORD resource,const char filename[]);

// internal file routines
BOOL FileExists(const char filename[]);
BOOL FileCopy(const char source[],const char destination[]);
BOOL FileMove(const char source[],const char destination[]);
BOOL FileDelete(const char filename[]);





#endif
