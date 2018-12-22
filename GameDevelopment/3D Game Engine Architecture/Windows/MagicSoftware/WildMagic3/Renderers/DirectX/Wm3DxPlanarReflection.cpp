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
#include "Wm3PlanarReflectionEffect.h"
using namespace Wm3;

//----------------------------------------------------------------------------
void DxRenderer::DrawPlanarReflection ()
{
    // Access the special effect.  Detach it from the node to allow the
    // effectless node drawing.
    assert( DynamicCast<PlanarReflectionEffect>(m_pkNode->GetEffect()) );
    PlanarReflectionEffectPtr spkPREffect =
        (PlanarReflectionEffect*)m_pkNode->GetEffect();
    m_pkNode->SetEffect(0);

    // Enable depth buffering.  Do not allow objects to change the current
    // depth buffer state.
    ms_hResult = m_pqDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
    assert( SUCCEEDED(ms_hResult) );
    m_bAllowZBufferState = false;

    // enable the stencil buffer
    ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILENABLE,TRUE);
    assert( SUCCEEDED(ms_hResult) );

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
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILFUNC,
            D3DCMP_ALWAYS);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILREF,
            i+1);
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
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILMASK,~0);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_COLORWRITEENABLE,0);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
        assert( SUCCEEDED(ms_hResult) );
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
        D3DVIEWPORT9 kViewport;
        ms_hResult = m_pqDevice->GetViewport(&kViewport);
        assert( SUCCEEDED(ms_hResult) );
        kViewport.MinZ = 1.0;
        kViewport.MaxZ = 1.0;
        ms_hResult = m_pqDevice->SetViewport(&kViewport);
        assert( SUCCEEDED(ms_hResult) );
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
            D3DSTENCILOP_KEEP);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILWRITEMASK,~0);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_ALWAYS);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_COLORWRITEENABLE,
            D3DCOLORWRITEENABLE_ALPHA|D3DCOLORWRITEENABLE_BLUE|
            D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_RED);
        assert( SUCCEEDED(ms_hResult) );
        Renderer::Draw(spkPlane);

        // restore the depth range and depth testing function
        kViewport.MinZ = 0.0;
        kViewport.MaxZ = 1.0;
        ms_hResult = m_pqDevice->SetViewport(&kViewport);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
        assert( SUCCEEDED(ms_hResult) );

        // Save the modelview transform before replacing it with the viewing
        // transform which will handle the reflection.
        D3DXMATRIX kOldModelViewMat;
        ms_hResult = m_pqDevice->GetTransform(D3DTS_VIEW,&kOldModelViewMat); 
        assert( SUCCEEDED(ms_hResult) );

        // We are about to render the reflected caster.  For that, we will
        // need to compute the reflection viewing matrix.
        Triangle3f kTri;
        spkPlane->GetWorldTriangle(0,kTri);
        Plane3f kWMPlane(kTri.V[0],kTri.V[1],kTri.V[2]);

        // create a DX plane as well
        D3DXVECTOR3 kPlanePoint(kTri.V[0].X(),kTri.V[0].Y(),kTri.V[0].Z());
        D3DXVECTOR3 kPlaneNormal(kWMPlane.Normal.X(),kWMPlane.Normal.Y(),
            kWMPlane.Normal.Z());
        D3DXPLANE kPlane;
        D3DXPlaneFromPointNormal(&kPlane,&kPlanePoint,&kPlaneNormal);

        D3DXMATRIX kReflectMat;
        D3DXMatrixReflect(&kReflectMat,&kPlane);
        D3DXMATRIX kNewModelViewMat;
        D3DXMatrixMultiply(&kNewModelViewMat,&kReflectMat,&kOldModelViewMat);
        ms_hResult = m_pqDevice->SetTransform(D3DTS_VIEW,&kNewModelViewMat); 
        assert( SUCCEEDED(ms_hResult) );

        // Setup a clip plane so that only objects above the mirror plane get
        // reflected.
        DWORD dwClipPlanesEnabled;
        ms_hResult = m_pqDevice->GetRenderState(D3DRS_CLIPPLANEENABLE,
            &dwClipPlanesEnabled);
        assert( SUCCEEDED(ms_hResult) );
        DWORD dwClipPlaneIndex = 0;
        ms_hResult = m_pqDevice->SetClipPlane(dwClipPlaneIndex,kPlane);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,
            (1 << dwClipPlaneIndex) | dwClipPlanesEnabled);
        assert( SUCCEEDED(ms_hResult) );

        // Reverse the cull direction.  Allow for models that are not
        // necessarily set up with front or back face culling.
        m_bReverseCullFace = true;

        // Render the reflected object.  Only render where the stencil buffer
        // contains the reference value.  Disable culling to allow out-of-view
        // objects to cast reflections.
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
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_COLORWRITEENABLE,
            D3DCOLORWRITEENABLE_ALPHA|D3DCOLORWRITEENABLE_BLUE|
            D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_RED);
        assert( SUCCEEDED(ms_hResult) );
        m_pkNode->Draw(*this,true);

        // restore the cull direction
        m_bReverseCullFace = false;

        // disable the extra clip plane
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,
            dwClipPlanesEnabled);
        assert( SUCCEEDED(ms_hResult) );

        // restore the modelview matrix
        ms_hResult = m_pqDevice->SetTransform(D3DTS_VIEW,&kOldModelViewMat); 
        assert( SUCCEEDED(ms_hResult) );

        // Manually enable spkPlane's textures so that the final texture unit
        // will blend the plane with the reflected object.
        int iUnit = 0;
        bool bUseExistingLast = false;
        Effect* pkPlaneEff = spkPlane->GetEffect();
        if (pkPlaneEff)
        {
            for (iUnit = 0; iUnit < pkPlaneEff->Textures.GetQuantity();
                 iUnit++)
            {
                EnableTexture(iUnit,iUnit,pkPlaneEff);
            }

            // Special case to avoid an unnecessary texture pass.  If the
            // final texture unit is a replace (or a decal), then we can just
            // modify that texture stage instead of creating a new one.
            if ( iUnit > 0 )
            {
                int iFinalApply = pkPlaneEff->Textures[iUnit-1]->Apply;
                if ( iFinalApply == Texture::AM_REPLACE
                ||   iFinalApply == Texture::AM_DECAL)
                {
                    iUnit--;
                    bUseExistingLast = true;
                }
            }
        }

        // Create a texture stage
        D3DCOLOR kShadowBlendColor = D3DCOLOR_COLORVALUE(0.0,0.0,0.0,
            fReflectance);
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_TEXTUREFACTOR,
            kShadowBlendColor);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetTextureStageState(iUnit,D3DTSS_COLOROP,
            D3DTOP_SELECTARG1);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetTextureStageState(iUnit,D3DTSS_ALPHAOP,
            D3DTOP_SELECTARG2);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetTextureStageState(iUnit,D3DTSS_COLORARG1,
            bUseExistingLast ? D3DTA_TEXTURE : D3DTA_CURRENT);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetTextureStageState(iUnit,D3DTSS_ALPHAARG2,
            D3DTA_TFACTOR);
        assert( SUCCEEDED(ms_hResult) );
        m_abTexUnitEnabled[iUnit] = true;

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
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILENABLE,TRUE);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_COLORWRITEENABLE,
            D3DCOLORWRITEENABLE_ALPHA|D3DCOLORWRITEENABLE_BLUE|
            D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_RED);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_SRCBLEND,
            D3DBLEND_INVSRCALPHA);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_DESTBLEND,
            D3DBLEND_SRCALPHA);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILFUNC,
            D3DCMP_EQUAL);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILREF,
            i+1);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILMASK,~0);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILWRITEMASK,~0);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILFAIL,
            D3DSTENCILOP_KEEP);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILZFAIL,
            D3DSTENCILOP_KEEP);
        assert( SUCCEEDED(ms_hResult) );
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILPASS,
            D3DSTENCILOP_INVERT);
        assert( SUCCEEDED(ms_hResult) );

        m_bAllowAlphaState = false;
        m_bAllowTextures = false;
        Renderer::Draw(spkPlane);
        m_bAllowAlphaState = true;
        m_bAllowTextures = true;

        // manually disable the plane's texture(s)
        if (pkPlaneEff)
        {
            for (iUnit = 0; iUnit < pkPlaneEff->Textures.GetQuantity(); 
                 iUnit++)
            {
                DisableTexture(iUnit,iUnit,pkPlaneEff);
            }
        }
        m_abTexUnitEnabled[iUnit] = false;

        // disable alpha blending
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
        assert( SUCCEEDED(ms_hResult) );
    }

    // disable the stencil buffer
    ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILENABLE,FALSE);
    assert( SUCCEEDED(ms_hResult) );

    // restore depth buffer state
    m_bAllowZBufferState = true;
    ms_hResult = m_pqDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
    assert( SUCCEEDED(ms_hResult) );

    // render the objects as usual
    m_pkNode->Draw(*this);

    // reattach the effect
    m_pkNode->SetEffect(spkPREffect);
}
//----------------------------------------------------------------------------
