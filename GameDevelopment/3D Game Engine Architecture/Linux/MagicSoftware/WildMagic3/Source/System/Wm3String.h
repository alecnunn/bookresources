// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3STRING_H
#define WM3STRING_H

#include "Wm3System.h"

namespace Wm3
{

class WM3_ITEM String
{
public:
    // construction and destruction
    String (const char* acText = 0);
    String (int iLength, const char* acText);
    String (const String& rkString);
    ~String ();

    // member access
    int GetLength () const;
    operator const char* () const;

    // assignment, comparisons, implicit conversion (support for hash tables)
    String& operator= (const String& rkString);
    String& operator+= (const String& rkString);
    String operator+ (const String& rkString);
    bool operator== (const String& rkString) const;
    bool operator!= (const String& rkString) const;
    operator unsigned int () const;

    // memory and disk footprint (support for streaming)
    int GetMemoryUsed () const;
    int GetDiskUsed () const;

    // Case-related
    String ToUpper () const;
    String ToLower () const;

private:
    // Text is stored as null-terminated character string in memory.  The
    // length counts all but the non-null character.  When written to disk,
    // the length is stored first and all but the non-null character are
    // stored second.
    int m_iLength;
    char* m_acText;
};

#include "Wm3String.inl"

}

#endif
