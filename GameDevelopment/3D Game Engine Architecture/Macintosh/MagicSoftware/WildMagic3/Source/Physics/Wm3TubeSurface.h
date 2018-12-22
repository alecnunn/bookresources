// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3TUBESURFACE_H
#define WM3TUBESURFACE_H

#include "Wm3Curve3.h"
#include "Wm3TriMesh.h"

namespace Wm3
{

class WM3_ITEM TubeSurface : public TriMesh
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    typedef float (*RadialFunction)(float);

    // Construction and destruction.  TubeSurface accepts responsibility
    // for deleting the input curve.  If rkUpVector is not the zero vector,
    // it will be used as 'up' in the frame calculations.  If it is the zero
    // vector, the Frenet frame will be used.  If bWantColors is 'true',
    // the vertex colors are allocated and set to black.  The application
    // needs to assign colors as needed.  If either of pkUVMin or pkUVMax is
    // not null, both must be not null.  In this case, texture coordinates are
    // generated for the surface.
    TubeSurface (Curve3f* pkMedial, RadialFunction oRadial, bool bClosed,
        const Vector3f& rkUpVector, int iMedialSamples, int iSliceSamples,
        bool bWantNormals, bool bWantColors, bool bSampleByArcLength,
        bool bInsideView, const Vector2f* pkUVMin, const Vector2f* pkUVMax);

    virtual ~TubeSurface ();

    // member access
    Curve3f*& Medial ();
    const Curve3f* GetMedial () const;
    RadialFunction& Radial ();
    RadialFunction GetRadial () const;
    Vector3f& UpVector ();
    const Vector3f& GetUpVector () const;
    int GetSliceSamples () const;

    // Generate vertices for the end slices.  These are useful when you build
    // an open tube and want to attach meshes at the ends to close the tube.
    // The input array must have size (at least) S+1 where S is the number
    // returned by GetSliceSamples.  Function GetTMinSlice is used to access
    // the slice corresponding to the medial curve evaluated at its domain
    // minimum, tmin.  Function GetTMaxSlice accesses the slice for the
    // domain maximum, tmax.  If the curve is closed, the slices are the same.
    void GetTMinSlice (Vector3f* akSlice);
    void GetTMaxSlice (Vector3f* akSlice);

    // If the medial curve is modified, for example if it is control point
    // based and the control points are modified, then you should call this
    // update function to recompute the tube surface geometry.
    void UpdateSurface ();

protected:
    TubeSurface ();

    // tessellation support
    int Index (int iS, int iM);
    void ComputeSinCos ();
    void ComputeVertices (Vector3f* akVertex);
    void ComputeNormals (const Vector3f* akVertex, Vector3f* akNormal);
    void ComputeUVs (const Vector2f& rkUVMin, const Vector2f& rkUVMax,
        Vector2f* akUV);
    void ComputeIndices (int& riTQuantity, int*& raiIndex, bool bInsideView);

    Curve3f* m_pkMedial;
    RadialFunction m_oRadial;
    int m_iMedialSamples, m_iSliceSamples;
    Vector3f m_kUpVector;
    float* m_afSin;
    float* m_afCos;
    bool m_bClosed, m_bSampleByArcLength;
};

WM3_REGISTER_STREAM(TubeSurface);
typedef Pointer<TubeSurface> TubeSurfacePtr;
#include "Wm3TubeSurface.inl"

}

#endif
