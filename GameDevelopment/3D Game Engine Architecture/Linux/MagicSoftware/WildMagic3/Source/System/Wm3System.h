// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3SYSTEM_H
#define WM3SYSTEM_H

#include "Wm3Platforms.h"

// common standard library headers
#include <cassert>
#include <cctype>
#include <cfloat>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <fstream>

// template classes
#include "Wm3TArray.h"
#include "Wm3THashSet.h"
#include "Wm3THashTable.h"
#include "Wm3TList.h"
#include "Wm3TSet.h"
#include "Wm3TStack.h"


namespace Wm3
{

class WM3_ITEM System
{
public:
    // swap byte order (size = 2, 4, or 8)
    static void SwapBytes (int iSize, void* pvValue);
    static void SwapBytes (int iSize, int iQuantity, void* pvValue);

    // The output is stored in little endian format.
    static void EndianCopy (int iSize, const void* pvSrc, void* pvDst);
    static void EndianCopy (int iSize, int iQuantity, const void* pvSrc,
        void* pvDst);

    // time utility (return value is seconds)
    static double GetTime ();

    // File support for buffer load and save.
    static bool Load (const char* acFilename, char*& racBuffer, int& riSize);
    static bool Save (const char* acFilename, const char* acBuffer,
        int iSize);
    static bool Append (const char* acFilename, char* acBuffer, int iSize);

    // Buffer read and write; hides endianness.  The return value is the
    // number of bytes read or written.  The function name suffix indicates
    // the number of bytes transferred per item.
    static int Read1 (const char* acBuffer, int iQuantity, void* pvData);
    static int Read2 (const char* acBuffer, int iQuantity, void* pvData);
    static int Read4 (const char* acBuffer, int iQuantity, void* pvData);
    static int Read8 (const char* acBuffer, int iQuantity, void* pvData);
    static int Write1 (char* acBuffer, int iQuantity, const void* pvData);
    static int Write2 (char* acBuffer, int iQuantity, const void* pvData);
    static int Write4 (char* acBuffer, int iQuantity, const void* pvData);
    static int Write8 (char* acBuffer, int iQuantity, const void* pvData);

    // File read and write; hides endianness.  The return value is the
    // number of bytes read or written.  The function name suffix indicates
    // the number of bytes transferred per item.
    static int Read1 (FILE* pkFile, int iQuantity, void* pvData);
    static int Read2 (FILE* pkFile, int iQuantity, void* pvData);
    static int Read4 (FILE* pkFile, int iQuantity, void* pvData);
    static int Read8 (FILE* pkFile, int iQuantity, void* pvData);
    static int Write1 (FILE* pkFile, int iQuantity, const void* pvData);
    static int Write2 (FILE* pkFile, int iQuantity, const void* pvData);
    static int Write4 (FILE* pkFile, int iQuantity, const void* pvData);
    static int Write8 (FILE* pkFile, int iQuantity, const void* pvData);

    // This is needed on the Macintosh because of its complicated application
    // structure.  The returned value is just the input, but has the side
    // effect of changing directory to the working directory of the
    // application.  The relative paths used for the data sets are then
    // resolved correctly.
    static const char* GetPath (const char* acFilename);

    // Creation of colors, hides endianness.
    static unsigned int MakeRGB (unsigned char ucR, unsigned char ucG,
        unsigned char ucB);
    static unsigned int MakeRGBA (unsigned char ucR, unsigned char ucG,
        unsigned char ucB, unsigned char ucA);

    // Allocation and deallocation of 2D arrays.  On deallocation, the array
    // pointer is set to null.
    template <class T> static void Allocate (int iCols, int iRows,
        T**& raatArray);
    template <class T> static void Deallocate (T**& raatArray);

    // Allocation and deallocation of 3D arrays.  On deallocation, the array
    // pointer is set to null.
    template <class T> static void Allocate (int iCols, int iRows,
        int iSlices, T***& raaatArray);
    template <class T> static void Deallocate (T***& raaatArray);
};

#include "Wm3System.inl"

}

#endif
