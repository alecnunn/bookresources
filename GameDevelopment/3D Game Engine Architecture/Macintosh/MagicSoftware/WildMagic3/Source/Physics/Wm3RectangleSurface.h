// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3RECTANGLESURFACE_H
#define WM3RECTANGLESURFACE_H

#include "Wm3ParametricSurface.h"
#include "Wm3TriMesh.h"

namespace Wm3
{

class WM3_ITEM RectangleSurface : public TriMesh
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    // Construction and destruction.  The input surface must be rectangular,
    // not triangular.  RectangleSurface accepts responsibility for deleting
    // the input surface.  If bWantColors is 'true', the vertex colors are
    // allocated and set to black.  The application needs to assign colors as
    // needed.  If either of pkUVMin or pkUVMax is not null, both must be not
    // null.  In this case, texture coordinates are generated for the
    // surface.
    RectangleSurface (ParametricSurfacef* pkSurface, int iUSamples,
        int iVSamples, bool bWantNormals, bool bWantColors, bool bDoubleSided,
        const Vector2f* pkUVMin, const Vector2f* pkUVMax);

    virtual ~RectangleSurface ();

    // member access
    ParametricSurfacef*& Surface ();
    const ParametricSurfacef* GetSurface () const;
    int GetUSamples () const;
    int GetVSamples () const;

    // If the surface is modified, for example if it is control point
    // based and the control points are modified, then you should call this
    // update function to recompute the rectangle surface geometry.
    void UpdateSurface ();

protected:
    RectangleSurface ();

    ParametricSurfacef* m_pkSurface;
    int m_iUSamples, m_iVSamples;
};

WM3_REGISTER_STREAM(RectangleSurface);
typedef Pointer<RectangleSurface> RectangleSurfacePtr;
#include "Wm3RectangleSurface.inl"

}

#endif
