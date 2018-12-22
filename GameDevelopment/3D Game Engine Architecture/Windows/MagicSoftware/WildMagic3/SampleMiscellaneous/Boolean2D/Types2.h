// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef TYPES2_H
#define TYPES2_H

#include "Edge2.h"
#include "Wm3Vector2.h"
using namespace Wm3;

#include <map>
#include <vector>
using namespace std;

// vertices
typedef map<Vector2d,int> VMap;
typedef map<Vector2d,int>::iterator VIterator;
typedef map<Vector2d,int>::const_iterator VCIterator;
typedef vector<Vector2d> VArray;

// edges
typedef map<Edge2,int> EMap;
typedef map<Edge2,int>::iterator EIterator;
typedef map<Edge2,int>::const_iterator ECIterator;
typedef vector<Edge2> EArray;

#endif
