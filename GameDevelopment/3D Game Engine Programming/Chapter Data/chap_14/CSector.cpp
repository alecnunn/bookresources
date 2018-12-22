// FILE: CSector.cpp

#include "include/CSector.h"
#include "include/CSelectionBuffer.h"
#include "include/CApplication.h"



// helper routine
ZFXMatrix g_CalcTransAttenNoRot(const ZFXVector &vcPos, float fRadius) {
   ZFXMatrix mA, mS, mTL, mB2, mTP, mX;

   float invRad = 0.5f / fRadius;

   mS.Identity();
   mB2.Identity();
   mTL.Identity();

   mS._11 = mS._22 = mS._33 = invRad;
   mTL.Translate(-vcPos.x, -vcPos.y, -vcPos.z);
   mB2.Translate(0.5f, 0.5f, 0.5f);

   mA = mTL * mS;
   mX = mA * mB2;

   mTP.TransposeOf( mX );
   return mTP;
   } // helper routine
/*----------------------------------------------------------------*/


// constructor
CSector::CSector(const char *chName) {
   Init();
   m_ID = 0;
   SetName(chName);
   } // constructor
/*----------------------------------------------------------------*/


// destructor
CSector::~CSector(void) {
   Release();
   } // destructor
/*----------------------------------------------------------------*/


// initialize attributes
void CSector::Init(void) {
   m_NumVerts     = 0;
   m_NumIndis     = 0;
   m_NumPorts     = 0;
   m_NumPolys     = 0;
   m_NumMeshs     = 0;
   m_NumPsInMeshs = 0;
   m_NumLights    = 0;
   m_NumEntys     = 0;
   m_NumSpawns    = 0;
   m_bHidden      = false;
   m_bSelected    = false;
   m_pC           = NULL;
   m_ppLob        = NULL;
   m_ppPoly       = NULL;
   m_ppMesh       = NULL;
   m_ppPortal     = NULL;
   m_ppLight      = NULL;
   m_ppEntity     = NULL;
   m_ppSpawn      = NULL;
   m_Type         = LOB_SECTOR;
   m_Ambient.fR   = 1.0f;
   m_Ambient.fG   = 1.0f;
   m_Ambient.fB   = 1.0f;
   m_Ambient.fA   = 1.0f;
   } // Init
/*----------------------------------------------------------------*/


// free memory and stuff
void CSector::Release(void) {
   UINT i=0;

   // free objects
   for (i=0; i<m_NumPolys; i++)
      SAFE_DELETE(m_ppPoly[i]);

   for (i=0; i<m_NumMeshs; i++)
      SAFE_DELETE(m_ppMesh[i]);

   for (i=0; i<m_NumPorts; i++)
      SAFE_DELETE(m_ppPortal[i]);

   for (i=0; i<m_NumLights; i++)
      SAFE_DELETE(m_ppLight[i]);

   for (i=0; i<m_NumEntys; i++)
      SAFE_DELETE(m_ppEntity[i]);

   for (i=0; i<m_NumSpawns; i++)
      SAFE_DELETE(m_ppSpawn[i]);

   // free list memory
   SAFE_FREE(m_ppPoly);
   SAFE_FREE(m_ppMesh);
   SAFE_FREE(m_ppPortal);
   SAFE_FREE(m_ppLight);
   SAFE_FREE(m_ppEntity);
   SAFE_FREE(m_ppSpawn);
   } // Release
/*----------------------------------------------------------------*/


// set the name member of this sector
void CSector::SetName(const char *chName) {
   int nL = strlen(chName) +1;

   if (nL>256) memcpy(m_chName, chName, 256);
   else memcpy(m_chName, chName, nL);
   } // SetName
/*----------------------------------------------------------------*/


// get the name member of this sector
void CSector::GetName(char *chName) {
   memcpy(chName, m_chName, strlen(m_chName)+1);
   } // GetName
/*----------------------------------------------------------------*/


// checks if sector's name is equal to parameter string
bool CSector::NameEqual(const char *chName) {
   if (strnicmp(chName, m_chName, 65535)==0)
      return true;
   return false;
   } // NameEqual
/*----------------------------------------------------------------*/
  

// set class's temporary pointer to appropriate list and counter
bool CSector::SetPointers(SELECTMODE sm) {
   if (sm==SM_POLYGON) {
      m_pC = &m_NumPolys;
      m_ppLob = (LPLEVELOBJECT*)m_ppPoly;
      }
   else if (sm==SM_MESH) {
      m_pC = &m_NumMeshs;
      m_ppLob = (LPLEVELOBJECT*)m_ppMesh;
      }
   else if (sm==SM_PORTAL) {
      m_pC = &m_NumPorts;
      m_ppLob = (LPLEVELOBJECT*)m_ppPortal;
      }
   else if (sm==SM_LIGHT) {
      m_pC = &m_NumLights;
      m_ppLob = (LPLEVELOBJECT*)m_ppLight;
      }
   else if (sm==SM_ENTITY) {
      m_pC = &m_NumEntys;
      m_ppLob = (LPLEVELOBJECT*)m_ppEntity;
      }
   else if (sm==SM_SPAWN) {
      m_pC = &m_NumSpawns;
      m_ppLob = (LPLEVELOBJECT*)m_ppSpawn;
      }
   else return false;
   return true;
   } // SetPointers
/*----------------------------------------------------------------*/


// the same again but not with SELECTMODE but with numerical param
bool CSector::SetPointers(UINT t) {
   if ( (t <= 0) || (t >= 6)) return false;

   else if (t == 1) {
      m_pC = &m_NumPolys;
      m_ppLob = (LPLEVELOBJECT*)m_ppPoly;
      }
   else if (t == 2) {
      m_pC = &m_NumPorts;
      m_ppLob = (LPLEVELOBJECT*)m_ppPortal;
      }
   else if (t == 3) {
      m_pC = &m_NumLights;
      m_ppLob = (LPLEVELOBJECT*)m_ppLight;
      }
   else if (t == 4) {
      m_pC = &m_NumEntys;
      m_ppLob = (LPLEVELOBJECT*)m_ppEntity;
      }
   else if (t == 5) {
      m_pC = &m_NumSpawns;
      m_ppLob = (LPLEVELOBJECT*)m_ppSpawn;
      }

   return true;
   } // SetPointers
/*----------------------------------------------------------------*/


// try to pick the closest object in the given select mode
void CSector::Pick(SELECTMODE sm, const ZFXRay &Ray, CSelectionBuffer *pSB) {
   LPLEVELOBJECT  pPicked=NULL, pPoly = NULL;

   float fMin=65535.0f, fD=0.0f;

   // polygon is a special case which
   // is treated again below

   if (!SetPointers(sm)) return;

   // loop through all objects in list
   for (UINT i=0; i<(*m_pC); i++) {
      // don't test hidden stuff
      if (m_ppLob[i]->IsHidden()) continue;

      // try to pick this one
      if (m_ppLob[i]->Picked(Ray, &fD)) {
         if (fD < fMin) {
            fMin = fD;
            pPicked = m_ppLob[i];
            }
         }
      }

   // if we should pick polys we need to
   // pick for polys in the meshs, too
   if (sm==SM_POLYGON) {
      for (i=0; i<m_NumMeshs; i++) {
         if (m_ppMesh[i]->IsHidden()) continue;

         if ( pPoly = m_ppMesh[i]->PickPoly(Ray, fMin, &fD))  {
            fMin = fD;
            pPicked = pPoly;
            pPoly = NULL;
            }
         }
      } // if [POLYGON]

   // if we picked up something add it
   if (pPicked) pSB->AddLob(pPicked);
   } // Pick
/*----------------------------------------------------------------*/


// save this sector to the given file stream
void CSector::Save(FILE *pFile) {
   UINT i=0, NumSkins=0;
   int  NewSkin=-1;
   int  n=0;

   UINT aSkins[400];

   // rework list of free polygons
   if (m_NumPolys > 0) {
      // sort free polygons by texture
      qsort(m_ppPoly, m_NumPolys, sizeof(LPPOLYGON), SortBySkin);

      // count number of skins in this sector for free polys
      UINT OldSkin = m_ppPoly[0]->GetSkinID();
      aSkins[NumSkins++] = OldSkin;
      for (i=1; i<m_NumPolys; i++) {
         NewSkin = m_ppPoly[i]->GetSkinID();
         if (NewSkin != OldSkin) {
            aSkins[NumSkins++] = NewSkin;
            }
         OldSkin = NewSkin;
         }
      }

   // count number of skins used for meshs
   for (UINT m=0; m<m_NumMeshs; m++) {
      m_ppMesh[m]->CountNonRedundantSkins(aSkins, &NumSkins);
      }

   // write name
   n = strlen(m_chName);
   fwrite(&n, sizeof(int), 1, pFile);
   fwrite(m_chName, n, 1, pFile);

   // write counts
   fwrite(&NumSkins,       sizeof(UINT),    1, pFile);
   fwrite(&m_NumVerts,     sizeof(UINT),    1, pFile);
   fwrite(&m_NumIndis,     sizeof(UINT),    1, pFile);
   fwrite(&m_NumPolys,     sizeof(UINT),    1, pFile);
   fwrite(&m_NumPsInMeshs, sizeof(UINT),    1, pFile);
   fwrite(&m_NumMeshs,     sizeof(UINT),    1, pFile);
   fwrite(&m_NumPorts,     sizeof(UINT),    1, pFile);
   fwrite(&m_NumLights,    sizeof(UINT),    1, pFile);
   fwrite(&m_NumEntys,     sizeof(UINT),    1, pFile);
   fwrite(&m_NumSpawns,    sizeof(UINT),    1, pFile);
   fwrite(&m_Aabb,         sizeof(ZFXAabb), 1, pFile);
   fwrite(&m_Ambient,      sizeof(ZFXCOLOR),1, pFile);
   fflush(pFile);

   // write complete polymeshes
   for (i=0; i<m_NumMeshs; i++)
      m_ppMesh[i]->Save(pFile);

   // write standalone polygons
   for (i=0; i<m_NumPolys; i++)
      m_ppPoly[i]->Save(pFile);

   // write all sector's portals
   for (i=0; i<m_NumPorts; i++)
      m_ppPortal[i]->Save(pFile);

   // write all sectors lights
   for (i=0; i<m_NumLights; i++)
      m_ppLight[i]->Save(pFile);

   // write all sectors entities
   for (i=0; i<m_NumEntys; i++)
      m_ppEntity[i]->Save(pFile);

   // write all sectors spawnpoints
   for (i=0; i<m_NumSpawns; i++)
      m_ppSpawn[i]->Save(pFile);
   } // Save
/*----------------------------------------------------------------*/


// load this sector to the given file stream
bool CSector::Load(FILE *pFile) {
   UINT i=0, NumSkins=0;
   int  n=0;

   // read name
   fread(&n, sizeof(int), 1, pFile);
   fread(m_chName, n, 1, pFile);
   m_chName[n] = '\0';

   // read counts
   fread(&NumSkins,       sizeof(UINT),    1, pFile);
   fread(&m_NumVerts,     sizeof(UINT),    1, pFile);
   fread(&m_NumIndis,     sizeof(UINT),    1, pFile);
   fread(&m_NumPolys,     sizeof(UINT),    1, pFile);
   fread(&m_NumPsInMeshs, sizeof(UINT),    1, pFile);
   fread(&m_NumMeshs,     sizeof(UINT),    1, pFile);
   fread(&m_NumPorts,     sizeof(UINT),    1, pFile);
   fread(&m_NumLights,    sizeof(UINT),    1, pFile);
   fread(&m_NumEntys,     sizeof(UINT),    1, pFile);
   fread(&m_NumSpawns,    sizeof(UINT),    1, pFile);
   fread(&m_Aabb,         sizeof(ZFXAabb), 1, pFile);
   fread(&m_Ambient,      sizeof(ZFXCOLOR),1, pFile);
 
   // allocate memory needed in 50 piece packages
   m_ppMesh   = (LPPOLYMESH*)  InitMem( (LPVOID*)m_ppMesh,
                               m_NumMeshs, sizeof(LPPOLYMESH));
   m_ppPoly   = (LPPOLYGON*)   InitMem( (LPVOID*)m_ppPoly,
                               m_NumPolys, sizeof(LPPOLYGON));
   m_ppPortal = (LPPORTAL*)    InitMem( (LPVOID*)m_ppPortal,
                               m_NumPorts, sizeof(LPPORTAL));
   m_ppLight  = (LPLIGHT*)     InitMem( (LPVOID*)m_ppLight,
                               m_NumLights, sizeof(LPLIGHT));
   m_ppEntity = (LPENTITY*)    InitMem( (LPVOID*)m_ppEntity,
                               m_NumEntys, sizeof(LPENTITY));
   m_ppSpawn  = (LPSPAWNPOINT*)InitMem( (LPVOID*)m_ppSpawn,
                               m_NumSpawns, sizeof(LPSPAWNPOINT));

   // load complete polymeshes
   for (i=0; i<m_NumMeshs; i++) {
      m_ppMesh[i] = new CPolymesh();
      if ( !m_ppMesh[i]->Load(pFile) ) {
         MessageBox(CApplication::m_G.hWnd,
                    "Critical error on loading meshs.",
                    "PanBox Edit - Error", 
                    MB_OK | MB_ICONERROR);
         delete m_ppMesh[i];
         m_NumMeshs  = i;
         m_NumPolys  = 0;
         m_NumPorts  = 0;
         m_NumLights = 0;
         m_NumEntys  = 0;
         return false;
         }
      }

   // load standalone poygons
   for (i=0; i<m_NumPolys; i++) {
      m_ppPoly[i] = new CPolygon();
      if ( !m_ppPoly[i]->Load(pFile) ) {
         MessageBox(CApplication::m_G.hWnd,
                    "Critical error on loading polygons.",
                    "PanBox Edit - Error", 
                    MB_OK | MB_ICONERROR);
         delete m_ppPoly[i];
         m_NumPolys = i;
         m_NumPorts  = 0;
         m_NumLights = 0;
         m_NumEntys  = 0;
         return false;
         }
      }

   // load all sector's portals
   for (i=0; i<m_NumPorts; i++) {
      m_ppPortal[i] = new CPortal();
      if ( !m_ppPortal[i]->Load(pFile) ) {
         MessageBox(CApplication::m_G.hWnd,
                    "Critical error on loading portals.",
                    "PanBox Edit - Error", 
                    MB_OK | MB_ICONERROR);
         delete m_ppPortal[i];
         m_NumPorts = i;
         m_NumLights = 0;
         m_NumEntys  = 0;
         return false;
         }
      }

   // load all sector's lights
   for (i=0; i<m_NumLights; i++) {
      m_ppLight[i] = new CLight();
      if ( !m_ppLight[i]->Load(pFile) ) {
         MessageBox(CApplication::m_G.hWnd,
                    "Critical error on loading lights.",
                    "PanBox Edit - Error", 
                    MB_OK | MB_ICONERROR);
         delete m_ppLight[i];
         m_NumLights = i;
         m_NumEntys  = 0;
         return false;
         }
      }

   // load all sector's entities
   for (i=0; i<m_NumEntys; i++) {
      m_ppEntity[i] = new CEntity();
      if ( !m_ppEntity[i]->Load(pFile) ) {
         MessageBox(CApplication::m_G.hWnd,
                    "Critical error on loading entities.",
                    "PanBox Edit - Error", 
                    MB_OK | MB_ICONERROR);
         delete m_ppEntity[i];
         m_NumEntys = i;
         return false;
         }
      }

   // load all sector's spawnpoints
   for (i=0; i<m_NumSpawns; i++) {
      m_ppSpawn[i] = new CSpawnPoint();
      if ( !m_ppSpawn[i]->Load(pFile) ) {
         MessageBox(CApplication::m_G.hWnd,
                    "Critical error on loading spawnpoints.",
                    "PanBox Edit - Error", 
                    MB_OK | MB_ICONERROR);
         delete m_ppSpawn[i];
         m_NumSpawns = i;
         return false;
         }
      }

   return true;
   } // Load
/*----------------------------------------------------------------*/


// loads a prefab into this sector which is just a list of polygons
void CSector::LoadPrefab(const char *ch, ZFXRenderDevice *pDevice,
                         CSelectionBuffer *pSB) {
   LPPOLYGON     pPoly = new CPolygon();
   LPLEVELOBJECT pLob = NULL;
   UINT NumPolys=0, NumErrors=0, n=0;
   int  nIndex=-1;
   char buffer[2048];

   FILE *pFile = fopen(ch, "rb");
   if (!pFile) return;

   pPoly->Reset();

   // read number of polygons to come
   fread(&NumPolys, sizeof(UINT), 1, pFile);

   if (NumPolys == 0) return;

   for (UINT i=0; i<NumPolys; i++) {
      pPoly->Reset();
      
      // read name of base texture
      fread(&n, sizeof(UINT), 1, pFile);
      fread(buffer, n, 1, pFile);
      buffer[n] = '\0';

      // is this texture already loaded?
      nIndex = cbhContainsString(CApplication::m_G.hGUI[
                                 CSK_LIST], buffer);

      // now load the polygon data
      if (!pPoly->Load(pFile)) {
         NumErrors++;
         continue;
         }

      // load the texture if needed
      if (nIndex == -1) {
         nIndex = CApplication::m_G.Level.CreateSkinID(
                                 pDevice, buffer, 1.0f );
         cbhAddItem(CApplication::m_G.hGUI[CSK_LIST],
                    buffer, NULL, true);
         }

      pPoly->SetSkinID(nIndex);
      pLob = AddObject( *pPoly );
      pSB->AddLob( pLob );
      }

   if (NumErrors != 0) {
      sprintf(buffer, "%d corrupted polys found.", NumErrors); 
      MessageBox(CApplication::m_G.hWnd,
                 buffer, "PanBox Edit - Error", 
                 MB_OK | MB_ICONERROR);
      }

   delete pPoly;
   fclose(pFile);
   } // LoadPrefab
/*----------------------------------------------------------------*/


// render all elements of the given type, hide hidden at wish
void CSector::Render(ZFXVertexCacheManager *pVCM, 
                     DWORD Type, bool bHide) {
   UINT i=0;

   static ZFXCOLOR clrL = { 1.0f, 1.0f, 0.0f, 1.0f };
   static ZFXCOLOR clrS = { 1.0f, 0.0f, 1.0f, 1.0f };
   static ZFXRENDERSTATE sm = RS_SHADE_LINES;

   // render meshes
   if ((Type & LOB_MESH) || (Type & LOB_ALL)) {
      for (i=0; i<m_NumMeshs; i++) {
         if ( !m_ppMesh[i]->IsSelected() )
            m_ppMesh[i]->Render( pVCM, bHide );
         }
      } // type

   // render standalone poygons
   if ((Type & LOB_POLYGON) || (Type & LOB_ALL)) {
      for (i=0; i<m_NumPolys; i++) {
         if ( !m_ppPoly[i]->IsSelected() )
            m_ppPoly[i]->Render( pVCM, bHide );
         }
      } // type

   // render entities
   if ((Type & LOB_ENTITY) || (Type & LOB_ALL)) {
      for (i=0; i<m_NumEntys; i++) {
         if ( !m_ppEntity[i]->IsSelected() )
            m_ppEntity[i]->Render( pVCM, bHide );
         }
      } // type

   // render spawnpoints
   if ((Type & LOB_SPAWNPOINT) || (Type & LOB_ALL)) {
      for (i=0; i<m_NumSpawns; i++) {
         if ( !m_ppSpawn[i]->IsSelected() )
            m_ppSpawn[i]->Render( pVCM, bHide );
         }
      } // type

   // render lights which have transparency
   if ((Type & LOB_LIGHT) || (Type & LOB_ALL)) {
      for (i=0; i<m_NumLights; i++) {
         if ( !m_ppLight[i]->IsSelected() ) {
/*
            if (m_ppLight[i]->IsShadowCasting())
               m_pDevice->SetShadeMode(sm, 0, &clrS);
            else 
               m_pDevice->SetShadeMode(sm, 0, &clrL);
*/
            m_ppLight[i]->Render( pVCM, bHide );
            }
         }
      } // type

   // render portals last due to transparency
   if ((Type & LOB_PORTAL) || (Type & LOB_ALL)) {
      for (i=0; i<m_NumPorts; i++) {
         if ( !m_ppPortal[i]->IsSelected() )
            m_ppPortal[i]->Render( pVCM, bHide );
         }
      } // type
   } // Render
/*----------------------------------------------------------------*/


// render all geometry with one pass per light
void CSector::RenderLit(ZFXRenderDevice *pDevice, bool bHide) {
   ZFXMatrix matA;
   UINT i=0;

   ZFXVertexCacheManager *pVCM = pDevice->GetVertexManager();

   // loop through all lights in the scene
   for (UINT l=0; l<m_NumLights; l++) {

      if (m_ppLight[l]->IsHidden()) continue;

      matA = g_CalcTransAttenNoRot( m_ppLight[l]->GetPosition(), 
                                    m_ppLight[l]->GetRadius());

      pDevice->SetShaderConstant(SHT_VERTEX, DAT_FLOAT, 
                                 20, 4, (void*)&matA);

      pDevice->SetShaderConstant(SHT_PIXEL, DAT_FLOAT, 0, 1,
                                 (void*)&m_ppLight[l]->GetColor());

      // render meshes
      for (i=0; i<m_NumMeshs; i++) {
         if ( !m_ppMesh[i]->IsSelected() )
            m_ppMesh[i]->Render( pVCM, bHide );
         }

      // render standalone poygons
      for (i=0; i<m_NumPolys; i++) {
         if ( !m_ppPoly[i]->IsSelected() )
            m_ppPoly[i]->Render( pVCM, bHide );
         }
      
      pVCM->ForcedFlushAll();
      }
   } // RenderLit
/*----------------------------------------------------------------*/


// select just everything in this sector
void CSector::SelectAll(SELECTMODE sm, CSelectionBuffer *pSB) {
   // polygon is a special case here, as
   // there can be polygons in meshs also
   if (sm==SM_POLYGON) {
      // select all from meshs first
      for (UINT m=0; m<m_NumMeshs; m++) {
         if (m_ppMesh[m]->IsHidden()) continue;
         m_ppMesh[m]->SelectAllPolys( pSB );
         }
      }

   // then bend pointers
   if (!SetPointers(sm)) return;
   
   // loop through all objects in list
   for (UINT i=0; i<(*m_pC); i++) {
      if (m_ppLob[i]->IsHidden()) continue;
      pSB->AddLob( m_ppLob[i] );
      }
   } // SelectAll
/*----------------------------------------------------------------*/


// select anything contained in the aabb
void CSector::SelectIntersecting(SELECTMODE sm, CSelectionBuffer *pSB, 
                                 const ZFXAabb &aabb, ZFXAXIS axis) {
   UINT i=0;

   // first we have the special cases of vertices
   // which can be in both, meshs and polygons 
   if (sm == SM_VERTEX) {
      for (i=0; i<m_NumMeshs; i++) {
         if ( m_ppMesh[i]->IsHidden() ) continue;
         else m_ppMesh[i]->GetIntersectingVerts(aabb,pSB,axis);
         }
      for (i=0; i<m_NumPolys; i++) {
         if ( m_ppPoly[i]->IsHidden() ) continue;
         else m_ppPoly[i]->GetIntersectingVerts(aabb, pSB, axis);
         }
      } // special case VERTEX
   // second we have the special case of polygons which
   // can be in meshs (free polys are treated below)
   else if (sm == SM_POLYGON) {
      for (i=0; i<m_NumMeshs; i++) {
         if ( m_ppMesh[i]->IsHidden() ) continue;
         else m_ppMesh[i]->GetIntersectingPolys(aabb,pSB,axis);
         }
      } // special case POLYGON

   // now all the standard cases
   if (!SetPointers(sm)) return;
   
   // loop through all objects in list
   for (i=0; i<(*m_pC); i++) {
      if (m_ppLob[i]->IsHidden()) continue;
      if ( m_ppLob[i]->Intersects(aabb, axis) )
            pSB->AddLob( m_ppLob[i] );
      }
   } // SelectIntersecting
/*----------------------------------------------------------------*/


// select only objects into the buffer that are not yet selected
void CSector::SelectInvert(SELECTMODE sm, CSelectionBuffer *pSB) {
   UINT i=0;
 
   // special case of polygons which in meshs
   if (sm == SM_POLYGON) {
      for (i=0; i<m_NumMeshs; i++) {
         if ( m_ppMesh[i]->IsHidden() ) continue;
         else m_ppMesh[i]->SelectInvert(pSB);
         }
      }
   else if (sm == SM_VERTEX) { return; }

   // now all the standard cases
   if (!SetPointers(sm)) return;
   
   // loop through all objects in list
   for (i=0; i<(*m_pC); i++) {
      if (m_ppLob[i]->IsHidden()) continue;
      // select unselected polygons
      if ( !m_ppLob[i]->IsSelected() ) 
         pSB->AddLob( m_ppLob[i] );
      // change flag on selected ones
      else m_ppLob[i]->DeSelect();
      }
   } // SelectInvert
/*----------------------------------------------------------------*/


// adds a standalone polygon, returns it's new address
LPLEVELOBJECT CSector::AddObject(const CLevelObject &Lob) {
   // cast away const
   LPLEVELOBJECT pLob = (LPLEVELOBJECT)&Lob;

   switch ( pLob->GetType() ) {
      case LOB_POLYGON:
         // allocate memory if neccessary
         m_ppPoly = (LPPOLYGON*) CheckMem( (LPVOID*)m_ppPoly,
                     m_NumPolys, sizeof(LPPOLYGON) );

         // create a new instance for a polygon
         m_ppPoly[m_NumPolys] = new CPolygon();
         m_ppPoly[m_NumPolys]->CopyOf( pLob );

         // update counters
         m_NumVerts += pLob->GetNumVerts();
         m_NumIndis += pLob->GetNumIndis();
         m_NumPolys++;
         return m_ppPoly[m_NumPolys-1];

      case LOB_MESH:
            // allocate memory if neccessary
         m_ppMesh = (LPPOLYMESH*) CheckMem( (LPVOID*)m_ppMesh,
                     m_NumMeshs, sizeof(LPPOLYMESH) );

         // create a new instance for a polymesh
         m_ppMesh[m_NumMeshs] = new CPolymesh();
         m_ppMesh[m_NumMeshs]->CopyOf( pLob );

         // update counters
         m_NumPsInMeshs += pLob->GetNumPolys();
         m_NumVerts += pLob->GetNumVerts();
         m_NumIndis += pLob->GetNumIndis();
         m_NumMeshs++;
         return m_ppMesh[m_NumMeshs-1];

      case LOB_ENTITY:
         // allocate memory if neccessary
         m_ppEntity = (LPENTITY*) CheckMem( (LPVOID*)m_ppEntity,
                      m_NumEntys, sizeof(LPENTITY) );

         // create a new instance for a polymesh
         m_ppEntity[m_NumEntys] = new CEntity();
         m_ppEntity[m_NumEntys]->CopyOf( pLob );
         m_NumEntys++;
         return m_ppEntity[m_NumEntys-1];

      case LOB_PORTAL:
         // allocate memory if neccessary
         m_ppPortal = (LPPORTAL*) CheckMem( (LPVOID*)m_ppPortal,
                      m_NumPorts, sizeof(LPPORTAL) );

         // create a new instance for a polymesh
         m_ppPortal[m_NumPorts] = new CPortal();
         m_ppPortal[m_NumPorts]->CopyOf( pLob );
         m_ppPortal[m_NumPorts]->SetItsSector(this);
         m_NumPorts++;
         return m_ppPortal[m_NumPorts-1];

      case LOB_LIGHT:
         // allocate memory if neccessary
         m_ppLight = (LPLIGHT*) CheckMem( (LPVOID*)m_ppLight,
                     m_NumLights, sizeof(LPLIGHT) );

         // create a new instance for a polymesh
         m_ppLight[m_NumLights] = new CLight();
         m_ppLight[m_NumLights]->CopyOf( pLob );
         m_NumLights++;
         return m_ppLight[m_NumLights-1];
      
      case LOB_SPAWNPOINT:
         // allocate memory if neccessary
         m_ppSpawn = (LPSPAWNPOINT*) CheckMem( (LPVOID*)m_ppSpawn,
                     m_NumSpawns, sizeof(LPSPAWNPOINT) );

         // create a new instance for a polymesh
         m_ppSpawn[m_NumSpawns] = new CSpawnPoint();
         m_ppSpawn[m_NumSpawns]->CopyOf( pLob );
         m_NumSpawns++;
         return m_ppSpawn[m_NumSpawns-1];

      default: return NULL;
      } // switch
   } // AddObject
/*----------------------------------------------------------------*/


/**
 * Clear the hide flag of all polygons in this sector.
 */
void CSector::HideNone(void) {
   for (UINT t=0; t<6; t++) {
      // meshs are the special case here
      if (t == 0) {
         for (UINT cm=0; cm<m_NumMeshs; cm++) {
            m_ppMesh[cm]->UnHide();
            m_ppMesh[cm]->HideNoPolys();
            }
         continue;
         }

      // otherwise bend pointers
      if ( !SetPointers(t) ) continue;
   
      // loop through all objects in list
      for (UINT i=0; i<(*m_pC); i++) 
         m_ppLob[i]->UnHide();
      }
   } // HideNone
/*----------------------------------------------------------------*/


/**
 * Hide all polygons in this sector which are selected.
 */
void CSector::HideSelected(SELECTMODE sm) {
   UINT i=0;
 
   // special case of polygons which are in meshs
   if (sm == SM_POLYGON) {
      for (i=0; i<m_NumMeshs; i++)
         m_ppMesh[i]->HideSelectedPolys();
      }
   else if (sm == SM_VERTEX) { return; }

   // now all the standard cases
   if (!SetPointers(sm)) return;
   
   // loop through all objects in list
   for (i=0; i<(*m_pC); i++) {
      if ( m_ppLob[i]->IsSelected() ) 
         m_ppLob[i]->Hide();
      }
   } // HideSelected
/*----------------------------------------------------------------*/


/**
 * Hide all polygons in this sector which are not selected.
 */
void CSector::HideUnselected(void) {
   for (UINT t=0; t<6; t++) {
      // meshs are special case here due to
      // their polys which can be selected
      if (t == 0) {
         for (UINT m=0; m<m_NumMeshs; m++)
            m_ppMesh[m]->HideUnselectedPolys();
         continue;
         }
      // otherwise bend pointers
      if ( !SetPointers(t) ) continue;
   
      // loop through all objects in list
      for (UINT i=0; i<(*m_pC); i++) {
         if ( ! m_ppLob[i]->IsSelected() ) 
            m_ppLob[i]->Hide();
         }
      }
   } // HideUnselected
/*----------------------------------------------------------------*/


/**
 * Invert the hidden state of all polygons in this sector.
 */
void CSector::HideInvert(void) {
   for (UINT t=0; t<6; t++) {
      // meshs are the special case here
      if (t == 0) {
         for (UINT cm=0; cm<m_NumMeshs; cm++) {
            // unhide hidden mesh objects
            if ( m_ppMesh[cm]->IsHidden() ) {
               m_ppMesh[cm]->UnHide();
               m_ppMesh[cm]->HideNoPolys();
               }
            // invert poly state on unhidden meshs
            else m_ppMesh[cm]->HideInvertPolys();
            }
         continue;
         }

      // otherwise bend pointers
      if ( !SetPointers(t) ) continue;
   
      // loop through all objects in list
      for (UINT i=0; i<(*m_pC); i++) {
         if (m_ppLob[i]->IsHidden()) 
            m_ppLob[i]->UnHide();
         else 
            m_ppLob[i]->Hide();
         }
      }
   } // HideInvert
/*----------------------------------------------------------------*/


/**
 * Remove the given lob from the sector by deleting it totally.
 */
void CSector::RemoveObject(LPLEVELOBJECT pLob) {
   LPPOLYMESH pMesh=NULL;

   LPLEVELOBJECT *ppLob = NULL;
   UINT *pC=NULL;

   // polygon is a special case here, as
   // there can be polygons in meshs also
   switch ( pLob->GetType() ) {
      case LOB_POLYGON:
         // update special counters
         m_NumVerts -= pLob->GetNumVerts();
         m_NumIndis -= pLob->GetNumIndis();

         // search the polygon in a mesh
         if ( ((LPPOLYGON)pLob)->IsPartOfMesh() ) {
            pMesh = ((LPPOLYGON)pLob)->GetParent();
            pMesh->RemovePolygon((LPPOLYGON)pLob);
            // delete empty meshs
            if ( !pMesh->GetNumPolys() )
               RemoveObject(pMesh);
            m_NumPsInMeshs--;
            return;
            }

         // otherwise bend pointers
         pC = &m_NumPolys;
         ppLob = (LPLEVELOBJECT*)m_ppPoly;
         break;
      // otherwise bend pointers to objects
      case LOB_MESH:
         // update special counters
         m_NumVerts -= pLob->GetNumVerts();
         m_NumIndis -= pLob->GetNumIndis();
         m_NumPsInMeshs -= pLob->GetNumPolys();

         pC = &m_NumMeshs;
         ppLob = (LPLEVELOBJECT*)m_ppMesh;
         break;
      case LOB_PORTAL:
         pC = &m_NumPorts;
         ppLob = (LPLEVELOBJECT*)m_ppPortal;
         break;
      case LOB_LIGHT:
         pC = &m_NumLights;
         ppLob = (LPLEVELOBJECT*)m_ppLight;
         break;
      case LOB_ENTITY:
         pC = &m_NumEntys;
         ppLob = (LPLEVELOBJECT*)m_ppEntity;
         break;
      case LOB_SPAWNPOINT:
         pC = &m_NumSpawns;
         ppLob = (LPLEVELOBJECT*)m_ppSpawn;
         break;
      default: return;
      } // switch
   
   // loop through all objects in list
   for (UINT i=0; i<(*pC); i++)
      if (ppLob[i] == pLob) break;
      
   // did we find the one we came for?
   if (i>=(*pC)) return;

   SAFE_DELETE(ppLob[i]);

   // copy last one to this position
   if (i != ((*pC)-1)) ppLob[i] = ppLob[(*pC)-1];
   (*pC)--;
   return;
   } // RemoveObject
/*----------------------------------------------------------------*/


UINT CSector::GetNumVertsM(void) {
   UINT Num=0;

   for (UINT i=0; i<m_NumMeshs; i++)
      Num += m_ppMesh[i]->GetNumVerts();
      
   return Num;
   } // GetNumVertsM
/*----------------------------------------------------------------*/


UINT CSector::GetNumIndisM(void) {
   UINT Num=0;

   for (UINT i=0; i<m_NumMeshs; i++)
      Num += m_ppMesh[i]->GetNumIndis();
      
   return Num;
   } // GetNumIndisM
/*----------------------------------------------------------------*/


// recalculate normals for polygon and mesh vertex normals
void CSector::RecalcVertexNormals(void) {
   for (UINT i=0; i<m_NumMeshs; i++)
      m_ppMesh[i]->CalcNormals();

   for (UINT j=0; j<m_NumPolys; j++)
      m_ppPoly[j]->CalcNormals();
   } // RecalcVertexNormals
/*----------------------------------------------------------------*/


/**
 * Remove all connections to this portal in this sector. For now
 * only one other portal and an arbitrary number of door entities
 * could have a connection to this portal.
 */
void CSector::RemovePortalConnections(UINT id) {
   
   for (UINT e=0; e<m_NumEntys; e++) 
      m_ppEntity[e]->RemovePortalConnections( id );

   for (UINT p=0; p<m_NumPorts; p++) 
      m_ppPortal[p]->DeconnectFrom( id );

   } // RemovePortalConnections
/*----------------------------------------------------------------*/


/**
 * Establishes all connections to the new portal, which means that
 * all other portals and all door entities are checked whether they
 * intersect the new portal and if they do saves the id for them.
 */
void CSector::NewPortalInTown(CPortal *pPortal) {
   ZFXAabb aabb1 = pPortal->GetAabb();
   ZFXAabb aabb2;

   for (UINT e=0; e<m_NumEntys; e++) {
      aabb2 = m_ppEntity[e]->GetAabb();

      if ( aabb1.Intersects(aabb2) )
         m_ppEntity[e]->ConnectTo( pPortal );
      }

   for (UINT p=0; p<m_NumPorts; p++) {
      aabb2 = m_ppPortal[p]->GetAabb();
      
      if ( aabb1.Intersects(aabb2) )
         m_ppPortal[p]->SetItsTarget( pPortal );
      }

   } // NewPortalInTown
/*----------------------------------------------------------------*/


/**
 * Checks the new door entity against all existing portals and checks
 * if the new door must be connected to one of the portals.
 */
void CSector::NewDoorInTown(CEntity *pDoor) {
   ZFXAabb aabb1 = pDoor->GetAabb();
   ZFXAabb aabb2;

   for (UINT p=0; p<m_NumPorts; p++) {
      aabb2 = m_ppPortal[p]->GetAabb();
      
      if ( aabb1.Intersects(aabb2) )
         pDoor->ConnectTo( m_ppPortal[p] );
      }

   } // NewDoorInTown
/*----------------------------------------------------------------*/