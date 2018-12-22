// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3IMAGEVERSION_H
#define WM3IMAGEVERSION_H

#include "Wm3System.h"

// Images are stored in files with extension wmif.  The header is of the
// form "Wild Magic Image File 3.xx" where the major version is 3 and the
// the minor version is xx in [00,99].  The length of the string is 26, but
// the null terminator is written to disk, so total number of file bytes used
// by the version is 27.  The current version is "3.00"

namespace Wm3
{

class WM3_ITEM ImageVersion
{
public:
    static const int MAJOR;       // 3
    static const int MINOR;       // 0
    static const char CURRENT[];  // "Wild Magic Image File 3.00"
    static const int LENGTH;      // 27 = strlen(CURRENT)+1

    ImageVersion (int iMajor = -1, int iMinor = -1);
    ImageVersion (const char* acString);

    int GetMajor () const;
    int GetMinor () const;

    // The version is valid if major is 3 and minor in [0,99].
    bool IsValid () const;

    // For comparisons of versions.  This is useful whenever a change to the
    // Image class causes a file format change.
    bool operator== (const ImageVersion& rkVersion) const;
    bool operator!= (const ImageVersion& rkVersion) const;
    bool operator<  (const ImageVersion& rkVersion) const;
    bool operator<= (const ImageVersion& rkVersion) const;
    bool operator>  (const ImageVersion& rkVersion) const;
    bool operator>= (const ImageVersion& rkVersion) const;

protected:
    int GetCombined () const;  // 100*major + minor

    int m_iMajor, m_iMinor;
};

}

#endif
