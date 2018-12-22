// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3BOXSURFACE_H
#define WM3BOXSURFACE_H

#include "Wm3BSplineVolume.h"
#include "Wm3Node.h"
#include "Wm3TriMesh.h"

namespace Wm3
{

class WM3_ITEM BoxSurface : public Node
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    // Construction and destruction.  BoxSurface accepts responsibility for
    // deleting the input volume.  If bWantColors is 'true', the vertex colors
    // are allocated and set to black.  The application needs to assign colors
    // as needed.  If bWantTextures is 'true', the texture coordinates are
    // allocated for each of the six TriMesh faces of the surface.  For each
    // face the coordinates are in the full set [0,1]^2.
    //
    // The six TriMesh children of this object are initially stored in the
    // order: u=0 face, u=1 face, v=0 face, v=1 face, w=0 face, w=1 face.  The
    // texture coordinate ordering preserves the (u,v,w) ordering of the box
    // coordinate space.
    //   u=0 face:  (v,w) in [0,1]^2, origin at vertex (0,0,0)
    //   u=1 face:  (v,w) in [0,1]^2, origin at vertex (1,0,0)
    //   v=0 face:  (u,w) in [0,1]^2, origin at vertex (0,0,0)
    //   v=1 face:  (u,w) in [0,1]^2, origin at vertex (0,1,0)
    //   w=0 face:  (u,v) in [0,1]^2, origin at vertex (0,0,0)
    //   w=1 face:  (u,v) in [0,1]^2, origin at vertex (0,0,1)

    BoxSurface (BSplineVolumef* pkVolume, int iUSamples, int iVSamples,
        int iWSamples, bool bWantNormals, bool bWantColors,
        Effect* pkEffect);

    virtual ~BoxSurface ();

    // member access
    BSplineVolumef*& Volume ();
    const BSplineVolumef* GetVolume () const;
    int GetUSamples () const;
    int GetVSamples () const;
    int GetWSamples () const;

    // If the volume is modified, then you should call this update function
    // to recompute the box surface geometry.
    void UpdateSurface ();

    // The order of the faces can change by sorting.  Given a direction,
    // the faces are ordered so that the "back facing" ones occur first in
    // the list followed by the "front facing" ones.  This supports correct
    // alpha blending (at least when the faces are only moderately perturbed
    // by the B-spline volume).  The process of enabling and disabling
    // causes changes in the render state (cull state, zbuffer state).
    void EnableSorting ();
    void DisableSorting ();
    void SortFaces (const Vector3f& rkWorldViewDirection);

protected:
    BoxSurface ();

    TriMesh* CreateFace (int iRows, int iCols, bool bWantNormals,
        bool bWantColors, Effect* pkEffect, bool bCCW,
        float fFaceValue, int aiPerm[3]);

    void UpdateFace (int iRows, int iCols, Vector3f* akVertex,
        Vector3f* akNormal, bool bCCW, float fFaceValue, int aiPerm[3]);

    BSplineVolumef* m_pkVolume;
    int m_iUSamples, m_iVSamples, m_iWSamples;
    bool m_bDoSort;
};

WM3_REGISTER_STREAM(BoxSurface);
typedef Pointer<BoxSurface> BoxSurfacePtr;
#include "Wm3BoxSurface.inl"

}

#endif
