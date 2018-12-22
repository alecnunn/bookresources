// File: CEntity.cpp:

#include "include/CEntity.h"

CEntity::CEntity(void) {
   Init();
   } // constructor
/*----------------------------------------------------------------*/

CEntity::~CEntity(void) {
   Release();
   } // destructor
/*----------------------------------------------------------------*/


// initialize to defaults
void CEntity::Init(void) {
   m_NumVerts    = 0;
   m_NumIndis    = 0;
   m_NumPolys    = 1;
   m_bSelected   = false;
   m_bHidden     = false;
   m_Type        = LOB_ENTITY;
   m_SubType     = ENT_UNKNOWN;
   m_pData       = NULL;
   m_nSize       = 0;
   memset(&m_Aabb, 0, sizeof(ZFXAabb));
   } // Init
/*----------------------------------------------------------------*/


// clean up all memory and stuff
void CEntity::Release(void) {
   if (m_pData) {
      free(m_pData);
      m_pData = NULL;
      }

   SAFE_FREE(m_pData);
   } // Release
/*----------------------------------------------------------------*/


// set this to a copy of another entity
void CEntity::CopyOf(CLevelObject *pLob) {
   ZFXENTITY SubType=ENT_UNKNOWN;

   if ( !pLob->IsOfType(m_Type) ) return;

   LPENTITY pEntity = (LPENTITY)pLob;

   Reset();

   m_Mesh.CopyOf( &pEntity->m_Mesh );
   SubType   = pEntity->m_SubType;
   m_Aabb    = m_Mesh.GetAabb();
   
   // allocate memory
   SetSubType(SubType);

   if (pEntity->m_pData && m_pData) {
      switch (m_SubType) {
         case ENT_DOOR:
            memcpy(m_pData, pEntity->m_pData, m_nSize);
            break;
         default: break;
         } // switch
      }
   } // CopyOf
/*----------------------------------------------------------------*/


// set the type of entity
void CEntity::SetSubType(ZFXENTITY SubType) {
   if (SubType == m_SubType) return;

   m_SubType = SubType;

   SAFE_FREE(m_pData);

   switch (m_SubType) {
      case ENT_DOOR:
         m_nSize = sizeof(ENTITYDOOR);
         m_pData = malloc(m_nSize);
         if (!m_pData) break;
         memset(m_pData, 0, m_nSize);
         break;
      default: break;
      } // switch
   } // SetSubType
/*----------------------------------------------------------------*/


// set the mesh object for this entity
void CEntity::SetGeometry(const CPolymesh *pMesh) {
   m_Mesh.CopyOf( ((CPolymesh*)pMesh) );
   } // SetGeometry
/*----------------------------------------------------------------*/


// set the data for the entity
void CEntity::SetData(const ENTITYDOOR &data) {
   if (!m_pData) return;
   if (m_SubType != ENT_DOOR) return;
   memcpy(m_pData, &data, sizeof(ENTITYDOOR));
   } // SetData

void CEntity::SetData(const ENTITYPLAYER &data) {
   if (!m_pData) return;
   if (m_SubType != ENT_PLAYER) return;
   memcpy(m_pData, &data, sizeof(ENTITYPLAYER));
   } // SetData
/*----------------------------------------------------------------*/


// Was the entity hit by the ray in world coordinates?
bool CEntity::Picked(const ZFXRay &Ray, float *pfD) {
   return m_Mesh.Picked(Ray, pfD);
   } // Picked
/*----------------------------------------------------------------*/


// Does this entity intersect the world coordinates aabb?
bool CEntity::Intersects(const ZFXAabb &aabb, ZFXAXIS axis) {
   return m_Mesh.Intersects(aabb, axis);
   } // Intersects
/*----------------------------------------------------------------*/


// translate this light object's world position
void CEntity::Translate(const ZFXVector &vcT) {
   m_Mesh.Translate(vcT);
   m_Aabb = m_Mesh.GetAabb();
   } // SetPosition
/*----------------------------------------------------------------*/


// saves this entity to a file
void CEntity::Save(FILE *pFile) {
   if (!pFile) return;

   fwrite(&m_SubType, sizeof(ZFXENTITY), 1, pFile);
   fwrite(&m_Aabb,    sizeof(ZFXAabb),   1, pFile);
   fwrite(&m_nSize,   sizeof(UINT),      1, pFile);
   fwrite(m_pData,    m_nSize,           1, pFile);

   m_Mesh.Save(pFile);
   } // Save
/*----------------------------------------------------------------*/


// loads this entity from a file
bool CEntity::Load(FILE *pFile) {
   ZFXENTITY SubType=ENT_UNKNOWN;

   if (!pFile) return false;

   fread(&SubType, sizeof(ZFXENTITY), 1, pFile);

   // allocate memory for pData
   SetSubType(SubType);

   fread(&m_Aabb,  sizeof(ZFXAabb), 1, pFile);
   fread(&m_nSize, sizeof(UINT),    1, pFile);
   fread(m_pData,  m_nSize, 1, pFile);

   return m_Mesh.Load(pFile);
   } // Load
/*----------------------------------------------------------------*/


// renders the entity using the vc manager
void CEntity::Render(ZFXVertexCacheManager *pVCM, bool bHide) {
   if (m_bHidden && bHide) return;
   m_Mesh.Render(pVCM, bHide);
   } // Render
/*----------------------------------------------------------------*/


// removes connections from this door to the portal
void CEntity::RemovePortalConnections(UINT id) {
   if (m_SubType != ENT_DOOR) return;

   ENTITYDOOR *pData=NULL;

   pData = (ENTITYDOOR*)m_pData;
   if (pData->Portal[0] == id) pData->Portal[0]=0;
   if (pData->Portal[1] == id) pData->Portal[1]=0;
   } // RemovePortalConnections
/*----------------------------------------------------------------*/


// connects entity to portal if it's a door and has a free slot
bool CEntity::ConnectTo(CPortal *pPortal) {
   if (m_SubType != ENT_DOOR) return false;

   ENTITYDOOR *pData=NULL;

   pData = (ENTITYDOOR*)m_pData;
   if (pData->Portal[0] != 0) {
      if (pData->Portal[1] == 0) {
         pData->Portal[1] = pPortal->GetID();
         return true;
         }
      else return false;
      }
   else {
      pData->Portal[0] = pPortal->GetID();
      return true;
      }
   } // RemovePortalConnections
/*----------------------------------------------------------------*/