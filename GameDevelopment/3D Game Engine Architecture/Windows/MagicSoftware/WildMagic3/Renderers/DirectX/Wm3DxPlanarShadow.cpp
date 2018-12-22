// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3DxRenderer.h"
#include "Wm3Matrix4.h"
#include "Wm3Node.h"
#include "Wm3PlanarShadowEffect.h"
using namespace Wm3;

//----------------------------------------------------------------------------
void DxRenderer::DrawPlanarShadow ()
{
    // Access the special effect.  Detach it from the node to allow the
    // effectless node drawing.
    assert( DynamicCast<PlanarShadowEffect>(m_pkNode->GetEffect()) );
    PlanarShadowEffectPtr spkPSEffect =
        (PlanarShadowEffect*)m_pkNode->GetEffect();
    m_pkNode->SetEffect(0);

    // draw the caster
    m_pkNode->Draw(*this);

    // number of planes limited by stencil buffer size
    int iQuantity = spkPSEffect->GetQuantity();
    if ( iQuantity >= m_iMaxStencilIndices-1 )
        iQuantity = m_iMaxStencilIndices-1;

    for (int i = 0; i < iQuantity; i++)
    {
        TriMeshPtr spkPlane = spkPSEffect->GetPlane(i);
        LightPtr spkProjector = spkPSEffect->GetProjector(i);
        ColorRGBA kShadowColor = spkPSEffect->GetShadowColor(i);

        // enable depth buffering
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
        assert( SUCCEEDED(ms_hResult) );
        m_bAllowZBufferState = false;

        // Enable the stencil buffer so that the shadow can be clipped by the
        // plane.
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILENABLE,TRUE);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILFUNC,
            D3DCMP_ALWAYS);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILREF,
            i+1);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILMASK,~0);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILFAIL,
            D3DSTENCILOP_KEEP);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILZFAIL,
            D3DSTENCILOP_KEEP);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILPASS,
            D3DSTENCILOP_REPLACE);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILWRITEMASK,~0);
        assert( SUCCEEDED(ms_hResult) );

        // draw the plane
        Renderer::Draw(spkPlane);

        // disable the stencil buffer and depth buffer
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILENABLE,FALSE);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_ALWAYS);
        assert( SUCCEEDED(ms_hResult) );
        m_bAllowZBufferState = true;

        // compute the equation for the shadow plane in world coordinates
        Triangle3f kTri;
        spkPlane->GetWorldTriangle(0,kTri);
        Plane3f kWMPlane(kTri.V[0],kTri.V[1],kTri.V[2]);

        // Create a DX plane as well
        D3DXVECTOR3 kPlanePoint(kTri.V[0].X(),kTri.V[0].Y(),kTri.V[0].Z());
        D3DXVECTOR3 kPlaneNormal(kWMPlane.Normal.X(),kWMPlane.Normal.Y(),
            kWMPlane.Normal.Z());
        D3DXPLANE kPlane;
        D3DXPlaneFromPointNormal(&kPlane,&kPlanePoint,&kPlaneNormal);

        // Conservative test to see if a shadow should be cast.  This can
        // cause incorrect results if the caster is large and intersects the
        // plane, but ordinarily we are not trying to cast shadows in such
        // situations.
        if ( m_pkNode->WorldBound->WhichSide(kWMPlane) < 0 )
        {
            // shadow caster is on far side of plane
            continue;
        }

        // If an ambient light, no shadows are created.
        if ( spkProjector->Type == Light::LT_AMBIENT )
            continue;

        // Create the light vector based on the light type.
        D3DXVECTOR4 kLightPosition;
        Vector3f kLightVec;
        switch ( spkProjector->Type )
        {
        case Light::LT_DIRECTIONAL:
            kLightVec = -spkProjector->GetWorldDVector();
            kLightPosition.x = kLightVec.X();
            kLightPosition.y = kLightVec.Y();
            kLightPosition.z = kLightVec.Z();
            kLightPosition.w = 0.0f;
            break;
        case Light::LT_POINT:
        case Light::LT_SPOT:
            kLightVec = spkProjector->GetWorldLocation();
            kLightPosition.x = kLightVec.X();
            kLightPosition.y = kLightVec.Y();
            kLightPosition.z = kLightVec.Z();
            kLightPosition.w = 1.0f;
            break;
        default:
            // should not get here
            assert( false );
            break;
        }

        // save the modelview transform
        D3DXMATRIX kOldModelViewMat;
        ms_hResult = m_pqDevice->GetTransform(D3DTS_VIEW,&kOldModelViewMat); 
        assert( SUCCEEDED(ms_hResult) );

        // compute the projection matrix for the light source
        D3DXMATRIX kShadowMat;
        D3DXMatrixShadow(&kShadowMat,&kLightPosition,&kPlane);

        D3DXMATRIX kNewModelViewMat;
        D3DXMatrixMultiply(&kNewModelViewMat,&kShadowMat,&kOldModelViewMat);
        ms_hResult = m_pqDevice->SetTransform(D3DTS_VIEW,&kNewModelViewMat); 
        assert( SUCCEEDED(ms_hResult) );

        // Alpha blending used to blend the shadow color with the appropriate
        // pixels drawn for the projection plane.

        // Setup the first and only texture stage to pass the
        // shadowcolor as an output to be blended during the alpha blending
        // stage.
        D3DCOLOR kShadowBlendColor = D3DCOLOR_COLORVALUE(kShadowColor.R(),
            kShadowColor.G(),kShadowColor.B(),kShadowColor.A());
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_TEXTUREFACTOR,
            kShadowBlendColor);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetTextureStageState(0,D3DTSS_COLOROP,
            D3DTOP_SELECTARG1);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,
            D3DTOP_SELECTARG1);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetTextureStageState(0,D3DTSS_COLORARG1,
            D3DTA_TFACTOR);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,
            D3DTA_TFACTOR);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetTextureStageState(1,D3DTSS_COLOROP,
            D3DTOP_DISABLE);
        assert( SUCCEEDED(ms_hResult) );

        // Let the renderer know this texture unit is enabled
        m_abTexUnitEnabled[0] = true;

        // Attenuate the currently stored color by using blending.
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_SRCBLEND,
            D3DBLEND_SRCALPHA);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_DESTBLEND,
            D3DBLEND_INVSRCALPHA);
        assert( SUCCEEDED(ms_hResult) );

        // Only draw where the plane has been drawn.
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILENABLE,TRUE);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILFUNC,
            D3DCMP_EQUAL);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILREF,
            i+1);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILMASK,~0);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILFAIL,
            D3DSTENCILOP_KEEP);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILZFAIL,
            D3DSTENCILOP_KEEP);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILPASS,
            D3DSTENCILOP_ZERO);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILWRITEMASK,~0);
        assert( SUCCEEDED(ms_hResult) );

        // Draw the caster again, but using only vertices and triangles
        // indices.  Disable culling to allow out-of-view objects to cast
        // shadows.
        m_bAllowGlobalState = false;
        m_bAllowLighting = false;
        m_bAllowNormals = false;
        m_bAllowColors = false;
        m_bAllowTextures = false;

        m_pkNode->Draw(*this,true);

        m_bAllowGlobalState = true;
        m_bAllowLighting = true;
        m_bAllowNormals = true;
        m_bAllowColors = true;
        m_bAllowTextures = true;

        // disable the texture
        m_abTexUnitEnabled[0] = false;

        // disable the stencil buffer
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILENABLE,FALSE);
        assert( SUCCEEDED(ms_hResult) );

        // disable alpha blending
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
        assert( SUCCEEDED(ms_hResult) );

        // restore the model-to-world transformation
        ms_hResult = m_pqDevice->SetTransform(D3DTS_VIEW,&kOldModelViewMat); 
        assert( SUCCEEDED(ms_hResult) );
    }

    // reattach the effect
    m_pkNode->SetEffect(spkPSEffect);
}
//----------------------------------------------------------------------------
