// FILE: CGamePortal.cpp

#include "include/CGamePortal.h"
#include "include/zfx.h"


// constructor
CGamePortal::CGamePortal(void) {
   m_pVerts   = NULL;
   m_pIndis   = NULL;
   m_pBrother = NULL;
   m_pSector  = NULL;
   m_NumVerts = 0;
   m_NumIndis = 0;
   m_bOpen    = true;

   m_vcCenter.Set(0.0f,0.0f,0.0f);
   } // constructor
/*----------------------------------------------------------------*/


// destructor
CGamePortal::~CGamePortal(void) {
   SAFE_DELETE_A(m_pVerts);
   SAFE_DELETE_A(m_pIndis);
   } // destructor
/*----------------------------------------------------------------*/


// renders the portal
void CGamePortal::Render(ZFXVertexCacheManager *pVCM, const ZFXVector &vc) {
   const ZFXCOLOR clr = { 1.0f, 0.0f, 1.0f, 1.0f };
   float Start[3], End[3];

   if (!m_bOpen) return;

   if (IsBackfacingTo(vc)) return;

   pVCM->Render(VID_UU, m_NumVerts, m_NumIndis, 
                m_pVerts, m_pIndis, 0);

   Start[0] = m_vcCenter.x;
   Start[1] = m_vcCenter.y;
   Start[2] = m_vcCenter.z;

   End[0] = Start[0] + m_Plane.m_vcN.x;
   End[1] = Start[1] + m_Plane.m_vcN.y;
   End[2] = Start[2] + m_Plane.m_vcN.z;

   pVCM->RenderLine(Start, End, &clr);
   } // Render
/*----------------------------------------------------------------*/


// builds a frustum from players position through the portal,
// returns false if portal is facing away from camera
bool CGamePortal::BuildFrustum(float,float,ZFXPlane*) {

/*

   // 3.Schritt: Frustrum Planes erzeugen
   #define defn pT->m_nAnz_Planes
   vcKamera = Spieler.vPos;

   // Near Plane
   pT->m_aFrustrums[defn].vN = xxxD3DMathe_Kreuzprodukt(
                                 (xxx3DMathe_V1subV2(vc1, vc0)), 
                                 (xxx3DMathe_V1subV2(vc3, vc0)));
   pT->m_aFrustrums[defn].fD = -xxxD3DMathe_Punktprodukt(
                                 pT->m_aFrustrums[defn].vN, vc0);
   defn++;

   // Far Plane
   pT->m_aFrustrums[defn].vN = g_VFrustrum[VF_F].vN;
   pT->m_aFrustrums[defn].fD = g_VFrustrum[VF_F].fD;
   defn++;

   // Seiten Plane 1:
   pT->m_aFrustrums[defn].vN = xxxD3DMathe_Kreuzprodukt(
                               (xxx3DMathe_V1subV2(vc0, vcKamera)), 
                               (xxx3DMathe_V1subV2(vc1, vcKamera)));
   pT->m_aFrustrums[defn].fD = -xxxD3DMathe_Punktprodukt(
                               pT->m_aFrustrums[defn].vN, vcKamera);
   defn++;
   
   // Seiten Plane 2:
   pT->m_aFrustrums[defn].vN = xxxD3DMathe_Kreuzprodukt(
                               (xxx3DMathe_V1subV2(vc1, vcKamera)), 
                               (xxx3DMathe_V1subV2(vc2, vcKamera)));
   pT->m_aFrustrums[defn].fD = -xxxD3DMathe_Punktprodukt(
                               pT->m_aFrustrums[defn].vN, vcKamera);
   defn++;

   // Seiten Plane 3:
   pT->m_aFrustrums[defn].vN = xxxD3DMathe_Kreuzprodukt(
                               (xxx3DMathe_V1subV2(vc2, vcKamera)), 
                               (xxx3DMathe_V1subV2(vc3, vcKamera)));
   pT->m_aFrustrums[defn].fD = -xxxD3DMathe_Punktprodukt(
                               pT->m_aFrustrums[defn].vN, vcKamera);
   defn++;

   // Seiten Plane 4:
   pT->m_aFrustrums[defn].vN = xxxD3DMathe_Kreuzprodukt(
                               (xxx3DMathe_V1subV2(vc3, vcKamera)), 
                               (xxx3DMathe_V1subV2(vc0, vcKamera)));
   pT->m_aFrustrums[defn].fD = -xxxD3DMathe_Punktprodukt(
                               pT->m_aFrustrums[defn].vN, vcKamera);
   defn++;
*/

   return true;
   } // BuildFrustum
/*----------------------------------------------------------------*/


// loads this portal from a file
bool CGamePortal::Load(FILE *pFile) {
   bool  bDummy=false;
   WORD *pDummy=NULL;

   ZFXVector vc0, vc1, vc2;

   if (!pFile) return false;

   // read counters
   fread(&m_ID,       sizeof(UINT),    1, pFile);
   fread(&m_BID,      sizeof(UINT),    1, pFile);
   fread(&m_NumVerts, sizeof(UINT),    1, pFile);
   fread(&m_NumIndis, sizeof(UINT),    1, pFile);
   fread(&bDummy,     sizeof(bool),    1, pFile);
   fread(&m_Aabb,     sizeof(ZFXAabb), 1, pFile);

   if (m_NumVerts == 0) {
      // critical error
      return false;
      }

   // allocate memory
   m_pVerts = new VERTEX[m_NumVerts];
   m_pIndis = new WORD[m_NumIndis];
   pDummy   = new WORD[m_NumVerts*2];
   if ( !m_pVerts || !m_pIndis || !pDummy) return false;

   // read data
   fread(m_pVerts, sizeof(VERTEX), m_NumVerts,   pFile);
   fread(m_pIndis, sizeof(WORD),   m_NumIndis,   pFile);
   fread(pDummy,   sizeof(WORD),   m_NumVerts*2, pFile);
   delete [] pDummy;

   // construct normal vector
   vc0.Set(m_pVerts[m_pIndis[0]].x, 
           m_pVerts[m_pIndis[0]].y, 
           m_pVerts[m_pIndis[0]].z);

   vc1.Set(m_pVerts[m_pIndis[1]].x, 
           m_pVerts[m_pIndis[1]].y, 
           m_pVerts[m_pIndis[1]].z);

   vc2.Set(m_pVerts[m_pIndis[2]].x, 
           m_pVerts[m_pIndis[2]].y, 
           m_pVerts[m_pIndis[2]].z);

   m_Plane.Set(vc0, vc1, vc2);

   // calculate portal center point
   m_vcCenter = (vc0 + vc1 + vc2) / 3.0f;

   return true;
   } // Load
/*----------------------------------------------------------------*/


// connects both portals as brothers leading to each other
void CGamePortal::SetBrotherRelationship(CGamePortal *pBrother) {
   if (!m_BID == pBrother->GetID() ) return;

   if (m_pBrother != pBrother) {
      m_pBrother = pBrother;
      pBrother->SetBrotherRelationship(this);
      }
   } // SetBrotherRelationship
/*----------------------------------------------------------------*/


// which sector is this portal leading to
CGameSector* CGamePortal::GetTargetSector(void) {
   if (!m_pBrother) return NULL;
   return m_pBrother->GetSector();
   } // GetTargetSector
/*----------------------------------------------------------------*/

// get sector / target sector by index, not by pointer
UINT CGamePortal::GetSectorIndex(void) { 
   if (m_pSector) 
      return m_pSector->GetIndex(); 
   return 0; 
   } // GetSectorIndex
/*----------------------------------------------------------------*/
UINT CGamePortal::GetTargetSectorIndex(void) { 
   if (m_pBrother) 
      return m_pBrother->GetSectorIndex();
   return 0; 
   } // GetTargetSectorIndex
/*----------------------------------------------------------------*/


// test if the portal only shows its back to the given position
bool CGamePortal::IsBackfacingTo(const ZFXVector &vcPos) {
   ZFXVector vcViewDir = (vcPos - m_vcCenter);
   vcViewDir.Normalize();
   return ((m_Plane.m_vcN * vcViewDir) < 0.0f);
   } // IsBackfacingTo
/*----------------------------------------------------------------*/


// test if the given point is on the backside of the portal
bool CGamePortal::IsOnBackside(const ZFXVector &vc) { 
   return (m_Plane.Classify(vc) == ZFXBACK);
   } // IsOnBackside
/*----------------------------------------------------------------*/


// test if portal is intersected by downwards ray
bool CGamePortal::IntersectsDownwardsRay(const ZFXVector &vcOrig,
                                         float fL) {

   // ray is too short
   if ((vcOrig.y-fL) > m_Aabb.vcMax.y) return false;

   // MISC tests
   if (vcOrig.y < m_Aabb.vcMin.y) return false;
   if (vcOrig.x < m_Aabb.vcMin.x) return false;
   if (vcOrig.x > m_Aabb.vcMax.x) return false;
   if (vcOrig.z < m_Aabb.vcMin.z) return false;
   if (vcOrig.z > m_Aabb.vcMax.z) return false;
   return true;
   } // IntersectsDownwardsRay
/*----------------------------------------------------------------*/


// test if the portal only shows its back to the given position
void CGamePortal::ChopFrustum(ZFXPlane *pFrustum) {

   } // ChopFrustum
/*----------------------------------------------------------------*/

