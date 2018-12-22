// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3OPENGLRENDERER_H
#define WM3OPENGLRENDERER_H

#include "Wm3RendererLibType.h"
#include "Wm3Renderer.h"
#include "Wm3OpenGLIncludes.h"
#include "Wm3Light.h"
#include "Wm3Texture.h"
#include "Wm3Geometry.h"
#include "Wm3VertexShader.h"
#include "Wm3PixelShader.h"

namespace Wm3
{

class WM3_RENDERER_ITEM OpenGLRenderer : public Renderer
{
public:
    // driver information
    static const char* GetVendor ();
    static const char* GetRenderer ();
    static const char* GetVersion ();
    static const char* GetGluVersion ();
    static const char* GetExtensions ();
    static bool IsMinimumVersion (int iMajor, int iMinor, int iRelease);

    // Test if a specified extension is supported.  If you need to test for
    // multiple extensions, the function must be called for each extension.
    // The extensions could vary per rendering context, so check for
    // extensions immediately after setting the context.
    static bool ExtensionSupported (const char* acExt);

    // Call this after the rendering context has been initialized.  The order
    // is necessary since this function will call ExtensionsSupported, a call
    // that requires an active rendering context.  If certain capabilities
    // must be established that are specific to a derived class, that class
    // is responsible for implementing its own query function and calling it
    // at the correct time.
    virtual void EstablishCapabilities ();

    // Abstract base class.  The InitializeState call is made by derived
    // class constructors after an OpenGL context is created.
    virtual ~OpenGLRenderer();
    void InitializeState ();

    virtual void SetBackgroundColor (const ColorRGB& rkColor);

    // full window buffer operations
    virtual void ClearBackBuffer ();
    virtual void ClearZBuffer ();
    virtual void ClearStencilBuffer ();
    virtual void ClearBuffers ();
    virtual void DisplayBackBuffer () = 0;

    // clear the buffer in the specified subwindow
    virtual void ClearBackBuffer (int iXPos, int iYPos, int iWidth,
        int iHeight);
    virtual void ClearZBuffer (int iXPos, int iYPos, int iWidth,
        int iHeight);
    virtual void ClearStencilBuffer (int iXPos, int iYPos, int iWidth,
        int iHeight);
    virtual void ClearBuffers (int iXPos, int iYPos, int iWidth,
        int iHeight);

    // specialized drawing
    virtual bool SelectFont (int iFontID);
    virtual void UnloadFont (int iFontID);
    virtual void Draw (int iX, int iY, const ColorRGBA& rkColor,
        const char* acText);
    virtual void Draw (const unsigned char* aucBuffer);

    // management of texture resources
    virtual void ReleaseTexture (Texture* pkTexture);

    // management of array resources
    virtual void ReleaseArray (CachedColorRGBAArray* pkArray);
    virtual void ReleaseArray (CachedColorRGBArray* pkArray);
    virtual void ReleaseArray (CachedIntArray* pkArray);
    virtual void ReleaseArray (CachedVector2fArray* pkArray);
    virtual void ReleaseArray (CachedVector3fArray* pkArray);

    // management of shader resources
    virtual void ReleaseShader (Shader* pkShader);

protected:
    // construction
    OpenGLRenderer (int iWidth, int iHeight);

    // camera updates
    virtual void OnFrustumChange ();
    virtual void OnViewPortChange ();
    virtual void OnFrameChange ();

    // model-to-world transformation handling
    void SetWorldMatrix ();
    float m_afWorldMatrix[16];

    // global render state management
    virtual void SetAlphaState (AlphaState* pkState);
    virtual void SetCullState (CullState* pkState);
    virtual void SetDitherState (DitherState* pkState);
    virtual void SetFogState (FogState* pkState);
    virtual void SetMaterialState (MaterialState* pkState);
    virtual void SetShadeState (ShadeState* pkState);
    virtual void SetWireframeState (WireframeState* pkState);
    virtual void SetZBufferState (ZBufferState* pkState);

    // light management
    virtual void EnableLighting (int eEnable = EL_ALL);
    virtual void DisableLighting ();
    virtual void EnableLight (int eEnable, int i, const Light* pkLight);
    virtual void DisableLight (int i, const Light* pkLight);

    // texture management
    virtual void EnableTexture (int iUnit, int i, Effect* pkEffect);
    virtual void DisableTexture (int iUnit, int i, Effect* pkEffect);
    virtual void SetActiveTextureUnit (int iUnit);

    // transformations
    virtual void SetWorldTransformation ();
    virtual void RestoreWorldTransformation ();
    virtual void SetScreenTransformation ();
    virtual void RestoreScreenTransformation ();

    // array management
    virtual void EnableVertices ();
    virtual void DisableVertices ();
    virtual void EnableNormals ();
    virtual void DisableNormals ();
    virtual void EnableColorRGBAs ();
    virtual void DisableColorRGBAs ();
    virtual void EnableColorRGBs ();
    virtual void DisableColorRGBs ();
    virtual void EnableUVs (Vector2fArray* pkUVs);
    virtual void DisableUVs (Vector2fArray* pkUVs);
    virtual void DrawElements ();

    // shader management
    virtual void EnableShader (VertexShader* pkVShader);
    virtual void EnableShader (PixelShader* pkPShader);
    virtual void DisableShader (VertexShader* pkVShader);
    virtual void DisableShader (PixelShader* pkPShader);
    virtual void SetConstantTransformM (int iOption, float* afData);
    virtual void SetConstantTransformP (int iOption, float* afData);
    virtual void SetConstantTransformMV (int iOption, float* afData);
    virtual void SetConstantTransformMVP (int iOption, float* afData);
    void GetTransform (Matrix4f& rkMat, int iOption, float* afData);

    // effects
    virtual void DrawBumpMap ();
    virtual void DrawEnvironmentMap ();
    virtual void DrawGlossMap ();
    virtual void DrawPlanarReflection ();
    virtual void DrawPlanarShadow ();
    virtual void DrawProjectedTexture ();

    // display list base indices for fonts/characters
    class DisplayListInfo
    {
    public:
        int Quantity;  // number of display lists, input to glGenLists
        int Start;     // start index, output from glGenLists
        int Base;      // base index for glListBase
    };
    TArray<DisplayListInfo> m_kDLInfo;

    // *** maps from Wild Magic enums to OpenGL enums ***

    // object types
    static GLenum ms_aeObjectType[Geometry::GT_MAX_QUANTITY];
    
    // global render state
    static GLenum ms_aeAlphaSrcBlend[AlphaState::SBF_QUANTITY];
    static GLenum ms_aeAlphaDstBlend[AlphaState::DBF_QUANTITY];
    static GLenum ms_aeAlphaTest[AlphaState::TF_QUANTITY];
    static GLenum ms_aeFrontFace[CullState::FT_QUANTITY];
    static GLenum ms_aeCullFace[CullState::CT_QUANTITY];
    static GLenum ms_aeFogDensity[FogState::DF_QUANTITY];
    static GLenum ms_aeFogApply[FogState::AF_QUANTITY];
    static GLenum ms_aeShade[ShadeState::SM_QUANTITY];
    static GLenum ms_aeZBufferCompare[ZBufferState::CF_QUANTITY];

    // texture effects
    static GLenum ms_aeTextureCorrection[Texture::CM_QUANTITY];
    static GLenum ms_aeTextureApply[Texture::AM_QUANTITY];
    static GLenum ms_aeTextureFilter[Texture::FM_QUANTITY];
    static GLenum ms_aeTextureCombineFunc[Texture::ACF_QUANTITY];
    static GLenum ms_aeTextureCombineSrc[Texture::ACS_QUANTITY];
    static GLenum ms_aeTextureCombineOperand[Texture::ACO_QUANTITY];
    static GLint  ms_afTextureCombineScale[Texture::ACSC_QUANTITY];
    static GLenum ms_aeTextureMipmap[Texture::MM_QUANTITY];

    // images
    static GLenum ms_aeImageComponents[Image::IT_QUANTITY];
    static GLenum ms_aeImageFormats[Image::IT_QUANTITY];
};

}

#endif
