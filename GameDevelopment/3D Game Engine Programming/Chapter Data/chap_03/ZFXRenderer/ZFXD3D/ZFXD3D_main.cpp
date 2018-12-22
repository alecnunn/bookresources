// File: ZFXD3D_main.cpp

#include "ZFX.h"           // return values and stuff
#include "ZFXD3D.h"        // class definition



/*-----------------------------------------------------------*/
/* ZFXD3D class implementation                               *
/*-----------------------------------------------------------*/

/**
 * If in windowed mode this function lets you switch between
 * all available child windows given as parameter array to
 * the original Init() function call.
 */
HRESULT ZFXD3D::UseWindow(UINT nHwnd) {
   LPDIRECT3DSURFACE9 pBack=NULL;

   if (!m_d3dpp.Windowed)
      return ZFX_OK;
   else if (nHwnd >= m_nNumhWnd)
      return ZFX_FAIL;

   // try to get the right back buffer
   if (FAILED(m_pChain[nHwnd]->GetBackBuffer(0,
      D3DBACKBUFFER_TYPE_MONO,
      &pBack))) {
      Log("error: GetBackBuffer() failed in UseWindow()");
      return ZFX_FAIL;
      }
   
   // set him for the device 
   m_pDevice->SetRenderTarget(0, pBack);
   pBack->Release();
   m_nActivehWnd = nHwnd;
   return ZFX_OK;
   } // UseWindow
/*----------------------------------------------------------------*/


/**
 * Clear the scene and prepare device for retrieving geometry.
 * -> IN: bool - clear pixel buffer?
 *        bool - clear depth buffer?
 *        bool - clear stencil buffer?
 */
HRESULT ZFXD3D::BeginRendering(bool bClearPixel, 
                               bool bClearDepth, 
                               bool bClearStencil) {
   DWORD dw=0;

   // is there anything at all to clear?
   if (bClearPixel || bClearDepth || bClearStencil) {
      if (bClearPixel)   dw |= D3DCLEAR_TARGET;
      if (bClearDepth)   dw |= D3DCLEAR_ZBUFFER;
   
      if (bClearStencil && m_bStencil)
         dw |= D3DCLEAR_STENCIL;

      if (FAILED(m_pDevice->Clear(0, NULL, dw, m_ClearColor, 1.0f, 0))) {
            this->Log("ZFX_FAIL: Clear() \n");
            return ZFX_FAIL;
            }
      }
   
   if (FAILED(m_pDevice->BeginScene()))
      return ZFX_FAIL;

   m_bIsSceneRunning = true;
   return ZFX_OK;
   }
/*----------------------------------------------------------------*/

/**
 * Just clear the scene. Only call this when Scene has already begun.
 * -> IN: bool - clear pixel buffer?
 *        bool - clear depth buffer?
 *        bool - clear stencil buffer?
 */
HRESULT ZFXD3D::Clear(bool bClearPixel, bool bClearDepth, 
                      bool bClearStencil) {
   DWORD dw=0;

   if (bClearPixel)   dw |= D3DCLEAR_TARGET;
   if (bClearDepth)   dw |= D3DCLEAR_ZBUFFER;
   
   if (bClearStencil && m_bStencil)
      dw |= D3DCLEAR_STENCIL;

   if (m_bIsSceneRunning)
      m_pDevice->EndScene();

   if (FAILED(m_pDevice->Clear(0, NULL, dw, m_ClearColor, 1.0f, 0))) {
         this->Log("ZFX_FAIL: Clear() \n");
         return ZFX_FAIL;
         }

   if (m_bIsSceneRunning)
      m_pDevice->BeginScene();
   
   return ZFX_OK;
   }
/*----------------------------------------------------------------*/


/**
 * End of render operations and flip scene to front buffer.
 */
void ZFXD3D::EndRendering(void) {
   m_pDevice->EndScene();
   m_pDevice->Present(NULL, NULL, NULL, NULL);
   m_bIsSceneRunning = false;
   }
/*----------------------------------------------------------------*/


/**
 * Change the color of the screen clearing operation.
 * -> IN: float - red
 *        float - green
 *        float - blue
 */
void ZFXD3D::SetClearColor(float fRed, float fGreen, float fBlue) {
   m_ClearColor = D3DCOLOR_COLORVALUE(fRed, fGreen, fBlue, 1.0f);
   }
/*----------------------------------------------------------------*/
