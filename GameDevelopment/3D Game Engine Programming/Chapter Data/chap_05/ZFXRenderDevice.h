// File: ZFXRenderDevice.h
#ifndef ZFXRENDERINTERFACE_H
#define ZFXRENDERINTERFACE_H


// I N C L U D E S /////////////////////////////////////////////////

#include <windows.h>          // windows types and stuff
#include <stdio.h>            // file stuff
#include "ZFXD3D/zfx.h"       // MATERIAL structure
#include "zfx3d.h"            // zfx 3d math library


// D E F I N E S ///////////////////////////////////////////////////



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
      UINT         m_nNumTextures;        // number (IDs) of materials
      ZFXSKIN     *m_pSkins;              // allocate mem for skins here
      ZFXMATERIAL *m_pMaterials;          // allocate mem for materials here
      ZFXTEXTURE  *m_pTextures;           // allocate mem for textures here


   public:
      ZFXSkinManager(void) {};
      virtual ~ZFXSkinManager(void) {};

      virtual HRESULT AddSkin(const ZFXCOLOR *pcAmbient,
                              const ZFXCOLOR *pcDiffuse,
                              const ZFXCOLOR *pcEmissive,
                              const ZFXCOLOR *pcSpecular,
                              float fSpecPower,
                              UINT *nSkinID)=0;

      virtual HRESULT AddTexture(UINT nSkinID, const char *chName,
                                 bool bAlpha, float fAlpha,
                                 ZFXCOLOR *cColorKeys,
                                 DWORD dwNumColorKeys)=0;

      virtual bool    MaterialEqual(const ZFXMATERIAL *pMat0, 
                                    const ZFXMATERIAL *pMat1)=0;

      virtual void    LogCurrentStatus(char *chLog, 
                                       bool bDetailed)=0;
   };
/*----------------------------------------------------------------*/


/**
 * ZFXRenderDevice is an abstract class which provides an interface
 * from which render dll's can inherit. This secures availability
 * of the necessary render functions.
 */
class ZFXRenderDevice {
   protected:
      HWND      m_hWnd;             // 3D render window handle
      HINSTANCE m_hDLL;             // dll module handle
      DWORD     m_dwWidth;          // screen width
      DWORD     m_dwHeight;         // screen height
      bool      m_bWindowed;        // windowed mode?
      char      m_chAdapter[256];   // graphics adapter name
      bool      m_bRunning;         // after succesful initialization
      FILE     *m_pLog;             // log file

      float     m_fNear,            // near clipping plane
                m_fFar;             // far clipping plane
      int       m_nMode;            // 0=perspective, 1=ortho
      int       m_nStage;           // stage (0-3) for viewport/projection
      RECT      m_rcVP[4];          // viewports for all 4 stages

      
   public:
      ZFXSkinManager *m_pSkinMan;   // material and textures

      ZFXRenderDevice(void) {};
      virtual ~ZFXRenderDevice(void) {};
      

      // INIT/RELEASE STUFF:
      // ===================
      
      // initialize engine, should first display dialog to select
      // graphics adapter and mode, and then start the 3D API
      virtual HRESULT Init(HWND, HWND, int, int, bool)=0;
      
      // release API specific stuff
      virtual void Release(void)=0;
      
      // is initialized?
      virtual bool IsRunning(void)=0;
      
      

      // MISC OPERATIONS:
      // ================

      // set view matrix from cam's vRight, vUp, vDir, vPos
      virtual HRESULT SetView3D(const ZFXVector&, const ZFXVector&, 
                                const ZFXVector&, const ZFXVector&)=0;

      // set look at matrix: from, at, world up
      virtual HRESULT SetViewLookAt(const ZFXVector&, const ZFXVector&, 
                                    const ZFXVector&)=0;

      // set engines near and far clipping plane
      virtual void    SetClippingPlanes(float, float)=0;

      // set mode for stage n, 0:=3D(perspective), 1:=2D(orthogonal)
      virtual HRESULT SetMode(int, int n)=0;

      // set perspective FOV and viewport for stage n
      virtual HRESULT SetFOV(float, RECT*, int n)=0;

      // get frustrum planes
      virtual HRESULT GetFrustrum(ZFXPlane*)=0;

      // screen to worldray, give 2 vectors for output
      virtual void Transform2Dto3D(const POINT &pt, 
                                   ZFXVector *vcO, 
                                   ZFXVector *vcD)=0;

      // cast world position to screen coordinates
      virtual POINT Transform3Dto2D(const ZFXVector &vcP)=0;



      // RENDERING STUFF:
      // ================
      
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

      // Fade screen to given color with given intensity
      virtual void FadeScreen(float fR, float fG, float fB, float fA)=0;

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