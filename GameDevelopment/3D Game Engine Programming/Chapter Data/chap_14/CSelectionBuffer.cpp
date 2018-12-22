// FILE: CSelectionBuffer.cpp

#include "include/CSelectionBuffer.h"
#include "include/CLevel.h"
#include "include/CApplication.h"

// constructor
CSelectionBuffer::CSelectionBuffer(void) {
   Init();
   } // constructor
/*----------------------------------------------------------------*/


// destructor
CSelectionBuffer::~CSelectionBuffer(void) {
   Release();
   } // destructor
/*----------------------------------------------------------------*/


// reinitialize the buffer
void CSelectionBuffer::Reset(void) {
   Release();
   Init();
   m_Aabb.vcCenter = ZFXVector(0,0,0);
   m_Aabb.vcMax    = ZFXVector(0,0,0);
   m_Aabb.vcMin    = ZFXVector(0,0,0);
   } // Init
/*----------------------------------------------------------------*/


// initialize attributes
void CSelectionBuffer::Init(void) {
   m_NumVerts     = 0;
   m_NumLobs      = 0;
   m_pVerts       = NULL;
   m_ppLob        = NULL;
   m_fD           = 0.05f;
   memset(&m_Aabb, 0, sizeof(ZFXAabb));
   } // Init
/*----------------------------------------------------------------*/


// release memory and stuff
void CSelectionBuffer::Release(void) {
   UINT i=0;

   for (i=0; i<m_NumLobs; i++)
      if (m_ppLob[i]) m_ppLob[i]->DeSelect();

   // free memory
   SAFE_FREE(m_ppLob);
   SAFE_FREE(m_pVerts);
   } // Release
/*----------------------------------------------------------------*/


// release memory but do not inform objects about deselection
void CSelectionBuffer::DestroyWithoutNotify(void) {
   // free memory
   SAFE_FREE(m_ppLob);
   SAFE_FREE(m_pVerts);

   Init();
   } // DestroyWithoutNotification
/*----------------------------------------------------------------*/


// copy all contents of the buffer and select the copies
void CSelectionBuffer::CopySelectedObjects(LPSECTOR pSector) {
   DWORD     Type=LOB_NONE;
   CPolygon  Poly;
   CPolymesh Mesh;
   CLight    Light;

   ZFXVector vcT(0.2f,0.2f,0.2f);
   
   // take care to deselect objects
   for (UINT i=0; i<m_NumLobs; i++) {
      m_ppLob[i]->DeSelect();
      
      switch ( m_ppLob[i]->GetType() ) {
         case LOB_POLYGON:
            Poly.CopyOf( (LPPOLYGON)m_ppLob[i] );
            Poly.Translate(vcT);
            m_ppLob[i] = (LPPOLYGON)pSector->AddObject(Poly);
            break;
         case LOB_MESH:
            Mesh.CopyOf( (LPPOLYMESH)m_ppLob[i] );
            Mesh.Translate(vcT);
            m_ppLob[i] = (LPPOLYMESH)pSector->AddObject(Mesh);
            break;
         case LOB_LIGHT:
            Light.CopyOf( (LPLIGHT)m_ppLob[i] );
            Light.Translate(vcT);
            m_ppLob[i] = (LPLIGHT)pSector->AddObject(Light);
            break;
         default: break;
         } // switch
      
      m_ppLob[i]->Select();
      } // for

   m_Aabb.vcCenter += vcT;
   m_Aabb.vcMax    += vcT;
   m_Aabb.vcMin    += vcT;
   } // CopySelectedObjects
/*----------------------------------------------------------------*/


// add a level object reference to the selection buffer
void CSelectionBuffer::AddLob(const LPLEVELOBJECT pLob) {
   // don't push more than once
   if ( pLob->IsSelected() ) return;

   // allocate memory if neccessary
   if ( (m_NumLobs%50) == 0 )
      m_ppLob = (LPLEVELOBJECT*) realloc(
                      m_ppLob, 
                      sizeof(LPLEVELOBJECT)
                      * (m_NumLobs+50) );
   // add to inventory
   m_ppLob[m_NumLobs] = pLob;
   m_NumLobs++;

   // inform poly about its selection
   pLob->Select();

   // recalculate aabb
   AddBoundingBox( m_ppLob[m_NumLobs-1]->GetAabb() );
   } // AddLob
/*----------------------------------------------------------------*/


// add a vertex reference to the selection buffer
void CSelectionBuffer::AddVertex(VERTEX *pVertex, 
                                 const LPPOLYGON pPoly) {
   ZFXAabb aabb;

   // don't push more than once
   for (UINT i=0; i<m_NumVerts; i++) {
      if (m_pVerts[i].pV == pVertex) return;
      }

   // allocate memory if neccessary
   if ( (m_NumVerts%50) == 0 )
      m_pVerts = (VERTEX_SB*) realloc(m_pVerts, 
                                      sizeof(VERTEX_SB) *
                                      (m_NumVerts+50));
   // add to inventory
   m_pVerts[m_NumVerts].pV    = pVertex;
   m_pVerts[m_NumVerts].pPoly = pPoly;
   m_NumVerts++;

   // enlarge bounding box
   aabb.vcMax    = ZFXVector(pVertex->x, pVertex->y, pVertex->z);
   aabb.vcMin    = ZFXVector(pVertex->x, pVertex->y, pVertex->z);
   aabb.vcCenter = ZFXVector(pVertex->x, pVertex->y, pVertex->z);

   aabb.vcMax += 0.05f;
   aabb.vcMin -= 0.05f;
   AddBoundingBox(aabb);
   } // AddVertex
/*----------------------------------------------------------------*/

 
// check if only one poly is selected and if it has exactly four
// vertices so we can build a portal from it.
LPPOLYGON CSelectionBuffer::IsPolyQuad(void) {

   if (GetNumObjects() != 1) return NULL;
   if (GetNumVerts()   != 0) return NULL;
   if (m_ppLob[0]->GetNumVerts() != 4) return NULL;

   return ((LPPOLYGON)m_ppLob[0]);
   } // IsPolyQuad
/*----------------------------------------------------------------*/


// render selected elements (in wireframe if wished)
void CSelectionBuffer::Render(ZFXRenderDevice *pDevice, float fPoint,
                              bool bWire, const ZFXVector *pvcTranslate,
                              const ZFXCOLOR *pAmb) {
   ZFXMatrix mat;
   UINT i=0;

   if ( GetNumObjects() < 1 ) return;

   ZFXVertexCacheManager *pVCM = pDevice->GetVertexManager();

   static ZFXCOLOR clrLG = { 0.3f, 1.0f, 0.3f, 1.0f };
   static ZFXCOLOR clrDG = { 1.0f, 1.0f, 0.7f, 1.0f };

   if (pvcTranslate) {
      mat.Identity();
      mat._41 = pvcTranslate->x;
      mat._42 = pvcTranslate->y;
      mat._43 = pvcTranslate->z;
      pDevice->SetWorldTransform(&mat);
      }

   // if wire then render as points first
   if (bWire) {
      pDevice->SetShadeMode(RS_SHADE_POINTS, fPoint, &clrDG);
      pDevice->SetDepthBufferMode(RS_DEPTH_NONE);

      for (i=0; i<m_NumLobs; i++) m_ppLob[i]->Render( pVCM, true );

      pDevice->SetDepthBufferMode(RS_DEPTH_READWRITE);
      pDevice->SetShadeMode(RS_SHADE_LINES, 0, &clrDG);

      for (i=0; i<m_NumLobs; i++) m_ppLob[i]->Render( pVCM, true );

      }
   else {
      pDevice->SetShadeMode(RS_SHADE_SOLID, 0, NULL);

      if (pAmb)
         pDevice->SetAmbientLight(pAmb->fR, pAmb->fG, pAmb->fB);
      
      for (i=0; i<m_NumLobs; i++) m_ppLob[i]->Render( pVCM, true );

      pDevice->SetAmbientLight(1.0f, 1.0f, 1.0f);
      }


   RenderVertices(pDevice, fPoint);

   // set wireframe if not yet and change color, render aabb
   pDevice->SetShadeMode(RS_SHADE_LINES, 0, &clrLG);
   RenderBoundingBox(pDevice);

   // back to default settings
   if (pvcTranslate) pDevice->SetWorldTransform(NULL);
   pDevice->SetShadeMode(RS_SHADE_SOLID, 0, NULL);
   } // Render
/*----------------------------------------------------------------*/


// render all vertices which are selected in the buffer
void CSelectionBuffer::RenderVertices(ZFXRenderDevice *pDevice, float fPoint) {
   static ZFXCOLOR clrDG = { 1.0f, 1.0f, 0.7f, 1.0f };
   VERTEX *pVerts = new VERTEX[m_NumVerts];

   for (UINT i=0; i<m_NumVerts; i++) {
      memcpy(&pVerts[i], m_pVerts[i].pV, sizeof(VERTEX));
      }

   pDevice->SetDepthBufferMode(RS_DEPTH_NONE);
   pDevice->SetShadeMode(RS_SHADE_POINTS, fPoint, &clrDG);

   pDevice->GetVertexManager()->Render(VID_UU, m_NumVerts, 
                                       0, pVerts, NULL, 0);

   pDevice->SetDepthBufferMode(RS_DEPTH_READWRITE);
   delete [] pVerts;
   } // RenderVertices
/*----------------------------------------------------------------*/


// delete all objects which are selected with this buffer
void CSelectionBuffer::DeleteSelectedObjects(LPLEVEL pLevel) {
   UINT id=0;

   CSector *pSector = pLevel->GetSelectedSector();

   for (UINT i=0; i<m_NumLobs; i++) {

      // if this is a portal then remove all
      // references to it from the level !!!
      if (m_ppLob[i]->GetType() == LOB_PORTAL) {
         id = ( (LPPORTAL)m_ppLob[i] )->GetID();
         pLevel->RemovePortalConnections( id );
         }

      pSector->RemoveObject(m_ppLob[i]);
      }

   // now we don't have anything left
   m_NumVerts = 0;
   m_NumLobs = 0;

   Reset();
   } // DeleteSelectedObjects
/*----------------------------------------------------------------*/


// merge all selected polygons into a single mesh object
void CSelectionBuffer::MergePolysToMesh(LPSECTOR pSector, HWND hWnd) {
   LPPOLYGON pPoly=NULL;
   CPolymesh Mesh;
   bool      bAsked=false;
   bool      bUseEm=false;

   if (m_NumLobs==0) return;

   // build a new mesh from selected polygons
   for (UINT i=0; i<m_NumLobs; i++) {

      if ( m_ppLob[i]->GetType() != LOB_POLYGON ) continue;
      else pPoly = (LPPOLYGON) m_ppLob[i];

      if ( pPoly->IsPartOfMesh() ) {
         if (bAsked && !bUseEm) continue;
         if (!bAsked) {
            if ( MessageBox(hWnd,
                    "At least one selected polygon is part\nof a mesh. Merge them also?",
                     "PanEdit - Warning", MB_YESNO|MB_ICONQUESTION)
                 == IDYES) bUseEm=true;
            bAsked=true;
            }
         }

      Mesh.AddPolygon( *pPoly );
      pSector->RemoveObject(pPoly);
      }

   // add this mesh to the sector
   pSector->AddObject(Mesh);
   m_NumLobs = 0;
   Reset();
   } // MergePolysToMesh
/*----------------------------------------------------------------*/


// merges all selected meshs into one big mesh
void CSelectionBuffer::MergeMeshs(LPSECTOR pSector) {
   if (m_NumLobs==0) return;

   // NEEDS TO BE IMPLEMENTED SOMEWHEN :)

   // just loop through the meshs' polys and add them
   // to a new poly. remove selected meshs from sector
   // and add new mesh instead

   m_NumLobs = 0;
   Reset();
   } // MergeMeshs
/*----------------------------------------------------------------*/


// fragment all selected meshs to build stand alone polygons
void CSelectionBuffer::FragmentMeshs(LPSECTOR pSector) {
   LPPOLYMESH pMesh=NULL;
   LPPOLYGON  pPoly=NULL;
   UINT n=0;

   if (m_NumLobs==0) return;

   // loop through all selected meshs
   for (UINT i=0; i<m_NumLobs; i++) {

      if ( m_ppLob[i]->GetType() != LOB_MESH ) continue;
      else pMesh = (LPPOLYMESH) m_ppLob[i];

      n = pMesh->GetNumPolys();

      // add all polys from the mesh to the sector
      for (UINT j=0; j<pMesh->GetNumPolys(); j++) {
         pPoly = pMesh->GetPolygon(j);
         pSector->AddObject( *pPoly );
         }
      // finally remove this mesh
      pSector->RemoveObject(pMesh);
      }
   m_NumLobs = 0;
   Reset();
   } // FragmentMeshs
/*----------------------------------------------------------------*/


// change front- and backside for all selected polagons
void CSelectionBuffer::InsideOut(void) {
   // loop through all selected objects
   for (UINT i=0; i<m_NumLobs; i++) {
      if (m_ppLob[i]->GetType() == LOB_MESH)
         ((LPPOLYMESH)m_ppLob[i])->InsideOut();

      else if (m_ppLob[i]->GetType() == LOB_POLYGON)
         ((LPPOLYGON)m_ppLob[i])->InsideOut();
      } // for
   } // InsideOut
/*----------------------------------------------------------------*/


// recalculate bounding box for the selection buffer
void CSelectionBuffer::AddBoundingBox(const ZFXAabb &aabb) {
   UINT i=0;

   // only one object
   if ( GetNumObjects() == 1) {
      m_Aabb = aabb;
      m_Aabb.vcMin -= m_fD;
      m_Aabb.vcMax += m_fD;
      }
   else {
      if ((aabb.vcMin.x-m_fD) < m_Aabb.vcMin.x) 
         m_Aabb.vcMin.x = aabb.vcMin.x-m_fD;
      if ((aabb.vcMin.y-m_fD) < m_Aabb.vcMin.y) 
         m_Aabb.vcMin.y = aabb.vcMin.y-m_fD;
      if ((aabb.vcMin.z-m_fD) < m_Aabb.vcMin.z) 
         m_Aabb.vcMin.z = aabb.vcMin.z-m_fD;

      if ((aabb.vcMax.x+m_fD) > m_Aabb.vcMax.x) 
         m_Aabb.vcMax.x = aabb.vcMax.x+m_fD;
      if ((aabb.vcMax.y+m_fD) > m_Aabb.vcMax.y) 
         m_Aabb.vcMax.y = aabb.vcMax.y+m_fD;
      if ((aabb.vcMax.z+m_fD) > m_Aabb.vcMax.z) 
         m_Aabb.vcMax.z = aabb.vcMax.z+m_fD;

      m_Aabb.vcCenter = (m_Aabb.vcMax + m_Aabb.vcMin)/2.0f;
      }
   } // AddBoundingBox
/*----------------------------------------------------------------*/


// render the bounding box of the selected stuff
void CSelectionBuffer::RenderBoundingBox(ZFXRenderDevice *pDevice) {
   CGeometry geo;
   CPolygon  pPolys[6];

   geo.CreateCube(NULL, pPolys, m_Aabb.vcCenter,
                  m_Aabb.vcMax.x - m_Aabb.vcMin.x,
                  m_Aabb.vcMax.y - m_Aabb.vcMin.y,
                  m_Aabb.vcMax.z - m_Aabb.vcMin.z);

   pDevice->SetShadeMode(RS_SHADE_LINES, 0, NULL);

   pPolys[0].Render( pDevice->GetVertexManager(), true );
   pPolys[1].Render( pDevice->GetVertexManager(), true );
   pPolys[2].Render( pDevice->GetVertexManager(), true );
   pPolys[3].Render( pDevice->GetVertexManager(), true );
   pPolys[4].Render( pDevice->GetVertexManager(), true );
   pPolys[5].Render( pDevice->GetVertexManager(), true );
   } // RenderBoundingBox
/*----------------------------------------------------------------*/


// test if the given point is inside the aabb along given axis
bool CSelectionBuffer::IsInside(const ZFXVector &vc, ZFXAXIS axis) {

   if (axis == X_AXIS) {
      if (vc.y < m_Aabb.vcMin.y) return false;
      if (vc.y > m_Aabb.vcMax.y) return false;
      if (vc.z < m_Aabb.vcMin.z) return false;
      if (vc.z > m_Aabb.vcMax.z) return false;
      }
   else if (axis == Y_AXIS) {
      if (vc.x < m_Aabb.vcMin.x) return false;
      if (vc.x > m_Aabb.vcMax.x) return false;
      if (vc.z < m_Aabb.vcMin.z) return false;
      if (vc.z > m_Aabb.vcMax.z) return false;
      }
   else {
      if (vc.x < m_Aabb.vcMin.x) return false;
      if (vc.x > m_Aabb.vcMax.x) return false;
      if (vc.y < m_Aabb.vcMin.y) return false;
      if (vc.y > m_Aabb.vcMax.y) return false;
      }
   return true;
   } // IsInside
/*----------------------------------------------------------------*/


// translate all selected stuff by the given vector
void CSelectionBuffer::Translate(const ZFXVector &vcT) {
   CPolymesh *pMesh=NULL;
   bool bActionPerformed=false;
   UINT i=0;

   // loop through all selected objects
   for (i=0; i<m_NumLobs; i++) {
      m_ppLob[i]->Translate(vcT);
      bActionPerformed = true;
      } // for

   // translate vertices the hard way
   for (i=0; i<m_NumVerts; i++) {
      m_pVerts[i].pV->x += vcT.x;
      m_pVerts[i].pV->y += vcT.y;
      m_pVerts[i].pV->z += vcT.z;
      m_pVerts[i].pPoly->CalcBoundingBox();
      
      // don't forget to adjust its aabb now
      if ( pMesh = m_pVerts[i].pPoly->GetParent() )
         pMesh->CalcBoundingBox();
      
      bActionPerformed = true;
      }

   // translate aabb if action performed
   if (bActionPerformed) {
      m_Aabb.vcCenter += vcT;
      m_Aabb.vcMin    += vcT;
      m_Aabb.vcMax    += vcT;
      }
   } // Translate
/*----------------------------------------------------------------*/


// rotate vertices, polygons and meshs
void CSelectionBuffer::Rotate(ZFXAXIS axis, float a) {
   ZFXVector vc = m_Aabb.vcCenter;

   // loop through all selected objects
   for (UINT i=0; i<m_NumLobs; i++) {
      
      if (m_ppLob[i]->IsOfType(LOB_MESH)) {
         ((LPPOLYMESH)m_ppLob[i])->Rotate(axis, vc, a);
         AddBoundingBox( m_ppLob[i]->GetAabb() );
         }

      else if (m_ppLob[i]->IsOfType(LOB_POLYGON)) {
         ((LPPOLYGON)m_ppLob[i])->Rotate(axis, vc, a);
         AddBoundingBox( m_ppLob[i]->GetAabb() );
         }
      } // for

   } // Rotate
/*----------------------------------------------------------------*/


// mirror polygons and meshs
void CSelectionBuffer::Mirror(ZFXAXIS axis) {
   ZFXVector vc = m_Aabb.vcCenter;

   // loop through all selected objects
   for (UINT i=0; i<m_NumLobs; i++) {
      
      if (m_ppLob[i]->IsOfType(LOB_MESH)) {
         ((LPPOLYMESH)m_ppLob[i])->Mirror(axis, vc);
         AddBoundingBox( m_ppLob[i]->GetAabb() );
         }

      else if (m_ppLob[i]->IsOfType(LOB_POLYGON)) {
         ((LPPOLYGON)m_ppLob[i])->Mirror(axis, vc);
         AddBoundingBox( m_ppLob[i]->GetAabb() );
         }
      } // for

   } // Mirror
/*----------------------------------------------------------------*/


// remap the texture coordinates for selected stuff
void CSelectionBuffer::TextureRemap(ZFXAXIS axis) {
   ZFXAabb aabb;

   aabb.vcMax    = m_Aabb.vcMax - m_fD;
   aabb.vcMin    = m_Aabb.vcMin + m_fD;

   // loop through all selected objects
   for (UINT i=0; i<m_NumLobs; i++) {
      if (m_ppLob[i]->GetType() == LOB_MESH) {
         ((LPPOLYMESH)m_ppLob[i])->CalcTextureCoords(axis, &aabb);
         }

      else if (m_ppLob[i]->GetType() == LOB_POLYGON) {
         ((LPPOLYGON)m_ppLob[i])->CalcTextureCoords(axis, &aabb);
         }
      } // for
   } // TextureRemap
/*----------------------------------------------------------------*/


// transform all texture coordinates by the given values
void CSelectionBuffer::TransTextureCoords(float ftU, float ftV, 
                                          float frU, float frV) {
   // brute force prevention of zero repeat
   if ( (frU < 0.0001f) && (frU > -0.0001f)) frU = 0.0001f;
   if ( (frV < 0.0001f) && (frV > -0.0001f)) frV = 0.0001f;

   // loop through all selected objects
   for (UINT i=0; i<m_NumLobs; i++) {
      if (m_ppLob[i]->GetType() == LOB_MESH)
         ((LPPOLYMESH)m_ppLob[i])->TransTextureCoords(ftU,ftV,frU,frV);

      else if (m_ppLob[i]->GetType() == LOB_POLYGON)
         ((LPPOLYGON)m_ppLob[i])->TransTextureCoords(ftU,ftV,frU,frV);
      } // for
   } // TransTextureCoors
/*----------------------------------------------------------------*/


// returns true if values are the same for all objects in buffer
bool CSelectionBuffer::GetTextureTrans(float *pftU, float *pftV,
                                       float *pfrU, float *pfrV) {
   bool b=true;
   
   float ftU=0.0f, ftV=0.0f, frU=0.0f, frV=0.0f;

   // loop through all selected objects
   for (UINT i=0; i<m_NumLobs; i++) {
      if (m_ppLob[i]->GetType() == LOB_MESH)
         ((LPPOLYMESH)m_ppLob[i])->GetTextureTrans(&ftU,&ftV,&frU,&frV);

      else if (m_ppLob[i]->GetType() == LOB_POLYGON)
         ((LPPOLYGON)m_ppLob[i])->GetTextureTrans(&ftU,&ftV,&frU,&frV);
      else
         continue;

      if (i>0) {
         if ( (*pftU != ftU) || (*pftV != ftV) ||
              (*pfrU != frU) || (*pfrV != frV) )
            return false;
         }
      // store values
      *pftU = ftU;  *pftV = ftV;
      *pfrU = frU;  *pfrV = frV;
      } // for

   return true;
   } // GetTextureTrans
/*----------------------------------------------------------------*/


// set the skins of all selected polygons and meshs to the given id
void CSelectionBuffer::SetSkinTo(UINT nID) {

   // loop through all selected objects
   for (UINT i=0; i<m_NumLobs; i++) {

      if (m_ppLob[i]->GetType() == LOB_MESH)
         ((LPPOLYMESH)m_ppLob[i])->SetSkinID(nID);

      else if (m_ppLob[i]->GetType() == LOB_POLYGON)
         ((LPPOLYGON)m_ppLob[i])->SetSkinID(nID);

      else continue;
      }
   } // SetSkinTo
/*----------------------------------------------------------------*/


// save all selected stuff as list of polygons as prefab
void CSelectionBuffer::SaveAsPrefab(const char *ch) {
   UINT nNumPolys=0;
   UINT i=0, n=0, SkinID=0;
   char buffer[2048];

   FILE *pPrefab = fopen(ch, "wb");

   // just save polys, now count 'em first
   for (i=0; i<m_NumLobs; i++) {
      if (m_ppLob[i]->GetType() == LOB_POLYGON)
         nNumPolys++;
      else if (m_ppLob[i]->GetType() == LOB_MESH)
         nNumPolys += m_ppLob[i]->GetNumPolys();
      }

   // write number of polys
   fwrite(&nNumPolys, sizeof(UINT), 1, pPrefab);

   // loop through all selected objects
   for (i=0; i<m_NumLobs; i++) {
      if (m_ppLob[i]->GetType() == LOB_MESH) {
         ((LPPOLYMESH)m_ppLob[i])->SavePurePolys(pPrefab);
         }

      else if (m_ppLob[i]->GetType() == LOB_POLYGON) {
         // get the basic texture name for this poly
         SkinID = ((LPPOLYGON)m_ppLob[i])->GetSkinID();
         SendMessage(CApplication::m_G.hGUI[CSK_LIST],
                     CB_GETLBTEXT, SkinID, 
                     (LPARAM)(LPCSTR)buffer);
         // save string length and string to file
         n = strlen(buffer);
         fwrite(&n, sizeof(UINT), 1, pPrefab);
         fwrite(buffer, n, 1, pPrefab);

         ((LPPOLYGON)m_ppLob[i])->Save(pPrefab);
         }
      }

   fclose(pPrefab);
   } // SetSkinTo
/*----------------------------------------------------------------*/


// save all selected stuff as list of polygons as prefab
void CSelectionBuffer::Snap(void) {
   ZFXVector vc;

   for (UINT i=0; i<m_NumVerts; i++) {
      vc.Set(m_pVerts[i].pV->x, 
             m_pVerts[i].pV->y, 
             m_pVerts[i].pV->z);
      CGeometry::Snap(&vc, 1.0f);
      m_pVerts[i].pV->x = vc.x;
      m_pVerts[i].pV->y = vc.y;
      m_pVerts[i].pV->z = vc.z;
      }
   } // Snap
/*----------------------------------------------------------------*/


// get a pointer to the first element if that exists
LPLEVELOBJECT CSelectionBuffer::GetHeadPointer(void) {
   if (m_NumLobs < 1) return NULL;

   return m_ppLob[0];
   } // GetHeadPointer
/*----------------------------------------------------------------*/