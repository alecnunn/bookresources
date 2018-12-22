// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3LIGHT_H
#define WM3LIGHT_H

#include "Wm3Spatial.h"
#include "Wm3ColorRGBA.h"

namespace Wm3
{

class WM3_ITEM Light : public Spatial
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    enum // Type
    {
        LT_AMBIENT,
        LT_DIRECTIONAL,
        LT_POINT,
        LT_SPOT,
        LT_QUANTITY
    };

    Light (int iType = LT_AMBIENT);
    virtual ~Light ();

    // light frame (local coordinates)
    //   default location  E = (0,0,0)
    //   default direction D = (0,0,-1)
    //   default up        U = (0,1,0)
    //   default right     R = (1,0,0)
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

    // For directional lights.  The direction vector must be unit length.
    // The up vector and left vector are generated automatically.
    void SetDirection (const Vector3f& rkDirection);

    // light frame (world coordinates)
    Vector3f GetWorldLocation () const;  // World.Translate
    Vector3f GetWorldDVector () const;   // World.Rotate column 0
    Vector3f GetWorldUVector () const;   // World.Rotate column 1
    Vector3f GetWorldRVector () const;   // World.Rotate column 2

    int Type;            // default: LT_AMBIENT
    ColorRGBA Ambient;   // default: ColorRGBA(0,0,0,1)
    ColorRGBA Diffuse;   // default: ColorRGBA(0,0,0,1)
    ColorRGBA Specular;  // default: ColorRGBA(0,0,0,1)
    float Intensity;     // default: 1
    float Constant;      // default: 1
    float Linear;        // default: 0
    float Quadratic;     // default: 0
    bool Attenuate;      // default: false
    bool On;             // default: true

    // spot light parameters (valid only when Type = LT_SPOT)
    float Exponent;
    float Angle;

private:
    // updates
    virtual void UpdateWorldBound ();
    void OnFrameChange ();

    // base class functions not supported
    virtual void UpdateState (TStack<GlobalState*>*, TStack<Light*>*) { /**/ }
    virtual void Draw (Renderer&, bool) { /**/ }

};

WM3_REGISTER_STREAM(Light);
typedef Pointer<Light> LightPtr;
#include "Wm3Light.inl"

}

#endif
