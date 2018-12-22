// File: ZFXD3D.h
#ifndef ZFXD3D_H
#define ZFXD3D_H

#include <windows.h>

BOOL WINAPI DllEntryPoint( HINSTANCE hDll,
                           DWORD     fdwReason,
                           LPVOID    lpvRserved );

#include "..\ZFXRenderDevice.h"    // Interface Schablone
#include <d3d9.h>


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
      
      // initialize the engine stuff
      HRESULT Init(HWND, const HWND*, int, int, int, bool);
      HRESULT InitWindowed(HWND, const HWND*, int, bool);
      BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
      
      
      // Interface functions
      HRESULT UseWindow(UINT nHwnd);
      void    Release(void);
      bool    IsRunning(void) { return m_bRunning; }
      HRESULT BeginRendering(bool,bool,bool);
      HRESULT Clear(bool, bool, bool);
      void    EndRendering(void);
      void    SetClearColor(float, float, float);
      
      
   private:
      ZFXD3DEnum           *m_pEnum;
      LPDIRECT3D9           m_pD3D;
      LPDIRECT3DDEVICE9     m_pDevice;
      LPDIRECT3DSWAPCHAIN9  m_pChain[MAX_3DHWND];
      D3DPRESENT_PARAMETERS m_d3dpp;
      D3DCOLOR              m_ClearColor;
      bool                  m_bIsSceneRunning;
      bool                  m_bStencil;
      
      // start api with values from dialog box
      HRESULT Go(void);
      
      // write to log file
      void Log(char *, ...);
   }; // class
/*----------------------------------------------------------------*/


#endif