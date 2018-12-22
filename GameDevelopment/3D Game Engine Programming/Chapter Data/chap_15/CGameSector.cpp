// FILE: CGameSector.cpp

#include "include/CGameSector.h"
#include "include/zfx.h"


#define ONSCREEN(x)    m_pDevice->BeginRendering(true,true,true); \
                       m_pDevice->DrawText(0,10,10,255,255,255,chName); \
                       m_pDevice->DrawText(0,10,25,255,255,255,x); \
                       m_pDevice->EndRendering();

/*----------------------------------------------------------------*/

int SortBySkin(const void* arg1, const void* arg2) {
   POLYGON *p1 = (POLYGON*)arg1;
   POLYGON *p2 = (POLYGON*)arg2;
   
   if( p1->SkinID > p2->SkinID )   return -1;
   if( p1->SkinID < p2->SkinID )   return +1;
   return 0;
   } // SortBySkin
/*----------------------------------------------------------------*/

// constructor
CGameSector::CGameSector(ZFXRenderDevice *pDevice, UINT Index, 
                         CGameLevel *pLevel) {
   m_pDevice          = pDevice;
   m_pLevel           = pLevel;
   m_BufID            = 0;
   m_NumSkins         = 0;
   m_NumVerts         = 0;
   m_NumIndis         = 0;
   m_NumPorts         = 0;
   m_NumEntys         = 0;
   m_NumLights        = 0;
   m_NumSpawns        = 0;
   m_NumMeshs         = 0;
   m_NumZFXPolys      = 0;
   m_NumPolys         = 0;
   m_Count            = 0;
   m_bLitRendered     = false;
   m_bDepthRendered   = false;
   m_bShadowsRendered = false;
   m_Index            = Index;

   m_OmniVSH    = 0;
   m_OmniPSH    = 0;
   m_BaseVSH    = 0;
   m_BasePSH    = 0;

   m_pSpawns      = NULL;
   m_pPolys       = NULL;
   m_pZFXPolys    = NULL;
   m_pOctree      = NULL;
   m_pVerts       = NULL;
   m_pIndis       = NULL;
   m_pBuffer      = NULL;
   m_pLights      = NULL;
   m_pPorts       = NULL;
   m_pMeshShadows = NULL;
   m_ppEntities   = NULL;
   } // constructor
/*----------------------------------------------------------------*/


// destructor
CGameSector::~CGameSector(void) {

   for (UINT i=0; i<m_NumPolys; i++) {
      if (m_pPolys[i].pVerts)
         SAFE_DELETE_A(m_pPolys[i].pVerts);
      if (m_pPolys[i].pIndis)
         SAFE_DELETE_A(m_pPolys[i].pIndis);
      }

   for (UINT j=0; j<m_NumEntys; j++) {
      SAFE_DELETE(m_ppEntities[j]);
      }
      
   SAFE_DELETE_A(m_ppEntities);
   SAFE_DELETE_A(m_pZFXPolys);
   SAFE_DELETE_A(m_pVerts);
   SAFE_DELETE_A(m_pIndis);
   SAFE_DELETE_A(m_pBuffer);
   SAFE_DELETE_A(m_pLights);
   SAFE_DELETE_A(m_pMeshShadows);
   SAFE_DELETE_A(m_pPorts);
   SAFE_DELETE_A(m_pSpawns);

   SAFE_FREE(m_pPolys);
   SAFE_DELETE(m_pOctree);
   } // destructor
/*----------------------------------------------------------------*/


/**
 * Just remember the shaders used for rendering the level geometry.
 */
void CGameSector::ReceiveShaders(UINT BaseVSH, UINT BasePSH,
                                 UINT OmniVSH, UINT OmniPSH) {
   m_BaseVSH = BaseVSH;
   m_BasePSH = BasePSH;
   m_OmniVSH = OmniVSH;
   m_OmniPSH = OmniPSH;
   } // ReceiveShaders
/*----------------------------------------------------------------*/


/**
 * Load the sector from an open level file. See editor for file
 * format (loading method there cause it is nearly the same).
 */
bool CGameSector::Load(FILE *pFile) {
   WORD      *pDummyWORD = NULL;
   UINT       NumPolys   = 0;
   UINT       NumPolysM  = 0;
   UINT       nSkin      = 0;
   UINT       NumV=0, NumI=0;
   int        nName      = 0;
   char       chName[2048];

   if (!pFile) return false;

   fread(&nName, sizeof(int), 1, pFile);
   fread(chName, nName, 1, pFile);
   chName[nName] = '\0';

   ONSCREEN("reading sector data...");

   fread(&m_NumSkins,  sizeof(UINT),    1, pFile);
   fread(&m_NumVerts,  sizeof(UINT),    1, pFile);
   fread(&m_NumIndis,  sizeof(UINT),    1, pFile);
   fread(&NumPolys,    sizeof(UINT),    1, pFile);
   fread(&NumPolysM,   sizeof(UINT),    1, pFile);
   fread(&m_NumMeshs,  sizeof(UINT),    1, pFile);
   fread(&m_NumPorts,  sizeof(UINT),    1, pFile);
   fread(&m_NumLights, sizeof(UINT),    1, pFile);
   fread(&m_NumEntys,  sizeof(UINT),    1, pFile);
   fread(&m_NumSpawns, sizeof(UINT),    1, pFile);
   fread(&m_Aabb,      sizeof(ZFXAabb), 1, pFile);
   fread(&m_Ambient,   sizeof(ZFXCOLOR),1, pFile);

   // allocate memory for lists
   m_pVerts  = new VERTEX[m_NumVerts];
   m_pIndis  = new WORD[m_NumIndis];
   m_pBuffer = new SECBUF[m_NumSkins];

   // save number of total polygons
   m_NumZFXPolys = NumPolysM + NumPolys;

   // allocate memory for upcoming stuff
   m_pZFXPolys = new ZFXPolygon[m_NumZFXPolys];


   // ###### SUCK IN ALL THAT NASTY CRAP ######

   if ( !LoadMeshs(pFile) ) return false;

   // load standalone polygons
   for (UINT p=0; p<NumPolys; p++) {
      if ( !LoadSinglePolygon(pFile) )
         return false;
      } // for

   // load the portals from the file
   if ( !LoadPortals(pFile) ) return false;

   // load the lights from the file
   if ( !LoadLights(pFile) ) return false;

   // load entities from the file
   if ( !LoadEntities(pFile) ) return false;

   // load spawnpoints from the file
   if ( !LoadSpawns(pFile) ) return false;

   ONSCREEN("creating sector octree...");

   // finally build the octree for this sector
   if (FAILED( CreateOctree() ))
      return false;

   if ( FAILED( CompilePolygonList() ))
      return false;

   // done with 'em
   SAFE_DELETE_A(m_pZFXPolys);
//   SAFE_DELETE_A(m_pVerts);
//   SAFE_DELETE_A(m_pIndis);
   return true;
   } // Load
/*----------------------------------------------------------------*/
 

/** 
 * Suck in all meshs from the level file.
 */
bool CGameSector::LoadMeshs(FILE *pFile) {
   UINT NumPolys=0;
   UINT StartPoly=0;
   UINT VertexBase=0;
   UINT NumV=0, NumI=0;
   ZFXAabb Aabb;

   VERTEX *pV=NULL;
   WORD   *pI=NULL;

   if (m_NumMeshs < 1) return true;

   m_pMeshShadows = new CShadowVolume[m_NumMeshs];

   for (UINT m=0; m<m_NumMeshs; m++) {
      // mark the first one
      StartPoly  = m_NumPolys;

      // 1. load the mesh header
      fread(&NumPolys, sizeof(UINT), 1, pFile);
      fread(&Aabb, sizeof(ZFXAabb), 1, pFile);

      // 2. load the meshs' polygons as single
      //    polygons into normal geometry list
      for (UINT pm=0; pm<NumPolys; pm++) {
         if ( !LoadSinglePolygon(pFile) )
            return false;
         }

      UINT _NumV=0, _NumI=0;

      // loop through all polys to extract vertex
      // data to get a complete list of this mesh
      for (UINT i=0, NumV=0, NumI=0; i<NumPolys; i++) {
         POLYGON *pPoly = &m_pPolys[StartPoly];

         pV = (VERTEX*)realloc(pV, sizeof(VERTEX) *
                            (NumV + pPoly[i].NumV));

         memcpy(&pV[NumV], pPoly[i].pVerts, 
                sizeof(VERTEX) * pPoly[i].NumV);
                
         NumV += pPoly[i].NumV;


         pI = (WORD*)realloc(pI, sizeof(WORD) *
                            (NumI + pPoly[i].NumI));

         // loop through all indices
         for (UINT j=0; j<pPoly[i].NumI; j++) {
            pI[NumI+j] = pPoly[i].pIndis[j] + VertexBase;
            }

         NumI += pPoly[i].NumI;
         VertexBase += pPoly[i].NumV;

         _NumV = NumV;
         _NumI = NumI;
         }

      // 3. build shadow volume for this mesh
      m_pMeshShadows[m].Init(pV, _NumV, pI, _NumI, 
                             sizeof(VERTEX), Aabb, 
                             m_pDevice);

      NumV = 0;
      NumI = 0;
      VertexBase = 0;
      SAFE_FREE(pV);
      SAFE_FREE(pI);
      } // meshs

   return true;
   } // LoadMeshs
/*----------------------------------------------------------------*/


/**
 * Suck in all portals from the level file.
 */
bool CGameSector::LoadPortals(FILE *pFile) {
   if (m_NumPorts > 0) {
      m_pPorts = new CGamePortal[m_NumPorts];
      
      for (UINT l=0; l<m_NumPorts; l++) {
         if (!m_pPorts[l].Load(pFile)) return false;
         m_pPorts[l].SetSector(this);
         }
      }
   return true;
   } // LoadPortals
/*----------------------------------------------------------------*/


/**
 * Suck in all lights from the level file.
 */
bool CGameSector::LoadLights(FILE *pFile) {
   if (m_NumLights > 0) {
      m_pLights = new CGameLight[m_NumLights];
      
      for (UINT l=0; l<m_NumLights; l++) {
         if (!m_pLights[l].Load(pFile, m_NumSkins)) 
            return false;
         }
      }
   return true;
   } // LoadLights
/*----------------------------------------------------------------*/


/**
 * Suck in all entities from the level file.
 */
bool CGameSector::LoadEntities(FILE *pFile) {
   ZFXENTITY Type=ENT_UNKNOWN;

   if (m_NumEntys > 0) {
      m_ppEntities = new LPGAMEENTITY[m_NumEntys];
      
      for (UINT e=0; e<m_NumEntys; e++) {
         // which type is this entity of?
         fread(&Type, sizeof(ZFXENTITY), 1, pFile);

         if ( Type == ENT_DOOR) 
            m_ppEntities[e] = new CGameDoor();
         else 
            return false;

         if (!m_ppEntities[e]->Load(pFile)) return false;
/*
         // if door then connect to portals
         if ( Type == ENT_DOOR ) {
            ((CGameDoor**)m_ppEntities)[e]->ConnectToPortals(m_pLevel);
            }
*/
         }
      }
   return true;
   } // LoadEntities
/*----------------------------------------------------------------*/


/**
 * Suck in all spawnpoints from the level file.
 */
bool CGameSector::LoadSpawns(FILE *pFile) {
   ZFXAabb aabb;
   UINT NumPolys=0;

   if (m_NumSpawns > 0) {
      m_pSpawns = new ZFXVector[m_NumSpawns];
      memset(m_pSpawns, 0, sizeof(ZFXVector)*m_NumSpawns);
      
      for (UINT s=0; s<m_NumSpawns; s++) {
         fread(&m_pSpawns[s], sizeof(ZFXVector), 1, pFile);
         fread(&aabb, sizeof(ZFXAabb), 1, pFile);

         // now comes the mesh but we don't need this
         fread(&NumPolys, sizeof(UINT), 1, pFile);
         fread(&aabb,     sizeof(ZFXAabb), 1, pFile);

         for (UINT i=0; i<NumPolys; i++) {
            if (!LoadAndSkipMeshData(pFile)) 
               return false;
            }
         } // for [polys in mesh]
      } // for [spawns]
   return true;
   } // LoadSpawns
/*----------------------------------------------------------------*/


// just used to set file pointer to correct position 
bool CGameSector::LoadAndSkipMeshData(FILE *pFile) {
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


/**
 * Make me the octree, baby =)
 */
HRESULT CGameSector::CreateOctree(void) {
   if (!m_pZFXPolys) return ZFX_FAIL;
   if (m_Count < 1) return ZFX_OK;

   m_pOctree = new ZFXOctree();

   m_pOctree->BuildTree(m_pZFXPolys, m_Count);

   // we don't need this list any longer
   SAFE_DELETE_A(m_pZFXPolys);
   return ZFX_OK;
   } // CreateOctree
/*----------------------------------------------------------------*/


/**
 * Render the sector which is stored in a static render buffer
 * from the ZFX dll. Use the appropriate indices for each skin. 
 */
HRESULT CGameSector::RenderDepth(ZFXPlane *pFrustum, 
                                 const ZFXVector &vcPos) {
   static CGameSector *pSector=NULL;

   HRESULT hr=ZFX_OK;
   ZFXAabb aabb;

   if (m_bDepthRendered) return ZFX_OK;
   else m_bDepthRendered = true;

   if (m_NumZFXPolys == 0) return ZFX_OK;

   // render the sector's geometry
   for (UINT i=0; i<this->m_NumSkins; i++) {

      hr = m_pDevice->GetVertexManager()->Render(
                              m_BufID, m_pBuffer[i].IBID,
                              m_pBuffer[i].SkinID);

      if (FAILED(hr)) return ZFX_FAIL;
      }

   // render the sector's doors
   for (UINT k=0; k<m_NumEntys; k++) {
      if (!m_ppEntities[k]->IsOfType(ENT_DOOR)) continue;
      aabb = m_ppEntities[k]->GetAabb();
      if (aabb.Cull(pFrustum, 6) == ZFXCULLED ) continue;
      m_ppEntities[k]->Render( m_pDevice );
      }

   // render neighboring sectors
   for (UINT j=0; j<m_NumPorts; j++) {
      // portal is inside a closed doorframe
      if ( !m_pPorts[j].IsOpen() ) continue;

      // portal is not in view frustum
      if ( m_pPorts[j].GetAabb().Cull(pFrustum, 6)
           == ZFXCULLED ) continue;

      // portal is backfacing to this position
      if ( m_pPorts[j].IsBackfacingTo(vcPos) ) continue;

      m_pPorts[j].ChopFrustum(pFrustum);

      pSector = m_pPorts[j].GetTargetSector();
      if (pSector) pSector->RenderDepth(pFrustum,vcPos);
      }
   return ZFX_OK;
   } // RenderDepth
/*----------------------------------------------------------------*/


/**
 * Render the sector which is stored in a static render buffer
 * from the ZFX dll. Use the appropriate indices for each skin. 
 */
HRESULT CGameSector::RenderLit(ZFXPlane *pFrustum, 
                               const ZFXVector &vcPos,
                               UINT *pNumT, UINT *pNumL) {
   static CGameSector *pSector=NULL;

static bool bTemp=true;
static DWORD dwTick=0;

   HRESULT hr=ZFX_OK;

   if (m_bLitRendered) return ZFX_OK;
   else m_bLitRendered = true;

   // set the ambient light
   m_pDevice->SetAmbientLight(   // 0.5f,0.5f,0.5f);
                              m_Ambient.fR,
                              m_Ambient.fG,
                              m_Ambient.fB);

 
   if (m_NumZFXPolys == 0) return ZFX_OK;
   
   
if ( (dwTick==0) && ((rand()%8)==0)) {
   bTemp=false;
   dwTick = GetTickCount();
   }
else if ((GetTickCount() - dwTick) > 100) {
   bTemp=true;
   dwTick = 0;
   }

   // render the sector's geometry
   for (UINT i=0; i<this->m_NumSkins; i++) {

      m_pDevice->ActivatePShader(m_BasePSH);
      m_pDevice->ActivateVShader(m_BaseVSH, VID_UU);
      


      hr = m_pDevice->GetVertexManager()->Render(
                              m_BufID, m_pBuffer[i].IBID,
                              m_pBuffer[i].SkinID);

      if (FAILED(hr)) return ZFX_FAIL;

      if (pNumT) (*pNumT) += m_pBuffer[i].NumTris;

      m_pDevice->ActivatePShader(m_OmniPSH);
      m_pDevice->ActivateVShader(m_OmniVSH, VID_UU);

      RenderLightPass(pFrustum, m_pBuffer[i].SkinID, bTemp);
      }

   // render the sector's doors
   RenderDoors(pFrustum, m_BaseVSH, m_BasePSH, 
               m_OmniVSH, m_OmniPSH, bTemp);

   // render neighboring sectors
   for (UINT j=0; j<m_NumPorts; j++) {
      // portal is inside a closed doorframe
      if ( !m_pPorts[j].IsOpen() ) continue;

      // portal is not in view frustum
      if ( m_pPorts[j].GetAabb().Cull(pFrustum, 6)
           == ZFXCULLED ) continue;

      // portal is backfacing to this position
      if ( m_pPorts[j].IsBackfacingTo(vcPos) ) continue;

      m_pPorts[j].ChopFrustum(pFrustum);

      pSector = m_pPorts[j].GetTargetSector();
      if (pSector) pSector->RenderLit(pFrustum,vcPos,pNumT,pNumL);
      }

   if (pNumL) (*pNumL) += m_NumLights;
   return ZFX_OK;
   } // RenderLit
/*----------------------------------------------------------------*/


/**
 * 
 */
HRESULT CGameSector::RenderShadows(ZFXPlane *pFrustum, 
                                   const ZFXVector &vcPos) {
   static CGameSector *pSector=NULL;

   HRESULT hr=ZFX_OK;

   if (m_bShadowsRendered) return ZFX_OK;
   else m_bShadowsRendered = true;

   // loop lights and seek shadow casters
   for (UINT l=0; l<m_NumLights; l++) {

      if ( !m_pLights[l].IsShadowCasting() ) continue;

      // render sector's shadow volumes for
      // this light source / shadow caster
      for (UINT m=0; m<m_NumMeshs; m++) {
         m_pMeshShadows[m].Render( &m_pLights[l] );
         }

      } // for [lights]

   // render neighboring sectors
   for (UINT j=0; j<m_NumPorts; j++) {
      // portal is inside a closed doorframe
      if ( !m_pPorts[j].IsOpen() ) continue;

      // portal is not in view frustum
      if ( m_pPorts[j].GetAabb().Cull(pFrustum, 6)
           == ZFXCULLED ) continue;

      // portal is backfacing to this position
      if ( m_pPorts[j].IsBackfacingTo(vcPos) ) continue;

      m_pPorts[j].ChopFrustum(pFrustum);

      pSector = m_pPorts[j].GetTargetSector();
      if (pSector) pSector->RenderShadows(pFrustum,vcPos);
      }

   return ZFX_OK;
   } // RenderShadows
/*----------------------------------------------------------------*/


/**
 * Render the sector which is stored in a static render buffer
 * from the ZFX dll. Use the appropriate indices for each skin,
 * but dont use shaders and therefor no light except for ambient. 
 */
HRESULT CGameSector::RenderAmbient(ZFXPlane *pFrustum, 
                                   const ZFXVector &vcPos) {
   static CGameSector *pSector=NULL;
   HRESULT hr=ZFX_OK;
   ZFXAabb aabb;
   UINT i=0;

   // set the ambient light
   m_pDevice->SetAmbientLight( //0.307f, 0.307f, 0.307f );
                              m_Ambient.fR,
                              m_Ambient.fG,
                              m_Ambient.fB);
                              
   if (m_NumZFXPolys == 0) return ZFX_OK;

   // RENDER FIRST PASS WITH AMBIENT LIGHT
   for (i=0; i<m_NumSkins; i++) {
      hr = m_pDevice->GetVertexManager()->Render(
                              m_BufID, m_pBuffer[i].IBID,
                              m_pBuffer[i].SkinID);

      if (FAILED(hr)) return hr;
      }

   for (i=0; i<m_NumEntys; i++) {
      if (!m_ppEntities[i]->IsOfType(ENT_DOOR)) continue;

      aabb = m_ppEntities[i]->GetAabb();

      if (aabb.Cull(pFrustum, 6) == ZFXCULLED ) continue;

      hr = m_ppEntities[i]->Render( m_pDevice );
      if (FAILED(hr)) return hr;
      }
     
   // render neighboring sectors
   for (UINT j=0; j<m_NumPorts; j++) {
      // portal is inside a closed doorframe
      if ( !m_pPorts[j].IsOpen() ) continue;

      // portal is not in view frustum
      if ( m_pPorts[j].GetAabb().Cull(pFrustum, 6)
           == ZFXCULLED ) continue;

      // portal is backfacing to this position
      if ( m_pPorts[j].IsBackfacingTo(vcPos) ) continue;

      m_pPorts[j].ChopFrustum(pFrustum);

      pSector = m_pPorts[j].GetTargetSector();
      if (pSector) pSector->RenderAmbient(pFrustum,vcPos);
      }

   return ZFX_OK;
   } // RenderAmbient
/*----------------------------------------------------------------*/


/** 
 * Render the polygon caches of all lights in an additive pass once
 * for each light but only polys with the given skin.
 */
void CGameSector::RenderLightPass(const ZFXPlane *pFrustum, 
                                  UINT nSkin, bool bFlicker) {
   ZFXMatrix mat;
   UINT IBID=0;

   if (!m_pDevice->CanDoShaders()) return;

   m_pDevice->UseAdditiveBlending(true);

   // RENDER AGAIN NOW ADDITIVE FOR ALL LIGHTS
   for (UINT i=0; i<m_NumLights; i++) {

      // this is a shadow caster and no light source
      if (m_pLights[i].IsShadowCasting()) 
         continue;

      // flickering light is off in this very moment
      if (m_pLights[i].IsFlickering() && bFlicker) 
         continue;

      // is this light shining on this skin at all?
      if ( !m_pLights[i].InfluencesSkin(nSkin, &IBID) )
         continue;

      // is lit geometry inside view frustum at all?
      if ( m_pLights[i].GetAabb().Cull(pFrustum, 6)
         == ZFXCULLED ) {
         continue;
         }
 
      mat = CalcTransAttenNoRot(m_pLights[i].GetPosition(),
                                m_pLights[i].GetRadius());

      m_pDevice->SetShaderConstant(SHT_VERTEX, DAT_FLOAT, 
                                   20, 4, (void*)&mat);

      m_pDevice->SetShaderConstant(SHT_PIXEL, DAT_FLOAT, 0, 1, 
                                   (void*)&m_pLights[i].GetColor());

      m_pDevice->GetVertexManager()->Render(m_BufID, IBID, nSkin);
      } // for [lights]

   m_pDevice->UseAdditiveBlending(false);
   } // RenderLights
/*----------------------------------------------------------------*/


/**
 * Render all doors currently sitting in this sector.
 */
void CGameSector::RenderDoors(const ZFXPlane *pFrustum, UINT BV,
                              UINT BP, UINT OV, UINT OP, 
                              bool bFlicker) {
   ZFXAabb   aabb;
   ZFXMatrix mat;

   for (UINT i=0; i<m_NumEntys; i++) {

      if (!m_ppEntities[i]->IsOfType(ENT_DOOR)) continue;

      aabb = m_ppEntities[i]->GetAabb();

      if (aabb.Cull(pFrustum, 6) == ZFXCULLED ) continue;

      // ambient pass 
      m_pDevice->ActivatePShader(BP);
      m_pDevice->ActivateVShader(BV, VID_UU);
      m_ppEntities[i]->Render( m_pDevice );

      m_pDevice->UseAdditiveBlending(true);
      m_pDevice->ActivatePShader(OP);
      m_pDevice->ActivateVShader(OV, VID_UU);

      // light passes
      for (UINT j=0; j<m_NumLights; j++) {

         // flickering light is off in this very moment
         if (m_pLights[j].IsFlickering() && bFlicker) 
            continue;

         if (!m_pLights[j].InfluencesAabb(aabb)) 
            continue;

         mat = CalcTransAttenNoRot(m_pLights[j].GetPosition(),
                                   m_pLights[j].GetRadius());

         m_pDevice->SetShaderConstant(SHT_VERTEX, DAT_FLOAT, 
                                      20, 4, (void*)&mat);

         m_pDevice->SetShaderConstant(SHT_PIXEL, DAT_FLOAT, 0, 1, 
                                      (void*)&m_pLights[j].GetColor());

         m_ppEntities[i]->Render( m_pDevice );
         }

      m_pDevice->UseAdditiveBlending(false);
      }
   } // RenderDoors
/*----------------------------------------------------------------*/


/**
 * Render all portals. This is to show off in front of chicks only.
 */
void CGameSector::RenderPortals(const ZFXVector &vc, bool b) {
   for (UINT i=0; i<m_NumPorts; i++) {
      if (b) {
         // check if portal is connected at all
         if ( !m_pPorts[i].GetBrother() )
            m_pPorts[i].Render( m_pDevice->GetVertexManager(), vc );

         // check if connected to itself
         else if ( m_pPorts[i].GetBrotherID() == 
                   m_pPorts[i].GetID() )
            m_pPorts[i].Render( m_pDevice->GetVertexManager(), vc );

         // check for portal looping to same sector
         else if ( m_pPorts[i].GetTargetSector() == this )
            m_pPorts[i].Render( m_pDevice->GetVertexManager(), vc );
         }
      else
         m_pPorts[i].Render( m_pDevice->GetVertexManager(), vc );
      }
   } // RenderPortals
/*----------------------------------------------------------------*/


/**
 * Render the octree. This is to show off in front of chicks only.
 */
void CGameSector::RenderOctree(bool bBox, bool bPolys) {
   // blow up ambient light values
   m_pDevice->SetAmbientLight(1.0f, 1.0f, 1.0f);

   // render that octree guy
//   if (m_pOctree) m_pOctree->Render(m_pDevice, bBox, bPolys);
   } // RenderOctree
/*----------------------------------------------------------------*/


/**
 * Calculates the attenuation matrix from a given omni light position
 * and its radius used for rendering with the pixel shader.
 */
ZFXMatrix CGameSector::CalcTransAttenNoRot(const ZFXVector &vcPos,
                                           float fRadius) {
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
   } // CalcTransAttenNoRot
/*----------------------------------------------------------------*/


/**
 * Updates all sector stuff that needs to be updated :)
 */
void CGameSector::Update(float fET) {
   // reset flags
   m_bLitRendered     = false;
   m_bDepthRendered   = false;
   m_bShadowsRendered = false;

   // update entities
   for (UINT i=0; i<m_NumEntys; i++) {
      m_ppEntities[i]->Update(fET);
      }
   } // Update
/*----------------------------------------------------------------*/


/**
 * Player has pushed the activate button
 */
void CGameSector::TouchAndUse(const ZFXVector &vcPos) {
   for (UINT i=0; i<m_NumEntys; i++) {
      m_ppEntities[i]->TouchAndUse(vcPos);
      }
   } // TouchAndUse
/*----------------------------------------------------------------*/


/**
 * Get a pointer to the portal with the given ID
 */
LPGAMEPORTAL CGameSector::GetPortal(UINT id) {
   for (UINT i=0; i<m_NumPorts; i++) {
      if (m_pPorts[i].GetID() == id) 
         return &m_pPorts[i];
      }
   return NULL;
   } // GetPortal
/*----------------------------------------------------------------*/


/**
 * Searches the brother for the given portal and connects both of them.
 */
bool CGameSector::FindBrother(LPGAMEPORTAL pPortal) {
   for (UINT i=0; i<m_NumPorts; i++) {
      if (m_pPorts[i].GetID() == pPortal->GetBrotherID()) 
         {
         m_pPorts[i].SetBrotherRelationship(pPortal);
         return true;
         }
      }
   return false;
   } // FindBrother
/*----------------------------------------------------------------*/


/**
 *
 */
void CGameSector::BuildAllPortalRelationships(void) {

   // 1. connect doors to portals
   for (UINT e=0; e<m_NumEntys; e++) {
      // if door then connect to portals
      if ( m_ppEntities[e]->IsOfType(ENT_DOOR) ) {
         ((CGameDoor**)m_ppEntities)[e]->ConnectToPortals(m_pLevel);
         }
      }

   // 2. connect portals to each other
   for (UINT p=0; p<m_NumPorts; p++) {
      m_pLevel->FindBrother( &m_pPorts[p] );
      }

   } // BuildAllPortalRelationships
/*----------------------------------------------------------------*/


/**
 * Check for collision of sector geometry with given aabb. Return
 * plane of collision and new sector id if aabb traversed a portal.
 */
bool CGameSector::TestCollision(UINT *pnS, const ZFXAabb &aabb, 
                                ZFXPlane *pP, bool *pbDoor) {
   // check collision with level geometry
   bool bCollision = m_pOctree->TestCollision(aabb, pP);
  
   if (pbDoor) *pbDoor = false;

   // check collision with an entity
   if (!bCollision) {
      for (UINT e=0; e<m_NumEntys; e++) {
         if (m_ppEntities[e]->TestCollision(aabb, pP)) {
            bCollision = true;
            if (pbDoor) *pbDoor = true;
            break;
            }
         } // for [entities]
      }

   // check for transwarp into a new sector via portal
   for (UINT i=0; i<m_NumPorts; i++) {

      // given aabb touches a portal
      if (m_pPorts[i].GetAabb().Intersects(aabb)) {
         // aabb center is on backside of the portal         
         if ( m_pPorts[i].IsOnBackside(aabb.vcCenter) ) {
            CGamePortal *pTarget = m_pPorts[i].GetBrother();
            if (!pTarget) { *pnS=99; break; }

            CGameSector *pSector = pTarget->GetSector();
            if (!pSector)  { *pnS=99; break; }

            *pnS = pSector->GetIndex();
            break;
            }
         }
      } // for [portals]

   return bCollision;
   } // TestCollision[aabb]
/*----------------------------------------------------------------*/


/**
 * Check for collision of sector geometry with given ray within
 * given distance. Return distance to collision and new sector 
 * id if aabb traversed a portal.
 */
bool CGameSector::TestCollision(UINT *pnS, const ZFXRay &Ray, 
                                float fL, float *pfD, bool *pbDoor) {
   ZFXAabb aabb;

   // check collision with level geometry
   bool bCollision = m_pOctree->TestCollision(Ray, fL, pfD);
 
   if (pbDoor) *pbDoor = false;

   // check collision with an entity
   if (!bCollision) {
      for (UINT e=0; e<m_NumEntys; e++) {
         if (m_ppEntities[e]->TestCollision(Ray, fL, pfD)) {
            bCollision = true;
            if (pbDoor) *pbDoor = true;
            break;
            }
         } // for [entities]
      }

   ZFXVector vcEnd = Ray.m_vcOrig + (Ray.m_vcDir*fL);

   // check for transwarp into a new sector via portal
   for (UINT i=0; i<m_NumPorts; i++) {
      aabb = m_pPorts[i].GetAabb();

      // given ray intersects or touches a portal
      if ( aabb.Intersects(Ray, fL, NULL) ||
           aabb.Contains(Ray, fL) ) {
         // line segment endpoint on backside of portal         
         if ( m_pPorts[i].IsOnBackside(vcEnd) ) {
            CGamePortal *pTarget = m_pPorts[i].GetBrother();
            if (!pTarget) { *pnS=99; break; }

            CGameSector *pSector = pTarget->GetSector();
            if (!pSector)  { *pnS=99; break; }

            *pnS = pSector->GetIndex();
            break;
            }
         }
      } // for [portals]

   return bCollision;
   } // TestCollision[ray]
/*----------------------------------------------------------------*/


/**
 * Get the accurate height for the given point above the level
 * geometry. Note that trhis value is the distance to the floor.
 */
bool CGameSector::GetFloor(const ZFXVector &vcPos, float *f, ZFXPlane *p) {
   bool bPortalFloor=false;

   // first check this sector's geometry
   bool bFloor = m_pOctree->GetFloor(vcPos,f,p);

   // check if ray is intersecting a (horizontal) portal
   for (UINT i=0; i<m_NumPorts; i++) {

      if (m_pPorts[i].IsBackfacingTo(vcPos)) 
         continue;

      if (!m_pPorts[i].IntersectsDownwardsRay(vcPos, *f))
         continue;

      // now we know a portal is below the player and it is closer
      // than any already found floor distance in this sector, so 
      // goto the neighboring sector and get the floor from there:
      CGameSector *pSector = m_pPorts[i].GetTargetSector();
      bPortalFloor = pSector->GetFloor(vcPos, f, p);
      }

   return (bFloor || bPortalFloor) ;
   } // GetFloor
/*----------------------------------------------------------------*/


/**
 * Loads a single polygon from the given file pointer and does two
 * things. First it adds this polygon to the instance POLYGON list
 * to keep them all together. Then it adds the polygon to the 
 * ZFXPolygon list which is used later on to build the octree.
 */
bool CGameSector::LoadSinglePolygon(FILE *pFile) {
   bool       DummyBOOL = false;
   float      DummyFLOAT[2];
   ZFXAabb    DummyAABB;
   WORD      *pDummyWORD = NULL;

   // data storages
   ZFXVector    *pVC     = NULL;
   unsigned int *puI     = NULL;

   if (!pFile) return false;

   // allocate sufficient memory
   if ( (m_NumPolys%100) == 0) {
      m_pPolys = (POLYGON*)realloc(m_pPolys, 
                 sizeof(POLYGON)*(m_NumPolys+100));
      if (!m_pPolys) return false;
      }

   POLYGON *pPoly = &m_pPolys[m_NumPolys];

   pPoly->pVerts = NULL;
   pPoly->pIndis = NULL;

   // read polygon header
   fread(&pPoly->NumV,   sizeof(UINT), 1, pFile);
   fread(&pPoly->NumI,   sizeof(UINT), 1, pFile);
   fread(&pPoly->SkinID, sizeof(UINT), 1, pFile);
   fread(&DummyAABB, sizeof(ZFXAabb), 1, pFile);
   fread(&DummyBOOL, sizeof(bool),    1, pFile);
   fread(DummyFLOAT, sizeof(float),   2, pFile);
   fread(DummyFLOAT, sizeof(float),   2, pFile);
     
   // allocate memory
   pVC = new ZFXVector[pPoly->NumV];
   puI = new unsigned int[pPoly->NumI];
   pDummyWORD = new WORD[pPoly->NumV*2];

   pPoly->pVerts = new VERTEX[pPoly->NumV];
   pPoly->pIndis = new WORD[pPoly->NumI];
   
   // read vertex and index data
   fread(pPoly->pVerts, sizeof(VERTEX), pPoly->NumV, pFile);
   fread(pPoly->pIndis, sizeof(WORD), pPoly->NumI, pFile);
   fread(pDummyWORD, sizeof(WORD), pPoly->NumV*2, pFile);

   // build vector list from vertex list
   for (UINT i=0; i<pPoly->NumV; i++) {
      pVC[i].Set(pPoly->pVerts[i].x, 
                 pPoly->pVerts[i].y, 
                 pPoly->pVerts[i].z);
      }

   // this is for octree list
   for (UINT j=0; j<pPoly->NumI; j++) {
      puI[j] = pPoly->pIndis[j];
      }

   // add to global list of polygons
   m_pZFXPolys[m_Count++].Set(pVC, pPoly->NumV, puI, pPoly->NumI);

   // adjust counter 
   m_NumPolys++;

   // free memory
   SAFE_DELETE_A(pVC);
   SAFE_DELETE_A(puI);
   SAFE_DELETE_A(pDummyWORD);
   return true;
   } // LoadSinglePolygon
/*----------------------------------------------------------------*/


/**
 * This method will create the index buffers for rendering. Besides
 * it will also create the light's caches to render lighting.
 */
HRESULT CGameSector::CompilePolygonList(void) {
   HRESULT    hr=ZFX_OK;
   POLYGON   *pPoly = NULL;
   ZFXVector  Tri[3];
   WORD       I[3];

   UINT     CountV=0,
            CountI=0,
            SkinIndis=0,
            Start=0,
            nSkin=0;

   if (m_NumPolys < 1) return ZFX_OK;

   // sort polygons by skin
   qsort(m_pPolys, m_NumPolys, sizeof(POLYGON), SortBySkin);

   for (UINT i=0; i<m_NumPolys; i++) {
      pPoly = &m_pPolys[i];

      // copy vertices to general list
      memcpy(&m_pVerts[CountV], pPoly->pVerts, 
             sizeof(VERTEX)*pPoly->NumV);

      // copy indices to general list
      for (UINT j=0; j<pPoly->NumI; j+=3) {
         I[0] = CountV + pPoly->pIndis[j];
         I[1] = CountV + pPoly->pIndis[j+1];
         I[2] = CountV + pPoly->pIndis[j+2];

         memcpy(&m_pIndis[CountI+j], I, sizeof(WORD)*3);

         Tri[0].Set(m_pVerts[I[0]].x,
                    m_pVerts[I[0]].y, 
                    m_pVerts[I[0]].z);

         Tri[1].Set(m_pVerts[I[1]].x, 
                    m_pVerts[I[1]].y, 
                    m_pVerts[I[1]].z);

         Tri[2].Set(m_pVerts[I[2]].x, 
                    m_pVerts[I[2]].y, 
                    m_pVerts[I[2]].z);

         // loop through all lights
         for (UINT k=0; k<m_NumLights; k++) {
            if (m_pLights[k].InfluencesTri(Tri)) {
               m_pLights[k].AddTriIndices(I[0], I[1], 
                                          I[2], Tri, 
                                          pPoly->SkinID);
               }
            } // for
         }

       // update counters
      CountV    += pPoly->NumV;
      CountI    += pPoly->NumI;
      SkinIndis += pPoly->NumI;

      // skin will change for the next polygon
      if ( (i == (m_NumPolys-1)) || 
           (pPoly->SkinID != m_pPolys[i+1].SkinID) ) {

         hr = m_pDevice->GetVertexManager()->
                         CreateIndexBuffer(SkinIndis,
                                           &m_pIndis[Start],
                                           &m_pBuffer[nSkin].IBID);
         if (FAILED(hr)) return hr;

         m_pBuffer[nSkin].SkinID  = pPoly->SkinID;
         m_pBuffer[nSkin].NumTris = (UINT)(((float)SkinIndis)/3);
         SkinIndis = 0;
         Start = CountI;
         nSkin++;
         }
      }

   // create a single static vertex buffer
   hr = m_pDevice->GetVertexManager()->CreateStaticBuffer(
                                    VID_UU, 2, CountV, 
                                    0,  m_pVerts, 
                                    NULL, &m_BufID);

   // close the caches for all lights
   for (UINT k=0; k<m_NumLights; k++) {
      m_pLights[k].CloseCaches(m_pDevice);
      }

   return hr;
   } // CompilePolygonList
/*----------------------------------------------------------------*/


/**
 * Returns position of a random spawnpoint if any
 */
bool CGameSector::GetSpawnPoint(ZFXVector *pSpawn) {
   if (m_NumSpawns == 0) return false;
   if (pSpawn == NULL) return false;

   int Index = rand()%m_NumSpawns;

   memcpy(pSpawn, &m_pSpawns[Index], sizeof(ZFXVector));
   return true;
   } // GetSpawnPoint
/*----------------------------------------------------------------*/


/** 
 * Debug only: Test if all Portals are connected to another one
 *             leading to another sector. Otherwise something
 *             must have went wrong or level data is invalid
 */
bool CGameSector::AllPortalsConnectedOk(void) {
   
   for (UINT i=0; i<m_NumPorts; i++) {

      // check if portal is connected at all
      if ( !m_pPorts[i].GetBrother() )
         return false;

      // check if connected to itself
      if ( m_pPorts[i].GetBrotherID() == 
           m_pPorts[i].GetID() )
         return false;

      // check for portal looping to same sector
      if ( m_pPorts[i].GetTargetSector() == this )
         return false;

      }
   return true;
   } // AllPortalsConnectedOk
/*----------------------------------------------------------------*/
