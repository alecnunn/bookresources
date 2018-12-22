// FILE: CGameBulletList.h

#ifndef CGAMEBULLETLIST_H
#define CGAMEBULLETLIST_H


#include <windows.h>
#include "ZFXRenderer.h"
#include "ZFXRenderDevice.h"
#include "ZFXAudioDevice.h"
#include "CGameLevel.h"
#include "CGamePlayer.h"



// P R O T O T Y P E S /////////////////////////////////////////////

class CGameBulletList {
   public: 
      CGameBulletList(LPZFXRENDERDEVICE, LPZFXAUDIODEVICE);
      virtual ~CGameBulletList(void);

      void      Update(CGameLevel*, LPGAMECHARACTER*, UINT,
                       LPGAMECHARACTER*, UINT, float fET);
      HRESULT   Render(ZFXVector vcDir);
      HRESULT   AddBullet(const BULLET&);

   private:
      LPZFXRENDERDEVICE  m_pDevice;
      LPZFXAUDIODEVICE   m_pADevice;
      VERTEX             m_vQuad[4];
      ZFXVector          m_vcQuad[4];
      BULLET            *m_pBullets;
      UINT               m_Num;
      UINT               m_NumMax;
      UINT               m_SkinID;
      UINT               m_iAudio_Gun;


      void   RemoveBullet(UINT);
   }; // class
typedef class CGameBulletList *LPGAMEBULLETLIST;
/*----------------------------------------------------------------*/

#endif