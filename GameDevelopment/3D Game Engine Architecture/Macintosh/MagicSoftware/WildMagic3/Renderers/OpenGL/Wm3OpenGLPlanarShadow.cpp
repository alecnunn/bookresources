// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3OpenGLRenderer.h"
#include "Wm3Matrix4.h"
#include "Wm3Node.h"
#include "Wm3PlanarShadowEffect.h"
using namespace Wm3;

//----------------------------------------------------------------------------
void OpenGLRenderer::DrawPlanarShadow ()
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
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_TRUE);
        m_bAllowZBufferState = false;

        // Enable the stencil buffer so that the shadow can be clipped by the
        // plane.
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_ALWAYS,i+1,~0);
        glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
        glStencilMask(~0);

        // draw the plane
        Renderer::Draw(spkPlane);

        // disable the stencil buffer and depth buffer
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_DEPTH_TEST);
        m_bAllowZBufferState = true;

        // compute the equation for the shadow plane in world coordinates
        Triangle3f kTri;
        spkPlane->GetWorldTriangle(0,kTri);
        Plane3f kPlane(kTri.V[0],kTri.V[1],kTri.V[2]);

        // Conservative test to see if a shadow should be cast.  This can
        // cause incorrect results if the caster is large and intersects the
        // plane, but ordinarily we are not trying to cast shadows in such
        // situations.
        if ( m_pkNode->WorldBound->WhichSide(kPlane) < 0 )
        {
            // shadow caster is on far side of plane
            continue;
        }

        // Compute the projection matrix for the light source.
        Matrix4f kProjectionTranspose;
        if ( spkProjector->Type == Light::LT_DIRECTIONAL )
        {
            float fNdD =
                kPlane.Normal.Dot(spkProjector->GetWorldDVector());
            if ( fNdD >= 0.0f )
            {
                // Projection must be onto the "positive side" of the plane.
                continue;
            }

            kProjectionTranspose.MakeObliqueProjection(kPlane.Normal,
                kTri.V[0],spkProjector->GetWorldDVector());
        }
        else if ( spkProjector->Type == Light::LT_POINT
        ||        spkProjector->Type == Light::LT_SPOT )
        {
            float fNdE =
                kPlane.Normal.Dot(spkProjector->GetWorldLocation());
            if ( fNdE <= 0.0f )
            {
                // Projection must be onto the "positive side" of the plane.
                continue;
            }

            kProjectionTranspose.MakePerspectiveProjection(kPlane.Normal,
                kTri.V[0],spkProjector->GetWorldLocation());
        }
        else
        {
            assert( false );
            kProjectionTranspose = Matrix4f::IDENTITY;
        }

        Matrix4f kProjection = kProjectionTranspose.Transpose();

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glMultMatrixf((const float*)kProjection);

        // Alpha blending used to blend the shadow color with the appropriate
        // pixels drawn for the projection plane.
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        ColorRGBA kSaveColor;
        glGetFloatv(GL_CURRENT_COLOR,(float*)kSaveColor);
        glColor4fv((const float*)kShadowColor);

        // Only draw where the plane has been drawn.
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_EQUAL,i+1,~0);
        glStencilOp(GL_KEEP,GL_KEEP,GL_ZERO);

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

        // disable the stencil buffer
        glDisable(GL_STENCIL_TEST);

        // restore current color
        glColor4fv((const float*)kSaveColor);

        // disable alpha blending
        glDisable(GL_BLEND);

        // restore the model-to-world transformation
        glPopMatrix();
    }

    // reattach the effect
    m_pkNode->SetEffect(spkPSEffect);
}
//----------------------------------------------------------------------------
