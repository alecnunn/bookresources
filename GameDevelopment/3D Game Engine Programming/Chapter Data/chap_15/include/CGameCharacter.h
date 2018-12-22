// FILE: CGameCharacter.h

#ifndef CGAMECHARACTER_H
#define CGAMECHARACTER_H


#include <windows.h>
#include "ZFXGeneral.h"
#include "ZFXRenderer.h"
#include "ZFXRenderDevice.h"
#include "ZFXNetworkDevice.h"
#include "CGameLevel.h"
#include "CShadowVolume.h"   
#include "CZFXModel.h"


const UINT MSG_UPDATE       = 4;
const UINT MSG_PLAYERLIST   = 5;
const UINT MSG_TOUCHANDUSE  = 6;
const UINT MSG_FIREBULLET   = 7;
const UINT MSG_PLAYERFROZEN = 8;


const float STEPHEIGHT = 0.1f;


// used for network update
typedef struct UPDATEPKG_TYPE {
   UINT      NID;
   ZFXVector vcPos;
   float     fRotY;
   } UPDATEPKG;
/*----------------------------------------------------------------*/

// used for bullets
typedef struct BULLET_TYPE {
   UINT      ID;        // owner
   bool      bIsBot;    // is owner bot
   UINT      SectorID;  // home sector
   ZFXVector vcPos;     // position
   ZFXVector vcDir;     // heading
   UCHAR     nDamage;   // hit points
   float     fSpeed;    // m/sec
   float     fLifeSpan; // sec
   } BULLET;
/*----------------------------------------------------------------*/


// P R O T O T Y P E S /////////////////////////////////////////////

class CGameCharacter {
   public:
      CGameCharacter(void);
      virtual ~CGameCharacter(void);

      // main methods
      virtual void Update(CGameLevel*, bool, CGameCharacter**, 
                          UINT, CGameCharacter**, UINT, float)=0;

      HRESULT Render(void);
      HRESULT RenderShadow(CGameLight*);

      void ApplyDamage(UINT, UINT OwnerID);

      bool TestCollision(const ZFXRay&, float, float*);

      HRESULT FireBullet(void)  { if (!IsFrozen()) return SendMsg_FireBullet(); return ZFX_OK; }
      HRESULT TouchAndUse(void) { if (!IsFrozen()) return SendMsg_TouchAndUse(); return ZFX_OK; }

      // network stuff
      UPDATEPKG GetUpdate(void);
      void      SetUpdate(UPDATEPKG*);

      // misc accessor methods
      void      SetID(UINT n)      { m_ID = n;                     }
      UINT      GetID(void)        { return m_ID;                  }
      UINT      GetCS(void)        { return m_CS;                  }
      ZFXVector GetPos(void)       { return m_pMCEgo->GetPos();    }
      ZFXVector GetRight(void)     { return m_pMCEgo->GetRight();  }
      ZFXVector GetUp(void)        { return m_pMCEgo->GetUp();     }
      ZFXVector GetDir(void)       { return m_pMCEgo->GetDir();    }
      UINT      GetEnergy(void)    { return m_nEnergy;             }
      float     GetTouchDown(void) { return m_fTouchDown;          }
      ZFXAabb   GetAabb(void);

      bool      IsIdle(void)   { return m_bIdle;                }
      bool      IsFrozen(void) { return (m_fFrozen > 0.0f);     }
      bool      IsBot(void)    { return m_bIsBot;               }

      void SetTouchDown(float f)  { m_fTouchDown = f; }
      void SetRotaSpeedY(float f) { if (!IsFrozen()) m_pMCEgo->SetRotationSpeedY(f); }
      void SetRotaSpeedX(float f) { if (!IsFrozen()) m_pMCEgo->SetRotationSpeedX(f); }
      void SetSpeed(float f)      { if (!IsFrozen()) m_pMCEgo->SetSpeed(f); m_bMoved=true; }
      void SetSlideSpeed(float f) { if (!IsFrozen()) m_pMCEgo->SetSlideSpeed(f); m_bMoved=true; }

   protected:
      LPZFXRENDERDEVICE   m_pRDevice;
      LPZFXNETWORKDEVICE  m_pNDevice;
      ZFXMCEgo           *m_pMCEgo;
      CZFXModel          *m_pModel;
      CShadowVolume      *m_pShadow;
      UINT                m_VSH;
      UINT                m_PSH;
      UINT                m_ID;
      UINT                m_CS;
      UINT                m_nEnergy;
      float               m_fFrozen;
      float               m_fTouchDown;
      bool                m_bMoved;
      bool                m_bIsFalling;
      bool                m_bIdle;
      bool                m_bIsBot;
      
      void InitShadowVolume(void);
      void UpdateModel(float fElapsed);
      bool Intersects(CGameCharacter**, UINT);

      // networking messages
      HRESULT SendMsg_TouchAndUse(void);
      HRESULT SendMsg_FireBullet(void);
      HRESULT SendMsg_PlayerFrozen(UINT);
   }; // class
typedef class CGameCharacter *LPGAMECHARACTER;
/*----------------------------------------------------------------*/

#endif