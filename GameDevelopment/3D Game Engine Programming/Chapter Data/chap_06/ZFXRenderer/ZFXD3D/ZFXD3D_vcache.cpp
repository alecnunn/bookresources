/*******************************************************************
 * ZFXEngine!                                                      *
 * (c)2003 by Stefan Zerbst | www.zfx.info                         *
 *-----------------------------------------------------------------*
 * File: ZFXD3D_vcache.cpp                                         *
 * part of render dll implementing direct3d rendering              *
 *******************************************************************/


// I N C L U D E S /////////////////////////////////////////////////

#include "ZFXD3D_vcache.h"    // class definition
#include "ZFXD3D.h"           // FVF definitions
#include "ZFX.h"              // return values and stuff


extern bool g_bLF;


// F U N C T I O N S ///////////////////////////////////////////////

/**
 * Constructor: Initializes vertex cache arrays
 */
ZFXD3DVCManager::ZFXD3DVCManager(ZFXD3DSkinManager *pSkinMan,
                                 LPDIRECT3DDEVICE9 pDevice,
                                 ZFXD3D *pZFXD3D, UINT nMaxVerts, 
                                 UINT nMaxIndis, FILE *pLog) {
   DWORD dwID=1;
   int   i=0;

   m_pSB    = NULL;
   m_pIB    = NULL;
   m_nNumSB = 0;
   m_nNumIB = 0;

   m_pLog          = pLog;
   m_pDevice       = pDevice;
   m_pZFXD3D       = pZFXD3D;
   m_pSkinMan      = pSkinMan;
   m_dwActiveCache = MAX_ID;
   m_dwActiveSB    = MAX_ID;
   m_dwActiveIB    = MAX_ID;

   for (i=0; i<NUM_CACHES; i++) {
      m_CachePS[i] = new ZFXD3DVCache(nMaxVerts, nMaxIndis, 
                                      sizeof(PVERTEX),
                                      pSkinMan, pDevice, this,
                                      dwID++, FVF_PVERTEX, pLog);

      m_CacheUU[i] = new ZFXD3DVCache(nMaxVerts, nMaxIndis, 
                                      sizeof(VERTEX),
                                      pSkinMan, pDevice, this,
                                      dwID++, FVF_VERTEX, pLog);
      
      m_CacheUL[i] = new ZFXD3DVCache(nMaxVerts, nMaxIndis, 
                                      sizeof(LVERTEX), 
                                      pSkinMan, pDevice, this,
                                      dwID++, FVF_LVERTEX, pLog);

      m_CacheCA[i] = new ZFXD3DVCache(nMaxVerts, nMaxIndis, 
                                      sizeof(CVERTEX), 
                                      pSkinMan, pDevice, this,
                                      dwID++, FVF_CVERTEX, pLog);

      m_Cache3T[i] = new ZFXD3DVCache(nMaxVerts, nMaxIndis, 
                                      sizeof(VERTEX3T), 
                                      pSkinMan, pDevice, this,
                                      dwID++, FVF_T3VERTEX, pLog);

      m_CacheTV[i] = new ZFXD3DVCache(nMaxVerts, nMaxIndis, 
                                      sizeof(TVERTEX), 
                                      pSkinMan, pDevice, this,
                                      dwID++, FVF_TVERTEX, pLog);
      } // for
   Log("online");
   } // constructor
/*----------------------------------------------------------------*/


/**
 * Destructor: Release vertex cache arrays
 */
ZFXD3DVCManager::~ZFXD3DVCManager(void) {
   UINT n=0;
   int  i=0;

   // release static buffers
   if ( m_pSB ) {
      for (n=0; n<m_nNumSB; n++) {
         if (m_pSB[n].pVB) {
            m_pSB[n].pVB->Release();
            m_pSB[n].pVB = NULL;
            }
         if (m_pSB[n].pIB) {
            m_pSB[n].pIB->Release();
            m_pSB[n].pIB = NULL;
            }
         }
      free( m_pSB );
      m_pSB = NULL;
      }

   // release index buffers
   for (n=0; n<m_nNumIB; n++) {
      if (m_pIB[n].pIB) {
         m_pIB[n].pIB->Release();
         m_pIB[n].pIB = NULL;
         }
      }

   // free memory for buffer lists
   if (m_pIB) { free(m_pIB); m_pIB=NULL; }

   // free dynamic vertex caches
   for (i=0; i<NUM_CACHES; i++) {
      if (m_CachePS[i]) {
         delete m_CachePS[i];
         m_CachePS[i] = NULL;
         }
      if (m_CacheUU[i]) {
         delete m_CacheUU[i];
         m_CacheUU[i] = NULL;
         }
      if (m_CacheCA[i]) {
         delete m_CacheCA[i];
         m_CacheCA[i] = NULL;
         }
      if (m_Cache3T[i]) {
         delete m_Cache3T[i];
         m_Cache3T[i] = NULL;
         }
      if (m_CacheTV[i]) {
         delete m_CacheTV[i];
         m_CacheTV[i] = NULL;
         }
      if (m_CacheUL[i]) {
         delete m_CacheUL[i];
         m_CacheUL[i] = NULL;
         }
      } // for
   Log("offline (ok)");
   } // destructor
/*----------------------------------------------------------------*/


ZFXRENDERSTATE ZFXD3DVCManager::GetShadeMode(void) 
   { return m_pZFXD3D->GetShadeMode(); }


/**
 * Create a static vertex/index buffer for the given data and returns
 * a handle to that buffer for later rendering processes.
 * -> IN:  ZFXVERTEXID - identify the vertex format used
 *         UINT        - skin that should be used
 *         UINT        - number of vertices to come
 *         UINT        - number of indices to come
 *         void        - pointer to vertex stream
 *         void        - pointer to index stream
 * -> OUT: UINT        - ID to the created buffer
 */
HRESULT ZFXD3DVCManager::CreateStaticBuffer(ZFXVERTEXID VertexID,
                                 UINT nSkinID, UINT nVerts, 
                                 UINT nIndis, const void *pVerts,
                                 const WORD *pIndis, UINT *pnID) {
   HRESULT  hr;
   DWORD    dwActualFVF;
   void    *pData;

   if (m_nNumSB >= (MAX_ID-1)) return ZFX_OUTOFMEMORY;

   // allocate memory for static buffers if needed
   if ( (m_nNumSB % 50) == 0) {
      int n = (m_nNumSB+50)*sizeof(ZFXSTATICBUFFER);
      m_pSB = (ZFXSTATICBUFFER*)realloc(m_pSB, n);
      if (!m_pSB) return ZFX_OUTOFMEMORY;
      }

   m_pSB[m_nNumSB].nNumVerts = nVerts;
   m_pSB[m_nNumSB].nNumIndis = nIndis;
   m_pSB[m_nNumSB].nSkinID   = nSkinID;
   m_pSB[m_nNumSB].pIB       = NULL;
   m_pSB[m_nNumSB].pVB       = NULL;

   // get size and format of vertex
   switch (VertexID) {
      case VID_PS: { 
         m_pSB[m_nNumSB].nStride = sizeof(PVERTEX); 
         m_pSB[m_nNumSB].dwFVF = FVF_PVERTEX;
         } break;
      case VID_UU: { 
         m_pSB[m_nNumSB].nStride = sizeof(VERTEX); 
         m_pSB[m_nNumSB].dwFVF = FVF_VERTEX;
         } break;
      case VID_UL: {
         m_pSB[m_nNumSB].nStride = sizeof(LVERTEX); 
         m_pSB[m_nNumSB].dwFVF = FVF_LVERTEX;
         } break;
      case VID_CA: {
         m_pSB[m_nNumSB].nStride = sizeof(CVERTEX); 
         m_pSB[m_nNumSB].dwFVF = FVF_CVERTEX;
         } break;
      case VID_3T: {
         m_pSB[m_nNumSB].nStride = sizeof(VERTEX3T); 
         m_pSB[m_nNumSB].dwFVF = FVF_T3VERTEX;
         } break;
      case VID_TV: {
         m_pSB[m_nNumSB].nStride = sizeof(TVERTEX); 
         m_pSB[m_nNumSB].dwFVF = FVF_TVERTEX;
         } break;

      default: return ZFX_INVALIDID;
      } // switch

   // create indexbuffer if needed
   if (nIndis > 0) {
      m_pSB[m_nNumSB].bIndis = true;
      m_pSB[m_nNumSB].nNumTris = int(nIndis / 3.0f);

      hr = m_pDevice->CreateIndexBuffer(
                           nIndis * sizeof(WORD),
                           D3DUSAGE_WRITEONLY,
                           D3DFMT_INDEX16,
                           D3DPOOL_DEFAULT,
                           &m_pSB[m_nNumSB].pIB, 
                           NULL);
      if (FAILED(hr)) return ZFX_CREATEBUFFER;

      // fill the index buffer
      if (SUCCEEDED(m_pSB[m_nNumSB].pIB->Lock(
                                     0, 0, (void**)
                                     (&pData), 0))) {
         memcpy(pData, pIndis, nIndis*sizeof(WORD));
         m_pSB[m_nNumSB].pIB->Unlock();
         }
      else return ZFX_BUFFERLOCK;
      }
   else {
      m_pSB[m_nNumSB].bIndis = false;
      m_pSB[m_nNumSB].nNumTris = int(nVerts / 3.0f);
      m_pSB[m_nNumSB].pIB = NULL;
      }

   // no need for FVF if shaders are used
   if (m_pZFXD3D->UsesShaders()) dwActualFVF = 0;
   else dwActualFVF = m_pSB[m_nNumSB].dwFVF;

   // create vertex buffer
   hr = m_pDevice->CreateVertexBuffer(
                           nVerts*m_pSB[m_nNumSB].nStride,
                           D3DUSAGE_WRITEONLY,
                           dwActualFVF,
                           D3DPOOL_DEFAULT,
                           &m_pSB[m_nNumSB].pVB, 
                           NULL);
   if (FAILED(hr)) return ZFX_CREATEBUFFER;

   // fill the vertex buffer
   if (SUCCEEDED(m_pSB[m_nNumSB].pVB->Lock(
                                  0, 0, (void**)
                                  (&pData), 0))) {
      memcpy(pData, pVerts, nVerts*m_pSB[m_nNumSB].nStride);
      m_pSB[m_nNumSB].pVB->Unlock();
      }
   else return ZFX_BUFFERLOCK;

   (*pnID) = m_nNumSB;
   m_nNumSB++;
   return ZFX_OK;
   } // CreateStaticBuffer
/*----------------------------------------------------------------*/


/**
 * Create a static index buffer for the given data and returns a 
 * handle to that buffer for later rendering processes.
 * -> IN:  UINT        - number of indices to come
 *         WORD        - pointer to index stream
 * -> OUT: UINT        - ID to the created buffer
 */
HRESULT ZFXD3DVCManager::CreateIndexBuffer(UINT nIndis,
                                           const WORD *pIndis, 
                                           UINT *pnID) {
   HRESULT  hr;
   void    *pData;

   if (m_nNumIB >= (MAX_ID-1)) return ZFX_OUTOFMEMORY;

   // allocate memory for static buffers if needed
   if ( (m_nNumIB % 50) == 0) {
      int n = (m_nNumIB+50)*sizeof(ZFXINDEXBUFFER);
      m_pIB = (ZFXINDEXBUFFER*)realloc(m_pIB, n);
      if (!m_pIB) return ZFX_OUTOFMEMORY;
      }

   m_pIB[m_nNumIB].nNumIndis = nIndis;
   m_pIB[m_nNumIB].nNumTris = int(nIndis / 3.0f);
   m_pIB[m_nNumIB].pIB = NULL;

   hr = m_pDevice->CreateIndexBuffer(
                        nIndis * sizeof(WORD),
                        D3DUSAGE_WRITEONLY,
                        D3DFMT_INDEX16,
                        D3DPOOL_DEFAULT,
                        &m_pIB[m_nNumIB].pIB, 
                        NULL);
   if (FAILED(hr)) return ZFX_CREATEBUFFER;

   // fill the index buffer
   if (SUCCEEDED(m_pIB[m_nNumIB].pIB->Lock(
                                  0, 0, (void**)
                                  (&pData), 0))) {
      memcpy(pData, pIndis, nIndis*sizeof(WORD));
      m_pIB[m_nNumIB].pIB->Unlock();
      }
   else return ZFX_BUFFERLOCK;

   (*pnID) = m_nNumIB;
   m_nNumIB++;
   return ZFX_OK;
   } // CreateIndexBuffer
/*----------------------------------------------------------------*/


/**
 * Flush all vertex caches no matter which vertex format.
 */
HRESULT ZFXD3DVCManager::ForcedFlushAll(void) {
   HRESULT hr = ZFX_OK;
   bool    bShaders = m_pZFXD3D->UsesShaders();
   int     i;

   for (i=0; i<NUM_CACHES; i++)
      if (!m_CachePS[i]->IsEmpty() )
         if (FAILED( m_CachePS[i]->Flush(bShaders) )) 
            hr = ZFX_FAIL;

   for (i=0; i<NUM_CACHES; i++)
      if (!m_CacheUU[i]->IsEmpty() )
         if (FAILED( m_CacheUU[i]->Flush(bShaders) )) 
            hr = ZFX_FAIL;

   for (i=0; i<NUM_CACHES; i++)
      if (!m_CacheUL[i]->IsEmpty() )
         if (FAILED( m_CacheUL[i]->Flush(bShaders) )) 
            hr = ZFX_FAIL;

   for (i=0; i<NUM_CACHES; i++)
      if (!m_CacheCA[i]->IsEmpty() )
         if (FAILED( m_CacheCA[i]->Flush(bShaders) )) 
            hr = ZFX_FAIL;

   for (i=0; i<NUM_CACHES; i++)
      if (!m_Cache3T[i]->IsEmpty() )
         if (FAILED( m_Cache3T[i]->Flush(bShaders) )) 
            hr = ZFX_FAIL;

   for (i=0; i<NUM_CACHES; i++)
      if (!m_CacheTV[i]->IsEmpty() )
         if (FAILED( m_CacheTV[i]->Flush(bShaders) )) 
            hr = ZFX_FAIL;
   return hr;
   } // ForcedFlushAll
/*----------------------------------------------------------------*/


/**
 * Flush all buffers. Call this function if you want to force all
 * contents send to render device to be drawn in backbuffer. This
 * will automaticly be done when calling EndScene() for RenderDevice.
 * -> IN: ZFXVERTEXID - ID for vertex type
 */
HRESULT ZFXD3DVCManager::ForcedFlush(ZFXVERTEXID VertexID) {
   ZFXD3DVCache **pCache=NULL;
   HRESULT hr = ZFX_OK;
   int i=0;

   switch (VertexID) {
      case VID_PS: { pCache = m_CachePS; } break;
      case VID_UU: { pCache = m_CacheUU; } break;
      case VID_UL: { pCache = m_CacheUL; } break;
      case VID_CA: { pCache = m_CacheCA; } break;
      case VID_3T: { pCache = m_Cache3T; } break;
      case VID_TV: { pCache = m_CacheTV; } break;

      // unknown vertex type
      default: return ZFX_INVALIDID;
      } // switch

   for (i=0; i<NUM_CACHES; i++)
      if (FAILED( pCache[i]->Flush(m_pZFXD3D->UsesShaders()) ))
         hr = ZFX_FAIL;

   return hr;
   } // ForcedFlush
/*----------------------------------------------------------------*/


/**
 * Render static buffers with the given skin. Indexbuffer is optional.
 * -> IN: UINT    - index of static vertexbuffer to use
 */
HRESULT ZFXD3DVCManager::Render(UINT nID) {
   HRESULT hr=ZFX_OK;
   int iT=0;

   ZFXRENDERSTATE sm = m_pZFXD3D->GetShadeMode();

   // active cache gets invalid
   m_dwActiveCache = MAX_ID;

   if (nID >= m_nNumSB) {
      Log("error: invalid static buffer ID");
      return ZFX_INVALIDPARAM;
      }

   // activate buffers if not already active
   if (m_dwActiveSB != nID) {
      // index buffer used?
      if (m_pSB[nID].bIndis) m_pDevice->SetIndices(m_pSB[nID].pIB);
   
      m_pDevice->SetStreamSource(0, m_pSB[nID].pVB, 
                                 0, m_pSB[nID].nStride);
      m_dwActiveSB = nID;
      }
   // there is maybe just another indexbuffer active
   else if (m_dwActiveIB != MAX_ID) {
      if (m_pSB[nID].bIndis) m_pDevice->SetIndices(m_pSB[nID].pIB);
      m_dwActiveIB = MAX_ID;
      }


   // is the device already using this skin?   
   if (m_pZFXD3D->GetActiveSkinID() != m_pSB[nID].nSkinID) {
      // set material for device
      ZFXSKIN *pSkin = &m_pSkinMan->m_pSkins[m_pSB[nID].nSkinID];
     
      
      if (sm == RS_SHADE_SOLID) {
         // set material for device
         ZFXMATERIAL *pMat = &m_pSkinMan->m_pMaterials[pSkin->nMaterial];

         if (!m_pZFXD3D->UsesShaders()) {
            D3DMATERIAL9 mat = { 
               pMat->cDiffuse.fR,  pMat->cDiffuse.fG,  pMat->cDiffuse.fB,  pMat->cDiffuse.fA,
               pMat->cAmbient.fR,  pMat->cAmbient.fG,  pMat->cAmbient.fB,  pMat->cAmbient.fA,
               pMat->cSpecular.fR, pMat->cSpecular.fG, pMat->cSpecular.fB, pMat->cSpecular.fA,
               pMat->cEmissive.fR, pMat->cEmissive.fG, pMat->cEmissive.fB, pMat->cEmissive.fA,
               pMat->fPower };

            m_pDevice->SetMaterial(&mat);
            }
         else {
            GetZFXD3D()->SetShaderConstant(SHT_PIXEL, DAT_FLOAT, 1, 1, &pMat->cAmbient);
            GetZFXD3D()->SetShaderConstant(SHT_PIXEL, DAT_FLOAT, 2, 1, &pMat->cDiffuse);
            GetZFXD3D()->SetShaderConstant(SHT_PIXEL, DAT_FLOAT, 3, 1, &pMat->cEmissive);
            GetZFXD3D()->SetShaderConstant(SHT_PIXEL, DAT_FLOAT, 4, 1, &pMat->cSpecular);
            }

         // forced rendering without textures?
         if ( m_pZFXD3D->UsesTextures() ) {
            // set texture for device
            for (iT=0; iT<8; iT++) {
               if (pSkin->nTexture[iT] != MAX_ID) {
                  m_pDevice->SetTexture(iT, (LPDIRECT3DTEXTURE9)
                                       m_pSkinMan->m_pTextures[
                                       pSkin->nTexture[iT]].pData);

                  m_pDevice->SetTextureStageState(iT, D3DTSS_TEXCOORDINDEX, 0);
                  m_pDevice->SetTextureStageState(iT, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                  m_pDevice->SetTextureStageState(iT, D3DTSS_COLORARG2, D3DTA_CURRENT);
                  m_pDevice->SetTextureStageState(iT, D3DTSS_COLOROP, m_pZFXD3D->GetTOP(iT));
                  }
               else break;
               } 
            // deactivate unused stages at all costs
            m_pDevice->SetTextureStageState(iT, D3DTSS_COLOROP, D3DTOP_DISABLE);
            }
         else {
            m_pDevice->SetTexture(0, NULL);
            m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
            }
         }
      else {
         ZFXCOLOR clrWire = m_pZFXD3D->GetWireColor();

         // set material
         D3DMATERIAL9 matW = { 
            clrWire.fR, clrWire.fG, clrWire.fB, clrWire.fA,
            clrWire.fR, clrWire.fG, clrWire.fB, clrWire.fA,
            0.0f,             0.0f,       0.0f,       1.0f,
            0.0f,             0.0f,       0.0f,       1.0f,
            1.0f };
         m_pDevice->SetMaterial(&matW);

         // set no texture for device
         m_pDevice->SetTexture(0, NULL);
         m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
         }
      
      // set alpha states for device
      if (pSkin->bAlpha) {
         m_pDevice->SetRenderState(D3DRS_ALPHAREF, 50);
         m_pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
         m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
         m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
         m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
         m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
         }
      else {
         m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
         m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
         }
      // skin will change now
      m_pZFXD3D->SetActiveSkinID(m_pSB[nID].nSkinID);
      } // [device->skin]

   // if using shaders they should be activated by the
   // ActivateVShader methods. Else set FVF
   if (!m_pZFXD3D->UsesShaders())
      m_pDevice->SetFVF(m_pSB[nID].dwFVF);

   //  should we use additive rendering?
   if (m_pZFXD3D->UsesAdditiveBlending()) {
      m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
      m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
      m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
      }

   //  should we use rendering to color buffer at all?
   if (!m_pZFXD3D->UsesColorBuffer()) {
      m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
      m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
      m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
      }


   // indexed primitive
   if (m_pSB[nID].bIndis) {
      // render content
      if ( sm == RS_SHADE_POINTS ) {
         hr = m_pDevice->DrawPrimitive(
                           D3DPT_POINTLIST, 0,
                           m_pSB[nID].nNumVerts);
         }
      else if ( sm == RS_SHADE_LINES ) {
         hr = m_pDevice->DrawIndexedPrimitive(
                        D3DPT_LINELIST,
                        0, 0, m_pSB[nID].nNumVerts,
                        0, m_pSB[nID].nNumIndis/2);
         }
      else if ( sm == RS_SHADE_HULLWIRE ) {
         hr = m_pDevice->DrawIndexedPrimitive(
                           D3DPT_LINESTRIP, 0,
                           0, m_pSB[nID].nNumVerts,
                           0, m_pSB[nID].nNumVerts);
         }
      else { // RS_SHADE_SOLID || RS_SHADE_TRIWIRE
         hr = m_pDevice->DrawIndexedPrimitive(
                           D3DPT_TRIANGLELIST, 0,
                           0, m_pSB[nID].nNumVerts,
                           0, m_pSB[nID].nNumTris);
         }
      }
   // non-indexed primitive
   else {
      // render content
      if ( sm == RS_SHADE_POINTS ) {
         hr = m_pDevice->DrawPrimitive(
                           D3DPT_POINTLIST, 0,
                           m_pSB[nID].nNumVerts);
         }
      else if ( sm == RS_SHADE_LINES ) {
         hr = m_pDevice->DrawPrimitive(
                           D3DPT_LINELIST, 0,
                           m_pSB[nID].nNumVerts/2);
         }
      else if ( sm == RS_SHADE_HULLWIRE ) {
         hr = m_pDevice->DrawPrimitive(
                           D3DPT_LINESTRIP, 0,
                           m_pSB[nID].nNumVerts);
         }
      else { // RS_SHADE_SOLID || RS_SHADE_TRIWIRE
         hr = m_pDevice->DrawPrimitive(
                           D3DPT_TRIANGLELIST, 0,
                           m_pSB[nID].nNumTris);
         }
      }

   return hr;
   } // Render
/*----------------------------------------------------------------*/


/**
 * Render static buffers with the given skin. Indexbuffer is optional.
 * -> IN: UINT    - index of static vertexbuffer to use
 */
HRESULT ZFXD3DVCManager::Render(UINT nSBID, UINT nIBID, UINT nSkin) {
   HRESULT hr=ZFX_OK;
   int iT=0;

   ZFXRENDERSTATE sm = m_pZFXD3D->GetShadeMode();

   // active cache gets invalid
   m_dwActiveCache = MAX_ID;

   if ((nSBID >= m_nNumSB) || (nIBID >= m_nNumIB)) {
      Log("error: invalid buffer ID");
      return ZFX_INVALIDPARAM;
      }

   // activate buffers if not already active
   if (m_dwActiveSB != nSBID) {
      m_pDevice->SetStreamSource(0, m_pSB[nSBID].pVB, 
                                 0, m_pSB[nSBID].nStride);
      m_dwActiveSB = nSBID;
      }
   if (m_dwActiveIB != nIBID) {
      m_pDevice->SetIndices(m_pIB[nIBID].pIB);
      m_dwActiveIB = nIBID;
      }


   // is the device already using this skin?   
   if (m_pZFXD3D->GetActiveSkinID() != nSkin) {
      // set material for device
      ZFXSKIN *pSkin = &m_pSkinMan->m_pSkins[nSkin];
     
      
      if (sm == RS_SHADE_SOLID) {
         // set material for device
         ZFXMATERIAL *pMat = &m_pSkinMan->m_pMaterials[pSkin->nMaterial];

         if (!m_pZFXD3D->UsesShaders()) {
            D3DMATERIAL9 mat = { 
               pMat->cDiffuse.fR,  pMat->cDiffuse.fG,  pMat->cDiffuse.fB,  pMat->cDiffuse.fA,
               pMat->cAmbient.fR,  pMat->cAmbient.fG,  pMat->cAmbient.fB,  pMat->cAmbient.fA,
               pMat->cSpecular.fR, pMat->cSpecular.fG, pMat->cSpecular.fB, pMat->cSpecular.fA,
               pMat->cEmissive.fR, pMat->cEmissive.fG, pMat->cEmissive.fB, pMat->cEmissive.fA,
               pMat->fPower };

            m_pDevice->SetMaterial(&mat);
            }
         else {
            GetZFXD3D()->SetShaderConstant(SHT_PIXEL, DAT_FLOAT, 1, 1, &pMat->cAmbient);
            GetZFXD3D()->SetShaderConstant(SHT_PIXEL, DAT_FLOAT, 2, 1, &pMat->cDiffuse);
            GetZFXD3D()->SetShaderConstant(SHT_PIXEL, DAT_FLOAT, 3, 1, &pMat->cEmissive);
            GetZFXD3D()->SetShaderConstant(SHT_PIXEL, DAT_FLOAT, 4, 1, &pMat->cSpecular);
            }

         // forced rendering without textures?
         if ( m_pZFXD3D->UsesTextures() ) {
            // set texture for device
            for (iT=0; iT<8; iT++) {
               if (pSkin->nTexture[iT] != MAX_ID) {
                  m_pDevice->SetTexture(iT, (LPDIRECT3DTEXTURE9)
                                       m_pSkinMan->m_pTextures[
                                       pSkin->nTexture[iT]].pData);

                  m_pDevice->SetTextureStageState(iT, D3DTSS_TEXCOORDINDEX, 0);
                  m_pDevice->SetTextureStageState(iT, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                  m_pDevice->SetTextureStageState(iT, D3DTSS_COLORARG2, D3DTA_CURRENT);
                  m_pDevice->SetTextureStageState(iT, D3DTSS_COLOROP, m_pZFXD3D->GetTOP(iT));
                  }
               else break;
               } 
            // deactivate unused stages at all costs
            m_pDevice->SetTextureStageState(iT, D3DTSS_COLOROP, D3DTOP_DISABLE);
            }
         else {
            m_pDevice->SetTexture(0, NULL);
            m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
            }
         }
      else {
         ZFXCOLOR clrWire = m_pZFXD3D->GetWireColor();
         // set material
         D3DMATERIAL9 matW = { 
            clrWire.fR, clrWire.fG, clrWire.fB, clrWire.fA,
            clrWire.fR, clrWire.fG, clrWire.fB, clrWire.fA,
            0.0f,             0.0f,       0.0f,       1.0f,
            0.0f,             0.0f,       0.0f,       1.0f,
            1.0f };
         m_pDevice->SetMaterial(&matW);

         // set no texture for device
         m_pDevice->SetTexture(0, NULL);
         m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
         }
      
      // set alpha states for device
      if (pSkin->bAlpha) {
         m_pDevice->SetRenderState(D3DRS_ALPHAREF, 50);
         m_pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
         m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
         m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
         m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
         m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
         }
      else {
         m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
         m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
         }
      // skin will change now
      m_pZFXD3D->SetActiveSkinID(nSkin);
      } // [device->skin]

   // if using shaders they should be activated by the
   // ActivateVShader methods. Else set FVF
   if (!m_pZFXD3D->UsesShaders())
      m_pDevice->SetFVF(m_pSB[nSBID].dwFVF);

   //  should we use additive rendering?
   if (m_pZFXD3D->UsesAdditiveBlending()) {
      m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
      m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
      m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
      }

   //  should we use rendering to color buffer at all?
   if (!m_pZFXD3D->UsesColorBuffer()) {
      m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
      m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
      m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
      }


   // render content
   if ( sm == RS_SHADE_POINTS ) {
      hr = m_pDevice->DrawPrimitive(
                        D3DPT_POINTLIST, 0,
                        m_pSB[nSBID].nNumVerts);
      }
   else if ( sm == RS_SHADE_LINES ) {
      hr = m_pDevice->DrawIndexedPrimitive(
                     D3DPT_LINELIST,
                     0, 0, m_pSB[nSBID].nNumVerts,
                     0, m_pIB[nIBID].nNumIndis/2);
      }
   else if ( sm == RS_SHADE_HULLWIRE ) {
      hr = m_pDevice->DrawIndexedPrimitive(
                        D3DPT_LINESTRIP, 0,
                        0, m_pSB[nSBID].nNumVerts,
                        0, m_pSB[nSBID].nNumVerts);
      }
   else { // RS_SHADE_SOLID || RS_SHADE_TRIWIRE
      hr = m_pDevice->DrawIndexedPrimitive(
                        D3DPT_TRIANGLELIST, 0,
                        0, m_pSB[nSBID].nNumVerts,
                        0, m_pIB[nIBID].nNumTris);
      }

   return hr;
   } // Render
/*----------------------------------------------------------------*/


/**
 * Render static buffers with the given skin. Indexbuffer is optional.
 * -> IN: UINT - index of static vertexbuffer to use
 *        UINT - skin to use while rendering the content
 *        UINT - min index used during this call
 *        UINT - max index used during this call
 *        UINT - number of primitives to render
 */
HRESULT ZFXD3DVCManager::Render(UINT nID, UINT SkinID, UINT StartIndex, 
                                UINT NumVerts, UINT NumTris) {
   HRESULT hr=ZFX_OK;
   int iT=0;

   ZFXRENDERSTATE sm = m_pZFXD3D->GetShadeMode();

   // active cache gets invalid
   m_dwActiveCache = MAX_ID;

   if (nID >= m_nNumSB) {
      Log("error: invalid static buffer ID");
      return ZFX_INVALIDPARAM;
      }

   // activate buffers if not already active
   if (m_dwActiveSB != nID) {
      // index buffer used?
      if (m_pSB[nID].bIndis) m_pDevice->SetIndices(m_pSB[nID].pIB);
   
      m_pDevice->SetStreamSource(0, m_pSB[nID].pVB, 
                                 0, m_pSB[nID].nStride);
      m_dwActiveSB = nID;
      }
   // there is maybe just another indexbuffer active
   else if (m_dwActiveIB != MAX_ID) {
      if (m_pSB[nID].bIndis) m_pDevice->SetIndices(m_pSB[nID].pIB);
      m_dwActiveIB = MAX_ID;
      }


   // is the device already using this skin?   
   if (m_pZFXD3D->GetActiveSkinID() != SkinID) {
      // set material for device
      ZFXSKIN *pSkin = &m_pSkinMan->m_pSkins[SkinID];
     
      
      if (sm == RS_SHADE_SOLID) {
         // set material for device
         ZFXMATERIAL *pMat = &m_pSkinMan->m_pMaterials[pSkin->nMaterial];

         if (!m_pZFXD3D->UsesShaders()) {
            D3DMATERIAL9 mat = { 
               pMat->cDiffuse.fR,  pMat->cDiffuse.fG,  pMat->cDiffuse.fB,  pMat->cDiffuse.fA,
               pMat->cAmbient.fR,  pMat->cAmbient.fG,  pMat->cAmbient.fB,  pMat->cAmbient.fA,
               pMat->cSpecular.fR, pMat->cSpecular.fG, pMat->cSpecular.fB, pMat->cSpecular.fA,
               pMat->cEmissive.fR, pMat->cEmissive.fG, pMat->cEmissive.fB, pMat->cEmissive.fA,
               pMat->fPower };

            m_pDevice->SetMaterial(&mat);
            }
         else {
            GetZFXD3D()->SetShaderConstant(SHT_PIXEL, DAT_FLOAT, 1, 1, &pMat->cAmbient);
            GetZFXD3D()->SetShaderConstant(SHT_PIXEL, DAT_FLOAT, 2, 1, &pMat->cDiffuse);
            GetZFXD3D()->SetShaderConstant(SHT_PIXEL, DAT_FLOAT, 3, 1, &pMat->cEmissive);
            GetZFXD3D()->SetShaderConstant(SHT_PIXEL, DAT_FLOAT, 4, 1, &pMat->cSpecular);
            }

         if ( m_pZFXD3D->UsesTextures() ) {
            // set texture for device
            for (iT=0; iT<8; iT++) {
               if (pSkin->nTexture[iT] != MAX_ID) {
                  m_pDevice->SetTexture(iT, (LPDIRECT3DTEXTURE9)
                                       m_pSkinMan->m_pTextures[
                                       pSkin->nTexture[iT]].pData);

                  m_pDevice->SetTextureStageState(iT, D3DTSS_TEXCOORDINDEX, 0);
                  m_pDevice->SetTextureStageState(iT, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                  m_pDevice->SetTextureStageState(iT, D3DTSS_COLORARG2, D3DTA_CURRENT);
                  m_pDevice->SetTextureStageState(iT, D3DTSS_COLOROP, m_pZFXD3D->GetTOP(iT));
                  }
               else break;
               } 
            // deactivate unused stages at all costs
            m_pDevice->SetTextureStageState(iT, D3DTSS_COLOROP, D3DTOP_DISABLE);
            }
         else {
            m_pDevice->SetTexture(0, NULL);
            m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
            }
         }
      else {
         ZFXCOLOR clrWire = m_pZFXD3D->GetWireColor();
         // set material
         D3DMATERIAL9 matW = { 
            clrWire.fR, clrWire.fG, clrWire.fB, clrWire.fA,
            clrWire.fR, clrWire.fG, clrWire.fB, clrWire.fA,
            0.0f,             0.0f,       0.0f,       1.0f,
            0.0f,             0.0f,       0.0f,       1.0f,
            1.0f };
         m_pDevice->SetMaterial(&matW);

         // set no texture for device
         m_pDevice->SetTexture(0, NULL);
         m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
         }
      
      // set alpha states for device
      if (pSkin->bAlpha) {
         m_pDevice->SetRenderState(D3DRS_ALPHAREF, 50);
         m_pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
         m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
         m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
         m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
         m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
         }
      else {
         m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
         m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
         }
      // skin will change now
      m_pZFXD3D->SetActiveSkinID(SkinID);
      } // [device->skin]

   // if using shaders they should be activated by the
   // ActivateVShader methods. Else set FVF
   if (!m_pZFXD3D->UsesShaders())
      m_pDevice->SetFVF(m_pSB[nID].dwFVF);

   // this method is for indexed primitives only
   if (!m_pSB[nID].bIndis) return ZFX_FAIL;

   //  should we use additive rendering?
   if (m_pZFXD3D->UsesAdditiveBlending()) {
      m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
      m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
      m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
      }

   //  should we use rendering to color buffer at all?
   if (!m_pZFXD3D->UsesColorBuffer()) {
      m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
      m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
      m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
      }

   // render content

// STATES OTHER THAT FINAL ELSE NOT EVALUATED /////////////

   if ( sm == RS_SHADE_POINTS ) {
      hr = m_pDevice->DrawPrimitive(
                        D3DPT_POINTLIST, 0,
                        NumVerts);
      }
   else if ( sm == RS_SHADE_LINES ) {
      hr = m_pDevice->DrawIndexedPrimitive(
                     D3DPT_LINELIST, 0,
                     0, NumVerts,
                     StartIndex, NumTris);
      }
   else if ( sm == RS_SHADE_HULLWIRE ) {
      hr = m_pDevice->DrawIndexedPrimitive(
                        D3DPT_LINESTRIP, 0,
                        0, NumVerts,
                        StartIndex, NumTris);
      }
   else { // RS_SHADE_SOLID || RS_SHADE_TRIWIRE
      hr = m_pDevice->DrawIndexedPrimitive(
                        D3DPT_TRIANGLELIST, 0,
                        0, NumVerts,
                        StartIndex, NumTris);
      }

   return hr;
   } // Render
/*----------------------------------------------------------------*/


/**
 * Takes the vertex and index lists and sorts them into one fitting
 * vertex cache that features the same vertexID. If none of those
 * caches is free one will be flushed out. Note that output of pixel
 * to any buffers is not immediately. Call ForcedFlush() if needed.
 * -> IN: ZFXVERTEXID - vertex type to be processed
 *        UINT        - number of vertices in this call
 *        UINT        - number of indices in this call
 *        void        - array of vertices
 *        WORD        - array of indices
 *        ZFXSKIN     - pointer to the skin used for those vertices
 */
HRESULT ZFXD3DVCManager::Render(ZFXVERTEXID VertexID,
                                UINT  nVerts, 
                                UINT  nIndis, 
                                const void *pVerts,
                                const WORD *pIndis,
                                UINT  SkinID) {
   ZFXD3DVCache **pCache=NULL,
                 *pCacheEmpty=NULL,
                 *pCacheFullest=NULL;
   int nEmptyVC   = -1;
   int nFullestVC = 0;

   bool bShaders = m_pZFXD3D->UsesShaders();
   
   // which vertex type is to be processed?
   switch (VertexID) {
      case VID_PS: { pCache = m_CachePS; } break;
      case VID_UU: { pCache = m_CacheUU; } break;
      case VID_UL: { pCache = m_CacheUL; } break;
      case VID_CA: { pCache = m_CacheCA; } break;
      case VID_3T: { pCache = m_Cache3T; } break;
      case VID_TV: { pCache = m_CacheTV; } break;
      default: return ZFX_INVALIDID;
      } // switch

   pCacheFullest = pCache[0];

   // active buffer might change now
   m_dwActiveSB = MAX_ID;

/*   
   // for wireframe sorting is not needed
   if ( GetShadeMode() != RS_SHADE_SOLID) {
      return pCache[0]->Add(nVerts, nIndis, pVerts, 
                            pIndis, bShaders);
      }
*/
   // SEARCH BEST FITTING CACHE FOR THE VERTICES

   // first check if any cache uses same skin. that is the
   // easiest option cause we can just add the verts then.
   for (int i=0; i<NUM_CACHES; i++) {
      if (pCache[i]->UsesSkin(SkinID))
         return pCache[i]->Add(nVerts, nIndis, pVerts, 
                               pIndis, bShaders);
     
      if (pCache[i]->IsEmpty())
         pCacheEmpty = pCache[i];

      if (pCache[i]->NumVerts() > pCacheFullest->NumVerts())
         pCacheFullest = pCache[i];
      }

   // no luck though. second option is to add vertices to
   // a flushed empty cache if any one is available.
   if (pCacheEmpty) {
      pCacheEmpty->SetSkin(SkinID, bShaders);
      return pCacheEmpty->Add(nVerts, nIndis, pVerts, 
                              pIndis, bShaders);
      }

   // still no luck with that one. final option is to force
   // one cache to flush itself so we get an empty cache. We
   // simply choose the fullest cache here to avoid render 
   // calls with just a few verts.
   pCacheFullest->Flush(bShaders);
   pCacheFullest->SetSkin(SkinID, bShaders);
   return pCacheFullest->Add(nVerts, nIndis, pVerts, 
                             pIndis, bShaders);
   } // Render
/*----------------------------------------------------------------*/


/**
 * Render without texture using VID_UU and no shaders
 */
HRESULT ZFXD3DVCManager::RenderNaked(UINT nNumV, const void *pVerts, bool b) {
   HRESULT hr=ZFX_OK;
   InvalidateStates();
   m_pZFXD3D->UseShaders(false);
   m_pDevice->SetTexture(0, NULL);

   if (b)
      m_pDevice->SetTexture(0, (LPDIRECT3DTEXTURE9)
                 m_pSkinMan->m_pTextures[2].pData);
   else
      m_pDevice->SetTexture(0, NULL);


   m_pDevice->SetFVF(FVF_PVERTEX);
   hr = m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 
                                   nNumV/3,
                                   pVerts,
                                   sizeof(PVERTEX));
   return hr;
   } // Render
/*----------------------------------------------------------------*/


/**
 * Set all states to invalid to force reset at next rendering.
 */
void ZFXD3DVCManager::InvalidateStates(void) {
   m_pZFXD3D->SetActiveSkinID(MAX_ID);
   m_dwActiveSB    = MAX_ID;
   m_dwActiveCache = MAX_ID;
   } // InvalidateStates
/*----------------------------------------------------------------*/


/**
 * Use normal Vertex Type here. Render a list of points.
 */
HRESULT ZFXD3DVCManager::RenderPoints(ZFXVERTEXID VID,
                                      UINT nVerts, 
                                      const void *pVerts,
                                      const ZFXCOLOR *pClr) {
   D3DMATERIAL9 mtrl;
   DWORD        dwFVF;
   int          nStride;

   // active skin and static buffer gets invalid
   InvalidateStates();

   if (pClr) {
      memset(&mtrl, 0, sizeof(D3DMATERIAL9));
	   mtrl.Diffuse.r = mtrl.Ambient.r = pClr->fR;
      mtrl.Diffuse.g = mtrl.Ambient.g = pClr->fG;
      mtrl.Diffuse.b = mtrl.Ambient.b = pClr->fB;
      mtrl.Diffuse.a = mtrl.Ambient.a = pClr->fA;
      m_pDevice->SetMaterial(&mtrl);
      }

   m_pDevice->SetTexture(0,NULL);

   // which vertex type is to be processed?
   switch (VID) {
      case VID_PS: { 
         nStride = sizeof(PVERTEX); 
         m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
         dwFVF   = FVF_PVERTEX;
         } break;
      case VID_UU: { 
         nStride = sizeof(VERTEX); 
         m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
         dwFVF   = FVF_VERTEX;
         } break;
      case VID_3T: { 
         nStride = sizeof(VERTEX3T); 
         m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
         dwFVF   = FVF_T3VERTEX;
         } break;
      case VID_TV: { 
         nStride = sizeof(TVERTEX); 
         m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
         dwFVF   = FVF_TVERTEX;
         } break;
      case VID_CA: { 
         nStride = sizeof(CVERTEX); 
         m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
         dwFVF   = FVF_CVERTEX;
         } break;
      case VID_UL: { 
         nStride = sizeof(LVERTEX);
         m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
         dwFVF   = FVF_LVERTEX;
         } break;
      default: return ZFX_INVALIDID;
      } // switch

   // shaders or FVF
   if ( m_pZFXD3D->UsesShaders() ) {
      m_pZFXD3D->ActivateVShader(0, VID);
      m_pZFXD3D->ActivatePShader(0);
      }
   else m_pDevice->SetFVF(dwFVF);

   // render list of points
   if (FAILED(m_pDevice->DrawPrimitiveUP(D3DPT_POINTLIST, nVerts, 
                                         pVerts, nStride))) {
      m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
      return ZFX_FAIL;
      }

   m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
   return ZFX_OK;
   } // RenderPoints
/*----------------------------------------------------------------*/


/**
 * Use normal Vertex Type here. Render a list of lines. 
 */
HRESULT ZFXD3DVCManager::RenderLines(ZFXVERTEXID VID,
                                     UINT nVerts, 
                                     const void *pVerts,
                                     const ZFXCOLOR *pClr,
                                     bool	bStrip) {
   D3DMATERIAL9 mtrl;
   DWORD        dwFVF;
   int          nStride;

   // we will change states
   ForcedFlushAll();

   // active skin and static buffer gets invalid
   InvalidateStates();

   if (pClr) {
      memset(&mtrl, 0, sizeof(D3DMATERIAL9));
	   mtrl.Diffuse.r = mtrl.Ambient.r = pClr->fR;
      mtrl.Diffuse.g = mtrl.Ambient.g = pClr->fG;
      mtrl.Diffuse.b = mtrl.Ambient.b = pClr->fB;
      mtrl.Diffuse.a = mtrl.Ambient.a = pClr->fA;
      m_pDevice->SetMaterial(&mtrl);
      }

   m_pDevice->SetTexture(0, NULL);
   m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
   m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
   m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

   // which vertex type is to be processed?
   switch (VID) {
      case VID_PS: { 
         nStride = sizeof(PVERTEX); 
         m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
         dwFVF   = FVF_PVERTEX;
         } break;
      case VID_UU: { 
         nStride = sizeof(VERTEX); 
         m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
         dwFVF   = FVF_VERTEX;
         } break;
      case VID_3T: { 
         nStride = sizeof(VERTEX3T); 
         m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
         dwFVF   = FVF_T3VERTEX;
         } break;
      case VID_TV: { 
         nStride = sizeof(TVERTEX); 
         m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
         dwFVF   = FVF_TVERTEX;
         } break;
      case VID_CA: { 
         nStride = sizeof(CVERTEX); 
         m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
         dwFVF   = FVF_CVERTEX;
         } break;
      case VID_UL: { 
         nStride = sizeof(LVERTEX);
         m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
         dwFVF   = FVF_LVERTEX;
         } break;
      default: return ZFX_INVALIDID;
      } // switch

   // shaders or FVF
   if ( m_pZFXD3D->UsesShaders() ) {
      m_pZFXD3D->ActivateVShader(0, VID);
      m_pZFXD3D->ActivatePShader(0);
      }
   else m_pDevice->SetFVF(dwFVF);

   // render list of lines
   if (!bStrip) {
      if (FAILED(m_pDevice->DrawPrimitiveUP(D3DPT_LINELIST, nVerts/2, 
                                            pVerts, nStride))) {
         m_pDevice->SetRenderState(D3DRS_LIGHTING , TRUE);
         return ZFX_FAIL;
         }
      }
   else {
      if (FAILED(m_pDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, nVerts-1, 
                                            pVerts, nStride))) {
         m_pDevice->SetRenderState(D3DRS_LIGHTING , TRUE);
         return ZFX_FAIL;
         }
      }
   m_pDevice->SetRenderState(D3DRS_LIGHTING , TRUE);
   return ZFX_OK;
   } // RenderLines
/*----------------------------------------------------------------*/


/**
 * Use normal Vertex Type here. Render one line. 
 */
HRESULT ZFXD3DVCManager::RenderLine(const float *fStart,
                                    const float *fEnd, 
                                    const ZFXCOLOR *pClr) {
   D3DMATERIAL9 mtrl;
   LVERTEX      pVerts[2];

   if (!pClr) return ZFX_INVALIDPARAM;

   // we will change states
   ForcedFlushAll();

   // active skin and static buffer gets invalid
   InvalidateStates();
   
   // make sure state is switched off
   GetZFXD3D()->UseShaders(false);

   // set coordinates
   pVerts[0].x = fStart[0];
   pVerts[0].y = fStart[1];
   pVerts[0].z = fStart[2];
   pVerts[1].x = fEnd[0];
   pVerts[1].y = fEnd[1];
   pVerts[1].z = fEnd[2];

   // set prelit
   pVerts[0].Color = pVerts[1].Color = D3DCOLOR_COLORVALUE(
      pClr->fR, pClr->fG,
      pClr->fB, pClr->fA);

   memset(&mtrl, 0, sizeof(D3DMATERIAL9));
	mtrl.Diffuse.r = mtrl.Ambient.r = pClr->fR;
   mtrl.Diffuse.g = mtrl.Ambient.g = pClr->fG;
   mtrl.Diffuse.b = mtrl.Ambient.b = pClr->fB;
   mtrl.Diffuse.a = mtrl.Ambient.a = pClr->fA;

   m_pDevice->SetMaterial(&mtrl);
   m_pDevice->SetTexture(0,NULL);

   m_pDevice->SetFVF(FVF_LVERTEX);
   m_pDevice->SetVertexShader(NULL);
   m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

   // render list of lines
   if (FAILED(m_pDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, 
                                         pVerts, sizeof(LVERTEX)))) {
      m_pDevice->SetFVF(NULL);
      m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
      return ZFX_FAIL;
      }
   m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
   m_pDevice->SetFVF(NULL);
   return ZFX_OK;
   } // RenderLine
/*----------------------------------------------------------------*/


/**
 * write outputstring to attribut outputstream if exists
 * -> IN: bool - flush immediately
 *        ...  - output values
 */
void ZFXD3DVCManager::Log(char *chString, ...) {

   char ch[256];
   char *pArgs;
   
   pArgs = (char*) &chString + sizeof(chString);
   vsprintf(ch, chString, pArgs);
   fprintf(m_pLog, "[ZFXD3DVCMngr]: ");
   fprintf(m_pLog, ch);
   fprintf(m_pLog, "\n");
   
   if (g_bLF)
      fflush(m_pLog);
   } // Log
/*----------------------------------------------------------------*/



// F U N C T I O N S ///////////////////////////////////////////////

/**
 * Constructor: Initialize members, especially vertex- and index-buffer.
 * -> IN: UINT       - max number of vertices in cache
 *        UINT       - max number of indices in cache
 *        UINT       - size of one vertex element
 *        SKINMAN    - pointer to initialized skin manager
 *        D3DDEVICE  - pointer to initialized d3d device
 */
ZFXD3DVCache::ZFXD3DVCache(UINT nVertsMax, UINT nIndisMax, 
                           UINT nStride,  ZFXD3DSkinManager *pSkinMan,
                           LPDIRECT3DDEVICE9 pDevice, ZFXD3DVCManager *pDad,
                           DWORD dwID, DWORD dwFVF, FILE *pLog) {
   HRESULT hr;

   m_pDevice      = pDevice;
   m_pSkinMan     = pSkinMan;
   m_pDad         = pDad;
   m_nNumVertsMax = nVertsMax;
   m_nNumIndisMax = nIndisMax;
   m_nNumVerts    = 0;
   m_nNumIndis    = 0;
   m_dwID         = dwID;
   m_dwFVF        = dwFVF;
   m_nStride      = nStride;
   m_pLog         = pLog;

   memset(&m_Skin, MAX_ID, sizeof(ZFXSKIN));
   m_SkinID = MAX_ID;

   // create the buffers
   m_pVB = NULL;
   m_pIB = NULL;

   hr = pDevice->CreateVertexBuffer(nVertsMax * nStride,
		                              D3DUSAGE_DYNAMIC | 
                                    D3DUSAGE_WRITEONLY,
                                    0, D3DPOOL_DEFAULT, 
                                    &m_pVB, NULL);
   if (FAILED(hr)) m_pVB = NULL;

   hr = pDevice->CreateIndexBuffer(nIndisMax * sizeof(WORD),
		                             D3DUSAGE_DYNAMIC | 
                                   D3DUSAGE_WRITEONLY,
                                   D3DFMT_INDEX16, 
                                   D3DPOOL_DEFAULT, &m_pIB, NULL);
   if (FAILED(hr)) m_pIB = NULL;
   } // constructor
/*----------------------------------------------------------------*/


/**
 * Destructor: Just release all of the stuff.
 */
ZFXD3DVCache::~ZFXD3DVCache(void) {
   if (m_pVB) {
      m_pVB->Release();
      m_pVB = NULL;
      }
   if (m_pIB) {
      m_pIB->Release();
      m_pIB = NULL;
      }
   } // destructor 
/*----------------------------------------------------------------*/


/**
 * Set the given texture for the given stage.
 * -> IN: ZFXSKIN - textures and material used by this polys
 */
void ZFXD3DVCache::SetSkin(UINT SkinID, bool bUseShaders) {
   // if a new texture is coming, flush all content of
   // the cache because that is using other textures
   if (!UsesSkin(SkinID)) {
      ZFXSKIN *pSkin = &m_pSkinMan->GetSkin(SkinID);

      if (!IsEmpty()) Flush(bUseShaders);
      
      memcpy(&m_Skin, pSkin, sizeof(ZFXSKIN));
      m_SkinID = SkinID;
      
      // skin changing so active cache is invalid any case
      m_pDad->InvalidateStates();
      }
   } // SetSkin
/*----------------------------------------------------------------*/


/**
 * Send the content of the cache to the renderer and resets the
 * cache to accept new content.
 */
HRESULT ZFXD3DVCache::Flush(bool bUseShaders) {
   HRESULT hr=ZFX_FAIL;
   int iT=0;

   // anything to do at all?
   if (m_nNumVerts <= 0) return ZFX_OK;

   // are the caches buffers still active?
   if ( m_pDad->GetActiveCache() != m_dwID) {


      // if using shaders they should be activated by the
      // ActivateVShader methods. Else set FVF
      if (!bUseShaders) m_pDevice->SetFVF(m_dwFVF);

      m_pDevice->SetIndices(m_pIB);
      m_pDevice->SetStreamSource(0, m_pVB, 0, m_nStride);
      m_pDad->SetActiveCache(m_dwID);
      } // [device->cache]


   // is the device already using this skin?   
   if (m_pDad->GetZFXD3D()->GetActiveSkinID() != m_SkinID) {
      // set material for device
      LPDIRECT3DTEXTURE9 pTex=NULL;
      ZFXMATERIAL *pMat = &m_pSkinMan->m_pMaterials[m_Skin.nMaterial];

      // WIREFRAME MODE NEEDS A SPECIAL CASE
      if (m_pDad->GetZFXD3D()->GetShadeMode() == RS_SHADE_SOLID) {
         if (!m_pDad->GetZFXD3D()->UsesShaders()) {
            D3DMATERIAL9 mat = { 
               pMat->cDiffuse.fR,  pMat->cDiffuse.fG,  pMat->cDiffuse.fB,  pMat->cDiffuse.fA,
               pMat->cAmbient.fR,  pMat->cAmbient.fG,  pMat->cAmbient.fB,  pMat->cAmbient.fA,
               pMat->cSpecular.fR, pMat->cSpecular.fG, pMat->cSpecular.fB, pMat->cSpecular.fA,
               pMat->cEmissive.fR, pMat->cEmissive.fG, pMat->cEmissive.fB, pMat->cEmissive.fA,
               pMat->fPower };

            m_pDevice->SetMaterial(&mat);
            }
         else {
            m_pDad->GetZFXD3D()->SetShaderConstant(SHT_PIXEL, DAT_FLOAT, 1, 1, &pMat->cAmbient);
            m_pDad->GetZFXD3D()->SetShaderConstant(SHT_PIXEL, DAT_FLOAT, 2, 1, &pMat->cDiffuse);
            m_pDad->GetZFXD3D()->SetShaderConstant(SHT_PIXEL, DAT_FLOAT, 3, 1, &pMat->cEmissive);
            m_pDad->GetZFXD3D()->SetShaderConstant(SHT_PIXEL, DAT_FLOAT, 4, 1, &pMat->cSpecular);
            }

         if ( m_pDad->GetZFXD3D()->UsesTextures() ) {
            // set texture for device
            for (iT=0; iT<8; iT++) {
               if (m_Skin.nTexture[iT] != MAX_ID) {
                  pTex = (LPDIRECT3DTEXTURE9)m_pSkinMan->m_pTextures[
                                             m_Skin.nTexture[iT]].pData;
                  if (FAILED(m_pDevice->SetTexture(iT, pTex)))
                     Log("error: SetTexture() in VC::Flush() failed");
               
                  // set texture operations
                  m_pDevice->SetTextureStageState(iT, D3DTSS_TEXCOORDINDEX, 0);
                  m_pDevice->SetTextureStageState(iT, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                  m_pDevice->SetTextureStageState(iT, D3DTSS_COLORARG2, D3DTA_CURRENT);
                  m_pDevice->SetTextureStageState(iT, D3DTSS_COLOROP, 
                                                  m_pDad->GetZFXD3D()->GetTOP(iT));
                  }
               else break;
               } // for
            // deactivate unused stages at all costs
            m_pDevice->SetTextureStageState(iT, D3DTSS_COLOROP, D3DTOP_DISABLE);
            }
         else {
            m_pDevice->SetTexture(0, NULL);
            m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
            }
         }
      else {
         ZFXCOLOR clrWire = m_pDad->GetZFXD3D()->GetWireColor();
         // set material
         D3DMATERIAL9 matW = { 
            clrWire.fR, clrWire.fG, clrWire.fB, clrWire.fA,
            clrWire.fR, clrWire.fG, clrWire.fB, clrWire.fA,
            0.0f,             0.0f,       0.0f,       1.0f,
            0.0f,             0.0f,       0.0f,       1.0f,
            1.0f };
         m_pDevice->SetMaterial(&matW);

         // set no texture for device
         m_pDevice->SetTexture(0, NULL);
         m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
         }

      // set alpha states for device
      if (m_Skin.bAlpha) {
         m_pDevice->SetRenderState(D3DRS_ALPHAREF, 50);
         m_pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
         m_pDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
         m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
         m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
         m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
         }
      else {
         m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
         m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
         }
      // now its active
      m_pDad->GetZFXD3D()->SetActiveSkinID(m_SkinID);
      } // [device->skin]

   //  should we use additive rendering?
   if (m_pDad->GetZFXD3D()->UsesAdditiveBlending()) {
      m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
      m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
      m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
      }

   //  should we use rendering to color buffer at all?
   if (!m_pDad->GetZFXD3D()->UsesColorBuffer()) {
      m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
      m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
      m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
      }

   ZFXRENDERSTATE sm = m_pDad->GetZFXD3D()->GetShadeMode();

   // render content
   if ( sm == RS_SHADE_POINTS ) {
      hr = m_pDevice->DrawPrimitive(
                        D3DPT_POINTLIST,
                        0, m_nNumVerts);
      }
   else if ( sm == RS_SHADE_LINES ) {
      hr = m_pDevice->DrawIndexedPrimitive(
                        D3DPT_LINELIST,
                        0, 0, m_nNumVerts,
                        0, m_nNumIndis/2);
      }
   else if ( sm == RS_SHADE_HULLWIRE ) {
      hr = m_pDevice->DrawIndexedPrimitive(
                        D3DPT_LINESTRIP,
                        0, 0, m_nNumVerts,
                        0, m_nNumVerts);
      }
   else { // RS_SHADE_SOLID || RS_SHADE_TRIWIRE
      hr = m_pDevice->DrawIndexedPrimitive(
                        D3DPT_TRIANGLELIST,
                        0, 0, m_nNumVerts,
                        0, m_nNumIndis/3);
      }

   if (FAILED(hr)) {
      Log("error: IDirect3DDevice::DrawIndexedPrimitive failed");
      return ZFX_FAIL;
      }

   // reset counters
   m_nNumVerts = 0;
   m_nNumIndis = 0;
   return ZFX_OK;
   } // Flush
/*----------------------------------------------------------------*/


/**
 * Fill data into the cache. If the cache is full it will be flushed
 * prior to accept new data. If no Indexlist is used set indexlist
 * parameter to NULL.
 * -> IN: UINT  - number of vertices in list
 *        UINT  - number of indices in list
 *        void* - pointer to vertex list
 *        WORD* - pointer to index list
 */
HRESULT ZFXD3DVCache::Add(UINT  nVerts, UINT nIndis, 
                          const void *pVerts,
                          const WORD *pIndices,
                          bool  bUseShaders) {
   BYTE *tmp_pVerts=NULL;         // pointer to VB memory
   WORD *tmp_pIndis=NULL;         // pointer to IB memory
   
   int   nSizeV = m_nStride*nVerts;
   int   nSizeI = sizeof(WORD)*nIndis;

   int   nPosV;
   int   nPosI;
   DWORD dwFlags;

   // lists will never fit into this cache
   if (nVerts>m_nNumVertsMax || nIndis>m_nNumIndisMax)
      return ZFX_BUFFERSIZE;

   // cache is already full, so flush it
   if ( (nVerts+m_nNumVerts > m_nNumVertsMax) ||
        (nIndis+m_nNumIndis > m_nNumIndisMax) ) {
      if ( Flush(bUseShaders) != ZFX_OK) {
         Log("error: Flush()");
         return ZFX_FAIL;
         }
      }

   // if nothing is in buffer discard content
   if (m_nNumVerts == 0) {
      nPosV = nPosI = 0;
      dwFlags = D3DLOCK_DISCARD;
      }
   // else just append data without overwrites
   else {
      nPosV = m_nStride*m_nNumVerts;
      nPosI = sizeof(WORD)*m_nNumIndis;
      dwFlags = D3DLOCK_NOOVERWRITE;
      }
          
   // now lock buffers
   if (FAILED(m_pVB->Lock(nPosV, nSizeV, (void**)&tmp_pVerts, dwFlags)))
      return ZFX_BUFFERLOCK;
   if (FAILED(m_pIB->Lock(nPosI, nSizeI, (void**)&tmp_pIndis, dwFlags))) {
      m_pVB->Unlock();
      return ZFX_BUFFERLOCK;
      }

   // copy vertex data into the buffer
   memcpy(tmp_pVerts, pVerts, nSizeV);

   // copy indices into the buffer
   int nBase = m_nNumVerts;
   if (!pIndices) nIndis = nVerts;

   for (UINT i=0; i<nIndis; i++) {
      if (pIndices != NULL)
         tmp_pIndis[i] = pIndices[i] + nBase;
      else
         tmp_pIndis[i] = i + nBase;
      m_nNumIndis++;
      }

   // add to count
   m_nNumVerts += nVerts;

   // unlock buffers
   m_pVB->Unlock();
   m_pIB->Unlock();
   return ZFX_OK;
   } // Add
/*----------------------------------------------------------------*/


/**
 * write outputstring to attribut outputstream if exists
 * -> IN: bool - flush immediately
 *        char - format string to output
 *        ...  - output values
 */
void ZFXD3DVCache::Log(char *chString, ...) {

   char ch[256];
   char *pArgs;
   
   pArgs = (char*) &chString + sizeof(chString);
   vsprintf(ch, chString, pArgs);
   fprintf(m_pLog, "[ZFXD3DVCache]: ");
   fprintf(m_pLog, ch);
   fprintf(m_pLog, "\n");
   
   if (g_bLF)
      fflush(m_pLog);
   } // Log
/*----------------------------------------------------------------*/
