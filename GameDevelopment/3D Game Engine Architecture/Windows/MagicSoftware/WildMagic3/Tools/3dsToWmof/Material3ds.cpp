// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Material3ds.h"
#include "Wm3CullState.h"
#include "Wm3TextureEffect.h"

//----------------------------------------------------------------------------
Material3ds::Material3ds ()
{
    m_bTwoSided = false;
    m_spkMat = 0;
    m_spkTex = 0;
    m_fUScale = 1.0f;
    m_fVScale = 1.0f;
    m_fUOffset = 0.0f;
    m_fVOffset = 0.0f;
}
//----------------------------------------------------------------------------
Material3ds::Material3ds (bool bTwoSided, MaterialStatePtr spkMat, 
    TexturePtr spkTex, float fUScale, float fVScale, float fUOffset,
    float fVOffset)
{
    m_bTwoSided = bTwoSided;
    m_spkMat = spkMat;
    m_spkTex = spkTex;
    m_fUScale = fUScale;
    m_fVScale = fVScale;
    m_fUOffset = fUOffset;
    m_fVOffset = fVOffset;
}
//----------------------------------------------------------------------------
Material3ds::~Material3ds ()
{
    m_spkMat = 0;
    m_spkTex = 0;
}
//----------------------------------------------------------------------------
void Material3ds::AttachStateTo (TriMeshPtr spkGeom, Vector2fArrayPtr spkUVs)
{
    spkGeom->SetGlobalState(m_spkMat);
    
    // Shading is specified in a shading subchunk as part of the material
    // chunk in the 3ds file, but it's not clear what that subchunk means.  
    // So, default to smooth shading.
    ShadeStatePtr spkShade = new ShadeState;
    spkShade->Shade = ShadeState::SM_SMOOTH;
    spkGeom->SetGlobalState (spkShade);

    if ( m_bTwoSided )
    {
        CullStatePtr spkCull = new CullState;
        spkCull->Enabled = false;
        spkGeom->SetGlobalState(spkCull);
    }

    // Must have texture coordinates and a valid texture to have texture
    // effects enabled
    if ( spkUVs && m_spkTex )
    {
        int iVerts = spkGeom->Vertices->GetQuantity();
        int iFaces = spkGeom->Indices->GetQuantity();

        // fix texture coordinates for offset and scale

        // This is messy, but if an object has multiple materials, it shares
        // the vertex, and UV data with other geometries, while having a
        // unique face listing.  Thus, not all the UV data needs to be updated,
        // since it may be being used by a different material.  Only vertices
        // included in the face list need to be legimately fixed.
        bool* abFixed = new bool[iVerts];
        int i;
        for (i = 0; i < iVerts; i++)
            abFixed[i] = false;

        // mins and maxes for texture coordinates to determine texture mode
        float fMinUV = 0.5f;
        float fMaxUV = 0.5f;

        for (i = 0; i < iFaces; i++)
        {
            int iV = (spkGeom->Indices->GetData())[i];
            if ( !abFixed[iV] )
            {
                // NOTE: No test cases for this yet and so I can only assume
                // the order and sign of these transformations.
                (*spkUVs)[iV].X() += m_fUOffset;
                (*spkUVs)[iV].X() /= m_fUScale;
                (*spkUVs)[iV].Y() += m_fVOffset;
                (*spkUVs)[iV].Y() /= m_fVScale;
                abFixed[iV] = true;

                // update min max info
                if ( fMinUV > (*spkUVs)[iV].X() )
                    fMinUV = (*spkUVs)[iV].X();
                if ( fMaxUV < (*spkUVs)[iV].X() )
                    fMaxUV = (*spkUVs)[iV].X();
                if ( fMinUV > (*spkUVs)[iV].Y() )
                    fMinUV = (*spkUVs)[iV].Y();
                if ( fMaxUV < (*spkUVs)[iV].Y() )
                    fMaxUV = (*spkUVs)[iV].Y();
            }
        }
        delete[] abFixed;

        // Intuit texture mode from min/max texture coordinates
        if ( fMinUV > 0.0f && fMaxUV < 1.0f )
        {
            m_spkTex->CoordU = Texture::WM_CLAMP;
            m_spkTex->CoordV = Texture::WM_CLAMP;
        }
        else if ( fMinUV >= 0.0f && fMaxUV <= 1.0f )
        {
            m_spkTex->CoordU = Texture::WM_CLAMP_EDGE;
            m_spkTex->CoordV = Texture::WM_CLAMP_EDGE;
        }
        else
        {
            m_spkTex->CoordU = Texture::WM_REPEAT;
            m_spkTex->CoordV = Texture::WM_REPEAT;
        }

        TextureEffectPtr spkTexEff = new TextureEffect(m_spkTex,spkUVs);
        spkGeom->SetEffect(spkTexEff);
    }
}
//----------------------------------------------------------------------------
