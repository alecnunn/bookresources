// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3Renderer.h"
#include "Wm3Camera.h"
#include "Wm3Effect.h"
#include "Wm3Geometry.h"
#include "Wm3Light.h"
#include "Wm3Node.h"
#include "Wm3ScreenPolygon.h"
#include "Wm3Texture.h"
#include "Wm3ShaderEffect.h"
using namespace Wm3;

Renderer::SetConstantFunction
Renderer::ms_aoSCFunction[ShaderConstant::SC_QUANTITY] =
{
    &Renderer::SetConstantTransformM,
    &Renderer::SetConstantTransformP,
    &Renderer::SetConstantTransformMV,
    &Renderer::SetConstantTransformMVP,
    &Renderer::SetConstantCameraPosition,
    &Renderer::SetConstantCameraDirection,
    &Renderer::SetConstantCameraUp,
    &Renderer::SetConstantCameraRight,
    &Renderer::SetConstantFogColor,
    &Renderer::SetConstantFogParams,
    &Renderer::SetConstantMaterialEmissive,
    &Renderer::SetConstantMaterialAmbient,
    &Renderer::SetConstantMaterialDiffuse,
    &Renderer::SetConstantMaterialSpecular,
    &Renderer::SetConstantMaterialShininess,
    &Renderer::SetConstantLightPosition,
    &Renderer::SetConstantLightDirection,
    &Renderer::SetConstantLightAmbient,
    &Renderer::SetConstantLightDiffuse,
    &Renderer::SetConstantLightSpecular,
    &Renderer::SetConstantLightSpotCutoff,
    &Renderer::SetConstantLightAttenParams,
};

//----------------------------------------------------------------------------
Renderer::Renderer (int iWidth, int iHeight)
    :
    m_kBackgroundColor(ColorRGB::WHITE),
    m_kDeferredObject(1024,128),
    m_kDeferredIsGeometry(1024,128)
{
    m_iDeferredQuantity = 0;
    m_iWidth = iWidth;
    m_iHeight = iHeight;
    m_pkCamera = 0;

    // Derived classes should set the maximum values, then resize the
    // m_kLight and m_kTexture arrays accordingly.  This will guarantee that
    // these arrays never reallocate their internal data in the SetState
    // call that is made on each drawing pass.
    m_iMaxLights = 0;
    m_iMaxTextures = 0;
    m_iMaxStencilIndices = 0;

    // current object and effect
    m_pkNode = 0;
    m_pkGeometry = 0;
    m_pkLocalEffect = 0;
    m_pkGlobalEffect = 0;

    m_bAllowGlobalState = true;
    m_bAllowAlphaState = true;
    m_bAllowCullState = true;
    m_bAllowDitherState = true;
    m_bAllowFogState = true;
    m_bAllowMaterialState = true;
    m_bAllowShadeState = true;
    m_bAllowWireframeState = true;
    m_bAllowZBufferState = true;
    m_bAllowLighting = true;
    m_bAllowNormals = true;
    m_bAllowColors = true;
    m_bAllowTextures = true;
    m_bAllowWorldTransform = true;
    m_bReverseCullFace = false;

    // windowed mode by default
    m_bFullscreen = false;

    // current font (id = 0 is the default font)
    m_iFontID = 0;

    // deferred drawing disabled
    DrawDeferred = 0;
}
//----------------------------------------------------------------------------
Renderer::~Renderer ()
{
    SetCamera(0);
}
//----------------------------------------------------------------------------
void Renderer::SetCamera (Camera* pkCamera)
{
    if ( m_pkCamera )
        m_pkCamera->m_pkRenderer = 0;

    if ( pkCamera )
        pkCamera->m_pkRenderer = this;

    m_pkCamera = pkCamera;
}
//----------------------------------------------------------------------------
void Renderer::Resize (int iWidth, int iHeight)
{
    m_iWidth = iWidth;
    m_iHeight = iHeight;
    OnViewPortChange();
}
//----------------------------------------------------------------------------
void Renderer::DrawScene (Node* pkScene)
{
    if ( pkScene )
    {
        pkScene->OnDraw(*this);

        if ( DrawDeferred )
        {
            (this->*DrawDeferred)();
            m_iDeferredQuantity = 0;
        }
    }
}
//----------------------------------------------------------------------------
void Renderer::Draw (Node* pkNode)
{
    if ( !DrawDeferred )
    {
        m_pkNode = pkNode;
        m_pkGlobalEffect = pkNode->GetEffect();

        // If you reach this assert, make sure you call DrawScene rather than
        // Draw on the root node of your scene graph.
        assert( m_pkGlobalEffect );
        (this->*m_pkGlobalEffect->Draw)();

        m_pkNode = 0;
        m_pkGlobalEffect = 0;
    }
    else
    {
        m_kDeferredObject.SetElement(m_iDeferredQuantity,pkNode);
        m_kDeferredIsGeometry.SetElement(m_iDeferredQuantity,false);
        m_iDeferredQuantity++;
    }
}
//----------------------------------------------------------------------------
void Renderer::Draw (Geometry* pkGeometry)
{
    if ( !DrawDeferred )
    {
        m_pkGeometry = pkGeometry;
        m_pkLocalEffect = pkGeometry->GetEffect();

        if ( m_pkLocalEffect )
            (this->*m_pkLocalEffect->Draw)();
        else
            DrawPrimitive();

        m_pkLocalEffect = 0;
        m_pkGeometry = 0;
    }
    else
    {
        m_kDeferredObject.SetElement(m_iDeferredQuantity,pkGeometry);
        m_kDeferredIsGeometry.SetElement(m_iDeferredQuantity,true);
        m_iDeferredQuantity++;
    }
}
//----------------------------------------------------------------------------
void Renderer::Draw (ScreenPolygon* pkPolygon)
{
    m_bAllowWorldTransform = false;
    Draw((Geometry*)pkPolygon);
    m_bAllowWorldTransform = true;
}
//----------------------------------------------------------------------------
void Renderer::SetGlobalState (
    GlobalStatePtr aspkState[GlobalState::MAX_STATE])
{
    GlobalState* pkState;

    if ( m_bAllowAlphaState )
    {
        pkState = aspkState[GlobalState::ALPHA];
        SetAlphaState((AlphaState*)pkState);
    }

    if ( m_bAllowCullState )
    {
        pkState = aspkState[GlobalState::CULL];
        SetCullState((CullState*)pkState);
    }

    if ( m_bAllowDitherState )
    {
        pkState = aspkState[GlobalState::DITHER];
        SetDitherState((DitherState*)pkState);
    }

    if ( m_bAllowFogState )
    {
        pkState = aspkState[GlobalState::FOG];
        SetFogState((FogState*)pkState);
    }

    if ( m_bAllowMaterialState )
    {
        pkState = aspkState[GlobalState::MATERIAL];
        SetMaterialState((MaterialState*)pkState);
    }

    if ( m_bAllowShadeState )
    {
        pkState = aspkState[GlobalState::SHADE];
        SetShadeState((ShadeState*)pkState);
    }

    if ( m_bAllowWireframeState )
    {
        pkState = aspkState[GlobalState::WIREFRAME];
        SetWireframeState((WireframeState*)pkState);
    }

    if ( m_bAllowZBufferState )
    {
        pkState = aspkState[GlobalState::ZBUFFER];
        SetZBufferState((ZBufferState*)pkState);
    }
}
//----------------------------------------------------------------------------
void Renderer::EnableTextures ()
{
    int iTMax, i;
    int iUnit = 0;

    // set the local-effect texture units
    if ( m_pkLocalEffect )
    {
        iTMax = m_pkLocalEffect->Textures.GetQuantity();
        if ( iTMax > m_iMaxTextures )
            iTMax = m_iMaxTextures;

        for (i = 0; i < iTMax; i++)
            EnableTexture(iUnit++,i,m_pkLocalEffect);
    }

    // set the global-effect texture units
    if ( m_pkGlobalEffect )
    {
        iTMax = m_pkGlobalEffect->Textures.GetQuantity();
        if ( iTMax > m_iMaxTextures )
            iTMax = m_iMaxTextures;

        for (i = 0; i < iTMax; i++)
            EnableTexture(iUnit++,i,m_pkGlobalEffect);
    }
}
//----------------------------------------------------------------------------
void Renderer::DisableTextures ()
{
    int iTMax, i;
    int iUnit = 0;

    // disable the local-effect texture units
    if ( m_pkLocalEffect )
    {
        iTMax = m_pkLocalEffect->Textures.GetQuantity();
        if ( iTMax > m_iMaxTextures )
            iTMax = m_iMaxTextures;

        for (i = 0; i < iTMax; i++)
            DisableTexture(iUnit++,i,m_pkLocalEffect);
    }

    // disable the global-effect texture units
    if ( m_pkGlobalEffect )
    {
        iTMax = m_pkGlobalEffect->Textures.GetQuantity();
        if ( iTMax > m_iMaxTextures )
            iTMax = m_iMaxTextures;

        for ( i = 0; i < iTMax; i++)
            DisableTexture(iUnit++,i,m_pkGlobalEffect);
    }
}
//----------------------------------------------------------------------------
void Renderer::ReleaseTextures (Spatial* pkScene)
{
    int i;

    // release all textures attached to this object
    Effect* pkEffect = pkScene->GetEffect();
    if ( pkEffect )
    {
        for (i = 0; i < pkEffect->Textures.GetQuantity(); i++)
        {
            Texture* pkTexture = pkEffect->Textures[i];
            if ( pkTexture )
                ReleaseTexture(pkTexture);
        }
    }

    // recurse the subtree of this object and release other textures
    Node* pkNode = DynamicCast<Node>(pkScene);
    if ( pkNode )
    {
        for (i = 0; i < pkNode->GetQuantity(); i++)
        {
            Spatial* pkChild = pkNode->GetChild(i);
            if ( pkChild )
                ReleaseTextures(pkChild);
        }
    }
}
//----------------------------------------------------------------------------
void Renderer::ReleaseArrays (Spatial* pkScene)
{
    Geometry* pkGeometry = DynamicCast<Geometry>(pkScene);
    if ( pkGeometry )
    {
        CachedVector3fArray* pkCVertices =
            DynamicCast<CachedVector3fArray>(pkGeometry->Vertices);
        if ( pkCVertices )
            ReleaseArray(pkCVertices);

        CachedIntArray* pkCIndices =
            DynamicCast<CachedIntArray>(pkGeometry->Indices);
        if ( pkCIndices )
            ReleaseArray(pkCIndices);

        CachedVector3fArray* pkCNormals =
            DynamicCast<CachedVector3fArray>(pkGeometry->Normals);
        if ( pkCNormals )
            ReleaseArray(pkCNormals);
    }

    int i;

    // release all colors and uv's attached to this object
    Effect* pkEffect = pkScene->GetEffect();
    if ( pkEffect )
    {
        CachedColorRGBAArray* pkCColorRGBAs =
            DynamicCast<CachedColorRGBAArray>(pkEffect->ColorRGBAs);
        if ( pkCColorRGBAs )
            ReleaseArray(pkCColorRGBAs);

        CachedColorRGBArray* pkCColorRGBs =
            DynamicCast<CachedColorRGBArray>(pkEffect->ColorRGBs);
        if ( pkCColorRGBs )
            ReleaseArray(pkCColorRGBs);

        for (i = 0; i < pkEffect->UVs.GetQuantity(); i++)
        {
            CachedVector2fArray* pkCUVs =
                DynamicCast<CachedVector2fArray>(pkEffect->UVs[i]);
            if ( pkCUVs )
                ReleaseArray(pkCUVs);
        }
    }

    // recurse the subtree of this object and release other arrays
    Node* pkNode = DynamicCast<Node>(pkScene);
    if ( pkNode )
    {
        for (i = 0; i < pkNode->GetQuantity(); i++)
        {
            Spatial* pkChild = pkNode->GetChild(i);
            if ( pkChild )
                ReleaseArrays(pkChild);
        }
    }
}
//----------------------------------------------------------------------------
void Renderer::DrawPrimitive ()
{
    if ( m_bAllowGlobalState )
        SetGlobalState(m_pkGeometry->States);

    if ( m_bAllowLighting )
        EnableLighting();

    EnableVertices();

    if ( m_bAllowNormals && m_pkGeometry->Normals )
        EnableNormals();

    if ( m_bAllowColors && m_pkLocalEffect )
    {
        if ( m_pkLocalEffect->ColorRGBAs )
            EnableColorRGBAs();
        else if ( m_pkLocalEffect->ColorRGBs )
            EnableColorRGBs();
    }

    if ( m_bAllowTextures )
        EnableTextures();

    if ( m_bAllowWorldTransform )
        SetWorldTransformation();
    else
        SetScreenTransformation();

    DrawElements();

    if ( m_bAllowWorldTransform )
        RestoreWorldTransformation();
    else
        RestoreScreenTransformation();

    if ( m_bAllowTextures )
        DisableTextures();

    if ( m_bAllowColors && m_pkLocalEffect )
    {
        if ( m_pkLocalEffect->ColorRGBAs )
            DisableColorRGBAs();
        else if ( m_pkLocalEffect->ColorRGBs )
            DisableColorRGBs();
    }

    if ( m_bAllowNormals && m_pkGeometry->Normals )
        DisableNormals();

    DisableVertices();

    if ( m_bAllowLighting )
        DisableLighting();
}
//----------------------------------------------------------------------------
void Renderer::DrawDeferredNoDraw ()
{
    // This function is supposed to do nothing.  Really!
}
//----------------------------------------------------------------------------
void Renderer::DrawDeferredNoSort ()
{
    // disable deferred drawing
    DrawFunction oSave = DrawDeferred;
    DrawDeferred = 0;

    for (int i = 0; i < m_iDeferredQuantity; i++)
    {
        if ( m_kDeferredIsGeometry[i] )
            Draw((Geometry*)m_kDeferredObject[i]);
        else
            Draw((Node*)m_kDeferredObject[i]);
    }

    // enable deferred drawing
    DrawDeferred = oSave;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// shader support
//----------------------------------------------------------------------------
void Renderer::ReleaseShaders (Spatial* pkScene)
{
    // release shaders attached to this object
    ShaderEffect* pkEffect = DynamicCast<ShaderEffect>(pkScene->GetEffect());
    if ( pkEffect )
    {
        ReleaseShader(pkEffect->VShader);
        ReleaseShader(pkEffect->PShader);
    }

    // recurse the subtree of this object and release other textures
    Node* pkNode = DynamicCast<Node>(pkScene);
    if ( pkNode )
    {
        for (int i = 0; i < pkNode->GetQuantity(); i++)
        {
            Spatial* pkChild = pkNode->GetChild(i);
            if ( pkChild )
                ReleaseShaders(pkChild);
        }
    }
}
//----------------------------------------------------------------------------
void Renderer::DrawShader ()
{
    // the effect is necessarily a shader
    ShaderEffect* pkSEffect = (ShaderEffect*)m_pkLocalEffect;

    // fog and materials are handled by the shader
    m_bAllowFogState = false;
    m_bAllowMaterialState = false;

    if ( m_bAllowGlobalState )
        SetGlobalState(m_pkGeometry->States);

    // no enabling of lighting, handled by the shader

    EnableVertices();

    if ( m_bAllowNormals && m_pkGeometry->Normals )
        EnableNormals();

    if ( m_bAllowColors && m_pkLocalEffect )
    {
        if ( m_pkLocalEffect->ColorRGBAs )
            EnableColorRGBAs();
        else if ( m_pkLocalEffect->ColorRGBs )
            EnableColorRGBs();
    }

    if ( m_bAllowTextures )
        EnableTextures();

    // Shaders can use uv coordinates that do not have textures associated
    // with them.  Enable those here.
    int iUnit = m_pkLocalEffect->Textures.GetQuantity();
    for (/**/; iUnit < m_pkLocalEffect->UVs.GetQuantity(); iUnit++)
    {
        SetActiveTextureUnit(iUnit);
        EnableUVs(m_pkLocalEffect->UVs[iUnit]);
    }

    // The shaders might use transformations, but rely on the derived
    // renderer to compute them.
    if ( m_bAllowWorldTransform )
        SetWorldTransformation();
    else
        SetScreenTransformation();

    if ( pkSEffect->VShader )
        EnableShader(pkSEffect->VShader);

    if ( pkSEffect->PShader )
        EnableShader(pkSEffect->PShader);

    DrawElements();

    if ( pkSEffect->VShader )
        DisableShader(pkSEffect->VShader);

    if ( pkSEffect->PShader )
        DisableShader(pkSEffect->PShader);

    if ( m_bAllowWorldTransform )
        RestoreWorldTransformation();
    else
        RestoreScreenTransformation();

    // Shaders may use UV coordinates that do not have textures associated
    // with them.  Disable those here.
    for (int iUnit = m_pkLocalEffect->Textures.GetQuantity(); iUnit <
        m_pkLocalEffect->UVs.GetQuantity(); iUnit++)
    {
        SetActiveTextureUnit(iUnit);
        DisableUVs(m_pkLocalEffect->UVs[iUnit]);
    }

    if ( m_bAllowTextures )
        DisableTextures();

    if ( m_bAllowColors && m_pkLocalEffect )
    {
        if ( m_pkLocalEffect->ColorRGBAs )
            DisableColorRGBAs();
        else if ( m_pkLocalEffect->ColorRGBs )
            DisableColorRGBs();
    }

    if ( m_bAllowNormals && m_pkGeometry->Normals )
        DisableNormals();

    DisableVertices();

    // no disabling of lighting, handled by the shader

    // restore fog and material state
    m_bAllowFogState = true;
    m_bAllowMaterialState = true;
}
//----------------------------------------------------------------------------
void Renderer::SetConstantCameraPosition (int, float* afData)
{
    Vector3f kWLocation = m_pkCamera->GetWorldLocation();
    afData[0] = kWLocation.X();
    afData[1] = kWLocation.Y();
    afData[2] = kWLocation.Z();
    afData[3] = 1.0f;
}
//----------------------------------------------------------------------------
void Renderer::SetConstantCameraDirection (int, float* afData)
{
    Vector3f kWDVector = m_pkCamera->GetWorldDVector();
    afData[0] = kWDVector.X();
    afData[1] = kWDVector.Y();
    afData[2] = kWDVector.Z();
    afData[3] = 0.0f;
}
//----------------------------------------------------------------------------
void Renderer::SetConstantCameraUp (int, float* afData)
{
    Vector3f kWUVector = m_pkCamera->GetWorldUVector();
    afData[0] = kWUVector.X();
    afData[1] = kWUVector.Y();
    afData[2] = kWUVector.Z();
    afData[3] = 0.0f;
}
//----------------------------------------------------------------------------
void Renderer::SetConstantCameraRight (int, float* afData)
{
    Vector3f kWRVector = m_pkCamera->GetWorldRVector();
    afData[0] = kWRVector.X();
    afData[1] = kWRVector.Y();
    afData[2] = kWRVector.Z();
    afData[3] = 0.0f;
}
//----------------------------------------------------------------------------
void Renderer::SetConstantFogColor (int, float* afData)
{
    FogState* pkFog = StaticCast<FogState>(
        m_pkGeometry->States[GlobalState::FOG]);
    afData[0] = pkFog->Color.R();
    afData[1] = pkFog->Color.G();
    afData[2] = pkFog->Color.B();
    afData[3] = pkFog->Color.A();
}
//----------------------------------------------------------------------------
void Renderer::SetConstantFogParams (int, float* afData)
{
    FogState* pkFog = StaticCast<FogState>(
        m_pkGeometry->States[GlobalState::FOG]);
    afData[0] = pkFog->Start;
    afData[1] = pkFog->End;
    afData[2] = pkFog->Density;
    afData[3] = ( pkFog->Enabled ? 1.0f : 0.0f );
}
//----------------------------------------------------------------------------
void Renderer::SetConstantMaterialEmissive (int, float* afData)
{
    MaterialState* pkMaterial = StaticCast<MaterialState>(
        m_pkGeometry->States[GlobalState::MATERIAL]);
    afData[0] = pkMaterial->Emissive.R();
    afData[1] = pkMaterial->Emissive.G();
    afData[2] = pkMaterial->Emissive.B();
    afData[3] = pkMaterial->Emissive.A();
}
//----------------------------------------------------------------------------
void Renderer::SetConstantMaterialAmbient (int, float* afData)
{
    MaterialState* pkMaterial = StaticCast<MaterialState>(
        m_pkGeometry->States[GlobalState::MATERIAL]);
    afData[0] = pkMaterial->Ambient.R();
    afData[1] = pkMaterial->Ambient.G();
    afData[2] = pkMaterial->Ambient.B();
    afData[3] = pkMaterial->Ambient.A();
}
//----------------------------------------------------------------------------
void Renderer::SetConstantMaterialDiffuse (int, float* afData)
{
    MaterialState* pkMaterial = StaticCast<MaterialState>(
        m_pkGeometry->States[GlobalState::MATERIAL]);
    afData[0] = pkMaterial->Diffuse.R();
    afData[1] = pkMaterial->Diffuse.G();
    afData[2] = pkMaterial->Diffuse.B();
    afData[3] = pkMaterial->Diffuse.A();
}
//----------------------------------------------------------------------------
void Renderer::SetConstantMaterialSpecular (int, float* afData)
{
    MaterialState* pkMaterial = StaticCast<MaterialState>(
        m_pkGeometry->States[GlobalState::MATERIAL]);
    afData[0] = pkMaterial->Specular.R();
    afData[1] = pkMaterial->Specular.G();
    afData[2] = pkMaterial->Specular.B();
    afData[3] = pkMaterial->Specular.A();
}
//----------------------------------------------------------------------------
void Renderer::SetConstantMaterialShininess (int, float* afData)
{
    MaterialState* pkMaterial = StaticCast<MaterialState>(
        m_pkGeometry->States[GlobalState::MATERIAL]);
    afData[0] = pkMaterial->Shininess;
    afData[1] = 0.0f;
    afData[2] = 0.0f;
    afData[3] = 0.0f;
}
//----------------------------------------------------------------------------
void Renderer::SetConstantLightPosition (int iOption, float* afData)
{
    Light* pkLight = m_pkGeometry->Lights[iOption];
    assert( pkLight && (pkLight->Type == Light::LT_POINT ||
        pkLight->Type == Light::LT_SPOT) );

    if ( pkLight )
    {
        Vector3f kWLocation = pkLight->GetWorldLocation();
        afData[0] = kWLocation.X();
        afData[1] = kWLocation.Y();
        afData[2] = kWLocation.Z();
        afData[3] = 1.0f;
    }
    else
    {
        afData[0] = 0.0f;
        afData[1] = 0.0f;
        afData[2] = 0.0f;
        afData[3] = 1.0f;
    }
}
//----------------------------------------------------------------------------
void Renderer::SetConstantLightDirection (int iOption, float* afData)
{
    Light* pkLight = m_pkGeometry->Lights[iOption];
    assert( pkLight && (pkLight->Type == Light::LT_DIRECTIONAL ||
        pkLight->Type == Light::LT_SPOT) );

    if ( pkLight )
    {
        Vector3f kWDVector = pkLight->GetWorldDVector();
        afData[0] = kWDVector.X();
        afData[1] = kWDVector.Y();
        afData[2] = kWDVector.Z();
        afData[3] = 0.0f;
    }
    else
    {
        afData[0] = 0.0f;
        afData[1] = 0.0f;
        afData[2] = 0.0f;
        afData[3] = 1.0f;
    }
}
//----------------------------------------------------------------------------
void Renderer::SetConstantLightAmbient (int iOption, float* afData)
{
    Light* pkLight = m_pkGeometry->Lights[iOption];
    assert( pkLight );

    if ( pkLight && pkLight->On )
    {
        afData[0] = pkLight->Ambient.R();
        afData[1] = pkLight->Ambient.G();
        afData[2] = pkLight->Ambient.B();
        afData[3] = pkLight->Ambient.A();
    }
    else
    {
        afData[0] = 0.0f;
        afData[1] = 0.0f;
        afData[2] = 0.0f;
        afData[3] = 1.0f;
    }
 }
//----------------------------------------------------------------------------
void Renderer::SetConstantLightDiffuse (int iOption, float* afData)
{
    Light* pkLight = m_pkGeometry->Lights[iOption];
    assert( pkLight );

    if ( pkLight && pkLight->On )
    {
        afData[0] = pkLight->Diffuse.R();
        afData[1] = pkLight->Diffuse.G();
        afData[2] = pkLight->Diffuse.B();
        afData[3] = pkLight->Diffuse.A();
    }
    else
    {
        afData[0] = 0.0f;
        afData[1] = 0.0f;
        afData[2] = 0.0f;
        afData[3] = 1.0f;
    }
}
//----------------------------------------------------------------------------
void Renderer::SetConstantLightSpecular (int iOption, float* afData)
{
    Light* pkLight = m_pkGeometry->Lights[iOption];
    assert( pkLight );

    if ( pkLight && pkLight->On )
    {
        afData[0] = pkLight->Specular.R();
        afData[1] = pkLight->Specular.G();
        afData[2] = pkLight->Specular.B();
        afData[3] = pkLight->Specular.A();
    }
    else
    {
        afData[0] = 0.0f;
        afData[1] = 0.0f;
        afData[2] = 0.0f;
        afData[3] = 1.0f;
    }
}
//----------------------------------------------------------------------------
void Renderer::SetConstantLightSpotCutoff (int iOption, float* afData)
{
    Light* pkLight = m_pkGeometry->Lights[iOption];
    assert( pkLight && pkLight->Type == Light::LT_SPOT );

    if ( pkLight )
    {
        afData[0] = pkLight->Angle;
        afData[1] = Mathf::Cos(afData[0]);
        afData[2] = Mathf::Sin(afData[0]);
        afData[3] = pkLight->Exponent;
    }
    else
    {
        afData[0] = 0.0f;
        afData[1] = 1.0f;
        afData[2] = 0.0f;
        afData[3] = 1.0f;
    }
}
//----------------------------------------------------------------------------
void Renderer::SetConstantLightAttenParams (int iOption, float* afData)
{
    Light* pkLight = m_pkGeometry->Lights[iOption];
    assert( pkLight );

    if ( pkLight )
    {
        afData[0] = pkLight->Constant;
        afData[1] = pkLight->Linear;
        afData[2] = pkLight->Quadratic;
        afData[3] = pkLight->Intensity;
    }
    else
    {
        afData[0] = 1.0f;
        afData[1] = 0.0f;
        afData[2] = 0.0f;
        afData[3] = 0.0f;
    }
}
//----------------------------------------------------------------------------
