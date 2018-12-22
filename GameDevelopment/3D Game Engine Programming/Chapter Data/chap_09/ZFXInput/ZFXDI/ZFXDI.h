// File: ZFXDI.h
#ifndef ZFXDI_H
#define ZFXDI_H

#include <windows.h>

#define DIRECTINPUT_VERSION 0x0800


BOOL WINAPI DllEntryPoint( HINSTANCE hDll,
                           DWORD     fdwReason,
                           LPVOID    lpvRserved );

#include "..\ZFXInputDevice.h"
#include "..\ZFX.h"
#include <dinput.h>


class ZFXKeyboard;
class ZFXMouse;
class ZFXJoystick;


/**
 * Class definition for a Direct3D render device, implementing the
 * interface ZFXRenderDevice
 */
class ZFXDI : public ZFXInputDevice {
   public:
      ZFXDI(HINSTANCE hDLL);
      ~ZFXDI(void);
      
      // initialize the engine stuff
      HRESULT Init(HWND, const RECT*, bool);
     
      // Interface functions
      void    Release(void);
      bool    IsRunning(void) { return m_bRunning; }
      bool    HasJoystick(char *pJoyName);
      HRESULT Update(void);
      
      // query input data and states
      HRESULT GetPosition(ZFXINPUTDEV idType, POINT *pPt);
      HRESULT GetJoyDeflection(float*, float*);
      POINT   GetMouseDelta(void);
      bool    IsPressed(ZFXINPUTDEV idType, UINT nBtn);
      bool    IsReleased(ZFXINPUTDEV idType, UINT nBtn);
      

   private:
      LPDIRECTINPUT8  m_pDI; 
      ZFXKeyboard    *m_pKB;
      ZFXMouse       *m_pMouse;
      ZFXJoystick    *m_pJoy;

      // write to log file
      void Log(char *, ...);
   }; // class
/*----------------------------------------------------------------*/


/**
 * Joystick / Gamepad Device
 */
class ZFXDIDevice {
   public:
      ZFXDIDevice(void) { /*nothing*/; }
      virtual ~ZFXDIDevice(void) { /*nothing*/; }

      // base class methods
      virtual void    Create(LPDIRECTINPUT8, HWND, FILE*);
      virtual void    Release(void);
      virtual HRESULT CrankUp(REFGUID rguid, LPCDIDATAFORMAT lpdf);
      
      // Accessor-Methods
      virtual void    GetPosition(POINT *pPoint)
         { (*pPoint).x = m_lX; (*pPoint).y = m_lY; }

      // virtual methods for heirs
      virtual HRESULT Init(void)=0;
      virtual HRESULT Update(void)=0;

   protected:
      virtual HRESULT GetData(ZFXINPUTDEV Type, void *pData, DWORD *dwNum);

      LPDIRECTINPUTDEVICE8  m_pDevice;
      LPDIRECTINPUT8        m_pDI;
      HWND                  m_hWnd;
      long                  m_lX;
      long                  m_lY;
      FILE                 *m_pLog;
      void Log(char *, ...);
   }; // class
/*----------------------------------------------------------------*/


/**
 * Keyboard Device
 */
class ZFXKeyboard : public ZFXDIDevice {
   public:
      ZFXKeyboard(LPDIRECTINPUT8, HWND, FILE*);
      virtual ~ZFXKeyboard(void);

      HRESULT Init(void);
      HRESULT Update(void);

      bool    IsPressed(UINT nID);
      bool    IsReleased(UINT nID);

   private:
      char  m_Keys[256];
      char  m_KeysOld[256];
   }; // class
/*----------------------------------------------------------------*/


/**
 * Mouse Device
 */
class ZFXMouse : public ZFXDIDevice {
   public:
      ZFXMouse(LPDIRECTINPUT8, HWND, FILE*);
      virtual ~ZFXMouse(void);

      HRESULT Init(void);
      HRESULT Update(void);

      void  SetCage(RECT rcCage) { m_rcCage = rcCage; }
      POINT GetMouseDelta(void) { return m_Delta; }

      bool IsPressed(UINT nBtn) 
         { if (nBtn<3) return m_bPressed[nBtn]; return false; }
      bool IsReleased(UINT nBtn)
         { if (nBtn<3) return m_bReleased[nBtn]; return false; }

   private:
      HANDLE     m_hEvent;
      RECT       m_rcCage;
      bool       m_bPressed[3];
      bool       m_bReleased[3];
      POINT      m_Delta;
   }; // class
/*----------------------------------------------------------------*/


/**
 * Joystick / Gamepad Device
 */
class ZFXJoystick : public ZFXDIDevice {
   public:
      ZFXJoystick(LPDIRECTINPUT8, HWND, FILE*);
      virtual ~ZFXJoystick(void);

      HRESULT Init(void);
      HRESULT Update(void);

      bool JoystickFound(void) { return m_bJoyFound; }
      BOOL EnumJoyCallback(const DIDEVICEINSTANCE *pInst);
      
      void GetName(char *pJoyName) 
         { memcpy(pJoyName, m_Name, sizeof(char)*256); }

      bool IsPressed(UINT nBtn) 
         { if (nBtn<m_dwNumBtns) return m_bPressed[nBtn]; return false; }

      bool IsReleased(UINT nBtn) 
         { if (nBtn<m_dwNumBtns) return m_bReleased[nBtn]; return false; }

      HRESULT GetJoyDeflection(float *pfX, float *pfY);

   private:
      GUID       m_guid;
      char       m_Name[256];
      bool       m_bJoyFound;
      bool       m_bPressed[12];
      bool       m_bReleased[12];
      DWORD      m_dwNumBtns;
   }; // class
/*----------------------------------------------------------------*/

#endif
