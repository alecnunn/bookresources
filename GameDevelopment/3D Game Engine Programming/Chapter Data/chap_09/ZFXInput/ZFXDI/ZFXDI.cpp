// File: ZFXDI.cpp

#include <windows.h>       // type definitions
#include "ZFXDI.h"         // class definition

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")


bool g_bLF=false;


/*-----------------------------------------------------------*/
/* DLL stuff implementation                                  *
/*-----------------------------------------------------------*/

/**
 * DLL Entry Point similar to WinMain()/main()
 */
BOOL WINAPI DllEntryPoint(HINSTANCE hDll,
                          DWORD     fdwReason,
                          LPVOID    lpvReserved) {
   switch(fdwReason) {
      // called when we attach to the DLL
      case DLL_PROCESS_ATTACH:
         /* dll init/setup stuff */
         break;
      case DLL_PROCESS_DETACH:
         /* dll shutdown/release stuff */
         break;
      default:
         break;
      };
   
   return TRUE;
   } // DllEntryPoint
/*----------------------------------------------------------------*/

/**
 * Exported create function: Creates a new ZFXInputDevice object.
 */
HRESULT CreateInputDevice(HINSTANCE hDLL, ZFXInputDevice **pDevice) {
   if(!*pDevice) {
      *pDevice = new ZFXDI(hDLL);
      return ZFX_OK;
      }
   return ZFX_FAIL;
   }
/*----------------------------------------------------------------*/

/**
 * Exported release function: Realeses the given ZFXInputDevice object.
 */
HRESULT ReleaseInputDevice(ZFXInputDevice **pDevice) {
   if(!*pDevice) {
      return ZFX_FAIL;
      }
   delete *pDevice;
   *pDevice = NULL;
   return ZFX_OK;
   }
/*----------------------------------------------------------------*/





/*-----------------------------------------------------------*/
/* ZFXDI class implementation                                *
/*-----------------------------------------------------------*/

/**
 * Constructor
 */
ZFXDI::ZFXDI(HINSTANCE hDLL) {
   m_hDLL        = hDLL;
   m_pDI         = NULL;
   m_pLog        = NULL;
   m_bRunning    = false;
   m_pKB         = NULL;
   m_pMouse      = NULL;
   m_pJoy        = NULL;

   // open a new logfile
   m_pLog = fopen("Log_ZFXInputDevice.txt", "w");
   Log("online (waiting for initialization call)");
   }
/*----------------------------------------------------------------*/


/**
 * Destructor
 */ 
ZFXDI::~ZFXDI() {
   Release();
   }
/*----------------------------------------------------------------*/


/**
 * Release all stuff.
 */
void ZFXDI::Release() {
   Log("shutting down DirectInput");

   if (m_pKB) {
      delete m_pKB;
      m_pKB = NULL;
      }
   
   if (m_pMouse) {
      delete m_pMouse;
      m_pMouse = NULL;
      }

   if (m_pJoy) {
      delete m_pJoy;
      m_pJoy = NULL;
      }

   if (m_pDI) {
      m_pDI->Release();
      m_pDI = NULL;
      }

   Log("offline (ok)");
   fclose(m_pLog);
   }
/*----------------------------------------------------------------*/


/**
 * Initializes at least keyboard and mouse, otherwise returns error.
 * If present joystick will also be initialized, but is not mandatory.
 * -> IN: HWND      - handle to main application window
 *        bool      - should log be made safe? (flush at once)
 */
HRESULT ZFXDI::Init(HWND hWnd, const RECT *prcCage, bool bSaveLog) {
   HRESULT hr;

   Log("calling initialization");
   
   m_hWndMain = hWnd;
   g_bLF      = bSaveLog;

   // create main DirectInput object
   if (FAILED (hr = DirectInput8Create(m_hDLL, DIRECTINPUT_VERSION, 
                           IID_IDirectInput8, (void**)&m_pDI,
                           NULL)) ) { 
      Log("error: DirectInput8Create() failed"); 
      return ZFX_FAIL;
      } 

   // create all input device objects
   m_pKB    = new ZFXKeyboard(m_pDI, hWnd, m_pLog);
   m_pMouse = new ZFXMouse(m_pDI, hWnd, m_pLog);
   m_pJoy   = new ZFXJoystick(m_pDI, hWnd, m_pLog);


   // initialize all input device objects
   if (FAILED( m_pKB->Init() )) {
      if (m_pKB) delete m_pKB;
      m_pKB = NULL;
      Log("error: Init(Keyboard) failed");
      return ZFX_FAIL;
      }
   
   if (FAILED( m_pMouse->Init() )) {
      if (m_pMouse) delete m_pMouse;
      m_pMouse = NULL;
      Log("error: Init(Mouse) failed");
      return ZFX_FAIL;
      }

   if (prcCage) m_pMouse->SetCage(*prcCage);
   
   if (FAILED( m_pJoy->Init() )) {
      if (m_pJoy) delete m_pJoy;
      m_pJoy = NULL;
      Log("warning: no joystick active");
      /* no need to fail */
      }

   Log("initialized (online and ready)");
   m_bRunning = true;
   return ZFX_OK;
   } // Init
/*----------------------------------------------------------------*/


/**
 * Query if a Joystick is active and ready or not. Also returns
 * Joytsicks name if any was found.
 */
bool ZFXDI::HasJoystick(char *pJoyName) {
   if (m_pJoy) {
      if (pJoyName) m_pJoy->GetName(pJoyName);
      return true;
      }
   return false;
   } // HasJoystick
/*----------------------------------------------------------------*/


/**
 * Update all input devices
 */
HRESULT ZFXDI::Update(void) {
   HRESULT hr;

   if (!IsRunning()) return ZFX_FAIL;

   if (m_pKB) {
      if ( FAILED( hr=m_pKB->Update() ) )
         return hr;
      }

   if (m_pMouse) {
      if ( FAILED( hr=m_pMouse->Update() ) )
         return hr;
      }

   if (m_pJoy) {
      if ( FAILED( hr=m_pJoy->Update() ) )
         return hr;
      }
   return ZFX_OK;
   } // Update
/*----------------------------------------------------------------*/


/**
 * If mouse or joystick return current position.
 */
HRESULT ZFXDI::GetPosition(ZFXINPUTDEV idType, POINT *pPt) {
   if (idType == IDV_MOUSE) {
      m_pMouse->GetPosition(pPt);
      return ZFX_OK;
      }
   else if (idType==IDV_JOYSTICK) {
      if (m_pJoy)
         m_pJoy->GetPosition(pPt);
      else {
         (*pPt).x = 0;
         (*pPt).y = 0;
         }
      return ZFX_OK;
      }
   else return ZFX_INVALIDPARAM;
   } // GetPosition
/*----------------------------------------------------------------*/


/**
 * If joystick return current intensity on axes.
 */
HRESULT ZFXDI::GetJoyDeflection(float *pfX, float *pfY) {
   if (m_pJoy)
      return m_pJoy->GetJoyDeflection(pfX, pfY);
   else {
      (*pfX) = 0.0f;
      (*pfY) = 0.0f;
      return ZFX_OK;
      }
   } // GetPosition
/*----------------------------------------------------------------*/


/**
 * Return the change of mouse cursor since last call to Update().
 */
POINT ZFXDI::GetMouseDelta(void) 
   { return m_pMouse->GetMouseDelta(); }
/*----------------------------------------------------------------*/


/**
 * Ask about button state.
 */
bool ZFXDI::IsPressed(ZFXINPUTDEV idType, UINT nBtn) {
   if (idType == IDV_MOUSE)
      return m_pMouse->IsPressed(nBtn);
   else if (idType==IDV_KEYBOARD)
      return m_pKB->IsPressed(nBtn);
   else if ( (idType==IDV_JOYSTICK) && (m_pJoy) )
         return m_pJoy->IsPressed(nBtn);
   else 
      return false;
   } // Pressed
/*----------------------------------------------------------------*/


/**
 * Ask about button/key state for mouse, joytsick or keyboard.
 */
bool ZFXDI::IsReleased(ZFXINPUTDEV idType, UINT nBtn) {
   if (idType == IDV_MOUSE)
      return m_pMouse->IsReleased(nBtn);
   else if (idType==IDV_KEYBOARD)
      return m_pKB->IsReleased(nBtn);
   else if ( (idType==IDV_JOYSTICK) && (m_pJoy) )
      return m_pJoy->IsReleased(nBtn);
   else 
      return false;
   } // Released
/*----------------------------------------------------------------*/


/**
 * write outputstring to attribut outputstream if exists
 * -> IN: bool - flush immediately
 *        char - format string to output
 *        ...  - output values
 */
void ZFXDI::Log(char *chString, ...) {

   char ch[256];
   char *pArgs;
   
   pArgs = (char*) &chString + sizeof(chString);
   vsprintf(ch, chString, pArgs);
   fprintf(m_pLog, "[ZFXDIDevice]: ");
   fprintf(m_pLog, ch);
   fprintf(m_pLog, "\n");
   
   if (g_bLF)
      fflush(m_pLog);
   } // Log
/*----------------------------------------------------------------*/



/*-----------------------------------------------------------*/
/* ZFXDIDevice class implementation                          *
/*-----------------------------------------------------------*/

/**
 * Set basic attributes to pointers.
 */
void ZFXDIDevice::Create(LPDIRECTINPUT8 pDI, HWND hWnd, FILE* pLog) {
   m_pLog    = pLog;
   m_hWnd    = hWnd;
   m_pDI     = pDI;
   m_pDevice = NULL;
   }
/*----------------------------------------------------------------*/


/**
 * Release the object.
 */ 
void ZFXDIDevice::Release(void) {
   if (m_pDevice) {
      m_pDevice->Unacquire();
      m_pDevice->Release();
      m_pDevice = NULL;
      }
   Log("input device offline (ok)");
   } // Release
/*----------------------------------------------------------------*/


/**
 * Call all stuff DirectInput needs to initialize an input device.
 */ 
HRESULT ZFXDIDevice::CrankUp(REFGUID rguid, LPCDIDATAFORMAT pdf) {
   DWORD dwFlags = DISCL_FOREGROUND | DISCL_NONEXCLUSIVE;

   // if device is already build destroy it
   if (m_pDevice) {
      m_pDevice->Unacquire();
      m_pDevice->Release();
      m_pDevice = NULL;
      }

   // 1. Step: create device
   if ( FAILED(m_pDI->CreateDevice(rguid, &m_pDevice, NULL))) {
      Log("error: CreateDevice failed");
      return ZFX_FAIL; 
      } 

   // set the correct device data format
   if ( FAILED(m_pDevice->SetDataFormat(pdf))) {
      Log("error: SetDataFormat failed");
      return ZFX_FAIL; 
      }

   // set the cooperation level with windows
   if ( FAILED(m_pDevice->SetCooperativeLevel(m_hWnd, dwFlags))) {
      Log("error: SetCoopLevel failed");
      return ZFX_FAIL; 
      }

   return ZFX_OK;
   } // CrankUp
/*----------------------------------------------------------------*/


/**
 * Get the state or data from the device object.
 * -> IN: ZFXINPUTDEV - keyboard, mouse or joystick
 *        void*       - stores the data
 *        DWORD*      - stores a counter (mouse only)
 */
HRESULT ZFXDIDevice::GetData(ZFXINPUTDEV Type, void *pData, 
                             DWORD *pdwNum) {
   HRESULT hr=ZFX_FAIL;
   size_t size=0;
   
   // is this a mouse?
   if (Type == IDV_MOUSE) {
      size = sizeof(DIDEVICEOBJECTDATA);

      hr=m_pDevice->GetDeviceData(size, (DIDEVICEOBJECTDATA*)
                                  pData, pdwNum, 0);
      }
   else {
      if (Type==IDV_KEYBOARD) size = sizeof(char)*256;
      else size = sizeof(DIJOYSTATE);

      hr = m_pDevice->GetDeviceState(size, pData);
      }

 
   if (FAILED(hr)) {
      // if lost or not yet acquired then acquire it at all costs
      if ( (hr==DIERR_NOTACQUIRED) || (hr==DIERR_INPUTLOST) ) {
         hr = m_pDevice->Acquire();
         while (hr==DIERR_INPUTLOST)
            hr = m_pDevice->Acquire();
         
         // if another application is using this input device
         // we have to give up and try next frame
         if (hr==DIERR_OTHERAPPHASPRIO) return ZFX_OK;
         
         // if we got back device then try again to read data
         if (SUCCEEDED(hr)) {
            if (Type == IDV_MOUSE)
               hr = m_pDevice->GetDeviceData(size, (DIDEVICEOBJECTDATA*)
                                             pData, pdwNum, 0);
            else 
               hr = m_pDevice->GetDeviceState(size, pData);
            }
         if (FAILED(hr)) return ZFX_FAIL;
         }
      else return ZFX_FAIL;
      }
   return ZFX_OK;
   } // GetData
/*----------------------------------------------------------------*/


/**
 * write outputstring to attribut outputstream if exists
 */
void ZFXDIDevice::Log(char *chString, ...) {

   char ch[256];
   char *pArgs;
   
   pArgs = (char*) &chString + sizeof(chString);
   vsprintf(ch, chString, pArgs);
   fprintf(m_pLog, "[ZFXDIDevice]: ");
   fprintf(m_pLog, ch);
   fprintf(m_pLog, "\n");
   
   if (g_bLF)
      fflush(m_pLog);
   } // Log
/*----------------------------------------------------------------*/