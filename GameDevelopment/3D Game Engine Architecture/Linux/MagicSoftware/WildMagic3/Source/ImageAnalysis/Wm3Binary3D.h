// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3BINARY3D_H
#define WM3BINARY3D_H

#include "Wm3Images.h"
#include <vector>

namespace Wm3
{

class WM3_ITEM Binary3D : public ImageInt3D
{
public:
    // Construction and destruction.  Binary3D accepts responsibility for
    // deleting the input data array.
    Binary3D (int iXBound, int iYBound, int iZBound, Eint* atData = 0);
    Binary3D (const Binary3D& rkImage);
    Binary3D (const char* acFilename);

    // Compute the connected components of a binary image.  The components in
    // the returned image are labeled with positive integer values.  If the
    // image is identically zero, then the components image is identically
    // zero and the returned quantity is zero.

    void GetComponents (int& riQuantity, ImageInt3D& rkComponents) const;

protected:
    // helper for component labeling
    void AddToAssociative (int i0, int i1, int* aiAssoc) const;
};

}

#endif
