// FILE: CGameEntity.cpp

#include "include/CGameEntity.h"
#include "include/zfx.h"


// constructor
CGameEntity::CGameEntity(void) {
   m_Type     = ENT_UNKNOWN;
   m_NumVerts = 0;
   m_NumIndis = 0;
   m_nSkin    = 0;
   m_pVerts   = NULL;
   m_pIndis   = NULL;
   } // constructor
/*----------------------------------------------------------------*/
 

// destructor
CGameEntity::~CGameEntity(void) {
   SAFE_FREE(m_pVerts);
   SAFE_FREE(m_pIndis);
   } // destructor
/*----------------------------------------------------------------*/


// loads this entity from a file
bool CGameEntity::Load(FILE *pFile) {
   UINT nSize=0;

   if (!pFile) return false;

   fread(&m_Aabb, sizeof(ZFXAabb),   1, pFile);
   fread(&nSize,  sizeof(UINT),      1, pFile);

   return true;
   } // Load
/*----------------------------------------------------------------*/


// **********************************
// * CGameDoor class implementation *
// **********************************

// constructor
CGameDoor::CGameDoor(void) {
   m_Type      = ENT_DOOR;
   m_bActive   = false;
   m_bOpening  = false;
   m_bPausing  = false;
   m_fSign     = 1.0f;
   m_fTime     = 0.0f;
   m_fDist     = 0.0f;
   m_fPause    = 0.0f;
   m_Axis      = X_AXIS;
   m_Portal[0] = 0;
   m_Portal[1] = 0;
   m_vcT.Set(0.0f,0.0f,0.0f);
   m_pPortal_A = NULL;
   m_pPortal_B = NULL;
   } // constructor
/*----------------------------------------------------------------*/
 

// destructor
CGameDoor::~CGameDoor(void) {

   } // destructor
/*----------------------------------------------------------------*/


// loads this door entity from a file
bool CGameDoor::Load(FILE *pFile) {
   UINT nSize=0;

   // load base class values
   if ( !CGameEntity::Load(pFile) ) return false;

   // load door specific stuff
   fread(&m_fDist,  sizeof(float),   1, pFile);
   fread(&m_fPause, sizeof(float),   1, pFile);
   fread(&m_Axis,   sizeof(ZFXAXIS), 1, pFile);
   fread(m_Portal,  sizeof(UINT),    2, pFile);

   if (m_fDist < 0.0f) {
      m_fSign = -1.0f;
      m_fDist *= -1.0f;
      }
   else m_fSign = 1.0f;

   m_Type = ENT_DOOR;

   return LoadMesh(pFile);
   } // Load
/*----------------------------------------------------------------*/


// render the mesh object for this door 
HRESULT CGameDoor::Render(ZFXRenderDevice *pDevice) {
   HRESULT hr=ZFX_OK;

   if (m_bActive) {
      ZFXMatrix mat;
      mat.Identity();
      mat.Translate(m_vcT.x,m_vcT.y,m_vcT.z);
      pDevice->SetWorldTransform(&mat);
      }

   hr = pDevice->GetVertexManager()->Render(
                              VID_UU, m_NumVerts,
                              m_NumIndis, m_pVerts,
                              m_pIndis, m_nSkin);

   if (m_bActive) {
      pDevice->SetWorldTransform(NULL);
      }

   return hr;
   } // Render
/*----------------------------------------------------------------*/


// check if the door is near the player
bool CGameDoor::TouchAndUse(const ZFXVector &vcPos) {
   ZFXRay Ray;

   ZFXVector vcDir = m_Aabb.vcCenter - vcPos;
   vcDir.Normalize();

   Ray.Set(vcPos, vcDir );

   if ( !Ray.Intersects(m_Aabb, 1.8f, NULL) )
      return false;

   m_fTime    = 0.0f;
   m_bActive  = true;
   m_bOpening = true;

   if (m_pPortal_A) m_pPortal_A->SetVisibility(true);
   if (m_pPortal_B) m_pPortal_B->SetVisibility(true);

   return true;
   } // TouchAndUse
/*----------------------------------------------------------------*/


// render the mesh object for this door 
void CGameDoor::Update(float fET) {
   // update position if moving
   if (m_bActive) {
      // opening sequence
      if (m_bOpening) {
         m_fTime += fET;

         // end of opening sequence
         if (m_fTime > m_fDist) {
            m_bOpening = false;
            m_bPausing = true;
            m_fTime = m_fDist;
            }
         }
      // pausing sequence
      else if (m_bPausing) {
         m_fTime += fET;

         // end of pausing sequence
         if (m_fTime > (m_fDist+m_fPause)) {
            m_bPausing = false;
            m_fTime = m_fDist;
            }
         }
      // closing sequence
      else {
         m_fTime -= fET;

         // end of closing sequence
         if (m_fTime < 0.0f) {
            m_vcT.Set(0.0f,0.0f,0.0f);
            m_bActive = false;
            if (m_pPortal_A) m_pPortal_A->SetVisibility(false);
            if (m_pPortal_B) m_pPortal_B->SetVisibility(false);
            }
         }

      // if not completely closed
      if (m_bActive && !m_bPausing) {
         if (m_Axis == X_AXIS) 
            m_vcT.Set(m_fTime*m_fSign,0.0f,0.0f);
         else if (m_Axis == Y_AXIS) 
            m_vcT.Set(0.0f,m_fTime*m_fSign,0.0f);
         else if (m_Axis == Z_AXIS) 
            m_vcT.Set(0.0f,0.0f,m_fTime*m_fSign);
         }
      }
   } // Update
/*----------------------------------------------------------------*/


// load mesh data from the file
bool CGameDoor::LoadMesh(FILE *pFile) {
   UINT    NumPolys=0;
   ZFXAabb aabb;
   UINT    NumVerts=0; 
   UINT    NumIndis=0;
   bool    dBOOL = false;
   float   dFLOAT[2] = {0.0f,0.0f};
   WORD   *pdWORD=NULL;

   fread(&NumPolys, sizeof(UINT),    1, pFile);
   fread(&aabb,     sizeof(ZFXAabb), 1, pFile);

   // suck in all polys in this mesh
   for (UINT i=0; i<NumPolys; i++) {

      // polygon header
      fread(&NumVerts, sizeof(UINT),    1, pFile);
      fread(&NumIndis, sizeof(UINT),    1, pFile);
      fread(&m_nSkin,  sizeof(UINT),    1, pFile);
      fread(&aabb,     sizeof(ZFXAabb), 1, pFile);
      fread(&dBOOL,    sizeof(bool),    1, pFile);
      fread(dFLOAT,    sizeof(float),   2, pFile);
      fread(dFLOAT,    sizeof(float),   2, pFile);

      m_pVerts = (VERTEX*)realloc(m_pVerts, sizeof(VERTEX)
                                    * (m_NumVerts+NumVerts));
      m_pIndis = (WORD*)realloc(m_pIndis, sizeof(WORD)
                                * (m_NumIndis+NumIndis));
      pdWORD = new WORD[NumVerts*2];

      // polygon data
      fread(&m_pVerts[m_NumVerts], sizeof(VERTEX), NumVerts, pFile);
      fread(&m_pIndis[m_NumIndis], sizeof(WORD),   NumIndis, pFile);
      fread(pdWORD, sizeof(WORD),  NumVerts*2, pFile);

      for (UINT i=0; i<NumIndis; i++) 
         m_pIndis[m_NumIndis+i] += m_NumVerts;

      SAFE_DELETE_A(pdWORD);
      m_NumVerts += NumVerts;
      m_NumIndis += NumIndis;
      }

   return true;
   } // LoadMesh
/*----------------------------------------------------------------*/


// connect this entity to its portals if it uses any
bool CGameDoor::ConnectToPortals(CGameLevel *pLevel) {
   if (!pLevel) return false;

   if (m_Portal[0] != 0) {
      m_pPortal_A = pLevel->GetPortal(m_Portal[0]);
      if (!m_pPortal_A) return false;
      else m_pPortal_A->SetVisibility(false);
      }

   if (m_Portal[1] != 0) {
      m_pPortal_B = pLevel->GetPortal(m_Portal[1]);
      if (!m_pPortal_B) return false;
      else m_pPortal_B->SetVisibility(false);
      }
   return true;
   } // ConnectToPortals
/*----------------------------------------------------------------*/


// test if given aabb is colliding with this door
bool CGameDoor::TestCollision(const ZFXAabb &aabb, ZFXPlane *pP) {
   ZFXAabb _aabb = aabb;

   // translate given aabb inverse to account
   // for entity translation if there is any
   if (m_bActive) {
      _aabb.vcCenter -= m_vcT;
      _aabb.vcMax    -= m_vcT;
      _aabb.vcMin    -= m_vcT;
      }

   if ( _aabb.Intersects(m_Aabb) ) {

/*
      // TO BE MORE ACCURATE IMPLEMENT THIS
      for (UINT i=0; i<m_NumIndis; i+=3) {
         if (ptAabb->Intersects( TRIANLGE ))
            return true;
         }
      return false;
*/
      return true;
      }
   else return false;
   } // TestCollision
/*----------------------------------------------------------------*/


// test if given line segment is colliding with this door
bool CGameDoor::TestCollision(const ZFXRay &_Ray, float fL, float *pfD) {
   ZFXRay Ray;
   
   Ray.Set( _Ray.m_vcOrig, _Ray.m_vcDir );

   // translate given ray inverse to account
   // for entity translation if there is any
   if (m_bActive) Ray.m_vcOrig = _Ray.m_vcOrig - m_vcT;

   if ( m_Aabb.Intersects(Ray, fL, pfD) ||
        m_Aabb.Contains(Ray, fL) ) {

/*
      // TO BE MORE ACCURATE IMPLEMENT THIS
      for (UINT i=0; i<m_NumIndis; i+=3) {
         if (Ray.Intersects( TRIANLGE ))
            return true;
         }
      return false;
*/
      return true;
      }
   else return false;
   } // TestCollision
/*----------------------------------------------------------------*/