// File: ZFXRenderDevice.h
#ifndef ZFXRENDERINTERFACE_H
#define ZFXRENDERINTERFACE_H


// I N C L U D E S /////////////////////////////////////////////////

#include <windows.h>          // windows types and stuff
#include <stdio.h>            // file stuff
#include "zfx3d.h"            // zfx 3d math library
#include "zfx.h"              // MATERIAL structure


// D E F I N E S ///////////////////////////////////////////////////

#define MAX_3DHWND   8
#define MAX_SHADER  20


typedef enum ZFXAXIS_TYPE { X_AXIS, Y_AXIS, Z_AXIS } ZFXAXIS;


// S T R U C T S ///////////////////////////////////////////////////

/**
 * ZFXMaterialManager is an abstract class which provides an interface
 * from which material managers can inherit. Those heirs need to 
 * implement a store for texture data in a way appropriate to the
 * API.
 */
class ZFXSkinManager {
   protected:
      UINT         m_nNumSkins;           // number (IDs) of skins
      UINT         m_nNumMaterials;       // number (IDs) of materials
      UINT         m_nNumTextures;        // number (IDs) of textures
      ZFXSKIN     *m_pSkins;              // allocate mem for skins here
      ZFXMATERIAL *m_pMaterials;          // allocate mem for materials here
      ZFXTEXTURE  *m_pTextures;           // allocate mem for textures here


   public:
      ZFXSkinManager(void) {};
      virtual ~ZFXSkinManager(void) {};

      virtual void        Reset(void)=0;

      virtual HRESULT     AddSkin(const ZFXCOLOR *pcAmbient,
                                  const ZFXCOLOR *pcDiffuse,
                                  const ZFXCOLOR *pcEmissive,
                                  const ZFXCOLOR *pcSpecular,
                                  float fSpecPower,
                                  UINT *nSkinID)=0;

      virtual HRESULT     AddTexture(UINT nSkinID, const char *chName,
                                     bool bAlpha, float fAlpha,
                                     ZFXCOLOR *cColorKeys,
                                     DWORD dwNumColorKeys)=0;

      virtual HRESULT     AddTextureHeightmapAsBump(UINT nSkinID,
                                     const char *chName)=0;

      virtual HRESULT     ExchangeTexture(UINT nSkinID, UINT nTexStage,
                                     const char *chName,
                                     bool bAlpha, float fAlpha,
                                     ZFXCOLOR *cColorKeys,
                                     DWORD dwNumColorKeys)=0;

      virtual HRESULT     ExchangeMaterial(UINT nSkinID,
                                       const ZFXCOLOR *pcAmb,
                                       const ZFXCOLOR *pcDif,
                                       const ZFXCOLOR *pcEmis,
                                       const ZFXCOLOR *pcSpec,
                                       float fSpecPower)=0;

      virtual bool        MaterialEqual(const ZFXMATERIAL *pMat0, 
                                        const ZFXMATERIAL *pMat1)=0;

      virtual bool        ColorEqual(const ZFXCOLOR *pCol0, 
                                     const ZFXCOLOR *pCol1)=0;

      virtual void        LogCurrentStatus(char *chLog, 
                                           bool bDetailed)=0;

      virtual UINT        GetNumSkins(void)=0;

      virtual ZFXSKIN     GetSkin(UINT nSkinID)=0;

      virtual ZFXMATERIAL GetMaterial(UINT nMatID)=0;

      virtual const char* GetTextureName(UINT nTexID, float *fAlpha, 
                                         ZFXCOLOR *AlphaKeys, 
                                         UCHAR *NumKeys)=0;
   };
/*----------------------------------------------------------------*/


/**
 *
 */
class ZFXVertexCacheManager {
   public:
      ZFXVertexCacheManager(void) {};
      virtual ~ZFXVertexCacheManager(void) {};

      virtual HRESULT CreateStaticBuffer(ZFXVERTEXID VertexID,
                                 UINT  nSkinID,
                                 UINT  nVerts, 
                                 UINT  nIndis, 
                                 const void *pVerts,
                                 const WORD *pIndis,
                                 UINT *pnID)=0;

      virtual HRESULT CreateIndexBuffer(UINT nIndis,
                                        const WORD *pIndis,
                                        UINT *pnID)=0;
      
      virtual HRESULT Render(ZFXVERTEXID VertexID,
                             UINT nVerts, 
                             UINT nIndis, 
                             const void *pVerts,
                             const WORD *pIndis,
                             UINT  SkinID)=0;

      virtual HRESULT RenderNaked(UINT nVerts, 
                                  const void *pVerts,
                                  bool bTextured)=0;

      virtual HRESULT Render(UINT nSBufferID)=0;

      virtual HRESULT Render(UINT SBID, UINT IBID, UINT Skin)=0;

      virtual HRESULT Render(UINT nSBufferID, UINT SkinID, 
                             UINT StartIndex, UINT NumVerts,
                             UINT NumTrims)=0;

      virtual HRESULT RenderPoints(ZFXVERTEXID     VertexID,
                                   UINT            nVerts,
                                   const void     *pVerts,
                                   const ZFXCOLOR *pClrl)=0;

      virtual HRESULT RenderLines(ZFXVERTEXID     VertexID,
                                  UINT            nVerts,
                                  const void     *pVerts,
                                  const ZFXCOLOR *pClrl,
                                  bool            bStrip)=0;

      virtual HRESULT RenderLine(const float *fStart,
                                 const float *fEnd, 
                                 const ZFXCOLOR *pClr)=0;

      virtual HRESULT ForcedFlushAll(void)=0;

      virtual HRESULT ForcedFlush(ZFXVERTEXID)=0;

      virtual void    InvalidateStates(void)=0;

      virtual ZFXRENDERSTATE GetShadeMode(void)=0;
   }; // class
/*----------------------------------------------------------------*/


/**
 * ZFXRenderDevice is an abstract class which provides an interface
 * from which render dll's can inherit. This secures availability
 * of the necessary render functions.
 */
class ZFXRenderDevice {
   protected:
      HWND            m_hWndMain;          // application main window
      HWND            m_hWnd[MAX_3DHWND];  // 3D render window handle
      UINT            m_nNumhWnd;          // number of hwnds in array
      UINT            m_nActivehWnd;       // which one is active (0=main hWnd)
      HINSTANCE       m_hDLL;              // dll module handle
      DWORD           m_dwWidth;           // screen width
      DWORD           m_dwHeight;          // screen height
      bool            m_bWindowed;         // windowed mode?
      char            m_chAdapter[256];    // graphics adapter name
      bool            m_bRunning;          // after succesful initialization
      bool            m_bUseShaders;       // shaders or fixed function pipeline
      bool            m_bCanDoShaders;     // hw can do shaders version 1.1
      bool            m_bAdditive;         // use additive rendering
      bool            m_bColorBuffer;      // use color buffer
      ZFXRENDERSTATE  m_ShadeMode;         // wireframe rendering?
      FILE           *m_pLog;              // log file
      ZFXCOLOR        m_clrWire;           // color for wireframe rendering

      float           m_fNear,             // near clipping plane
                      m_fFar;              // far clipping plane
      ZFXENGINEMODE   m_Mode;              // persp., ortho or 2d
      int             m_nStage;            // stage (0-3) for viewport/projection
      ZFXVIEWPORT     m_VP[4];             // viewports for all 4 stages

      ZFXSkinManager        *m_pSkinMan;   // material and textures
      ZFXVertexCacheManager *m_pVertexMan; // manage vertices/indices
      

   public:
      ZFXRenderDevice(void) {};
      virtual ~ZFXRenderDevice(void) {};

      virtual ZFXSkinManager* GetSkinManager(void)=0;
      virtual ZFXVertexCacheManager* GetVertexManager(void)=0;
      

      // INIT/RELEASE STUFF:
      // ===================
      
      // initialize engine, should first display dialog to select
      // graphics adapter and mode, and then start the 3D API
      virtual HRESULT Init(HWND, const HWND*, int, int, int, bool)=0;
      virtual HRESULT InitWindowed(HWND, const HWND*, int, bool)=0;
      
      // release API specific stuff
      virtual void Release(void)=0;
      
      // is initialized?
      virtual bool IsRunning(void)=0;

      virtual bool IsWindowed(void)=0;
      virtual void GetResolution(POINT*)=0;
      
      

      // MISC OPERATIONS:
      // ================

      // activate or deactivate shaders
      virtual bool    UsesShaders(void)=0;
      virtual void    UseShaders(bool)=0;
      virtual bool    CanDoShaders(void)=0;
      virtual HRESULT SetShaderConstant(ZFXSHADERTYPE,ZFXDATATYPE,
                                        UINT,UINT,const void*)=0;

      // activate additive blending
      virtual void    UseAdditiveBlending(bool)=0;
      virtual bool    UsesAdditiveBlending(void)=0;

      // set view matrix from cam's vRight, vUp, vDir, vPos
      virtual HRESULT SetView3D(const ZFXVector&, const ZFXVector&, 
                                const ZFXVector&, const ZFXVector&)=0;

      // set look at matrix: from, at, world up
      virtual HRESULT SetViewLookAt(const ZFXVector&, const ZFXVector&, 
                                    const ZFXVector&)=0;

      // set engines near and far clipping plane
      virtual void    SetClippingPlanes(float, float)=0;

      // set mode for stage n, 0:=3D(perspective), 1:=2D(orthogonal)
      virtual HRESULT SetMode(ZFXENGINEMODE, int n)=0;

      virtual void    SetOrthoScale(float fScale, int nStage)=0;

      // set perspective FOV and viewport for stage n
      virtual HRESULT InitStage(float, ZFXVIEWPORT*, int n)=0;

      // get frustrum planes
      virtual HRESULT GetFrustrum(ZFXPlane*)=0;

      // screen to worldray, give 2 vectors for output
      virtual void    Transform2Dto3D(const POINT &pt, 
                                      ZFXVector *vcO, 
                                      ZFXVector *vcD)=0;

      // cast cursor position to ortho world space
      virtual ZFXVector Transform2Dto2D(UINT nHwnd,float,const POINT*,ZFXAXIS)=0;

      // cast world position to screen coordinates
      virtual POINT Transform3Dto2D(const ZFXVector &vcP)=0;

      // set world transformation matrix or NULL
      virtual void SetWorldTransform(const ZFXMatrix*)=0;

      // activate backface culling
      virtual void SetBackfaceCulling(ZFXRENDERSTATE)=0;

      // activate stencil buffer
      virtual void SetStencilBufferMode(ZFXRENDERSTATE,DWORD)=0;

      // activate stencil shadow volume settings
      virtual void UseStencilShadowSettings(bool)=0;

      // activate color buffer
      virtual void UseColorBuffer(bool)=0;
      virtual bool UsesColorBuffer(void)=0;

      // activate usage of texture
      virtual void UseTextures(bool)=0;
      virtual bool UsesTextures(void)=0;

      // activate depth buffer
      virtual void SetDepthBufferMode(ZFXRENDERSTATE)=0;
//      virtual void SetDepthBiasValue(UINT)=0;

      // activate wireframe mode
      virtual void SetShadeMode(ZFXRENDERSTATE,float,const ZFXCOLOR*)=0;
      virtual ZFXRENDERSTATE GetShadeMode(void)=0;

      virtual HRESULT SetTextureStage(UCHAR, ZFXRENDERSTATE)=0;

      virtual HRESULT SetLight(const ZFXLIGHT*, UCHAR)=0;


      // SHADER STUFF:
      // =============

      // create shaders
      virtual HRESULT CreateVShader(const void *pData, 
                                    UINT nSize,
                                    bool bLoadFromFile,
                                    bool bIsCompiled,
                                    UINT *pID)=0;
      virtual HRESULT CreatePShader(const void *pData, 
                                    UINT nSize,
                                    bool bLoadFromFile,
                                    bool bIsCompiled,
                                    UINT *pID)=0;
      
      virtual HRESULT ActivateVShader(UINT id, ZFXVERTEXID VertexID)=0;
      virtual HRESULT ActivatePShader(UINT id)=0;


      // RENDERING STUFF:
      // ================
      
      // switch swap chain to hwnd in array from Init()
      virtual HRESULT UseWindow(UINT nHwnd)=0;

      // clear buffers and prepare for rendering
      virtual HRESULT BeginRendering(bool bClearPixel, 
                                     bool bClearDepth, 
                                     bool bClearStencil)=0;
      
      // end rendering and flip pixel buffer to front
      virtual void EndRendering(void)=0;

      // clear pixel-, depth-, stencil-buffer
      virtual HRESULT Clear(bool, bool, bool)=0;
      
      // change background color
      virtual void SetClearColor(float fRed, float fGreen, float fBlue)=0;

      // fade screen to given color with given intensity
      virtual void FadeScreen(float fR,float fG,float fB,float fA)=0;

      // type like "Arial", nWeight (0=default,700=bold), italic?, 
      // underline?, strike out?, size, return ID
      virtual HRESULT CreateFont(const char*,int,bool,bool,bool,DWORD,UINT*)=0;

      // draw text: font id, x, y, r, g, b, format string, variables
      virtual HRESULT DrawText(UINT,int,int,UCHAR,UCHAR,UCHAR,char*,...)=0;



      // LIGHT STUFF:
      // ============
      
      // set ambient light level
      virtual void SetAmbientLight(float fRed, float fGreen, float fBlue)=0;

   }; // class

typedef class ZFXRenderDevice *LPZFXRENDERDEVICE;
/*----------------------------------------------------------------*/


extern "C" {
   HRESULT CreateRenderDevice(HINSTANCE hDLL, ZFXRenderDevice **pInterface);
   typedef HRESULT (*CREATERENDERDEVICE)(HINSTANCE hDLL, ZFXRenderDevice **pInterface);
   
   HRESULT ReleaseRenderDevice(ZFXRenderDevice **pInterface);
   typedef HRESULT (*RELEASERENDERDEVICE)(ZFXRenderDevice **pInterface);
   }
/*----------------------------------------------------------------*/

#endif