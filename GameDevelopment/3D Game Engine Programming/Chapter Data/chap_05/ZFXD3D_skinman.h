/*******************************************************************
 * ZFXEngine!                                                      *
 * (c)2003 by Stefan Zerbst | www.zfx.info                         *
 *-----------------------------------------------------------------*
 * File: ZFXD3D_skinman.h                                          *
 * part of render dll implementing direct3d rendering              *
 *******************************************************************/

#ifndef ZFXD3D_SKINMAN_H
#define ZFXD3D_SKINMAN_H


#include <d3d9.h>
#include "../ZFXRenderDevice.h"  // material manager base class
#include "ZFX.h"                 // return values and structs

#define MAX_ID 65535

class ZFXD3DSkinManager : public ZFXSkinManager {

   friend class ZFXD3DVCache;

   public:
      ZFXD3DSkinManager(LPDIRECT3DDEVICE9 pDevice, FILE *pLog);
      ~ZFXD3DSkinManager(void);

      HRESULT AddSkin(const ZFXCOLOR *pcAmbient,
                      const ZFXCOLOR *pcDiffuse,
                      const ZFXCOLOR *pcEmissive,
                      const ZFXCOLOR *pcSpecular,
                      float fSpecPower, UINT *nSkinID);

      HRESULT AddTexture(UINT nSkinID, const char *chName,
                         bool bAlpha, float fAlpha,
                         ZFXCOLOR *cColorKeys,
                         DWORD dwNumColorKeys);

      bool MaterialEqual(const ZFXMATERIAL *pMat0, 
                         const ZFXMATERIAL *pMat1);

      void LogCurrentStatus(char *chLog, bool bDetailed);

   protected:
      LPDIRECT3DDEVICE9  m_pDevice;
      FILE              *m_pLog;


      inline bool ColorEqual(const ZFXCOLOR *pCol0, 
                             const ZFXCOLOR *pCol1);
      HRESULT CreateTexture(ZFXTEXTURE *pTexture, bool bAlpha);
      HRESULT SetAlphaKey(LPDIRECT3DTEXTURE9 *ppTexture,
                          UCHAR R, UCHAR G, 
                          UCHAR B, UCHAR A);
      HRESULT SetTransparency(LPDIRECT3DTEXTURE9 *ppTexture,
                              UCHAR Alpha);
      DWORD   MakeD3DColor(UCHAR R, UCHAR G, UCHAR B, UCHAR A);
      void    Log(char *, ...);
   };

#endif