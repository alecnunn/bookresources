// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3DXRENDERER_H
#define WM3DXRENDERER_H

#include "Wm3RendererLibType.h"
#include "Wm3Renderer.h"
#include "Wm3Light.h"
#include "Wm3Texture.h"
#include "Wm3Geometry.h"
#include "Wm3VertexShader.h"
#include "Wm3PixelShader.h"
#include "Wm3Camera.h"
#include "Wm3DxApplyParameters.h"

namespace Wm3
{

class WM3_RENDERER_ITEM DxRenderer : public Renderer
{
public:
    // construction
    DxRenderer (HWND hWnd, int iWidth, int iHeight);
    virtual ~DxRenderer();

    virtual bool BeginScene ();
    virtual void EndScene ();

    // full window buffer operations
    virtual void ClearBackBuffer ();
    virtual void ClearZBuffer ();
    virtual void ClearStencilBuffer ();
    virtual void ClearBuffers ();
    virtual void DisplayBackBuffer ();

    // clear the buffer in the specified subwindow
    virtual void ClearBackBuffer (int iXPos, int iYPos, int iWidth,
        int iHeight);
    virtual void ClearZBuffer (int iXPos, int iYPos, int iWidth, int iHeight);
    virtual void ClearStencilBuffer (int iXPos, int iYPos, int iWidth,
        int iHeight);
    virtual void ClearBuffers (int iXPos, int iYPos, int iWidth, int iHeight);

    // specialized drawing
    virtual bool SelectFont (int iFontID);
    int LoadFont (const char* acFace, int iSize, bool bBold, bool bItalic);
    void UnloadFont (int iFontID);
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
    void EstablishCapabilities ();

    // camera updates
    virtual void OnFrustumChange ();
    virtual void OnViewPortChange ();
    virtual void OnFrameChange ();

    // model-to-world transformation handling
    void SetWorldMatrix ();
    float m_afWorldMatrix[16];

    // device management
    virtual void OnDeviceLost ();
    virtual void ResetDevice ();
    IDevice* GetDevice ();

    // global render state management
    virtual void InitializeState ();
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
    virtual ITexture* CreateTexture (Image* pkImage);
    virtual void SetActiveTextureUnit (int iUnit);

    // transformations
    virtual void SetWorldTransformation ();
    virtual void RestoreWorldTransformation ();
    virtual void SetScreenTransformation ();
    virtual void RestoreScreenTransformation ();

    // Array management.  Calling these will enable or disable components of
    // the current geometry object.
    virtual void EnableVertices ();
    virtual void DisableVertices ();
    virtual void EnableNormals ();
    virtual void DisableNormals ();
    virtual void EnableColorRGBAs ();
    virtual void DisableColorRGBAs ();
    virtual void EnableColorRGBs ();
    virtual void DisableColorRGBs ();
    void EnableUVs (Vector2fArray* pkUVs);
    void DisableUVs (Vector2fArray* pkUVs);
    virtual void DrawElements ();

    // Array management member variables.  The pointers are set to the current
    // data (enabled) or null (disabled).
    Vector3fArray* m_pkCurVertices;
    Vector3fArray* m_pkCurNormals;
    ColorRGBArray* m_pkCurColorRGBs;
    ColorRGBAArray* m_pkCurColorRGBAs;
    Vector2fArray** m_apkCurUVs;
    bool* m_abTexUnitEnabled;

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

    // member variables
    IMainPtr m_pqMain;
    IDevicePtr m_pqDevice;
    D3DPRESENT_PARAMETERS m_kPresent;
    bool m_bDeviceLost;
    bool m_bCursorVisible;
    RECT m_rcWindow;

    // which texture unit enable/disable UVs applies to
    int m_iCurTextureUnit;
    bool m_bSpecularEnable;

    D3DXMATRIX m_kViewMatrix;
    D3DXMATRIX m_kProjMatrix;

    // Simulate a matrix "stack," such as what OpenGL has.
    // At the moment, only one matrix can be stored and
    // restored.
    void StoreWorldMatrix ();
    void RestoreWorldMatrix ();
    float m_afStoredMatrix[16];

    // font handling
    TArray<IFontPtr> m_kFontArray;

    // maintain a list of all textures, so that they can be released on exit
    TArray<ITexture*> m_kTextures;

    // for error checking and exception handling
    static HRESULT ms_hResult;

    static D3DPRIMITIVETYPE ms_aeObjectType[Geometry::GT_MAX_QUANTITY];
    static DWORD ms_adwAlphaSrcBlend[];
    static DWORD ms_adwAlphaDstBlend[];
    static DWORD ms_adwAlphaTest[];
    static DWORD ms_adwCullType[];
    static DWORD ms_adwFogDensity[];
    static DWORD ms_adwShadeMode[];
    static DWORD ms_adwZBufferCompare[];
    static DWORD ms_adwTexMagFilter[];
    static DWORD ms_adwTexMinFilter[];
    static DWORD ms_adwTexMipFilter[];
    static DWORD ms_adwTexWrapMode[];
    static DWORD ms_adwTexCombineSource[];
    static DWORD ms_adwTexCombineOperand[];
    static DWORD ms_adwTexFormat[];
    static int ms_aiTexSize[];

    // set up texture stage to pass color and alpha to next stage
    void SetTextureStageApplyPass (const int iUnit);

private:
    static const ApplyParameters ms_kApPass;
    static const ApplyParameters ms_kApColorReplace;
    static const ApplyParameters ms_kApColorDecal;
    static const ApplyParameters ms_kApColorModulate;
    static const ApplyParameters ms_kApColorBlend;
    static const ApplyParameters ms_kApColorAdd;
    static const ApplyParameters ms_kApAlphaReplace;
    static const ApplyParameters ms_kApAlphaDecal;
    static const ApplyParameters ms_kApAlphaModulate;
    static const ApplyParameters ms_kApAlphaBlend;
    static const ApplyParameters ms_kApAlphaAdd;

    static const float ms_fSqrtMaxReal;
};

}

#endif
