// File: ZFXJoystick.cpp

#include <windows.h>       // type definitions
#include "ZFXDI.h"         // class definition


extern bool g_bLF;


// Yeah, evil callback stuff. Let's rock C++, let's rock!
ZFXJoystick *g_pThis=NULL;
CALLBACK gEnumJoyCallback(const DIDEVICEINSTANCE* pInst, void* pUserData);



/*-----------------------------------------------------------*/
/* ZFXJoystick class implementation                          *
/*-----------------------------------------------------------*/

/**
 * Constructor
 */
ZFXJoystick::ZFXJoystick(LPDIRECTINPUT8 pDI, HWND hWnd, FILE* pLog) 
   { Create(pDI, hWnd, pLog); }
/*----------------------------------------------------------------*/


/**
 * Destructor
 */ 
ZFXJoystick::~ZFXJoystick() 
   { Log("releasing joystick"); Release(); }
/*----------------------------------------------------------------*/


/**
 * Initializes the joystick device
 */
HRESULT ZFXJoystick::Init(void) {
   DIPROPRANGE diprg;
   DIDEVCAPS   diCaps;

   Log("crancking up joystick");

   // some inits
   memset(m_bPressed, 0, sizeof(m_bPressed));
   memset(m_bReleased, 0, sizeof(m_bReleased));
   m_bJoyFound = false;
   m_lX = m_lY = 0;
   g_pThis     = this;

   // first enumerate available joysticks
   m_pDI->EnumDevices(DI8DEVCLASS_GAMECTRL, 
                      (LPDIENUMDEVICESCALLBACK)
                      gEnumJoyCallback,
                      &m_guid, DIEDFL_ATTACHEDONLY);

   // nothing found? so there is no joy at all!
   if (!m_bJoyFound) {
      Log("no joystick found");
      return ZFX_FAIL;
      }

   // final settings
   diprg.diph.dwSize = sizeof(DIPROPRANGE);
   diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
   diprg.diph.dwHow = DIPH_BYOFFSET;
   diprg.lMin = -1000;
   diprg.lMax = +1000;

   diprg.diph.dwObj = DIJOFS_X;
   m_pDevice->SetProperty(DIPROP_RANGE, &diprg.diph);

   diprg.diph.dwObj = DIJOFS_Y;                    // y-Achse
   m_pDevice->SetProperty(DIPROP_RANGE, &diprg.diph);

   // check number of buttons on this thing
   if (SUCCEEDED(m_pDevice->GetCapabilities(&diCaps)))
      m_dwNumBtns = diCaps.dwButtons;
   else
      m_dwNumBtns = 4;

   Log("joystick online (%d buttons, \"%s\")", m_dwNumBtns, m_Name);
   return ZFX_OK;
   } // Init
/*----------------------------------------------------------------*/


/**
 *
 */
BOOL CALLBACK gEnumJoyCallback(const DIDEVICEINSTANCE* pInst,
                              void* pUserData) {
   return g_pThis->EnumJoyCallback(pInst);
   }
/*----------------------------------------------------------------*/


/**
 * Called for every enumerated Joystick. If we manage to create a
 * device interface for one of them, then stop the enumeration.
 */
BOOL ZFXJoystick::EnumJoyCallback(const DIDEVICEINSTANCE *pInst) {
   // if we found any then 'go create' it *g*
   if (SUCCEEDED(CrankUp(pInst->guidInstance, &c_dfDIJoystick))) {
      m_bJoyFound = true;
      strcpy(m_Name, (char*)pInst->tszProductName);
      return DIENUM_STOP;
      }
   return DIENUM_CONTINUE;
   } // EnumJoyCallback
/*----------------------------------------------------------------*/


/**
 * Update all input devices
 */
HRESULT ZFXJoystick::Update(void) {
   DIJOYSTATE js;
   
   // Poll the device, this might not be needed by
   // some joysticks but it won't hurt them, though
   m_pDevice->Poll();

   // try to get the data from the Joystick
   if (FAILED(GetData(IDV_JOYSTICK, &js, NULL))) {
      Log("error: GetData(Joystick) failed");
      return ZFX_FAIL;
      }

   // get the button states
   for (DWORD i=0; i<m_dwNumBtns; i++) {
      m_bReleased[i] = false;
      if (js.rgbButtons[i] & 0x80)
         m_bPressed[i] = true;
      else {
         if (m_bPressed[i]) m_bReleased[i] = true;
         m_bPressed[i] = false;
         }
      }

   // save the sticks position
   m_lX = js.lX;
   m_lY = js.lY;

   return ZFX_OK;
   } // Update
/*----------------------------------------------------------------*/


/**
 * Return the intensity of joystick deflection on both axes in the
 * range from [-1.0f, +1.0f].
 */
HRESULT ZFXJoystick::GetJoyDeflection(float *pfX, float *pfY) {
   (*pfX) = m_lX / 1000.0f;
   (*pfY) = m_lY / 1000.0f;
   return ZFX_OK;
   } // 
/*----------------------------------------------------------------*/
