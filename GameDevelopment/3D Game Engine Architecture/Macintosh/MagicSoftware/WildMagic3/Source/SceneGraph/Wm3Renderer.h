// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3RENDERER_H
#define WM3RENDERER_H

#include "Wm3ColorRGB.h"
#include "Wm3ColorRGBA.h"
#include "Wm3AlphaState.h"
#include "Wm3CullState.h"
#include "Wm3DitherState.h"
#include "Wm3FogState.h"
#include "Wm3MaterialState.h"
#include "Wm3ShadeState.h"
#include "Wm3WireframeState.h"
#include "Wm3ZBufferState.h"
#include "Wm3CachedColorRGBAArray.h"
#include "Wm3CachedColorRGBArray.h"
#include "Wm3CachedIntArray.h"
#include "Wm3CachedVector2fArray.h"
#include "Wm3CachedVector3fArray.h"
#include "Wm3ShaderConstant.h"

namespace Wm3
{

class Camera;
class Effect;
class Geometry;
class Light;
class Node;
class PixelShader;
class ScreenPolygon;
class Shader;
class Spatial;
class Texture;
class VertexShader;

class WM3_ITEM Renderer
{
public:
    // Abstract API for renderers.  Each graphics API must implement this
    // layer.
    virtual ~Renderer ();

    // Only one type of renderer can exist in an application.  The active
    // renderer type implements GetType() to return one of the enumerations.
    enum
    {
        RT_OPENGL,
        RT_DIRECT3D,
        RT_QUANTITY
    };
    static int GetType ();

    // camera access and support
    void SetCamera (Camera* pkCamera);
    Camera* GetCamera () const;

    // window parameters
    int GetWidth () const;
    int GetHeight () const;
    virtual void Resize (int iWidth, int iHeight);
    virtual void ToggleFullscreen ();

    // background color access
    virtual void SetBackgroundColor (const ColorRGB& rkColor);
    const ColorRGB& GetBackgroundColor () const;

    // pre- and postdraw semantics
    virtual bool BeginScene ();
    virtual void EndScene ();

    // full window buffer operations
    virtual void ClearBackBuffer () = 0;
    virtual void ClearZBuffer () = 0;
    virtual void ClearStencilBuffer () = 0;
    virtual void ClearBuffers () = 0;
    virtual void DisplayBackBuffer () = 0;

    // clear the buffer in the specified subwindow
    virtual void ClearBackBuffer (int iXPos, int iYPos, int iWidth,
        int iHeight) = 0;
    virtual void ClearZBuffer (int iXPos, int iYPos, int iWidth,
        int iHeight) = 0;
    virtual void ClearStencilBuffer (int iXPos, int iYPos, int iWidth,
        int iHeight) = 0;
    virtual void ClearBuffers (int iXPos, int iYPos, int iWidth,
        int iHeight) = 0;

    // object drawing
    void DrawScene (Node* pkScene);
    virtual void Draw (ScreenPolygon* pkPolygon);

    // text drawing
    virtual int LoadFont (const char* acFace, int iSize, bool bBold,
        bool bItalic) = 0;
    virtual void UnloadFont (int iFontID) = 0;
    virtual bool SelectFont (int iFontID) = 0;
    virtual void Draw (int iX, int iY, const ColorRGBA& rkColor,
        const char* acText) = 0;

    // 2D drawing
    virtual void Draw (const unsigned char* aucBuffer) = 0;

    int GetMaxLights () const;
    int GetMaxTextures () const;
    int GetMaxStencilIndices () const;

    // management of texture resources
    virtual void ReleaseTexture (Texture* pkTexture) = 0;
    void ReleaseTextures (Spatial* pkScene);

    // management of array resources
    virtual void ReleaseArray (CachedColorRGBAArray* pkArray) = 0;
    virtual void ReleaseArray (CachedColorRGBArray* pkArray) = 0;
    virtual void ReleaseArray (CachedIntArray* pkArray) = 0;
    virtual void ReleaseArray (CachedVector2fArray* pkArray) = 0;
    virtual void ReleaseArray (CachedVector3fArray* pkArray) = 0;
    void ReleaseArrays (Spatial* pkScene);

    // management of shader resources
    virtual void ReleaseShader (Shader* pkShader) = 0;
    void ReleaseShaders (Spatial* pkScene);

    // deferred drawing (for render state sorting)
    typedef void (Renderer::*DrawFunction)();
    DrawFunction DrawDeferred;

    // no drawing (useful for profiling scene graph overhead)
    void DrawDeferredNoDraw ();

    // draw all objects without sorting
    void DrawDeferredNoSort ();

protected:
    // abstract base class
    Renderer (int iWidth, int iHeight);

    // global render state management
    void SetGlobalState (GlobalStatePtr aspkState[GlobalState::MAX_STATE]);
    virtual void SetAlphaState (AlphaState* pkState) = 0;
    virtual void SetCullState (CullState* pkState) = 0;
    virtual void SetDitherState (DitherState* pkState) = 0;
    virtual void SetFogState (FogState* pkState) = 0;
    virtual void SetMaterialState (MaterialState* pkState) = 0;
    virtual void SetShadeState (ShadeState* pkState) = 0;
    virtual void SetWireframeState (WireframeState* pkState) = 0;
    virtual void SetZBufferState (ZBufferState* pkState) = 0;

    // light management
    enum
    {
        EL_AMBIENT = 1,
        EL_DIFFUSE = 2,
        EL_SPECULAR = 4,
        EL_ALL = 7
    };

    virtual void EnableLighting (int eEnable = EL_ALL) = 0;
    virtual void DisableLighting () = 0;
    virtual void EnableLight (int eEnable, int i, const Light* pkLight) = 0;
    virtual void DisableLight (int i, const Light* pkLight) = 0;

    // texture management
    void EnableTextures ();
    void DisableTextures ();
    virtual void EnableTexture (int iUnit, int i, Effect* pkEffect) = 0;
    virtual void DisableTexture (int iUnit, int i, Effect* pkEffect) = 0;
    virtual void SetActiveTextureUnit (int iUnit) = 0;

    // transformations
    virtual void SetWorldTransformation () = 0;
    virtual void RestoreWorldTransformation () = 0;
    virtual void SetScreenTransformation () = 0;
    virtual void RestoreScreenTransformation () = 0;

    // array management
    virtual void EnableVertices () = 0;
    virtual void DisableVertices () = 0;
    virtual void EnableNormals () = 0;
    virtual void DisableNormals () = 0;
    virtual void EnableColorRGBAs () = 0;
    virtual void DisableColorRGBAs () = 0;
    virtual void EnableColorRGBs () = 0;
    virtual void DisableColorRGBs () = 0;
    virtual void EnableUVs (Vector2fArray* pkUVs) = 0;
    virtual void DisableUVs (Vector2fArray* pkUVs) = 0;
    virtual void DrawElements () = 0;

    // shader management
    virtual void EnableShader (VertexShader* pkVShader) = 0;
    virtual void EnableShader (PixelShader* pkPShader) = 0;
    virtual void DisableShader (VertexShader* pkVShader) = 0;
    virtual void DisableShader (PixelShader* pkPShader) = 0;

    virtual void SetConstantTransformM (int iOption, float* afData) = 0;
    virtual void SetConstantTransformP (int iOption, float* afData) = 0;
    virtual void SetConstantTransformMV (int iOption, float* afData) = 0;
    virtual void SetConstantTransformMVP (int iOption, float* afData) = 0;
    void SetConstantCameraPosition (int iOption, float* afData);
    void SetConstantCameraDirection (int iOption, float* afData);
    void SetConstantCameraUp (int iOption, float* afData);
    void SetConstantCameraRight (int iOption, float* afData);
    void SetConstantFogColor (int iOption, float* afData);
    void SetConstantFogParams (int iOption, float* afData);
    void SetConstantMaterialEmissive (int iOption, float* afData);
    void SetConstantMaterialAmbient (int iOption, float* afData);
    void SetConstantMaterialDiffuse (int iOption, float* afData);
    void SetConstantMaterialSpecular (int iOption, float* afData);
    void SetConstantMaterialShininess (int iOption, float* afData);
    void SetConstantLightPosition (int iOption, float* afData);
    void SetConstantLightDirection (int iOption, float* afData);
    void SetConstantLightAmbient (int iOption, float* afData);
    void SetConstantLightDiffuse (int iOption, float* afData);
    void SetConstantLightSpecular (int iOption, float* afData);
    void SetConstantLightSpotCutoff (int iOption, float* afData);
    void SetConstantLightAttenParams (int iOption, float* afData);

    typedef void (Renderer::*SetConstantFunction)(int,float*);
    static SetConstantFunction ms_aoSCFunction[ShaderConstant::SC_QUANTITY];

    // window parameters
    int m_iWidth, m_iHeight;
    ColorRGB m_kBackgroundColor;

    // camera for establishing view frustum
    Camera* m_pkCamera;

    // values set by derived renderer
    int m_iMaxLights;
    int m_iMaxTextures;
    int m_iMaxStencilIndices;

    // current object and special effects for drawing
    Node* m_pkNode;
    Geometry* m_pkGeometry;
    Effect* m_pkLocalEffect;
    Effect* m_pkGlobalEffect;

    // Fine-tuned control of drawing.  Default 'true' for all flags.
    bool m_bAllowGlobalState;
    bool m_bAllowAlphaState;
    bool m_bAllowCullState;
    bool m_bAllowDitherState;
    bool m_bAllowFogState;
    bool m_bAllowMaterialState;
    bool m_bAllowShadeState;
    bool m_bAllowWireframeState;
    bool m_bAllowZBufferState;
    bool m_bAllowLighting;
    bool m_bAllowNormals;
    bool m_bAllowColors;
    bool m_bAllowTextures;
    bool m_bAllowWorldTransform;

    // support for mirror effects (default 'false')
    bool m_bReverseCullFace;

    // toggle for fullscreen/window mode
    bool m_bFullscreen;

    // current font for text drawing
    int m_iFontID;

    // deferred drawing
    int m_iDeferredQuantity;
    TArray<Spatial*> m_kDeferredObject;
    TArray<bool> m_kDeferredIsGeometry;

// internal use
public:
    // camera access and support
    virtual void OnFrustumChange () = 0;
    virtual void OnViewPortChange () = 0;
    virtual void OnFrameChange () = 0;

    // object drawing
    void Draw (Node* pkNode);
    void Draw (Geometry* pkGeometry);

    // drawing functions
    void DrawPrimitive ();
    void DrawShader ();
    virtual void DrawBumpMap () = 0;
    virtual void DrawEnvironmentMap () = 0;
    virtual void DrawGlossMap () = 0;
    virtual void DrawPlanarReflection () = 0;
    virtual void DrawPlanarShadow () = 0;
    virtual void DrawProjectedTexture () = 0;
};

#include "Wm3Renderer.inl"

}

#endif
