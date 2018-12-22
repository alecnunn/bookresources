// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3CAMERA_H
#define WM3CAMERA_H

#include "Wm3Spatial.h"
#include "Wm3Plane3.h"
#include "Wm3Ray3.h"

namespace Wm3
{

class WM3_ITEM Camera : public Spatial
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    // construction
    Camera ();

    // Camera frame (local coordinates)
    //   default location  E = (0,0,0)
    //   default direction D = (0,0,-1)
    //   default up        U = (0,1,0)
    //   default right     R = (1,0,0)
    // If a rotation matrix is used for the axis directions, the columns of
    // the matrix are [D U R].  That is, the view direction is in column 0,
    // the up direction is in column 1, and the right direction is in
    // column 2.
    void SetFrame (const Vector3f& rkLocation, const Vector3f& rkDVector,
        const Vector3f& rkUVector, const Vector3f& rkRVector);
    void SetFrame (const Vector3f& rkLocation, const Matrix3f& rkAxes);
    void SetLocation (const Vector3f& rkLocation);
    void SetAxes (const Vector3f& rkDVector, const Vector3f& rkUVector,
        const Vector3f& rkRVector);
    void SetAxes (const Matrix3f& rkAxes);
    Vector3f GetLocation () const;  // Local.Translate
    Vector3f GetDVector () const;   // Local.Rotate column 0
    Vector3f GetUVector () const;   // Local.Rotate column 1
    Vector3f GetRVector () const;   // Local.Rotate column 2

    // camera frame (world coordinates)
    Vector3f GetWorldLocation () const;  // World.Translate
    Vector3f GetWorldDVector () const;   // World.Rotate column 0
    Vector3f GetWorldUVector () const;   // World.Rotate column 1
    Vector3f GetWorldRVector () const;   // World.Rotate column 2

    // Set the view frustum.  The interval [rmin,rmax] is measured in the
    // right direction R.  These are the "left" and "right" frustum values.
    // The interval [umin,umax] is measured in the up direction U.  These are
    // the "bottom" and "top" values.  The interval [dmin,dmax] is measured
    // in the view direction D.  These are the "near" and "far" values.
    void SetFrustum (float fRMin, float fRMax, float fUMin, float fUMax,
        float fDMin, float fDMax);

    // Set symmetric view frustum (umin = -umax, rmin = -rmax) using field of
    // view in the "up" direction and an aspect ratio "width/height".  This
    // call is the equivalent of gluPerspective in OpenGL.  As such, the field
    // of view in this function must be specified in degrees and be in the
    // interval (0,180).
    void SetFrustum (float fUpFovDegrees, float fAspectRatio, float fDMin,
        float fDMax);

    void GetFrustum (float& rfRMin, float& rfRMax, float& rfUMin,
        float& rfUMax, float& rfDMin, float& rfDMax) const;

    float GetDMin () const;
    float GetDMax () const;
    float GetUMin () const;
    float GetUMax () const;
    float GetRMin () const;
    float GetRMax () const;

    // viewport (contained in [0,1]^2)
    void SetViewPort (float fLeft, float fRight, float fTop, float fBottom);
    void GetViewPort (float& rfLeft, float& rfRight, float& rfTop,
        float& rfBottom);

    // access to stack of world culling planes
    int GetPlaneQuantity () const;
    const Plane3f* GetPlanes () const;
    void PushPlane (const Plane3f& rkPlane);
    void PopPlane ();

    // Support for determining if the view frustum is fully on one side of a
    // plane.  The "positive side" of the plane is the half space to which the
    // plane normal points.  The "negative side" is the other half space.  The
    // flag "no side" indicates the plane itself.  The function returns +1 if
    // the view frustum is fully on the positive side of the plane, -1 if the
    // view frustum is fully on the negative side of the plane, or 0 if the
    // view frustum straddles the plane.  The input plane is in world
    // coordinates and the world camera coordinate system is used for the
    // test.
    int WhichSide (const Plane3f& rkPlane) const;

    // Mouse picking support.  The (x,y) input point is in left-handed screen
    // coordinates (what you usually read from the windowing system).  The
    // function returns 'true' if and only if the input point is located in
    // the current viewport.  When 'true', the origin and direction values
    // are valid and are in world coordinates.  The direction vector is unit
    // length.
    bool GetPickRay (int iX, int iY, int iWidth, int iHeight, Ray3f& rkRay)
        const;

protected:
    // update callbacks
    virtual void UpdateWorldBound ();
    void OnFrustumChange ();
    void OnViewPortChange ();
    void OnFrameChange ();

    // view frustum
    float m_fDMin, m_fDMax, m_fUMin, m_fUMax, m_fRMin, m_fRMax;

    // viewport
    float m_fPortL, m_fPortR, m_fPortT, m_fPortB;

    // Bit flag to store whether or not a plane is active in the culling
    // system.  A bit of 1 means the plane is active, otherwise the plane is
    // inactive.  The first 6 planes are the view frustum planes.  Indices
    // are:  left = 0, right = 1, bottom = 2, top = 3, near = 4, far = 5.
    // The system currently supports at most 32 culling planes.
    unsigned int m_uiPlaneState;

    // world planes: left = 0, right = 1, bottom = 2, top = 3, near = 4,
    // far = 5, extra culling planes >= 6
    enum 
    {
        CAM_FRUSTUM_PLANES = 6,
        CAM_MAX_WORLD_PLANES = 32
    };
    int m_iPlaneQuantity;
    Plane3f m_akWPlane[CAM_MAX_WORLD_PLANES];

    // values computed in OnFrustumChange that are needed in OnFrameChange
    float m_afCoeffL[2], m_afCoeffR[2], m_afCoeffB[2], m_afCoeffT[2];

// internal use
public:
    // culling support in Spatial::OnDraw
    void SetPlaneState (unsigned int uiPlaneState);
    unsigned int GetPlaneState () const;
    bool Culled (const BoundingVolume* pkWBound);

    // culling support in Portal::Draw
    bool Culled (int iVertexQuantity, const Vector3f* akVertex,
        bool bIgnoreNearPlane);

    // The renderer to which this camera has been attached.  The camera is
    // considered to be active if this pointer is not null.  By necessity, a
    // camera cannot be attached to multiple renderers, but a camera may be
    // shared by renderers as long as only one renderer at a time uses the
    // camera.  The renderer is responsible for setting this Camera member.
    Renderer* m_pkRenderer;

private:
    // base class functions not supported
    virtual void UpdateState (TStack<GlobalState*>*, TStack<Light*>*) { /**/ }
    virtual void Draw (Renderer&, bool) { /**/ }
};

WM3_REGISTER_STREAM(Camera);
typedef Pointer<Camera> CameraPtr;
#include "Wm3Camera.inl"

}

#endif
