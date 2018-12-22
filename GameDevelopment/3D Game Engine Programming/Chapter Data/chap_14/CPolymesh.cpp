// File: CPolymesh.cpp:


#include "include/CPolymesh.h"
#include "include/CSelectionBuffer.h"
#include "include/CApplication.h"


CPolymesh::CPolymesh(void) {
   Init();
   } // constructor
/*----------------------------------------------------------------*/

CPolymesh::~CPolymesh(void) {
   Release();
   } // destructor
/*----------------------------------------------------------------*/


// init all stuff
void CPolymesh::Init(void) {
   m_NumPolys      = 0;
   m_NumVerts      = 0;
   m_NumIndis      = 0;
   m_ppPoly        = NULL;
   m_bSelected     = false;
   m_bHidden       = false;
   m_bSupprBoxCalc = false;
   m_Type          = LOB_MESH;
   memset(&m_Aabb, 0, sizeof(ZFXAabb));
   } // Init
/*----------------------------------------------------------------*/


// release all stuff
void CPolymesh::Release(void) {
   if (m_ppPoly) {

      for (UINT i=0; i<m_NumPolys; i++)
         SAFE_DELETE(m_ppPoly[i]);

      SAFE_FREE(m_ppPoly);
      }
   } // Release
/*----------------------------------------------------------------*/


/**
 * Make this instance the copy of another polymesh object.
 */
void CPolymesh::CopyOf(CLevelObject *pLob) {
   
   if ( !pLob->IsOfType(m_Type) ) return;

   LPPOLYMESH pMesh = (LPPOLYMESH)pLob;

   Reset();

   // suppress multiple obb clacs
   m_bSupprBoxCalc = true;

   for (UINT i=0; i<pMesh->m_NumPolys; i++)
      AddPolygon( *pMesh->m_ppPoly[i] );

   CalcBoundingBox();
   m_bSupprBoxCalc = false;
   } // CopyOf
/*----------------------------------------------------------------*/


/**
 *  Turn the insides out by switching all polygons inside out.
 */
void CPolymesh::InsideOut(void) {
   for (UINT i=0; i<m_NumPolys; i++)
      m_ppPoly[i]->InsideOut();
   } // InsideOut
/*----------------------------------------------------------------*/


/**
 * Creates a polymesh from a list of polygons and skips old data.
 */
void CPolymesh::CreateFromPolygons(const CPolygon *pPolys, UCHAR n) {
   Release();
   Init();

   // suppress multiple obb calcs
   m_bSupprBoxCalc = true;

   for (UINT i=0; i<n; i++) AddPolygon(pPolys[i]);

   CalcBoundingBox();
   m_bSupprBoxCalc = false;
   } // CreateFromPolygons
/*----------------------------------------------------------------*/


/**
 * Removes the given polygon. 
 */
HRESULT CPolymesh::RemovePolygon(const LPPOLYGON pPoly) {
   UINT i=0;

   // seek reference to be deleted
   for (i=0; i<m_NumPolys; i++) {
      if (m_ppPoly[i] == pPoly) break;
      }

   if (i >= m_NumPolys) return ZFX_FAIL;

   // adjust counters
   m_NumVerts -= m_ppPoly[i]->GetNumVerts();
   m_NumIndis -= m_ppPoly[i]->GetNumIndis();

   // delete it, copy last one to this position
   SAFE_DELETE(m_ppPoly[i]);
   if (i != (m_NumPolys-1)) m_ppPoly[i] = m_ppPoly[m_NumPolys-1];
   m_NumPolys--;

   // could be faster method here
   CalcBoundingBox();

   return ZFX_OK;
   } // RemovePolygon
/*----------------------------------------------------------------*/


/**
 * Add a new polygon to the existing polymesh.
 */
HRESULT CPolymesh::AddPolygon(const CPolygon &Poly) {
   // allocate memory if neccessary
   m_ppPoly = (LPPOLYGON*) CheckMem( (LPVOID*)m_ppPoly,
               m_NumPolys, sizeof(LPPOLYGON) );

   // create a new instance and copy polygon
   m_ppPoly[m_NumPolys] = new CPolygon();
   m_ppPoly[m_NumPolys]->CopyOf( (LPLEVELOBJECT)&Poly );
   m_ppPoly[m_NumPolys]->SetAsPartOfMesh(this);

   // adjust counters
   m_NumVerts += m_ppPoly[m_NumPolys]->GetNumVerts();
   m_NumIndis += m_ppPoly[m_NumPolys]->GetNumIndis();
   m_NumPolys++;

   if (m_bSupprBoxCalc) return S_OK;

   // could be faster method here
   CalcBoundingBox();

   return S_OK;
   } // AddPolygon
/*----------------------------------------------------------------*/


/**
 * Render all the polygons in this polymesh.
 */
void CPolymesh::Render(ZFXVertexCacheManager *pVCMan, bool bHide) {
   if ( IsHidden() ) return;

   for (UINT i=0; i<m_NumPolys; i++) {
      // don't render hidden polygons
      if ( bHide && m_ppPoly[i]->IsHidden() ) continue;

      m_ppPoly[i]->Render(pVCMan, bHide);
      }
   } // Render 
/*----------------------------------------------------------------*/


/**
 * Select all polygons in this mesh.
 */
void CPolymesh::SelectAllPolys(CSelectionBuffer *pSB) {
   for (UINT i=0; i<m_NumPolys; i++) {
      if ( !m_ppPoly[i]->IsHidden() )
         pSB->AddLob(m_ppPoly[i]);
      }
   } // SelectAllPolys
/*----------------------------------------------------------------*/


/**
 * Select all polygons in this mesh.
 */
void CPolymesh::SelectInvert(CSelectionBuffer *pSB) {
   for (UINT i=0; i<m_NumPolys; i++) {

      if ( m_ppPoly[i]->IsHidden() ) continue;

      // select unselected polys from mesh
      if ( !m_ppPoly[i]->IsSelected() )
         pSB->AddLob(m_ppPoly[i]);
      // change flag on selected ones
      else m_ppPoly[i]->DeSelect();
      }
   } // SelectInvert
/*----------------------------------------------------------------*/


/**
 * Unhide all polygons in this mesh.
 */
void CPolymesh::HideNoPolys(void) {
   for (UINT i=0; i<m_NumPolys; i++) 
      m_ppPoly[i]->UnHide();
   } // HideNoPolys 
/*----------------------------------------------------------------*/


/**
 * Hide all selected polys in this mesh.
 */
void CPolymesh::HideSelectedPolys(void) {
   for (UINT i=0; i<m_NumPolys; i++) 
      if ( m_ppPoly[i]->IsSelected() ) m_ppPoly[i]->Hide();
   } // HideSelectedPolys 
/*----------------------------------------------------------------*/


/**
 * Hide all unselected polys in this mesh.
 */
void CPolymesh::HideUnselectedPolys(void) {
   if ( IsSelected() ) return;

   for (UINT i=0; i<m_NumPolys; i++) 
      if ( !m_ppPoly[i]->IsSelected() ) m_ppPoly[i]->Hide();
   } // HideUnselectedPolys 
/*----------------------------------------------------------------*/


/**
 * Invert the hide state of all polygons in this mesh.
 */
void CPolymesh::HideInvertPolys(void) {
   for (UINT i=0; i<m_NumPolys; i++) 
      if ( m_ppPoly[i]->IsHidden() ) m_ppPoly[i]->UnHide();
      else m_ppPoly[i]->Hide();
   } // HideInvertPolys 
/*----------------------------------------------------------------*/


/**
 * Loop through all polygons in this mesh and count the number
 * of different skins used which are not yet contained in the
 * given array.
 */
void CPolymesh::CountNonRedundantSkins(UINT *Array, UINT *pNum) {
   bool bFound=false;
   UINT Skin=0;

   for (UINT i=0; i<m_NumPolys; i++) {
      Skin = m_ppPoly[i]->GetSkinID();

      for (UINT j=0; j<(*pNum); j++) {
         if (Array[j] == Skin) {
            bFound = true;
            break;
            }
         } // for

      if (bFound) {
         bFound = false;
         continue;
         }
      else {
         Array[*pNum] = Skin;
         (*pNum)++;
         }
      } // for
   } // CountNonRedundantSkins 
/*----------------------------------------------------------------*/


/**
 * Save all polygons in the polymesh to a file.
 */
void CPolymesh::Save(FILE *pFile) {
   if (!pFile) return;

   fwrite(&m_NumPolys, sizeof(UINT), 1, pFile);
   fwrite(&m_Aabb, sizeof(ZFXAabb), 1, pFile);
   fflush(pFile);

   // sort polygons by texture
   qsort(m_ppPoly, m_NumPolys, sizeof(LPPOLYGON), SortBySkin);

   for (UINT i=0; i<m_NumPolys; i++)
      m_ppPoly[i]->Save(pFile);
   } // Save 
/*----------------------------------------------------------------*/


/**
 * Save only meshs polygons to a file.
 */
void CPolymesh::SavePurePolys(FILE *pFile) {
   UINT n=0, SkinID=0;
   char buffer[2048];

   if (!pFile) return;
   for (UINT i=0; i<m_NumPolys; i++) {
      // get the basic texture name for this poly
      SkinID = m_ppPoly[i]->GetSkinID();
      SendMessage(CApplication::m_G.hGUI[CSK_LIST],
                  CB_GETLBTEXT, SkinID, 
                  (LPARAM)(LPCSTR)buffer);
      // save string length and string to file
      n = strlen(buffer);
      fwrite(&n, sizeof(UINT), 1, pFile);
      fwrite(buffer, n, 1, pFile);
      
      m_ppPoly[i]->Save(pFile);
      }
   } // Save 
/*----------------------------------------------------------------*/


/**
 * Load all polygons in the polymesh from a file.
 */
bool CPolymesh::Load(FILE *pFile) {
   if (!pFile) return false;

   fread(&m_NumPolys, sizeof(UINT), 1, pFile);
   fread(&m_Aabb, sizeof(ZFXAabb), 1, pFile);

   if (m_NumPolys == 0) {
      // critical error
      return false;
      }

   // allocate memory
   m_ppPoly = (LPPOLYGON*)InitMem((LPVOID*)m_ppPoly,m_NumPolys,sizeof(LPPOLYGON));

   for (UINT i=0; i<m_NumPolys; i++) {
      m_ppPoly[i] = new CPolygon();
     
      if ( !m_ppPoly[i]->Load(pFile) ) {
         // critical error
         delete m_ppPoly[i];
         m_NumPolys = i;
         return false;
         }

      m_ppPoly[i]->SetAsPartOfMesh(this);
      }
   return true;
   } // Load 
/*----------------------------------------------------------------*/


/**
 * Pick the polys in the polymesh with the world ray and push their
 * pointers into the selection buffer.
 */
CPolygon* CPolymesh::PickPoly(const ZFXRay &Ray, float fMin, float *pfD) {
   CPolygon *pPoly = NULL;

   if ( IsHidden() ) return NULL;
   
   for (UINT i=0; i<m_NumPolys; i++) {

      // don't pick hidden polygons
      if ( m_ppPoly[i]->IsHidden() ) continue;

      // try to pick this mesh
      if ( m_ppPoly[i]->Picked(Ray, pfD) ) {

         // select only the closest
         if (*pfD < fMin) {
            fMin = *pfD;
            pPoly = m_ppPoly[i];
            }
         }
      }
   return pPoly;
   } // PickPoly
/*----------------------------------------------------------------*/


/**
 * Is this polymesh picked up by the world coordinates ray?
 */
bool CPolymesh::Picked(const ZFXRay &Ray, float *pfD) {
   if ( IsHidden() ) return false;

   if ( !m_Aabb.Intersects(Ray, NULL) ) return false;

   else if ( PickPoly(Ray,9999.9f,pfD) != NULL )
      return true;

   return false;
   } // Picked
/*----------------------------------------------------------------*/


/**
 * Does this polymesh intersect the world coordinates aabb?
 */
bool CPolymesh::Intersects(const ZFXAabb &aabb, ZFXAXIS axis) {
   if ( IsHidden() ) return false;

   // first check if polymesh aabb is at least 
   if (!m_Aabb.Intersects(aabb)) return false;

   // if any poly intersects the mesh really intersects
   for (UINT i=0; i<m_NumPolys; i++) {
      if (m_ppPoly[i]->IsHidden()) continue;
      if (m_ppPoly[i]->Intersects(aabb, axis))
         return true;
      }

   // no, just its aabb
   return false;
   } // Intersects
/*----------------------------------------------------------------*/


/**
 * Select all polygons from mesh which intersect aabb.
 */
void CPolymesh::GetIntersectingPolys(const ZFXAabb &aabb, 
                                     CSelectionBuffer *pSB,
                                     ZFXAXIS axis) {
   if ( IsHidden() ) return;

   // first check if polymesh aabb is at least 
   if (!m_Aabb.Intersects(aabb)) return;

   // check if any poly intersects the mesh really intersects
   for (UINT i=0; i<m_NumPolys; i++) {
      if (m_ppPoly[i]->IsHidden()) continue;
      if (m_ppPoly[i]->Intersects(aabb, axis))
         pSB->AddLob( m_ppPoly[i] );

      }
   } // GetIntersectingPolys
/*----------------------------------------------------------------*/


/**
 * Select all polygons from mesh which intersect aabb.
 */
void CPolymesh::GetIntersectingVerts(const ZFXAabb &aabb, 
                                     CSelectionBuffer *pSB,
                                     ZFXAXIS axis) {
   if ( IsHidden() ) return;

   // collect intersecting verts from mesh's polygons
   for (UINT i=0; i<m_NumPolys; i++) {
      m_ppPoly[i]->GetIntersectingVerts(aabb, pSB, axis);
      }
   } // GetIntersectingVerts
/*----------------------------------------------------------------*/


/**
 * Set the skin ID for all polys in the mesh.
 */
void CPolymesh::SetSkinID(UINT n) {
   for (UINT i=0; i<m_NumPolys; i++)
      m_ppPoly[i]->SetSkinID(n);
   } // SetSkinID
/*----------------------------------------------------------------*/


/**
 * Calculate the obb for this polymesh by adding up poly's boxes.
 */
void CPolymesh::CalcBoundingBox(void) {
   ZFXVector vcMax, vcMin, vcTemp;
   ZFXAabb   Aabb;

   if (m_NumPolys < 1)  return;

   // get any sub bounding box
   Aabb = m_ppPoly[0]->GetAabb();
   vcMax = vcMin = Aabb.vcCenter;

   for (UINT i=0; i<m_NumPolys; i++) {
      Aabb = m_ppPoly[i]->GetAabb();

      // get obb one side's extreme values
      vcTemp = Aabb.vcMax;

      if      ( vcTemp.x > vcMax.x ) vcMax.x = vcTemp.x;
      else if ( vcTemp.x < vcMin.x ) vcMin.x = vcTemp.x;
      if      ( vcTemp.y > vcMax.y ) vcMax.y = vcTemp.y;
      else if ( vcTemp.y < vcMin.y ) vcMin.y = vcTemp.y;
      if      ( vcTemp.z > vcMax.z ) vcMax.z = vcTemp.z;
      else if ( vcTemp.z < vcMin.z ) vcMin.z = vcTemp.z;

      // get obb other side's extreme values
      vcTemp = Aabb.vcMin;

      if      ( vcTemp.x > vcMax.x ) vcMax.x = vcTemp.x;
      else if ( vcTemp.x < vcMin.x ) vcMin.x = vcTemp.x;
      if      ( vcTemp.y > vcMax.y ) vcMax.y = vcTemp.y;
      else if ( vcTemp.y < vcMin.y ) vcMin.y = vcTemp.y;
      if      ( vcTemp.z > vcMax.z ) vcMax.z = vcTemp.z;
      else if ( vcTemp.z < vcMin.z ) vcMin.z = vcTemp.z;
      }

   m_Aabb.vcCenter = (vcMax + vcMin) / 2.0f;
   m_Aabb.vcMax    = vcMax;
   m_Aabb.vcMin    = vcMin;
   } // CalcBoundingBox
/*----------------------------------------------------------------*/


// Calculate planar texture coordinates with the given aabb.
void CPolymesh::CalcTextureCoords(ZFXAXIS axis, const ZFXAabb *pAabb) {
   if (!pAabb) pAabb = &m_Aabb;

   for (UINT i=0; i<m_NumPolys; i++) {
      m_ppPoly[i]->CalcTextureCoords(axis, pAabb);
      } // for
   } // CalcTextureCoords
/*----------------------------------------------------------------*/


// Calculate vertex normals.
void CPolymesh::CalcNormals(void) {
   for (UINT i=0; i<m_NumPolys; i++) {
      m_ppPoly[i]->CalcNormals();
      } // for
   } // CalcNormals
/*----------------------------------------------------------------*/


// transform all texture coordinates by the given values
void CPolymesh::TransTextureCoords(float ftU, float ftV, 
                                   float frU, float frV) {
   // brute force prevention of zero repeat
   if ( (frU < 0.0001f) && (frU > -0.0001f)) frU = 0.0001f;
   if ( (frV < 0.0001f) && (frV > -0.0001f)) frV = 0.0001f;

   // transform all polygon texture coordinates
   for (UINT i=0; i<m_NumPolys; i++)
      m_ppPoly[i]->TransTextureCoords(ftU,ftV,frU,frV);
   } // TransTextureCoors
/*----------------------------------------------------------------*/


// returns true if values are the same for all polys in the mesh
bool CPolymesh::GetTextureTrans(float *pftU, float *pftV,
                                float *pfrU, float *pfrV) {
   float ftU=0.0f, ftV=0.0f, frU=0.0f, frV=0.0f;

   for (UINT i=0; i<m_NumPolys; i++) {
      m_ppPoly[i]->GetTextureTrans(&ftU, &ftV, &frU, &frV);

      if (i>0) {
         if ( (*pftU != ftU) || (*pftV != ftV) ||
              (*pfrU != frU) || (*pfrV != frV) )
            return false;
         }

      // store values
      *pftU = ftU;  *pftV = ftV;
      *pfrU = frU;  *pfrV = frV;
      }

   return true;
   } // GetTextureTrans
/*----------------------------------------------------------------*/


// translate polygon by given vector
void CPolymesh::Translate(const ZFXVector &vcT) {
   // just translate the polygons, they will
   // take care of bounding box recalculation
   for (UCHAR i=0; i<m_NumPolys; i++)
      m_ppPoly[i]->Translate(vcT);
   } // Translate
/*----------------------------------------------------------------*/