/*******************************************************************
 * ZFXEngine!                                                      *
 * (c)2003 by Stefan Zerbst | www.zfx.info                         *
 *-----------------------------------------------------------------*
 * File: ZFXD3D_vcache.h                                           *
 * part of render dll implementing direct3d rendering              *
 *******************************************************************/

#ifndef ZFXD3D_VCACHE_H
#define ZFXD3D_VCACHE_H


#include <d3d9.h>
#include "ZFX.h"
#include "ZFXD3D_skinman.h"

class ZFXD3DVCManager;
class ZFXD3DVCache;

#define NUM_CACHES      10    // number of caches in manager


// struct for static vertex/index data
typedef struct ZFXSTATICBUFFER_TYPE {
   int   nStride;
   UINT  nSkinID;
   bool  bIndis;
   int   nNumVerts;
   int   nNumIndis;
   int   nNumTris;
   DWORD dwFVF;
   LPDIRECT3DVERTEXBUFFER9 pVB;
   LPDIRECT3DINDEXBUFFER9  pIB;
   } ZFXSTATICBUFFER;
/*----------------------------------------------------------------*/

// struct for index data
typedef struct ZFXINDEXBUFFER_TYPE {
   int   nNumIndis;
   int   nNumTris;
   LPDIRECT3DINDEXBUFFER9  pIB;
   } ZFXINDEXBUFFER;
/*----------------------------------------------------------------*/


class ZFXD3D;

/**
 * Class to manage static and dynamic vertex bunches, optionally
 * using indices during rendering process.
 */
class ZFXD3DVCManager : public ZFXVertexCacheManager {
   public:
      ZFXD3DVCManager(ZFXD3DSkinManager *pSkinMan,
                      LPDIRECT3DDEVICE9 pDevice,
                      ZFXD3D *pZFXD3D, UINT nMaxVerts,
                      UINT nMaxIndis, FILE *pLog);
      ~ZFXD3DVCManager(void);

      HRESULT CreateStaticBuffer(ZFXVERTEXID VertexID,
                                 UINT nSkinID,
                                 UINT nVerts, 
                                 UINT nIndis, 
                                 const void *pVerts,
                                 const WORD *pIndis,
                                 UINT *pnID);

      HRESULT CreateIndexBuffer(UINT,const WORD*,UINT*);
      
      HRESULT Render(ZFXVERTEXID VertexID,
                     UINT  nVerts, 
                     UINT  nIndis, 
                     const void *pVerts,
                     const WORD *pIndis,
                     UINT  SkinID);

      HRESULT RenderNaked(UINT, const void*, bool);

      HRESULT Render(UINT);
      HRESULT Render(UINT,UINT,UINT);
      HRESULT Render(UINT, UINT, UINT, UINT, UINT);

      HRESULT RenderPoints(ZFXVERTEXID     VertexID,
                           UINT            nVerts,
                           const void     *pVerts,
                           const ZFXCOLOR *pClrl);

      HRESULT RenderLines(ZFXVERTEXID     VertexID,
                          UINT            nVerts,
                          const void     *pVerts,
                          const ZFXCOLOR *pClrl,
                          bool            bStrip);

      HRESULT RenderLine(const float *fStart,
                         const float *fEnd, 
                         const ZFXCOLOR *pClr);

      HRESULT ForcedFlushAll(void);

      HRESULT ForcedFlush(ZFXVERTEXID VertexID);

      DWORD   GetActiveCache(void)       { return m_dwActiveCache; }
      void    SetActiveCache(DWORD dwID) { m_dwActiveCache = dwID; }

      ZFXD3D* GetZFXD3D(void) { return m_pZFXD3D; }

      void    InvalidateStates(void);

      ZFXRENDERSTATE GetShadeMode(void);


   private:
      ZFXD3DSkinManager *m_pSkinMan;
      LPDIRECT3DDEVICE9  m_pDevice;
      ZFXD3D            *m_pZFXD3D;

      ZFXSTATICBUFFER   *m_pSB;
      ZFXINDEXBUFFER    *m_pIB;
      UINT               m_nNumSB;
      UINT               m_nNumIB;
      ZFXD3DVCache      *m_CachePS[NUM_CACHES];    // position only
      ZFXD3DVCache      *m_CacheUU[NUM_CACHES];    // Untransformed Unlit
      ZFXD3DVCache      *m_CacheUL[NUM_CACHES];    // Untransformed Lit
      ZFXD3DVCache      *m_CacheCA[NUM_CACHES];    // character animation
      ZFXD3DVCache      *m_Cache3T[NUM_CACHES];    // three textures
      ZFXD3DVCache      *m_CacheTV[NUM_CACHES];    // uu with tanget
      DWORD              m_dwActiveCache;
      DWORD              m_dwActiveSB;   
      DWORD              m_dwActiveIB;
      FILE              *m_pLog;

      void Log(char *, ...);
   }; // class
/*----------------------------------------------------------------*/


/**
 * Class for batching together polygons to be rendered in dynamic buffers.
 * Will flush content if requested or if max number of verts/indis in
 * cache is reached.
 */
class ZFXD3DVCache {
   public:

      ZFXD3DVCache(UINT nVertsMax, UINT nIndisMax, 
                   UINT nStride, ZFXD3DSkinManager *pSkinMan,
                   LPDIRECT3DDEVICE9 pDevice, ZFXD3DVCManager *pDad,
                   DWORD dwID, DWORD dwFVF, FILE *pLog);

      ~ZFXD3DVCache(void);

      HRESULT Flush(bool bUseShaders);

      HRESULT Add(UINT  nVerts, UINT nIndis, 
                  const void *pVerts,
                  const WORD *pIndis,
                  bool  bUseShaders);

      void SetSkin(UINT  SkinID, bool bUseShader);
      bool UsesSkin(UINT SkinID) { return (m_SkinID == SkinID); }
      bool IsEmpty(void) { if (m_nNumVerts>0) return false; return true; }
      int  NumVerts(void) { return m_nNumVerts; }



   private:
      LPDIRECT3DVERTEXBUFFER9  m_pVB;
      LPDIRECT3DINDEXBUFFER9   m_pIB;
      LPDIRECT3DDEVICE9        m_pDevice;
      ZFXD3DSkinManager       *m_pSkinMan;
      ZFXD3DVCManager         *m_pDad;
      ZFXSKIN                  m_Skin;
      UINT                     m_SkinID;
      DWORD                    m_dwID;
      DWORD                    m_dwFVF;
      FILE                    *m_pLog;

      UINT  m_nNumVertsMax;   // maximum verts in buffer
      UINT  m_nNumIndisMax;   // maximum indices in buffer
      UINT  m_nNumVerts;      // actual number in buffer
      UINT  m_nNumIndis;      // actual number in buffer
      UINT  m_nStride;        // stride of one vertex element

      void Log(char *, ...);
   }; // class
/*----------------------------------------------------------------*/

#endif
