// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3PARTICLES_H
#define WM3PARTICLES_H

#include "Wm3TriMesh.h"
#include "Wm3FloatArray.h"

namespace Wm3
{

class Camera;

class WM3_ITEM Particles : public TriMesh
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    // construction and destruction
    Particles (Vector3fArrayPtr spkLocations, FloatArrayPtr spkSizes,
        bool bWantNormals);
    virtual ~Particles ();

    // data members
    Vector3fArrayPtr Locations;
    FloatArrayPtr Sizes;
    float SizeAdjust;

    void SetActiveQuantity (int iActiveQuantity);
    int GetActiveQuantity () const;

    virtual void SetEffect (Effect* pkEffect);

    // If the Particles effect attributes are modified, you should call this
    // function to make sure the TriMesh effect attributes are updated.  That
    // is, the attributes of the four corners of each particle rectangle must
    // be regenerated from the newly modified particle attributes.
    void RefreshEffect ();

protected:
    Particles ();

    // generate attributes for the triangle mesh from the Particle effect
    void GenerateColorRGBs ();
    void GenerateColorRGBAs ();
    void GenerateUVs ();

    // Drawing.  The particles are billboards that always face the camera.
    void GenerateParticles (const Camera* pkCamera);
    virtual void Draw (Renderer& rkRenderer, bool bNoCull = false);

    // Allow application to specify fewer than the maximum number of vertices
    // to draw.
    int m_iActiveQuantity;

    // Store the effect that applies to the particle data.  The data member
    // Geometry::m_spkEffect will store the derived effect that applies to
    // the triangle mesh that represents the particles.
    EffectPtr m_spkParticleEffect;
};

WM3_REGISTER_STREAM(Particles);
typedef Pointer<Particles> ParticlesPtr;
#include "Wm3Particles.inl"

}

#endif

