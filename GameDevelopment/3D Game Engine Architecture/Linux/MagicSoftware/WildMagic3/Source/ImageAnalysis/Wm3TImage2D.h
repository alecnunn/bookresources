// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3TIMAGE2D_H
#define WM3TIMAGE2D_H

#include "Wm3TImage.h"

namespace Wm3
{

template <class T>
class TImage2D : public TImage<T>
{
public:
    // Construction and destruction.  TImage2D accepts responsibility for
    // deleting the input data array.
    TImage2D (int iXBound, int iYBound, T* atData = 0);
    TImage2D (const TImage2D& rkImage);
    TImage2D (const char* acFilename);

    // data access
    T& operator() (int iX, int iY) const;

    // conversion between 2D coordinates and 1D indexing
    int GetIndex (int iX, int iY) const;
    void GetCoordinates (int iIndex, int& riX, int& riY) const;

    // assignment
    TImage2D& operator= (const TImage2D& rkImage);
    TImage2D& operator= (T tValue);
};

#include "Wm3TImage2D.inl"

}

#endif
