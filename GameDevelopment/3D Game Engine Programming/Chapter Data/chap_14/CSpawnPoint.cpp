// File: CSpawnPoint.cpp:

#include "include/CSpawnPoint.h"

CSpawnPoint::CSpawnPoint(void) {
   Init();
   } // constructor
/*----------------------------------------------------------------*/

CSpawnPoint::~CSpawnPoint(void) {
   Release();
   } // destructor
/*----------------------------------------------------------------*/


// initialize to defaults
void CSpawnPoint::Init(void) {
   CGeometry geo;

   m_NumVerts    = 0;
   m_NumIndis    = 0;
   m_NumPolys    = 1;
   m_vcPos       = ZFXVector(0,0,0);
   m_bSelected   = false;
   m_bHidden     = false;
   m_Type        = LOB_SPAWNPOINT;
   memset(&m_Aabb, 0, sizeof(ZFXAabb));

   geo.CreateLight(&m_Mesh, NULL, 
                   ZFXVector(0,0,0), 
                   0.2f, 0.2f, 0.2f);
   
   m_Mesh.SetSkinID(1);
   } // Init
/*----------------------------------------------------------------*/


// clean up all memory and stuff
void CSpawnPoint::Release(void) {
   /* nothing to do */
   } // Release
/*----------------------------------------------------------------*/


// set the position of this spawn point
void CSpawnPoint::SetPosition(const ZFXVector &vcPos) {
   // use the translate method here which
   // will maintain aabb, mesh and stuff
   ZFXVector vcT = vcPos - m_vcPos;
   Translate(vcT);
   } // SetPosition
/*----------------------------------------------------------------*/


// translate this spawn points world position
void CSpawnPoint::Translate(const ZFXVector &vcT) {
   m_vcPos += vcT;
   m_Mesh.Translate(vcT);
   m_Aabb = m_Mesh.GetAabb();
   } // SetPosition
/*----------------------------------------------------------------*/


// set this to a copy of another spawn point
void CSpawnPoint::CopyOf(CLevelObject *pLob) {
   
   if ( !pLob->IsOfType(m_Type) ) return;

   LPSPAWNPOINT pSpawnPoint = (LPSPAWNPOINT)pLob;

   Reset();

   m_Aabb     = pLob->GetAabb();
   m_vcPos    = pSpawnPoint->m_vcPos;
   m_Mesh.CopyOf( &pSpawnPoint->m_Mesh );
   } // CopyOf
/*----------------------------------------------------------------*/


// saves this spawn point to a file
void CSpawnPoint::Save(FILE *pFile) {
   if (!pFile) return;

   fwrite(&m_vcPos, sizeof(ZFXVector), 1, pFile);
   fwrite(&m_Aabb,  sizeof(ZFXAabb),   1, pFile);

   m_Mesh.Save(pFile);
   } // Save
/*----------------------------------------------------------------*/


// loads this spawn point from a file
bool CSpawnPoint::Load(FILE *pFile) {
   if (!pFile) return false;

   fread(&m_vcPos, sizeof(ZFXVector), 1, pFile);
   fread(&m_Aabb,  sizeof(ZFXAabb),   1, pFile);

   return m_Mesh.Load(pFile);
   } // Load
/*----------------------------------------------------------------*/


// renders the spawn point using the vc manager
void CSpawnPoint::Render(ZFXVertexCacheManager *pVCM, bool bHide) {
   if (m_bHidden && bHide) return;
   m_Mesh.Render(pVCM, bHide);
   } // Render
/*----------------------------------------------------------------*/




