// File: CPortal.cpp:

#include "include/CPortal.h"

CPortal::CPortal(void) {
   Init();
   } // constructor
/*----------------------------------------------------------------*/

CPortal::~CPortal(void) {
   Release();
   } // destructor
/*----------------------------------------------------------------*/


// initialize to defaults
void CPortal::Init(void) {
   m_NumVerts    = 0;
   m_NumIndis    = 0;
   m_NumPolys    = 1;
   m_ID          = 0;
   m_BID         = 0;
   m_bSelected   = false;
   m_bHidden     = false;
   m_bVisible    = true;
   m_pVerts      = NULL;
   m_pIndis      = NULL;
   m_pLineIndis  = NULL;
   m_pDad        = NULL;
   m_pBrother    = NULL;
   m_Type        = LOB_PORTAL;
   memset(&m_Aabb, 0, sizeof(ZFXAabb));
   } // Init
/*----------------------------------------------------------------*/


// clean up all memory and stuff
void CPortal::Release(void) {
   SAFE_DELETE_A(m_pVerts);
   SAFE_DELETE_A(m_pIndis);
   SAFE_DELETE_A(m_pLineIndis);
   } // Release
/*----------------------------------------------------------------*/


// set this to a copy of another portal
void CPortal::CopyOf(CLevelObject *pLob) {
   
   if ( !pLob->IsOfType(m_Type) ) return;

   LPPORTAL pPort = (LPPORTAL)pLob;

   Reset();

   // vertex and index lists
   SetGeometry(pPort->m_pVerts, pPort->m_pIndis, 
               pPort->m_Aabb);
   
   // attributes
   m_pDad     = pPort->m_pDad;
   m_pBrother = pPort->m_pBrother;
   m_ID       = pPort->m_ID;
   m_BID      = pPort->m_BID;
   m_bVisible = pPort->m_bVisible;
   } // CopyOf
/*----------------------------------------------------------------*/


// set the target, which is another portal this one is leading to
void CPortal::SetItsTarget(CPortal *pP) {
   // prevent infinite loops
   if (pP->GetID() == m_ID) return;

   if (pP) { 
      m_pBrother = pP;
      m_BID = m_pBrother->GetID(); 
      }
   else { 
      m_pBrother = NULL;
      m_BID = 0; 
      }
   } // SetItsTarget
/*----------------------------------------------------------------*/


// set portal vertex- and indexlist, must contain exactly 1 rectangle
void CPortal::SetGeometry(const VERTEX *pVerts, const WORD *pIndis,
                          const ZFXAabb &aabb) {
   Reset();
   
   m_NumVerts = 4;
   m_NumIndis = 6;

   m_pVerts = new VERTEX[m_NumVerts];
   m_pIndis = new WORD[m_NumIndis];
   m_pLineIndis = new WORD[m_NumVerts*2];
   
   if (!m_pVerts || !m_pIndis || !m_pLineIndis) return;

   memcpy(m_pVerts, pVerts, sizeof(VERTEX)*4);
   memcpy(m_pIndis, pIndis, sizeof(WORD)*6);

   for (UINT i=0; i<m_NumVerts; i++) {
      m_pLineIndis[(i*2)] = i;
      m_pLineIndis[(i*2)+1] = i+1;
      if (i==m_NumVerts-1) m_pLineIndis[(i*2)+1] = 0;
      }

   memcpy(&m_Aabb, &aabb, sizeof(ZFXAabb));
   } // SetGeometry
/*----------------------------------------------------------------*/


// Was the portal hit by the ray in world coordinates?
bool CPortal::Picked(const ZFXRay &Ray, float *pfD) {
   ZFXVector vc0, vc1, vc2;
   WORD      I0, I1, I2;

   // loop through the portals's tris
   for (UINT i=0; i<m_NumIndis; i+=3) {

      I0 = m_pIndis[i];
      I1 = m_pIndis[i+1];
      I2 = m_pIndis[i+2];

      vc0.Set( m_pVerts[I0].x, m_pVerts[I0].y, m_pVerts[I0].z );
      vc1.Set( m_pVerts[I1].x, m_pVerts[I1].y, m_pVerts[I1].z );
      vc2.Set( m_pVerts[I2].x, m_pVerts[I2].y, m_pVerts[I2].z );

      // intersect ray with triangle (cast away const)
      if ( ((ZFXRay)Ray).Intersects(vc0, vc1, vc2, true, pfD) )
         return true;
      }
   return false;
   } // Picked
/*----------------------------------------------------------------*/


// Does this portal intersect the world coordinates aabb?
bool CPortal::Intersects(const ZFXAabb &aabb, ZFXAXIS axis) {
   // first check if portal's aabb does at least 
   if (!m_Aabb.Intersects(aabb)) return false;

   // if any vertex intersects the portal really intersects
   for (UINT i=0; i<m_NumVerts; i++) {
      // simplify to 2D problem
      if (axis == X_AXIS) {
         if ((m_pVerts[i].y < aabb.vcMax.y) &&
             (m_pVerts[i].y > aabb.vcMin.y) &&
             (m_pVerts[i].z < aabb.vcMax.z) &&
             (m_pVerts[i].z > aabb.vcMin.z))
            return true;
         }
      else if (axis == Y_AXIS) {
         if ((m_pVerts[i].x < aabb.vcMax.x) &&
             (m_pVerts[i].x > aabb.vcMin.x) &&
             (m_pVerts[i].z < aabb.vcMax.z) &&
             (m_pVerts[i].z > aabb.vcMin.z))
            return true;
         }
      else if (axis == Z_AXIS) {
         if ((m_pVerts[i].y < aabb.vcMax.y) &&
             (m_pVerts[i].y > aabb.vcMin.y) &&
             (m_pVerts[i].x < aabb.vcMax.x) &&
             (m_pVerts[i].x > aabb.vcMin.x))
            return true;
         }
      } // for
   // no just its aabb
   return false;
   } // Intersects
/*----------------------------------------------------------------*/


// save this portal to a file
void CPortal::Save(FILE *pFile) {
   if (!pFile) return;

   // write counters
   fwrite(&m_ID,       sizeof(UINT),    1, pFile);
   fwrite(&m_BID,      sizeof(UINT),    1, pFile);
   fwrite(&m_NumVerts, sizeof(UINT),    1, pFile);
   fwrite(&m_NumIndis, sizeof(UINT),    1, pFile);
   fwrite(&m_bVisible, sizeof(bool),    1, pFile);
   fwrite(&m_Aabb,     sizeof(ZFXAabb), 1, pFile);

   // write data
   fwrite(m_pVerts,     sizeof(VERTEX), m_NumVerts,   pFile);
   fwrite(m_pIndis,     sizeof(WORD),     m_NumIndis,   pFile);
   fwrite(m_pLineIndis, sizeof(WORD),     m_NumVerts*2, pFile);
   } // Save
/*----------------------------------------------------------------*/


// load this portal from a file
bool CPortal::Load(FILE *pFile) {
   if (!pFile) return false;

   // read counters
   fread(&m_ID,       sizeof(UINT),    1, pFile);
   fread(&m_BID,      sizeof(UINT),    1, pFile);
   fread(&m_NumVerts, sizeof(UINT),    1, pFile);
   fread(&m_NumIndis, sizeof(UINT),    1, pFile);
   fread(&m_bVisible, sizeof(bool),    1, pFile);
   fread(&m_Aabb,     sizeof(ZFXAabb), 1, pFile);

   if (m_NumVerts == 0) {
      // critical error
      return false;
      }

   // allocate memory
   m_pVerts     = new VERTEX[m_NumVerts];
   m_pIndis     = new WORD[m_NumIndis];
   m_pLineIndis = new WORD[m_NumVerts*2];
   if ( !m_pVerts || !m_pIndis || !m_pLineIndis) return false;

   // read data
   fread(m_pVerts,     sizeof(VERTEX), m_NumVerts,   pFile);
   fread(m_pIndis,     sizeof(WORD),   m_NumIndis,   pFile);
   fread(m_pLineIndis, sizeof(WORD),   m_NumVerts*2, pFile);
   return true;
   } // Load
/*----------------------------------------------------------------*/


// renders the portal using the vc manager
void CPortal::Render(ZFXVertexCacheManager *pVCM, bool bHide) {
   if (m_bHidden && bHide) return;
 
   bool bAsLine = (pVCM->GetShadeMode() == RS_SHADE_LINES);

   if (bAsLine) {
      pVCM->Render(VID_UU, m_NumVerts, m_NumVerts*2, 
                   m_pVerts, m_pLineIndis, 0);
      }
   else {
      pVCM->Render(VID_UU, m_NumVerts, m_NumIndis, 
                   m_pVerts, m_pIndis, 0);
      }
   } // Render
/*----------------------------------------------------------------*/

