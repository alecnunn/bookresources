// FILE: CGamePlayer.h

#ifndef CGAMEPLAYER_H
#define CGAMEPLAYER_H


#include <windows.h>
#include "ZFXGeneral.h"
#include "ZFXRenderer.h"
#include "ZFXRenderDevice.h"
#include "ZFXNetworkDevice.h"
#include "CGameLevel.h"
#include "CShadowVolume.h"   
#include "CZFXModel.h"
#include "CGameCharacter.h"



// P R O T O T Y P E S /////////////////////////////////////////////

class CGamePlayer : public CGameCharacter {
   public: 
      CGamePlayer(const char*, LPZFXRENDERDEVICE, 
                  LPZFXNETWORKDEVICE,
                  UINT, UINT, CGameLevel*);
      virtual ~CGamePlayer(void);

      // main methods
      void      Update(CGameLevel*, bool, CGameCharacter**, 
                       UINT, CGameCharacter**, UINT, float);

   private:

   }; // class
typedef class CGamePlayer *LPGAMEPLAYER;
/*----------------------------------------------------------------*/

#endif