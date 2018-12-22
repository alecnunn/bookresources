// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3TUPLE_H
#define WM3TUPLE_H

#include "Wm3System.h"

namespace Wm3
{

template <int N>
class Tuple
{
public:
    // construction
    Tuple ();
    Tuple (const Tuple& rkT);

    // coordinate access
    operator const int* () const;
    operator int* ();
    int operator[] (int i) const;
    int& operator[] (int i);

    // assignment
    Tuple& operator= (const Tuple& rkT);

private:
    int m_aiTuple[N];
};

#include "Wm3Tuple.inl"

typedef Tuple<2> Tuple2;
typedef Tuple<3> Tuple3;

}

#endif
