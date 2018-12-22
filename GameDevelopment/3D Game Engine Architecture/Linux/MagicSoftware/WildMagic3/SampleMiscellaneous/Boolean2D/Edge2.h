// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef EDGE2_H
#define EDGE2_H

class Edge2
{
public:
    Edge2 ();
    Edge2 (int j0, int j1);

    bool operator< (const Edge2& rkE) const;

    int i0, i1;
};

#endif
