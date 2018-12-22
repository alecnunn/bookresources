// FILE: CLevel.cpp

#include "include/CLevel.h"
#include "include/CApplication.h"


CLevel::CLevel(void) {
   Init();
   } // constructor
/*----------------------------------------------------------------*/


CLevel::~CLevel(void) {
   Release();
   } // destructor
/*----------------------------------------------------------------*/


// init all stuff
void CLevel::Init(void) {
   m_nActive    = 0;
   m_NumSectors = 0;
   m_PID        = 1;    // 0 is invalid ID [!!!]
   m_ppSector   = NULL;
   sprintf(m_chName, " ");
   } // Init
/*----------------------------------------------------------------*/


// set the name of this level
void CLevel::SetName(const char *chName) {
   int nL = strlen(chName) +1;
   if (nL>MAX_PATH) memcpy(m_chName, chName, MAX_PATH);
   else memcpy(m_chName, chName, nL);
   } // SetName
/*----------------------------------------------------------------*/


// get the name of this level
void CLevel::GetName(char *chName) {
   memcpy(chName, m_chName, strlen(m_chName)+1);
   } // GetName
/*----------------------------------------------------------------*/


// check if level has already a name
bool CLevel::HasName(void) {
   if ( strcmp(m_chName, " ")==0 ) return false;
   else return true;
   } // HasName
/*----------------------------------------------------------------*/


// release all stuff
void CLevel::Release(void) {
   if (m_ppSector) {

      for (UINT i=0; i<m_NumSectors; i++) {
         delete m_ppSector[i];
         m_ppSector[i] = NULL;
         }

      free(m_ppSector);
      m_ppSector=NULL;
      }
   m_NumSectors=0;
   } // Release
/*----------------------------------------------------------------*/


// empty out the whole level
void CLevel::ClearAll(ZFXSkinManager *pSM) {
   Release();
   pSM->Reset();
   Init();
   } // ClearAll
/*----------------------------------------------------------------*/


// loads a level from a file
bool CLevel::LoadLevel(const char *ch, HWND hcbSector, 
                       HWND hcbSkins, ZFXRenderDevice *pDevice) {
   FILE *pFile=NULL;
   char  buffer[1024];
   int   n=0;
   UINT  NumSkins=0, i=0;
 
   if ( !(pFile = fopen(ch, "rb")) )
      return false;

   ClearAll( pDevice->GetSkinManager() );
   cbhResetContent(hcbSector);
   cbhResetContent(hcbSkins);

   fread(&m_NumSectors, sizeof(UINT), 1, pFile);
   fread(&m_PID,        sizeof(UINT), 1, pFile);
   fread(&NumSkins,     sizeof(UINT), 1, pFile);

   // allocate memory
   n = ((int)(((float)m_NumSectors)/50)+1)*50;
   m_ppSector = (LPSECTOR*)malloc(sizeof(LPSECTOR)*n);

   // now load all sectors
   for (i=0; i<m_NumSectors; i++) {
      m_ppSector[i] = new CSector("Empty");
      m_ppSector[i]->Load(pFile);

      // update sector list in combo box
      m_ppSector[i]->GetName(buffer);
      cbhAddItem(hcbSector, buffer, NULL, true);
      SelectSector(buffer);
      }

   // now load all the skins
   for (i=0; i<NumSkins; i++) 
      LoadSkin(hcbSkins, pDevice->GetSkinManager(), pFile);

   fclose(pFile);
   SetName(ch);

   return true;
   } // LoadLevel
/*----------------------------------------------------------------*/


// saves a level to a file
bool CLevel::SaveLevel(const char *ch, ZFXRenderDevice *pDevice) {
   FILE *pFile=NULL;
   UINT i=0;
 
FILE *pLog=fopen("Save_Log.txt", "w");

   if ( !(pFile = fopen(ch, "wb")) )
      return false;

if(pLog){fprintf(pLog, "Datei geoeffnet\n"); fflush(pLog);}

   // rearrange normals
   RecalcVertexNormals();

   UINT NumSkins = pDevice->GetSkinManager()->GetNumSkins();

if(pLog){fprintf(pLog, "Anzahl Skins geholt\n"); fflush(pLog);}

   fwrite(&m_NumSectors, sizeof(UINT), 1, pFile);
   fwrite(&m_PID,        sizeof(UINT), 1, pFile);
   fwrite(&NumSkins,     sizeof(UINT), 1, pFile);

if(pLog){fprintf(pLog, "Basisdaten geschrieben\n"); fflush(pLog);}

   // now save all sectors
   for (i=0; i<m_NumSectors; i++) {
      m_ppSector[i]->Save(pFile);

if(pLog){fprintf(pLog, "Ein Sektor wurde geschrieben\n"); fflush(pLog);}

      fflush(pFile);
      }

   // save all the skins
   for (i=0; i<NumSkins; i++)
      SaveSkin(pDevice->GetSkinManager(), pFile, i);


if(pLog){fprintf(pLog, "Die Skins wurden geschrieben\n"); fflush(pLog);}
fclose(pLog);

   fclose(pFile);
   SetName(ch);
   return true;
   } // SaveLevel
/*----------------------------------------------------------------*/


/**
 * Recalculate normals for all geometry vertices in all sectors.
 */
void CLevel::RecalcVertexNormals(void) {
   for (UINT i=0; i<m_NumSectors; i++)
      m_ppSector[i]->RecalcVertexNormals();
   } // RecalcVertexNormals
/*----------------------------------------------------------------*/


/**
 * Renders all elements of given type from the active sector.
 */
void CLevel::RenderActive(ZFXRenderDevice *pDevice, DWORD Type, 
                          bool bHide) {

   pDevice->SetTextureStage(1, RS_TEX_MODULATE);
   pDevice->SetTextureStage(2, RS_NONE);

   m_ppSector[m_nActive]->Render(pDevice->GetVertexManager(),
                                 Type, bHide);
   } // RenderActive
/*----------------------------------------------------------------*/


/**
 * Renders all elements of given type from the active sector using
 * the omni lights point of view.
 */
void CLevel::RenderActiveLit(ZFXRenderDevice *pDevice, bool bHide) {
   m_ppSector[m_nActive]->RenderLit(pDevice, bHide);
   } // RenderActive
/*----------------------------------------------------------------*/


/**
 * Renders all elements of given type from the inactive sectors.
 */
void CLevel::RenderInActive(ZFXRenderDevice *pDevice, DWORD Type, 
                            bool bHide) {

   ZFXVertexCacheManager *pVCM = pDevice->GetVertexManager();

   // render only inactive sectors
   for (UINT i=0; i<m_NumSectors; i++) {
      if (i != m_nActive) {
         m_ppSector[i]->Render(pVCM, Type, bHide);
         }
      }
   } // RenderActive
/*----------------------------------------------------------------*/


/**
 * Adds a new, empty sector to this level. Allocates memory for
 * the new sector if there isn't enough space to take one more.
 */
HRESULT CLevel::AddSector(const char *ch) {
   // make sure that name doesn't exist yet
   if (SectorExists(ch)) return E_FAIL;

   // allocate memory if neccessary
   if ((m_NumSectors%50) == 0)
   m_ppSector = (LPSECTOR*)realloc(m_ppSector, 
                 sizeof(LPSECTOR)*(m_NumSectors+50));

   // create a new instance for a sector
   m_ppSector[m_NumSectors] = new CSector(ch);

   m_NumSectors++;
   return S_OK;
   } // AddSector
/*----------------------------------------------------------------*/


/**
 * Removes a new sector from the level if it finds the name.
 */
HRESULT CLevel::RemoveSector(const char *ch) {
   UINT i=0;

   // try to find the sector
   for (i=0; i<m_NumSectors; i++) {
      if (m_ppSector[i]->NameEqual(ch)) break;
      }

   // sector with that name was not found
   if ( i == m_NumSectors ) return E_FAIL;

   // delete this instance
   delete m_ppSector[i];

   // copy the last one to this slot
   m_ppSector[i] = m_ppSector[m_NumSectors-1];
   m_ppSector[m_NumSectors-1] = NULL;

   m_NumSectors--;
   return S_OK;
   } // RemoveSector
/*----------------------------------------------------------------*/


/**
 * Removes all connections to this portal in the whole level.
 */
void CLevel::RemovePortalConnections(UINT id) {
   for (UINT i=0; i<m_NumSectors; i++)
      m_ppSector[i]->RemovePortalConnections(id);
   } // RemovePortalConnections
/*----------------------------------------------------------------*/


/**
 * Establishes all connections for the new portal in town.
 */
void CLevel::NewPortalInTown(CPortal *pPortal) {
   if (!pPortal) return;

   for (UINT i=0; i<m_NumSectors; i++)
      m_ppSector[i]->NewPortalInTown( pPortal );
   } // NewPortalInTown
/*----------------------------------------------------------------*/


/**
 * Establishes all connections for the new portal in town.
 */
void CLevel::NewDoorInTown(CEntity *pEntity) {
   // better save than sorry
   if (!pEntity) return;
   if (pEntity->GetSubType() != ENT_DOOR) return;

   for (UINT i=0; i<m_NumSectors; i++)
      m_ppSector[i]->NewDoorInTown( pEntity );
   } // NewDoorInTown
/*----------------------------------------------------------------*/


/**
 * Checks if a sector using this name already exists in the level.
 */
bool CLevel::SectorExists(const char *ch) {
   for (UINT i=0; i<m_NumSectors; i++) {
      if (m_ppSector[i]->NameEqual(ch))
         return true;
      }
   return false;
   } // Release
/*----------------------------------------------------------------*/


/**
 * Checks if a sector using this name already exists in the level.
 */
bool CLevel::SelectSector(const char *ch) {
   for (UINT i=0; i<m_NumSectors; i++) {
      if (m_ppSector[i]->NameEqual(ch)) {
         m_nActive = i;
         return true;
         }
      }
   return false;
   } // Release
/*----------------------------------------------------------------*/


/**
 * Loads the given texture and returns the skin ID.
 */
UINT CLevel::CreateSkinID(ZFXRenderDevice *pDevice, const char *ch,
                          float fTP) {
   static ZFXCOLOR clrW = { 1.0f, 1.0f, 1.0f, 1.0f };
   static ZFXCOLOR clrB = { 0.0f, 0.0f, 0.0f, 1.0f };
   HRESULT hr=ZFX_FAIL;
   UINT ID=0;

   char chPath[MAX_PATH];
   
   bool bTP = (fTP < 1.0f);

   strcpy(chPath, "data/textures/");
   strcat(chPath, ch);

   if ( strcmp(chPath, "data/textures/portal.bmp")==0 ||
        strcmp(chPath, "data/textures/light.bmp")==0 )
      pDevice->GetSkinManager()->AddSkin(&clrW, &clrW, &clrW,
                                         &clrW, 1.0f, &ID);
   else
      pDevice->GetSkinManager()->AddSkin(&clrW, &clrW, &clrB,
                                         &clrW, 1.0f, &ID);

   hr = pDevice->GetSkinManager()->AddTexture(ID,chPath,bTP,fTP,NULL,0);
   
   if (FAILED(hr)) 
      fprintf(CApplication::m_G.pLog, 
              "CLevel::CreateSkin[AddTexture(\"%s\")] failed \n", ch);

   return ID;
   } // CreateSkinID
/*----------------------------------------------------------------*/


/**
 * 
 */
void CLevel::SaveSkin(ZFXSkinManager *pSM, FILE *pFile, UINT n) {
   ZFXSKIN     Skin = pSM->GetSkin(n);
   ZFXMATERIAL Mtrl = pSM->GetMaterial(Skin.nMaterial);
   ZFXCOLOR    Clr[256];
   float       fAlpha=0.0f;
   UCHAR       nNumClr=0;
   char        chTex[MAX_PATH] = "\0";
   UINT        nSize=0;
   UINT        nNumTex=0;
   
   memset(Clr, 0, sizeof(ZFXCOLOR)*256);

   for (nNumTex=0; nNumTex<8; nNumTex++)
      if (Skin.nTexture[nNumTex] > 65000) break;
   
   fwrite(&Skin.bAlpha, sizeof(bool), 1, pFile);
   fwrite(&Mtrl,    sizeof(float), 17, pFile);
   fwrite(&nNumTex, sizeof(UINT), 1, pFile);

   for (UINT i=0; i<nNumTex; i++) {
      strcpy(chTex, pSM->GetTextureName(Skin.nTexture[i],
                                        &fAlpha, Clr, 
                                        &nNumClr));
      nSize = strlen(chTex);
      fwrite(&nSize, sizeof(UINT), 1, pFile);
      fwrite(chTex, nSize, 1, pFile);

      if ( !Skin.bAlpha) continue;

      // write alpha restore information
      fwrite(&fAlpha,  sizeof(float), 1, pFile);
      fwrite(&nNumClr, sizeof(UCHAR), 1, pFile);
      fwrite(Clr, sizeof(ZFXCOLOR), nNumClr, pFile);
      }
   } // SaveSkin
/*----------------------------------------------------------------*/


/**
 * 
 */
void CLevel::LoadSkin(HWND hCB, ZFXSkinManager *pSM, FILE *pFile) {
   ZFXMATERIAL Mtrl;
   ZFXCOLOR    Clr[256];
   HRESULT     hr;
   float fAlpha=0.0f;
   UCHAR nNumClr=0;
   char  ch[MAX_PATH];
   bool  bAlpha=false;
   UINT  nSize=0;
   UINT  nNumTex=0;
   UINT  Dummy=0;

   memset(&Mtrl, 0, sizeof(ZFXMATERIAL));
   memset(&Clr, 0, sizeof(ZFXCOLOR)*256);
   memset(ch, 0, sizeof(char)*MAX_PATH);
   
   fread(&bAlpha,  sizeof(bool),  1, pFile);
   fread(&Mtrl,    sizeof(float), 17, pFile);
   fread(&nNumTex, sizeof(UINT),  1, pFile);

   // create the skin's material now
   pSM->AddSkin(&Mtrl.cAmbient, &Mtrl.cDiffuse, 
                &Mtrl.cEmissive, &Mtrl.cSpecular,
                1.0f, &Dummy);

   // load skin's textures now
   for (UINT i=0; i<nNumTex; i++) {
      fread(&nSize, sizeof(UINT), 1, pFile);
      fread(ch, nSize, 1, pFile);
      ch[nSize] = '\0';
/*
if (strcmp(ch, "data/textures/wall_space_03.bmp")==0)
strcpy(ch, "data/textures/wall_wb_03.bmp");
*/
      if (i==0) cbhAddItem(hCB, &ch[14], NULL, true);

      if (!bAlpha) {
         hr = pSM->AddTexture(Dummy, ch, bAlpha, 1.0f, NULL, 0);
         if (FAILED(hr))
            (CApplication::m_G.pLog, "failed \"%s\"\n", ch);
         continue;
         }

      // read alpha restore information
      fread(&fAlpha,  sizeof(float), 1, pFile);
      fread(&nNumClr, sizeof(UCHAR), 1, pFile);
      fread(Clr, sizeof(ZFXCOLOR), nNumClr, pFile);
      
      hr = pSM->AddTexture(Dummy, ch, bAlpha, fAlpha, Clr, nNumClr);
      if (FAILED(hr))
         (CApplication::m_G.pLog, "failed \"%s\"\n", ch);
      }
   } // LoadSkin
/*----------------------------------------------------------------*/


void CLevel::GetNumLobs(UINT *pV, UINT *pI, UINT *pM, UINT *pPl, 
                        UINT *pPt, UINT *pL, UINT *pE) {
   for (UINT i=0; i<m_NumSectors; i++) {
      if (pV)  *pV  += m_ppSector[i]->GetNumVerts();
      if (pI)  *pI  += m_ppSector[i]->GetNumIndis();
      if (pPl) *pPl += m_ppSector[i]->GetNumPolys();
      if (pPt) *pPt += m_ppSector[i]->GetNumPorts();
      if (pL)  *pL  += m_ppSector[i]->GetNumLights();
      if (pM)  *pM  += m_ppSector[i]->GetNumMeshs();
      if (pE)  *pE  += m_ppSector[i]->GetNumEntities();
      }
   } // GetNumLobs
/*----------------------------------------------------------------*/
