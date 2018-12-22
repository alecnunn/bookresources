// File: CPolygon.cpp:

#include "include/CApplication.h"
#include "include/CPolygon.h"


int SortBySkin(const void* arg1, const void* arg2) {
   LPPOLYGON *p1 = (LPPOLYGON*)arg1;
   LPPOLYGON *p2 = (LPPOLYGON*)arg2;
   
   if( (*p1)->GetSkinID() > (*p2)->GetSkinID() )   return -1;
   if( (*p1)->GetSkinID() < (*p2)->GetSkinID() )   return +1;
   return 0;
   } // SortBySkin
/*----------------------------------------------------------------*/



// POLYGON CLASS IMPLEMENTATION ////////////////////////////////////

CPolygon::CPolygon(void) {
   Init();
   } // constructor
/*----------------------------------------------------------------*/

CPolygon::~CPolygon(void) {
   Release();
   } // destructor
/*----------------------------------------------------------------*/


// initialize to defaults
void CPolygon::Init(void) {
   m_NumVerts    = 0;
   m_NumIndis    = 0;
   m_NumPolys    = 1;
   m_SkinID      = 2;      // this is the default texture
   m_fTexOff[0]  = 0.0f;
   m_fTexOff[1]  = 0.0f;
   m_fTexRep[0]  = 1.0f;
   m_fTexRep[1]  = 1.0f;
   m_bSelected   = false;
   m_bHidden     = false;
   m_bPartOfMesh = false;
   m_pDad        = NULL;
   m_pVerts      = NULL;
   m_pIndis      = NULL;
   m_pLineIndis  = NULL;
   m_Type        = LOB_POLYGON;
   memset(&m_Aabb, 0, sizeof(ZFXAabb));
   } // Init
/*----------------------------------------------------------------*/


// clean up all memory and stuff
void CPolygon::Release(void) {
   SAFE_DELETE_A(m_pVerts);
   SAFE_DELETE_A(m_pIndis);
   SAFE_DELETE_A(m_pLineIndis);
   } // Release
/*----------------------------------------------------------------*/


// set this to a copy of another polygon
void CPolygon::CopyOf(CLevelObject *pLob) {

   if ( !pLob->IsOfType(m_Type) ) return;

   LPPOLYGON pPoly = (LPPOLYGON)pLob;

   Reset();
   SetVertices(pPoly->m_pVerts, pPoly->m_NumVerts);
   SetIndices(pPoly->m_pIndis, pPoly->m_NumIndis);
   m_SkinID = pPoly->m_SkinID;
   m_Aabb   = pPoly->m_Aabb;

   m_fTexOff[0] = pPoly->m_fTexOff[0];
   m_fTexOff[1] = pPoly->m_fTexOff[1];
   m_fTexRep[0] = pPoly->m_fTexRep[0];
   m_fTexRep[1] = pPoly->m_fTexRep[1];
   } // CopyOf
/*----------------------------------------------------------------*/


// turn the insides out (switch front- and backside)
void CPolygon::InsideOut(void) {
   WORD *pNewIndis = new WORD[m_NumIndis];

   // inverse indexlist results in clockorder change
   for (UINT i=0; i<m_NumIndis; i++) {
      pNewIndis[i] = m_pIndis[m_NumIndis-1-i];
      }

   // apply changes
   SetIndices(pNewIndis, m_NumIndis);

   SAFE_DELETE_A(pNewIndis);
   } // InsideOut
/*----------------------------------------------------------------*/


// create a portal from this poly (if it has exactly 4 vertices)
bool CPolygon::CreatePortal(CPortal *pPortal) {
   if (m_NumVerts != 4) return false;

   pPortal->SetGeometry(m_pVerts, m_pIndis, m_Aabb);

   return true;
   } // CreatePortal
/*----------------------------------------------------------------*/


// set polygon vertexlist 
void CPolygon::SetVertices(const VERTEX *pVerts, UINT NumVerts) {
   SAFE_DELETE_A(m_pVerts);
   SAFE_DELETE_A(m_pLineIndis);

   m_pVerts = new VERTEX[NumVerts];
   m_pLineIndis = new WORD[NumVerts*2];

   if ( !m_pVerts || !m_pLineIndis ) return;

   memcpy(m_pVerts, pVerts, sizeof(VERTEX)*NumVerts);
   m_NumVerts = NumVerts;

   for (UINT i=0; i<NumVerts; i++) {
      m_pLineIndis[(i*2)] = i;
      m_pLineIndis[(i*2)+1] = i+1;

      if (i==NumVerts-1) m_pLineIndis[(i*2)+1] = 0;
      }

   // calculate aabb
   CalcBoundingBox();
   } // SetVertices
/*----------------------------------------------------------------*/

 
// set polygon indexlist 
void CPolygon::SetIndices(const WORD *pIndis, UINT NumIndis) {
   SAFE_DELETE_A(m_pIndis);

   m_pIndis = new WORD[NumIndis];
   if (!m_pIndis) return;
   memcpy(m_pIndis, pIndis, sizeof(WORD)*NumIndis);
   m_NumIndis = NumIndis;

   CalcNormals();
   } // SetIndices
/*----------------------------------------------------------------*/
 

// renders the polygon using the vc manager
void CPolygon::Render(ZFXVertexCacheManager *pVCM, bool bHide) {
   if (m_bHidden && bHide) return;

   bool bAsLine = (pVCM->GetShadeMode() == RS_SHADE_LINES);

   if (bAsLine) {
      pVCM->Render(VID_UU, m_NumVerts, m_NumVerts*2, 
                   m_pVerts, m_pLineIndis, m_SkinID);
      }
   else {
      pVCM->Render(VID_UU, m_NumVerts, m_NumIndis, 
                   m_pVerts, m_pIndis, m_SkinID);
      }
   } // Render
/*----------------------------------------------------------------*/


// was the polygon hit by the ray in world coordinates?
bool CPolygon::Picked(const ZFXRay &Ray, float *pfD) {
   ZFXVector vc0, vc1, vc2;
   WORD      I0, I1, I2;

   // loop through the polygon's tris
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


// save this polygon to a file
void CPolygon::Save(FILE *pFile) {
   if (!pFile) return;

   // write counters
   fwrite(&m_NumVerts,    sizeof(UINT),    1, pFile);
   fwrite(&m_NumIndis,    sizeof(UINT),    1, pFile);
   fwrite(&m_SkinID,      sizeof(UINT),    1, pFile);
   fwrite(&m_Aabb,        sizeof(ZFXAabb), 1, pFile);
   fwrite(&m_bPartOfMesh, sizeof(bool),    1, pFile);
   fwrite(m_fTexOff,      sizeof(float),   2, pFile);
   fwrite(m_fTexRep,      sizeof(float),   2, pFile);

   // write data
   fwrite(m_pVerts,     sizeof(VERTEX), m_NumVerts,   pFile);
   fwrite(m_pIndis,     sizeof(WORD),   m_NumIndis,   pFile);
   fwrite(m_pLineIndis, sizeof(WORD),   m_NumVerts*2, pFile);
   fflush(pFile);
   } // Save
/*----------------------------------------------------------------*/


// load this polygon from a file
bool CPolygon::Load(FILE *pFile) {
   if (!pFile) return false;

   // read counters
   fread(&m_NumVerts,    sizeof(UINT),    1, pFile);
   fread(&m_NumIndis,    sizeof(UINT),    1, pFile);
   fread(&m_SkinID,      sizeof(UINT),    1, pFile);
   fread(&m_Aabb,        sizeof(ZFXAabb), 1, pFile);
   fread(&m_bPartOfMesh, sizeof(bool),    1, pFile);
   fread(m_fTexOff,      sizeof(float),   2, pFile);
   fread(m_fTexRep,      sizeof(float),   2, pFile);

   if (m_NumVerts == 0) {
      // critical error
      return false;
      }
   else
      {
      // allocate memory
      m_pVerts     = new VERTEX[m_NumVerts];
      m_pIndis     = new WORD[m_NumIndis];
      m_pLineIndis = new WORD[m_NumVerts*2];

      if ( !m_pVerts || !m_pIndis || !m_pLineIndis) return false;
      }

   // read data
   fread(m_pVerts,     sizeof(VERTEX), m_NumVerts,   pFile);
   fread(m_pIndis,     sizeof(WORD),   m_NumIndis,   pFile);
   fread(m_pLineIndis, sizeof(WORD),   m_NumVerts*2, pFile);
   
   CalcNormals();
   return true;
   } // Load
/*----------------------------------------------------------------*/


// calculate polygon's bounding box
void CPolygon::CalcBoundingBox(void) {
   ZFXVector vcMax, vcMin;

   // pick random start value from poly
   vcMax.x = vcMin.x = m_pVerts[0].x;
   vcMax.y = vcMin.y = m_pVerts[0].y;
   vcMax.z = vcMin.z = m_pVerts[0].z;

   // seek final values for each coordinate
   for (UINT i=0; i<m_NumVerts; i++) {
      
      if ( m_pVerts[i].x > vcMax.x ) 
         vcMax.x = m_pVerts[i].x;
      else if ( m_pVerts[i].x < vcMin.x ) 
         vcMin.x = m_pVerts[i].x;

      if ( m_pVerts[i].y > vcMax.y ) 
         vcMax.y = m_pVerts[i].y;
      else if ( m_pVerts[i].y < vcMin.y ) 
         vcMin.y = m_pVerts[i].y;

      if ( m_pVerts[i].z > vcMax.z ) 
         vcMax.z = m_pVerts[i].z;
      else if ( m_pVerts[i].z < vcMin.z ) 
         vcMin.z = m_pVerts[i].z;

      } // for

   m_Aabb.vcMax    = vcMax;
   m_Aabb.vcMin    = vcMin;
   m_Aabb.vcCenter = (vcMax + vcMin) / 2.0f;
   } // CalcBoundingBox
/*----------------------------------------------------------------*/


// calculate polygon's normals
void CPolygon::CalcNormals(void) {
   ZFXVector vc0, vc1, vc2, vcN;

   vc0.x = m_pVerts[m_pIndis[0]].x;
   vc0.y = m_pVerts[m_pIndis[0]].y;
   vc0.z = m_pVerts[m_pIndis[0]].z;

   vc1.x = m_pVerts[m_pIndis[1]].x;
   vc1.y = m_pVerts[m_pIndis[1]].y;
   vc1.z = m_pVerts[m_pIndis[1]].z;

   vc2.x = m_pVerts[m_pIndis[2]].x;
   vc2.y = m_pVerts[m_pIndis[2]].y;
   vc2.z = m_pVerts[m_pIndis[2]].z;

   vcN.Cross( (vc1-vc0), (vc2-vc0) );
   vcN.Normalize();

   // set the normal for each vertex
   for (UINT i=0; i<m_NumVerts; i++) {
      m_pVerts[i].vcN[0] = vcN.x;
      m_pVerts[i].vcN[1] = vcN.y;
      m_pVerts[i].vcN[2] = vcN.z;
      }

   } // CalcNormals
/*----------------------------------------------------------------*/


// triangulate (poly must be konvex)
void CPolygon::Triangulate() {
   UINT n=0, temp=0;

   SAFE_DELETE_A(m_pIndis);

   // n-poly => (n-2) triangles
   m_NumIndis = (m_NumVerts-2)*3;
   m_pIndis = new WORD[m_NumIndis];

   if (!m_pIndis) return;

   for (UINT i=0; i<(m_NumVerts-2); i++) {
      m_pIndis[(3*i)]   = 0;
      m_pIndis[(3*i)+1] = temp+1;
      m_pIndis[(3*i)+2] = temp+2;
      temp++;
      }
   } // Triangulate
/*----------------------------------------------------------------*/


// translate polygon by given vector
void CPolygon::Translate(const ZFXVector &vcT) {
   for (UINT i=0; i<m_NumVerts; i++) {
      m_pVerts[i].x += vcT.x;
      m_pVerts[i].y += vcT.y;
      m_pVerts[i].z += vcT.z;
      }
   m_Aabb.vcCenter += vcT;
   m_Aabb.vcMax    += vcT;
   m_Aabb.vcMin    += vcT;

   if (m_bPartOfMesh) m_pDad->CalcBoundingBox();
   } // Translate
/*----------------------------------------------------------------*/


// rotates the polygon around the given position
void CPolygon::Rotate(ZFXAXIS axis, const ZFXVector &vcPos, float a) {
   ZFXMatrix mat;
   ZFXVector vc;

   if (axis == X_AXIS) mat.RotaX(a);
   if (axis == Y_AXIS) mat.RotaY(a);
   if (axis == Z_AXIS) mat.RotaZ(a);

   for (UINT i=0; i<m_NumVerts; i++) {
      // detranslate to origin
      vc.x = m_pVerts[i].x - vcPos.x;
      vc.y = m_pVerts[i].y - vcPos.y;
      vc.z = m_pVerts[i].z - vcPos.z;

      // rotate
      vc = mat * vc;

      // retranslate to position
      m_pVerts[i].x = vc.x + vcPos.x;
      m_pVerts[i].y = vc.y + vcPos.y;
      m_pVerts[i].z = vc.z + vcPos.z;
      }
   
   CalcBoundingBox();
   if (m_bPartOfMesh) m_pDad->CalcBoundingBox();
   } // Rotate
/*----------------------------------------------------------------*/


// mirrors the polygon on the given axis
void CPolygon::Mirror(ZFXAXIS axis, const ZFXVector &vcPos) {
   for (UINT i=0; i<m_NumVerts; i++) {
      // detranslate to origin
      m_pVerts[i].x -= vcPos.x;
      m_pVerts[i].y -= vcPos.y;
      m_pVerts[i].z -= vcPos.z;

      // mirror
      if (axis == X_AXIS) m_pVerts[i].x *= -1.0f;
      if (axis == Y_AXIS) m_pVerts[i].y *= -1.0f;
      if (axis == Z_AXIS) m_pVerts[i].z *= -1.0f;

      // retranslate to position
      m_pVerts[i].x += vcPos.x;
      m_pVerts[i].y += vcPos.y;
      m_pVerts[i].z += vcPos.z;
      }
   
   CalcBoundingBox();
   if (m_bPartOfMesh) m_pDad->CalcBoundingBox();
   } // Mirror
/*----------------------------------------------------------------*/


// translate polygon by given vector
void CPolygon::SetAsPartOfMesh(CPolymesh *p) {
   if (p) {
      m_pDad = p;
      m_bPartOfMesh = true;
      }
   else {
      m_pDad = NULL;
      m_bPartOfMesh = false;
      }
   } // SetAsPartOfMesh
/*----------------------------------------------------------------*/


/**
 * Does this polygon intersect the world coordinates aabb?
 */
bool CPolygon::Intersects(const ZFXAabb &aabb, ZFXAXIS axis) {
   // first check if polygon's aabb does at least 
   if (!m_Aabb.Intersects(aabb)) return false;

   // if any vertex intersects the polygon really intersects
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


/**
 * Selects all vertices from polygon which intersect the aabb.
 */
void CPolygon::GetIntersectingVerts(const ZFXAabb &aabb,
                                    CSelectionBuffer *pSB,
                                    ZFXAXIS axis) {
   // add all intersecting vertices to the buffer
   for (UINT i=0; i<m_NumVerts; i++) {
      // simplify to 2D problem
      if (axis == X_AXIS) {
         if ((m_pVerts[i].y < aabb.vcMax.y) &&
             (m_pVerts[i].y > aabb.vcMin.y) &&
             (m_pVerts[i].z < aabb.vcMax.z) &&
             (m_pVerts[i].z > aabb.vcMin.z))
            pSB->AddVertex(&m_pVerts[i], this);
         }
      else if (axis == Y_AXIS) {
         if ((m_pVerts[i].x < aabb.vcMax.x) &&
             (m_pVerts[i].x > aabb.vcMin.x) &&
             (m_pVerts[i].z < aabb.vcMax.z) &&
             (m_pVerts[i].z > aabb.vcMin.z))
            pSB->AddVertex(&m_pVerts[i], this);
         }
      else if (axis == Z_AXIS) {
         if ((m_pVerts[i].y < aabb.vcMax.y) &&
             (m_pVerts[i].y > aabb.vcMin.y) &&
             (m_pVerts[i].x < aabb.vcMax.x) &&
             (m_pVerts[i].x > aabb.vcMin.x))
            pSB->AddVertex(&m_pVerts[i], this);
         }
      }
   } // GetIntersectingVerts
/*----------------------------------------------------------------*/


/**
 * Calculate planar texture coords with the given or member aabb.
 */
void CPolygon::CalcTextureCoords(ZFXAXIS axis, const ZFXAabb *pAabb) {
   VERTEX *pVert=NULL;
   ZFXAabb aabb;

   if (pAabb) aabb = *pAabb;
   else aabb = m_Aabb;

   m_fTexOff[0] = m_fTexOff[1] = 0.0f;
   m_fTexRep[0] = m_fTexRep[1] = 1.0f;

   ZFXVector vcSize = aabb.vcMax - aabb.vcMin;

   for (UINT i=0; i<m_NumVerts; i++) {
      pVert = &m_pVerts[i];

      // planar texturing 
      if (axis == X_AXIS) {
         pVert->tu = (pVert->z - aabb.vcMin.z)/vcSize.z;
         pVert->tv = (pVert->y - aabb.vcMax.y)/vcSize.y;
         }
      else if (axis == Y_AXIS) {
         pVert->tu = (pVert->x - aabb.vcMin.x)/vcSize.x;
         pVert->tv = (pVert->z - aabb.vcMax.z)/vcSize.z;
         }
      else if (axis == Z_AXIS) {
         pVert->tu = (pVert->x - aabb.vcMin.x)/vcSize.x;
         pVert->tv = (pVert->y - aabb.vcMax.y)/vcSize.y;
         }
      } // for
   } // CalcTextureCoords
/*----------------------------------------------------------------*/


/**
 * Set texture offset and repeat to given values. But take care 
 * not to overwrite old settings.
 */
void CPolygon::TransTextureCoords(float ftU, float ftV, 
                                  float frU, float frV) {
   
   if (frU == 0.0f) frU = 0.00001f;
   if (frV == 0.0f) frV = 0.00001f;

   // loop through all vertices
   for (UINT i=0; i<m_NumVerts; i++) {
      // undo old offset and repeat
      if (m_fTexOff[0] != 0.0f)
         m_pVerts[i].tu -= m_fTexOff[0];
      if (m_fTexOff[1] != 0.0f)
         m_pVerts[i].tv -= m_fTexOff[1];

      if ((m_fTexRep[0] != 1.0f) &&
         (m_fTexRep[0] != 0.0f) ) {
         m_pVerts[i].tu /= m_fTexRep[0];
         }
      if ((m_fTexRep[1] != 1.0f) &&
          (m_fTexRep[1] != 0.0f) ) {
         m_pVerts[i].tv /= m_fTexRep[1];
         }

      // set to new values
      m_pVerts[i].tu = m_pVerts[i].tu*frU + ftU;
      m_pVerts[i].tv = m_pVerts[i].tv*frV + ftV;
      }
   
   m_fTexOff[0] = ftU;
   m_fTexOff[1] = ftV;
   m_fTexRep[0] = frU;
   m_fTexRep[1] = frV;
   } // TransTextureCoors
/*----------------------------------------------------------------*/


/**
 * Get texture offset and repeat from the polygon.
 */
void CPolygon::GetTextureTrans(float *pftU, float *pftV, 
                               float *pfrU, float *pfrV) {
   if (pftU) *pftU = m_fTexOff[0];
   if (pftV) *pftV = m_fTexOff[1];
   if (pfrU) *pfrU = m_fTexRep[0];
   if (pfrV) *pfrV = m_fTexRep[1];
   } // GetTextureTrans
/*----------------------------------------------------------------*/
