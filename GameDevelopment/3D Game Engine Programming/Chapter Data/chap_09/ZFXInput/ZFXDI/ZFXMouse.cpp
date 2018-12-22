// File: ZFXMouse.cpp

#include <windows.h>       // type definitions
#include "ZFXDI.h"         // class definition


extern bool g_bLF;


#define BUFFER_SIZE 16


/*-----------------------------------------------------------*/
/* ZFXMouse class implementation                             *
/*-----------------------------------------------------------*/

/**
 * Constructor
 */
ZFXMouse::ZFXMouse(LPDIRECTINPUT8 pDI, HWND hWnd, FILE* pLog) 
   { Create(pDI, hWnd, pLog); }
/*----------------------------------------------------------------*/


/**
 * Destructor
 */ 
ZFXMouse::~ZFXMouse(void) 
   { Log("releasing mouse"); Release(); }
/*----------------------------------------------------------------*/


/**
 * Initializes the mouse device
 */
HRESULT ZFXMouse::Init(void) {
   // clear out structs for mouse buttons
   memset(m_bPressed,  0, sizeof(bool)*3);
   memset(m_bReleased, 0, sizeof(bool)*3);
   m_lX = m_lY = 0;

   m_rcCage.left   = -1;
   m_rcCage.right  = -1;
   m_rcCage.top    = -1;
   m_rcCage.bottom = -1;

   Log("crancking up mouse");
   if (FAILED(CrankUp(GUID_SysMouse, &c_dfDIMouse)))
      return ZFX_FAIL;

   // activate event notification for the mouse
   if (!(m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL))) {
      Log("error: CreateEvent(Mouse) failed");
      return ZFX_FAIL;
      }
  if (FAILED( m_pDevice->SetEventNotification(m_hEvent))) {
      Log("error: SetEventNotification(Mouse) failed");
      return ZFX_FAIL;
      }

   // create buffered input for the mouse
   DIPROPDWORD dipdw;
   dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
   dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
   dipdw.diph.dwObj        = 0;
   dipdw.diph.dwHow        = DIPH_DEVICE;
   dipdw.dwData            = BUFFER_SIZE;

   if (FAILED( m_pDevice->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph))) {
      Log("error: SetProperty(Mouse) failed");
      return ZFX_FAIL;
      }

   // acquire the device to make it work
   m_pDevice->Acquire();
   Log("mouse online");
   return ZFX_OK;
   } // Init
/*----------------------------------------------------------------*/


/**
 * Update all input devices
 */
HRESULT ZFXMouse::Update(void) {
   DIDEVICEOBJECTDATA od[BUFFER_SIZE];
   DWORD        dwNumElem = BUFFER_SIZE;

   bool bCage = (m_rcCage.right != -1);

   m_Delta.x = m_Delta.y = 0;
      
   // try to get the data from the mouse
   if (FAILED(GetData(IDV_MOUSE, &od[0], &dwNumElem))) {
      Log("error: GetData(Mouse) failed");
      return ZFX_FAIL;
      }

   // clear those
   m_bReleased[0] = m_bReleased[1] = m_bReleased[2] = false;

   // now we have 'dwNumElem' of mouse events
   for (DWORD i=0; i<dwNumElem; i++) {
      switch (od[i].dwOfs) {
         // MOVEMENT
         case DIMOFS_X: {
            m_lX += od[i].dwData;
            m_Delta.x = od[i].dwData;

            if (bCage & (m_lX < m_rcCage.left))
               m_lX = m_rcCage.left;
            else if (bCage & (m_lX > m_rcCage.right))
               m_lX = m_rcCage.right;
            } break;

         case DIMOFS_Y: {
            m_lY += od[i].dwData;
            m_Delta.y = od[i].dwData;
            
            if (bCage & (m_lY < m_rcCage.top))
               m_lY = m_rcCage.top;
            else if (bCage & (m_lY > m_rcCage.bottom))
               m_lY = m_rcCage.bottom;
            } break;

         // BUTTON STATES
         case DIMOFS_BUTTON0: {
            if (od[i].dwData & 0x80) {
               m_bPressed[0] = true;
               }
            else {
               if (m_bPressed[0])
                  m_bReleased[0] = true;

               m_bPressed[0] = false;
               }
            } break;

         case DIMOFS_BUTTON1: {
            if (od[i].dwData & 0x80)
               m_bPressed[1] = true;
            else {
               if (m_bPressed[1])
                  m_bReleased[1] = true;

               m_bPressed[1] = false;
               }
            } break;

         case DIMOFS_BUTTON2: {
            if (od[i].dwData & 0x80)
               m_bPressed[2] = true;
            else {
               if (m_bPressed[2])
                  m_bReleased[2] = true;

               m_bPressed[2] = false;
               }
            } break;
         }; // switch
      } // for

   return ZFX_OK;
   } // Update
/*----------------------------------------------------------------*/


