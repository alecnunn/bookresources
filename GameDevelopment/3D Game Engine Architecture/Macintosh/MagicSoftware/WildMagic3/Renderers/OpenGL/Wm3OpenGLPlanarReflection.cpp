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
#include "Wm3PlanarReflectionEffect.h"
using namespace Wm3;

//----------------------------------------------------------------------------
void OpenGLRenderer::DrawPlanarReflection ()
{
    // Access the special effect.  Detach it from the node to allow the
    // effectless node drawing.
    assert( DynamicCast<PlanarReflectionEffect>(m_pkNode->GetEffect()) );
    PlanarReflectionEffectPtr spkPREffect =
        (PlanarReflectionEffect*)m_pkNode->GetEffect();
    m_pkNode->SetEffect(0);

    // Enable depth buffering.  Do not allow objects to change the current
    // depth buffer state.
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    m_bAllowZBufferState = false;

    // enable the stencil buffer
    glEnable(GL_STENCIL_TEST);

    // number of planes limited by stencil buffer size
    int iQuantity = spkPREffect->GetQuantity();
    if ( iQuantity >= m_iMaxStencilIndices-1 )
        iQuantity = m_iMaxStencilIndices-1;

    for (int i = 0; i < iQuantity; i++)
    {
        TriMeshPtr spkPlane = spkPREffect->GetPlane(i);
        float fReflectance = spkPREffect->GetReflectance(i);

        // Render the mirror into the stencil plane.  All visible mirror
        // pixels will have the stencil value of the mirror.  Make sure that
        // no pixels are written to the depth buffer or color buffer, but use
        // depth buffer testing so that the stencil will not be written where
        // the plane is behind something already in the depth buffer.
        glStencilFunc(GL_ALWAYS,i+1,~0);
        glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
        glStencilMask(~0);
        glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
        glDepthMask(GL_FALSE);
        Renderer::Draw(spkPlane);

        // Render the mirror plane again by only processing pixels where the
        // stencil buffer contains the reference value.  This time there are
        // no changes to the stencil buffer and the depth buffer value is
        // reset to the far clipping plane.  This is done by setting the range
        // of depth values in the viewport volume to be [1,1].  Since the
        // mirror plane cannot also be semi-transparent, we do not care what
        // is behind the mirror plane in the depth buffer.  We need to move
        // the depth buffer values back where the mirror plane will be
        // rendered so that when the reflected object is rendered, it can be
        // depth buffered correctly.  Note that the rendering of the reflected
        // object will cause depth value to be written, which will appear to
        // be behind the mirror plane.  Enable writes to the color buffer.
        // Later when we want to render the reflecting plane and have it blend
        // with the background, which should contain the reflected caster, we
        // want to use the same blending function so that the pixels where the
        // reflected object was not rendered will contain the reflecting plane
        // colors.  In that case, the blending result will have the reflecting
        // plane appear to be opaque when in reality it was blended with
        // blending coefficients adding to one.
        glDepthRange(1.0,1.0);
        glDepthFunc(GL_ALWAYS);
        glStencilFunc(GL_EQUAL,i+1,~0);
        glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
        glStencilMask(~0);
        glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
        glDepthMask(GL_TRUE);
        Renderer::Draw(spkPlane);

        // restore the depth range and depth testing function
        glDepthFunc(GL_LESS);
        glDepthRange(0.0,1.0);

        // compute the equation for the mirror plane in world coordinates
        Triangle3f kTri;
        spkPlane->GetWorldTriangle(0,kTri);
        Plane3f kPlane(kTri.V[0],kTri.V[1],kTri.V[2]);

        // compute the reflection matrix
        Matrix4f kReflectionTranspose;
        kReflectionTranspose.MakeReflection(kPlane.Normal,kTri.V[0]);
        Matrix4f kReflection = kReflectionTranspose.Transpose();
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glMultMatrixf((const float*)kReflection);

        // Enable a clip plane so that only objects above the mirror plane
        // are reflected.
        GLdouble adPlane[4] =
        {
            (double)kPlane.Normal.X(),
            (double)kPlane.Normal.Y(),
            (double)kPlane.Normal.Z(),
            (double)-kPlane.Constant
        };
        glClipPlane(GL_CLIP_PLANE0,adPlane);
        glEnable(GL_CLIP_PLANE0);

        // Reverse the cull direction.  Allow for models that are not
        // necessarily set up with front or back face culling.
        m_bReverseCullFace = true;

        // Render the reflected object.  Only render where the stencil buffer
        // contains the reference value.  Disable culling to allow out-of-view
        // objects to cast reflections.
        glStencilFunc(GL_EQUAL,i+1,~0);
        glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
        m_pkNode->Draw(*this,true);

        // restore the cull direction
        m_bReverseCullFace = false;

        // disable the extra clip plane
        glDisable(GL_CLIP_PLANE0);

        // restore the modelview matrix
        glPopMatrix();

        // We are about to render the reflecting plane again.  Reset to the
        // global state for the reflecting plane.  We want to blend the
        // reflecting plane with what is already in the color buffer,
        // particularly either the image of the reflected caster or the
        // reflecting plane.  All we want for the reflecting plane at this
        // stage is to force the alpha channel to always be the reflectance
        // value for the reflecting plane.  Render the reflecting plane
        // wherever the stencil buffer is set to the reference value.  This
        // time clear the stencil buffer reference value where it is set.
        // Perform the normal depth buffer testing and writes.  Allow the
        // color buffer to be written to, but this time blend the reflecting
        // plane with the values in the color buffer based on the reflectance
        // value.  Note that where the stencil buffer is set, the color buffer
        // has either color values from the reflecting plane or the reflected
        // object.  Blending will use src=1-alpha (reflecting plane) and
        // dest=alpha background (reflecting plane or reflected object).
        glEnable(GL_BLEND);
        glBlendColor(0.0f,0.0f,0.0f,fReflectance);
        glBlendFunc(GL_ONE_MINUS_CONSTANT_ALPHA,GL_CONSTANT_ALPHA);
        glStencilFunc(GL_EQUAL,i+1,~0);
        glStencilOp(GL_KEEP,GL_KEEP,GL_INVERT);
        glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
        m_bAllowAlphaState = false;
        Renderer::Draw(spkPlane);
        m_bAllowAlphaState = true;

        // disable alpha blending
        glDisable(GL_BLEND);
    }

    // disable the stencil buffer
    glDisable(GL_STENCIL_TEST);

    // restore depth buffer state
    m_bAllowZBufferState = true;
    glDisable(GL_DEPTH_TEST);

    // render the objects as usual
    m_pkNode->Draw(*this);

    // reattach the effect
    m_pkNode->SetEffect(spkPREffect);
}
//----------------------------------------------------------------------------
