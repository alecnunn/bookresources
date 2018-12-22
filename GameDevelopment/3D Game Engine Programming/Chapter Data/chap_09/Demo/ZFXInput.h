// File: ZFXRenderer.h
#ifndef ZFXINPUT_H
#define ZFXINPUT_H


// I N C L U D E S /////////////////////////////////////////////////

#include "ZFXInputDevice.h"


// D E F I N E S ///////////////////////////////////////////////////


// S T R U C T S ///////////////////////////////////////////////////

/**
 * Just used to create, get a pointer to, and release an input device.
 */
class ZFXInput {
   public:
      ZFXInput(HINSTANCE hInst);
      ~ZFXInput(void);
      
      HRESULT          CreateDevice(void);
      LPZFXINPUTDEVICE GetDevice(void) { return m_pDevice; }
      HINSTANCE        GetModule(void) { return m_hDLL;    }
      void             Release(void);
      
   private:
      ZFXInputDevice  *m_pDevice;
      HINSTANCE        m_hInst;
      HMODULE          m_hDLL;
   }; // class

typedef class ZFXInput *LPZFXINPUT;
/*----------------------------------------------------------------*/

#endif