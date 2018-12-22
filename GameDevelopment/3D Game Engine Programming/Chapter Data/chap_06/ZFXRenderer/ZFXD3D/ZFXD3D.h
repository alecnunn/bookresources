/*******************************************************************
 * ZFXEngine!                                                      *
 * (c)2003 by Stefan Zerbst | www.zfx.info                         *
 *-----------------------------------------------------------------*
 * File: ZFXD3D.h                                                  *
 * part of render dll implementing direct3d rendering              *
 *******************************************************************/

#ifndef ZFXD3D_H
#define ZFXD3D_H


// I N C L U D E S /////////////////////////////////////////////////

#include <d3d9.h>                  // direct3d 9 interface
#include <d3dx9.h>                 // font object, shader compiler
#include <windows.h>               // windows types and stuff
#include "..\ZFXRenderDevice.h"    // Interface Schablone
#include "..\zfx3d.h"              // zfx 3d math library
#include "ZFXD3D_vcache.h"         // vertex cache (dynamic IB/VB)


BOOL WINAPI DllEntryPoint( HINSTANCE hDll,
                           DWORD     fdwReason,
                           LPVOID    lpvRserved );


// L I N K S ///////////////////////////////////////////////////////

#pragma comment(lib, "vfw32.lib")   
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "..\\zfx3d.lib")


#define LPDIRECT3DVDECL9 LPDIRECT3DVERTEXDECLARATION9

#define IDENTITY(m) {memset(&m,0,sizeof(D3DMATRIX));m._11=m._22=m._33=m._44=1.0f;}


#define FVF_VERTEX  (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)
#define FVF_PVERTEX (D3DFVF_XYZ)
#define FVF_LVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#define FVF_CVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)
#define FVF_T3VERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX3)
#define FVF_TVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE3(0))

// S T R U C T S ///////////////////////////////////////////////////

// one for each AdapterFormat-BackbufferFormat-WindowMode
// (windowed or fullscreen) combination that is valid
struct ZFXCOMBOINFO {
   UINT       nAdapter;             // belongs to
   D3DDEVTYPE d3dDevType;           // HAL, SW, REF
   bool       bWindowed;            // windowed mode

   D3DFORMAT  fmtAdapter;           // pixelbuffer
   D3DFORMAT  fmtBackBuffer;        // backbuffer
   D3DFORMAT  fmtDepthStencil;      // z/stencil format

   DWORD      dwBehavior;           // vertex processing
   D3DMULTISAMPLE_TYPE msType;      // multisample type
   };
/*----------------------------------------------------------------*/

// up to three for each adapter
struct ZFXDEVICEINFO {
   UINT         nAdapter;        // belongs to
   D3DDEVTYPE   d3dDevType;      // HAL, SW, REF
   D3DCAPS9     d3dCaps;         // capabilites
   ZFXCOMBOINFO d3dCombo[80];    // combo
   UINT         nNumCombo;       // number of combos
   };
/*----------------------------------------------------------------*/

struct ZFXADAPTERINFO {
   D3DADAPTER_IDENTIFIER9 d3dAdapterIdentifier;
   UINT                   nAdapter;         // which one
   D3DDISPLAYMODE         d3ddspmd[150];    // display modes
   UINT                   nNumModes;        // number of modes
   ZFXDEVICEINFO          d3dDevs[3];       // list of devices
   UINT                   nNumDevs;         // number of devices
   };
/*----------------------------------------------------------------*/

// helper functions
TCHAR*    D3DDevTypeToString(D3DDEVTYPE devType);
TCHAR*    D3DFormatToString(D3DFORMAT format);
TCHAR*    BehaviorTypeToString(DWORD vpt);
void*     GetSelectedItem(HWND hWnd);
void      AddItem(HWND hWnd, char *ch, void *pData);
bool      ContainsString(HWND hWnd, char *ch);


// enumeration stuff
class ZFXD3DEnum {
   public:
      ZFXD3DEnum(int nMinDepth, int nMinStencil)  
         { m_nMinDepth=nMinDepth;  m_nMinStencil=nMinStencil; }
      ~ZFXD3DEnum(void) { ;/* nothing to do */ }

      // enumerate all the stuff
      HRESULT Enum(HWND,HWND,HWND,HWND,HWND,HWND,HWND,FILE*);

      // combobox selection changed
      void ChangedAdapter(void);
      void ChangedDevice(void);
      void ChangedAdapterFmt(void);
      void ChangedWindowMode(void);

      // get final settings to crank up
      void GetSelections(ZFXDEVICEINFO *pD, D3DDISPLAYMODE *dspmd,
                         D3DFORMAT *fmtA, D3DFORMAT *fmtB);


      LPDIRECT3D9    m_pD3D;
      ZFXADAPTERINFO m_xAdapterInfo[10];
      DWORD          m_dwNumAdapters;
      

   private:
      D3DDISPLAYMODE m_dspmd;    // current desktop display mode
      D3DFORMAT m_fmtAdapter[5]; // list of possible adapter formats
      UINT      m_nNumFmt;       // adapter formats possible
      UINT      m_nMinWidth;     // minimum screen with
      UINT      m_nMinHeight;    // minimum screen height
      UINT      m_nMinBits;      // minimum backbuffer bits
      UINT      m_nMinDepth;     // minimum depth bits
      UINT      m_nMinStencil;   // minimum stencil bits
      FILE     *m_pLog;          // log file opened by zfxd3d class

      // handle to GUI items given from zfxd3d dialog
      HWND m_hADAPTER;           // adapter combobox 
      HWND m_hMODE;              // mode combobox
      HWND m_hDEVICE;            // device combobox
      HWND m_hADAPTERFMT;        // adapter format combobox
      HWND m_hBACKFMT;           // backbuffer fmt combobox
      HWND m_hWND;               // radiobtn windowed
      HWND m_hFULL;              // radiobtn fullscreen
      
      HRESULT EnumAdapters(void);
      HRESULT EnumDevices(ZFXADAPTERINFO &);
      HRESULT EnumCombos(ZFXDEVICEINFO &);

      UINT    GetBits(D3DFORMAT);
      HRESULT ConfirmDevice(D3DCAPS9*,DWORD,D3DFORMAT);
      bool    ConfirmDepthFmt(ZFXCOMBOINFO*);

   }; // class
/*----------------------------------------------------------------*/


/**
 * Class definition for a Direct3D render device, implementing the
 * interface ZFXRenderDevice
 */
class ZFXD3D : public ZFXRenderDevice {
   public:
      ZFXD3D(HINSTANCE hDLL);
      ~ZFXD3D(void);
      
      ZFXSkinManager* GetSkinManager(void)
         { return m_pSkinMan; }
      ZFXVertexCacheManager* GetVertexManager(void)
         { return m_pVertexMan; }

      // initialize the engine stuff
      HRESULT Init(HWND, const HWND*, int, int, int, bool);
      HRESULT InitWindowed(HWND, const HWND*, int, bool);
      BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
      
      
      // INTERFACE FUNCTIONS:
      // ====================

      // general stuff
      void      Release(void);
      bool      IsRunning(void)  { return m_bRunning;  }
      bool      IsWindowed(void) { return m_bWindowed; }
      void      GetResolution(POINT *pPt)
                   { pPt->x=m_dwWidth;  pPt->y=m_dwHeight; }

      UINT      GetActiveSkinID(void)     { return m_nActiveSkin;    }
      void      SetActiveSkinID(UINT nID) { m_nActiveSkin = nID;     }
      UINT      GetActiveVShadID(void)    { return m_nActiveVShader; }
      UINT      GetActivePShadID(void)    { return m_nActivePShader; }

      // main/rendering stuff
      HRESULT   CreateFont(const char*,int,bool,bool,bool,DWORD,UINT*);
      HRESULT   DrawText(UINT,int,int,UCHAR,UCHAR,UCHAR,char*,...);
      HRESULT   BeginRendering(bool,bool,bool);
      HRESULT   Clear(bool, bool, bool);
      HRESULT   UseWindow(UINT nHwnd);
      void      EndRendering(void);
      void      FadeScreen(float, float, float, float);
      void      SetClearColor(float, float, float);
      void      SetAmbientLight(float, float, float);
      void      SetWorldTransform(const ZFXMatrix*);
      void      SetBackfaceCulling(ZFXRENDERSTATE);
      void      SetDepthBufferMode(ZFXRENDERSTATE);
      void      SetStencilBufferMode(ZFXRENDERSTATE,DWORD);
      void      UseStencilShadowSettings(bool);
      ZFXCOLOR  GetWireColor(void) { return m_clrWire; }
      void      SetShadeMode(ZFXRENDERSTATE,float,const ZFXCOLOR*);
      ZFXRENDERSTATE GetShadeMode(void) { return m_ShadeMode; }
      HRESULT   SetTextureStage(UCHAR, ZFXRENDERSTATE);
      HRESULT   SetLight(const ZFXLIGHT*, UCHAR);

      // shader stuff
      HRESULT   CreateVShader(const void*, UINT, bool, bool, UINT*);
      HRESULT   CreatePShader(const void*, UINT, bool, bool, UINT*);
      HRESULT   ActivateVShader(UINT, ZFXVERTEXID);
      HRESULT   ActivatePShader(UINT);

      // misc stuff
      void      UseAdditiveBlending(bool);
      bool      UsesAdditiveBlending(void)  { return m_bAdditive; }
      void      UseColorBuffer(bool);
      bool      UsesColorBuffer(void)  { return m_bColorBuffer; }
      void      UseTextures(bool);
      bool      UsesTextures(void) { return m_bTextures; }
      void      UseShaders(bool);
      bool      CanDoShaders(void) { return m_bCanDoShaders; }
      bool      UsesShaders(void)  { return m_bUseShaders;   }
      HRESULT   SetShaderConstant(ZFXSHADERTYPE,ZFXDATATYPE,UINT,UINT,const void*);
      HRESULT   SetView3D(const ZFXVector&, const ZFXVector&, 
                          const ZFXVector&, const ZFXVector&);
      HRESULT   SetViewLookAt(const ZFXVector&, const ZFXVector&, 
                              const ZFXVector&);
      void      SetClippingPlanes(float, float);
      void      SetOrthoScale(float, int);
      HRESULT   SetMode(ZFXENGINEMODE, int n);
      HRESULT   InitStage(float, ZFXVIEWPORT*, int n);
      HRESULT   GetFrustrum(ZFXPlane*);
      void      Transform2Dto3D(const POINT&, ZFXVector*, ZFXVector*);
      POINT     Transform3Dto2D(const ZFXVector&);
      ZFXVector Transform2Dto2D(UINT,float,const POINT*,ZFXAXIS);
      
      D3DTEXTUREOP GetTOP(UCHAR n) { return m_TOP[n]; }

   private:
      ZFXD3DEnum           *m_pEnum;               // enumerate d3d
      LPDIRECT3D9           m_pD3D;                // direct3d interface
      LPDIRECT3DDEVICE9     m_pDevice;             // direct3d device
      D3DPRESENT_PARAMETERS m_d3dpp;               // active pp's
      LPDIRECT3DSWAPCHAIN9  m_pChain[MAX_3DHWND];  // swap chains
      D3DCOLOR              m_ClearColor;          // background
      bool                  m_bIsSceneRunning;     // during scene
      bool                  m_bStencil;            // stencil bits?
      bool                  m_bTextures;           // use textures?
      D3DMATERIAL9          m_StdMtrl;             // standard material
      UINT                  m_nActiveSkin;         // currently active
      UINT                  m_nActiveVShader;      // currently active
      UINT                  m_nActivePShader;      // currently active
      LPD3DXFONT           *m_pFont;               // font objects
      UINT                  m_nNumFonts;           // number of fonts
      D3DTEXTUREOP          m_TOP[8];              // active tops

      D3DMATRIX m_mView2D,                         // view matrix orthogonal
                m_mView3D,                         // view matrix perspective
                m_mProj2D,                         // orthogonal projection (whole screen)
                m_mProjP[4],                       // perspective projection
                m_mProjO[4],                       // orthogonal projection
                m_mWorld,                          // active world matrix
                m_mViewProj,                       // combo matrix for 3D
                m_mWorldViewProj;                  // combo matrix for 3D
                

      LPDIRECT3DVDECL9      m_pDeclVertex;         // declarations
      LPDIRECT3DVDECL9      m_pDeclPVertex;        // declarations
      LPDIRECT3DVDECL9      m_pDeclLVertex;        // declarations
      LPDIRECT3DVDECL9      m_pDeclCVertex;        // declarations
      LPDIRECT3DVDECL9      m_pDecl3TVertex;       // declarations
      LPDIRECT3DVDECL9      m_pDeclTVertex;        // declarations

      LPDIRECT3DVERTEXSHADER9 m_pVShader[MAX_SHADER];
      LPDIRECT3DPIXELSHADER9  m_pPShader[MAX_SHADER];
      UINT                    m_nNumVShaders;
      UINT                    m_nNumPShaders;

      // start api with values from dialog box
      HRESULT Go(void);
      HRESULT OneTimeInit(void);

     
      // MISC STUFF
      void    Prepare2D(void);
      void    PrepareShaderStuff(void);
      void    CalcViewProjMatrix(void);
      void    CalcWorldViewProjMatrix(void);
      HRESULT CalcPerspProjMatrix(float, float, D3DMATRIX*);
      void    CalcOrthoProjMatrix(float,float,float,float,float,float,int);

      // write to log file
      void Log(char *, ...);
      void LogDeviceCaps(D3DCAPS9 *pCaps);
   }; // class
/*----------------------------------------------------------------*/


#endif