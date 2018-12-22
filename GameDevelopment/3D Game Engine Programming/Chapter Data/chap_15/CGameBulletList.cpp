// FILE: CGameBulletList.cpp


#include "include/CGameBulletList.h"


const float BULLET_SIZE = 0.03f;

/**
 *
 */
CGameBulletList::CGameBulletList(LPZFXRENDERDEVICE pDevice,
                                 LPZFXAUDIODEVICE  pADevice) {
   m_pDevice  = pDevice;
   m_pADevice = pADevice;
   m_pBullets = NULL;
   m_Num      = 0;
   m_NumMax   = 100;
   m_SkinID   = 0;

   m_iAudio_Gun = 0;

   ZFXCOLOR clr = { 1.0f, 1.0f, 1.0f, 1.0f };
   ZFXCOLOR clrAlpha = { 1.0f, 0.0f, 1.0f, 0.0f };

   m_pBullets = (BULLET*)malloc(sizeof(BULLET)*m_NumMax);

   memset(m_vQuad,  0, sizeof(VERTEX)*4);
   memset(m_vcQuad, 0, sizeof(ZFXVector)*4);

   m_pDevice->GetSkinManager()->AddSkin(&clr, &clr, &clr,
                                        &clr, 1.0f, 
                                        &m_SkinID);

   m_pDevice->GetSkinManager()->AddTexture(m_SkinID, 
                            "data/textures/icycle.bmp",
                            true, 1.0f, &clrAlpha, 1);

   // base vertices
   m_vcQuad[0].x = -BULLET_SIZE;
   m_vcQuad[0].y =  BULLET_SIZE;
   m_vcQuad[0].z =  0.0f;

   m_vcQuad[1].x =  BULLET_SIZE;
   m_vcQuad[1].y = -BULLET_SIZE;
   m_vcQuad[1].z =  0.0f;

   m_vcQuad[2].x = -BULLET_SIZE;
   m_vcQuad[2].y = -BULLET_SIZE;
   m_vcQuad[2].z =  0.0f;

   m_vcQuad[3].x =  BULLET_SIZE;
   m_vcQuad[3].y =  BULLET_SIZE;
   m_vcQuad[3].z =  0.0f;

   // texture coordinates
   m_vQuad[0].tu = 0.0f;
   m_vQuad[0].tv = 0.0f;
   m_vQuad[1].tu = 1.0f;
   m_vQuad[1].tv = 1.0f;
   m_vQuad[2].tu = 0.0f;
   m_vQuad[2].tv = 1.0f;
   m_vQuad[3].tu = 1.0f;
   m_vQuad[3].tv = 0.0f;

   // load weapon sound
   m_pADevice->LoadSound( "data\\sounds\\gun.wav", &m_iAudio_Gun );
   } // constructor
/*----------------------------------------------------------------*/


/**
 *
 */
CGameBulletList::~CGameBulletList(void) {
   if (m_pBullets) {
      free(m_pBullets);
      m_pBullets = NULL;
      }

   SAFE_FREE(m_pBullets);
   } // destructor
/*----------------------------------------------------------------*/


/**
 *
 */
HRESULT CGameBulletList::AddBullet(const BULLET &Bullet) {

   // more memory needed?
   if (m_Num >= m_NumMax) {
      m_NumMax += 100;
      m_pBullets = (BULLET*)realloc(m_pBullets,
                    sizeof(BULLET)*m_NumMax);
      if (!m_pBullets) return ZFX_FAIL;
      }

   // copy new bullet data to list
   memcpy(&m_pBullets[m_Num], &Bullet, sizeof(BULLET));

   // play weapon sound
   m_pADevice->PlaySound( m_iAudio_Gun, false );

   m_Num++;
   return ZFX_OK;
   } // AddBullet
/*----------------------------------------------------------------*/


/**
 *
 */
HRESULT CGameBulletList::Render(ZFXVector vcDir) {
   static ZFXCOLOR clr = {1.0f, 0.0f, 0.0f, 1.0f};

   if (m_Num <= 0) return ZFX_OK;

   WORD   Indis[6] = { 0, 1, 2, 0, 3, 1 };

   ZFXVector vc[4];
   ZFXMatrix mat;

   for (UINT i=0; i<m_Num; i++) {

      mat.Billboard(m_pBullets[i].vcPos, vcDir);

      vc[0] = mat * m_vcQuad[0];
      vc[1] = mat * m_vcQuad[1];
      vc[2] = mat * m_vcQuad[2];
      vc[3] = mat * m_vcQuad[3];

      memcpy(&m_vQuad[0], &vc[0], sizeof(float)*4);
      memcpy(&m_vQuad[1], &vc[1], sizeof(float)*4);
      memcpy(&m_vQuad[2], &vc[2], sizeof(float)*4);
      memcpy(&m_vQuad[3], &vc[3], sizeof(float)*4);

      m_pDevice->GetVertexManager()->Render(VID_UU, 4, 6, 
                                            m_vQuad, Indis,
                                            m_SkinID);
      }

   return ZFX_OK;
   } // Render
/*----------------------------------------------------------------*/


/**
 *
 */
void CGameBulletList::Update(CGameLevel *pLevel, 
                             LPGAMECHARACTER *pCharacters,
                             UINT NumC, 
                             LPGAMECHARACTER *pBots,
                             UINT NumB, float fElapsed) {
   if (m_Num <= 0) return;
   
   BULLET *pBull=NULL;
   ZFXRay  Ray;
   float   fDL=0.0f, // level hit distance
           fDD=0.0f, // hit dummy distance
           fDC=0.0f, // player hit distance
           fL=0.0f;
   bool    bLevelCol=false,
           bCharCol=false;

   LPGAMECHARACTER pHitOne=NULL;
   LPGAMECHARACTER pObj=NULL;

   // move particles
   for (UINT i=0; i<m_Num; i++) {
      pBull    = &m_pBullets[i];
      bCharCol = false;

      // traversed length this frame
      fL = pBull->fSpeed*fElapsed;

      // reduce remaining life time
      pBull->fLifeSpan -= fElapsed;

      if (pBull->fLifeSpan <= 0) { RemoveBullet(i); i--; }

      // build ray for this frame
      Ray.Set(pBull->vcPos, pBull->vcDir);

      // check for collision with level geometry
      bLevelCol = pLevel->TestCollision(
                            &pBull->SectorID, Ray, 
                            fL, &fDL, NULL);

      // check for collision with characters
      for (UINT j=0; j<(NumC+NumB); j++) {

         // test player characters 
         if (j<NumC) {
            pObj = pCharacters[j];
            // make sure you dont hit yourself
            if (pBull->ID == pObj->GetID())
               continue;
            }
         // test npcs
         else pObj = pBots[j-NumC];

         if ( pObj->TestCollision(Ray, fL, &fDD) ) {
            // gotcha
            bCharCol = true;
            
            // save nearest hit object
            if ( fDC==0.0f || fDD<fDC ) {
               fDC = fDD;
               pHitOne = pObj;
               }
            }
         } // for [characters]

      // CALCULATE FINAL RESULTS FOR COLLISION
      // we hit both, decide whos closer
      if (bCharCol && bLevelCol) {
         if (fDL < fDC) bCharCol = false;
         else bLevelCol = false;
         }

      // if we still hit a character
      if (bCharCol) 
         pHitOne->ApplyDamage(pBull->nDamage, pBull->ID);

      // if collided then remove bullet
      if (bLevelCol || bCharCol) {
         RemoveBullet(i); 
         i--; 
         }
      // else update position
      else {
         pBull->vcPos += pBull->vcDir  *
                         pBull->fSpeed *
                         fElapsed;
         }
      }
   } // Update
/*----------------------------------------------------------------*/


/**
 *
 */
void CGameBulletList::RemoveBullet(UINT i) {

   if (i >= m_Num) return;

   // copy last one to this position
   // and reduce total list count
   m_pBullets[i] = m_pBullets[m_Num-1];
   m_Num--;
   } // Update
/*----------------------------------------------------------------*/