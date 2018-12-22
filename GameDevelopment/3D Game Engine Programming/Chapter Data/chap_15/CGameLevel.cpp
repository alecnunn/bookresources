// FILE: CGameLevel.cpp

#include "include/CGameLevel.h"
#include "include/zfx.h"


// constructor
CGameLevel::CGameLevel(ZFXRenderDevice *pDevice) {
   Init(pDevice);
   } // constructor
/*----------------------------------------------------------------*/


// destructor
CGameLevel::~CGameLevel(void) {
   Release();
   } // destructor
/*----------------------------------------------------------------*/


// set default values
void CGameLevel::Init(ZFXRenderDevice *pDevice) {
   m_pDevice    = pDevice;
   m_pSectors   = NULL;
   m_NumSectors = 0;
   m_NumSpawns  = 0;
   m_PSH        = 0;
   m_VSH        = 0;
   } // Init
/*----------------------------------------------------------------*/


// clean up all stuff
void CGameLevel::Release(void) {
   for (UINT i=0; i<m_NumSectors; i++) 
      SAFE_DELETE( m_pSectors[i] );

   SAFE_DELETE_A(m_pSectors);
   } // Release
/*----------------------------------------------------------------*/


// reinitialize all atsuff
void CGameLevel::Reset(void) {
   Release();
   Init(m_pDevice);
   } // Reset
/*----------------------------------------------------------------*/


// make the shaders know to this instance
void CGameLevel::BroadcastShaders(UINT BaseVSH,  UINT BasePSH, 
                                  UINT OmniVSH,  UINT OmniPSH,
                                  UINT DepthVSH, UINT DepthPSH) {
   m_PSH = DepthPSH;
   m_VSH = DepthVSH;

   for (UINT i=0; i<m_NumSectors; i++)
      m_pSectors[i]->ReceiveShaders(BaseVSH, 
                                    BasePSH, 
                                    OmniVSH,
                                    OmniPSH);
   } // BroadcastShaders
/*----------------------------------------------------------------*/


// load a level file
bool CGameLevel::Load(const char *pchName) {
   UINT       NumSkins   = 0;
   UINT       DummyUINT  = 0;

   FILE *pFile = fopen(pchName, "rb");

   if (!pFile) return false;

   fread(&m_NumSectors, sizeof(UINT), 1, pFile);
   fread(&DummyUINT,    sizeof(UINT), 1, pFile);
   fread(&NumSkins,     sizeof(UINT), 1, pFile);

   m_pSectors = new LPGAMESECTOR[m_NumSectors];

   // load all sectors
   for (UINT s=0; s<m_NumSectors; s++) {
      m_pDevice->BeginRendering(true,true,true);
      m_pDevice->DrawText(0,10,10,255,255,255,"new sector...");
      m_pDevice->EndRendering();

      m_pSectors[s] = new CGameSector(m_pDevice, s, this);
      m_pSectors[s]->Load(pFile);
      m_NumSpawns += m_pSectors[s]->GetNumSpawns();
      }

   // load all skins
   for (UINT sn=0; sn<NumSkins; sn++) {
      LoadSkin(m_pDevice->GetSkinManager(), pFile);
      }
 
   fclose(pFile);

   BuildAllPortalRelationships();
   return true;
   } // Load
/*----------------------------------------------------------------*/


HRESULT CGameLevel::Render(UINT nSector, UINT *pNumT, UINT *pNumL,
                           const ZFXVector &vcPos) {
   ZFXPlane Frustum[6];
   ZFXAabb  aabb;
   HRESULT  hr=ZFX_OK;

   m_pDevice->GetFrustrum(Frustum);

   // set world transform to no transform
   m_pDevice->SetWorldTransform(NULL);

   if (nSector >= m_NumSectors) 
      return ZFX_INVALIDPARAM;

      if ( !m_pDevice->CanDoShaders() ) {
         m_pDevice->UseShaders(false);
         hr = m_pSectors[nSector]->RenderAmbient(Frustum, vcPos);
         }
      else {

         m_pDevice->UseShaders(true);
         m_pDevice->ActivateVShader(m_VSH, VID_UU);
         m_pDevice->ActivatePShader(m_PSH);

         // render first pass ambient with no textures
         m_pDevice->UseTextures(false);
         m_pDevice->UseColorBuffer(false);
         hr = m_pSectors[nSector]->RenderDepth(Frustum, vcPos);

         m_pDevice->UseTextures(true);
         m_pDevice->UseColorBuffer(true);
         m_pDevice->SetDepthBufferMode(RS_DEPTH_READONLY);
         hr = m_pSectors[nSector]->RenderLit(
                                      Frustum, vcPos,
                                      pNumT, pNumL);

         m_pDevice->SetDepthBufferMode(RS_DEPTH_READWRITE);
         m_pDevice->UseShaders(false);
         }


   return hr;
   } // Render
/*----------------------------------------------------------------*/


// render the shadows from all sectors
HRESULT CGameLevel::RenderShadows(UINT nSector, const ZFXVector &vcPos) {
   ZFXPlane Frustum[6];
   ZFXAabb  aabb;
   HRESULT  hr=ZFX_OK;

   m_pDevice->GetFrustrum(Frustum);

   // set world transform to no transform
   m_pDevice->SetWorldTransform(NULL);

   if (nSector >= m_NumSectors) 
      return ZFX_INVALIDPARAM;

   if ( !m_pDevice->CanDoShaders() ) {
      hr = ZFX_NOSHADERSUPPORT;
      }
   else {
      m_pDevice->UseStencilShadowSettings(true);

      hr = m_pSectors[nSector]->RenderShadows(Frustum, vcPos);

      m_pDevice->UseStencilShadowSettings(false);
      }

   m_pDevice->SetStencilBufferMode(RS_STENCIL_ENABLE, 0);
   m_pDevice->SetStencilBufferMode(RS_STENCIL_REF, 1);
   m_pDevice->SetStencilBufferMode(RS_STENCIL_FUNC_LESSEQUAL, 0);
   m_pDevice->SetStencilBufferMode(RS_STENCIL_PASS_KEEP, 0);

   m_pDevice->FadeScreen(0.0f, 0.0f , 0.0f, 0.5f);

   m_pDevice->SetStencilBufferMode(RS_STENCIL_DISABLE, 0);
   return hr;
   } // Render
/*----------------------------------------------------------------*/


// update entities and all other stuff as needed
void CGameLevel::Update(float fET) {
   for (UINT s=0; s<m_NumSectors; s++)
      m_pSectors[s]->Update(fET);
   } // Update
/*----------------------------------------------------------------*/


// update entities, give vector if user pushed activate key
void CGameLevel::TouchAndUse(const ZFXVector &vcPos) {
   for (UINT s=0; s<m_NumSectors; s++) {
      m_pSectors[s]->TouchAndUse(vcPos);
      }
   } // TouchAndUse
/*----------------------------------------------------------------*/


// render the octree of each sector, just for evaluation
void CGameLevel::RenderOctree(bool bBox, bool bPolys) {
   for (UINT s=0; s<m_NumSectors; s++) {
      m_pSectors[s]->RenderOctree(bBox, bPolys);
      }

   m_pDevice->SetShadeMode(RS_SHADE_SOLID, 0, NULL);
   m_pDevice->SetDepthBufferMode(RS_DEPTH_READWRITE);
   } // RenderOctree
/*----------------------------------------------------------------*/


// render the portals of each sector, just for evaluation
// feed in true if you only want to render wrong portals
void CGameLevel::RenderPortals(const ZFXVector &vc, bool b) {

   for (UINT s=0; s<m_NumSectors; s++) {
      m_pSectors[s]->RenderPortals( vc, b );
      }

   } // RenderPortals
/*----------------------------------------------------------------*/


// find the next appropriate height underneath the given point
bool CGameLevel::GetFloor(UINT nS, const ZFXVector &vc, float *pf,
                          ZFXPlane *pP) {
   if (nS >= m_NumSectors) return false;
   return m_pSectors[nS]->GetFloor(vc, pf, pP); 
   } // GetFloor
/*----------------------------------------------------------------*/


/**
 * Collision detection with the aabb in the level. Returns new
 * sector id if aabb center has traversed a portal and returns
 * plane of the polygon collided with if any.
 */
bool CGameLevel::TestCollision(UINT *pnS, const ZFXAabb &aabb, 
                               ZFXPlane *pP, bool *pbDoor) {
   if (*pnS >= m_NumSectors) return false;
   return m_pSectors[*pnS]->TestCollision(pnS, aabb, pP, pbDoor); 
   } // TestCollision
/*----------------------------------------------------------------*/


/**
 * Collision detection with ray in the level. Returns new
 * sector id if line endpoint is beyond a portal and returns
 * distance to point of intersection if any.
 */
bool CGameLevel::TestCollision(UINT *pnS, const ZFXRay &Ray, 
                               float fL, float *pfD, bool *pbDoor) {
   if (*pnS >= m_NumSectors) return false;
   return m_pSectors[*pnS]->TestCollision(pnS, Ray, fL, pfD, pbDoor); 
   } // TestCollision
/*----------------------------------------------------------------*/


void CGameLevel::LoadSkin(ZFXSkinManager *pSM, FILE *pFile) {
   ZFXMATERIAL Mtrl;
   ZFXCOLOR    Clr[256];
   HRESULT     hr;
   float fAlpha=0.0f;
   UCHAR nNumClr=0;
   char  ch[MAX_PATH];
   bool  bAlpha=false;
   UINT  nSize=0;
   UINT  nNumTex=0;
   UINT  ID=0;

   memset(&Mtrl, 0, sizeof(ZFXMATERIAL));
   memset(&Clr, 0, sizeof(ZFXCOLOR)*256);
   memset(ch, 0, sizeof(char)*MAX_PATH);
   
   fread(&bAlpha,  sizeof(bool),  1, pFile);
   fread(&Mtrl,    sizeof(float), 17, pFile);
   fread(&nNumTex, sizeof(UINT),  1, pFile);

   // create the skin's material now
   pSM->AddSkin(&Mtrl.cAmbient, &Mtrl.cDiffuse, 
                &Mtrl.cEmissive, &Mtrl.cSpecular,
                1.0f, &ID);

   // load skin's textures now
   for (UINT i=0; i<nNumTex; i++) {
      fread(&nSize, sizeof(UINT), 1, pFile);
      fread(ch, nSize, 1, pFile);
      ch[nSize] = '\0';

      if (!bAlpha) {
         hr = pSM->AddTexture(ID, ch, bAlpha, 1.0f, NULL, 0);
         continue;
         }

      // read alpha restore information
      fread(&fAlpha,  sizeof(float), 1, pFile);
      fread(&nNumClr, sizeof(UCHAR), 1, pFile);
      fread(Clr, sizeof(ZFXCOLOR), nNumClr, pFile);
      
      hr = pSM->AddTexture(ID, ch, bAlpha, fAlpha, Clr, nNumClr);
      }
   } // LoadSkin
/*----------------------------------------------------------------*/


// find the portal to the given id in the level's sectors
LPGAMEPORTAL CGameLevel::GetPortal(UINT id) {
   LPGAMEPORTAL pPortal=NULL;

   for (UINT s=0; s<m_NumSectors; s++) {
      pPortal = m_pSectors[s]->GetPortal(id);
      if (pPortal) return pPortal;
      }

   return NULL;
   } // GetPortal
/*----------------------------------------------------------------*/


/**
 * Searches the brother for the given portal andconnects both of them.
 */
bool CGameLevel::FindBrother(LPGAMEPORTAL pPortal) {
   if (pPortal->GetBrotherID() == 0) return true;

   for (UINT s=0; s<m_NumSectors; s++) {
      if (m_pSectors[s]->FindBrother(pPortal)) return true;
      }
   return false;
   } // FindBrother
/*----------------------------------------------------------------*/


/**
 *
 */
void CGameLevel::BuildAllPortalRelationships(void) {
   for (UINT s=0; s<m_NumSectors; s++) {
      m_pSectors[s]->BuildAllPortalRelationships();
      }
   } // BuildAllPortalRelationships
/*----------------------------------------------------------------*/


/**
 *
 */
bool CGameLevel::GetSpawnPoint(UINT *pnSID, ZFXVector *pvc) {
   if ( !pnSID || !pvc ) return false;

   // no spawn points defined at all
   if (m_NumSpawns < 1) return false;

   int  Index=0;
   bool bFound=false;
   
   // get a random one
   while (!bFound) {
      Index = rand()%m_NumSectors;

      bFound = m_pSectors[Index]->GetSpawnPoint(pvc);
      *pnSID = Index;
      }

   return true;
   } // GetSpawnPoint
/*----------------------------------------------------------------*/


/** 
 * Debug only: Test if all Portals are connected to another one
 *             leading to another sector. Otherwise something
 *             must have went wrong or level data is invalid
 */
bool CGameLevel::AllPortalsConnectedOk(void) {
   for (UINT s=0; s<m_NumSectors; s++) {
      if ( !m_pSectors[s]->AllPortalsConnectedOk() )
         return false;
      }
   return true;
   } // AllPortalsConnected
/*----------------------------------------------------------------*/

