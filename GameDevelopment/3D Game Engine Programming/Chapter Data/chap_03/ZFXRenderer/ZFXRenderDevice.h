// File: ZFXRenderDevice.h
#ifndef ZFXRENDERINTERFACE_H
#define ZFXRENDERINTERFACE_H


// I N C L U D E S /////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>


// D E F I N E S ///////////////////////////////////////////////////

#define MAX_3DHWND 8



// S T R U C T S ///////////////////////////////////////////////////

/**
 * ZFXRenderDevice is an abstract class which provides an interface
 * from which render dll's can inherit. This secures availability
 * of the necessary render functions.
 */
class ZFXRenderDevice {
   protected:
      HWND           m_hWndMain;          // application main window
      HWND           m_hWnd[MAX_3DHWND];  // 3D render window handle
      UINT           m_nNumhWnd;          // number of hwnds in array
      UINT           m_nActivehWnd;       // which one is active (0=main hWnd)
      HINSTANCE      m_hDLL;              // dll module handle
      DWORD          m_dwWidth;           // screen width
      DWORD          m_dwHeight;          // screen height
      bool           m_bWindowed;         // windowed mode?
      char           m_chAdapter[256];    // graphics adapter name
      bool           m_bRunning;          // after succesful initialization
      FILE          *m_pLog;              // log file
      
      
   public:
      ZFXRenderDevice(void) {};
      virtual ~ZFXRenderDevice(void) {};
      
      
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