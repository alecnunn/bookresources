// FILE: CGameLight.cpp

#include "include/CGameLight.h"
#include "include/zfx.h"


// constructor
CGameLight::CGameLight(void) {
   m_vcPos    = ZFXVector(0.0f, 0.0f, 0.0f);
   m_Clr.fR   = 1.0f;
   m_Clr.fG   = 1.0f;
   m_Clr.fB   = 1.0f;
   m_Clr.fA   = 1.0f;
   m_fR       = 0.0f;
   m_fR2      = 0.0f;
   m_bFlicker = false;
   m_bShadow  = false;
   m_NumSkins = 0;

   m_Aabb.vcMax = ZFXVector(-99999.9f,-99999.9f,-99999.9f);
   m_Aabb.vcMin = ZFXVector(99999.9f, 99999.9f, 99999.9f);

   m_pCache   = NULL;
   } // constructor
/*----------------------------------------------------------------*/
 

// destructor
CGameLight::~CGameLight(void) {
   for (UINT i=0; i<m_NumSkins; i++)
      SAFE_FREE(m_pCache[i].pIndis);

   SAFE_DELETE_A(m_pCache);
   } // destructor
/*----------------------------------------------------------------*/


// loads this light from a file
bool CGameLight::Load(FILE *pFile, UINT NumSkins) {
   ZFXAabb aabb;
   UINT    NumPolys=0;

   m_NumSkins = NumSkins;

   m_pCache = new LIGHTCACHE[NumSkins];
   memset(m_pCache, 0, sizeof(LIGHTCACHE)*NumSkins);

   if (!pFile) return false;

   fread(&m_vcPos,    sizeof(ZFXVector), 1, pFile);
   fread(&aabb,       sizeof(ZFXAabb),   1, pFile);
   fread(&m_Clr,      sizeof(ZFXCOLOR),  1, pFile);
   fread(&m_fR,       sizeof(float),     1, pFile);
   fread(&m_bFlicker, sizeof(bool),      1, pFile);
   fread(&m_bShadow,  sizeof(bool),      1, pFile);

   m_fR2 = m_fR * m_fR;

   // Now comes the mesh but we don't need this
   fread(&NumPolys, sizeof(UINT), 1, pFile);
   fread(&aabb,     sizeof(ZFXAabb), 1, pFile);

   for (UINT i=0; i<NumPolys; i++) {
      if (!LoadAndSkipMeshData(pFile)) 
         return false;
      }

   return true;
   } // Load
/*----------------------------------------------------------------*/


// just used to set file pointer to correct position 
bool CGameLight::LoadAndSkipMeshData(FILE *pFile) {
   UINT    NumVerts=0, NumIndis=0, SkinID=0;
   ZFXAabb aabb;
   bool    bDummy=false;
   float   fDummy[2] = { 0.0f, 0.0f };

   bool bResult=true;

   if (!pFile) return false;

   fread(&NumVerts, sizeof(UINT),    1, pFile);
   fread(&NumIndis, sizeof(UINT),    1, pFile);
   fread(&SkinID,   sizeof(UINT),    1, pFile);
   fread(&aabb,     sizeof(ZFXAabb), 1, pFile);
   fread(&bDummy,   sizeof(bool),    1, pFile);
   fread(fDummy,    sizeof(float),   2, pFile);
   fread(fDummy,    sizeof(float),   2, pFile);

   if (NumVerts == 0) bResult = false;

   VERTEX *pVerts     = new VERTEX[NumVerts];
   WORD   *pIndis     = new WORD[NumIndis];
   WORD   *pLineIndis = new WORD[NumVerts*2];

   if ( !pVerts || !pIndis || !pLineIndis) return false;

   fread(pVerts,     sizeof(VERTEX), NumVerts,   pFile);
   fread(pIndis,     sizeof(WORD),   NumIndis,   pFile);
   fread(pLineIndis, sizeof(WORD),   NumVerts*2, pFile);

   delete [] pVerts;
   delete [] pIndis;
   delete [] pLineIndis;

   return bResult;
   } // LoadAndSkipMeshData
/*----------------------------------------------------------------*/


// evaluate whether light influences the given tri at all
bool CGameLight::InfluencesTri(const ZFXVector *pPoints) {
   ZFXVector vc, vcN;
   float     fR2 = m_fR2*2.3f;
   float     fD2 = 0.0f;
   bool      bInside=false;

   // one vertex insinde light sphere?
   for (UINT i=0; i<3; i++) {
      vc = pPoints[i] - m_vcPos;
      fD2 = vc.GetSqrLength();
      if (fD2 <= fR2) { bInside = true; break; }
      }


   // else maybe just intersection with polyogn area
   if (!bInside) {

      // calc tri bounding box
      ZFXAabb aabb( pPoints[0], pPoints[0]);
      for (int i=0; i<3; i++) {
         if ( pPoints[i].x > aabb.vcMax.x ) 
            aabb.vcMax.x = pPoints[i].x;
         else if ( pPoints[i].x < aabb.vcMin.x ) 
            aabb.vcMin.x = pPoints[i].x;

         if ( pPoints[i].y > aabb.vcMax.y ) 
            aabb.vcMax.y = pPoints[i].y;
         else if ( pPoints[i].y < aabb.vcMin.y ) 
            aabb.vcMin.y = pPoints[i].y;

         if ( pPoints[i].z > aabb.vcMax.z ) 
            aabb.vcMax.z = pPoints[i].z;
         else if ( pPoints[i].z < aabb.vcMin.z ) 
            aabb.vcMin.z = pPoints[i].z;
         }

      // aabb vs sphere intersection test, ARVO algorithm
      float s=0, d=0;

      if      ( m_vcPos.x < aabb.vcMin.x ) s = m_vcPos.x - aabb.vcMin.x;
      else if ( m_vcPos.x > aabb.vcMax.x ) s = m_vcPos.x - aabb.vcMax.x;
      d += s*s; 

      if      ( m_vcPos.y < aabb.vcMin.y ) s = m_vcPos.y - aabb.vcMin.y;
      else if ( m_vcPos.y > aabb.vcMax.y ) s = m_vcPos.y - aabb.vcMax.y;
      d += s*s; 

      if      ( m_vcPos.z < aabb.vcMin.z ) s = m_vcPos.z - aabb.vcMin.z;
      else if ( m_vcPos.z > aabb.vcMax.z ) s = m_vcPos.z - aabb.vcMax.z;
      d += s*s; 

      if ( d <= m_fR2 ) 
         bInside = true;
      else 
         return false;
      }


   // build triangle center point
   vc = (pPoints[0] + pPoints[1] + pPoints[2])/3.0f;

   // vector triangle to light
   vc = m_vcPos - vc;
   vc.Normalize();

   // calculate triangle normal
   vcN.Cross( (pPoints[1] - pPoints[0]), 
              (pPoints[2] - pPoints[0]) );
   vcN.Normalize();

   // check angle between normal and light
   if ( (vcN * vc) < 0) return false;

   return true;
   } // InfluencesTri
/*----------------------------------------------------------------*/


// add triangle indices to cache with given skin id
HRESULT CGameLight::AddTriIndices(WORD i0, WORD i1, WORD i2, // indices
                                  const ZFXVector *pvc,      // vertices
                                  UINT nSkin) {              // skin id
   UINT i=0, nCount=0;

   for (i=0; i<m_NumSkins; i++) {
      // cache using this skin
      if (nSkin == m_pCache[i].nSkin) break;

      // empty cache will get this skin
      if (m_pCache[i].NumIndis == 0) {
         m_pCache[i].nSkin = nSkin;
         break;
         }
      }

   if (i >= m_NumSkins) return ZFX_INVALIDPARAM;
   
   nCount = m_pCache[i].NumIndis;
   m_pCache[i].pIndis = (WORD*)realloc(m_pCache[i].pIndis,
                          sizeof(WORD)*(nCount+3));

   if (!m_pCache[i].pIndis) return ZFX_OUTOFMEMORY;

   m_pCache[i].pIndis[nCount]   = i0;
   m_pCache[i].pIndis[nCount+1] = i1;
   m_pCache[i].pIndis[nCount+2] = i2;
   m_pCache[i].NumIndis += 3;

   if (!pvc) return ZFX_INVALIDPARAM;

   for (char v=0; v<3; v++) {
      if (pvc[v].x > m_Aabb.vcMax.x) m_Aabb.vcMax.x = pvc[v].x;
      else if (pvc[v].x < m_Aabb.vcMin.x) m_Aabb.vcMin.x = pvc[v].x;
      if (pvc[v].y > m_Aabb.vcMax.y) m_Aabb.vcMax.y = pvc[v].y;
      else if (pvc[v].y < m_Aabb.vcMin.y) m_Aabb.vcMin.y = pvc[v].y;
      if (pvc[v].z > m_Aabb.vcMax.z) m_Aabb.vcMax.z = pvc[v].z;
      else if (pvc[v].z < m_Aabb.vcMin.z) m_Aabb.vcMin.z = pvc[v].z;
      }

   m_Aabb.vcCenter = (m_Aabb.vcMax + m_Aabb.vcMin) / 2.0f;

   return ZFX_OK;
   } // AddTriIndices
/*----------------------------------------------------------------*/


// close the light caches and create index buffers for 'em
HRESULT CGameLight::CloseCaches(ZFXRenderDevice *pDevice) {
   UINT    i=0, nCount=0;
   HRESULT hr;

   ZFXVertexCacheManager *pVM = pDevice->GetVertexManager();

   for (i=0; i<m_NumSkins; i++) {
      if (m_pCache[i].NumIndis == 0) continue;
      
      m_pCache[i].ID = 0;

      hr = pVM->CreateIndexBuffer(m_pCache[i].NumIndis,
                                  m_pCache[i].pIndis,
                                  &m_pCache[i].ID);

      if (FAILED(hr)) return ZFX_FAIL;
      }
   return ZFX_OK;
   } // CloseCaches
/*----------------------------------------------------------------*/


// returns ID of index buffer for given skin if influence at all
bool CGameLight::InfluencesSkin(UINT nSkin, UINT *pIBID) {
   for (UINT i=0; i<m_NumSkins; i++) {
      if (m_pCache[i].nSkin == nSkin) {
         if (m_pCache[i].NumIndis < 3) 
            return false;

         if (pIBID) *pIBID = m_pCache[i].ID;
         return true;
         }
      }
   return false;
   } // InfluencesSkin
/*----------------------------------------------------------------*/


// check if the given aabb is inside the lights influence radius
bool CGameLight::InfluencesAabb(const ZFXAabb &aabb) {
   ZFXVector vc, vcN;
   float     fR2 = m_fR2*1.3f;
   float     fD2 = 0.0f;
   bool      bInside=false;

   vc = aabb.vcCenter - m_vcPos;
   fD2 = vc.GetSqrLength();
   if (fD2 <= fR2) bInside = true;

   vc = aabb.vcMin - m_vcPos;
   fD2 = vc.GetSqrLength();
   if (fD2 <= fR2) bInside = true;

   vc = aabb.vcMax - m_vcPos;
   fD2 = vc.GetSqrLength();
   if (fD2 <= fR2) bInside = true;

   if (!bInside) return false;
   return true;
   } // InfluencesAabb
/*----------------------------------------------------------------*/