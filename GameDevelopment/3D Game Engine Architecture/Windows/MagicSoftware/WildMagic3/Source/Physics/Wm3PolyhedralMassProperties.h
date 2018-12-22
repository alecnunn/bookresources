// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3POLYHEDRALMASSPROPERTIES_H
#define WM3POLYHEDRALMASSPROPERTIES_H

#include "Wm3Matrix3.h"

namespace Wm3
{

// The input triangle mesh must represent a polyhedron.  The triangles are
// represented as triples of indices <V0,V1,V2> into the vertex array.
// The connectivity array has iTQuantity such triples.  The Boolean value
// bBodyCoords is 'true' if you want the inertia tensor to be relative to
// body coordinates, 'false' if you want it in world coordinates.

template <class Real> WM3_ITEM
void ComputeMassProperties (const Vector3<Real>* akVertex, int iTQuantity,
    const int* aiIndex, bool bBodyCoords, Real& rfMass,
    Vector3<Real>& rkCenter, Matrix3<Real>& rkInertia);

}

#endif
