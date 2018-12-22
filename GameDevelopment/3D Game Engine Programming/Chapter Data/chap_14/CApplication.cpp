// FILE: CApplication.cpp


#include "include/CApplication.h"
#include "include/ZFXRenderDevice.h"
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")



bool RadioChecked(HWND hWnd)
   { return (SendMessage(hWnd,BM_GETCHECK,0,0)== BST_CHECKED); }


GLOBALS CApplication::m_G;

// call this after initializing the window
HRESULT CApplication::Init(HWND hWnd, HINSTANCE hInst) {
   HRESULT hr=ZFX_OK;

   m_NewTime = m_OldTime = timeGetTime();
   m_dt = 0.0f;

   memset(m_fPosX, 0, sizeof(float)*4);
   memset(m_fPosY, 0, sizeof(float)*4);
   memset(m_fPosZ, 0, sizeof(float)*4);
   m_fScale[0] = m_fScale[1] = m_fScale[2] = 10.0f;
   m_vcFix.Set(0.0f,0.0f,0.0f);
   m_fRotX = m_fRotY = 0.0f;
   m_fDistOrtho = 200.0f;

   // init globals struct
   memset(&m_G, 0, sizeof(GLOBALS));
   m_G.hWnd  = hWnd;
   m_G.hInst = hInst;
   m_G.pLog = fopen("log_main.txt", "w");

   // init file dialog
   m_G.FileDlg.Init(hWnd);
   m_G.bChanged = false;

   // load the menu as resource
   m_hm = LoadMenu(m_G.hInst, "PanBoxMenu");
   SetMenu(m_G.hWnd, m_hm);

   // load the toolbar
   m_Toolbar.Init(&m_G);

   CreateGUI();
   
   // presettings
   m_bDrawGrid[0]   = m_bDrawGrid[1]   = true;
   m_bDrawPoints[0] = true;
   m_bDrawPoints[1] = false;
   m_bDrawWired[0]  = true; 
   m_bDrawWired[1]  = false;
   m_bDrawInact[0]  = true;
   m_bDrawInact[1]  = true;
   m_bDrawInactW    = true;
   m_bDrawLight     = false;
   m_G.bNeedRedraw[0] = true;
   m_G.bNeedRedraw[1] = true;
   m_G.bNeedRedraw[2] = true;
   m_G.bNeedRedraw[3] = true;

   m_bIsSelecting       = 255; // = no selecting yet
   m_bIsMovingSelection = 255; // = no moving yet
   m_bIsCreating        = 255; // = no creation is progress

   InvalidateSelectionRect(true);

   m_Axis[0] = Z_AXIS;  // front view
   m_Axis[1] = X_AXIS;  // side view
   m_Axis[2] = Y_AXIS;  // top view

   m_NumVerts = 0;
   m_bSnap    = true;

   if (FAILED (hr = InitRenderer())) return hr;
   
   if (FAILED (hr = InitShaders())) 
      {
      /* shaders only needed for per pixel lighting */;
      }

   // let's start with a new file
   m_Toolbar.OnFileNew();

   return ZFX_OK;
   } // Init
/*----------------------------------------------------------------*/


// call this on exiting the application
void CApplication::Release(void) {

   if (m_pRenderer) {
      delete m_pRenderer;
      m_pRenderer = NULL;
      }
   if (m_G.pLog) {
      fclose(m_G.pLog);
      m_G.pLog = NULL;
      }
   } // Release
/*----------------------------------------------------------------*/
 

void CApplication::Update(void) {
   static float fCount = 0.0f;

   static DWORD dwCursorUpdate = GetTickCount();

   DWORD dwCursorTime = GetTickCount();


   // get cursor information
   if ( (dwCursorTime - dwCursorUpdate) > 1 )
      {
      UpdateCursor();
      dwCursorUpdate = dwCursorTime;
      }

   if (fCount > 120.0f) {
      m_OldTime = timeGetTime();

      RenderTick();
      fCount = 0.0f;
      m_NewTime = timeGetTime();	
	   m_dt =  ((float)(m_NewTime - m_OldTime))/1000.0f;
      }
   else fCount += 1.0f;
   } // Update
/*----------------------------------------------------------------*/


// call this if you want to render the views
void CApplication::RenderTick(void) {
   OnViewOrthogonal(0);
   OnViewOrthogonal(1);
   OnViewOrthogonal(2);
   OnViewPerspective();
   } // RenderTick
/*----------------------------------------------------------------*/


// call this prior to rendering into perspective view
void CApplication::OnViewPerspective() {
   ZFXMatrix mT, mX, mY;
   CSector *pSector = NULL;

   if (!m_G.bNeedRedraw[3]) return;
    
   mT.Identity();
   mT.Translate(-m_vcFix.x, -m_vcFix.y, -m_vcFix.z);
   
   mX.RotaX(m_fRotX);
   mY.RotaY(m_fRotY);
   m_World[3] = mT * mY * mX;
   m_World[3]._41 += m_fPosX[3];
   m_World[3]._42 += m_fPosY[3];
   m_World[3]._43 += m_fPosZ[3];

   // camera and world transform
   m_pRDevice->SetView3D(ZFXVector(1,0,0), ZFXVector(0,1,0), 
                         ZFXVector(0,0,1), ZFXVector(0,0,0));

   // activate perspective child view
   m_pRDevice->SetMode(EMD_PERSPECTIVE, 0);
   m_pRDevice->SetClearColor(0.3f,0.3f,0.3f);
   m_pRDevice->UseWindow(3);
 
   m_pRDevice->SetWorldTransform(&m_World[3]);

   // START RENDERING TO PERSPECTIVE VIEW

   m_pRDevice->BeginRendering(true,true,true);

   // render
   RenderPerspGrid();
   RenderInActive(false);
   RenderGeometry(false, 0.05f);
   RenderEntities(false);

   // render selection buffer
   if (!m_bDrawWired[1]) 
      if (m_bDrawLight)
         m_G.SelBuf.Render(m_pRDevice, 0.05f, false, NULL,
                           &m_G.Level.GetSelectedSector()->
                           GetAmbient());
      else
         m_G.SelBuf.Render(m_pRDevice,0.05f,false,NULL,NULL);

   m_G.SelBuf.Render(m_pRDevice, 0.05f, true, NULL, NULL);

   // those have alpha so render last
   RenderLights(false);
   RenderSpawns(true);
   RenderPortals(false);
           
   m_pRDevice->DrawText(m_nZFXFont, 10, 10, 255,255,255, 
                   "3D View - Look At: (X:%5.2f, Y:%5.2f, Z:%5.2f)  [%4.0f fps]",
                   m_vcFix.x, m_vcFix.y, m_vcFix.z, 1.0f/m_dt);
   // END RENDERING TO PERSPECTIVE VIEW
   m_pRDevice->SetShadeMode(RS_SHADE_SOLID,0,NULL);

   m_pRDevice->EndRendering();
   m_G.bNeedRedraw[3] = false;
   } // OnViewPerspective
/*----------------------------------------------------------------*/

 
// call this prior to rendering into orthogonal view
void CApplication::OnViewOrthogonal(UCHAR n) {
   static ZFXVector vcR(1,0,0);
   static ZFXVector vcU(0,1,0);
   static ZFXVector vcD(0,0,1);
   static ZFXVector vcP(0,0,0);

   if (!m_G.bNeedRedraw[n]) return;

   ZFXVector vcT;
   ZFXMatrix mT;
   float fSize;
   mT.Identity();

   m_pRDevice->SetOrthoScale(m_fScale[n], 0);

   if (m_fScale[n] < 5.0f) fSize = 1.5f;
   else fSize = 1.0f;
 
   // switch to mode and child view
   m_pRDevice->SetMode(EMD_ORTHOGONAL, 0);
   m_pRDevice->SetClearColor(0.3f,0.3f,0.3f);
   m_pRDevice->UseWindow(n);

   if (n==0) { // XY plane: front
      vcP.Set(m_fPosX[n], m_fPosY[n], -m_fDistOrtho);
      m_pRDevice->SetView3D(vcR, vcU,  vcD, vcP);
      }
   if (n==1) { // ZY plane: side
      vcP.Set(m_fDistOrtho, m_fPosY[n], m_fPosZ[n]);
      m_pRDevice->SetView3D(vcD, vcU, vcR*-1, vcP);
      }
   if (n==2) { // XZ plane: top
      vcP.Set(m_fPosX[n], m_fDistOrtho, m_fPosZ[n]);
      m_pRDevice->SetView3D(vcR, vcD, vcU*-1, vcP);
      }
   
   m_pRDevice->SetWorldTransform(NULL);

   // START RENDERING TO PERSPECTIVE VIEW
   m_pRDevice->BeginRendering(true,true,true);

   // render grid
   RenderOrthoGrid(n);


   RenderInActive(true);
   RenderGeometry(true, fSize);
   RenderEntities(true);
   RenderLights(true);
   RenderSpawns(true);
   RenderPortals(true);
   RenderPolyUnderConstr(n);

   // render selection buffer
   if (m_bIsMovingSelection != 255) {
      vcT = (m_vcEndMove-m_vcStartMove);
      m_G.SelBuf.Render(m_pRDevice, fSize, m_bDrawWired[0], 
                        &vcT, &m_G.Level.
                        GetSelectedSector()->GetAmbient());
      }
   else {
      m_G.SelBuf.Render(m_pRDevice, fSize, m_bDrawWired[0], 
                        NULL, &m_G.Level.
                        GetSelectedSector()->GetAmbient());
      }


   if (n==0) m_pRDevice->DrawText(m_nZFXFont, 10, 10, 255, 255, 255,
                           "Front View (Scale:%f)", m_fScale[n]);
   else if (n==1) m_pRDevice->DrawText(m_nZFXFont, 10, 10, 255, 255, 255,
                           "Side View (Scale:%f)", m_fScale[n]);
   else if (n==2) m_pRDevice->DrawText(m_nZFXFont, 10, 10, 255, 255, 255,
                           "Top View (Scale:%f)", m_fScale[n]);

   m_pRDevice->SetShadeMode(RS_SHADE_SOLID, 0, NULL);
   if (m_bIsSelecting == n) RenderSelectionRect();

   // END RENDERING TO PERSPECTIVE VIEW
   m_pRDevice->EndRendering();
   m_G.bNeedRedraw[n] = false;
   } // OnViewOrthogonal
/*----------------------------------------------------------------*/


// check which windows the mouse is over
void CApplication::UpdateCursor(void) {
   ZFXVector vc;
   char buffer[256];
   sprintf(buffer, "(-,-,-)");

   // set different mouse cursors
   if (MouseInside(m_G.hWnd3D[0], NULL)) {
      SetCursor(LoadCursor(NULL, IDC_CROSS));
      m_CurView = 0;
      vc = m_pRDevice->Transform2Dto2D(0,m_fScale[0],NULL,Z_AXIS);
      vc += ZFXVector(m_fPosX[0],m_fPosY[0],m_fPosZ[0]);
      sprintf(buffer, "(%4.3f,%4.3f,%4.3f)", vc.x, vc.y, vc.z);
      }
   else if (MouseInside(m_G.hWnd3D[1], NULL)) {
      SetCursor(LoadCursor(NULL, IDC_CROSS));
      m_CurView = 1;
      vc = m_pRDevice->Transform2Dto2D(1,m_fScale[1],NULL,X_AXIS);
      vc += ZFXVector(m_fPosX[1],m_fPosY[1],m_fPosZ[1]);
      sprintf(buffer, "(%4.3f,%4.3f,%4.3f)", vc.x, vc.y, vc.z);
      }

   else if (MouseInside(m_G.hWnd3D[2], NULL)) {
      SetCursor(LoadCursor(NULL, IDC_CROSS));
      m_CurView = 2;
      vc = m_pRDevice->Transform2Dto2D(2,m_fScale[2],NULL,Y_AXIS);
      vc += ZFXVector(m_fPosX[2],m_fPosY[2],m_fPosZ[2]);
      sprintf(buffer, "(%4.3f,%4.3f,%4.3f)", vc.x, vc.y, vc.z);
      }

   else if (MouseInside(m_G.hWnd3D[3], NULL)) {
      SetCursor(LoadCursor(NULL, IDC_CROSS));
      m_CurView = 3;
      }
   
   else {
      SetCursor(LoadCursor(NULL, IDC_ARROW));
      m_CurView = -1;
      }

   SetWindowText(m_G.hGUI[MOUSE], buffer);
   } // CheckCursorType
/*----------------------------------------------------------------*/


// call once on closing the main window, close only on return true
bool CApplication::OnClose(void) {
   if (CheckForcedSave()) {
      DestroyMenu(m_hm);
      DestroyWindow(m_G.hWnd);
      return true;
      }
   return false;
   } // OnClose
/*----------------------------------------------------------------*/


// call this each time the delete key is pressed
void CApplication::OnDeleteKey(void) {
   m_G.SelBuf.DeleteSelectedObjects( &m_G.Level );
   m_G.bChanged = true;
   OnSelBufChange();
   } // OnDeleteKey
/*----------------------------------------------------------------*/


// call this whenever the mouse moved
void CApplication::OnMouseMove(WPARAM wParam, LPARAM lParam) {
   long lDiffY, lDiffX;
   int  n = m_CurView;
   float fDeltaT, fDeltaS;

   // return if selecting in ortho view
   if (m_bIsSelecting != 255) {
      InvalidateView(m_bIsSelecting);
      return;
      }

   // make sure to render poly under construction
   if (m_bIsCreating != 255) {
      InvalidateView(0);
      InvalidateView(1);
      InvalidateView(2);
      }

   m_ptCursorOld   = m_ptCursorNew;
   m_ptCursorNew.x = LOWORD(lParam);
   m_ptCursorNew.y = HIWORD(lParam);

   lDiffY = m_ptCursorNew.y - m_ptCursorOld.y;
   lDiffX = m_ptCursorNew.x - m_ptCursorOld.x;

   if (m_fScale[n] < 3.0f) {
      fDeltaT = 0.08f;
      fDeltaS = 0.1f;
      }
   else if (m_fScale[n] < 15.0f) {
      fDeltaT = 0.2f;
      fDeltaS = 0.5f;
      }
   else {
      fDeltaT = 0.8f;
      fDeltaS = 1.0f;
      }

   // cursor in perspective child view
   if (m_CurView == 3) {

      if (wParam & MK_CONTROL)  return;
      if (wParam & MK_SHIFT)    return;

      // rotating
      if ( wParam & MK_LBUTTON ) {
         if (lDiffY < 0)      m_fRotX += 0.05f;
         else if (lDiffY > 0) m_fRotX -= 0.05f;
         
         if (lDiffX < 0)      m_fRotY += 0.05f;
         else if (lDiffX > 0) m_fRotY -= 0.05f;
         InvalidateView(3);
         }
      // translating
      if ( wParam & MK_RBUTTON ) {
         if (lDiffY < 0)      m_fPosY[3] += 0.08f;
         else if (lDiffY > 0) m_fPosY[3] -= 0.08f;
         
         if (lDiffX < 0)      m_fPosX[3] -= 0.08f;
         else if (lDiffX > 0) m_fPosX[3] += 0.08f;
         InvalidateView(3);
         }
      // zooming
      if ( wParam & MK_MBUTTON ) {
         if (lDiffY < 0)      m_fPosZ[3] += 0.30f;
         else if (lDiffY > 0) m_fPosZ[3] -= 0.30f;
         InvalidateView(3);
         }
      } // persp. wnd
   // cursor in orthogonal child view
   else {
      // translating ortho view
      if ( wParam & MK_RBUTTON ) {
         if (lDiffY < 0) {
            if ((n==0) || (n==1)) m_fPosY[n] -= fDeltaT;
            else if (n==2) m_fPosZ[n] -= fDeltaT;
            }
         else if (lDiffY > 0) {
            if ((n==0) || (n==1)) m_fPosY[n] += fDeltaT;
            else if (n==2) m_fPosZ[n] += fDeltaT;
            }
         
         if (lDiffX < 0) {
            if ((n==0) || (n==2)) m_fPosX[n] += fDeltaT;
            else if (n==1)  m_fPosZ[n] += fDeltaT;
            }
         else if (lDiffX > 0) {
            if ((n==0) || (n==2)) m_fPosX[n] -= fDeltaT;
            else if (n==1) m_fPosZ[n] -= fDeltaT;
            }
         InvalidateView(n);
         }
      // zooming ortho view
      else if ( wParam & MK_MBUTTON ) {
         if (lDiffY < 0)      m_fScale[n] -= fDeltaS;
         else if (lDiffY > 0) m_fScale[n] += fDeltaS;

         if (m_fScale[n] < 0.5f) m_fScale[n] = 0.5f;
         InvalidateView(n);
         }
      // translating selection buffer
      else if ( wParam & MK_LBUTTON ) {
         if (m_bIsMovingSelection != 255) {
            m_vcEndMove = m_pRDevice->Transform2Dto2D(
                           m_bIsMovingSelection, 
                           m_fScale[m_bIsMovingSelection], 
                           NULL,  
                           m_Axis[m_bIsMovingSelection]);
            
            // add translation of viewport
            m_vcEndMove.x += m_fPosX[m_bIsMovingSelection];
            m_vcEndMove.y += m_fPosY[m_bIsMovingSelection];
            m_vcEndMove.z += m_fPosZ[m_bIsMovingSelection];
            CGeometry::Snap(&m_vcEndMove, m_fScale[m_bIsMovingSelection]);
            m_G.bChanged = true;
            }
         InvalidateAllViews();
         }
      } // if [ORTHO VIEW]
   } // OnMouseMove
/*----------------------------------------------------------------*/


// call this whenever the mouse is clicked
void CApplication::OnMouseClick(bool bDown, WPARAM wParam, LPARAM lParam) {
   ZFXVector  vc;
   CSector   *pSector;
   POINT      pt;
   ZFXRay     Ray;

   UINT bCtrl = wParam & MK_CONTROL;
   UINT bShft = wParam & MK_SHIFT;

   // get mouse screen position
   GetCursorPos(&pt);


   // RIGHT MOUSE BUTTON
   if (wParam & MK_RBUTTON) {
      // if polygon tool then end creation
      if (m_CurrentTool == TL_POLYGON) {
         OnPolyCreationEvent(true, NULL);
         InvalidateView(m_bIsCreating);
         }
      }

   // EXIT IF NOT LEFT MOUSE BUTTON
   if ( ! (wParam & MK_LBUTTON) ) return;

   // WHILE BUILDING SELECTION RECT
   if (m_bIsSelecting != 255) {
      OrthoSelection(bDown, pt);
      InvalidateView(m_bIsSelecting);
      return;
      }

   // WHILE MOVING SELECTION IN ORTHO
   if (m_bIsMovingSelection != 255) {
      // on release button end translation
      if (!bDown) {
         InvalidateAllViews();
         m_bIsMovingSelection = 255;
         m_G.SelBuf.Translate(m_vcEndMove-m_vcStartMove);
         }
      }

   // OTHER ACTION IN ANY ORTHO VIEW
   if (m_CurView < 3) {
      // get point of click in world coordinates
      vc = m_pRDevice->Transform2Dto2D(m_CurView, 
                                       m_fScale[m_CurView], 
                                       NULL,  
                                       m_Axis[m_CurView]);
      vc.x += m_fPosX[m_CurView];
      vc.y += m_fPosY[m_CurView];
      vc.z += m_fPosZ[m_CurView];

      // if selection tool then select or move
      if (m_CurrentTool == TL_SELECT) {
         // start moving selection in ortho view
         if (m_G.SelBuf.IsInside(vc, m_Axis[m_CurView])) {
            // portals cannot be moved
            if (m_SelectMode != SM_PORTAL) {
               m_bIsMovingSelection = m_CurView;
               CGeometry::Snap(&vc, m_fScale[m_CurView]);
               m_vcStartMove = vc;
               m_vcEndMove   = vc;
               }
            }
         // beginn building ortho selection rect
         else {
            OrthoSelection(bDown, pt);
            InvalidateView(m_CurView);
            }
         }
      // if polygon tool then create it
      else if ( (m_CurrentTool == TL_POLYGON) && bDown) {
         CGeometry::Snap(&vc, m_fScale[m_CurView]);
         OnPolyCreationEvent(false, &vc);
         InvalidateAllViews();
         }
      }

   // OTHER ACTION IN PERSPECTIVE VIEW
   else if (m_CurView == 3) {

      // position relative to client area
      ScreenToClient(m_G.hWnd3D[3], &pt);

      // CONTROL KEY IS DOWN
      if ( bCtrl ) {
         // reset if shift is not pressed
         if (!bShft) m_G.SelBuf.Reset();

         // switch to poly picking if verts are active
         if (m_SelectMode == SM_VERTEX) {
            SendMessage(m_G.hGUI[CSM_POLYGON],BM_SETCHECK,(WPARAM)BST_CHECKED,0);
            SendMessage(m_G.hGUI[CSM_VERTEX],BM_SETCHECK,(WPARAM)BST_UNCHECKED,0);
            SetSelectMode(SM_POLYGON);
            }

         // get world ray from mouse position
         m_pRDevice->Transform2Dto3D(pt, &Ray.m_vcOrig, &Ray.m_vcDir);

/*DEBUG THIS SHIT*/ Ray.m_vcOrig.x = Ray.m_vcOrig.y = Ray.m_vcOrig.z = 0.0f;
   
         Ray.DeTransform(m_World[3]);
   
         // now pick in the selected sector
         pSector = m_G.Level.GetSelectedSector();
         pSector->Pick(m_SelectMode, Ray, &m_G.SelBuf);

         OnSelBufChange();
         } // control key
      } // persp. view
   } // OnMouseClick
/*----------------------------------------------------------------*/


// call upon left mouse button down or up event in ortho view
void CApplication::OrthoSelection(bool bStart, POINT ptCursor) {
   CSector *pSector;
   ZFXAabb aabb;
   UCHAR   n;

   // if: start of selection
   if (bStart) {
      m_bIsSelecting = n = m_CurView;
      m_vcSel[0] = m_pRDevice->Transform2Dto2D(n,m_fScale[n],NULL,m_Axis[n]);
      m_vcSel[0] += ZFXVector(m_fPosX[n], m_fPosY[n], m_fPosZ[n]);
      return;
      }

   // in which window did selection start?
   n = m_bIsSelecting;

   // get endpoint of selection
   m_vcSel[1] = m_pRDevice->Transform2Dto2D(n, m_fScale[n],NULL,m_Axis[n]);
   m_vcSel[1] += ZFXVector(m_fPosX[n], m_fPosY[n], m_fPosZ[n]);

   if (m_Axis[n] == X_AXIS) {
      m_vcSel[0].x = -m_fDistOrtho;
      m_vcSel[1].x =  m_fDistOrtho;
      }
   else if (m_Axis[n] == Y_AXIS) {
      m_vcSel[0].y = -m_fDistOrtho;
      m_vcSel[1].y =  m_fDistOrtho;
      }
   else {
      m_vcSel[0].z = -m_fDistOrtho;
      m_vcSel[1].z =  m_fDistOrtho;
      }

   m_bIsSelecting = 255;

   // build min/max vectors
   aabb.vcMax.x = ZFXMAX(m_vcSel[0].x, m_vcSel[1].x);
   aabb.vcMax.y = ZFXMAX(m_vcSel[0].y, m_vcSel[1].y);
   aabb.vcMax.z = ZFXMAX(m_vcSel[0].z, m_vcSel[1].z);
   aabb.vcMin.x = ZFXMIN(m_vcSel[0].x, m_vcSel[1].x);
   aabb.vcMin.y = ZFXMIN(m_vcSel[0].y, m_vcSel[1].y);
   aabb.vcMin.z = ZFXMIN(m_vcSel[0].z, m_vcSel[1].z);
   aabb.vcCenter = (aabb.vcMax + aabb.vcMin)/2.0f;

   // now select selected stuff :)
   m_G.SelBuf.Reset();
   pSector = m_G.Level.GetSelectedSector();
   pSector->SelectIntersecting(m_SelectMode, &m_G.SelBuf, 
                               aabb, m_Axis[n]);
   OnSelBufChange();
   } // OrthoSelection
/*----------------------------------------------------------------*/


// do we need to save before continuing, returns if saved afterwards
bool CApplication::CheckForcedSave(void) {
   if (!m_G.bChanged) return true;

   if ( MessageBox(m_G.hWnd, "Unsaved changes. " \
                   "Save level now?", "PanEdit - Warning", 
                   MB_YESNO | MB_ICONWARNING) == IDYES) {
      if ( m_G.Level.HasName() ) m_Toolbar.OnFileSave();
      else m_Toolbar.OnFileSaveAs();
      return (!m_G.bChanged);
      }
   return true;
   } // CheckForcedSave
/*----------------------------------------------------------------*/


// proceed messages from the menu
bool CApplication::MsgProcMenu(WORD wCmd) {
   int i=0;

   InvalidateAllViews();

   switch (wCmd) {

      // FILE MENU:
      // ==========
      case IDM_FILE_NEW:
         m_Toolbar.OnFileNew();
         return true;
      case IDM_FILE_OPEN:
         m_Toolbar.OnFileOpen();
         return true;
      case IDM_FILE_SAVE:
         if (m_G.Level.HasName()) m_Toolbar.OnFileSave();
         else m_Toolbar.OnFileSaveAs();
         return true;
      case IDM_FILE_SAVE_AS:
         m_Toolbar.OnFileSaveAs();
         return true;

      // EDIT MENU:
      // ==========
      case IDM_EDIT_SNAP:
         m_bSnap = ChangeCheckState(IDM_EDIT_SNAP);
         return true;
      case IDM_EDIT_SNAPVERTS:
         m_G.SelBuf.Snap();
         return true;
      case IDM_EDIT_MERGE:
         if (m_SelectMode == SM_POLYGON) {
            m_G.SelBuf.MergePolysToMesh(
            m_G.Level.GetSelectedSector(),
            m_G.hWnd);
            OnSelBufChange();
            }
         else {
            m_G.SelBuf.MergeMeshs(
            m_G.Level.GetSelectedSector());
            OnSelBufChange();
            }
         return true;
      case IDM_EDIT_INSIDEOUT:
         m_G.SelBuf.InsideOut();
         m_G.bChanged = true;
         return true;
      case IDM_EDIT_FRAGMENT:
         m_G.SelBuf.FragmentMeshs(
            m_G.Level.GetSelectedSector() );
         m_G.bChanged = true;
         OnSelBufChange();
         return true;
      case IDM_EDIT_INSERTCOPY:
         m_G.SelBuf.CopySelectedObjects(
            m_G.Level.GetSelectedSector() );
         m_G.bChanged = true;
         return true;
      case IDM_EDIT_DELETE:
         OnDeleteKey();
         return true;
      case IDM_EDIT_SELECTALL:
         m_G.SelBuf.Reset();
         m_G.Level.GetSelectedSector()->SelectAll(m_SelectMode, &m_G.SelBuf);
         OnSelBufChange();
         return true;
      case IDM_EDIT_ROTA_X:
         m_G.SelBuf.Rotate(X_AXIS, (float)ZFXPI2);
         m_G.bChanged = true;
         return true;
      case IDM_EDIT_ROTA_Y:
         m_G.SelBuf.Rotate(Y_AXIS, (float)ZFXPI2);
         m_G.bChanged = true;
         return true;
      case IDM_EDIT_ROTA_Z:
         m_G.SelBuf.Rotate(Z_AXIS, (float)ZFXPI2);
         m_G.bChanged = true;
         return true;
      case IDM_EDIT_ROTA_A:
         DialogBox(m_G.hInst, "DlgRotate", 
                   m_G.hWnd, CB_RotateObject);
         m_G.bChanged = true;
         return true;
      case IDM_EDIT_MIRROR_X:
         m_G.SelBuf.Mirror(X_AXIS);
         m_G.bChanged = true;
         return true;
      case IDM_EDIT_MIRROR_Y:
         m_G.SelBuf.Mirror(Y_AXIS);
         m_G.bChanged = true;
         return true;
      case IDM_EDIT_MIRROR_Z:
         m_G.SelBuf.Mirror(Z_AXIS);
         m_G.bChanged = true;
         return true;


      // CAMERA MENU:
      // ============
      case IDM_CAM_TO_ORIG:
         m_vcFix.Set(0,0,0);
         m_fPosX[3] = 0.0f;
         m_fPosY[3] = 0.0f;
         m_fPosZ[3] = 5.0f;
         return true;
      case IDM_CAM_TO_SEL:
         m_vcFix = m_G.SelBuf.GetCenter();
         m_fPosX[3] = 0.0f;
         m_fPosY[3] = 0.0f;
         m_fPosZ[3] = 5.0f;
         return true;
      case IDM_CAM_CENTER:
         for (i=0; i<3; i++) {
            m_fPosX[i]  = 0.0f;
            m_fPosY[i]  = 0.0f;
            m_fPosZ[i]  = 0.0f;
            m_fScale[i] = 10.0f;
            }
         return true;


      // INSERT MENU:
      // ============
      case IDM_INSERT_MODELL:
         MessageBeep(0);
         return true;

      case IDM_INSERT_LIGHT:
         MessageBeep(0);
         return true;


      // VIEWPORT MENU:
      // ==============
      case IDM_PORTS_GRID3D:
         m_bDrawGrid[1] = ChangeCheckState(IDM_PORTS_GRID3D);
         return true;
      case IDM_PORTS_VERTS3D:
         m_bDrawPoints[1] = ChangeCheckState(IDM_PORTS_VERTS3D);
         return true;
      case IDM_PORTS_WIRE3D:
         m_bDrawWired[1] = ChangeCheckState(IDM_PORTS_WIRE3D);
         return true;
      case IDM_PORTS_LIGHT:
         m_bDrawLight = ChangeCheckState(IDM_PORTS_LIGHT);
         return true;
      case IDM_PORTS_INACTIVE3D:
         m_bDrawInact[1] = ChangeCheckState(IDM_PORTS_INACTIVE3D);
         return true;
      case IDM_PORTS_INACTIVE3DW:
         m_bDrawInactW = ChangeCheckState(IDM_PORTS_INACTIVE3DW);
         return true;
      case IDM_PORTS_GRID:
         m_bDrawGrid[0] = ChangeCheckState(IDM_PORTS_GRID);
         return true;
      case IDM_PORTS_VERTS:
         m_bDrawPoints[0] = ChangeCheckState(IDM_PORTS_VERTS);
         return true;
      case IDM_PORTS_WIRE:
         m_bDrawWired[0] = ChangeCheckState(IDM_PORTS_WIRE);
         return true;
      case IDM_PORTS_INACTIVE:
         m_bDrawInact[0] = ChangeCheckState(IDM_PORTS_INACTIVE);
         return true;

      // HELP MENU:
      // ==========
      case IDM_HELP_INDEX:
         MessageBeep(0);
         return true;

      case IDM_HELP_INFO:
         DialogBox(m_G.hInst, "DlgInfoZFX", m_G.hWnd, CB_InfoBox);
         return true;

      } // switch

   return false;
   } // MsgProcMenu
/*----------------------------------------------------------------*/


// proceed messages from the GUI (buttons and stuff)
bool CApplication::MsgProcGUI(WORD wCmd) {
   char buffer[1024];
   UINT nID=0;

   InvalidateAllViews();

   // set focus back to receive WM_KEYDOWN messages
   // in main window, combobx is special case here
   if ( (wCmd != IDC_SECTOR) &&
        (wCmd != IDC_SKLIST) ) 
        SetFocus(m_G.hWnd);

   switch (wCmd) {
      // COMBO-BOX ACTIVE SECTOR
      case IDC_SECTOR:
         GetWindowText(m_G.hGUI[SECTOR], buffer, 1024);
         m_G.Level.SelectSector(buffer);
         return true;

      // RADIO CONTROL SELECTION MODE
      case IDC_SMMESH:
         if ( RadioChecked(m_G.hGUI[ CSM_MESH ]) )
            SetSelectMode(SM_MESH);
         return true;
      case IDC_SMPOLYGON:
         if ( RadioChecked(m_G.hGUI[ CSM_POLYGON ]) )
            SetSelectMode(SM_POLYGON);
         return true;
      case IDC_SMVERTEX:
         if ( RadioChecked(m_G.hGUI[ CSM_VERTEX ]) )
            SetSelectMode(SM_VERTEX);
         return true;
      case IDC_SMPORTAL:
         if ( RadioChecked(m_G.hGUI[ CSM_PORTAL ]) )
            SetSelectMode(SM_PORTAL);
         return true;
      case IDC_SMENTITY:
         if ( RadioChecked(m_G.hGUI[ CSM_ENTITY ]) )
            SetSelectMode(SM_ENTITY);
         return true;
      case IDC_SMLIGHT:
         if ( RadioChecked(m_G.hGUI[ CSM_LIGHT ]) )
            SetSelectMode(SM_LIGHT);
         return true;
      case IDC_SMSPAWN:
         if ( RadioChecked(m_G.hGUI[ CSM_SPAWN ]) )
            SetSelectMode(SM_SPAWN);
         return true;

      // PRESELECTION CHOICES [ALL, NONE, INVERT]
      case IDC_SALL:
         m_G.SelBuf.Reset();
         m_G.Level.GetSelectedSector()->SelectAll(m_SelectMode, &m_G.SelBuf);
         OnSelBufChange();
         return true;
      case IDC_SNONE:
         m_G.SelBuf.Reset();
         OnSelBufChange();
         return true;
      case IDC_SINV:
         m_G.SelBuf.DestroyWithoutNotify();
         m_G.Level.GetSelectedSector()->SelectInvert(m_SelectMode, &m_G.SelBuf);
         OnSelBufChange();
         return true;


      // HIDE MENU OPTIONS
      case IDC_HNONE:
         m_G.Level.GetSelectedSector()->HideNone();
         return true;
      case IDC_HSEL:
         m_G.Level.GetSelectedSector()->HideSelected(m_SelectMode);
         m_G.SelBuf.Reset();
         OnSelBufChange();
         return true;
      case IDC_HUNS:
         m_G.Level.GetSelectedSector()->HideUnselected();
         return true;
      case IDC_HINV:
         m_G.Level.GetSelectedSector()->HideInvert();
         m_G.SelBuf.Reset();
         OnSelBufChange();
         return true;
      case IDC_H3D:
         m_b3DHide = (SendMessage(m_G.hGUI[CH_3D],BM_GETCHECK,
                                 BST_CHECKED,0) == BST_CHECKED);
         return true;


      // TOOL MENU OPTIONS
      // m_NumVerts to 0 equals aborting poly creation
      case IDC_TSEL:
         m_CurrentTool = TL_SELECT;
         m_NumVerts = 0;
         return true;
      case IDC_TTRANS:
         m_CurrentTool = TL_TRANSLATE;
         m_NumVerts = 0;
         return true;
      case IDC_TROTA:
         m_CurrentTool = TL_ROTATE;
         m_NumVerts = 0;
         return true;
      case IDC_TPOLY:
         m_CurrentTool = TL_POLYGON;
         m_NumVerts = 0;
         return true;

      // TEXTURE COORDINATES MENU OPTIONS
      case IDC_TCX:
         m_G.SelBuf.TextureRemap(X_AXIS);
         return true;
      case IDC_TCY:
         m_G.SelBuf.TextureRemap(Y_AXIS);
         return true;
      case IDC_TCZ:
         m_G.SelBuf.TextureRemap(Z_AXIS);
         return true;
      case IDC_TCDLG:
         m_G.hTexDlg = CreateDialog(m_G.hInst, "DlgTexture", 
                                    m_G.hWnd, CB_TextureBox);
         return true;
      
      // SKIN SELECT/EDIT/APPLY MENU
      case IDC_SKLIST:
         if (cbhGetSelectedIndex(m_G.hGUI[CSK_LIST])<2)
            EnableWindow(m_G.hGUI[CSK_EDIT], FALSE);
         else
            EnableWindow(m_G.hGUI[CSK_EDIT], TRUE);
         return true;
      case IDC_SKNEW:
         SetCurrentDirectory(m_G.Dir);
         DialogBox(m_G.hInst, "DlgAddSkin", 
                   m_G.hWnd, CB_NewSkinBox);
         return true;
      case IDC_SKEDIT:
         SetCurrentDirectory(m_G.Dir);
         DialogBox(m_G.hInst, "DlgEditSkin", 
                   m_G.hWnd, CB_EditSkinBox);
         return true;
      case IDC_SKMAT:
         DialogBox(m_G.hInst, "DlgEditMaterial", 
                   m_G.hWnd, CB_EditMaterialBox);
         return true;
      case IDC_SKAPPLY:
         // get selected skin but skin portal and light
         // skins which always take up first two slots
         nID = cbhGetSelectedIndex(m_G.hGUI[CSK_LIST]);
         if (nID>1) m_G.SelBuf.SetSkinTo(nID);
         return true;
      } // switch

   return false;
   } // MsgProcGUI
/*----------------------------------------------------------------*/
 

/**
 * Change mode of selection if needed.
 */
void CApplication::SetSelectMode(SELECTMODE sm) {
   if (m_SelectMode != sm) {
      // set mode
      m_SelectMode = sm;

      // clear selection buffer
      m_G.SelBuf.Reset();

      // set selected objects display 
      OnSelBufChange();
      }
   } // SetSelectMode
/*----------------------------------------------------------------*/


/**
 * Checks if the cursor position is inside the given window and
 * returns to cursor position relative to upper left corner of
 * that window in the second parameter if true.
 */ 
bool CApplication::MouseInside(HWND hWnd, POINT *pPt) {
   POINT ptC;
   RECT  rc;

   GetWindowRect(hWnd, &rc);
   GetCursorPos(&ptC);

   if (ptC.x < rc.left)   return false;
   if (ptC.x > rc.right)  return false;
   if (ptC.y < rc.top)    return false;
   if (ptC.y > rc.bottom) return false;
   
   if (pPt) {
      ScreenToClient(hWnd, &ptC);
      pPt->x = ptC.x;
      pPt->y = ptC.y;
      }
   return true;
   } // MouseInside
/*----------------------------------------------------------------*/


// Startup ZFXRenderer and ZFXRenderDevice
HRESULT CApplication::InitRenderer(void) {
   HRESULT  hr=ZFX_OK;
   UINT     n=0;
   RECT     rcWnd;
   int      x, y;


   // CREATE THE FOUR VIEWS:
   // ======================
   GetClientRect(m_G.hWnd, &rcWnd);

   rcWnd.right  -= 200;
   rcWnd.top    += 30;
   rcWnd.bottom -= 30;

   for (int i=0; i<4; i++) {
      if ( (i==0) || (i==2) ) x = 5;
      else x = rcWnd.right/2;

      if ( (i==0) || (i==1) ) y = 5 + rcWnd.top;
      else y = rcWnd.bottom/2 + rcWnd.top;

      m_G.hWnd3D[i] = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("static"), 
                           NULL, WS_CHILD | SS_BLACKRECT | WS_VISIBLE,
                           x+210, y, rcWnd.right/2-10, rcWnd.bottom/2-10, 
                           m_G.hWnd, NULL, m_G.hInst, NULL);
      }
 
   // RENDER DEVICE:
   // ==============
   m_pRenderer = new ZFXRenderer(m_G.hInst);
   if (FAILED( m_pRenderer->CreateDevice("Direct3D") ))
      return ZFX_FAIL;

   if ( !(m_pRDevice = m_pRenderer->GetDevice()) )
      return ZFX_FAIL;

   // init render device
   if (FAILED( hr = m_pRDevice->InitWindowed(m_G.hWnd, m_G.hWnd3D, 4, true) )) {
      delete m_pRenderer;
      m_pRenderer = NULL;
      return hr;
      }
 
   // create a font object to render stats
   if (FAILED( m_pRDevice->CreateFont("Arial", 0, false, false,
                                      false, 20, &m_nFontID) )) {
      fprintf(m_G.pLog, "error: ZFXRenderDevice::CreateFont failed\n");
      return ZFX_FAIL;
      }

   m_pRDevice->UseShaders(false);
 
   // create a nice font object
   m_pRDevice->CreateFont("Arial", 0, false, false, false, 8, &m_nZFXFont);
   m_Toolbar.SetDevice(m_pRDevice);
   return ZFX_OK;
   } // InitRenderer
/*----------------------------------------------------------------*/


// renders a grid to the given child view
void CApplication::RenderOrthoGrid(UCHAR n) {
   POINT ptMin = { 0, 0 }, ptMax;
   RECT rc;
   ZFXVector vcMin, vcMax, vcT1, vcT2;
   ZFXMatrix mRota;
   float     fA, fB, fC, fD;
   LVERTEX  *pVerts=NULL;
   int       i=0, j=0;

   if ( !m_bDrawGrid[0] ) return;

   bool bSnapGrid = (m_fScale[n] <= 15.0f);

   static DWORD dwUnit = DWORD_ARGB(255,61,61,61);
   static DWORD dwMean = DWORD_ARGB(255,148,148,148);
   static DWORD dwSnap = DWORD_ARGB(255,71,71,71);

   // orthogonal
   GetClientRect(m_G.hWnd3D[n], &rc);
   ptMax.x = rc.right;
   ptMax.y = rc.bottom;
   vcT1 = m_pRDevice->Transform2Dto2D(n, m_fScale[n], &ptMin, m_Axis[n]);
   vcT2 = m_pRDevice->Transform2Dto2D(n, m_fScale[n], &ptMax, m_Axis[n]);
   vcT1 += ZFXVector( m_fPosX[n], m_fPosY[n], m_fPosZ[n]);
   vcT2 += ZFXVector( m_fPosX[n], m_fPosY[n], m_fPosZ[n]);
   
   vcMin.x = ZFXMIN(floorf(vcT1.x), floorf(vcT2.x));
   vcMax.x = ZFXMAX( ceilf(vcT1.x),  ceilf(vcT2.x));
   vcMin.y = ZFXMIN(floorf(vcT1.y), floorf(vcT2.y));
   vcMax.y = ZFXMAX( ceilf(vcT1.y),  ceilf(vcT2.y));
   vcMin.z = ZFXMIN(floorf(vcT1.z), floorf(vcT2.z));
   vcMax.z = ZFXMAX( ceilf(vcT1.z),  ceilf(vcT2.z));
  
   // rotate the grid with respect to viewpoint
   if (n==0) mRota.Identity();
   else if (n==1) mRota.RotaY(-1.570796f);
   else if (n==2) mRota.RotaX(1.570796f);
   
   m_pRDevice->SetWorldTransform(&mRota);
   m_pRDevice->SetDepthBufferMode(RS_DEPTH_NONE);
   
   // HORIZONTAL LINES FIRST
   if (n==0) { fA=vcMin.x; fB=vcMax.x; fC=vcMin.y; fD=vcMax.y; }
   if (n==1) { fA=vcMin.z; fB=vcMax.z; fC=vcMin.y; fD=vcMax.y; }
   if (n==2) { fA=vcMin.x; fB=vcMax.x; fC=vcMin.z; fD=vcMax.z; }
   
   if (bSnapGrid)
      pVerts = (LVERTEX*)malloc(sizeof(LVERTEX)*(int(fD-fC))*9*2*2);
   else
      pVerts = (LVERTEX*)malloc(sizeof(LVERTEX)*(int(fD-fC))*2);

   // horizontal lines
   for (i=0; fC<fD; fC++) {
      pVerts[i].x = fA;
      pVerts[i].y = pVerts[i+1].y = fC;
      pVerts[i].z = pVerts[i+1].z = 0.0f;
      pVerts[i+1].x = fB;
      
      if (pVerts[i].y == 0.0f) {
         pVerts[i].Color   = dwMean;
         pVerts[i+1].Color = dwMean;
         }
      else {
         pVerts[i].Color   = dwUnit;
         pVerts[i+1].Color = dwUnit;
         }
      i+= 2;
      
      // render snap grid
      if (bSnapGrid) {
         for (float y=0.1f; y<1.0f; y+=0.1f) {
            pVerts[i].x = fA;
            pVerts[i+1].x = fB;
            pVerts[i].y = pVerts[i+1].y = fC + y;
            pVerts[i].z = pVerts[i+1].z = 0.0f;
            pVerts[i].Color = dwSnap;
            pVerts[i+1].Color = dwSnap;
            i += 2;
            }
         }
      } // for [horz lines]
   
   m_pRDevice->GetVertexManager()->RenderLines(VID_UL, i, pVerts,
                                               NULL, false);

   // VERTICAL LINES NOW   
   if (n==0) { fA=vcMin.y; fB=vcMax.y; fC=vcMin.x; fD=vcMax.x; }
   if (n==1) { fA=vcMin.y; fB=vcMax.y; fC=vcMin.z; fD=vcMax.z; }
   if (n==2) { fA=vcMin.z; fB=vcMax.z; fC=vcMin.x; fD=vcMax.x; }
   
   if (bSnapGrid)
      pVerts = (LVERTEX*)realloc(pVerts, sizeof(LVERTEX)*(int(fD-fC))*9*2*2);
   else
      pVerts = (LVERTEX*)realloc(pVerts, sizeof(LVERTEX)*(int(fD-fC))*2);

   // vertical lines
   for (i=0; fC<fD; fC++) {
      pVerts[i].x = pVerts[i+1].x = fC;
      pVerts[i].y = fA;
      pVerts[i+1].y = fB;
      pVerts[i].z = pVerts[i+1].z = 0.0f;

      if (pVerts[i].x == 0.0f) {
         pVerts[i].Color   = dwMean;
         pVerts[i+1].Color = dwMean;
         }
      else {
         pVerts[i].Color   = dwUnit;
         pVerts[i+1].Color = dwUnit;
         }
      i+= 2;
      
      // render snap grid
      if (bSnapGrid) {
         for (float x=0.1f; x<1.0f; x+=0.1f) {
            pVerts[i].y = fA;
            pVerts[i+1].y = fB;
            pVerts[i].x = pVerts[i+1].x = fC + x;
            pVerts[i].z = pVerts[i+1].z = 0.0f;
            pVerts[i].Color = dwSnap;
            pVerts[i+1].Color = dwSnap;
            i += 2;
            }
         }
      } // for [vert lines]
   m_pRDevice->GetVertexManager()->RenderLines(VID_UL, i, pVerts,
                                               NULL, false);

   free(pVerts);
   m_pRDevice->SetDepthBufferMode(RS_DEPTH_READWRITE);
   m_pRDevice->SetWorldTransform(NULL);
   } // RenderOrthoGrid
/*----------------------------------------------------------------*/


// renders a grid to the given child view
void CApplication::RenderPerspGrid(void) {
   LVERTEX   v[84];

   if ( !m_bDrawGrid[1] ) return;

   static DWORD dwDW = DWORD_ARGB(255,115,115,115);
   static DWORD dwMW = DWORD_ARGB(255,191,191,191);

   for (int j=0; j<21; j++) {
      v[2*j].x = -10.0f;
      v[2*j].y = v[2*j+1].y = 0.0f;
      v[2*j].z = v[2*j+1].z = j-10.0f;
      v[2*j+1].x = 10.0f;
      
      v[42+(2*j)].x   = v[42+(2*j+1)].x = j-10.0f;
      v[42+(2*j)].y   = v[42+(2*j+1)].y = 0.0f;
      v[42+(2*j)].z   = -10.0f;
      v[42+(2*j+1)].z = 10.0f;
      
      v[2*j].Color        = dwDW;
      v[2*j+1].Color      = dwDW;
      v[42+(2*j)].Color   = dwDW;
      v[42+(2*j+1)].Color = dwDW;
      }
   m_pRDevice->GetVertexManager()->RenderLines(VID_UL, 84, v, NULL, false);
   
   v[0].x = v[2].z = -10.0f;
   v[0].y = v[2].y = 0.0f;
   v[0].z = v[2].x = 0.0f;
   v[1].x = v[3].z = 10.0f;
   v[1].y = v[3].y = 0.0f;
   v[1].z = v[3].x = 0.0f;
   v[0].Color = v[1].Color = v[2].Color = v[3].Color = dwMW;
   m_pRDevice->GetVertexManager()->RenderLines(VID_UL, 4, v, NULL, false);
   } // RenderPerspGrid
/*----------------------------------------------------------------*/


bool CApplication::ChangeCheckState(UINT MenuID) {
   DWORD dwItem;

   dwItem = GetMenuState(m_hm, MenuID, MF_BYCOMMAND); 
         
   if ( !(dwItem & MF_CHECKED)) { 
      CheckMenuItem(m_hm, MenuID, MF_BYCOMMAND | MF_CHECKED); 
      return true;
      }
   else {
      CheckMenuItem(m_hm, MenuID, MF_BYCOMMAND | MF_UNCHECKED); 
      return false;
      }
   } // ChangeState
/*----------------------------------------------------------------*/


bool CApplication::InvalidateSelectionRect(bool bInvalidate) {
   if (bInvalidate) {
      m_rcOrthoSel.bottom = -1;
      m_rcOrthoSel.top    = -1;
      m_rcOrthoSel.left   = -1;
      m_rcOrthoSel.right  = -1;
      return true;
      }
   else {
      if ( (m_rcOrthoSel.bottom == -1) &&
           (m_rcOrthoSel.top    == -1) &&
           (m_rcOrthoSel.left   == -1) &&
           (m_rcOrthoSel.right  == -1) )
         return true;
      else 
         return false;
      }
   } // InvalidateSelectionRect
/*----------------------------------------------------------------*/


/**
 * Helper routine to encapsulate reused functionality
 */
void CApplication::CheckTbState(UINT NumObj, UINT id, SELECTMODE sm) {
   if ( (NumObj == 1) && (m_SelectMode == sm) )
      m_Toolbar.EnableButton(id, true);
   else
      m_Toolbar.EnableButton(id, false);
   } // ActivateIfSingle
/*----------------------------------------------------------------*/


/**
 * Updates the child window displaying the actual selection count.
 */
void CApplication::OnSelBufChange(void) {
   char buffer[256];
   BOOL n = TRUE;
   UINT bEnable;
   UINT NumObj = m_G.SelBuf.GetNumObjects();
   UINT SelSkin = cbhGetSelectedIndex(m_G.hGUI[CSK_LIST]);

   InvalidateAllViews();

   // output info text
   if ( m_SelectMode == SM_VERTEX )
      sprintf(buffer, "%d Vertices", NumObj);
   else if ( m_SelectMode == SM_POLYGON )
      sprintf(buffer, "%d Polygons", NumObj);
   else if ( m_SelectMode == SM_MESH )
      sprintf(buffer, "%d Meshs", NumObj);
   else if ( m_SelectMode == SM_PORTAL )
      sprintf(buffer, "%d Portals", NumObj);
   else if ( m_SelectMode == SM_ENTITY )
      sprintf(buffer, "%d Entities", NumObj);
   else if ( m_SelectMode == SM_LIGHT )
      sprintf(buffer, "%d Lights", NumObj);
   else if ( m_SelectMode == SM_SPAWN )
      sprintf(buffer, "%d Spawns", NumObj);
   
   SetWindowText(m_G.hGUI[CS_NUM], buffer);

   // activate texture menu if appropriate
   if ( (m_SelectMode != SM_POLYGON) &&
        (m_SelectMode != SM_MESH   ) )
      n = FALSE;

   // activate portal features if appropriate
   if ( (m_SelectMode == SM_POLYGON) &&
        (m_G.SelBuf.IsPolyQuad()) )
      m_Toolbar.EnableButton(IDTB_MAKE_PORTAL, true);
   else 
      m_Toolbar.EnableButton(IDTB_MAKE_PORTAL, false);

   m_Toolbar.EnableButton(IDTB_GEO_INSIDEOUT, n);
   m_Toolbar.EnableButton(IDTB_MESH_SAVE, n);
   EnableWindow(m_G.hGUI[GRP4],    n);
   EnableWindow(m_G.hGUI[CTC_X],   n);
   EnableWindow(m_G.hGUI[CTC_Y],   n);
   EnableWindow(m_G.hGUI[CTC_Z],   n);
   EnableWindow(m_G.hGUI[CTC_DLG], n);

   CheckTbState(NumObj, IDTB_LIGHT_EDIT,  SM_LIGHT);
   CheckTbState(NumObj, IDTB_MAKE_ENTITY, SM_MESH);
   CheckTbState(NumObj, IDTB_EDIT_ENTITY, SM_ENTITY);

// no need for that functionality for now
// CheckTbState(NumObj, IDTB_EDIT_SPAWN,  SM_SPAWN);

   // enable menu items if appropriate
   if ( NumObj > 0) {
      if ( (m_SelectMode == SM_POLYGON) ||
           (m_SelectMode == SM_MESH) ) {
         bEnable = MF_ENABLED;
         if (SelSkin > 1)
            EnableWindow(m_G.hGUI[CSK_APPLY], TRUE);
         else
            EnableWindow(m_G.hGUI[CSK_APPLY], FALSE);
         }
      else {
         EnableWindow(m_G.hGUI[CSK_APPLY], FALSE);
         bEnable = MF_GRAYED;
         }
      }
   else {
      bEnable = MF_GRAYED;
      EnableWindow(m_G.hGUI[CSK_APPLY], FALSE);
      }

   EnableMenuItem(m_hm, IDM_EDIT_DELETE, bEnable);
   EnableMenuItem(m_hm, IDM_EDIT_INSIDEOUT, bEnable);

   // allow copying for meshs, polys and lights
   if ( (NumObj > 0) &&
        ( (m_SelectMode == SM_POLYGON) ||
          (m_SelectMode == SM_MESH) ||
          (m_SelectMode == SM_LIGHT) )) {
      EnableMenuItem(m_hm, IDM_EDIT_INSERTCOPY, MF_ENABLED);
      }
   else
      EnableMenuItem(m_hm, IDM_EDIT_INSERTCOPY, MF_GRAYED);

   // submenu "rotate 90°" and "mirror on"
   HMENU hmEdit = GetSubMenu(m_hm, 1);
   EnableMenuItem(hmEdit, 8, MF_BYPOSITION | bEnable);
   EnableMenuItem(hmEdit, 9, MF_BYPOSITION | bEnable);

   if (m_G.SelBuf.GetNumVerts() > 0)
      EnableMenuItem(m_hm, IDM_EDIT_SNAPVERTS, MF_ENABLED);
   else
      EnableMenuItem(m_hm, IDM_EDIT_SNAPVERTS, MF_GRAYED);


   // merge only if more than one object
   if ( NumObj > 1 )
      EnableMenuItem(m_hm, IDM_EDIT_MERGE, MF_ENABLED);
   else EnableMenuItem(m_hm, IDM_EDIT_MERGE, MF_GRAYED);

   // fragment only if meshs are selected
   if ( (bEnable==MF_ENABLED) && (m_SelectMode==SM_MESH) )
      EnableMenuItem(m_hm, IDM_EDIT_FRAGMENT, MF_ENABLED);
   else EnableMenuItem(m_hm, IDM_EDIT_FRAGMENT, MF_GRAYED);

   // camera to selection only if at least one selected
   if ( NumObj > 0 )
      EnableMenuItem(m_hm, IDM_CAM_TO_SEL, MF_ENABLED);
   else 
      EnableMenuItem(m_hm, IDM_CAM_TO_SEL, MF_GRAYED);
   } // OnSelBufChange
/*----------------------------------------------------------------*/


/**
 * Call this if the user clicks to set a new vertex for the polygon
 * under construction or if right mouse was clicked to end operation.
 */
void CApplication::OnPolyCreationEvent(bool bEnd, const ZFXVector *pvc) {
   LPPOLYGON pPoly=NULL;
   CPolygon  NewPoly;
   CSector  *pSector=NULL;

   // save in which view creation started
   if (m_NumVerts == 0) m_bIsCreating = m_CurView;

   // end construction
   if (bEnd) {
      // at least one triangle
      if (m_NumVerts>2) {
         // create the new polygon
         NewPoly.Reset();
         NewPoly.SetVertices(m_vVerts, m_NumVerts);
         NewPoly.Triangulate();
         NewPoly.CalcTextureCoords(m_Axis[m_CurView],NULL);

         // add to current sector
         pSector = m_G.Level.GetSelectedSector();      
         pPoly = (LPPOLYGON)pSector->AddObject(NewPoly);

         m_G.SelBuf.Reset();
         m_G.SelBuf.AddLob(pPoly);
         m_G.bChanged = true;
         OnSelBufChange();
         m_bIsCreating = 255;
         }

      // simulate radio control clicks
      SendMessage(m_G.hGUI[CSM_POLYGON],BM_CLICK,0,0);
      SendMessage(m_G.hGUI[CT_SEL],BM_CLICK,0,0);

      // set selection tool and select this poly
      m_CurrentTool = TL_SELECT;
      m_NumVerts = 0;
      }
   else if (!bEnd) {
      // no more space for one more
      if (m_NumVerts > 255) return;

      // copy coordinates from click point
      m_vVerts[m_NumVerts].x  = pvc->x;
      m_vVerts[m_NumVerts].y  = pvc->y;
      m_vVerts[m_NumVerts].z  = pvc->z;
      m_vVerts[m_NumVerts].tu = 0.0f;
      m_vVerts[m_NumVerts].tv = 0.0f;
      m_NumVerts++;
      }
   } // OnPolyCreationEvent
/*----------------------------------------------------------------*/


// render the polygon while it is being created
void CApplication::RenderPolyUnderConstr(UINT nView) {
   static ZFXCOLOR clrL = { 0.0f, 1.0f, 0.0f, 1.0f };
   static ZFXCOLOR clrP = { 1.0f, 1.0f, 0.0f, 1.0f };
   
   if (m_NumVerts < 1)  return;
   if (m_NumVerts > 255) return;
   
   // get point from mouse cursor
   ZFXVector vc = m_pRDevice->Transform2Dto2D(
                         m_bIsCreating, 
                         m_fScale[m_bIsCreating], 
                         NULL, 
                         m_Axis[m_bIsCreating]);
   vc.x += m_fPosX[m_bIsCreating];
   vc.y += m_fPosY[m_bIsCreating];
   vc.z += m_fPosZ[m_bIsCreating];
   CGeometry::Snap(&vc, m_fScale[m_bIsCreating]);

   m_vVerts[m_NumVerts].x = vc.x;
   m_vVerts[m_NumVerts].y = vc.y;
   m_vVerts[m_NumVerts].z = vc.z;


   m_pRDevice->SetDepthBufferMode(RS_DEPTH_NONE);

   m_pRDevice->GetVertexManager()->RenderLines(VID_UU, m_NumVerts+1, 
                                               m_vVerts, &clrL, true);

   m_pRDevice->SetShadeMode(RS_SHADE_POINTS, 1.0f, &clrP);

   m_pRDevice->GetVertexManager()->Render(VID_UU, 
                                          m_NumVerts+1, 0, 
                                          m_vVerts, NULL, 0);

   m_pRDevice->SetShadeMode(RS_SHADE_SOLID, 0, NULL);
   m_pRDevice->SetDepthBufferMode(RS_DEPTH_READWRITE);
   } // RenderConstruction
/*----------------------------------------------------------------*/


// render a rect if user is selecting in ortho view
void CApplication::RenderSelectionRect(void) {
   ZFXVector vc;
   VERTEX v[5];

   static ZFXCOLOR clrV = {1.0f, 0.0f, 0.0f};

   // origin of selection rect
   v[0].x = m_vcSel[0].x;
   v[0].y = m_vcSel[0].y;
   v[0].z = m_vcSel[0].z;

   if (m_bIsSelecting == 0) {
      vc = m_pRDevice->Transform2Dto2D(0, m_fScale[0], NULL, Z_AXIS);
      vc += ZFXVector(m_fPosX[0], m_fPosY[0], m_fPosZ[0]);
      v[1].x = m_vcSel[0].x;
      v[1].y = vc.y;
      v[1].z = vc.z;
      v[3].x = vc.x;
      v[3].y = m_vcSel[0].y;
      v[3].z = vc.z;
      }
   else if (m_bIsSelecting == 1) {
      vc = m_pRDevice->Transform2Dto2D(1, m_fScale[1], NULL, X_AXIS);
      vc += ZFXVector(m_fPosX[1], m_fPosY[1], m_fPosZ[1]);
      v[1].x = vc.z;
      v[1].y = vc.y;
      v[1].z = m_vcSel[0].z;
      v[3].x = vc.x;
      v[3].y = m_vcSel[0].y;
      v[3].z = vc.z;
      }
   else if (m_bIsSelecting == 2) {
      vc = m_pRDevice->Transform2Dto2D(2, m_fScale[2], NULL, Y_AXIS);
      vc += ZFXVector(m_fPosX[2], m_fPosY[2], m_fPosZ[2]);
      v[1].x = m_vcSel[0].x;
      v[1].y = vc.y;
      v[1].z = vc.z;
      v[3].x = vc.x;
      v[3].y = vc.y;
      v[3].z = m_vcSel[0].z;
      }

   // final vertices
   v[2].x = vc.x;
   v[2].y = vc.y;
   v[2].z = vc.z;
   v[4] = v[0];

   m_pRDevice->GetVertexManager()->RenderLines(VID_UU,5,v,&clrV,true);
   } // RenderSelectionRect
/*----------------------------------------------------------------*/


// render inactive sectors in smooth dark red
void CApplication::RenderInActive(bool bOrtho) {
   static ZFXCOLOR clrW = {0.4f, 0.2f, 0.2f};
   static ZFXCOLOR clrI = {0.5f, 0.3f, 0.3f};

   if ( bOrtho && !m_bDrawInact[0]) return;
   if (!bOrtho && !m_bDrawInact[1]) return;

   DWORD dwFlags = LOB_POLYGON | LOB_MESH;

   if (bOrtho) {
      m_pRDevice->SetBackfaceCulling(RS_CULL_NONE);
      m_pRDevice->SetShadeMode(RS_SHADE_LINES, 0, &clrI);
      m_pRDevice->SetDepthBufferMode(RS_DEPTH_READONLY);
      m_G.Level.RenderInActive(m_pRDevice, dwFlags, true);
      m_pRDevice->SetDepthBufferMode(RS_DEPTH_READWRITE);
      }
   else {
      if (m_bDrawInactW) {
         m_pRDevice->SetShadeMode(RS_SHADE_TRIWIRE, 0, &clrW);
         m_pRDevice->SetBackfaceCulling(RS_CULL_NONE);
         }

      m_G.Level.RenderInActive(m_pRDevice, dwFlags, m_b3DHide);
      }

   // back to default settings
   m_pRDevice->SetBackfaceCulling(RS_CULL_CCW);
   m_pRDevice->SetShadeMode(RS_SHADE_SOLID,0,NULL);
   } // RenderInActive
/*----------------------------------------------------------------*/


// render geometry, which is polygons and meshs
void CApplication::RenderGeometry(bool bOrtho, float fSize) {
   DWORD dwFlags = LOB_POLYGON | LOB_MESH;

   static ZFXCOLOR clrM = {0.4f, 0.5f, 1.0f, 1.0f};
   static ZFXCOLOR clrP = {0.7f, 0.7f, 0.7f, 1.0f};
   static ZFXCOLOR clrV = {1.0f, 0.0f, 0.0f, 1.0f};

   // retrieve ambient color value from this sector
   ZFXCOLOR amb = m_G.Level.GetSelectedSector()->GetAmbient();

   // orthogonal view
   if (bOrtho) {
      m_pRDevice->SetBackfaceCulling(RS_CULL_NONE);

      // if user wants to see wireframe
      if (m_bDrawWired[0]) {
         m_pRDevice->SetShadeMode(RS_SHADE_LINES, 0, &clrM);
         m_G.Level.RenderActive(m_pRDevice, LOB_MESH, true);

         m_pRDevice->SetShadeMode(RS_SHADE_LINES, 0, &clrP);
         m_G.Level.RenderActive(m_pRDevice, LOB_POLYGON, true);
         }
      else {
         m_pRDevice->SetShadeMode(RS_SHADE_SOLID, 0, NULL);
         m_G.Level.RenderActive(m_pRDevice, dwFlags, true);
         }

      // if user wants to see vertices
      if (m_bDrawPoints[0]) {
         m_pRDevice->SetShadeMode(RS_SHADE_POINTS, fSize, &clrV);
         m_pRDevice->SetDepthBufferMode(RS_DEPTH_NONE);
         m_G.Level.RenderActive(m_pRDevice, dwFlags, true);
         m_pRDevice->SetDepthBufferMode(RS_DEPTH_READWRITE);
         }
      }
   // perspective view
   else {
      // if user wants to see wireframe
      if (m_bDrawWired[1]) {
         m_pRDevice->SetShadeMode(RS_SHADE_LINES, 0, &clrP);
         m_G.Level.RenderActive(m_pRDevice, dwFlags, m_b3DHide);
         }
      else {
         m_pRDevice->SetShadeMode(RS_SHADE_SOLID, 0, NULL);

         if (m_bDrawLight)
            m_pRDevice->SetAmbientLight(amb.fR, amb.fG, amb.fB);
         else
            m_pRDevice->SetAmbientLight(1.0f, 1.0f, 1.0f);

         // activate our base shaders with ambient light
         m_pRDevice->UseShaders(true);
         m_pRDevice->ActivatePShader(m_BasePSH);
         m_pRDevice->ActivateVShader(m_BaseVSH, VID_UU);

         m_G.Level.RenderActive(m_pRDevice, dwFlags, m_b3DHide);
 
         // if we should render lighting also then just do it
         if (m_bDrawLight) {
            // activate additive blending and omni light shaders
            m_pRDevice->UseAdditiveBlending(true);
            m_pRDevice->ActivatePShader(m_OmniPSH);
            m_pRDevice->ActivateVShader(m_OmniVSH, VID_UU);
 
            m_G.Level.RenderActiveLit(m_pRDevice, m_b3DHide);
            }

         // back to defaults and no shaders and blending
         m_pRDevice->UseShaders(false);
         m_pRDevice->UseAdditiveBlending(false);
         m_pRDevice->SetAmbientLight(1.0f, 1.0f, 1.0f);
         }

      // if user wants to see vertices
      if (m_bDrawPoints[1]) {
         m_pRDevice->SetShadeMode(RS_SHADE_POINTS,fSize,&clrV);
         m_G.Level.RenderActive(m_pRDevice, dwFlags, m_b3DHide);
         }
      }

   // back to default settings
   m_pRDevice->SetBackfaceCulling(RS_CULL_CCW);
   m_pRDevice->SetShadeMode(RS_SHADE_SOLID,0,NULL);
   } // RenderGeometry
/*----------------------------------------------------------------*/


// render portals as pink wirehull and transparent polys
void CApplication::RenderPortals(bool bOrtho) {
   static ZFXCOLOR clr = { 1.0f, 0.0f, 1.0f, 1.0f };
   ZFXRENDERSTATE sm = RS_SHADE_LINES;

   m_pRDevice->SetShadeMode(sm, 0, &clr);
   m_G.Level.RenderActive(m_pRDevice, LOB_PORTAL, true);

   if (!bOrtho) {
      sm = RS_SHADE_SOLID;
      m_pRDevice->SetShadeMode(sm, 0, &clr);
      m_G.Level.RenderActive(m_pRDevice, LOB_PORTAL, m_b3DHide);
      }

   // back to default settings
   m_pRDevice->SetShadeMode(RS_SHADE_SOLID, 0, NULL);
   } // RenderPortals
/*----------------------------------------------------------------*/


// render lights as yellow wirehull and transparent polys
void CApplication::RenderLights(bool bOrtho) {
   static ZFXCOLOR clr = { 1.0f, 1.0f, 0.0f, 1.0f };
   ZFXRENDERSTATE sm = RS_SHADE_LINES;

   m_pRDevice->SetShadeMode(sm, 0, &clr);
   m_G.Level.RenderActive(m_pRDevice, LOB_LIGHT, true);

   if (!bOrtho) {
      sm = RS_SHADE_SOLID;
      m_pRDevice->SetShadeMode(sm, 0, &clr);
      m_G.Level.RenderActive(m_pRDevice, LOB_LIGHT, m_b3DHide);
      }

   // back to default settings
   m_pRDevice->SetShadeMode(RS_SHADE_SOLID, 0, NULL);
   } // RenderLights
/*----------------------------------------------------------------*/


// render spawnpoints as green wirehull and transparent polys
void CApplication::RenderSpawns(bool bOrtho) {
   static ZFXCOLOR clr = { 0.0f, 1.0f, 0.0f, 1.0f };
   ZFXRENDERSTATE sm = RS_SHADE_LINES;

   m_pRDevice->SetShadeMode(sm, 0, &clr);
   m_G.Level.RenderActive(m_pRDevice, LOB_SPAWNPOINT, true);

   if (!bOrtho) {
      sm = RS_SHADE_SOLID;
      m_pRDevice->SetShadeMode(sm, 0, &clr);
      m_G.Level.RenderActive(m_pRDevice, LOB_SPAWNPOINT, m_b3DHide);
      }

   // back to default settings
   m_pRDevice->SetShadeMode(RS_SHADE_SOLID, 0, NULL);
   } // RenderSpawns
/*----------------------------------------------------------------*/


// render lights as yellow wirehull and transparent polys
void CApplication::RenderEntities(bool bOrtho) {
   static ZFXCOLOR clr  = {0.0f, 1.0f, 1.0f, 1.0f};
   static ZFXCOLOR clrV = {1.0f, 0.0f, 0.0f, 1.0f};

   if (bOrtho) {
      m_pRDevice->SetBackfaceCulling(RS_CULL_NONE);

      // if user wants to see wireframe
      if (m_bDrawWired[0]) {
         m_pRDevice->SetShadeMode(RS_SHADE_LINES, 0, &clr);
         m_G.Level.RenderActive(m_pRDevice, LOB_ENTITY, true);
         }
      else {
         m_pRDevice->SetAmbientLight(1.0f, 1.0f, 1.0f);
         m_pRDevice->SetShadeMode(RS_SHADE_SOLID, 0, NULL);
         m_G.Level.RenderActive(m_pRDevice, LOB_ENTITY, true);
         }

      m_pRDevice->SetBackfaceCulling(RS_CULL_CCW);
      m_pRDevice->SetShadeMode(RS_SHADE_SOLID, 0, NULL);
      }
   else {
      m_pRDevice->SetShadeMode(RS_SHADE_LINES, 0, &clr);
      m_G.Level.RenderActive(m_pRDevice, LOB_ENTITY, true);
      m_pRDevice->SetShadeMode(RS_SHADE_SOLID, 0, NULL);
      m_G.Level.RenderActive(m_pRDevice, LOB_ENTITY, m_b3DHide);
      }
   } // RenderEntities
/*----------------------------------------------------------------*/


// create GUI except for toolbar and menu
void CApplication::CreateGUI(void) {

   HFONT hFont;

   hFont = CreateFont(16, 6, 0, 0, FW_NORMAL, 
                      FALSE, FALSE, FALSE, 
                      DEFAULT_CHARSET,
                      OUT_DEFAULT_PRECIS, 
                      CLIP_DEFAULT_PRECIS, 
                      DEFAULT_QUALITY, 
                      DEFAULT_PITCH,
                      "Sans Serif");

   m_G.hGUI[S1] = STC("Choose Active Sector:",5,35,200,20,5000);

   m_G.hGUI[SECTOR] = cbhCreate(m_G.hWnd, m_G.hInst,
                                (HMENU)IDC_SECTOR,
                                5,55,200,150, true);

  m_G.hGUI[MOUSE] = STC("(-,-,-)",5,85,200,20,IDC_MOUSE);

   // SELECTION MODE FRAME AND CHILD CONTROLS
   m_G.hGUI[GRP1]       = CreateWindow("BUTTON", "Selection Menu",
                                     WS_CHILD|WS_VISIBLE|BS_GROUPBOX,
                                     1, 105, 208, 140, m_G.hWnd,
                                     (HMENU)5000, m_G.hInst, NULL);

   m_G.hGUI[CSM_MESH]    = CreateWindow("BUTTON", "Mesh", WS_CHILD|WS_VISIBLE|
                                     BS_PUSHLIKE|BS_AUTORADIOBUTTON|WS_GROUP , 
                                     12, 125, 62, 20, m_G.hWnd, 
                                     (HMENU)IDC_SMMESH, m_G.hInst, NULL);
   
   m_G.hGUI[CSM_POLYGON] = CreateWindow("BUTTON", "Polygon", WS_CHILD|WS_VISIBLE|
                                     BS_PUSHLIKE|BS_AUTORADIOBUTTON, 
                                     74, 125, 62, 20, m_G.hWnd, 
                                     (HMENU)IDC_SMPOLYGON, m_G.hInst, NULL);

   m_G.hGUI[CSM_VERTEX] = CreateWindow("BUTTON", "Vertex", WS_CHILD|WS_VISIBLE|
                                     BS_PUSHLIKE|BS_AUTORADIOBUTTON, 
                                     136, 125, 62, 20, m_G.hWnd, 
                                     (HMENU)IDC_SMVERTEX, m_G.hInst, NULL);

   m_G.hGUI[CSM_PORTAL] = CreateWindow("BUTTON", "Portal", WS_CHILD|WS_VISIBLE|
                                     BS_PUSHLIKE|BS_AUTORADIOBUTTON, 
                                     12, 145, 62, 20, m_G.hWnd, 
                                     (HMENU)IDC_SMPORTAL, m_G.hInst, NULL);

   m_G.hGUI[CSM_ENTITY] = CreateWindow("BUTTON", "Entity", WS_CHILD|WS_VISIBLE|
                                     BS_PUSHLIKE|BS_AUTORADIOBUTTON, 
                                     74, 145, 62, 20, m_G.hWnd, 
                                     (HMENU)IDC_SMENTITY, m_G.hInst, NULL);

   m_G.hGUI[CSM_LIGHT]  = CreateWindow("BUTTON", "Light", WS_CHILD|WS_VISIBLE|
                                     BS_PUSHLIKE|BS_AUTORADIOBUTTON, 
                                     136, 145, 62, 20, m_G.hWnd, 
                                     (HMENU)IDC_SMLIGHT, m_G.hInst, NULL);

   m_G.hGUI[CSM_SPAWN]  = CreateWindow("BUTTON", "Spawn", WS_CHILD|WS_VISIBLE|
                                     BS_PUSHLIKE|BS_AUTORADIOBUTTON, 
                                     12, 165, 62, 20, m_G.hWnd, 
                                     (HMENU)IDC_SMSPAWN, m_G.hInst, NULL);

   m_G.hGUI[CSM_DUMMY_1] = CreateWindow("BUTTON", "Dummy", WS_CHILD|WS_VISIBLE|
                                     BS_PUSHLIKE|BS_AUTORADIOBUTTON, 
                                     74, 165, 62, 20, m_G.hWnd, 
                                     (HMENU)IDC_DUMMY_1, m_G.hInst, NULL);

   m_G.hGUI[CSM_DUMMY_2] = CreateWindow("BUTTON", "Dummy", WS_CHILD|WS_VISIBLE|
                                     BS_PUSHLIKE|BS_AUTORADIOBUTTON, 
                                     136, 165, 62, 20, m_G.hWnd, 
                                     (HMENU)IDC_DUMMY_2, m_G.hInst, NULL);

   SendMessage(m_G.hGUI[CSM_POLYGON], BM_SETCHECK, BST_CHECKED, 0);
   EnableWindow(m_G.hGUI[CSM_DUMMY_1], FALSE);
   EnableWindow(m_G.hGUI[CSM_DUMMY_2], FALSE);
   m_SelectMode = SM_POLYGON;
   m_b3DHide    = true;
   

   // PRESELECTION BUTTONS:
   m_G.hGUI[CS_ALL]  = CreateWindow("BUTTON", "All", WS_CHILD|WS_VISIBLE|
                                     BS_PUSHLIKE, 26, 192, 50, 20, m_G.hWnd, 
                                     (HMENU)IDC_SALL, m_G.hInst, NULL);

   m_G.hGUI[CS_NONE] = CreateWindow("BUTTON", "None", WS_CHILD|WS_VISIBLE|
                                     BS_PUSHLIKE, 80, 192, 50, 20, m_G.hWnd, 
                                     (HMENU)IDC_SNONE, m_G.hInst, NULL);

   m_G.hGUI[CS_INV]  = CreateWindow("BUTTON", "Invert", WS_CHILD|WS_VISIBLE|
                                     BS_PUSHLIKE, 134, 192, 50, 20, m_G.hWnd, 
                                     (HMENU)IDC_SINV, m_G.hInst, NULL);

   m_G.hGUI[S2] = STC("Selected Objects:",10,220,115,20,5000);
   m_G.hGUI[CS_NUM] = STC("0",120,220,80,20,IDC_SNUM);


   // HIDE MENU OPTIONS:
   m_G.hGUI[GRP2]   = CreateWindow("BUTTON", "Hide Menu",
                                     WS_CHILD|WS_VISIBLE|BS_GROUPBOX,
                                     1, 255, 208, 75, m_G.hWnd,
                                     (HMENU)5000, m_G.hInst, NULL);

   m_G.hGUI[CH_SEL] = CreateWindow("BUTTON", "Selected", WS_CHILD|WS_VISIBLE|
                                     BS_PUSHBUTTON, 43, 275, 75, 20, m_G.hWnd, 
                                     (HMENU)IDC_HSEL, m_G.hInst, NULL);

   m_G.hGUI[CH_UNS] = CreateWindow("BUTTON", "Unselected", WS_CHILD|WS_VISIBLE|
                                     BS_PUSHBUTTON, 123, 275, 75, 20, m_G.hWnd, 
                                     (HMENU)IDC_HUNS, m_G.hInst, NULL);

   m_G.hGUI[CH_INV] = CreateWindow("BUTTON", "Invert", WS_CHILD|WS_VISIBLE|
                                     BS_PUSHBUTTON, 43, 300, 75, 20, m_G.hWnd, 
                                     (HMENU)IDC_HINV, m_G.hInst, NULL);

   m_G.hGUI[CH_NONE] = CreateWindow("BUTTON", "Unhide All", WS_CHILD|WS_VISIBLE|
                                     BS_PUSHBUTTON, 123, 300, 75, 20, m_G.hWnd, 
                                     (HMENU)IDC_HNONE, m_G.hInst, NULL);

   m_G.hGUI[S3] = STC("3D",14,278,25,20,5000);

   m_G.hGUI[CH_3D] = CreateWindow("BUTTON", "", WS_CHILD|WS_VISIBLE|
                                     BS_AUTOCHECKBOX, 14, 296, 25, 20, m_G.hWnd, 
                                     (HMENU)IDC_H3D, m_G.hInst, NULL);

   SendMessage(m_G.hGUI[CH_3D],BM_SETCHECK,BST_CHECKED,0);


   // TOOL MENU
   m_G.hGUI[GRP3]     = CreateWindow("BUTTON", "Tool Menu",
                                     WS_CHILD|WS_VISIBLE|BS_GROUPBOX,
                                     1, 340, 208, 70, m_G.hWnd,
                                     (HMENU)5000, m_G.hInst, NULL);

   m_G.hGUI[CT_SEL]   = CreateWindow("BUTTON", "Select/Move", WS_CHILD|WS_VISIBLE|
                                     BS_PUSHLIKE|BS_AUTORADIOBUTTON|WS_GROUP, 
                                     12, 360, 92, 20, m_G.hWnd, 
                                     (HMENU)IDC_TSEL, m_G.hInst, NULL);
   
   m_G.hGUI[CT_POLY]  = CreateWindow("BUTTON", "Make Polygon", WS_CHILD|WS_VISIBLE|
                                     BS_PUSHLIKE|BS_AUTORADIOBUTTON, 
                                     104, 360, 92, 20, m_G.hWnd, 
                                     (HMENU)IDC_TPOLY, m_G.hInst, NULL);

   m_G.hGUI[CT_TRANS] = CreateWindow("BUTTON", "Dummy Tool", WS_CHILD|WS_VISIBLE|
                                     BS_PUSHLIKE|BS_AUTORADIOBUTTON, 
                                     12, 380, 92, 20, m_G.hWnd, 
                                     (HMENU)IDC_TTRANS, m_G.hInst, NULL);
   
   m_G.hGUI[CT_ROTA]  = CreateWindow("BUTTON", "Dummy Tool", WS_CHILD|WS_VISIBLE|
                                     BS_PUSHLIKE|BS_AUTORADIOBUTTON, 
                                     104, 380, 92, 20, m_G.hWnd, 
                                     (HMENU)IDC_TROTA, m_G.hInst, NULL);

   SendMessage(m_G.hGUI[CT_SEL], BM_SETCHECK, BST_CHECKED, 0);
   m_CurrentTool = TL_SELECT;

   EnableWindow(m_G.hGUI[CT_TRANS], FALSE);
   EnableWindow(m_G.hGUI[CT_ROTA],  FALSE);


   // TEXTURECOORD MENU
   m_G.hGUI[GRP4]     = CreateWindow("BUTTON", "Texture Menu",
                                     WS_CHILD|WS_VISIBLE|BS_GROUPBOX,
                                     1, 420, 208, 75, m_G.hWnd,
                                     (HMENU)5000, m_G.hInst, NULL);

   m_G.hGUI[CTC_Z]    = CreateWindow("BUTTON", "Front (XY)", WS_CHILD|WS_VISIBLE|
                                     BS_PUSHBUTTON, 6, 440, 62, 20, m_G.hWnd, 
                                     (HMENU)IDC_TCZ, m_G.hInst, NULL);
   
   m_G.hGUI[CTC_X]    = CreateWindow("BUTTON", "Side (YZ)", WS_CHILD|WS_VISIBLE|
                                     BS_PUSHBUTTON, 73, 440, 62, 20, m_G.hWnd, 
                                     (HMENU)IDC_TCX, m_G.hInst, NULL);

   m_G.hGUI[CTC_Y]    = CreateWindow("BUTTON", "Top (XZ)", WS_CHILD|WS_VISIBLE|
                                     BS_PUSHBUTTON, 140, 440, 62, 20, m_G.hWnd, 
                                     (HMENU)IDC_TCY, m_G.hInst, NULL);

   m_G.hGUI[CTC_DLG]  = CreateWindow("BUTTON", "Base Coords", WS_CHILD|WS_VISIBLE|
                                     BS_PUSHBUTTON, 6, 465, 96, 20, m_G.hWnd, 
                                     (HMENU)IDC_TCDLG, m_G.hInst, NULL);

   m_G.hGUI[CTC_DLG2] = CreateWindow("BUTTON", "Detail Coords", WS_CHILD|WS_VISIBLE|
                                     BS_PUSHBUTTON, 106, 465, 96, 20, m_G.hWnd, 
                                     (HMENU)IDC_TCDLG2, m_G.hInst, NULL);

   EnableWindow(m_G.hGUI[CTC_DLG2], FALSE);

   // TEXTURECOORD MENU
   m_G.hGUI[GRP5]      = CreateWindow("BUTTON", "Skin Menu",
                                     WS_CHILD|WS_VISIBLE|BS_GROUPBOX,
                                     1, 505, 208, 80, m_G.hWnd,
                                     (HMENU)5000, m_G.hInst, NULL);

   m_G.hGUI[CSK_LIST]  = cbhCreate(m_G.hWnd, m_G.hInst,
                                (HMENU)IDC_SKLIST,
                                8,525,192,150, false);

   m_G.hGUI[CSK_NEW]   = CreateWindow("BUTTON", "New", WS_CHILD|WS_VISIBLE|
                                     BS_PUSHBUTTON, 8, 555, 45, 20, m_G.hWnd, 
                                     (HMENU)IDC_SKNEW, m_G.hInst, NULL);

   m_G.hGUI[CSK_EDIT]  = CreateWindow("BUTTON", "Tex", WS_CHILD|WS_VISIBLE|
                                     BS_PUSHBUTTON, 56, 555, 45, 20, m_G.hWnd, 
                                     (HMENU)IDC_SKEDIT, m_G.hInst, NULL);
   
   m_G.hGUI[CSK_MAT]   = CreateWindow("BUTTON", "Mat", WS_CHILD|WS_VISIBLE|
                                     BS_PUSHBUTTON, 104, 555, 45, 20, m_G.hWnd, 
                                     (HMENU)IDC_SKMAT, m_G.hInst, NULL);

   m_G.hGUI[CSK_APPLY] = CreateWindow("BUTTON", "Apply", WS_CHILD|WS_VISIBLE|
                                     BS_PUSHBUTTON, 153, 555, 45, 20, m_G.hWnd, 
                                     (HMENU)IDC_SKAPPLY, m_G.hInst, NULL);

   OnSelBufChange();

    // set fonts for the child controls
   for (int i=0; i<MAX_HWND; i++) 
      SendMessage(m_G.hGUI[i], WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE,0));
   } // CreateGUI
/*----------------------------------------------------------------*/


// create the shaders used for level geometry
HRESULT CApplication::InitShaders(void) {
   HRESULT hr;

   // transform position, move ambient
   // and text coords to pixel shader
   // c0 : world * view * proj matrix
   // c4 : ambient light value
   const char BaseVSH[] =
         "vs.1.1                 \n"\
         "dcl_position0 v0       \n"\
         "dcl_normal0   v3       \n"\
         "dcl_texcoord0 v6       \n"\
         "m4x4 oPos, v0, c0      \n"\
         "mov oD0, c4            \n"\
         "mov oT0, v6            \n";

   // transform position and 2nd time
   // with attenuation matrix, move text
   // coords to pixel shader
   // c0  : world * view * proj matrix
   // c20 : attenuation matrix
   const char OmniVSH[] =
         "vs.1.1                 \n"\
         "dcl_position0 v0       \n"\
         "dcl_normal0   v3       \n"\
         "dcl_texcoord0 v6       \n"\
         "m4x4 oPos, v0, c0      \n"\
         "mov oT0, v6            \n"\
         "m4x4 oT1, v0, c20      \n";

   // just modify text color with ambient
   const char BasePSH[] =
         "ps.1.1                 \n"\
         "tex t0                 \n"\
         "mul r1, t0, c3         \n"\
         "mad r0, t0, v0, r1     \n";

   // calulate point light with text color
   // c0 : light color
   const char OmniPSH[] =
         "ps.1.1                       \n"\
         "tex      t0                  \n"\
         "texcoord t1                  \n"\
         "dp3_sat r0, t1_bx2, t1_bx2	\n"\
         "mul r0, c0, 1-r0             \n"\
         "mul r0, r0, t0               \n";

   // create the two vertex shaders
   hr = m_pRDevice->CreateVShader(BaseVSH, sizeof(BaseVSH), 
                                  false, false, &m_BaseVSH);
   if (FAILED(hr)) {
      fprintf(m_G.pLog, "error: CreateVS(\"BaseVSH\") failed\n");
      return hr;
      }

   hr = m_pRDevice->CreateVShader(OmniVSH, sizeof(OmniVSH), 
                                  false, false, &m_OmniVSH);
   if (FAILED(hr)) {
      fprintf(m_G.pLog, "error: CreateVS(\"OmniVSH\") failed\n");
      return hr;
      }
   // create the two pixel shaders
   hr = m_pRDevice->CreatePShader(BasePSH, sizeof(BasePSH), 
                                  false, false, &m_BasePSH);
   if (FAILED(hr)) {
      fprintf(m_G.pLog, "error: CreatePS(\"BasePSH\") failed\n");
      return hr;
      }

   hr = m_pRDevice->CreatePShader(OmniPSH, sizeof(OmniPSH), 
                                  false, false, &m_OmniPSH);
   if (FAILED(hr)) {
      fprintf(m_G.pLog, "error: CreatePS(\"OmniPSH\") failed\n");
      return hr;
      }

   return ZFX_OK;
   } // InitShaders
/*----------------------------------------------------------------*/


void CApplication::InvalidateView(int n) {
   if ((n<0) || (n>3)) return;
   m_G.bNeedRedraw[n] = true; 
   } // InvalidateView

void CApplication::InvalidateAllViews(void) {
   m_G.bNeedRedraw[0] = true; 
   m_G.bNeedRedraw[1] = true; 
   m_G.bNeedRedraw[2] = true; 
   m_G.bNeedRedraw[3] = true; 
   } // InvalidateAllViews
/*----------------------------------------------------------------*/
