// FILE: CToolbar.cpp

#include "include/CToolbar.h"
#include "include/CApplication.h"

extern CApplication g_MyApp;


#define NUM_TOOLBAR_BUTTONS 22
#define NUM_SEPS             6


/**
 * Initializes the toolbar and hooks it to the window.
 */
HRESULT CToolbar::Init(GLOBALS *pG) {
   TBADDBITMAP tb_bitmap;
 
   m_pDevice = NULL;
   m_pG = pG;

	// create the toolbar
	m_hWndTB = CreateWindowEx(0, TOOLBARCLASSNAME, (LPSTR)NULL, 
                             WS_CHILD|TBSTYLE_FLAT|CCS_ADJUSTABLE|WS_BORDER,
									  0, 0, 32, 32, pG->hWnd, (HMENU)IDR_TOOLBAR, pG->hInst, NULL);

   if (!m_hWndTB) {
      return E_FAIL;
      }

	// send the TB_BUTTONSTRUCTSIZE message, which is required for backward compatibility
	SendMessage(m_hWndTB, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), 0);

	// add the bitmap containing button images to the toolbar
	tb_bitmap.hInst = pG->hInst;
	tb_bitmap.nID   = IDR_TOOLBAR;

	if ( SendMessage(m_hWndTB, TB_ADDBITMAP, 
                    (WPARAM)NUM_TOOLBAR_BUTTONS,
                    (WPARAM)&tb_bitmap) == -1) {
      return E_FAIL;
      }

   if (FAILED(CreateButtons())) return E_FAIL;

   m_Shape = SHP_NONE;

	ShowWindow(m_hWndTB, SW_SHOW);
   return S_OK;
   } // Init
/*----------------------------------------------------------------*/


/**
 * Create the buttons and its message IDs for the toolbar.
 */
HRESULT CToolbar::CreateButtons(void) {
   TBBUTTON tb_button[NUM_TOOLBAR_BUTTONS+NUM_SEPS];

	// fill the TBBUTTON array with button information
   memset(tb_button, 0, sizeof(tb_button));

   for (int i=0, s=0; i<(NUM_TOOLBAR_BUTTONS + NUM_SEPS); i++) {
      if ( (i==3) || (i==6) || (i==10) || (i==18) ||
           (i==20) || (i==24) ) {
	      tb_button[i].fsStyle = TBSTYLE_SEP;
         s++;
         }
      else {
         tb_button[i].iBitmap = i-s;
         tb_button[i].fsStyle = TBSTYLE_BUTTON;
	      tb_button[i].fsState = TBSTATE_ENABLED;
         }
      }

   // set message IDs for the buttons

   // FILE PART OF THE TOOLBAR
	tb_button[0].idCommand  = IDTB_FILE_NEW;
   tb_button[1].idCommand  = IDTB_FILE_OPEN;
   tb_button[2].idCommand  = IDTB_FILE_SAVE;

   tb_button[4].idCommand  = IDTB_MESH_OPEN;
   tb_button[5].idCommand  = IDTB_MESH_SAVE;

   // LEVEL PART OF THE TOOLBAR
   tb_button[7].idCommand  = IDTB_LEVEL_ATTRIBUTES;
   tb_button[8].idCommand  = IDTB_SECTOR_NEW;
   tb_button[9].idCommand  = IDTB_SECTOR_DELETE;

   // SECTOR PART OF THE TOOLBAR
   tb_button[11].idCommand = IDTB_SECTOR_ATTRIBUTES;
   tb_button[12].idCommand = IDTB_SECTOR_AMBIENT;
   tb_button[13].idCommand = IDTB_INSERT_LIGHT;
   tb_button[14].idCommand = IDTB_INSERT_SPAWN;
   tb_button[15].idCommand = IDTB_LIGHT_EDIT;
   tb_button[16].idCommand = IDTB_EDIT_ENTITY;
   tb_button[17].idCommand = IDTB_EDIT_SPAWN;

   // GEOMETRY MANIPULATION
   tb_button[19].idCommand  = IDTB_GEO_INSIDEOUT;

   // PORTAL PART OF THE TOOLBAR
   tb_button[21].idCommand  = IDTB_MAKE_ENTITY;
   tb_button[22].idCommand  = IDTB_MAKE_PORTAL;
   tb_button[23].idCommand  = IDTB_PORTAL_CONNECT;

   // INSERT GEOMETRY TO SECTOR PART OF THE TOOLBAR
   tb_button[25].idCommand = IDTB_INSERT_SHAPE_RECT;
   tb_button[26].idCommand = IDTB_INSERT_SHAPE_CUBE;
   tb_button[27].idCommand = IDTB_INSERT_SHAPE_LIGHT;


	// add the buttons to the toolbar
	if ( !(SendMessage(m_hWndTB, TB_ADDBUTTONS, 
                      (WPARAM)NUM_TOOLBAR_BUTTONS+NUM_SEPS, 
                      (LPARAM)(LPTBBUTTON)&tb_button))) {
      return E_FAIL;
      }

   EnableButton(IDTB_MAKE_PORTAL, false);
   EnableButton(IDTB_EDIT_SPAWN,  false);
   EnableButton(IDTB_MESH_SAVE,   false);
   EnableButton(IDTB_INSERT_SHAPE_RECT, false);
   EnableButton(IDTB_PORTAL_CONNECT, false);

   return S_OK;
   } // CreateButtons
/*----------------------------------------------------------------*/


/**
 * Process messages from toolbar buttons. Return true if any button
 * was handled and false if wCmd was not recognized.
 */
bool CToolbar::MsgProc(WORD wCmd) {
   CSpawnPoint Spawn;
   CLight      Light;

   switch (wCmd) {
      case IDTB_FILE_NEW:
         OnFileNew();
         return true;
      case IDTB_FILE_OPEN:
         OnFileOpen();
         return true;
      case IDTB_FILE_SAVE:
         if (m_pG->Level.HasName()) OnFileSave();
         else OnFileSaveAs();
         return true;

      case IDTB_MESH_OPEN:
         OnPrefabOpen();
         return true;
      case IDTB_MESH_SAVE:
         OnPrefabSave();
         return true;

      case IDTB_LEVEL_ATTRIBUTES:
         DialogBox(m_pG->hInst, "DlgLevelAttrib", 
                   m_pG->hWnd, CB_LevelAttribBox);
         return true;

      case IDTB_SECTOR_NEW:
         OnSectorNew();
         return true;
      case IDTB_SECTOR_ATTRIBUTES:
         DialogBox(m_pG->hInst, "DlgSectorAttrib", 
                   m_pG->hWnd, CB_SectorAttribBox);
         return true;
      case IDTB_SECTOR_AMBIENT:
         DialogBox(m_pG->hInst, "DlgSectorAmbient",
                   m_pG->hWnd, CB_SectorAmbientBox);
         return true;
      case IDTB_SECTOR_DELETE:
         OnSectorDelete();
         return true;

      case IDTB_GEO_INSIDEOUT:
         m_pG->SelBuf.InsideOut();
         m_pG->bNeedRedraw[0] = true;
         m_pG->bNeedRedraw[1] = true;
         m_pG->bNeedRedraw[2] = true;
         m_pG->bNeedRedraw[3] = true;
         return true;

      case IDTB_MAKE_ENTITY:
         OnMakeEntity();
         return true;

      case IDTB_MAKE_PORTAL:
         OnMakePortal();
         return true;

      case IDTB_INSERT_SHAPE_RECT:
         m_Shape = SHP_NONE;
         return true;
      case IDTB_INSERT_SHAPE_CUBE:
         m_Shape = SHP_CUBE;
         DialogBox(m_pG->hInst, "DlgInsertShape", 
                   m_pG->hWnd, CB_InsertShape);
         return true;
      case IDTB_INSERT_SHAPE_LIGHT:
         m_Shape = SHP_LIGHT;
         DialogBox(m_pG->hInst, "DlgInsertShape", 
                   m_pG->hWnd, CB_InsertShape);
         return true;

      case IDTB_INSERT_LIGHT:
         Light.SetPosition(ZFXVector(0,0,0));
         Light.SetColor(1.0f, 1.0f, 1.0f);
         Light.SetRadius(3.0f);
         m_pG->Level.GetSelectedSector()->AddObject( Light );
         m_pG->bChanged = true;
         m_pG->bNeedRedraw[0] = true;
         m_pG->bNeedRedraw[1] = true;
         m_pG->bNeedRedraw[2] = true;
         m_pG->bNeedRedraw[3] = true;
         return true;

      case IDTB_INSERT_SPAWN:
         Spawn.SetPosition(ZFXVector(0,0,0));
         m_pG->Level.GetSelectedSector()->AddObject( Spawn );
         m_pG->bChanged = true;
         m_pG->bNeedRedraw[0] = true;
         m_pG->bNeedRedraw[1] = true;
         m_pG->bNeedRedraw[2] = true;
         m_pG->bNeedRedraw[3] = true;
         return true;

      case IDTB_LIGHT_EDIT:
         DialogBox(m_pG->hInst, "DlgEditLight", 
                   m_pG->hWnd, CB_EditLightBox);
         return true;

      case IDTB_EDIT_ENTITY:
         OnEditEntity();
         return true;

      case IDTB_EDIT_SPAWN:
         OnEditSpawn();
         return true;

      } // switch
   return false;
   } // MsgProc
/*----------------------------------------------------------------*/


void CToolbar::OnFileNew(void) {
   CheckForcedSave();

   m_pG->SelBuf.DestroyWithoutNotify();

   m_pG->bChanged = false;

   SetCurrentDirectory(m_pG->Dir);

   // empty out the level
   m_pG->Level.ClearAll( m_pDevice->GetSkinManager() );
   m_pG->Level.AddSector("Empty Sector");
   m_pG->Level.SelectSector("Empty Sector");
   m_pG->Level.CreateSkinID(m_pDevice, "portal.bmp", 0.35f);
   m_pG->Level.CreateSkinID(m_pDevice, "light.bmp", 0.35f);
   m_pG->Level.CreateSkinID(m_pDevice, "default.bmp", 1.0f);

   // reset combo box
   cbhResetContent(m_pG->hGUI[SECTOR]);
   cbhResetContent(m_pG->hGUI[CSK_LIST]);
   cbhAddItem(m_pG->hGUI[SECTOR], "Empty Sector", NULL, true);
   cbhAddItem(m_pG->hGUI[CSK_LIST], "portal.bmp", NULL, true);
   cbhAddItem(m_pG->hGUI[CSK_LIST], "light.bmp", NULL, true);
   cbhAddItem(m_pG->hGUI[CSK_LIST], "default.bmp", NULL, true);

   // set window text
   SetWindowText(m_pG->hWnd, "PanBox Edit - New Level...");
   } // OnFileNew
/*----------------------------------------------------------------*/


void CToolbar::OnFileOpen(void) {
   static char chFile[MAX_PATH], chTitle[MAX_PATH];
   char chWindow[MAX_PATH];

   // any changes we need to save?
   CheckForcedSave();

   m_pG->FileDlg.SetMode(FMD_LEVEL);

   // open file dialog and load from file
   if ( ( m_pG->FileDlg.DlgFileOpen(m_pG->hWnd, 
                                    chFile,
                                    chTitle)) != 0)
      {
      
      m_pDevice->GetSkinManager()->Reset();

      SetCurrentDirectory(m_pG->Dir);

      if ( m_pG->Level.LoadLevel(chFile, m_pG->hGUI[SECTOR], 
                                 m_pG->hGUI[CSK_LIST], m_pDevice) ) {
         sprintf(chWindow, "PanBox Edit - %s", chFile);
         SetWindowText(m_pG->hWnd, chWindow);
         m_pG->bChanged = false;
         }
      }
   } // OnFileOpen
/*----------------------------------------------------------------*/


void CToolbar::OnFileSaveAs(void) {
   static char chFile[MAX_PATH], chTitle[MAX_PATH];
   char chWindow[MAX_PATH];

   m_pG->FileDlg.SetMode(FMD_LEVEL);

   if ( ( m_pG->FileDlg.DlgFileSave(m_pG->hWnd, 
                                    chFile, 
                                    chTitle)) != 0 )
      {
      if ( m_pG->Level.SaveLevel(chFile, m_pDevice) ) {
         sprintf(chWindow, "PanBox Edit - %s", chFile);
         SetWindowText(m_pG->hWnd, chFile);
         m_pG->bChanged = false;
         }
      }

   m_pG->bNeedRedraw[0] = true;
   m_pG->bNeedRedraw[1] = true;
   m_pG->bNeedRedraw[2] = true;
   m_pG->bNeedRedraw[3] = true;
   } // OnFileSaveAs
/*----------------------------------------------------------------*/


void CToolbar::OnFileSave(void) {
   char chName[MAX_PATH];

   m_pG->Level.GetName(chName);
   if (m_pG->Level.SaveLevel(chName, m_pDevice))
      m_pG->bChanged = false;
   } // OnFileSave
/*----------------------------------------------------------------*/


void CToolbar::OnPrefabSave(void) {
   static char chFile[MAX_PATH], chTitle[MAX_PATH];

   m_pG->FileDlg.SetMode(FMD_PREFAB);

   if ( ( m_pG->FileDlg.DlgFileSave(m_pG->hWnd, 
                                    chFile, 
                                    chTitle)) != 0 ) {
      SetCurrentDirectory(m_pG->Dir);
      m_pG->SelBuf.SaveAsPrefab(chFile);
      }

   m_pG->bNeedRedraw[0] = true;
   m_pG->bNeedRedraw[1] = true;
   m_pG->bNeedRedraw[2] = true;
   m_pG->bNeedRedraw[3] = true;
   } // OnPrefabSave
/*----------------------------------------------------------------*/


void CToolbar::OnPrefabOpen(void) {
   static char chFile[MAX_PATH], chTitle[MAX_PATH];

   m_pG->FileDlg.SetMode(FMD_PREFAB);

   // open file dialog and load from file
   if ( ( m_pG->FileDlg.DlgFileOpen(m_pG->hWnd, 
                                    chFile,
                                    chTitle)) != 0)
      {
      SetCurrentDirectory(m_pG->Dir);
      m_pG->SelBuf.Reset();
      m_pG->Level.GetSelectedSector()->LoadPrefab(chFile, 
                                 g_MyApp.GetRenderDevice(),
                                 &m_pG->SelBuf);
      m_pG->bChanged = true;
      }

   m_pG->bNeedRedraw[0] = true;
   m_pG->bNeedRedraw[1] = true;
   m_pG->bNeedRedraw[2] = true;
   m_pG->bNeedRedraw[3] = true;
   } // OnFileOpen
/*----------------------------------------------------------------*/


void CToolbar::OnSectorNew(void) {
   if ( DialogBox(m_pG->hInst, "DlgNewSector", 
                  m_pG->hWnd, CB_NewSectorBox) == 1)
      m_pG->bChanged = true;

   m_pG->bNeedRedraw[0] = true;
   m_pG->bNeedRedraw[1] = true;
   m_pG->bNeedRedraw[2] = true;
   m_pG->bNeedRedraw[3] = true;
   } // OnSectorNew
/*----------------------------------------------------------------*/


void CToolbar::OnSectorDelete(void) {
   char buffer[1024];
   char chQuest[2048];

   // take care that we have at least one sector
   if ( m_pG->Level.GetNumSectors() <= 1 ) {
      MessageBox(m_pG->hWnd, "Last sector can't be deleted.", 
                 "PanEdit - Notification",
                 MB_OK | MB_ICONINFORMATION);
      return;
      }

   // get selected item from sector-combobox
   GetWindowText(m_pG->hGUI[SECTOR], buffer, 1024);
   sprintf(chQuest, "Delete sector \"%s\"?", buffer);

   // ask if we should really delete it
   if ( MessageBox(m_pG->hWnd, chQuest, "PanEdit - Question",
                   MB_YESNO | MB_ICONQUESTION) == IDNO )
      return;

   cbhRemoveSelectedItem(m_pG->hGUI[SECTOR]);
   m_pG->Level.RemoveSector(buffer);

   cbhSelectFirstItem(m_pG->hGUI[SECTOR]);
   cbhGetSelectedText(m_pG->hGUI[SECTOR], buffer);
   m_pG->Level.SelectSector(buffer);
   m_pG->bChanged = true;

   m_pG->bNeedRedraw[0] = true;
   m_pG->bNeedRedraw[1] = true;
   m_pG->bNeedRedraw[2] = true;
   m_pG->bNeedRedraw[3] = true;
   } // OnSectorDelete
/*----------------------------------------------------------------*/


// used to check and query user if level needs saving
void CToolbar::CheckForcedSave(void) {
   if (!m_pG->bChanged) return;

   if ( MessageBox(m_pG->hWnd, "Unsaved changes. " \
                   "Save level now?", "PanEdit - Warning", 
                   MB_YESNO | MB_ICONWARNING) == IDYES) {
      if (m_pG->Level.HasName()) OnFileSave();
      else OnFileSaveAs();
      }

   m_pG->bNeedRedraw[0] = true;
   m_pG->bNeedRedraw[1] = true;
   m_pG->bNeedRedraw[2] = true;
   m_pG->bNeedRedraw[3] = true;
   } // HandleForcedSave
/*----------------------------------------------------------------*/


// if insert shape button was pressed
void CToolbar::OnInsertShape(bool bInv, bool bPoly, // inverted?, as polygons?
                             ZFXVector vcP,         // position
                             ZFXVector vcD) {       // dimensions
   CGeometry  Geo;
   CPolygon   Poly[8];
   CPolymesh  Mesh;
   CSector   *pSector = m_pG->Level.GetSelectedSector(); 
   UCHAR i=0;

   switch (m_Shape) {

      case SHP_CUBE:
         if (bInv)
            { Geo.CreateCubeInv(&Mesh, Poly, vcP, vcD.x, vcD.y, vcD.z); }
         else 
            { Geo.CreateCube(&Mesh, Poly, vcP, vcD.x, vcD.y, vcD.z); }

         if (bPoly) 
            { for (i=0;i<6;i++) pSector->AddObject(Poly[i]); }
         else
            { pSector->AddObject(Mesh); }
         break;

      case SHP_LIGHT:
         if (bInv)
            { Geo.CreateLightInv(&Mesh, Poly, vcP, vcD.x, vcD.y, vcD.z); }
         else 
            { Geo.CreateLight(&Mesh, Poly, vcP, vcD.x, vcD.y, vcD.z); }

         if (bPoly) 
            { for (i=0;i<8;i++) pSector->AddObject(Poly[i]); }
         else
            { pSector->AddObject(Mesh); }
         break;

      default: return;
      } // switch
   
   m_pG->bChanged = true;
   m_pG->bNeedRedraw[0] = true;
   m_pG->bNeedRedraw[1] = true;
   m_pG->bNeedRedraw[2] = true;
   m_pG->bNeedRedraw[3] = true;
   } // OnInsertShape
/*----------------------------------------------------------------*/


// if edit entity button was pressed
void CToolbar::OnEditEntity(void) {
   CEntity *pEntity=NULL;
   
   pEntity = (LPENTITY)m_pG->SelBuf.GetHeadPointer();

   if (!pEntity) return;

   switch ( pEntity->GetSubType() ) {
      case ENT_DOOR:
         DialogBox(m_pG->hInst, "DlgEditEntDoor", 
                   m_pG->hWnd, CB_EditEntDoor);
         break;

      default: break;
      } // switch
   } // OnEditEntity
/*----------------------------------------------------------------*/


// if edit spawnpoint button was pressed
void CToolbar::OnEditSpawn(void) {
   CSpawnPoint *pSpawn=NULL;
   
   pSpawn = (LPSPAWNPOINT)m_pG->SelBuf.GetHeadPointer();

   if (!pSpawn) return;

   /* do edit stuff here */
   } // OnEditSpawn
/*----------------------------------------------------------------*/


// if make portal button was pressed
void CToolbar::OnMakePortal(void) {
   LPSECTOR  pSector=NULL;
   LPPOLYGON pPoly;
   CPortal   Portal;
   LPPORTAL  pPortal=NULL;

   pSector = m_pG->Level.GetSelectedSector();
   pPoly   = m_pG->SelBuf.IsPolyQuad();

   if ( !pPoly) return;

   // create portal from poy geometry
   pPoly->CreatePortal(&Portal);

   // get a new id for it and add to active sector
   Portal.SetID( m_pG->Level.CreatePortalID() );
   pPortal = (LPPORTAL)pSector->AddObject(Portal);

   m_pG->Level.NewPortalInTown( pPortal );

   // delete the original polygon
   m_pG->SelBuf.DeleteSelectedObjects( & m_pG->Level );
   m_pG->bChanged = true;
   m_pG->bNeedRedraw[0] = true;
   m_pG->bNeedRedraw[1] = true;
   m_pG->bNeedRedraw[2] = true;
   m_pG->bNeedRedraw[3] = true;
   } // OnMakePortal
/*----------------------------------------------------------------*/


// if make entity button was pressed
void CToolbar::OnMakeEntity(void) {
   ENTITYDOOR DoorData;
   LPSECTOR   pSector=NULL;
   LPPOLYMESH pMesh;
   CEntity    Entity;
   LPENTITY   pEntity=NULL;

   int Type = DialogBox(m_pG->hInst, "DlgEntityType", 
                        m_pG->hWnd, CB_EntityType);

   if (Type == -1) return;

   pSector = m_pG->Level.GetSelectedSector();
   pMesh   = (LPPOLYMESH)m_pG->SelBuf.GetHeadPointer();

   if ( !pMesh) return;

   // sliding door entity
   if (Type == ENT_DOOR) {
      DoorData.Axis   = X_AXIS;
      DoorData.fDist  = 1.2f;
      DoorData.fPause = 1.0f;

      // create entity from mesh geometry
      Entity.SetGeometry(pMesh);
      Entity.SetSubType(ENT_DOOR);
      Entity.SetData(DoorData);

      // get a new id for it and add to active sector
      pEntity = (LPENTITY)pSector->AddObject(Entity);
      m_pG->Level.NewDoorInTown(pEntity);
      }
   // unknown entity type
   else {
      return;
      }

   // delete the original polygon
   m_pG->SelBuf.DeleteSelectedObjects( &m_pG->Level );
   m_pG->bChanged = true;
   m_pG->bNeedRedraw[0] = true;
   m_pG->bNeedRedraw[1] = true;
   m_pG->bNeedRedraw[2] = true;
   m_pG->bNeedRedraw[3] = true;
   } // OnMakeEntity
/*----------------------------------------------------------------*/


// enable or disable a button from menu
void CToolbar::EnableButton(UINT id, bool bEnable) {
   SendMessage(m_hWndTB, TB_ENABLEBUTTON, (WPARAM)id, 
               (LPARAM)MAKELONG(bEnable,0)); 
   } // EnableButton
/*----------------------------------------------------------------*/