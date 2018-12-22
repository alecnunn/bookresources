// FILE: CGamePlayer.cpp


#include "include/CGamePlayer.h"

const float FREEZE_TIME = 5.0f;


CGamePlayer::CGamePlayer(const char *chModel, 
                         LPZFXRENDERDEVICE pRDevice,
                         LPZFXNETWORKDEVICE pNDevice,
                         UINT VSH, UINT PSH,
                         CGameLevel *pLevel) {
   ZFXVector vc(0.0f, 1.5f, 0.0f);

   m_pModel     = NULL;
   m_pShadow    = NULL;
   m_pMCEgo     = NULL;
   m_pRDevice   = pRDevice;
   m_pNDevice   = pNDevice;
   m_VSH        = VSH;
   m_PSH        = PSH;
   m_ID         = 0;
   m_bMoved     = false;
   m_bIsFalling = false;
   m_bIdle      = true;
   m_bIsBot     = false;
   m_CS         = 0;
   m_nEnergy    = 100;
   m_fFrozen    = 0.0f;
   
   // load character model object
   m_pModel = new CZFXModel( (const PCHAR)chModel, pRDevice, 1.8f);
   m_pModel->SetAnimation(false, 1 );
   
   m_pMCEgo = new ZFXMCEgo();

   // get random respawn point
   pLevel->GetSpawnPoint(&m_CS, &vc);
   m_pMCEgo->SetPos( vc );

   InitShadowVolume();
   } // constructor
/*----------------------------------------------------------------*/


CGamePlayer::~CGamePlayer(void) {
   SAFE_DELETE(m_pModel);
   SAFE_DELETE(m_pShadow);
   SAFE_DELETE(m_pMCEgo);
   } // destructor
/*----------------------------------------------------------------*/


void CGamePlayer::Update(CGameLevel *pLevel, bool bFull, 
                         LPGAMECHARACTER *ppPlayers, 
                         UINT nNumP, 
                         LPGAMECHARACTER *ppBots, 
                         UINT nNumB, float fElapsed) {
   UINT     nNS=m_CS;  // current sector
   float    fH=0.0f;   // current height
   ZFXAabb  aabb;
   ZFXPlane Plane;

   // update freeze counter if frozen
   if (m_fFrozen > 0.0f) {
      m_fFrozen -= fElapsed;

      if (m_fFrozen <= 0.0f) {
         m_fFrozen = 0.0f;
         m_nEnergy = 100;
         }
      }


   // full update means level collision check
   if ( (m_bMoved || m_bIsFalling) && bFull) {

      // get positions before and after movement
      ZFXVector vcPosOld = m_pMCEgo->GetPos();
      m_pMCEgo->Update( fElapsed );
      ZFXVector vcPosNew = m_pMCEgo->GetPos();

      // get correct height above floor
      if (pLevel->GetFloor(m_CS, vcPosNew, &fH, NULL)) 
         {
         // set correct eye height above floor
         float fNew = vcPosNew.y - fH + 1.5f;

         // player is falling / going downstairs
         if ( (fNew < vcPosOld.y) &&
              ((vcPosOld.y - fNew) > STEPHEIGHT) ) {
            vcPosNew.y = vcPosOld.y - STEPHEIGHT;
            m_bIsFalling = true;
            }
         // player is going upstairs
         else if ( (fNew > vcPosOld.y) &&
                   ((fNew - vcPosOld.y) > STEPHEIGHT) ) {
            vcPosNew.y = vcPosOld.y + STEPHEIGHT;
            m_bIsFalling = true;
            }
         // player steps up, down or level
         else {
            vcPosNew.y = fNew;
            m_bIsFalling = false;
            }
         }
      // player not above any level polygon 
      else vcPosNew.y = m_pMCEgo->GetPos().y;

      // build players bounding box
      aabb.vcMax    = vcPosNew + 0.1f;
      aabb.vcMin    = vcPosNew - 0.1f;
      aabb.vcMax.y  = vcPosNew.y + 0.0f;
      aabb.vcMin.y  = vcPosNew.y - 0.8f;
      aabb.vcCenter = (aabb.vcMax + aabb.vcMin) / 2.0f;

      // if collision occured don't allow movement
      if ( pLevel->TestCollision(&nNS, aabb, &Plane, NULL) ||
           Intersects( (LPGAMECHARACTER*)ppPlayers, nNumP) ||
           Intersects( (LPGAMECHARACTER*)ppBots, nNumB) ) {
         m_pMCEgo->SetPos(vcPosOld);
         }
      else {
         m_pMCEgo->SetPos(vcPosNew);
         m_CS = nNS;
         }
      } // if moved|falling

   UpdateModel(fElapsed);                          
   } // Update 
/*----------------------------------------------------------------*/


