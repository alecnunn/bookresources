// File: ZFXRenderer.h
#ifndef ZFXRENDERDEVICE_H
#define ZFXRENDERDEVICE_H


// I N C L U D E S /////////////////////////////////////////////////

#include "ZFXRenderDevice.h"


// D E F I N E S ///////////////////////////////////////////////////


// S T R U C T S ///////////////////////////////////////////////////

/**
 * Just used to create, get a pointer to, and release a render device,
 * wether this is d3d or opengl or whatelse depends on dll which is 
 * selected in the CreateDevice() call.
 */
class ZFXRenderer {
   public:
      ZFXRenderer(HINSTANCE hInst);
      ~ZFXRenderer(void);
      
      HRESULT            CreateDevice(const char *chAPI);
      LPZFXRENDERDEVICE  GetDevice(void) { return m_pDevice; }
      HINSTANCE          GetModule(void) { return m_hDLL;    }
      void               Release(void);
      
   private:
      ZFXRenderDevice  *m_pDevice;
      HINSTANCE         m_hInst;
      HMODULE           m_hDLL;
   }; // class

typedef class ZFXRenderer *LPZFXRENDERER;
/*----------------------------------------------------------------*/

#endif
