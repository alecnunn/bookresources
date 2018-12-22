// FILE: CGameCharacter.cpp


#include "include/CGameCharacter.h"

const float FREEZE_TIME = 5.0f;


CGameCharacter::CGameCharacter(void) {
   m_pModel     = NULL;
   m_pShadow    = NULL;
   m_pMCEgo     = NULL;
   m_pRDevice   = NULL;
   m_pNDevice   = NULL;
   m_bIsBot     = false;
   m_VSH        = 0;
   m_PSH        = 0;
   m_ID         = 0;
   m_bMoved     = false;
   m_bIsFalling = false;
   m_bIdle      = true;
   m_CS         = 0;
   m_nEnergy    = 100;
   m_fFrozen    = 0.0f;
   m_fTouchDown = 0.0f;
   } // constructor
/*----------------------------------------------------------------*/


CGameCharacter::~CGameCharacter(void) {
   SAFE_DELETE(m_pModel);
   SAFE_DELETE(m_pShadow);
   SAFE_DELETE(m_pMCEgo);
   } // destructor
/*----------------------------------------------------------------*/


void CGameCharacter::InitShadowVolume(void) {
   CVERTEX *pVerts = NULL;
   WORD    *pIndis = NULL;

   UINT NumV = m_pModel->GetVertices(&pVerts);
   UINT NumI = m_pModel->GetIndices(&pIndis);
   
   m_pShadow = new CShadowVolume();
   
   m_pShadow->Init((void*)pVerts, NumV, pIndis, 
                   NumI, sizeof(CVERTEX),
                   m_pModel->GetAabb(), m_pRDevice);
   } // InitShadowVolume
/*----------------------------------------------------------------*/
  

void CGameCharacter::UpdateModel(float fElapsed) {
   CVERTEX *pVerts = NULL;
   WORD    *pIndis = NULL;

   // skin the mesh based on elapsed time
   m_pModel->Update(fElapsed);
   
   // get skinned mesh data
   m_pModel->GetVertices(&pVerts);
   m_pModel->GetIndices(&pIndis);
   
   // update geometry for shadow volume
   m_pShadow->UpdateGeometry((void*)pVerts, pIndis,
                             m_pModel->GetAabb());
   } // UpdateModel
/*----------------------------------------------------------------*/
   
   
HRESULT CGameCharacter::Render(void) {
   ZFXMatrix mat;
   HRESULT   hr=ZFX_OK;

   ZFXVector vcRot = m_pMCEgo->GetRotation();

   mat.Identity();
   mat.RotaY( -vcRot.y );
   mat.SetTranslation(m_pMCEgo->GetPos());
   mat._42 += m_fTouchDown;
   m_pRDevice->SetWorldTransform(&mat);

   // if we can use shaders then do it
   if (m_pRDevice->CanDoShaders() ) {
      m_pRDevice->UseShaders(true);
      m_pRDevice->ActivateVShader(m_VSH, VID_CA);
      m_pRDevice->ActivatePShader(m_PSH);
      }
   
   hr = m_pModel->Render();

   m_pRDevice->SetWorldTransform(NULL);
   return hr;
   } // Render
/*----------------------------------------------------------------*/


HRESULT CGameCharacter::RenderShadow(CGameLight *pL) {
	return m_pShadow->Render(pL);
   } // Render
/*----------------------------------------------------------------*/


UPDATEPKG CGameCharacter::GetUpdate(void) {
   UPDATEPKG pkg;

   pkg.NID   = m_ID;
   pkg.vcPos = m_pMCEgo->GetPos();
   pkg.fRotY = m_pMCEgo->GetRotation().y;

   return pkg;
   } // GetUpdate
/*----------------------------------------------------------------*/


void CGameCharacter::SetUpdate(UPDATEPKG *pPkg) {
   // is this really one meant for us
   if (m_ID != pPkg->NID) return;

   ZFXVector vcOld = m_pMCEgo->GetRotation();

   m_pMCEgo->SetPos( pPkg->vcPos );
   m_pMCEgo->SetRotation( vcOld.x,
                          pPkg->fRotY,
                          vcOld.z );

   // now player is active
   m_bIdle = false;
   } // SetUpdate
/*----------------------------------------------------------------*/


// apply damage to this character instance
void CGameCharacter::ApplyDamage(UINT nDamage, UINT OwnerID) {

   if (m_nEnergy >= nDamage) 
      m_nEnergy -= nDamage;
   else {
      m_nEnergy = 0;
      m_fFrozen = FREEZE_TIME;

      SendMsg_PlayerFrozen(OwnerID);
      }
   } // ApplyDamage
/*----------------------------------------------------------------*/


// test for collision with line segment
bool CGameCharacter::TestCollision(const ZFXRay &_Ray, float fL, 
                                   float *pfD) {
   UINT     NumVerts=0;
   UINT     NumIndis=0;
   CVERTEX *pVerts=NULL;
   WORD    *pIndis=NULL;

   // transform world ray to model space
   ZFXRay Ray = _Ray;
   ZFXVector vcPos = m_pMCEgo->GetPos();
   vcPos.y -= 1.5f;
   Ray.m_vcOrig -= vcPos;

   ZFXVector vc0(0,0,0), vc1(0,0,0), vc2(0,0,0);

   ZFXAabb aabb = m_pModel->GetAabb();

   if ( aabb.Intersects(Ray, fL, pfD) ||
        aabb.Contains(Ray, fL) ) {

      NumVerts = m_pModel->GetVertices(&pVerts);
      NumIndis = m_pModel->GetIndices(&pIndis);
/*
      for (UINT i=0; i<NumIndis; i+=3) {

         vc0.Set(pVerts[i].x, pVerts[i].y, pVerts[i].z);
         vc1.Set(pVerts[i+1].x, pVerts[i+1].y, pVerts[i+2].z);
         vc2.Set(pVerts[i+2].x, pVerts[i+1].y, pVerts[i+2].z);

         if ( Ray.Intersects(vc0, vc1, vc2, true, pfD) )
            return true;
         }
*/
return true;

      return false;
      }

   return false;
   } // TestCollision 
/*----------------------------------------------------------------*/


/**
 * Send message to the network about touch and use event.
 */
HRESULT CGameCharacter::SendMsg_TouchAndUse(void) {
   ZFXPACKAGE msgPkg;

   ZFXVector vcPos = m_pMCEgo->GetPos();

   msgPkg.nLength   = sizeof(ZFXVector);
   msgPkg.nType     = MSG_TOUCHANDUSE;
   msgPkg.nSender   = m_ID;
   msgPkg.pData     = &vcPos;

   return m_pNDevice->SendToServer(&msgPkg);
   } // SendMsg_TouchAndUse
/*----------------------------------------------------------------*/


/**
 * Send message to the network about firing event.
 */
HRESULT CGameCharacter::SendMsg_FireBullet(void) {
   ZFXPACKAGE msgPkg;
   BULLET     Bullet;

   ZFXVector vcPos = m_pMCEgo->GetPos();
   ZFXVector vcDir = m_pMCEgo->GetDir();

   // randomize position a bit
   vcPos.x += (float(  (rand()%11) - 5 )) / 500.0f;
   vcPos.y += (float(  (rand()%11) - 5 )) / 500.0f;
   vcPos.z += (float(  (rand()%11) - 5 )) / 500.0f;

   // randomize direction
   vcDir.x += (float(  (rand()%11) - 5 )) / 400.0f;
   vcDir.y += (float(  (rand()%11) - 5 )) / 400.0f;
   vcDir.z += (float(  (rand()%11) - 5 )) / 400.0f;

   vcDir.Normalize();

   Bullet.ID        = m_ID;
   Bullet.bIsBot    = m_bIsBot;
   Bullet.SectorID  = m_CS;
   Bullet.vcPos     = vcPos;
   Bullet.vcPos.y  -= 0.5f;
   Bullet.vcDir     = vcDir;
   Bullet.fLifeSpan = 5.0f;
   Bullet.fSpeed    = 10.0f;
   Bullet.nDamage   = 10;

   msgPkg.nLength   = sizeof(BULLET);
   msgPkg.nType     = MSG_FIREBULLET;
   msgPkg.nSender   = m_ID;
   msgPkg.pData     = &Bullet;

   return m_pNDevice->SendToServer(&msgPkg);
   } // SendMsg_TouchAndUse
/*----------------------------------------------------------------*/


/**
 * Send message to the network about freezing of this player.
 */
HRESULT CGameCharacter::SendMsg_PlayerFrozen(UINT nBy) {
   ZFXPACKAGE msgPkg;

   msgPkg.nLength   = sizeof(UINT);
   msgPkg.nType     = MSG_PLAYERFROZEN;
   msgPkg.nSender   = m_ID;
   msgPkg.pData     = &nBy;

   return m_pNDevice->SendToServer(&msgPkg);
   } // SendMsg_PlayerFrozen
/*----------------------------------------------------------------*/


/**
 * Check for intersection with a list of players.
 */
bool CGameCharacter::Intersects(LPGAMECHARACTER *ppCharacters, UINT nNum) {
   ZFXAabb aabb;

   ZFXVector vcPos = m_pMCEgo->GetPos();
   vcPos.y -= 1.5f;

   for (UINT i=0; i<nNum; i++) {
      // dont test against ourselves 
      if (ppCharacters[i] == this) continue;

      aabb = ppCharacters[i]->GetAabb();

      if ( GetAabb().Intersects(aabb) ) 
         return true;
      }

   return false;
   } // Intersects
/*----------------------------------------------------------------*/


/**
 * Returns the characters current aabb in world space
 */
ZFXAabb CGameCharacter::GetAabb(void) {
   ZFXAabb aabb = m_pModel->GetAabb();

   ZFXVector vcPos = m_pMCEgo->GetPos();
   vcPos.y -= 1.5f;

   aabb.vcCenter += vcPos;
   aabb.vcMax    += vcPos;
   aabb.vcMin    += vcPos;

   return aabb;
   } // GetAabb
/*----------------------------------------------------------------*/

