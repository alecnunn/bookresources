// File: ZFXAudio.h
#ifndef ZFXAUDIO_H
#define ZFXAUDIO_H


// I N C L U D E S /////////////////////////////////////////////////

#include "ZFXAudioDevice.h"


// D E F I N E S ///////////////////////////////////////////////////


// S T R U C T S ///////////////////////////////////////////////////

/**
 * Just used to create, get a pointer to, and release an input device.
 */
class ZFXAudio {
   public:
      ZFXAudio(HINSTANCE hInst);
      ~ZFXAudio(void);
      
      HRESULT          CreateDevice(void);
      LPZFXAUDIODEVICE GetDevice(void) { return m_pDevice; }
      HINSTANCE        GetModule(void) { return m_hDLL;    }
      void             Release(void);
      
   private:
      ZFXAudioDevice  *m_pDevice;
      HINSTANCE        m_hInst;
      HMODULE          m_hDLL;
   }; // class

typedef class ZFXAudio *LPZFXAUDIO;
/*----------------------------------------------------------------*/

#endif