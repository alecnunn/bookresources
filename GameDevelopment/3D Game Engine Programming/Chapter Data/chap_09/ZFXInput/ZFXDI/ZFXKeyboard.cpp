// File: ZFXKeyboard.cpp

#include <windows.h>       // type definitions
#include "ZFXDI.h"         // class definition


extern bool g_bLF;



/*-----------------------------------------------------------*/
/* ZFXKeyboard class implementation                          *
/*-----------------------------------------------------------*/

/**
 * Constructor
 */
ZFXKeyboard::ZFXKeyboard(LPDIRECTINPUT8 pDI, HWND hWnd, FILE* pLog) 
   { Create(pDI, hWnd, pLog); }
/*----------------------------------------------------------------*/


/**
 * Destructor
 */ 
ZFXKeyboard::~ZFXKeyboard(void)
   { Log("releasing keyboard"); Release(); }
/*----------------------------------------------------------------*/


/**
 * Initializes the keyboard device.
 */
HRESULT ZFXKeyboard::Init(void) {
   
   Log("crancking up keyboard");
   if (FAILED(CrankUp(GUID_SysKeyboard, &c_dfDIKeyboard)))
      return ZFX_FAIL;

   // clear out the structs
   memset(m_Keys, 0, sizeof(m_Keys));
   memset(m_KeysOld, 0, sizeof(m_KeysOld));

   // acquire the device to make it work
   m_pDevice->Acquire();
   Log("keyboard online");
   return ZFX_OK;
   } // Init
/*----------------------------------------------------------------*/


/**
 * Update the keyboard device.
 */
HRESULT ZFXKeyboard::Update(void) {
   // copy keyboard state from last frame
   memcpy(m_KeysOld, m_Keys, sizeof(m_Keys));

   // try to get the data from the keyboard
   if (FAILED(GetData(IDV_KEYBOARD, &m_Keys[0], NULL))) {
      Log("error: GetData(Keyboard) failed");
      return ZFX_FAIL;
      }
   return ZFX_OK;
   } // Update
/*----------------------------------------------------------------*/


/**
 * Check if the given key is pressed. Use WinAPI VK_ defines here.
 */
bool ZFXKeyboard::IsPressed(UINT nID) {
   // translate virtual code to scan code
   if (m_Keys[nID] & 0x80)
      return true;
   return false;
   } // IsPressed
/*----------------------------------------------------------------*/


/**
 * Check if the given key is released. Use WinAPI VK_ defines here.
 */
bool ZFXKeyboard::IsReleased(UINT nID) {
   // translate virtual code to scan code
   if ( (m_KeysOld[nID]&0x80) && !(m_Keys[nID]&0x80) )
      return true;
   return false;
   } // IsPressed
/*----------------------------------------------------------------*/