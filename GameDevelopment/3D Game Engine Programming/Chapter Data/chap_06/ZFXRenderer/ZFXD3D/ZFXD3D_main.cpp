// File: ZFXD3D_main.cpp

#include "ZFX.h"           // return values and stuff
#include "ZFXD3D.h"        // class definition

#include <wingdi.h>

extern bool g_bLF;


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
      return ZFX_INVALIDID;

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
            Log("error: Clear()");
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
         Log("error: Clear()");
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
   // flush all vertex buffers
   if (FAILED(m_pVertexMan->ForcedFlushAll()))
      Log("error: ForceFlushAll() from EndRendering() failed");

   m_pDevice->EndScene();

   if (m_d3dpp.Windowed && (m_nNumhWnd > 0) ) {
      if (FAILED(m_pChain[m_nActivehWnd]->Present(NULL, NULL, NULL, NULL, 0)))
         Log("error: Chain->Present() from EndRendering() failed");
      }
   else {
      if (FAILED(m_pDevice->Present(NULL, NULL, NULL, NULL)))
         Log("error: Deviec->Present() from EndRendering() failed");
      }

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


/**
 * Set the world transformation matrix transforming any objects
 * rendered till setting changes again. This will force a flush
 * of all vertex buffers.
 * -> IN: ZFXMatrix* - new matrix or NULL for no transform
 */
void ZFXD3D::SetWorldTransform(const ZFXMatrix *mWorld) {
   // last chance check
   m_pVertexMan->ForcedFlushAll();

   // set class attribute 'world matrix'
   if (!mWorld) {
      ZFXMatrix m; m.Identity();
      memcpy(&m_mWorld, &m, sizeof(D3DMATRIX)); 
      }
   else
      memcpy(&m_mWorld, mWorld, sizeof(D3DMATRIX)); 

   // recalculate connected values
   CalcWorldViewProjMatrix();

   // if shaders set transform as constant to device
   if (m_bCanDoShaders) {
      ZFXMatrix mTranspose;
      mTranspose.TransposeOf(*(ZFXMatrix*)&m_mWorldViewProj);
      m_pDevice->SetVertexShaderConstantF(0, (float*)&mTranspose, 4);
      }
   
   m_pDevice->SetTransform(D3DTS_WORLD, &m_mWorld);
   } // SetWorldTransform
/*----------------------------------------------------------------*/


/**
 * Set ambient light level to given values. If shaders are used the
 * ambient color is stored in C4.
 */
void ZFXD3D::SetAmbientLight(float fRed, float fGreen, float fBlue) {
   // last chance check
   m_pVertexMan->ForcedFlushAll();

   int nRed   = (int)(fRed * 255.0f);
   int nGreen = (int)(fGreen * 255.0f);
   int nBlue  = (int)(fBlue * 255.0f);

   if (m_bCanDoShaders) {
      // default setting to use as diffuse vertex color
      float fCol[4] = { fRed, fGreen, fBlue, 1.0f };
      m_pDevice->SetVertexShaderConstantF(4, fCol, 1);
      }

   m_pDevice->SetRenderState(D3DRS_AMBIENT,  
                             D3DCOLOR_XRGB(nRed, 
                             nGreen, nBlue));
   } // SetAmbientLight
/*----------------------------------------------------------------*/


/**
 * Creates a D3DXFont object from a GDI font the caller submitted
 * and returns its ID to the caller for later use.
 */
HRESULT ZFXD3D::CreateFont(const char *chType, int nWeight, bool bItalic,
                           bool bUnderline, bool bStrike, DWORD dwSize, 
                           UINT *pID) {
   HRESULT hr;
   HFONT   hFont;
   HDC     hDC;
   int     nHeight;

   if (!pID) return ZFX_INVALIDPARAM;

   hDC = GetDC( NULL );
   nHeight = -MulDiv(dwSize, GetDeviceCaps(hDC, LOGPIXELSY), 72);
   ReleaseDC(NULL, hDC);

   // create GDI font
   hFont = ::CreateFont(nHeight,      // logical height
                        0, 0, 0,      // avrg.width, esc.angle, orient.angle 
                        nWeight,      // thickness, 0=default, 700=bold
                        bItalic, bUnderline, bStrike,
                        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                        DEFAULT_PITCH | FF_DONTCARE, 
                        chType);      // Arial, etc...

   if (hFont == NULL) return ZFX_FAIL;

   m_pFont = (LPD3DXFONT*)realloc(m_pFont, sizeof(LPD3DXFONT)*(m_nNumFonts+1));

   // build D3DX font from GDI font
   hr = D3DXCreateFont(m_pDevice, hFont, &m_pFont[m_nNumFonts]);

   DeleteObject(hFont);

   if (SUCCEEDED(hr)) {
      (*pID) = m_nNumFonts;
      m_nNumFonts++;
      return ZFX_OK;
      }
   else return ZFX_FAIL;
   } // CreateFont
/*----------------------------------------------------------------*/


/**
 * Draws text using the previously created font object.
 */
HRESULT ZFXD3D::DrawText(UINT nID, int x, int y, UCHAR r,  
                         UCHAR g, UCHAR b, char *ch, ...) {
   RECT rc = { x, y, 0, 0 };
   char cch[1024];
   char *pArgs;
   
   // put variables into the string
   pArgs = (char*) &ch + sizeof(ch);
   vsprintf(cch, ch, pArgs);

   if (nID >= m_nNumFonts) return ZFX_INVALIDPARAM;

   m_pFont[nID]->Begin();
      // calculate actual size of the text
      m_pFont[nID]->DrawText(cch, -1, &rc, DT_SINGLELINE | DT_CALCRECT, 0);
      // now draw the text 
      m_pFont[nID]->DrawText(cch, -1, &rc, DT_SINGLELINE, 
                             D3DCOLOR_ARGB(255,r,g,b));
   m_pFont[nID]->End();

   return ZFX_OK;
   } // DrawText
/*----------------------------------------------------------------*/


/**
 * Fade the screen to given color by blending a semi-transparent
 * quad over the whole backbuffer.
 * -> IN: float - red
 *        float - green
 *        float - blue
 *        float - alpha
 */
void ZFXD3D::FadeScreen(float fR, float fG, float fB, float fA) {
   ZFXENGINEMODE OldMode;
   LVERTEX       v[4];
   bool          bChanged=false;
   WORD          wI[6] = { 0, 1, 2, 0, 3, 1 };
   DWORD         dwColor = D3DCOLOR_COLORVALUE(fR,fG,fB,fA);

   if (m_bIsSceneRunning)
      m_pDevice->EndScene();

   // actual skin gets invalid
   SetActiveSkinID(MAX_ID);

   v[0].x     = (float)m_dwWidth;   // oben rechts
   v[0].y     = 0.0f;
   v[0].z     = 1.0f;
   v[0].tu    = 1.0f;
   v[0].tv    = 0.0f;
   v[0].Color = dwColor;

   v[1].x     = 0.0f;   // unten links
   v[1].y     = (float)m_dwHeight;
   v[1].z     = 1.0f;
   v[1].tu    = 0.0f;
   v[1].tv    = 1.0f;
   v[1].Color = dwColor;

   v[2].x     = 0.0f;   // oben links
   v[2].y     = 0.0f;
   v[2].z     = 1.0f;
   v[2].tu    = 0.0f;
   v[2].tv    = 0.0f;
   v[2].Color = dwColor;

   v[3].x     = (float)m_dwWidth;   // unten rechts
   v[3].y     = (float)m_dwHeight;
   v[3].z     = 1.0f;
   v[3].tu    = 1.0f;
   v[3].tv    = 1.0f;
   v[3].Color = dwColor;

   SetWorldTransform(NULL);
   SetView3D(ZFXVector(1,0,0), ZFXVector(0,1,0),
             ZFXVector(0,0,1), ZFXVector(0,0,0));

   UseShaders(false);
   m_pDevice->SetTexture(0, NULL);
//   m_pDevice->SetStreamSource(0, NULL, 0, sizeof(LVERTEX));
//   m_pDevice->SetIndices(NULL);
   m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
   m_pDevice->SetFVF(FVF_LVERTEX);

   // check projection mode
   if (m_Mode != EMD_TWOD) {
      bChanged = true;
      OldMode  = m_Mode;
      SetMode(EMD_TWOD, m_nStage);
      }

   // set material with alpha value
   D3DMATERIAL9 mtrl;
   memset(&mtrl, 0, sizeof(D3DMATERIAL9));
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
   mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
   mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
   mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
	m_pDevice->SetMaterial(&mtrl);

   // activate alpha blending using alpha from material
   m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

   m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
   m_pDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
   m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
   m_pDevice->SetTextureStageState(0,D3DTSS_ALPHAOP, D3DTOP_MODULATE);
   m_pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
   m_pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

   // dont need zbuffer
   SetDepthBufferMode(RS_DEPTH_NONE);
   
   // dont need cool shading, skip to gain speed
   m_pDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);

   m_pDevice->BeginScene();

   if (FAILED(m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,
                                                0, 4, 2, wI, D3DFMT_INDEX16,
                                                v, sizeof(LVERTEX))))
      Log("error: DrawPrimitiveUP()");

   m_pDevice->EndScene();

   if (bChanged) SetMode(OldMode, m_nStage);

   m_pDevice->SetMaterial(&m_StdMtrl);

   // restore old d3d states
   m_pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2, D3DTA_CURRENT);
   m_pDevice->SetTextureStageState(0,D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
   m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
   m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
   m_pDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);

   SetDepthBufferMode(RS_DEPTH_READWRITE);

   if (m_bIsSceneRunning)
      m_pDevice->BeginScene();
   } // FadeScreen
/*----------------------------------------------------------------*/


