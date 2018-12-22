// File: ZFXAudio.h
#ifndef ZFXNETWORK_H
#define ZFXNETWORK_H


// I N C L U D E S /////////////////////////////////////////////////

#include "ZFXNetworkDevice.h"


// D E F I N E S ///////////////////////////////////////////////////


// S T R U C T S ///////////////////////////////////////////////////

/**
 * Just used to create, get a pointer to, and release an input device.
 */
class ZFXNetwork {
   public:
      ZFXNetwork(HINSTANCE hInst);
      ~ZFXNetwork(void);
      
      HRESULT            CreateDevice(void);
      LPZFXNETWORKDEVICE GetDevice(void) { return m_pDevice; }
      HINSTANCE          GetModule(void) { return m_hDLL;    }
      void               Release(void);
      
   private:
      ZFXNetworkDevice  *m_pDevice;
      HINSTANCE          m_hInst;
      HMODULE            m_hDLL;
   }; // class

typedef class ZFXNetwork *LPZFXNETWORK;
/*----------------------------------------------------------------*/

#endif